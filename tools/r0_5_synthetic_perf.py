#!/usr/bin/env python3
"""R0.5 synthetic CPU/SDL performance baseline reproduction workflow.

Generates synthetic fixtures, compiles benchmark binaries, runs CPU and SDL
(dummy/software) measurements, and writes logs/CSV under build/validation/R0.5/.
No owned Wolfenstein data, no network, no new dependencies.
"""
from __future__ import annotations
import argparse, os, shutil, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tests"))
from game_fixtures import game_files  # noqa: E402

SDK_DEFAULT = ROOT.parent / "cart/vendor/SDL"
CC = os.environ.get("CC", "gcc")
CFLAGS = ["-std=c11", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion", "-Wshadow", "-Werror", "-g"]
LDFLAGS = ["-pthread", "-lm"]


def run(cmd, log_path: Path, timeout: int, env=None, cwd=ROOT):
    log_path.parent.mkdir(parents=True, exist_ok=True)
    print(f"START {log_path.name}: {' '.join(cmd)}", flush=True)
    start = time.time()
    with log_path.open("w", encoding="utf-8") as log:
        log.write(f"COMMAND: {' '.join(cmd)}\n")
        log.write(f"TIMEOUT: {timeout}s\n")
        log.flush()
        try:
            proc = subprocess.run(cmd, cwd=cwd, env=env, stdout=log, stderr=subprocess.STDOUT,
                                  text=True, timeout=timeout)
            rc = proc.returncode
        except subprocess.TimeoutExpired:
            log.write("\nRESULT: TIMEOUT\n")
            rc = 124
        log.write(f"EXIT: {rc}\n")
    elapsed = time.time() - start
    status = "PASS" if rc == 0 else "FAIL"
    print(f"{status} {log_path.name} exit={rc} elapsed={elapsed:.1f}s", flush=True)
    return rc


def compile_bench(sdk: Path, out: Path, extra_flags, sources, log_path: Path, timeout: int):
    cmd = [CC, *CFLAGS, *extra_flags, "-I./src", f"-I{sdk/'include'}",
           "tests/synthetic_perf_benchmark.c", *sources,
           str(sdk / "build/libSDL3.a"), *LDFLAGS, "-o", str(out)]
    return run(cmd, log_path, timeout)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sdk", default=os.environ.get("SDL_SDK", str(SDK_DEFAULT)),
                    help="SDL3 SDK path (default: ../cart/vendor/SDL or $SDL_SDK)")
    ap.add_argument("--out", default=str(ROOT / "build/validation/R0.5"),
                    help="Output directory for logs/data/binaries")
    ap.add_argument("--timeout", type=int, default=600, help="Per-step timeout seconds")
    args = ap.parse_args()

    sdk = Path(args.sdk)
    out = Path(args.out)
    logs = out / "logs"
    data = out / "perf-data"
    user_cpu = out / "perf-user"
    user_sdl = out / "perf-user-sdl"
    out.mkdir(parents=True, exist_ok=True)
    logs.mkdir(parents=True, exist_ok=True)

    if not (sdk / "build/libSDL3.a").is_file():
        print(f"FAIL prerequisite missing SDL static lib: {sdk/'build/libSDL3.a'}", file=sys.stderr)
        return 2
    if shutil.which(CC) is None:
        print(f"FAIL prerequisite missing compiler: {CC}", file=sys.stderr)
        return 2

    # Synthetic data
    if data.exists():
        shutil.rmtree(data)
    data.mkdir(parents=True)
    game_files(data)
    print(f"PASS synthetic fixtures generated at {data}", flush=True)

    engine = sorted(str(p) for p in (ROOT / "src/engine").glob("*.c"))
    platform = sorted(str(p) for p in (ROOT / "src/platform").glob("*.c"))
    plat_min = [str(ROOT / "src/platform/data_file.c"), str(ROOT / "src/platform/save_file.c")]

    rc = 0
    cpu_bin = out / "synthetic_perf_bench"
    sdl_bin = out / "synthetic_perf_bench_sdl"
    rc |= compile_bench(sdk, cpu_bin, [], [*engine, *plat_min], logs / "perf-compile-cpu.log", args.timeout)
    rc |= compile_bench(sdk, sdl_bin, ["-DWOLF3D_BENCHMARK"], [*engine, *platform], logs / "perf-compile-sdl.log", args.timeout)
    if rc != 0:
        print("FAIL compilation failed; see logs", file=sys.stderr)
        return 1

    if user_cpu.exists():
        shutil.rmtree(user_cpu)
    user_cpu.mkdir(parents=True)
    rc |= run([str(cpu_bin), str(data), str(user_cpu)], logs / "perf-cpu.stdout.log", args.timeout)

    if user_sdl.exists():
        shutil.rmtree(user_sdl)
    user_sdl.mkdir(parents=True)
    env = dict(os.environ)
    env["SDL_VIDEODRIVER"] = "dummy"
    env["SDL_RENDER_DRIVER"] = "software"
    env["SDL_AUDIODRIVER"] = "dummy"
    rc |= run([str(sdl_bin), str(data), str(user_sdl), "--sdl"], logs / "perf-sdl.stdout.log", args.timeout, env=env)

    # Extract CSV/META for convenience
    for name in ["perf-cpu.stdout.log", "perf-sdl.stdout.log"]:
        p = logs / name
        csv = logs / name.replace(".stdout.log", ".csv")
        if p.exists():
            rows = [l for l in p.read_text(encoding="utf-8", errors="replace").splitlines()
                    if l.startswith("CSV_HEADER,") or l.startswith("CSV,") or l.startswith("META,")]
            csv.write_text("\n".join(rows) + ("\n" if rows else ""), encoding="utf-8")
            print(f"PASS wrote {csv}", flush=True)

    summary = out / "perf-summary.txt"
    lines = [f"R0.5 synthetic perf summary: {'PASS' if rc == 0 else 'FAIL'}\n"]
    for p in [logs / "perf-cpu.csv", logs / "perf-sdl.csv"]:
        if p.exists():
            lines.append(p.read_text(encoding="utf-8", errors="replace"))
    summary.write_text("".join(lines), encoding="utf-8")
    print(f"{'PASS' if rc == 0 else 'FAIL'} summary written to {summary}", flush=True)
    return 0 if rc == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
