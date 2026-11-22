import importlib.util
import json
import os
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RUNNER_PATH = ROOT / "tools" / "validation" / "run.py"


def load_runner():
    spec = importlib.util.spec_from_file_location("w3r_validation_run", RUNNER_PATH)
    mod = importlib.util.module_from_spec(spec)
    assert spec and spec.loader
    sys.modules[spec.name] = mod
    spec.loader.exec_module(mod)
    return mod


class ValidationRunnerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.runner = load_runner()

    def make_job(self, **kw):
        defaults = dict(
            id="j1",
            milestone="R0.6",
            milestones=("R0.6", "all"),
            description="test job",
            command=[sys.executable, "-c", "print('ok')"],
            timeout=30,
            mandatory=True,
            category="meta",
        )
        defaults.update(kw)
        return self.runner.Job(**defaults)

    def test_successful_job_produces_pass(self):
        with tempfile.TemporaryDirectory() as td:
            run_dir = Path(td)
            job = self.make_job(command=[sys.executable, "-c", "print('hello')"])
            r = self.runner.run_job(job, run_dir)
            self.assertEqual(r.status, "PASS")
            self.assertEqual(r.exit_code, 0)
            self.assertTrue((run_dir / "j1.log").exists())
            self.assertIn("hello", (run_dir / "j1.log").read_text())

    def test_nonzero_exit_produces_fail(self):
        with tempfile.TemporaryDirectory() as td:
            run_dir = Path(td)
            job = self.make_job(command=[sys.executable, "-c", "import sys; sys.exit(7)"])
            r = self.runner.run_job(job, run_dir)
            self.assertEqual(r.status, "FAIL")
            self.assertEqual(r.exit_code, 7)

    def test_timeout_produces_timeout_and_kills_child(self):
        with tempfile.TemporaryDirectory() as td:
            run_dir = Path(td)
            pidfile = run_dir / "child.pid"
            cmd = [sys.executable, "-c",
                   "import os,time,subprocess,sys;"
                   f"p=subprocess.Popen([sys.executable,'-c','import time; time.sleep(60)']);"
                   f"open(r'{pidfile}','w').write(str(p.pid));"
                   "time.sleep(60)"]
            job = self.make_job(command=cmd, timeout=2)
            start = time.time()
            r = self.runner.run_job(job, run_dir)
            elapsed = time.time() - start
            self.assertEqual(r.status, "TIMEOUT")
            self.assertLess(elapsed, 15)
            # child process should be dead
            if pidfile.exists():
                child_pid = int(pidfile.read_text().strip())
                # wait a moment for cleanup
                for _ in range(20):
                    try:
                        os.kill(child_pid, 0)
                    except ProcessLookupError:
                        break
                    time.sleep(0.1)
                else:
                    self.fail(f"child process {child_pid} still alive after timeout")

    def test_missing_prereq_mandatory_is_not_run(self):
        with tempfile.TemporaryDirectory() as td:
            run_dir = Path(td)
            job = self.make_job(requires=("sdl-sdk",), mandatory=True)
            # Force missing prereq by pointing SDL_SDK to nonexistent
            old = os.environ.get("SDL_SDK")
            os.environ["SDL_SDK"] = "/tmp/definitely-missing-sdl-sdk"
            try:
                r = self.runner.run_job(job, run_dir)
            finally:
                if old is None:
                    os.environ.pop("SDL_SDK", None)
                else:
                    os.environ["SDL_SDK"] = old
            self.assertEqual(r.status, "NOT RUN")
            self.assertIsNone(r.exit_code)

    def test_missing_prereq_optional_is_skip(self):
        with tempfile.TemporaryDirectory() as td:
            run_dir = Path(td)
            job = self.make_job(requires=("sdl-sdk",), mandatory=False)
            old = os.environ.get("SDL_SDK")
            os.environ["SDL_SDK"] = "/tmp/definitely-missing-sdl-sdk"
            try:
                r = self.runner.run_job(job, run_dir)
            finally:
                if old is None:
                    os.environ.pop("SDL_SDK", None)
                else:
                    os.environ["SDL_SDK"] = old
            self.assertIn(r.status, ("SKIP", "NOT RUN"))

    def test_json_summary_matches_execution(self):
        with tempfile.TemporaryDirectory() as td:
            run_dir = Path(td) / "R0.6" / "testrun"
            run_dir.mkdir(parents=True)
            j1 = self.make_job(id="pass-job", command=[sys.executable, "-c", "pass"])
            j2 = self.make_job(id="fail-job", command=[sys.executable, "-c", "import sys; sys.exit(2)"])
            started = self.runner._utcnow()
            r1 = self.runner.run_job(j1, run_dir)
            r2 = self.runner.run_job(j2, run_dir)
            ended = self.runner._utcnow()
            sp = self.runner.write_summary(run_dir, "R0.6", [r1, r2], started, ended)
            data = json.loads(sp.read_text())
            self.assertEqual(data["overall_status"], "FAIL")
            self.assertEqual(len(data["jobs"]), 2)
            ids = [j["id"] for j in data["jobs"]]
            self.assertEqual(ids, ["pass-job", "fail-job"])
            self.assertEqual(data["jobs"][0]["status"], "PASS")
            self.assertEqual(data["jobs"][1]["status"], "FAIL")

    def test_required_job_failure_causes_nonzero_overall(self):
        r1 = self.runner.JobResult("a","R0.6","d",["x"],"meta",True,10,"PASS",0,1.0,"","","l")
        r2 = self.runner.JobResult("b","R0.6","d",["x"],"meta",True,10,"FAIL",1,1.0,"","","l")
        self.assertEqual(self.runner.overall_status([r1,r2]), "FAIL")

    def test_optional_skip_does_not_fail_overall(self):
        r1 = self.runner.JobResult("a","R0.6","d",["x"],"meta",True,10,"PASS",0,1.0,"","","l")
        r2 = self.runner.JobResult("b","R0.6","d",["x"],"meta",False,10,"SKIP",None,0.0,"","","l","missing")
        self.assertEqual(self.runner.overall_status([r1,r2]), "PASS")

    def test_unknown_milestone_rejected(self):
        proc = subprocess.run([sys.executable, str(RUNNER_PATH), "R9.9"], capture_output=True, text=True)
        self.assertNotEqual(proc.returncode, 0)
        self.assertIn("unknown milestone", proc.stderr + proc.stdout)

    def test_invalid_args_rejected(self):
        proc = subprocess.run([sys.executable, str(RUNNER_PATH)], capture_output=True, text=True)
        self.assertNotEqual(proc.returncode, 0)

    def test_dry_run_lists_selected_jobs(self):
        proc = subprocess.run([sys.executable, str(RUNNER_PATH), "R0.3", "--dry-run"], capture_output=True, text=True)
        self.assertEqual(proc.returncode, 0, proc.stdout + proc.stderr)
        self.assertIn("r03-engine-synthetic", proc.stdout)
