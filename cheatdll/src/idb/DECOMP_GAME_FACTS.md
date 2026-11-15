# DECOMP_GAME_FACTS — owner-supplied game semantics for decompilation context

Companion to DECOMP_STATE.md (session state), DECOMP_TEMPLATE.md
(workflow rules), and DECOMP_HIERARCHY.md (class hierarchy). This file
records game-level facts supplied by the project owner — object/group
semantics, unit registries, terrain meanings, resource names — that aid
understanding, naming, and comment quality in replacements.

Rules (same as the state/hierarchy files):
- Entries are CONTEXT, never grounding. Anything used in code must still
  be re-verified against the reference files (battlegrounds_x2.lst asm +
  idb headers) and cited as if this file did not exist. The greps remain
  the authority.
- Semantics from this file justify COMMENTS and identifier choices; they
  never justify inventing field names, restructuring control flow, or
  overriding asm evidence (DECOMP_TEMPLATE.md constraints 3/6).
- Entries are owner-supplied unless marked "decode-derived"; decode-
  derived entries carry their asm citations like any ledger entry.

## object_group registry (RGE_Master_Static_Object::object_group, __int16 @+30)

Owner-supplied 2026-11-15 (game-term semantics; the binary never exports
this as an enum):

| value | meaning |
|---|---|
| 0  | Unused |
| 1  | Herd Animal |
| 2  | Fambaa (animal) |
| 3  | Unused |
| 4  | Wild Animal |
| 5  | Monster/Trouble |
| 6  | Wall |
| 7  | Farm |
| 8  | Gate |
| 9  | Fortress/AA Turret |
| 10 | Turret |
| 11 | Cruiser |
| 12 | Unused |
| 13 | Destroyer |
| 14 | Utility Trawler |
| 15 | Frigate 1 |
| 16 | AA Destroyer 1 |
| 17 | Transport Ship |
| 18 | Building |
| 19 | Doppelganger |
| 20 | Other/Dead/Projectile |
| 21 | Command Base |
| 22 | Cliff |
| 23 | Fish |
| 24 | Unused |
| 25 | Shore Fish |
| 26 | Game Engine Stuff |
| 27 | Fruit Bush |
| 28 | Holocron |
| 29 | Nova |
| 30 | Ore |
| 31 | Tree/Carbon |
| 32 | Artillery |
| 33 | AA Mobile |
| 34 | Undeployed Cannon |
| 35 | Pummel |
| 36 | Cannon |
| 37 | Unused |
| 38 | Unused |
| 39 | Frigate 2 |
| 40 | AA Destroyer 2 |
| 41 | Unused |
| 42 | Bridge/Eye Candy |
| 43 | Bomber |
| 44 | Bounty Hunter |
| 45 | Cargo Trader |
| 46 | Commander |
| 47 | Scout |
| 48 | Fighter |
| 49 | Grenade Trooper |
| 50 | Jedi |
| 51 | Jedi w/Holocron |
| 52 | Trooper |
| 53 | War Machine |
| 54 | Medic |
| 55 | AA Trooper |
| 56 | Mounted Trooper |
| 57 | Fambaa Shield Generator |
| 58 | Worker |
| 59 | Air Transport |
| 60 | Dangerous Animal |
| 61 | Power Droid |
| 62 | Air Cruiser |
| 63 | Geonosian Warrior |
| 64 | Jedi Starfighter |

Re-verify hint: values are game semantics, not layout — they do not rot
with idb_types.h regenerations. The FIELD (name/type/offset @+30) is
grounded in the master structs and re-verifies there.

## Where group values gate owned code (decode-derived, asm-cited)

- 34 (Undeployed Cannon) / 36 (Cannon): TRIBE_Master_Building_Object::
  check_placement @0x5C7B50 forwards ALL twelve arguments verbatim to the
  base TRIBE_Master_Combat_Object::check_placement when the PLACED
  object's group is 34/36 (asm 0x5C7B76/0x5C7B80 -> forward tail
  0x5C7F99) — cannons run the Combat(70) placement rules entirely, with
  no building-side extra scan.
- 8 (Gate) vs 6 (Wall): the same function's object scan skips a blocker
  when the placed object is group 8 (Gate), the blocker's master is
  group 6 (Wall), and the blocker is owned by the placing player
  (asm 0x5C7D1A-0x5C7D2E) — a Gate is placed over the player's own Wall
  segments.
- 31 (Tree/Carbon): the same scan's group-31 terrain gate — blockers in
  group 31 with object_state > 2 and a non-null tile are skipped unless
  the tile's terrain_type (RGE_Tile @+5) is in the gating set
  {10,13,17,18,19,20,21} (original jump table byte_5C7FEC/off_5C7FE4,
  lst :764212-764217). The onCheckPlacement_carbon patch (terrain.cpp
  :1452, hook 0x005C7E2C) makes the set version-gated via isCarbon at
  DAT v9+ (baked into the replacement).
- 27 (Fruit Bush): RGE_Master_Static_Object::check_placement
  persistently WRITES this->radius_z = 1.0 for group-27 placements
  (real field store, asm 0x48E7FD — per the state-file ledger) so bushes
  block later placements; the Building scan's blocker gate additionally
  admits group-27 blockers past the rz > 0 test directly
  (rz > 0 || master id == 50 || grp == 27, asm 0x5C7EBB-0x5C7EC6).
- 23 (Fish) / 25 (Shore Fish): the Building scan substitutes
  radius_z = 1.0 for fish blockers whose rz == 0 (asm 0x5C7E74-0x5C7E86)
  — zero-radius fish still block building placement.
- 19 (Doppelganger) as an object_group is a distinct registry from
  master_type 25 (RGE_MASTER_TYPE_DOPPLEGANGER); the two namespaces are
  unrelated (master_type registry: DECOMP_HIERARCHY.md).

## Related registries

- master_type discriminants: canonical in DECOMP_HIERARCHY.md (owner-
  supplied; enum RGE_MASTER_TYPE in
  src/engine/object/RGE_Master_Static_Object.h).

## Magic-constant findings registry (decode-derived, accumulating)

The owner-directed registry for magic constants encountered in decodes.
Each entry accumulates observations with asm citations; entries graduate
to full meanings as knowledge accumulates. Status per entry: CONFIRMED /
PARTIAL / UNKNOWN. Owner-supplied meanings, when they arrive, move into
the sections above and the entry is marked.

- **prog_mode** — RGE_Base_Game::prog_mode @+476 (0x1DC). PARTIAL:
  compared `== 7` to gate live-world lookups — RGE Master Static
  check_placement (prog-mode compare, asm-verified 2026-11-14) and
  TRIBE Master Combat check_placement (`prog_mode == 7 -> RGE_Game_
  World__object (:6086) re-fetch`, asm 0x5C9xxx pass). Working
  interpretation: 7 = active in-game mode; editor/menu values unknown.
- **gameType** — TRIBE_Game::gameType (0x5EF1A0, u8 __thiscall,
  idb_functions.h:5513). UNKNOWN: value 6 gates the desolid-building
  pass-through and own-moving-unit skip in the Building check_placement
  (asm 0x5C7DD3-0x5C7DDE).
- **master id == 50** — RGE_Master_Static_Object::id (__int16 @+24).
  UNKNOWN: skip-gate for moving blockers and rz-gate escape in the
  Building scan (0x5C7CF3/0x5C7EBB); rz-substitution exclusion in the
  Combat(70) scan (rz==0 && type 80 && id != 50 -> 1.0). Game meaning
  not yet supplied — do not name it in code.
- **object_state** — RGE_Static_Object::object_state (u8 @+88). PARTIAL:
  `>= 7` = not placement-scannable (all three check_placement scans
  gate on state < 7); `== 2` with master speed > 0 = the "moving" case
  in the Building own-unit gate. Full state list unknown.
- **movement_type == 1** — master movement_type (@+120). PARTIAL: the
  Combat(70) check_placement fly-over rule (both objects movement_type
  1 -> 2D-radius overlap test only, asm 0x5C95xx). Other values unknown.
- **desolid_flag** — TRIBE_Building_Object::desolid_flag (u8 @+565).
  PARTIAL: set Building(80) blockers with group not 34/36 become
  pass-through unless mutual allies AND gameType == 6 (Building scan,
  asm 0x5C7D4D-0x5C7D86). Field name from idb_types.h; behavior decoded.
- **terrain_type set {10,13,17,18,19,20,21}** — RGE_Tile::terrain_type
  (u8 @+5). UNKNOWN per-value: the legacy group-31 placement-gate set
  (jump table byte_5C7FEC/off_5C7FE4); at DAT v9+ the isCarbon set
  replaces it (terrain.cpp:952, onCheckPlacement_carbon).
- **radius_z zero/substitution** — master radius_z (float @+68). PARTIAL:
  rz == 0 fails the blocker gates; specific classes get 1.0 substituted
  (type-80 with id != 50, group 23/25 blockers; placed group 27 objects
  get a persistent store) — working interpretation: "treat as having
  blocking height".
- **flt_657DC0 = 8.0f** — .rdata constant (lst :987837). CONFIRMED:
  the check_placement scan-rectangle margin (+-8.0 tiles) across the
  RGE Static / Combat(70) / Building scans and RGE_Command__do_command_
  order (DATA XREF, lst :987837).

## Open — meanings not yet supplied

(Folded into the registry above; entries there carry the UNKNOWN status.)
