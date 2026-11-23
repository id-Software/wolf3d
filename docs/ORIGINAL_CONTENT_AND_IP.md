# Original game and IP separation specification

Status: PROPOSAL. This is engineering/licensing planning, not a legal clearance finding.

## Goal

Ship an independently recognisable original retro FPS whose installed game does not require Wolfenstein 3D proprietary assets, branding or data. Keep its fast grid-based raycaster and classic-era feel without reproducing protected expressions.

## Audit required before replacement

Inventory every source file, dependency, asset, map, name, menu string, UI glyph, font, audio file, weapon/enemy sprite, texture, executable/package identifier, documentation claim, marketing screenshot and third-party tool. Record origin, author, upstream URL or licence text, modification history, distribution rights, replacement disposition and release status.

Do not assume Wolfenstein source code is public domain or that rebranding discharges any GPL obligations. Source licence compliance, trademarks, proprietary artwork, third-party artwork/audio and original-content provenance are distinct checks. Maintain required copyright notices and original source licence terms. A commercial release may be possible with some open source licensing arrangements, but require a specific audit rather than a blanket assertion.

## Separation architecture

- Keep engine/platform responsibilities distinct from game identity/content; derive game resources from data bindings instead of scattered hardcoded identifiers.
- Support a versioned original content pack with manifest, resource IDs, sprites, wall textures, sounds, levels, HUD/menu definitions and game metadata.
- Legacy original-game data, when lawful to test, remains an optional **external** compatibility source and is never quietly bundled in the original distribution.
- Missing/invalid content fails with actionable diagnostics; no silent fallback to uncleared legacy assets.
- Add tests that scan staged release outputs for forbidden names/data and launch original content with legacy directories missing.
- Capture provenance for all new art and sound (created, commissioned, licensed, generated and reviewed), including attribution requirements.

## R1 revision acceptance

R1.1 licence/provenance matrix complete; R1.2 references classified; R1.3 engine/content boundary documented and covered; R1.4 legacy isolation regression; R1.5 staged asset manifest and original-only launch definition. Detailed R1 implementation depends on R0 audit. Do not delete legacy files or claim legal release readiness as part of documentation approval.
