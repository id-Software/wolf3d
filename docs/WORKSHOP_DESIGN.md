# Workshop: first-person grid authoring proposal

Status: PROPOSAL. Simple walk-around building, not CAD, not Ironclaw3D's structural/freeform 3D system.

## Product experience

Enter Workshop from game UI. Walk using normal game movement. Aim at a tile/edge, see a clearly coloured valid/invalid preview, click/place or delete. Choose a simple palette: wall, door, floor/wall appearance, decorations, player start, enemy, item and secret. A wall drag creates a whole straight run; a room rectangle creates perimeter walls with a valid interior. Every object snaps to the game's tile grid. No flying, separate top-down CAD tool, freeform mesh sculpting or complex material node interface in v1.

Create -> edit -> save -> playtest -> return -> continue editing. Hot-loading means changes are applied at a safe game/editor boundary or explicitly rebuilt only where needed; no unsafe live mutation during active simulation frames.

## One shared model and API

The same validated world representation and content IDs drive game simulation, renderer, save/load and Workshop UI. Native C operations, independent of SDL input/rendering, should include concepts such as `world_new`, `world_validate`, `world_set_tile`, `world_place_object`, `world_remove_object`, `world_begin_edit`, `world_commit_edit`, `world_undo`, `world_redo`, `world_save` and `world_load`. These are illustrative **proposed API names**, not implemented symbols or ABI promises.

Avoid global editor-only world state. Define object identity and ownership, mutation atomicity, deterministic operation order, versioned formats, error codes and asset resolution. The editor can be temporarily paused for playtest; the authored world is durable and game-session transient state is separate.

## Constraints

- Respect current grid, wall collision and raycasting constraints; do not introduce multi-storey blocks or unconstrained heights merely for editing.
- Mouse/keyboard first. Controller/Android touch editing can be a separate later design with confirmed need.
- Clear affordances: highlight active tool, readable preview, snap target and invalid placement reason; undo is essential before extensive content authoring.
- Validate occupied tiles, doors, spawn reachability, map edges, duplicates, resource limits and unknown IDs.
- Save/load must produce the same semantic map; version migrations must be explicit and tested.
- Workshop is optional to playing the shipped original game.

## Delivery slices

R4 headless edit and transaction semantics -> R5 in-world selection, placement, line/room tools and objects -> R6 atomic persistence, hot-load and return loop -> R7 full gameplay placement, undo UI and new-player usability -> R8 distribution.

## First proving demo

Using original placeholder art, create a small square room, add a corridor and door, place a start and an enemy, save to disk, restart, play it, then return to Workshop and edit a wall. Record deterministic API test, screenshot capture in Classic and Enhanced, invalid-placement cases and an observed hands-on session. Only then broaden the palette.
