# R0: baseline and test-foundation milestone

Status: PROPOSED / NOT RUN. Goal: understand and protect existing Wolf3D Revive behaviour before changing game identity, content format or world editing.

## R0.1 Inventory and clean build

Tasks: inventory source tree, branches, external dependencies, content/assets, licences, map inputs, save formats, existing tests and packaging. Document actual CART SDK requirements and clean Linux/Windows build instructions. Find all distributed Wolfenstein-specific assets and references, without deleting anything.

Acceptance: A01 source/module and asset inventory is complete with provenance status; A02 debug/release clean-checkout builds run and warnings recorded; A03 existing game smoke path documented; A04 no unverified claims about licensing or test coverage.

## R0.2 Ownership/architecture

Tasks: diagram current engine/platform/game/content responsibilities, state ownership, renderer/frame clock, map loading, actor logic, input and asset lifetime. Identify the smallest stable native C world operations needed later by Workshop; specify, do not implement, new APIs.

Acceptance: A05 dependency map matches inspected source; A06 coupling and proposed extraction points are concrete; A07 Classic/Enhanced compatibility risks are listed.

## R0.3 Deterministic headless CPU harness

Tasks: add test runner with focused suites for tile queries, ray intersection, collision, fixed-step or existing 70-Hz timing (verify actual rules), actors, map bounds and malformed input. Headless tests must not require SDL window creation.

Acceptance: A08 fixed inputs yield matching final state across multiple rendering rates; A09 deterministic fixtures and failure diagnostics committed; A10 no gameplay timing changes introduced as a side effect.

## R0.4 Matched render baselines

Tasks: script repeatable scene/camera/input captures for Classic 4:3 and Enhanced widescreen with resolution, aspect, software scaling, resize/fullscreen, HUD and door/sprite clipping cases. Compare new captures to locked baseline with a documented tolerance policy. Do not assume pixel identity where rendering platforms differ.

Acceptance: A11 baseline images and capture metadata reproducible; A12 all required modes visually inspected; A13 automated comparisons distinguish expected platform variance from regressions.

## R0.5 Sanitizer + profiling baseline

Tasks: run native AddressSanitizer/UndefinedBehaviorSanitizer where toolchain permits; leak checks, lifecycle/restarts, resource error paths. Measure frame time and allocations in representative gameplay and empty scenes, with fixed host/build/mode parameters.

Acceptance: A14 results include toolchain and exit codes; A15 all application-owned sanitizer findings resolved or blocked with explicit owner; A16 baseline benchmark raw logs stored, no invented target FPS result.

## R0.6 CI and validation command surface

Tasks: stable commands like `tools/test.sh` and `python3 tools/validation/run.py R0.4` only **after implementing them**; separate CPU, graphics, sanitizer and performance jobs with explicit timeouts and progress logging. Write actionable failure diagnostics and preserve output under ignored build artifacts.

Acceptance: A17 a clean checkout can run documented automated suites; A18 requested revision selects the correct jobs and fails non-zero when required jobs fail; A19 hanging/unsupported jobs are surfaced as failed or unrun, never fake pass.

## R0.7 Review and baseline sign-off

Tasks: publish R0 validation report covering criteria A01–A19, source revisions, environment, exact commands, pass/fail/partial/not-run matrix, graphics captures, performance results, open issues, owner and next action.

Acceptance: A20 all critical criteria pass for tested targets; A21 out-of-scope platforms recorded separately; A22 architecture/test review explicitly approved before R1/R2 implementation.

## Evidence

Suggested generated: `build/validation/R0.x/*`, command logs, JSON test summaries, screenshots, frame-time CSV. Committed report: `docs/milestones/R0/VALIDATION.md` after actual runs. This PLAN is **not** itself an execution report.

## Review discipline

No major engine restructuring, IP deletion, content schema migration or Workshop UI in R0. Preserve current playable functionality, Classic/Enhanced presentation and reproducible licensing provenance. Milestone is OPEN until evidenced checks pass.
