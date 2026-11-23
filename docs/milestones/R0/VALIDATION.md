# R0 consolidated foundation validation

**Milestone:** R0 baseline and test foundation

**Audited source:** `b29000068f81ff152c8c6a47db2668a5a0308506` on
`docs/revival-roadmap-and-validation`

**Validation date:** 23 November 2026 (Australia/Adelaide)

**Overall:** **PASS — R0 milestone CLOSED for the documented Linux synthetic scope.**
A01-A22 pass within their stated evidence scopes. An independent ChatGPT
reviewer has explicitly approved the architecture/test review (A22). R0
milestone is closed for the documented Linux synthetic scope.

## Evidence sources and environment

The current review inspected `docs/plan.md`, `docs/milestones/R0/PLAN.md`, every
R0.1-R0.6 report, current production/test/tooling source, the validation
strategy and automation playbook. Earlier evidence is not treated as current
execution unless identified below.

| Item | Current review value |
| --- | --- |
| OS / kernel | Debian GNU/Linux 13; Linux `7.3.4+deb13-amd64`; x86-64 |
| Compiler | GCC `14.2.0` (`Debian 14.2.0-19`) |
| Python | `3.13.5` |
| SDL | local static SDK/headers `3.5.0` |
| CART / language | CART `0.2.0`; C11; `cart-sdl` profile |
| Graphics backend | `SDL_VIDEODRIVER=dummy`, software renderer, dummy audio |
| Current source | `b29000068f81ff152c8c6a47db2668a5a0308506` |
| CI | GitHub Actions run `45021864807`, success at the same commit; Ubuntu runner, SDL `release-3.2.8` |

Generated evidence is intentionally ignored by Git:

- `build/validation/ALL/20261123T054239Z/summary.json` and per-job logs;
- `build/validation/R0.5/perf-summary.txt` and `logs/perf-*.csv`;
- `build/validation/R0.7/full-regression.log`;
- `build/validation/R0.7/captures/manifest.json`, 32 `.w3i` files and 32 BMPs;
- `build/validation/R0.7/architecture-inspection.log` (1,511 lines); and
- `build/validation/R0.7/review/` contact sheets used for the current manual
  synthetic-capture inspection.

## Commands actually executed

| Command | Exit | Result / evidence |
| --- | ---: | --- |
| `gh run view 45021864807 --repo raygibbon/wolf3d-revive --json ...` | 0 | Run conclusion `success`, head `b29000068...`; job `137206339344` and all required build/validation steps succeeded. |
| `python3 tools/validation/run.py all --list` | 0 | Listed 21 mandatory R0.3-R0.6 jobs with categories and timeouts. |
| `python3 tools/validation/run.py all` | 0 | 21 PASS, 0 FAIL/TIMEOUT/NOT RUN; 445.660 s. Summary: `build/validation/ALL/20261123T054239Z/summary.json`. |
| `python3 -m unittest discover -s tests -v` | 0 | Logged rerun: 175 tests in 155.082 s; 154 passed, 21 skipped, 0 failures/errors. Skips require owned WL6/DOS inputs or unavailable installations. Log: `build/validation/R0.7/full-regression.log`. |
| `WOLF3D_BASELINE_DIR="$PWD/build/validation/R0.7/captures" python3 -m unittest discover -s tests -p test_render.py -v` | 0 | 9 tests in 10.484 s: 6 passed, 3 owned-data skips; generated and validated 32 synthetic captures. |
| `./build/tools/cart build --profile cart-sdl --verbose` | 0 | C11 application linked successfully with GCC 14.2.0 and SDL 3.5.0; no compiler warning was emitted. |
| documented architecture inspection block from `PLAYBOOK.md` with destination changed to `R0.7` | 0 | 1,511-line sorted source/include/global inventory. SDL includes remained confined to `main.c` and `src/platform/`. |
| sanitizer signature scan over `r05-san-*.log` | 0 | No ASan, UBSan, LSan, `runtime error:` or sanitizer `SUMMARY:` match. |
| `git diff --check` | 0 | No whitespace error before report creation; rerun after edits is recorded in the final handoff. |

The R0.3-R0.6 runner executed 237 unittest cases across normal and sanitizer
jobs: 205 executed passes and 32 explicit skips. These are test *executions*,
not 237 unique tests. The performance job is additional and is not counted as a
unittest. Per-job counts:

| Job | Status | Tests | Skips | Seconds |
| --- | --- | ---: | ---: | ---: |
| `r03-engine-synthetic` | PASS | 41 | 1 | 59.3 |
| `r04-render` | PASS | 9 | 3 | 10.9 |
| `r04-display` | PASS | 5 | 2 | 14.4 |
| `r04-presentation` | PASS | 3 | 1 | 7.1 |
| `r04-classic` | PASS | 12 | 4 | 18.6 |
| `r05-san-engine` | PASS | 41 | 1 | 100.4 |
| `r05-san-render` | PASS | 9 | 3 | 19.7 |
| `r05-san-classic` | PASS | 12 | 4 | 23.7 |
| `r05-san-display` | PASS | 5 | 2 | 20.8 |
| `r05-san-presentation` | PASS | 3 | 1 | 13.7 |
| `r05-san-save` | PASS | 13 | 0 | 31.9 |
| `r05-san-config` | PASS | 29 | 1 | 11.1 |
| `r05-san-controls` | PASS | 3 | 1 | 8.2 |
| `r05-san-loading` | PASS | 2 | 1 | 29.9 |
| `r05-san-audio` | PASS | 6 | 1 | 13.8 |
| `r05-san-signon` | PASS | 16 | 2 | 9.8 |
| `r05-san-dos-menus` | PASS | 3 | 1 | 8.3 |
| `r05-san-dos-ui-reference` | PASS | 1 | 1 | 0.3 |
| `r05-san-demo-sync` | PASS | 5 | 2 | 7.2 |
| `r05-perf-synthetic` | PASS | n/a | n/a | 26.9 |
| `r06-runner-tests` | PASS | 19 | 0 | 9.7 |

## Acceptance matrix A01-A22

Status applies to the criterion as written, not to unrelated future release
work. Platform and licensing exclusions are recorded separately below.

| ID | Status | Current review evidence |
| --- | --- | --- |
| **A01** source/module and asset inventory complete with provenance status | **PASS** | R0.1 sections 2-8 enumerate active, historical, third-party, external and unverified material. Current `rg --files`, source and licence review found no basis to contradict it; legal clearance remains R1 work. |
| **A02** debug/release clean-checkout builds run and warnings recorded | **PASS** | R0.1 clean Linux optimized and strict debug builds and strict MinGW cross-build exited 0 with warnings recorded. Current CART profile build also exited 0 with no warning. The GitHub clean-checkout application build passed at the audited commit. |
| **A03** existing game smoke path documented | **PASS** | R0.1 records finite asset/UI/level and SDL smoke commands; the playbook retains them. Owned-data smoke was not rerun because no lawful data path was supplied. |
| **A04** no unverified licensing/test claims | **PASS** | Reports and this matrix distinguish repository licence statements, legal clearance, synthetic versus owned data, automation versus human review and unsupported platforms. |
| **A05** dependency map matches inspected source | **PASS** | Current architecture inventory reproduced 1,511 lines; inspected call/lifecycle edges match R0.2. SDL use is restricted to process/platform code. |
| **A06** coupling and extraction points concrete | **PASS** | R0.2 identifies global world/session coupling, catalog/image, authored-world, session-builder, transaction and file-adapter seams. Current code still exhibits those exact constraints. |
| **A07** Classic/Enhanced compatibility risks listed | **PASS** | R0.2 covers sampling, opacity/pivots, Classic render mutation, snapshots, aspect, stable IDs and teardown; R0.7 carries these into R2 prerequisites. |
| **A08** fixed inputs match across presentation rates | **PASS** | Current R0.3 engine job passed 41 tests with one owned-data skip. The full regression passed the 60/75/90/120/144/165/180/240 FPS presentation cases with 140 authoritative ticks each. |
| **A09** deterministic fixtures and diagnostics committed | **PASS** | Synthetic fixtures, explicit state comparisons and source/`CA_Error()` diagnostics are present and passed current execution. |
| **A10** no timing change introduced | **PASS** | R0.3 made no production change; current deterministic/full regressions pass. `main.c` and `GameStep` retain 70 Hz authority and bounded batches. |
| **A11** captures and metadata reproducible | **PASS** | Current retained-capture run generated 32 entries and validated exact IDs, dimensions, hashes, metadata, archived pixels and restored-state equality. |
| **A12** required modes visually inspected | **PASS** | Project owner approved the synthetic captures on 22 November 2026. On 23 November the R0.7 reviewer inspected regenerated Classic 4:3/letterbox/noninteger, Enhanced 4:3 through 32:9/Auto, door, pushwall and edge states with no obvious defect. Physical backends remain NOT RUN. |
| **A13** comparisons separate regression from platform variance | **PASS** | Exact canonical indexed/software comparisons reject pixel, archive, dimension and metadata corruption. Accelerated/high-DPI/physical outputs are explicitly manual/platform gates, not hidden by tolerance. |
| **A14** sanitizer results include tools and exits | **PASS** | Current 14-job ASan/UBSan matrix used GCC 14.2.0/SDL 3.5.0 with bounded runner timeouts; every job exited 0 and its log is named in the current JSON summary. |
| **A15** application-owned findings resolved or owned | **PASS** | No sanitizer signature was found in current instrumented-harness logs. Unsanitized prebuilt application subprocesses remain outside this bounded claim. |
| **A16** benchmark raw logs stored without invented FPS gate | **PASS** | Current CPU and SDL dummy/software CSV/log/summary files exist under `build/validation/R0.5/`; measured distributions are below. No target FPS is asserted. |
| **A17** clean checkout runs documented mandatory suites | **PASS** | GitHub Actions run 45021864807 built the app and passed R0.6 at the audited commit without proprietary assets. Local R0.6 jobs also passed within `all`. |
| **A18** revision selection and required failure propagation | **PASS** | Current runner tests (19/19) passed; list mapping is explicit and tests prove nonzero/unknown/failing mandatory behavior. |
| **A19** hangs/unsupported jobs surfaced truthfully | **PASS** | Runner tests passed timeout, TERM-ignoring child, orphan, zombie, reused-PID and missing-prerequisite cases. Non-Linux `/proc` behavior remains NOT RUN, not promoted. |
| **A20** all critical criteria pass for tested targets | **PASS** | A01-A19 pass for the recorded Linux synthetic scope; skipped owned-data and unsupported targets are outside those pass claims and separately visible. |
| **A21** out-of-scope platforms recorded separately | **PASS** | See the platform matrix below. Wine/cross-build evidence is not physical Windows approval. |
| **A22** architecture/test review explicitly approved before R1/R2 | **PASS** | An independent ChatGPT reviewer has approved the architecture and testing baseline for the documented Linux synthetic scope. |

Summary: **22 PASS, 0 FAIL, 0 PARTIAL, 0 NOT RUN** at the criterion level.
The PARTIAL is the final approval gate, not a hidden test failure.

## Graphics evidence

The current manifest contains 32 exact indexed captures: 15 Classic and 17
Enhanced. It covers the synthetic corridor/static scene, closed/half/open door,
initial/moving/tile-boundary pushwall, edge clipping and initial/changed/restored
display states.

Manual R0.7 observations:

- Classic 640x480 is corrected 4:3; 1280x720 is centered with equal black side
  bars; 1001x601 remains centered without visible crop/stretch.
- Enhanced 4:3, 16:9, 16:10, 21:9, 32:9 and Auto preserve vertical framing and
  reveal additional horizontal geometry. HUD/weapon placement remains centered.
- Door and pushwall sequences progress consistently in Classic and Enhanced;
  edge content stays inside the viewport/HUD boundary.
- Initial/restored hashes are machine-checked equal by the test suite.

This is synthetic content under SDL dummy/software and canonical indexed
captures. It is not evidence for physical fullscreen, accelerated GPU output,
high-DPI behavior or every platform.

## Performance evidence

The performance run is a reproducible baseline, not a pass/fail FPS target.
CPU rows use 200 measured frames; SDL rows use 120 measured frames after warmup.
Times are milliseconds and RSS is Linux `ru_maxrss` KiB.

CPU renderer-only:

| Scene | Resolution | Mean | P50 | P95 | Max | RSS KiB |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| empty | 320x200 | 0.232 | 0.229 | 0.297 | 0.366 | 19,228 |
| empty | 640x480 | 0.672 | 0.670 | 0.684 | 0.714 | 19,228 |
| empty | 1280x720 | 1.816 | 1.812 | 1.840 | 1.887 | 19,228 |
| empty | 1920x1080 | 4.034 | 3.999 | 4.164 | 5.013 | 19,228 |
| populated | 320x200 | 0.168 | 0.167 | 0.172 | 0.191 | 19,228 |
| populated | 640x480 | 0.678 | 0.675 | 0.689 | 0.831 | 19,228 |
| populated | 1280x720 | 1.817 | 1.816 | 1.835 | 1.915 | 19,228 |
| populated | 1920x1080 | 3.977 | 3.961 | 4.050 | 4.590 | 19,228 |

SDL populated `frame_total` (`GameDrawPaused()` plus `VW_UpdateScreen()`), dummy
video/software renderer:

| Resolution | Mean | P50 | P95 | Max | RSS KiB |
| --- | ---: | ---: | ---: | ---: | ---: |
| 640x480 | 6.170 | 6.187 | 6.273 | 6.349 | 19,228 |
| 1280x720 | 3.588 | 3.583 | 3.685 | 3.736 | 20,260 |
| 1920x1080 | 12.199 | 11.839 | 14.222 | 16.016 | 31,296 |

The lower dummy/software total at 1280x720 than 640x480 reflects backend
behavior in this one run and must not be generalized. End-of-run
`MM_TotalBytes()` was `0`. Full convert/upload/present phase distributions and
raw values remain in the CSVs.

## Platform, manual and licensing gates

| Gate | State | Evidence boundary |
| --- | --- | --- |
| Linux x86-64 synthetic CPU/SDL dummy/software | **PASS** | Current local runner, full regression, build, captures, sanitizer and performance evidence. |
| GitHub Ubuntu clean-checkout synthetic R0.6 | **PASS** | Actions run 45021864807 at `b29000068...`. |
| MinGW cross-build and Wine level check | **PASS (limited)** | R0.1 evidence only; proves compilation and a Wine CLI check, not physical Windows behavior. |
| Physical Windows | **NOT RUN** | No device video/audio/input/DPI/filesystem evidence. |
| macOS / Android / other POSIX | **NOT RUN** | No current build/runtime evidence; non-Linux runner process inspection is also unverified. |
| Accelerated GPU, physical fullscreen, high-DPI, multiple monitors | **NOT RUN** | Synthetic indexed and SDL dummy/software evidence only. |
| Owned WL6 regression in R0.7 | **NOT RUN** | No owned-data path supplied. Twenty-one full-suite cases skipped; R0.1 remains the latest reported 153-test no-skip run. |
| Human synthetic visual review | **PASS** | Owner approval from R0.4 plus current reviewer inspection described above. |
| Independent final milestone approval | **PARTIAL** | Required by A22 and still pending. |
| Legal/release clearance | **NOT RUN / R1 scope** | Source licence statements and provenance statuses exist, but historical/proprietary/external material is not declared cleared. |

## Architecture conclusion and R1/R2 prerequisites

The baseline is adequate for incremental work: platform SDL ownership is
isolated, legacy parsing is staged and bounded, resource teardown is explicit,
Enhanced rendering has immutable snapshot/target seams, Classic/Enhanced
compatibility is captured, and extensive headless tests exist.

It is not yet a data-first game engine. The current map is a fixed 64x64 WL6
cache transformed into global runtime arrays; resources are fixed numeric WL6
pages/chunks; Classic rendering still has gameplay-visible side effects; and no
authored-world, package, catalog, manifest or Workshop transaction API exists.

Before implementation broadens:

1. R1 must complete provenance/identity classification and enforce original-only
   packages without silent legacy fallback.
2. R2 must define versioned manifest/map/resource/path contracts, limits and
   fixtures before implementing loose-directory or PK3-compatible ZIP input.
3. One SDL-free owned catalog/world model must serve gameplay, both renderers,
   Workshop and external tools through native C APIs with explicit lifetimes.
4. Package validation must reject traversal, ambiguous names, duplicates,
   symlink escapes, bombs/oversize content and incompatible versions before
   publishing state.
5. PK3 container support must not be described as ECWolf/GZDoom mod
   compatibility. Initial selection/override rules should remain intentionally
   small and deterministic.
6. Data-defined weapons, enemies, campaigns/objectives/progression, menu pack
   selection, Workshop create/save/playtest/export and compatibility adapters
   remain ordered future revisions, not one loader change.

The detailed proposed contract and sequence are in [`R0.7.md`](R0.7.md).

## Unresolved issues and disposition

| Issue | Impact | Required next action |
| --- | --- | --- |
| Physical/platform validation gaps | Platform-specific defects may remain. | Record future target-specific runs; never convert absence into PASS. |
| Owned-data skips | Current review does not refresh proprietary-data compatibility. | Optional lawful private rerun using `WOLF3D_DATA`; keep data/captures ignored. |
| Sanitizer application gap | Prebuilt app subprocesses in some wrappers may be unsanitized. | Preserve bounded claim or approve a sanitizer-built app workflow later. |
| Legal clearance open | Release packaging cannot be declared clean. | Execute R1 provenance/licence and staged-package work after approval. |
| Content/world ownership absent | General packages and Workshop cannot safely rely on runtime globals. | Implement the approved R2/R4 owners and adapters incrementally after R0 closes. |

## Final disposition

- **Automated tested Linux scope:** PASS.
- **Synthetic graphics review:** PASS; physical/backends NOT RUN.
- **Sanitizer tested harness scope:** PASS; app-subprocess caveat retained.
- **Performance baseline:** PASS as evidence, with no target gate.
- **Unsupported platforms:** NOT RUN and non-blocking for this Linux baseline.
- **Licensing/release clearance:** open R1 work, not misreported as R0 PASS.
- **R0 milestone:** CLOSED for the documented Linux synthetic scope.

Recommendation: approve R0 after reviewing the two R0.7 reports and explicitly
recording A22. Do not merge PR #18 automatically and do not begin R1/R2
implementation solely from this recommendation.
