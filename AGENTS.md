# AGENTS.md — Wolf3D Revive agent working agreement

Applies to Codex, Gemma/OpenCode and any automation working in this repository. Read this file **before changing code**. This is the project's durable, vendor-neutral agent entry point, not a claim that any agent automatically remembers previous chats.

## Source of truth and work order

1. Read `docs/plan.md`, then the selected `docs/milestones/Rn/PLAN.md` and its revision contract (when created).
2. Read `docs/validation/TEST_STRATEGY.md` and `docs/automation/PLAYBOOK.md` to discover existing workflows.
3. Inspect actual source/config/tests for the selected change, then use recorded commands; do not assume planned scripts already exist.
4. Work on one approved revision; document a scope change before expanding it.
5. Execute focused tests, required regression, and relevant safety checks. Produce evidence, not assertions.
6. Update the relevant validation report and playbook if workflow knowledge changed; report unfinished checks honestly.

## Engineering boundaries

- C11 + SDL3 + existing CART configuration; preserve Classic 4:3 and Enhanced widescreen unless the revision explicitly changes them.
- No large rewrite or new dependency to solve a local defect; prefer small modules with justified ownership.
- Original-game work: respect code licences and document asset provenance; do not copy or package uncleared Wolfenstein material.
- Workshop: same map representation and native C world operations as gameplay, headless testable; do not make authoring functionality depend on UI callbacks.
- Deterministic simulation, bounded memory and graceful handling of malformed files are default requirements.
- Keep generated binaries, logs, screenshots and temporary artefacts out of source control unless a review explicitly requires golden fixtures.

## Stop agents repeating work

**Search before solving.** Check `docs/automation/PLAYBOOK.md`, `tools/`, `tests/`, past revision validation and existing commands before creating a helper or troubleshooting a recurring failure.

**Capture a recurring task.** When you see the same multi-step sequence a second time, or it is clearly going to repeat across revisions/agents, add a reusable, reviewed command/script or a deterministic documented procedure as part of the change (or open a separately scoped task if implementation is not approved). Examples: clean build setup, smoke runs, screenshot capture, validation, test fixture generation, PR review evidence and reproducible failure diagnosis.

**Automate only when safe.** Scripts must be local, deterministic where feasible, idempotent for setup, non-destructive by default, with explicit arguments, prerequisite checks, clear progress, bounded runtime, exit status and an output/log path. No silently downloading dependencies, disabling sanitizers, force-pushing, deleting branches, merging PRs, removing user files, or silently changing milestone status. Operations that mutate remote state require explicit approval.

**Avoid duplicate helper scripts.** Extend the canonical tool instead of creating `test2.sh`, `validate-new.py`, etc. Add a targeted test for new tooling; record how to invoke it in the playbook.

**Make the next agent independent.** End each revision with: current state, commit/branch, modified files, tests run and exit status, outstanding blockers, exact next command and updated references. Do not rely on chat history as the only record.

## Truthful gate states

For every required acceptance criterion use `PASS`, `FAIL`, `PARTIAL`, or `NOT RUN`. A compile is not a test pass. A skipped graphics, sanitizer or human usability check is not a pass. Log a hung stage and reason. Do not claim a milestone is completed until its explicit gates and review are satisfied.

## Repository entry points

- `docs/plan.md`: roadmap and revision ordering
- `docs/milestones/R0/PLAN.md`: first implementation and baseline acceptance contract
- `docs/validation/TEST_STRATEGY.md`: planned test layers and evidence
- `docs/automation/PLAYBOOK.md`: reusable procedures, script conventions and automation backlog
- `docs/ORIGINAL_CONTENT_AND_IP.md`: copyright/asset separation
- `docs/WORKSHOP_DESIGN.md`: creator experience and canonical world API

**Current phase:** roadmap proposal. Do not claim tools such as `tools/test.sh` or `tools/validation/run.py` exist until R0 implements and verifies them.
