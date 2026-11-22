"""Dependency-free Wolf3D indexed-capture manifest and comparison helpers."""
from __future__ import annotations

from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
import struct
import zipfile

SCHEMA_VERSION = 1
FORMAT = "w3r-indexed-v1"
SOURCE_REVISION = "R0.4-baseline-v1"


@dataclass(frozen=True)
class IndexedCapture:
    width: int
    height: int
    pixels: bytes


@dataclass(frozen=True)
class Difference:
    changed_pixels: int
    bounding_box: tuple[int, int, int, int] | None


def _read_indexed_data(data: bytes, label: str) -> IndexedCapture:
    if len(data) < 16 or data[:4] != b"W3RI":
        raise ValueError(f"{label}: not a {FORMAT} capture")
    version, width, height = struct.unpack_from("<III", data, 4)
    if version != SCHEMA_VERSION:
        raise ValueError(f"{label}: unsupported indexed schema {version}")
    if not width or not height or width > 8192 or height > 8192:
        raise ValueError(f"{label}: invalid dimensions {width}x{height}")
    expected = 16 + width * height
    if len(data) != expected:
        raise ValueError(f"{label}: expected {expected} bytes, found {len(data)}")
    return IndexedCapture(width, height, data[16:])


def read_indexed(path: Path) -> IndexedCapture:
    return _read_indexed_data(path.read_bytes(), str(path))


def compare_pixels(expected: IndexedCapture, actual: IndexedCapture) -> Difference:
    if (expected.width, expected.height) != (actual.width, actual.height):
        raise ValueError(
            f"dimension mismatch: expected {expected.width}x{expected.height}, "
            f"found {actual.width}x{actual.height}"
        )
    changed = 0
    min_x, min_y = actual.width, actual.height
    max_x = max_y = -1
    for offset, (left, right) in enumerate(zip(expected.pixels, actual.pixels)):
        if left == right:
            continue
        changed += 1
        x, y = offset % actual.width, offset // actual.width
        min_x, min_y = min(min_x, x), min(min_y, y)
        max_x, max_y = max(max_x, x), max(max_y, y)
    box = None if not changed else (min_x, min_y, max_x, max_y)
    return Difference(changed, box)


def require_exact_pixels(expected: IndexedCapture, actual: IndexedCapture,
                         label: str = "capture") -> None:
    difference = compare_pixels(expected, actual)
    if difference.changed_pixels:
        raise ValueError(
            f"{label}: pixel mismatch; changed_pixels={difference.changed_pixels}, "
            f"bounding_box={difference.bounding_box}"
        )


def compare_metadata(expected: dict, actual: dict) -> None:
    if expected != actual:
        keys = sorted(set(expected) | set(actual))
        changed = [key for key in keys if expected.get(key) != actual.get(key)]
        details = ", ".join(
            f"{key}: {expected.get(key)!r} != {actual.get(key)!r}" for key in changed
        )
        raise ValueError(f"metadata mismatch: {details}")


def fixture_identity(data_directory: Path) -> str:
    digest = hashlib.sha256()
    for path in sorted(p for p in data_directory.iterdir() if p.is_file()):
        digest.update(path.name.encode("utf-8") + b"\0")
        digest.update(path.read_bytes())
    return "sha256:" + digest.hexdigest()


def _spec(name: str) -> dict:
    parts = name.split("-")
    mode = "classic" if "classic" in parts else "enhanced"
    scene = next(scene for scene in (
        "corridor-static", "door-motion", "pushwall-motion",
        "edge-clipping", "display-transition"
    ) if name.startswith(scene))
    state = "static"
    for value in ("closed", "half-open", "open", "initial", "moving",
                  "tile-boundary", "letterbox", "resize", "restored", "wide", "auto"):
        if f"-{value}-" in f"-{name}-":
            state = value
            break
    dimensions = next(part for part in parts if "x" in part and part.replace("x", "").isdigit())
    width, height = map(int, dimensions.split("x"))
    aspect = "4:3" if mode == "classic" else "auto"
    for token, value in (("4x3", "4:3"), ("16x9", "16:9"), ("16x10", "16:10"),
                         ("21x9", "21:9"), ("32x9", "32:9")):
        if token in parts:
            aspect = value
    if mode == "enhanced" and aspect == "auto" and "auto" not in parts:
        aspect = "16:9"
    world_state = {"random_seed": 0, "view_size": 15, "weapon": "pistol"}
    if scene == "door-motion":
        world_state["door"] = {"tile": [9, 3], "position": {
            "closed": 0, "half-open": 32768, "open": 65535
        }[state]}
    if scene == "pushwall-motion":
        world_state["pushwall"] = {
            "initial": {"tile": [8, 3], "state": 1, "position": 0},
            "moving": {"tile": [8, 3], "state": 64, "position": 32},
            "tile-boundary": {"tile": [9, 3], "state": 128, "position": 0},
        }[state]
    return {
        "scene_id": scene,
        "rendering_mode": mode,
        "state": state,
        "simulation_tick": 0,
        "camera": {"tile": [2, 3], "offset": [32768, 32768], "angle": 0},
        "world_state": world_state,
        "internal_dimensions": [320, 200] if mode == "classic" else [width, height],
        "output_dimensions": [width, height],
        "aspect_ratio": aspect,
        "sdl_backend": "not-applicable-canonical-indexed",
        "capture_method": "classic-framebuffer" if mode == "classic" and dimensions == "320x200"
                          else "classic-corrected-nearest" if mode == "classic"
                          else "WL_RenderFrameAspect",
        "pixel_format": FORMAT,
        "comparison_policy": "exact-indexed",
    }


def capture_manifest(capture_directory: Path, data_directory: Path) -> dict:
    palette = capture_directory / "palette.rgb6"
    palette_bytes = palette.read_bytes()
    if len(palette_bytes) != 768:
        raise ValueError(f"{palette}: expected 768 palette bytes, found {len(palette_bytes)}")
    entries = []
    for path in sorted(capture_directory.glob("*.w3i")):
        capture = read_indexed(path)
        metadata = _spec(path.stem)
        if metadata["output_dimensions"] != [capture.width, capture.height]:
            raise ValueError(f"{path}: filename dimensions disagree with capture header")
        metadata.update({
            "id": path.stem,
            "fixture_identity": fixture_identity(data_directory),
            "source_revision": SOURCE_REVISION,
            "palette_identity": "sha256:" + hashlib.sha256(palette_bytes).hexdigest(),
            "pixel_hash": "sha256:" + hashlib.sha256(capture.pixels).hexdigest(),
        })
        entries.append(metadata)
    if not entries:
        raise ValueError(f"{capture_directory}: no indexed captures produced")
    return {"schema_version": SCHEMA_VERSION, "captures": entries}


def validate_manifest(actual: dict, expected_path: Path, capture_directory: Path,
                      archive_path: Path) -> None:
    expected = json.loads(expected_path.read_text(encoding="utf-8"))
    if expected.get("schema_version") != actual.get("schema_version"):
        raise ValueError("metadata mismatch: manifest schema version")
    expected_entries = {entry["id"]: entry for entry in expected["captures"]}
    actual_entries = {entry["id"]: entry for entry in actual["captures"]}
    if expected_entries.keys() != actual_entries.keys():
        raise ValueError("metadata mismatch: capture ID set")
    with zipfile.ZipFile(archive_path) as archive:
        for capture_id in sorted(expected_entries):
            left, right = dict(expected_entries[capture_id]), dict(actual_entries[capture_id])
            expected_hash, actual_hash = left.pop("pixel_hash"), right.pop("pixel_hash")
            compare_metadata(left, right)
            if expected_hash == actual_hash:
                continue
            baseline = _read_indexed_data(archive.read(capture_id + ".w3i"), capture_id)
            candidate = read_indexed(capture_directory / (capture_id + ".w3i"))
            difference = compare_pixels(baseline, candidate)
            raise ValueError(
                f"{capture_id}: pixel hash mismatch; changed_pixels="
                f"{difference.changed_pixels}, bounding_box={difference.bounding_box}, "
                f"expected={expected_hash}, actual={actual_hash}"
            )


def write_manifest(path: Path, manifest: dict) -> None:
    path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def write_baseline_archive(path: Path, capture_directory: Path) -> None:
    with zipfile.ZipFile(path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for capture in sorted(capture_directory.glob("*.w3i")):
            info = zipfile.ZipInfo(capture.name, date_time=(1980, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o100644 << 16
            archive.writestr(info, capture.read_bytes(), compresslevel=9)


def write_bmp(path: Path, capture: IndexedCapture, palette_rgb6: bytes) -> None:
    row_size = (capture.width + 3) & ~3
    pixel_size = row_size * capture.height
    offset = 14 + 40 + 256 * 4
    header = struct.pack("<2sIHHI", b"BM", offset + pixel_size, 0, 0, offset)
    dib = struct.pack("<IIIHHIIIIII", 40, capture.width, capture.height, 1, 8,
                      0, pixel_size, 2835, 2835, 256, 256)
    colors = bytearray()
    for index in range(256):
        red, green, blue = palette_rgb6[index * 3:index * 3 + 3]
        expand = lambda value: (value << 2) | (value >> 4)
        colors.extend((expand(blue), expand(green), expand(red), 0))
    padding = b"\0" * (row_size - capture.width)
    with path.open("wb") as output:
        output.write(header + dib + colors)
        for y in range(capture.height - 1, -1, -1):
            start = y * capture.width
            output.write(capture.pixels[start:start + capture.width] + padding)


def create_review_bmps(capture_directory: Path) -> None:
    palette = (capture_directory / "palette.rgb6").read_bytes()
    for indexed in sorted(capture_directory.glob("*.w3i")):
        write_bmp(indexed.with_suffix(".bmp"), read_indexed(indexed), palette)
