"""Resolution-independent indexed rendering, SDL composition and isolation."""
import os
import hashlib
import json
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
import zipfile
from test_engine import ROOT
from game_fixtures import finale_files
from visual_baseline import (IndexedCapture, capture_manifest, compare_metadata,
                             compare_pixels, create_review_bmps, validate_manifest,
                             require_exact_pixels, write_baseline_archive, write_manifest)

BASELINE_MANIFEST = ROOT/'tests/fixtures/visual_baselines.json'
BASELINE_ARCHIVE = ROOT/'tests/fixtures/visual_baselines.zip'


def _rebuild_archive(source: Path, destination: Path, *, omit: set[str] | None = None,
                     corrupt: set[str] | None = None) -> None:
    """Copy a baseline archive, omitting IDs or mutating one pixel per corrupt ID."""
    omit = omit or set()
    corrupt = corrupt or set()
    with zipfile.ZipFile(source) as original, zipfile.ZipFile(destination, "w") as rebuilt:
        for info in original.infolist():
            capture_id = info.filename[:-4] if info.filename.endswith(".w3i") else info.filename
            if capture_id in omit:
                continue
            data = original.read(info.filename)
            if capture_id in corrupt:
                data = data[:16] + bytes([data[16] ^ 1]) + data[17:]
            rebuilt.writestr(info, data)


class RenderTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory(prefix='w3r-render-build-')
        cls.addClassCleanup(cls.build.cleanup)
        cls.flags = ['-std=c11','-Wall','-Wextra','-Wpedantic','-Wconversion','-Wshadow','-Werror','-g']
        if os.environ.get('SANITIZE') == '1': cls.flags += ['-fsanitize=address,undefined','-fno-omit-frame-pointer']
        cls.cc = shlex.split(os.environ.get('CC','gcc'))
        cls.target = Path(cls.build.name)/'target'
        subprocess.run(cls.cc+cls.flags+['-I'+str(ROOT/'src'),str(ROOT/'tests/render_target_tests.c'),str(ROOT/'src/engine/WL_RENDER_TARGET.c'),'-o',str(cls.target)],check=True)
        sdk = Path(os.environ.get('SDL_SDK',ROOT.parent/'cart/vendor/SDL'))
        cls.binary = Path(cls.build.name)/'render'
        sources = sorted(p for p in (ROOT/'src').rglob('*.c') if p.name!='main.c')
        subprocess.run(cls.cc+cls.flags+['-DWOLF3D_DISPLAY_TEST','-I'+str(ROOT/'src'),'-I'+str(sdk/'include'),str(ROOT/'tests/render_tests.c'),*map(str,sources),str(sdk/'build/libSDL3.a'),'-pthread','-lm','-o',str(cls.binary)],check=True)

    def test_original_framebuffer_hashes(self):
        if not os.environ.get('WOLF3D_DATA'): self.skipTest('Owned data unavailable')
        data=Path(os.environ['WOLF3D_DATA'])
        identities=json.loads((ROOT/'tests/fixtures/dos_demo_hashes.json').read_text())['assets']
        files={p.name.upper():p for p in data.iterdir()}
        for name,identity in identities.items():
            if name not in files or hashlib.sha256(files[name].read_bytes()).hexdigest()!=identity:
                self.skipTest('Different original data revision')
        binary=Path(self.build.name)/'original'
        sources=sorted((ROOT/'src/engine').glob('*.c'))
        subprocess.run(self.cc+self.flags+['-I'+str(ROOT/'src'),str(ROOT/'tests/render_original_tests.c'),*map(str,sources),str(ROOT/'src/platform/data_file.c'),str(ROOT/'src/platform/save_file.c'),'-lm','-o',str(binary)],check=True)
        output=subprocess.check_output([str(binary),str(data)],text=True)
        self.assertEqual(output,(ROOT/'tests/fixtures/original_render_hashes.csv').read_text())

    def test_native_framebuffer_hashes(self):
        if not os.environ.get('WOLF3D_DATA'): self.skipTest('Owned data unavailable')
        data=Path(os.environ['WOLF3D_DATA'])
        identities=json.loads((ROOT/'tests/fixtures/dos_demo_hashes.json').read_text())['assets']
        files={p.name.upper():p for p in data.iterdir()}
        for name,identity in identities.items():
            if name not in files or hashlib.sha256(files[name].read_bytes()).hexdigest()!=identity:
                self.skipTest('Different original data revision')
        binary=Path(self.build.name)/'native'
        sources=sorted((ROOT/'src/engine').glob('*.c'))
        subprocess.run(self.cc+self.flags+['-I'+str(ROOT/'src'),str(ROOT/'tests/render_native_hashes.c'),*map(str,sources),str(ROOT/'src/platform/data_file.c'),str(ROOT/'src/platform/save_file.c'),'-lm','-o',str(binary)],check=True)
        output=subprocess.check_output([str(binary),str(data)],text=True)
        self.assertEqual(output,(ROOT/'tests/fixtures/native_render_hashes.csv').read_text())

    def test_aspect(self):
        binary=Path(self.build.name)/'aspect'
        subprocess.run(self.cc+self.flags+['-I'+str(ROOT/'src'),str(ROOT/'tests/aspect_tests.c'),str(ROOT/'src/engine/WL_ASPECT.c'),'-lm','-o',str(binary)],check=True)
        subprocess.run([str(binary)],check=True)

    def test_target(self): subprocess.run([str(self.target)],check=True)
    def run_render(self,owned=False):
        with tempfile.TemporaryDirectory(prefix='w3r-render-data-') as directory:
            root=Path(directory);data=root/'data';data.mkdir()
            if owned:
                if not os.environ.get('WOLF3D_DATA'): self.skipTest('Owned data unavailable')
                data=Path(os.environ['WOLF3D_DATA'])
            else: finale_files(data)
            env=dict(os.environ,SDL_VIDEODRIVER='dummy',SDL_RENDER_DRIVER='software',SDL_AUDIODRIVER='dummy',XDG_DATA_HOME=str(root))
            captures = None
            if not owned:
                configured = os.environ.get('WOLF3D_BASELINE_DIR')
                captures = Path(configured) if configured else root/'visual-baseline'
                captures.mkdir(parents=True, exist_ok=True)
                env['WOLF3D_BASELINE_DIR'] = str(captures)
            subprocess.run([str(self.binary),str(data),str(root),"owned" if owned else "synthetic"],env=env,check=True)
            shots=list((root/'wolf3d-revive').glob('screenshot-*.bmp'))
            self.assertEqual(len(shots),2)
            for shot in shots: self.assertEqual(shot.read_bytes()[:2],b'BM')
            self.assertEqual((root/"slot1.w3s").read_bytes()[96:],(root/"slot2.w3s").read_bytes()[96:])
            if captures:
                manifest = capture_manifest(captures, data)
                if os.environ.get('WOLF3D_UPDATE_BASELINES') == '1':
                    write_manifest(BASELINE_MANIFEST, manifest)
                    write_baseline_archive(BASELINE_ARCHIVE, captures)
                validate_manifest(manifest, BASELINE_MANIFEST, captures, BASELINE_ARCHIVE)
                initial = next(x for x in manifest['captures'] if x['id']=='display-transition-classic-initial-640x480')
                restored = next(x for x in manifest['captures'] if x['id']=='display-transition-classic-restored-640x480')
                self.assertEqual(initial['pixel_hash'], restored['pixel_hash'])
                initial = next(x for x in manifest['captures'] if x['id']=='display-transition-enhanced-initial-640x480-4x3')
                restored = next(x for x in manifest['captures'] if x['id']=='display-transition-enhanced-restored-640x480-4x3')
                self.assertEqual(initial['pixel_hash'], restored['pixel_hash'])
                write_manifest(captures/'manifest.json', manifest)
                if configured: create_review_bmps(captures)
    def test_render_and_settings(self): self.run_render()
    def test_owned_render(self): self.run_render(True)

    def test_visual_comparator_failures(self):
        original = IndexedCapture(3, 2, bytes((1, 2, 3, 4, 5, 6)))
        self.assertEqual(compare_pixels(original, original).changed_pixels, 0)
        changed = IndexedCapture(3, 2, bytes((1, 2, 3, 4, 9, 6)))
        difference = compare_pixels(original, changed)
        self.assertEqual(difference.changed_pixels, 1)
        self.assertEqual(difference.bounding_box, (1, 1, 1, 1))
        with self.assertRaisesRegex(ValueError, 'changed_pixels=1.*bounding_box=\\(1, 1, 1, 1\\)'):
            require_exact_pixels(original, changed, 'single-pixel mutation')
        with self.assertRaisesRegex(ValueError, 'dimension mismatch'):
            compare_pixels(original, IndexedCapture(2, 3, original.pixels))
        with self.assertRaisesRegex(ValueError, 'metadata mismatch'):
            compare_metadata({'scene_id':'corridor-static'}, {'scene_id':'door-motion'})

    def test_validate_manifest_rejects_missing_archive_entry(self):
        """A matching candidate must not hide a missing archived baseline."""
        manifest = json.loads(BASELINE_MANIFEST.read_text(encoding='utf-8'))
        missing_id = manifest['captures'][0]['id']
        with tempfile.TemporaryDirectory(prefix='w3r-baseline-missing-') as directory:
            root = Path(directory)
            captures = root/'captures'
            with zipfile.ZipFile(BASELINE_ARCHIVE) as archive:
                archive.extractall(captures)
            broken = root/'baseline.zip'
            _rebuild_archive(BASELINE_ARCHIVE, broken, omit={missing_id})
            with self.assertRaisesRegex(ValueError, 'capture ID set'):
                validate_manifest(manifest, BASELINE_MANIFEST, captures, broken)

    def test_validate_manifest_rejects_corrupted_archived_pixels(self):
        """Every archived pixel payload is verified even when hashes agree."""
        manifest = json.loads(BASELINE_MANIFEST.read_text(encoding='utf-8'))
        corrupted_id = manifest['captures'][0]['id']
        with tempfile.TemporaryDirectory(prefix='w3r-baseline-corrupt-') as directory:
            root = Path(directory)
            captures = root/'captures'
            with zipfile.ZipFile(BASELINE_ARCHIVE) as archive:
                archive.extractall(captures)
            broken = root/'baseline.zip'
            _rebuild_archive(BASELINE_ARCHIVE, broken, corrupt={corrupted_id})
            with self.assertRaisesRegex(ValueError, 'archived baseline hash mismatch'):
                validate_manifest(manifest, BASELINE_MANIFEST, captures, broken)
