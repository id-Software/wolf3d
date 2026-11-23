# Wolf3D Revive: original game and Workshop roadmap

Status: **PROPOSAL, not implementation approval**. Version 0.1, 22 November 2026.

This roadmap adopts Ironclaw3D's milestone -> numbered revision -> acceptance tests -> evidence report -> code review -> merge workflow. The two product workstreams are (A) original IP/content and an independently playable original game, and (B) a first-person Workshop. They share one world representation, native C world-edit operations and content loader. Keep Wolf3D Revive deliberately simpler than Ironclaw3D.

## Non-negotiable decisions

- Preserve the existing playable engine and Classic 4:3 / Enhanced widescreen behaviour until explicitly superseded and regression-tested.
- C11 and SDL3 remain the baseline; use existing CART configuration and rendering approach. No speculative engine rewrite, Vulkan requirement, new modelling system or editor-only game logic.
- Original assets, branding, audio, names, story and distributable maps are mandatory for the new standalone game. Source licensing is a separate question; replacing assets does NOT remove GPL or other upstream obligations. Audit upstream and all redistributable material before claiming release readiness.
- Original game and Workshop use the same versioned map/content model and game-world API. Core authoring operations must work headlessly without SDL UI.
- Workshop is first-person, walk-around building, not a CAD or top-down editor; grid snap, placement preview, bulk wall/room tools and immediate playtest. No free-flight requirement in v1.
- Native C interfaces are the canonical operations. Add scripting only with a future separately approved need.
- A revision or milestone is not 'done' because it compiles or because most tests pass. Record failed/untested gates explicitly. Do not silently weaken gates.
- Scope changes or new format revisions require an explicit decision record and migration tests.

## Milestones and ordered revisions

Each row is proposed and may be refined by R0 evidence. Each milestone closes only after all its revision reports and the milestone validation report are approved.

| Milestone | Objective | Proposed revisions | Exit gate |
| --- | --- | --- | --- |
| R0 Baseline | Establish reliable baseline and architecture inventory | R0.1 source/build/asset inventory; R0.2 ownership and dependency map; R0.3 deterministic headless CPU harness; R0.4 matched display captures; R0.5 sanitizers/performance baseline; R0.6 CI/check runner; R0.7 baseline acceptance | Repeatable native tests, captured visual baseline, unresolved issues recorded |
| R1 IP separation | Classify and isolate Wolfenstein-specific content | R1.1 licence/provenance inventory; R1.2 symbols/branding/text/audio audit; R1.3 engine vs game boundary; R1.4 legacy-content isolation; R1.5 distributable-asset manifest | Original mode can be developed without pulling restricted/uncleared assets into packages; licences documented |
| R2 Data/content | Shared data-first map and resources | R2.1 versioned map contract; R2.2 validated loader and clear errors; R2.3 named resources and content manifest; R2.4 safe package/loose paths; R2.5 compatibility/migration fixtures; R2.6 headless content validation | Valid/malformed/old content reliably handled; round-trip and bounded resource tests |
| R3 Original game | Deliver independently playable original content | R3.1 identity/concept and art/audio specifications; R3.2 original textures/HUD/menus; R3.3 original characters/weapons/sounds; R3.4 gameplay/data bindings; R3.5 playable campaign slice; R3.6 no-legacy-assets smoke and licence review | Playable original level works from a clean package without proprietary Wolfenstein data |
| R4 World authoring API | Build editor-capable world operations | R4.1 ownership and edit transactions; R4.2 validated tile/door/object mutations; R4.3 placement/snapping policy; R4.4 undo/redo contract; R4.5 headless authoring regression | No UI-only creation capability; gameplay and Workshop share operations |
| R5 First-person Workshop | Minimal walk-around construction | R5.1 mode entry/exit; R5.2 first-person selector and preview; R5.3 place/delete walls; R5.4 drag/extend walls and rectangular room tool; R5.5 doors/props/materials; R5.6 input/display usability | Create a small traversable level without editing files or flying |
| R6 Persistence/playtest | Author/play/return loop | R6.1 map save/load and atomic writes; R6.2 validation and recovery; R6.3 enter playtest from Workshop; R6.4 return with edits preserved; R6.5 resource hot-load safety; R6.6 complete round-trip test | Build -> save -> unload -> load -> play -> return -> edit succeeds repeatably |
| R7 Authoring completeness | Usable level creation workflow | R7.1 player starts and exits; R7.2 enemies/items/secrets; R7.3 selection, delete, copy and undo UI; R7.4 map management; R7.5 accessibility/onboarding; R7.6 user testing | New player can produce and play a valid level with documented workflow |
| R8 Release | Original game and creator distribution | R8.1 pack/export/import; R8.2 content security and compatibility; R8.3 Windows/Linux verification; R8.4 performance and memory limits; R8.5 licensing/credits; R8.6 release acceptance | Reproducible distributable original game + Workshop, clean licence and platform sign-off |

## Workstream dependencies

R0 gates all implementation. R1 precedes R3 release packaging. R2 is prerequisite for R3 content delivery and R4 edits. R4 precedes R5 and R6. R5 and R6 meet before R7 usability; R8 follows the exit gates. R3 art production can proceed alongside R4/R5 only after the resource/map contracts are defined. Legacy compatibility is a test goal, not permission to redistribute restricted art.

## Revision contract

Every Rn.m document must specify: purpose, in/out scope, prerequisites, affected modules and C APIs, fixture/asset requirements, acceptance criteria with IDs, validation commands, regression gates, failure injection, measured limits where relevant, review checklist, rollback strategy, and evidence paths. A passing check must reference actual output; planned commands are not results. Separate automated passes from manual visual or fresh-player acceptance.

## Merge/approval rules

1. Create one branch or PR per small revision (or explicitly documented group).
2. Require build, focused test, dependent regression and formatting/diff checks.
3. Report exit codes, environment, exact commands, screenshots/captures and performance context; do not invent performance.
4. Review memory safety, deterministic behaviour, API ownership, packaging/licences and compatibility.
5. Fix findings, rerun impacted checks and update evidence.
6. Mark **PASS / FAIL / PARTIAL / NOT RUN** for each criterion. Block milestone closure on critical FAIL/PARTIAL/NOT RUN.
7. Record exceptions as approved ADRs with their consequences, never silently suppress tests.


## Agent continuity and automation-first development

`AGENTS.md` is the shared entry point for Codex, Gemma/OpenCode and future agents. `docs/automation/PLAYBOOK.md` is the reusable operations register. **Before re-solving a task**, agents must inspect existing scripts, test fixtures, acceptance evidence and documented commands. When a procedure repeats, or will recur predictably, it should become one canonical, tested command or documented manual checklist. Do not duplicate tools or rely on chat context for project state. Every revision must record whether it discovered or automated repeated work and leave a concrete handoff for the next agent. Automation remains bounded, non-destructive and must never silently approve gates, suppress test failures or merge PRs.

## Recommended starting point

Implement **R0.1 first**, based on actual repository source and runtime rather than assumptions. Then author R0.2–R0.7 test fixtures. Do not start IP deletion or a large Workshop UI rewrite first.

Reference methodology: Ironclaw3D docs/M10_VALIDATION.md, docs/M3_VALIDATION.md, tools/test.sh, and docs/ENGINE_MODERNISATION_CHECKLIST.md. These are examples of evidence/reporting; do not copy its renderer, architecture, game features or performance thresholds uncritically.

See [AGENTS.md](../AGENTS.md), [automation playbook](automation/PLAYBOOK.md), [R0 contract](milestones/R0/PLAN.md), [validation design](validation/TEST_STRATEGY.md), [original content/IP](ORIGINAL_CONTENT_AND_IP.md), and [Workshop](WORKSHOP_DESIGN.md).
