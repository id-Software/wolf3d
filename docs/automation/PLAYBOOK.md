# Reusable workflow playbook and automation backlog

Status: PROPOSAL / WORKFLOW SPECIFICATION. This file is long-lived operational knowledge for Codex and Gemma, not a replacement for source inspection.

## Rule: solve once, reuse across agents

Whenever a procedure requires repeated investigation, repeated commands or repeated manual validation:
1. Search this playbook, `tools/`, `tests/` and existing Rn.m validation reports first.
2. Identify stable inputs, outputs, preconditions, failure conditions and side effects.
3. Implement/extend a **single canonical script** for recurring deterministic work, with unit or smoke tests. For non-automatable actions, maintain one precise repeatable checklist.
4. Record command, owner, parameters, dependencies, expected exit codes, evidence output and platform limitations here.
5. Invoke that workflow in subsequent milestone acceptance criteria; do not ask another agent to rediscover it.
6. If a task cannot safely be automated, capture why and keep a documented manual gate.

A second occurrence triggers an automation candidate. Tasks with high risk or likely repeated use can be prioritised immediately. Do not add complicated orchestration for a one-off task.

## Canonical automation candidates

| ID | Repeated work | Proposed canonical interface | Planned milestone | Status |
| --- | --- | --- | --- | --- |
| AUTO-01 | Build/smoke environment checks | `tools/doctor.sh` | R0.1/R0.6 | R0.1 PROCEDURE DOCUMENTED; SCRIPT NOT IMPLEMENTED |
| AUTO-02 | Fast headless/unit regressions | `tools/test.sh` | R0.3/R0.6 | EXISTING UNITTEST SUITE; WRAPPER NOT IMPLEMENTED |
| AUTO-03 | SDL graphics/display regression | `tools/test.sh --graphics` | R0.4/R0.6 | EXISTING UNITTEST COVERAGE + R0.4 BASELINE; WRAPPER DEFERRED TO R0.6 |
| AUTO-04 | ASan/UBSan test matrix | `SANITIZE=1 tools/test.sh` | R0.5/R0.6 | NOT IMPLEMENTED |
| AUTO-05 | Revision-based gating and progress/logs | `python3 tools/validation/run.py R0.4` | R0.6 | NOT IMPLEMENTED |
| AUTO-06 | Deterministic capture comparison | `python3 -m unittest discover -s tests -p test_render.py -v` | R0.4 | IMPLEMENTED IN EXISTING RENDER UNITTEST DRIVER |
| AUTO-07 | Content provenance and banned-asset scanning | `tools/audit_content.py` (interface subject to design) | R1 | NOT IMPLEMENTED |
| AUTO-08 | Map/content fixture validation | `tools/validate_content.py` (interface subject to design) | R2 | NOT IMPLEMENTED |
| AUTO-09 | Workshop round-trip integration | revision-selected test fixture | R6.6 | NOT IMPLEMENTED |
| AUTO-10 | PR evidence/checklist creation | CI/template based on real test metadata | R0.6 | NOT IMPLEMENTED |
| AUTO-11 | Texture/resource dimension inventory | extend future `tools/audit_content.py` with bounded metadata-only inspection | R1/R2 | R0.1 ONE-OFF INVENTORY ONLY; NOT IMPLEMENTED |

The table is a **backlog**; no script is claimed to exist. Replace speculative names with tested stable commands when each revision delivers.

## Verified R0.1 procedures

These commands were inspected and executed on 22 November 2026. Full results,
limitations and tool versions are in [`docs/milestones/R0/R0.1.md`](../milestones/R0/R0.1.md).
They are the current command surface, not the proposed R0.6 wrappers.

Prerequisites:

- CART 0.2.0 available as `build/tools/cart` (ignored) or by an explicit path.
- a built SDL3 tree at `../cart/vendor/SDL` for profile `cart-sdl`.
- GCC and Python 3; owned-data checks additionally require a legal external
  WL6 directory supplied through `WOLF3D_DATA` or `--data`.
- no dependency installer or game-data downloader is invoked by these commands.

Configured optimized build:

```sh
./build/tools/cart build --profile cart-sdl --verbose
```

CART's desktop `build` command has no named debug/release mode. The profile
build currently receives CART's `-O0 -g` followed by project `-O2`; GCC applies
the final optimization option. A strict debug compile of the same source set is:

```sh
gcc -std=c11 -O0 -g3 \
  -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror \
  -Isrc -I../cart/vendor/SDL/include \
  src/main.c src/engine/*.c src/platform/*.c \
  ../cart/vendor/SDL/build/libSDL3.a -pthread -lm \
  -o build/wolf3d-revive-debug
```

`cart doctor` does not accept profiles. In the current layout its base check
correctly reports missing ignored `vendor/SDL`; verify the sibling profile
prerequisites explicitly until AUTO-01 is implemented. For a clean-checkout
gate, invoke a known CART executable by absolute path from a detached clean
worktree; do not run `cart clean` in the primary checkout because `build/` also
contains the ignored CART executable.

Finite validation and smoke commands:

```sh
build/cart-sdl/linux/wolf3d-revive --data /absolute/path/to/WL6 --check-assets
build/cart-sdl/linux/wolf3d-revive --data /absolute/path/to/WL6 --check-ui
build/cart-sdl/linux/wolf3d-revive --data /absolute/path/to/WL6 --check-level

SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy \
  XDG_DATA_HOME="$PWD/build/validation/smoke-user" \
  build/cart-sdl/linux/wolf3d-revive \
  --data /absolute/path/to/WL6 --level --no-audio --frames 3
```

Existing full regression:

```sh
python3 -m unittest discover -s tests -v
WOLF3D_DATA=/absolute/path/to/WL6 python3 -m unittest discover -s tests -v
```

The first form is redistributable/synthetic coverage. The second enables the
private compatibility checks and must never stage the input data or generated
captures. In R0.1 the owned-data form ran 153 tests with no skips and exit 0.
The suite already exercises both Classic and Enhanced SDL paths, but it does
not replace the planned R0.4 capture/human-review gate or R0.5 sanitizer gate.

## Architecture inspection procedure

R0.1 and R0.2 both required the same source-boundary inventory. Until a
reviewed helper exists, use this deterministic, read-only procedure from the
repository root and save its output in the revision evidence directory:

```sh
mkdir -p build/validation/R0.2

{
  git rev-parse HEAD
  find src -type f \( -name '*.c' -o -name '*.h' \) -print | sort
  rg -n '^#include[[:space:]]+[<"]' src
  rg -n '#include[[:space:]]+<SDL3/' src
  ctags -x --kinds-C=v src/engine/*.c src/engine/*.h
  rg -n '\b(gamestate|player|tilemap|actorat|objlist|statobjlist|doorobjlist|spotvis|areabyplayer)\b' src
} > build/validation/R0.2/architecture-inspection.log
```

Replace `R0.2` with the selected revision. `ctags` must be available and its
implementation/version should be noted when output format matters. The scan
finds dependency and global-state candidates; it does not prove call direction
or ownership. Trace startup, shutdown, allocation, mutation and borrowed
pointer lifetime in the implementation before recording an architecture edge.
For Mermaid diagrams, review every arrow against that trace and check code
fences are balanced. Do not infer purity from a function name: R0.2 found
Classic rendering also mutates visibility/activation bookkeeping.

**Automation candidate AUTO-12:** add a small architecture inventory command
after R0.3 establishes test helpers. It should emit sorted direct includes,
SDL include violations outside the approved platform/main allow-list, and a
curated global-state reference inventory. It must not attempt to infer semantic
ownership. Add golden/failure tests and extend this canonical procedure rather
than creating revision-specific scan scripts.

Store repeatable revision evidence under ignored
`build/validation/<revision>/`. Record each command and exit separately; a
successful compile is not a test result. Until R0.6, callers must supply their
own bounded timeout and preserve complete stdout/stderr.

## Verified R0.4 graphics baseline

The synthetic Classic/Enhanced baseline is part of the existing render
unittest driver; no proprietary game data or external image package is needed:

```sh
python3 -m unittest discover -s tests -p test_render.py -v
```

The command compiles the established C render harness, constructs the existing
synthetic archives, generates all canonical indexed captures in a temporary
directory, verifies the archive/manifest capture ID set, checks every archived
capture's dimensions and SHA-256, and then compares candidate dimensions,
metadata, hashes and exact pixels with `tests/fixtures/visual_baselines.json`
and `tests/fixtures/visual_baselines.zip`. Candidate hash equality never skips
archive validation. It exits nonzero on a mismatch, including a missing archive
entry or corrupted archived pixels. Owned-data compatibility cases remain skips
when `WOLF3D_DATA` is absent; the synthetic baseline itself is required and
cannot skip.

To retain palette-correct BMPs and a run manifest for manual inspection:

```sh
WOLF3D_BASELINE_DIR="$PWD/build/validation/R0.4/captures" \
  python3 -m unittest discover -s tests -p test_render.py -v
```

This writes generated `.w3i`, `.bmp` and `manifest.json` files below the ignored
build directory. Review Classic corrected 4:3 at 640x480, letterboxed 1280x720
and non-integer 1001x601; then review Enhanced 4:3, 16:9, 16:10, 21:9, 32:9
and Auto. Check corridor depth and sprites, the three door and pushwall states,
viewport-edge clipping, HUD/weapon placement, black bars and restored display
images. Record the reviewer, platform/backend, date and each result separately;
capture generation is not human approval.

Baseline replacement is a review operation, not a normal validation step.
After an intentional renderer change, set `WOLF3D_UPDATE_BASELINES=1` only in a
reviewed working tree, inspect the resulting manifest/archive diff and retained
BMPs, and run the ordinary command again without that variable. Never update a
baseline merely to make an unexplained failure pass.

## Required script contract

- Compatible with the target toolchain; avoid unnecessary new dependencies.
- `--help` or documented usage, failure on invalid arguments and missing prerequisites.
- Clear `START`, `PASS`, `FAIL`, `SKIP/NOT RUN` lines and an unmistakable final summary.
- Non-zero exit when any required stage fails; never swallow underlying failures.
- Bounded execution with per-stage timeout and progress markers so a long sanitizer suite cannot silently hang.
- Separate fast CPU tests from optional/required graphics, platform and longer-running jobs.
- Preserve stdout/stderr logs and metadata under `build/validation/<revision>/` (or documented equivalent).
- No network installation, hidden environment mutation or destructive cleanup by default.
- Prefer idempotent operations; if not idempotent, document backups, output overwrite behaviour and recovery.
- Include tests for the script's failure path, timeout, argument parsing and exit propagation.

## Evidence and state handoff

A revision report should contain:

```text
Revision: Rn.m
Status: PASS | FAIL | PARTIAL | NOT RUN
Branch / commit:
Host / OS / compiler / SDL:
Changes:
Commands executed (real, not proposed):
Outputs / exit codes / log paths:
Acceptance matrix:
Performance / captured image evidence:
Open defects and exact reproduction:
Next action and next command:
Reusable procedure added or updated:
```

A failed gate remains failed even if another agent is expected to fix it. Reference logs and PR numbers so the next agent can resume without starting over.

## Revision process template

1. Read `AGENTS.md`, `docs/plan.md` and this playbook.
2. Select the next unclosed Rn.m contract and confirm prerequisites.
3. Check existing scripts before introducing a tool.
4. Make a narrowly scoped change on a branch.
5. Run focused validation then mandated regression; log real results.
6. Improve the shared command/playbook if this work is or will be repeated.
7. Attach acceptance evidence; request review.
8. Resolve review findings and rerun necessary gates before merge.
9. Record next step and current blockers for agent handoff.

## Safety and scope of automation

CI can run checks and post summaries once approved. Automatic fixes, PR merges, deletion of branches, uploads or publishing are distinct opt-in privileges and must not be implied by this plan. Never store credentials, tokens, private artifacts or protected game assets in scripts/logs. Human visual reviews, fresh-player usability, legal licence decisions and release sign-off remain manual gates even if supported by automated evidence.

## Reference

Ironclaw3D `tools/test.sh` and M3/M10 validation reports demonstrate the discipline: actual commands, explicit failures, reproducible captures, metrics and open acceptance gates. Adapt to Wolf3D's existing SDL renderer and fixed-tick semantics rather than copying Ironclaw-specific code.
