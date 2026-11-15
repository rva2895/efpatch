# DECOMP_HIERARCHY — verified class hierarchy for the master-object family

Companion to DECOMP_STATE.md (session state), DECOMP_TEMPLATE.md
(workflow rules), and DECOMP_GAME_FACTS.md (game-level semantics).
This file holds the structural class-relationship
facts: derivation trees, per-tier layout identities, and the per-edge
evidence that grounds them.

Rules (same as the state file):
- Entries are LEADS plus recorded verification results. Anything used
  in code must still be re-verified against idb_types.h /
  battlegrounds_x2.lst and cited as if this file did not exist.
- Entries rot: after ANY idb_types.h regeneration, re-run the
  mechanical struct walks and re-check the ctor base-call anchors
  cited per edge below.

## Master-side tree (derivation, master_type discriminant, sizeof)

```
RGE_Master_Static_Object   (10, 208 = 0xD0)
├─ TRIBE_Master_Tree_Object   (15, 208 = 0xD0, +0 — Static mirror)
└─ RGE_Master_Animated_Object   (20, 212 = 0xD4)
   ├─ RGE_Master_Doppleganger_Object   (25, 212 = 0xD4, +0)
   └─ RGE_Master_Moving_Object   (30, 260 = 0x104)
      └─ RGE_Master_Action_Object   (40, 300 = 0x12C)
         └─ RGE_Master_Combat_Object   (50 COMBAT_BASE, 380 = 0x17C)
            ├─ RGE_Master_Missile_Object   (60, 392 = 0x188)
            └─ TRIBE_Master_Combat_Object   (70 COMBAT, 456 = 0x1CC)
               └─ TRIBE_Master_Building_Object   (80 BUILDING, 552 = 0x228)
```

- TREE (15): TRIBE_Master_Tree_Object — OWNED 2026-11-15. Exactly 6
  exports in [0x5C9780, 0x5C9A20) (scalar_deleting_destructor,
  ctor2/3/4, make_new_obj, make_new_master — idb_functions.h:5140-5145;
  extents lst :766699/:766717, :766728/:766785, :766794/:766856,
  :766865/:766927, :766936/:766992, :767001/:767049); struct
  idb_types.h:22170 (Static master mirror, 208 = 0xD0), Vtbl :22254
  (.rdata:006674A4), runtime struct :21889 (RGE_Static_Object mirror,
  180 = 0xB4). Child of Static: ctorN calls RGE_Master_Static_Object
  ctorN(do_setup=0) + Tree vfptr + if(do_setup) STATIC setupN (Tree
  has NO setup/copy/modify/save overrides — inherits Static's) +
  master_type = 15; SEH funclets jmp RGE_Master_Static_Object__
  destructor (chunks 0x646780/A0/C0); make_new_master = Static
  copy-ctor(do_setup=0) + vfptr + Static setup + master_type = 15
  AFTER the setup (the Static setup sets 10, Tree overrides — asm
  0x5C9A03); make_new_obj has NO recycle path, allocates 0xB4 and
  calls the runtime TRIBE_Tree_Object ctor1 (idb_functions.h:4084,
  0x00564DB0, tobj = the Tree master). The earlier "TREE has NO
  exported class" note was WRONG (the class was missed because it
  carries the TRIBE_ prefix like Combat/Building).
- Master_type values are discriminant REGISTRY values, not depth
  indicators: Missile (60) sits numerically between Combat_Base (50)
  and Combat (70) but is a SIBLING side branch, not a chain link
  (see the disproof below).

## Runtime-side tree (sizeof; layout-verified)

```
RGE_Static_Object   (180 = 0xB4)
└─ RGE_Animated_Object   (184 = 0xB8)
   ├─ RGE_Doppleganger_Object   (220 = 0xDC)
   └─ RGE_Moving_Object   (316 = 0x13C)
      └─ RGE_Action_Object   (324 = 0x144)
         └─ RGE_Combat_Object   (380 = 0x17C)
            ├─ RGE_Missile_Object   (396 = 0x18C)
            └─ TRIBE_Combat_Object   (452 = 0x1C4)
               └─ TRIBE_Building_Object   (632 = 0x278)
```

Runtime-side verification status: the tree is LAYOUT-verified
(mechanical walks; each runtime struct = parent prefix exact + child
extension). The runtime constructors' internal base-call chains have
NOT yet been decoded — the master-side ctor calls are the verified
ladder evidence; runtime-side ctor decoding is future scope.

## Per-edge evidence (master side)

Every edge is verified by FOUR evidence classes:
1. child ctorN calls parent ctorN with do_setup = 0 (the ladder),
2. child setupN calls parent setupN (master_type forced after),
3. child destructor / SEH unwind funclets target the IMMEDIATE
   parent's destructor,
4. child flattened struct = parent layout exact prefix + child
   extension (mechanical walk), and the child (or its setup) stores
   its own master_type.

| child (master_type) | child ctors (hooked) | base ctor called | SEH funclet jmp target | extension (offsets) |
|---|---|---|---|---|
| Tree (15) | 0x5C97B0/0x5C9830/0x5C98B0 | RGE_Master_Static_Object ctorN | chunks 0x646780/A0/C0 -> RGE_Master_Static_Object__destructor | +0 (Static mirror 208 = 0xD0; no overrides beyond the 6 owned exports — inherits Static's setups/copy/modify/save) |
| Animated (20) | 0x4885C0/0x488640/0x4886D0 (ctor1 0x488520) | RGE_Master_Static_Object ctorN | chunks 0x642030/50/70 -> RGE_Master_Static_Object__destructor | +4: speed @208 |
| Doppleganger (25) | 0x48A370/0x48A3F0/0x48A480 | Animated ctorN | 0x642190/0x6421B0/0x6421D0 -> RGE_Master_Animated_Object__destructor | +0 (vfptr swap; dopple state lives in the runtime class) |
| Moving (30) | 0x48AE10/0x48AEA0/0x48AF40 | Animated ctorN | 0x6422F0/10/30 -> RGE_Master_Animated_Object__destructor | +48: move_sprite@212, run_sprite@216, turn_speed@220, size_class@224, obj_trail_id/options/spacing@226-235, move_algorithem@236, m* yaw block@240-259 |
| Action (40) | 0x4879E0/0x487A50/0x487AD0 | Moving ctorN | 0x641F50/70/90 -> RGE_Master_Moving_Object__destructor | +40: tasks@260, default_task@264, search_radius@268, work_rate@272, gbg_work_rate_2@276, drop_site@280, backup_drop_site@282, task_by_group@284, command_sound@288, move_sound@292, run_pattern@296, gap@297-299 |
| Combat_Base (50) | 0x488BC0/0x488C30/0x488CB0 | Action ctorN | 0x6420F0/10/30 -> RGE_Master_Action_Object__destructor | +80: fight_sprite@300 ... missed_missile_spread@376 (armor/weapon tables, weapon block, orig_* set) |
| Missile (60) | 0x48A6B0/0x48A780/0x48A870 | RGE_Master_Combat_Object ctorN | 0x642230/50/70 -> RGE_Master_Combat_Object__destructor | +12: missile_type@380, targetting_type@381, missile_hit_info@382, missile_die_info@383, area_effect_specials@384, ballistics_ratio@388 |
| Combat (70) | 0x5C83C0/0x5C8430/0x5C84B0 (ctor1 0x5C8340) | RGE_Master_Combat_Object ctorN | chunks 0x6466C0/E0/0x646700 (ctor1 chunk 0x6466A0 read: jmp RGE_Master_Combat_Object__destructor) | +76: build_inventory[3]@380, build_pts_required@398, id_of_building_obj@400, button_location@402, orig_pierce_armor@404, rear/flank_attack_modifier@408/412, tribe_unit_type@416, volley block@420-447, special_attack_flag/hero_flag@448/449, garrison_sprite@452 |
| Building (80) | 0x5C6A50/0x5C6AC0/0x5C6B40 | TRIBE_Master_Combat_Object ctorN | 0x6465E0/0x646600/0x646620 -> TRIBE_Master_Combat_Object__destructor | +96: construction_sound/sprite/snow_sprite@456/460/464, building_connect_flag/facet@468/470, build_and_go_away@472, on_build_make_obj/tile/overlay/tech@474-481, can_burn@482, linked_id[4]/linked_x[4]/linked_y[4]@484/492/508, construction_id@524, open_close_id@526, open_close_sound@528, garrison_units@532, garrison_heal_rate@536, garrison_repair_rate@540, salvage_id@544, pillage_attr[6]@546 |

## Sibling disproof: Missile (60) is NOT in the Combat(70)/Building(80) ancestry

The Missile master extension occupies 380-391 (missile_type@380,
targetting_type@381, missile_hit_info@382, missile_die_info@383,
area_effect_specials@384, ballistics_ratio@388). If Combat(70) or
Building(80) descended from Missile(60), their flattened structs would
carry those fields at 380+. They do not: in TRIBE_Master_Combat_Object
and TRIBE_Master_Building_Object the 380 slot is build_inventory[3]
(they go missed_missile_spread@376 straight to build_inventory@380).
The runtime side mirrors this: at 380, RGE_Missile_Object has
max_range while TRIBE_Combat_Object has attr_amt[6]. Missile(60) and
Combat(70) are therefore SIBLINGS under Combat_Base(50), and Building
extends the Combat(70) tier.

## Per-tier identities (vftables and struct definitions)

| tier | vftable (.rdata) | master struct @idb_types.h | runtime struct @idb_types.h |
|---|---|---|---|
| Static | 0x0065B40C | :1203 | :1438 |
| Tree | 0x006674A4 | :22170 | :21889 |
| Animated | 0x0065AF00 | :14875 | :16039 |
| Doppleganger | 0x0065B100 | :21766 | :21468 |
| Moving | 0x0065B304 | :11025 | :6690 |
| Action | 0x0065ADFC | :10681 | :7421 |
| Combat_Base | 0x0065AFF8 | :6939 | :7316 |
| Missile | 0x0065B1FC | :21287 | :20933 |
| Combat (70) | 0x0066738C | :15690 | :16322 |
| Building (80) | 0x00667280 | :14163 | :9724 |

Factory identity: each tier's make_new_obj instantiates ITS OWN
tier's runtime class (Static -> RGE_Static_Object, ...; Missile ->
RGE_Missile_Object; Combat(70) -> TRIBE_Combat_Object; Building ->
TRIBE_Building_Object, via TRIBE_Building_Object ctor1 [with uID,
idb_functions.h:3907] or ctor2 [idb_functions.h:3911];
Doppleganger -> RGE_Doppleganger_Object).

## master_type discriminant registry (owner-supplied)

10 STATIC, 15 TREE (TRIBE_Master_Tree_Object — exactly 6 exports,
OWNED; see the tree note above), 20 ANIMATED, 25 DOPPLEGANGER,
30 MOVING, 40 ACTION, 50 COMBAT_BASE, 60 MISSILE, 70 COMBAT,
80 BUILDING. Enum RGE_MASTER_TYPE lives in
src/engine/object/RGE_Master_Static_Object.h (IDA did not export it).

## Other verified relations

- RGE_Base_Game ⊂ TRIBE_Game — derived extends by 53 fields
  (117 base + 53 = 170); vtbls 99/99 slots, names identical in order.
  Evidence: struct prefix idb_types.h:16759-16876 vs :17449-17619;
  vtbl slot names :16887-16985 vs :17627-17725. Verified 2026-11-14.
  Runtime note: the base_game global is IDB-typed `TRIBE_Game*`
  (battlegrounds_x2.c:43930) — IDA's own runtime-type evidence for
  base→derived access on this chain; repo slot type `RGE_Base_Game**`
  (common.h:102).

Verified-unrelated verdicts are recorded here as well (none to date);
they justify future cast hard stops.

## History

- 2026-11-15: the tree above replaced an earlier chat-only diagram
  whose second row wrongly implied a linear Missile -> Combat(70) ->
  Building(80) descent and whose Doppleganger connector was ambiguous.
  The disproof recorded above is the standing evidence.
