#!/usr/bin/env python3
"""Unified R0 validation runner.

Lightweight orchestrator over existing Python unittest suites and scripts.
No third-party dependencies. Writes logs + JSON summary under build/validation/.
"""
from __future__ import annotations

import argparse
import json
import os
import platform
import shutil
import signal
import subprocess
import sys
import time
from dataclasses import asdict, dataclass, field
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD_VALIDATION = ROOT / "build" / "validation"
PY = sys.executable


@dataclass
class Job:
    id: str
    milestone: str  # primary milestone
    milestones: tuple[str, ...]  # milestones that include this job
    description: str
    command: list[str]
    timeout: int
    mandatory: bool = True
    category: str = "cpu"  # cpu|graphics|sanitizer|performance|meta
    env: dict[str, str] = field(default_factory=dict)
    requires: tuple[str, ...] = ()  # e.g. "sdl-sdk","cc"
    notes: str = ""


@dataclass
class JobResult:
    id: str
    milestone: str
    description: str
    command: list[str]
    category: str
    mandatory: bool
    timeout: int
    status: str  # PASS|FAIL|TIMEOUT|SKIP|NOT RUN
    exit_code: int | None
    duration_s: float
    started_at: str
    ended_at: str
    log_file: str
    message: str = ""


def _utcnow():
    return datetime.now(timezone.utc).isoformat()


def _run_id():
    return datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")


def _display_path(p: Path) -> str:
    try:
        return str(p.resolve().relative_to(ROOT.resolve()))
    except Exception:
        return str(p)


def _has_sdl_sdk() -> bool:
    sdk = Path(os.environ.get("SDL_SDK", ROOT.parent / "cart/vendor/SDL"))
    return (sdk / "build/libSDL3.a").is_file() or (sdk / "include/SDL3/SDL.h").is_file()


def _has_cc() -> bool:
    return shutil.which(os.environ.get("CC", "gcc")) is not None


def _prereq_missing(job: Job) -> str | None:
    for req in job.requires:
        if req == "sdl-sdk" and not _has_sdl_sdk():
            return f"missing prerequisite: SDL SDK (set SDL_SDK or build ../cart/vendor/SDL)"
        if req == "cc" and not _has_cc():
            return f"missing prerequisite: C compiler ({os.environ.get('CC','gcc')})"
        if req == "asan":
            # Best-effort check: gcc present; actual runtime checked by test run.
            if not _has_cc():
                return "missing prerequisite: C compiler for ASan/UBSan"
    return None


def build_jobs() -> list[Job]:
    env_base = {}
    sdl_env = {
        "SDL_VIDEODRIVER": "dummy",
        "SDL_RENDER_DRIVER": "software",
        "SDL_AUDIODRIVER": "dummy",
    }
    sanitize_env = {"SANITIZE": "1"}

    jobs: list[Job] = []

    # R0.3 - deterministic headless CPU harness
    jobs.append(Job(
        id="r03-engine-synthetic",
        milestone="R0.3",
        milestones=("R0.3", "R0.6", "all"),
        description="Engine synthetic/headless unit tests (R0.3 scope; owned-data cases may skip)",
        command=[PY, "-m", "unittest", "discover", "-s", "tests", "-p", "test_engine.py", "-v"],
        timeout=900,
        mandatory=True,
        category="cpu",
        env=env_base,
    ))

    # R0.4 - matched render baselines / graphics
    for tid, desc, timeout in [
        ("r04-render", "Render synthetic baseline and comparator tests", 600),
        ("r04-display", "Display/presentation geometry tests", 600),
        ("r04-presentation", "Presentation timing/interpolation tests", 600),
        ("r04-classic", "Classic renderer/view tests", 600),
    ]:
        p = tid.replace("r04-", "test_") + ".py"
        jobs.append(Job(
            id=tid,
            milestone="R0.4",
            milestones=("R0.4", "R0.6", "all"),
            description=desc,
            command=[PY, "-m", "unittest", "discover", "-s", "tests", "-p", p, "-v"],
            timeout=timeout,
            mandatory=True,
            category="graphics",
            env=sdl_env,
            requires=("sdl-sdk", "cc"),
        ))

    # R0.5 - sanitizer suites (representative instrumented harness coverage)
    sanitizer_tests = [
        ("r05-san-engine", "test_engine.py", 1200, "Sanitizer: engine/lifecycle/resource harnesses"),
        ("r05-san-render", "test_render.py", 1200, "Sanitizer: render Classic/Enhanced harnesses"),
        ("r05-san-classic", "test_classic.py", 1200, "Sanitizer: classic renderer harnesses"),
        ("r05-san-display", "test_display.py", 1200, "Sanitizer: display lifecycle harnesses"),
        ("r05-san-presentation", "test_presentation.py", 1200, "Sanitizer: presentation/frame-clock harnesses"),
        ("r05-san-save", "test_save.py", 1200, "Sanitizer: save/resource harnesses"),
        ("r05-san-config", "test_config.py", 1200, "Sanitizer: config resource harnesses"),
        ("r05-san-controls", "test_controls.py", 900, "Sanitizer: controls harnesses"),
        ("r05-san-loading", "test_loading.py", 900, "Sanitizer: loading harnesses"),
        ("r05-san-audio", "test_audio.py", 900, "Sanitizer: audio harnesses"),
        ("r05-san-signon", "test_signon.py", 900, "Sanitizer: signon harnesses"),
        ("r05-san-dos-menus", "test_dos_menus.py", 900, "Sanitizer: DOS menu harnesses"),
        ("r05-san-dos-ui-reference", "test_dos_ui_reference.py", 900, "Sanitizer: DOS UI reference harnesses"),
        ("r05-san-demo-sync", "test_demo_sync.py", 1200, "Sanitizer: demo sync harnesses"),
    ]
    for jid, pat, timeout, desc in sanitizer_tests:
        jobs.append(Job(
            id=jid,
            milestone="R0.5",
            milestones=("R0.5", "all"),
            description=desc,
            command=[PY, "-m", "unittest", "discover", "-s", "tests", "-p", pat, "-v"],
            timeout=timeout,
            mandatory=True,
            category="sanitizer",
            env={**sdl_env, **sanitize_env},
            requires=("sdl-sdk", "cc", "asan"),
            notes="Instrumented C harnesses only where wrappers compile with SANITIZE=1; prebuilt WOLF3D_BINARY subprocesses are not ASan-instrumented.",
        ))

    # R0.5 performance
    jobs.append(Job(
        id="r05-perf-synthetic",
        milestone="R0.5",
        milestones=("R0.5", "all"),
        description="Synthetic CPU/SDL performance baseline (evidence, no FPS gate)",
        command=[PY, "tools/r0_5_synthetic_perf.py", "--timeout", "600"],
        timeout=1200,
        mandatory=True,
        category="performance",
        env=sdl_env,
        requires=("sdl-sdk", "cc"),
        notes="Performance is evidence; no target FPS pass/fail gate is applied.",
    ))

    # R0.6 - validation runner tests
    jobs.append(Job(
        id="r06-runner-tests",
        milestone="R0.6",
        milestones=("R0.6", "all"),
        description="Validation runner orchestration unit tests",
        command=[PY, "-m", "unittest", "discover", "-s", "tests", "-p", "test_validation_runner.py", "-v"],
        timeout=300,
        mandatory=True,
        category="meta",
    ))

    return jobs


STATUS_PASS = "PASS"
STATUS_FAIL = "FAIL"
STATUS_TIMEOUT = "TIMEOUT"
STATUS_SKIP = "SKIP"
STATUS_NOT_RUN = "NOT RUN"


def select_jobs(jobs: list[Job], milestone: str, categories: set[str] | None = None) -> list[Job]:
    m = milestone.upper()
    selected = [j for j in jobs if m in [x.upper() for x in j.milestones] or m == "ALL" and "all" in [x.lower() for x in j.milestones]]
    if categories:
        selected = [j for j in selected if j.category in categories]
    # de-duplicate by id preserving order
    seen = set()
    out = []
    for j in selected:
        if j.id not in seen:
            seen.add(j.id)
            out.append(j)
    return out


def _pgid_alive(pgid: int) -> bool:
    try:
        os.killpg(pgid, 0)
        return True
    except ProcessLookupError:
        return False
    except PermissionError:
        return True
    except Exception:
        return False


def kill_process_group(proc: subprocess.Popen, pgid: int | None = None) -> None:
    """Best-effort bounded cleanup of a job process group.

    The direct child is always reaped. Group signals are only sent to the
    captured pgid (proc.pid at start_new_session launch) and guarded by liveness
    checks to avoid targeting reused process-group IDs.
    """
    if pgid is None:
        pgid = proc.pid

    def _term_group() -> None:
        if not _pgid_alive(pgid):
            return
        try:
            os.killpg(pgid, signal.SIGTERM)
        except ProcessLookupError:
            pass
        except PermissionError:
            pass
        except Exception:
            pass

    def _kill_group() -> None:
        if not _pgid_alive(pgid):
            return
        try:
            os.killpg(pgid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        except PermissionError:
            pass
        except Exception:
            pass

    # Escalate even if parent already exited: descendants may still run.
    _term_group()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        _kill_group()
        try:
            proc.kill()
        except Exception:
            pass
        try:
            proc.wait(timeout=5)
        except Exception:
            pass
    except Exception:
        try:
            proc.kill()
        except Exception:
            pass
        try:
            proc.wait(timeout=5)
        except Exception:
            pass

    # Final bounded check: if group remains alive (e.g. descendant ignored TERM
    # and direct child already reaped), send KILL once more.
    if _pgid_alive(pgid):
        _kill_group()


def run_job(job: Job, run_dir: Path) -> JobResult:
    log_path = run_dir / f"{job.id}.log"
    log_path.parent.mkdir(parents=True, exist_ok=True)
    started = time.time()
    started_at = _utcnow()
    print(f"START [{job.milestone}] {job.id}: {job.description}", flush=True)
    print(f"  timeout={job.timeout}s cmd={' '.join(job.command)}", flush=True)

    prereq = _prereq_missing(job)
    if prereq:
        status = STATUS_NOT_RUN if job.mandatory else STATUS_SKIP
        ended_at = _utcnow()
        msg = prereq
        log_path.write_text(f"{msg}\n", encoding="utf-8")
        print(f"{status} [{job.milestone}] {job.id} ({msg})", flush=True)
        return JobResult(job.id, job.milestone, job.description, job.command, job.category,
                         job.mandatory, job.timeout, status, None, 0.0, started_at, ended_at,
                         _display_path(log_path), msg)

    env = os.environ.copy()
    env.update(job.env)
    # Never allow baseline updates from validation runner
    env.pop("WOLF3D_UPDATE_BASELINES", None)

    header = [
        f"job={job.id}",
        f"milestone={job.milestone}",
        f"description={job.description}",
        f"timeout={job.timeout}",
        f"command={' '.join(job.command)}",
        f"started_at={started_at}",
        "",
    ]
    exit_code: int | None = None
    status = STATUS_FAIL
    message = ""
    timed_out = False

    with log_path.open("w", encoding="utf-8") as log:
        log.write("\n".join(header))
        log.flush()
        try:
            proc = subprocess.Popen(
                job.command,
                cwd=ROOT,
                env=env,
                stdout=log,
                stderr=subprocess.STDOUT,
                text=True,
                start_new_session=True,
            )
        except Exception as e:
            ended_at = _utcnow()
            duration = time.time() - started
            msg = f"failed to start: {e}"
            log.write(msg + "\n")
            print(f"FAIL [{job.milestone}] {job.id} ({msg})", flush=True)
            return JobResult(job.id, job.milestone, job.description, job.command, job.category,
                             job.mandatory, job.timeout, STATUS_FAIL, None, duration, started_at, ended_at,
                             _display_path(log_path), msg)
        pgid = proc.pid
        try:
            proc.wait(timeout=job.timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            kill_process_group(proc, pgid)
            try:
                proc.wait(timeout=5)
            except Exception:
                pass
        except KeyboardInterrupt:
            kill_process_group(proc, pgid)
            try:
                proc.wait(timeout=5)
            except Exception:
                pass
            raise
        # Best-effort cleanup for descendants that outlive the direct child.
        if _pgid_alive(pgid):
            kill_process_group(proc, pgid)

        exit_code = proc.returncode
        if timed_out:
            status = STATUS_TIMEOUT
            message = f"timed out after {job.timeout}s"
            log.write(f"\nRESULT: TIMEOUT after {job.timeout}s\n")
        elif exit_code == 0:
            status = STATUS_PASS
            log.write("\nRESULT: PASS\n")
        else:
            status = STATUS_FAIL
            message = f"exit {exit_code}"
            log.write(f"\nRESULT: FAIL exit={exit_code}\n")

    ended_at = _utcnow()
    duration = time.time() - started
    print(f"{status} [{job.milestone}] {job.id} exit={exit_code} duration={duration:.1f}s" + (f" ({message})" if message else ""), flush=True)
    return JobResult(job.id, job.milestone, job.description, job.command, job.category,
                     job.mandatory, job.timeout, status, exit_code, duration, started_at, ended_at,
                     _display_path(log_path), message)


def overall_status(results: list[JobResult]) -> str:
    if not results:
        return STATUS_FAIL
    for r in results:
        if r.mandatory and r.status in (STATUS_FAIL, STATUS_TIMEOUT, STATUS_NOT_RUN):
            return STATUS_FAIL
        if r.mandatory and r.status == STATUS_SKIP:
            # mandatory skip is treated as failure unless explicitly not possible - be conservative
            return STATUS_FAIL
    # All mandatory jobs passed
    if any(r.status == STATUS_PASS for r in results):
        return STATUS_PASS
    return STATUS_FAIL


def write_summary(run_dir: Path, milestone: str, results: list[JobResult], started_at: str, ended_at: str) -> Path:
    summary = {
        "schema_version": 1,
        "milestone": milestone,
        "started_at": started_at,
        "ended_at": ended_at,
        "duration_s": sum(r.duration_s for r in results),
        "overall_status": overall_status(results),
        "environment": {
            "platform": platform.platform(),
            "python": sys.version.split()[0],
            "cwd": str(ROOT),
            "cc": shutil.which(os.environ.get("CC", "gcc")) or "",
        },
        "jobs": [asdict(r) for r in results],
    }
    path = run_dir / "summary.json"
    path.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return path


def print_summary(results: list[JobResult], summary_path: Path) -> None:
    counts = {}
    for r in results:
        counts[r.status] = counts.get(r.status, 0) + 1
    overall = overall_status(results)
    print("\n=== Validation summary ===", flush=True)
    for r in results:
        flag = "*" if r.mandatory else " "
        print(f" {flag} {r.status:7s} {r.id:28s} {r.duration_s:7.1f}s exit={r.exit_code} {r.log_file}", flush=True)
    print(f"--- counts: {counts}", flush=True)
    print(f"--- overall: {overall}", flush=True)
    print(f"--- summary: {summary_path.relative_to(ROOT)}", flush=True)


def parse_args(argv: list[str]) -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="Run Wolf3D Revive milestone validation jobs.")
    ap.add_argument("milestone", nargs="?", help="Milestone to run: R0.3, R0.4, R0.5, R0.6, all")
    ap.add_argument("--list", action="store_true", help="List jobs and exit")
    ap.add_argument("--dry-run", action="store_true", help="Select and print jobs without executing")
    ap.add_argument("--category", action="append", choices=["cpu", "graphics", "sanitizer", "performance", "meta"],
                    help="Only run jobs in selected category (repeatable)")
    ap.add_argument("--run-id", help="Override run id (default: UTC timestamp)")
    ap.add_argument("--out-root", default=str(BUILD_VALIDATION), help="Validation output root")
    ap.add_argument("--job", action="append", dest="jobs", help="Run only specific job id(s) (repeatable)")
    args = ap.parse_args(argv)
    if not args.list and not args.milestone:
        ap.error("milestone is required unless --list is used")
    return args


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv if argv is not None else sys.argv[1:])
    jobs = build_jobs()
    known_milestones = {"R0.3", "R0.4", "R0.5", "R0.6", "ALL"}

    if args.list:
        for j in jobs:
            ms = ",".join(j.milestones)
            mand = "mandatory" if j.mandatory else "optional"
            print(f"{j.id:28s} {j.milestone:5s} {j.category:11s} {mand:9s} timeout={j.timeout:4d}s  [{ms}]  {j.description}")
        return 0

    milestone = args.milestone.upper()
    if milestone not in known_milestones:
        print(f"FAIL unknown milestone: {args.milestone}. Expected one of {sorted(known_milestones)}", file=sys.stderr)
        return 2

    categories = set(args.category) if args.category else None
    selected = select_jobs(jobs, milestone, categories)
    if args.jobs:
        wanted = set(args.jobs)
        selected = [j for j in selected if j.id in wanted]
        missing = wanted - {j.id for j in selected}
        if missing:
            print(f"FAIL unknown/unselcted job id(s): {sorted(missing)}", file=sys.stderr)
            return 2
    if not selected:
        print("FAIL no jobs selected", file=sys.stderr)
        return 2

    run_id = args.run_id or _run_id()
    run_dir = Path(args.out_root) / milestone / run_id
    run_dir.mkdir(parents=True, exist_ok=True)

    print(f"=== Validation run {milestone} run_id={run_id} ===", flush=True)
    print(f"output={run_dir.relative_to(ROOT)} jobs={len(selected)}", flush=True)
    for j in selected:
        mand = "mandatory" if j.mandatory else "optional"
        print(f" - {j.id} [{j.category}/{mand}] timeout={j.timeout}s :: {j.description}", flush=True)

    if args.dry_run:
        return 0

    started_at = _utcnow()
    results: list[JobResult] = []
    try:
        for j in selected:
            results.append(run_job(j, run_dir))
    except KeyboardInterrupt:
        print("FAIL interrupted", file=sys.stderr)
        # mark remaining as NOT RUN
        done = {r.id for r in results}
        now = _utcnow()
        for j in selected:
            if j.id in done:
                continue
            lp = run_dir / f"{j.id}.log"
            lp.write_text("NOT RUN: interrupted\n", encoding="utf-8")
            results.append(JobResult(j.id, j.milestone, j.description, j.command, j.category,
                                     j.mandatory, j.timeout, STATUS_NOT_RUN, None, 0.0, now, now,
                                     str(lp.relative_to(ROOT)), "interrupted"))
        ended_at = _utcnow()
        sp = write_summary(run_dir, milestone, results, started_at, ended_at)
        print_summary(results, sp)
        return 130

    ended_at = _utcnow()
    summary_path = write_summary(run_dir, milestone, results, started_at, ended_at)
    print_summary(results, summary_path)
    return 0 if overall_status(results) == STATUS_PASS else 1


if __name__ == "__main__":
    raise SystemExit(main())
