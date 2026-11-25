# DECOMP_GAME_FACTS вЂ” owner-supplied game semantics for decompilation context

Companion to DECOMP_STATE.md (session state), DECOMP_TEMPLATE.md
(workflow rules), and DECOMP_HIERARCHY.md (class hierarchy). This file
records game-level facts supplied by the project owner вЂ” object/group
semantics, unit registries, terrain meanings, resource names вЂ” that aid
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

Re-verify hint: values are game semantics, not layout вЂ” they do not rot
with idb_types.h regenerations. The FIELD (name/type/offset @+30) is
grounded in the master structs and re-verifies there.

## master_id registry (RGE_Master_Static_Object::id, __int16 @+24)

Owner-supplied 2026-11-15:

| value | meaning |
|---|---|
| 50  | Farm |
| 276 | Monument |
| 335 | Shield Generator |
| 357 | Depleted Farm |

Other master_id values: added here as the owner designates them (see the
awaiting-designation list below).

## action_type registry (RGE_Action::action_type @+4 / RGE_Task::action_type @+6, __int16)

Owner-supplied 2026-11-22 (Pickup unit corrected to 132 same day).
Consumers in owned code: work2 (the task-acceptance gate == 7 Attack;
the confirm block == 9 Attack2), is_idle (the type-107 Artifact
placeholder check), setUnitAIAction (the 12-entry table), create_task_
action (the construction switch: 4 Explore / 5 Gather / 7 Attack /
0xA Bird / 0xD Guard / 0x15 Make), actionTypeIfOrdered (the 104
Convert range logic).

| value | meaning |
|---|---|
| 1   | Move to |
| 2   | Follow |
| 3   | Enter |
| 4   | Explore |
| 5   | Gather |
| 7   | Attack |
| 9   | Attack2 |
| 10  | Bird |
| 12  | Transport |
| 13  | Guard |
| 21  | Make |
| 101 | Build |
| 102 | Make unit |
| 103 | Make tech |
| 104 | Convert |
| 105 | Heal |
| 106 | Repair |
| 107 | Artifact |
| 108 | Discovery artifact |
| 110 | Hunt |
| 111 | Trade |
| 120 | Wonder |
| 121 | Farm |
| 123 | Housing |
| 125 | Unpack |
| 131 | Offboard trade |
| 132 | Pickup unit |
| 135 | Capture |
| 136 | Deliver unit |
| 149 | Shepherd |

## UnitAI action registry (UnitAIModule::currentActionValue, int)

Owner-supplied 2026-11-22. The module's current-action id
(setUnitAIAction's second resolved value; work2's type()==9 attack
confirm pairs it with the autoAttackOrderFlag clear).

| value | meaning |
|---|---|
| 600 | attack |
| 601 | defend |
| 602 | build |
| 603 | heal |
| 604 | convert |
| 605 | explore |
| 606 | stop |
| 607 | run away |
| 608 | retreat |
| 609 | gather |
| 610 | move |
| 611 | patrol |
| 612 | follow |
| 613 | hunt |
| 614 | transport |
| 615 | trade |
| 616 | evasive move |
| 617 | enter |
| 618 | repair |
| 619 | train |
| 620 | research |
| 621 | unload |
| 622 | seek and destroy |
| 623 | explore and destroy |
| 624 | group gather |
| 628 | wait |
| 629 | attack roundup |
| 630 | scout |
| 631 | control |
| 632 | shepherd |
| 699 | released |

## UnitAI order registry (UnitAIModule::currentOrderValue, int) - DERIVED

Decode-derived 2026-11-22 (NOT owner-supplied): every setUnitAIAction
table pair satisfies order = action + 100 (all 12 pairs, asm
0x409C57-0x409CE3: 1->710/610, 3->717/617, 4->705/605, 5->709/609,
9->700/600, 12->714/614, 101->702/602, 104->704/604, 105->703/603,
106->718/618, 110->713/613, 111->715/615). The order names mirror the
action names:

| value | meaning (= action + 100) |
|---|---|
| 700 | attack order |
| 702 | build order |
| 703 | heal order |
| 704 | convert order |
| 705 | explore order |
| 709 | gather order |
| 710 | move order |
| 713 | hunt order |
| 714 | transport order |
| 715 | trade order |
| 717 | enter order |
| 718 | repair order |

Enums: RGE_ACTION_TYPE / RGE_UNITAI_ACTION / RGE_UNITAI_ORDER in
src/engine/object/RGE_Action_Types.h (the RGE_MASTER_TYPE precedent).

Re-verify hint: game semantics, not layout вЂ” do not rot with
idb_types.h regenerations. The FIELD (name/type/offset @+24) re-verifies
in the master structs.

## master_id values encountered - resolved designations

| value | meaning (owner-supplied 2026-11-15) | where (decode-derived, asm-cited) |
|---|---|---|
| 276 | Monument | TRIBE_Building_Object runtime ctor1/ctor2 post-setup: master id == 276 -> RGE_Action_Object__set_task(master->default_task) fires at construction completion (asm 0x553335-0x55334A / 0x5535A1-0x5535B6) |
| 335 | Shield Generator | RGE_Doppleganger_Object runtime ctor: the construction-sprite swap gate excludes master id 335 (with master_type 80, group != 8 (Gate), construction_id >= 0) (asm 0x46FD28, word @+0x18) |
| 83 | Worker transform target (the task_by_group == 1 variant) - owner designation owed | RGE_Combat_Object::attack2_new: the WORKER (object_group 58) ordered to attack transforms via transform() into owner->master_objects[83] (asm 0x4452B4, [eax+14Ch] = 83*4) - the armed-worker conversion; transform is OWNED (transform_new routes) |
| 293 | Worker transform target (the task_by_group != 1 variant) - owner designation owed | the same attack2 block: master_objects[293] (asm 0x4452C6, [ecx+494h] = 293*4) |
| 992 | AA counter-battery projectile master (the GE Assault Mechanic vs Aircraft) - owner designation owed | attack4_new: the projectile-master selection - the launcher is a GE Assault Mech AND the target is an Aircraft -> make_new_obj from master_objects[992] (asm 0x44425E, [ecx+0F80h] = 992*4) - the AA-homing missile (the launcher arg = 0 at init_missile) |
| 836 | AA counter-battery projectile master (the Air Cruiser vs Aircraft) - owner designation owed | the same selection: the launcher is an Air Cruiser AND the target is an Aircraft -> master_objects[836] (asm 0x444283, [ecx+0D10h] = 836*4) |


## Where group values gate owned code (decode-derived, asm-cited)

- 34 (Undeployed Cannon) / 36 (Cannon): TRIBE_Master_Building_Object::
  check_placement @0x5C7B50 forwards ALL twelve arguments verbatim to the
  base TRIBE_Master_Combat_Object::check_placement when the PLACED
  object's group is 34/36 (asm 0x5C7B76/0x5C7B80 -> forward tail
  0x5C7F99) вЂ” cannons run the Combat(70) placement rules entirely, with
  no building-side extra scan.
- 8 (Gate) vs 6 (Wall): the same function's object scan skips a blocker
  when the placed object is group 8 (Gate), the blocker's master is
  group 6 (Wall), and the blocker is owned by the placing player
  (asm 0x5C7D1A-0x5C7D2E) вЂ” a Gate is placed over the player's own Wall
  segments.
- 31 (Tree/Carbon): the same scan's group-31 terrain gate вЂ” blockers in
  group 31 with object_state > 2 and a non-null tile are skipped unless
  the tile's terrain_type (RGE_Tile @+5) is in the gating set
  {10,13,17,18,19,20,21} (original jump table byte_5C7FEC/off_5C7FE4,
  lst :764212-764217). The onCheckPlacement_carbon patch (terrain.cpp
  :1452, hook 0x005C7E2C) makes the set version-gated via isCarbon at
  DAT v9+ (baked into the replacement).
- 27 (Fruit Bush): RGE_Master_Static_Object::check_placement
  persistently WRITES this->radius_z = 1.0 for group-27 placements
  (real field store, asm 0x48E7FD вЂ” per the state-file ledger) so bushes
  block later placements; the Building scan's blocker gate additionally
  admits group-27 blockers past the rz > 0 test directly
  (rz > 0 || master id == 50 (Farm) || grp == 27, asm 0x5C7EBB-0x5C7EC6).
- 23 (Fish) / 25 (Shore Fish): the Building scan substitutes
  radius_z = 1.0 for fish blockers whose rz == 0 (asm 0x5C7E74-0x5C7E86)
  вЂ” zero-radius fish still block building placement.
- 19 (Doppelganger) as an object_group is a distinct registry from
  master_type 25 (RGE_MASTER_TYPE_DOPPELGANGER); the two namespaces are
  unrelated (master_type registry: DECOMP_HIERARCHY.md).
- 35 (Pummel): TRIBE_Combat_Object runtime ctor_read (TRIBE_Combat_
  Object3 @0x559F30) resyncs an own-master Pummel's speed at load вЂ”
  own_master != 0 && master object_group == 35 -> master_obj->speed =
  owner->master_objects[id]->speed (asm 0x559FFC-0x55A021).
- 34/36 + 8 (runtime side): the TRIBE_Building_Object ctor tails'
  doppleganger-install gate (asm 0x55334F-0x55337F) вЂ” cannons (34/36),
  desolid buildings, object_flags-bit-2 buildings, and linked non-gates
  install no doppleganger.

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

- **RGE_Action::state idle set {0, 1, 2, 3, 13, 14}** - CONFIRMED
  (owner-supplied state registry 2026-11-22, below): the is_idle
  no-op set reads as None / Done / Wait / Failed / Search /
  Invalidated - an action that is absent, finished, waiting, failed,
  (re)searching, or invalidated leaves the unit idle (asm jz chain
  0x4099F5-0x409A10). The update-removal pair in update_new
  (state == Search || Done -> remove_action, asm 0x4098EF-0x4098F5)
  reads on the same registry. Enum: RGE_ACTION_STATE in
  src/engine/object/RGE_Action_Types.h.

## RGE_Action::state registry (RGE_Action::state, unsigned __int8 @+0xC)

Owner-supplied 2026-11-22. The per-action state machine (is_idle's
idle set; the list-update removal pair Search/Done in update_new;
conversion.cpp's check_meditation_and_purge tests {7 Work2, 6 Work,
4 Goto} - unowned site, registry context only).

| value | meaning |
|---|---|
| 0  | None |
| 1  | Done |
| 2  | Wait |
| 3  | Search |
| 4  | Goto |
| 5  | Goto2 |
| 6  | Work |
| 7  | Work2 |
| 8  | Return |
| 9  | Turn |
| 10 | Delay |
| 11 | Move |
| 12 | Attack |
| 13 | Failed |
| 14 | Invalidated |
| 15 | MoveNoSearch |

- **notify mType 5xx namespace** — UNKNOWN per-value (owner-directed
  2026-11-23: an enum for the 5xx notify types will be made when more
  values are known; the 6xx values are the UnitAI action registry
  (RGE_UNITAI_ACTION in RGE_Action_Types.h) and get the enum applied at
  call sites). Observations: 500 = notify_new's own-processing gate
  (the object_state == 0 branch consults the UnitAI module only when
  mType == 500; notify @0x445110, idb_functions.h:1096); 500 =
  UNDER-ATTACK (damage_new sends it to the unit: caller = the attacker
  id - for missiles fired_from_id @+0x184, else attackingObject->id;
  recipient = id; p1 = the attacker id; p2 = ftol(hp); p3 =
  object_group; asm 0x4448D4-0x4448EB); 512 =
  do_attack_new's single-target attack-notify mType (asm 0x44781 push
  200h, the caller = recipient = id form); 530 = the owner-level
  unit-attacked notify (damage_new sends it to the victim's owner when
  the attacker's owner differs: caller = id, recipient/p1 = the
  attacker id, p2 = ftol(hp), p3 = group; asm 0x44915 push 212h; the
  -1 form when attackingObject == 0); 527 = attack4_new's target-side
  under-attack notify (fired when the victim's owner is an AI:
  owner->computerPlayer (slot 7) == 1; caller = target->id,
  recipient/p1 = the attacker's id, p2 = ftol(target->hp), p3 = the
  target's object_group; asm 0x44436A push 20Fh). THE GARRISON-REQUEST
  LIST: TribeTacticalAIModule::gbg_garrisonUnits (the idb:
  std::vector<UnitPair>; UnitPair = {int first_id; int second_id} =
  {the garrisoning unit's id, the garrison target's id}) - the AI
  issues via taskToEnter (0x005AA530: operator_new(8), the dedup scan,
  the unitsTaskedThisUpdate bookkeeping, then sendUnitAIOrder(player,
  id, unit_id, 717, target_id, ...) - 717 = the ENTER order-value
  (617+100)); enter_new removes the fulfilled record (the shift-left +
  the _Last decrement); the destructor frees each record then the
  array. ATTRIBUTE INDICES (the shield subsystem): attributes[25] =
  the Shield recharge interval (owner-supplied), attributes[10] = the
  recharge multiplier, attributes[26] = the shield drop-off rate (the
  gbg_doShieldRecharge/gbg_doShieldDropoff consumers).
  Re-verify: the notify chain (UnitAIModule::notify, the owner vtbl
  notify) fixes the meanings - a future decode target.

- **shield_recharge_accumulator (RGE_Static_Object +0x8, float; the
  2026-11-23 IDB rename of gbg_unknown_2; the flattened prefix) = the
  shield charge-tick accumulator (seconds)** — CONFIRMED
  (decode-derived 2026-11-23; attribute 25 owner-supplied "Shield
  recharge interval"): RGE_Static_Object::gbg_doShieldRecharge
  (idb_functions.h:3822, 0x0054ED30) grows it by world_time_delta_
  seconds each frame; crossing attributes[25] fires recharge ticks -
  sp += the tier-scaled amount (2/4/8/12/16/20 by the sp bands
  100/1000/2000/3000/4000) x attributes[10] (the recharge multiplier)
  - consuming one interval per tick (the while loop handles multiple
  elapsed intervals per frame), capped at master->hp (the shield max
  doubles as the master's hp field). The sibling
  gbg_doShieldDropoff (0x0054EE70) uses the same accumulator with a
  hardcoded 1.0 s interval: each tick sp -= attributes[26] (the
  drop-off rate), floored at 0. Persisted (the ctor_read stream
  .c:297907 + the save write .c:299930); zeroed at construction
  (.c:297518/:297780).

- **shield_hit_timer (RGE_Static_Object +0xC, float; the 2026-11-23
  IDB rename of gbg_unknown_3, int -> float) = the shield-hit flash
  timer (seconds)** — CONFIRMED (decode-derived 2026-11-23): damage writes the 1.5f bits
  on SHIELD-engaged hits (the absorb or pierce branches only - never
  the plain-hp branch; both tiers: RGE_Combat_Object::damage asm
  0x444894/0x444876 + RGE_Static_Object::damage .c:301695);
  RGE_Static_Object::update decays it by world_time_delta_seconds per
  frame, clamping at 0 (.c:299536-299541, the *(float*)& form); the
  VIEW/occlusion draw layer tints the unit's occluding shape YELLOW
  while it is active (.c:447887-447888: the timer > 0 ->
  OccludingShapeNum = color_yellow); the ctor/recycle families zero
  it; the save family persists it as 4 raw bytes (.c:455188/:457977).
  Same-named fields in OTHER structs are distinct (the World's sound
  index, the tacticalAI's ManagedArray_int_, the oc/gbg list-head
  form) - the name-grep noise. IDB CORRECTION APPLIED (the regen
  2026-11-23): gbg_unknown_3 -> shield_hit_timer (int -> float) and
  gbg_unknown_2 -> shield_recharge_accumulator (float, the name only)
  across the runtime tree; damage_new's marker store re-rendered to
  this_->shield_hit_timer = 1.5f (the re-verified from the regenerated
  render).

- **prog_mode** вЂ” RGE_Base_Game::prog_mode @+476 (0x1DC). PARTIAL:
  compared `== 7` to gate live-world lookups вЂ” RGE Master Static
  check_placement (prog-mode compare, asm-verified 2026-11-14) and
  TRIBE Master Combat check_placement (`prog_mode == 7 -> RGE_Game_
  World__object (:6086) re-fetch`, asm 0x5C9xxx pass). Working
  interpretation: 7 = active in-game mode; editor/menu values unknown.
- **gameType** вЂ” TRIBE_Game::gameType (0x5EF1A0, u8 __thiscall,
  idb_functions.h:5513). UNKNOWN: value 6 gates the desolid-building
  pass-through and own-moving-unit skip in the Building check_placement
  (asm 0x5C7DD3-0x5C7DDE).
- **master id 50 = Farm / 357 = Depleted Farm** вЂ” RGE_Master_Static_
  Object::id (__int16 @+24). CONFIRMED (owner-supplied 2026-11-15;
  registry above). Observations: id-50 masters never block placement
  (moving skip asm 0x5C7CF3, rz-gate escape 0x5C7EBB, rz==0 exclusion
  in the Combat(70) scan) вЂ” farms are walkable; and the Building
  runtime destructor/recycle terrain-decay gate keys on id 50/357
  (asm 0x553976-0x5539A7 / 0x553B68-0x553B93): destroying or
  recycling a Farm or Depleted Farm reverts the underlying terrain
  (change_terrain(27)) while the game is running, unless the tile
  terrain is already in the skip set {8, 31, 228}.
- **farm-decay terrain set {8, 31, 228} + revert target 27** вЂ”
  RGE_Map__get_terrain result (u8; terrain-id namespace, distinct
  from object_group/master_id). UNKNOWN per-value: the Building
  runtime destructor/recycle skip the revert for 8/31 and (FARM_DEAD
  patch) 228, and revert to 27 (change_terrain(0x1B)) otherwise. 228
  is patch-era (terrain_gen.cpp onFarmDead2/3, baked 2026-11-15).
- **object_state** вЂ” RGE_Static_Object::object_state (u8 @+88). Machine decoded 2026-11-24
  (RGE_Static_Object::update asm 0x54EF00-0x54F1BB):
  the update-driven machine: 0/2 -> hp < 1.0 -> **3** (the death
  sprite) -> **3/5** (the shared attribute-held decay body; the
  NEGDECAY regen mod lives here) -> **6** (undead, master undead_flag)
  or **7** -> **8** (spawn_death_obj); **8** = cleanup (update returns
  (recyclable != 0) + 1 = 2 recycle / 1 remove - the caller consumption
  unowned, inferred); the container mirroring (inside_obj state > 2
  propagates); **1/4** no update work, set elsewhere (UNKNOWN).
  Standing: 2 = ACTIVE (the shield tick, check_damage_sprites, full
  notify pass, the placement "moving" case (state 2 + master
  speed > 0, the Building own-unit gate), the doppleganger ceiling
  < 3); 0 = passive (the mType-500-only notify); 7/8 = >= 7 = not
  placement-scannable (all three check_placement scans gate on
  state < 7); the corpses 3/5/6 remain scannable/blocking; sleep =
  the sleep_flag/goto_sleep_flag pair + the list dormancy, not a
  state. Death threshold hp < 1.0.
- **movement_type == 1** вЂ” master movement_type (@+120). PARTIAL: the
  Combat(70) check_placement fly-over rule (both objects movement_type
  1 -> 2D-radius overlap test only, asm 0x5C95xx). Other values unknown.
- **desolid_flag** вЂ” TRIBE_Building_Object::desolid_flag (u8 @+565).
  PARTIAL: set Building(80) blockers with group not 34/36 become
  pass-through unless mutual allies AND gameType == 6 (Building scan,
  asm 0x5C7D4D-0x5C7D86). Field name from idb_types.h; behavior decoded.
- **terrain_type set {10,13,17,18,19,20,21}** вЂ” RGE_Tile::terrain_type
  (u8 @+5). UNKNOWN per-value: the legacy group-31 placement-gate set
  (jump table byte_5C7FEC/off_5C7FE4); at DAT v9+ the isCarbon set
  replaces it (terrain.cpp:952, onCheckPlacement_carbon).
- **radius_z zero/substitution** вЂ” master radius_z (float @+68). PARTIAL:
  rz == 0 fails the blocker gates; specific classes get 1.0 substituted
  (type-80 with id != 50 (Farm), group 23/25 blockers; placed group 27
  objects get a persistent store) вЂ” working interpretation: "treat as
  having blocking height".
- **flt_657DC0 = 8.0f** вЂ” .rdata constant (lst :987837). CONFIRMED:
  the check_placement scan-rectangle margin (+-8.0 tiles) across the
  RGE Static / Combat(70) / Building scans and RGE_Command__do_command_
  order (DATA XREF, lst :987837).
- **CheckForCollisions query parameters {32, 25, -2}** — the OBSystem
  vtbl idx 19 call's int arguments as made by passableTile/passableTile2
  (asm 0x4A1BF7/0x4A1BEB/0x4A1BE9 and the passableTile twins; vtbl
  idb_types.h:9964/10167). UNKNOWN per-value: 32 (0x20) = the 5th int
  arg, 25 (0x19) = the 8th int arg, -2 (0xFFFFFFFE) = the 9th int arg.
  Positional guesses (a detail/level mask, a max-hits cap matching the
  obSystemCollisions array, a player/exclusion code) are NOT grounded.
  Re-verify: the callee's implementation body in the .lst fixes the
  meanings - a future decode target.

- **sound_id 38** — RGE_Base_Game__play_sound. PARTIAL (decode-derived
  2026-11-24, asm 0x445034 push 26h): the capture fanfare —
  RGE_Combat_Object::update plays it when a capture_flag unit changes
  ownership to the local player (the get_player compare, asm
  0x445008-0x445015), suppressed when the master is a Building with a
  linked_owner (asm 0x445017-0x445028). Other users unknown.

## Open вЂ” meanings not yet supplied

(Folded into the registry above; entries there carry the UNKNOWN status.
Plus the awaiting-designation master_id table: 276, 335.)

## RGE_Player::attributes registry (float* @+0xAC = 172; [index] = +4*index)

Owner-supplied 2026-11-25 (the complete table; the consumed values are
enumerated as RGE_PLAYER_ATTRIBUTE in RGE_Action_Types.h):

| idx | meaning | idx | meaning |
|---|---|---|---|
| 0 | Food Storage | 110 | P1 Kill Value |
| 1 | Carbon Storage | 111 | P2 Kill Value |
| 2 | Ore Storage | 112 | P3 Kill Value |
| 3 | Nova Storage | 113 | P4 Kill Value |
| 4 | Population Headroom | 114 | P5 Kill Value |
| 5 | Conversion Range | 115 | P6 Kill Value |
| 6 | Current Tech Level | 116 | P7 Kill Value |
| 7 | Holocrons Captured | 117 | P8 Kill Value |
| 8 | Unused (Trade Bonus) | 118 | P1 Razings |
| 9 | Trade Goods | 119 | P2 Razings |
| 10 | Recharge Rate of Shields | 120 | P3 Razings |
| 11 | Current Population | 121 | P4 Razings |
| 12 | Corpse Decay Time | 122 | P5 Razings |
| 13 | Remarkable Discovery | 123 | P6 Razings |
| 14 | Monuments Captured | 124 | P7 Razings |
| 15 | Meat Storage | 125 | P8 Razings |
| 16 | Berry Storage | 126 | P1 Razing Value |
| 17 | Fish Storage | 127 | P2 Razing Value |
| 18 | Power Core Range | 128 | P3 Razing Value |
| 19 | Unpowered Building Work Rate Multiplier | 129 | P4 Razing Value |
| 20 | Units Killed | 130 | P5 Razing Value |
| 21 | Technology Count | 131 | P6 Razing Value |
| 22 | % Map Explored | 132 | P7 Razing Value |
| 23 | Submarine Detection | 133 | P8 Razing Value |
| 24 | Shield Generator Range | 134 | Standing Fortresses |
| 25 | Shield Recharge Interval | 135 | Hit Points Razed |
| 26 | Drop-off Time of Shields | 136 | Kills by P1 |
| 27 | Enable Jedi Conversion | 137 | Kills by P2 |
| 28 | Enable Building Conversion | 138 | Kills by P3 |
| 29 | Fambaa Shield Generator Range | 139 | Kills by P4 |
| 30 | Unused (Building Limit) | 140 | Kills by P5 |
| 31 | Enable A-A Attack for AT-AT | 141 | Kills by P6 |
| 32 | Bonus Population Cap | 142 | Kills by P7 |
| 33 | Power Core Shielding | 143 | Kills by P8 |
| 34 | Force | 144 | Razings by P1 |
| 35 | Force Recharging Rate | 145 | Razings by P2 |
| 36 | Farm Food Amount | 146 | Razings by P3 |
| 37 | Civilian Population | 147 | Razings by P4 |
| 38 | Shields On for Bombers/Fighters | 148 | Razings by P5 |
| 39 | All Techs Achieved | 149 | Razings by P6 |
| 40 | Military Population | 150 | Razings by P7 |
| 41 | Conversions | 151 | Razings by P8 |
| 42 | Standing Monuments | 152 | Value Killed by Others |
| 43 | Razings | 153 | Value Razed by Others |
| 44 | Kill Ratio | 154 | Killed by Others |
| 45 | Survival to Finish | 155 | Razed by Others |
| 46 | Tribute Inefficiency | 156 | Tribute from P1 |
| 47 | Building Self Regeneration Timer | 157 | Tribute from P2 |
| 48 | Town Center Unavailable | 158 | Tribute from P3 |
| 49 | Gold Counter | 159 | Tribute from P4 |
| 50 | Reveal Ally | 160 | Tribute from P5 |
| 51 | Shielding | 161 | Tribute from P6 |
| 52 | Monasteries | 162 | Tribute from P7 |
| 53 | Tribute Sent | 163 | Tribute from P8 |
| 54 | All Ruins Captured | 164 | Value Current Units |
| 55 | All Relics Captured | 165 | Value Current Buildings |
| 56 | Enable Stealth for Masters | 166 | Food Total |
| 57 | Kidnap Storage | 167 | Carbon Total |
| 58 | Masters Can See Hidden Units | 168 | Ore Total |
| 59 | Tech Level 1 Tech ID | 169 | Nova Total |
| 60 | Tech Level 2 Tech ID | 170 | Total Value of Kills |
| 61 | Tech Level 3 Tech ID | 171 | Total Tribute Received |
| 62 | Building Housing Rate | 172 | Total Value of Razings |
| 63 | Tech Level 4 Tech ID | 173 | Total Fortresses Built |
| 64 | Gather Accumulator | 174 | Total Monuments Built |
| 65 | Salvage Decay Rate | 175 | Tribute Score |
| 66 | Unused (Allow Formations) | 176 | Convert Min Adjustment |
| 67 | Can Convert | 177 | Convert Max Adjustment |
| 68 | Hit Points Killed | 178 | Convert Resist Min Adjustment |
| 69 | Killed P1 | 179 | Convert Resist Max Adjustment |
| 70 | Killed P2 | 180 | Convert Building Min |
| 71 | Killed P3 | 181 | Convert Building Max |
| 72 | Killed P4 | 182 | Convert Building Chance |
| 73 | Killed P5 | 183 | Reveal Enemy |
| 74 | Killed P6 | 184 | Value Wonders Castles |
| 75 | Killed P7 | 185 | Food Score |
| 76 | Killed P8 | 186 | Carbon Score |
| 77 | Conversion Resistance | 187 | Ore Score |
| 78 | Trade Vig Rate | 188 | Nova Score |
| 79 | Ore Mining Productivity | 189 | Carbon Gathering Productivity |
| 80 | Queued Units | 190 | Food-gathering Productivity |
| 81 | Training Count | 191 | Holocron Nova Production Rate |
| 82 | Start with Packed Town Center | 192 | Converted Units Die |
| 83 | Boarding Recharge Rate | 193 | Meditation |
| 84 | Starting Villagers | 194 | Crenellations |
| 85 | Tech Cost Modifier | 195 | Construction Rate Modifier |
| 86 | Tech Time Modifier | 196 | Hun Wonder Bonus |
| 87 | Concentration | 197 | Spies Discount |
| 88 | Fish Trap Food Amount | 198 | Unknown |
| 89 | Medic Healing Rate | 199 | Unknown |
| 90 | Healing Range | 200 | Misc Counter 1 |
| 91 | Starting Food | 201 | Misc Counter 2 |
| 92 | Starting Carbon | 202 | Misc Counter 3 |
| 93 | Starting Ore | 203 | Misc Counter 4 |
| 94 | Starting Nova | 204 | Misc Counter 5 |
| 95 | Enable PTWC / Kidnap / Loot | 205 | Unknown |
| 96 | Berserker Heal Timer | 206 | Unknown |
| 97 | Dominant Sheep Control | 207 | Unknown |
| 98 | Object Cost Sum | 208 | Unknown |
| 99 | Tech Cost Sum | 209 | Unknown |
| 100 | Holocron Nova Sum | 210 | Unknown |
| 101 | Trade Income Sum | 211 | Buildings are self powered |
| 102 | P1 Tribute | 212 | Aircraft Regenerates Hit Points |
| 103 | P2 Tribute | 213 | Max Force After Conversion |
| 104 | P3 Tribute | 214 | Nova Mining Productivity |
| 105 | P4 Tribute | 215 | Nova generation rate (small) |
| 106 | P5 Tribute | 216 | Nova generation rate (large) |
| 107 | P6 Tribute | 217 | Ore generation rate |
| 108 | P7 Tribute | 218 | Food generation rate |
| 109 | P8 Tribute | | |

Consumed-by-decode notes: [10] the shield-pair multiplier, [25] the
shield tick interval, [26] the drop-off tick amount (the CONFIRMED
shield registry above); [36]/[88] = the Make Work timer sources keyed
on task->attribute_type == 16 (Berry Storage -> Farm Food Amount, else
Fish Trap Food Amount); [5]/[28] = actionTypeIfOrdered's 104-Convert
range logic (the Conversion Range + the Enable Building Conversion
gate); [37]/[40] = the TRIBE_Combat destructor population decrements;
the enter_target worker deposit: the types 15/16/17 (Meat/Berry/Fish
Storage) bank into [0] Food Storage; the types 0-3 bank into themselves
(Food/Carbon/Ore/Nova Storage).

## RGE_Master_Static_Object::object_flags bit registry (u32 @+172; the BYTE-wide tests in the asm)

Owner-supplied 2026-11-25:

| bit (mask) | meaning |
|---|---|
| 0 (0x01) | garrison unit |
| 1 (0x02) | ship unit |
| 2 (0x04) | stealth unit |
| 3 (0x08) | detector unit |
| 4 (0x10) | mechanical unit |
| 5 (0x20) | biological unit |
| 6 (0x40) | self-shielding unit |
| 7 (0x80) | invisible unit |

Consumed-by-decode notes: bit 1 (0x02, SHIP UNIT) = the Transport
next_to_drop_off_terrain's scan gate (the ship transports require the
shore terrains 2/4 in the scan rect; the non-ships accept any terrain)
and the Explore calculateTargetPoint's firstTileAlongLine split (the
ship targets walk the water-tile terrains 2/37; the land targets the
land walk). The other bits = the registry for the future decodes.
