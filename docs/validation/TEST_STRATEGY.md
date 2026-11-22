# Validation suite specification

Status: PROPOSAL. Test tooling and commands below are targets, **not claims of existing implementation**.

## Design goals

Follow the evidence discipline of Ironclaw3D: independently runnable CPU tests, real SDL graphics/application tests, sanitizer runs, deterministic replay, malformed resource fixtures, matched captures, measured resource use and a gate-by-gate acceptance report. Prefer small bounded suites that show progress and fail clearly over a monolithic silent test script.

## Layers

| Layer | Planned coverage | Example fixtures |
| --- | --- | --- |
| Build/static | C11 debug/release, warnings, clean tree, ownership boundaries | build output, include scan |
| CPU unit | raycasting, collision, map/tile bounds, player and actor ticks | known ray grid, corner walls |
| Determinism | same recorded input at varied presentation FPS and pause/stall | 30/60/144/240 render rates, fixed tick trace |
| Resource/data | versioned maps, manifests, missing/corrupt/oversize files | v1/v2-compatible, malformed NUL/paths |
| World authoring | set/remove tile, door/object placement, snap, rollback, undo/redo | small rectangle, overlap and invalid coordinate |
| Persistence | atomic save, reload equality, interruption/corruption recovery | round-trip golden map |
| Workshop application | cursor/preview, edit, save, playtest, return, mode transitions | scripted input capture + manual check |
| Graphics | Classic 4:3 and Enhanced widescreen, sprites/HUD/clipping, resize | frozen camera scenes and captures |
| Security | path traversal/symlinks, bounded sizes, invalid UTF-8/IDs where relevant, archive bombs | adversarial manifests |
| Lifecycle | create/destroy, repeated restarts, missing assets, allocation failures | 100 restarts, fault injection |
| Sanitizers | ASan, UBSan, supported leak checking | full and focused test subsets |
| Performance | CPU ticks, frame time p50/p95, peak allocations/resources | representative large/small scenes |
| Release | original-content-only launch, platform smoke, licence manifest | staged release with legacy content absent |

## Proposed command interface (R0.6 to implement)

```sh
tools/test.sh                    # fast CPU and content regression
tools/test.sh --graphics         # real renderer/application checks
SANITIZE=1 tools/test.sh         # CPU sanitizer checks
SANITIZE=1 tools/test.sh --graphics
python3 tools/validation/run.py R0.4
python3 tools/validation/run.py R6.6
```

Never present these as working commands until the scripts exist and pass. Use fixed test names, per-case START/PASS/FAIL messages, process exit status, timeout handling and logs within `build/validation/`. A broken/hanging stage must not prevent an accurate failure report.

## Mandatory Workshop golden scenario

1. Create a new map through the native C authoring API.
2. Place several snapped walls and a door; reject an invalid placement without partial mutation.
3. Save with validation and atomic replacement.
4. Destroy/unload the world; reload and compare semantic state.
5. Enter playtest in actual game mode; verify collision, door and player start.
6. Return to Workshop without losing edits, change one tile and undo/redo.
7. Save again and verify through a fresh process/load.

Include failure cases for invalid coordinate, protected player start, occupied tiles, missing textures, truncated file, corrupt version, disk write failure, unsafe paths and restart after error. Support no-window headless operation for the core steps. UI-only capture is supplemental.

## Reporting and reproducibility

Each revision validation report includes commit SHA, host/OS/compiler/SDL, command and arguments, exit code, focused suites, mandatory regressions, captured image paths, raw benchmark data, known limitations and an Axx pass matrix. Separate "not run" from "pass", and human playtest from automated simulation. Do not hide toolchain/driver leaks, platform omissions or partial failures.

## Stage gates

- R0 baseline: clean builds, deterministic CPU harness, graphics baselines, sanitizer evidence.
- R1/R3 original content: source and asset provenance plus staged package smoke with legacy game data intentionally absent.
- R2 content: valid/malformed fixtures, format compatibility and memory/resource bounds.
- R4/R5 authoring: API headless tests, collision/visibility regression, screenshot inspection and operator input tests.
- R6 persistence: complete golden scenario, fault injection and save/reload durability.
- R7 usability: new-player observation separately documented and acceptance criteria evaluated.
- R8 release: target platforms tested, clean install, reproducible packages and complete licence sign-off.

No numerical performance budget is asserted without first collecting R0 baseline and specifying representative minimum hardware.
