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
| AUTO-01 | Build/smoke environment checks | `tools/doctor.sh` | R0.1/R0.6 | NOT IMPLEMENTED |
| AUTO-02 | Fast headless/unit regressions | `tools/test.sh` | R0.3/R0.6 | NOT IMPLEMENTED |
| AUTO-03 | SDL graphics/display regression | `tools/test.sh --graphics` | R0.4/R0.6 | NOT IMPLEMENTED |
| AUTO-04 | ASan/UBSan test matrix | `SANITIZE=1 tools/test.sh` | R0.5/R0.6 | NOT IMPLEMENTED |
| AUTO-05 | Revision-based gating and progress/logs | `python3 tools/validation/run.py R0.4` | R0.6 | NOT IMPLEMENTED |
| AUTO-06 | Deterministic capture comparison | `tools/capture_baseline.sh` (interface subject to design) | R0.4 | NOT IMPLEMENTED |
| AUTO-07 | Content provenance and banned-asset scanning | `tools/audit_content.py` (interface subject to design) | R1 | NOT IMPLEMENTED |
| AUTO-08 | Map/content fixture validation | `tools/validate_content.py` (interface subject to design) | R2 | NOT IMPLEMENTED |
| AUTO-09 | Workshop round-trip integration | revision-selected test fixture | R6.6 | NOT IMPLEMENTED |
| AUTO-10 | PR evidence/checklist creation | CI/template based on real test metadata | R0.6 | NOT IMPLEMENTED |

The table is a **backlog**; no script is claimed to exist. Replace speculative names with tested stable commands when each revision delivers.

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
