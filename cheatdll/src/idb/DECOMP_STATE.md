# DECOMP_STATE — session state for the decompilation workflow

Read this file FIRST, at session start. Companions: DECOMP_TEMPLATE.md
(workflow rules), DECOMP_HIERARCHY.md (verified class hierarchy), and
DECOMP_GAME_FACTS.md (owner-supplied game-level semantics).

Rules:
- Entries here are LEADS, never grounding. Anything used in code must be
  re-verified against the reference files and cited as if this file did
  not exist. The greps remain the authority.
- New entries are proposed in the conversation and written only on owner
  approval (DECOMP_TEMPLATE.md constraint 9).
- Every entry carries its citation and how to re-verify. Entries rot;
  the re-verify hint is the invalidation mechanism.

## Verified environment facts

- Reference file sizes (2026-11-16, after the BVector-correction regen):
  idb_types.h 37,087 lines; idb_functions.h 6,415 lines (6,403
  definitions); battlegrounds_x2.c 15,215,288 bytes / 464,785 lines;
  battlegrounds_x2.lst 64,357,608 bytes / 1,111,934 lines; common.h 109
  lines; common.cpp 314 lines.
  Re-verify: `(Get-Content <file> | Measure-Object -Line).Lines`
- idb_functions.h address-literal lookups are unique: an address like
  `0x00401970` matches exactly one line (verified 2026-11-14).
  Re-verify: `Select-String -Path cheatdll\src\idb\idb_functions.h -Pattern '0x00401970'`
- battlegrounds_x2.c: every function body is preceded by an
  `//----- (00XXXXXX) -----` address marker (verified at the get_sprite
  body, around :156005).
- battlegrounds_x2.lst: functions are delimited by
  `.text:00XXXXXX <name> proc near` through the matching `endp` line.
  Example: RGE_Master_Static_Object__get_sprite = [0x0048B8B0, 0x0048B910)
  (lst :237299ff). Extent excludes inter-function padding.
- Game .text = [0x00401000, 0x00648000): functionlist.cpp:9210 issues
  VirtualProtect(0x00401000, 0x00247000, ...). 0x401000 + 0x247000 = 0x648000.
- Patch call-site volume ~1,350 across cheatdll\*.cpp (grep of
  setHook/setHookCall/writeNops/write_nop_seq, 2026-11-14). Re-verify: same grep.
- No direct `*(TYPE*)0xADDR = ...` writes exist in cheatdll sources
  (verified 2026-11-14).
- No `enum` definitions in idb_types.h, common.h, common.cpp
  (verified 2026-11-14).
- Full patch-writer inventory (both layers, incl. `reloc` at
  conditions.cpp:871) lives in DECOMP_TEMPLATE.md, "Collision audit".

## Owner rulings

- 2026-11-14: All build configurations patch the same game binary.
  WriteProcessMemory vs the Voobly interface is a load-context detail
  only. No version conditioning in installers.
- 2026-11-14: Every replaced function is named `<Class>__<method>_new`,
  mandatory (canonical names are taken by idb_functions.h variables).
  Uniqueness fallback: if `X_new` itself exists (originals like
  TRIBE_Screen_Sed__command_new idb_functions.h:3561, decoder_input_new
  :6129 end in `_new`), escalate `X_new2`, `X_new3`, ... to first free
  name; cite the grep.
- 2026-11-14: `install_*` installer names verified collision-free
  (only a parameter `install_func`, common.h:90).
- 2026-11-14: Bake-in suggestions are report-only; applied only on
  explicit owner confirmation / OVERRIDE (template constraint 7).
- 2026-11-14: Subsystem directories are convenience-only. Reuse an
  existing `src/engine/` subsystem first; propose a new one with one-line
  reasoning. Approved derivations so far: (none ratified yet).
- 2026-11-14: All repository edits are approval-gated (template
  constraint 9), including this file and DECOMP_TEMPLATE.md.
- Pre-existing project conventions (observed, cited): __thiscall targets
  are replaced by `__fastcall f(Class* this_, DWORD dummy, args...)`
  (mousewheel.cpp:14, overlay.cpp:15, mpsetupscreen.cpp:171, land.cpp:4);
  engine globals use the pointer-to-slot convention `(*glob)->field`
  (aiactions.cpp:4, common.cpp:7).
- 2026-11-14: base_game typing mismatch is a known historical artifact.
  The IDB types the global as `TRIBE_Game*` (battlegrounds_x2.c:43930);
  the repo defines `RGE_Base_Game** const` (common.h:102, bound at
  common.cpp:7 = 0x006A3684). Ruling: ALL `base_game->field` in
  decompiled source translate to `(*base_game)->field`, with fields
  grounded via `RGE_Base_Game` (the repo's static type). Both structs are
  fully defined (idb_types.h:16758-16876 and :17448-17619; identical
  prefix, `world` at :16827/:17517 — see DECOMP_HIERARCHY.md). A
  decompiled access to a TRIBE_Game-only field (beyond RGE_Base_Game's
  body) cannot compile through the repo typing → Information Request.
- 2026-11-14: The functionlist.cpp profiler is effectively dead code:
  setFunctionListHooks() is block-commented (functionlist.cpp:9199-9216)
  and its only call site is commented out (dllmain.cpp:363). Footnote:
  if ever revived, it must skip engine-tree-owned addresses (its table
  already lists get_sprite @ functionlist.cpp:2111).
- 2026-11-14: RGE_Master_Static_Object::object_flags (idb_types.h:1269,
  unsigned int) bits 8-15 = object_civ — the civ the master object is
  defined for (owner ruling). Helper: get_object_civ(), inline in
  src/engine/object/RGE_Master_Static_Object.h.
- 2026-11-14: RGE_Master_Static_Object::master_type (idb_types.h:1206,
  unsigned __int8) discriminants, owner-supplied. Enum RGE_MASTER_TYPE
  in src/engine/object/RGE_Master_Static_Object.h: 10 STATIC, 15 TREE,
  20 ANIMATED, 25 DOPPELGANGER, 30 MOVING, 40 ACTION, 50 COMBAT_BASE,
  60 MISSILE, 70 COMBAT, 80 BUILDING. Corroborated by raw-value usage
  in advcheat.cpp:77 (>=70) and advtriggereffect.cpp:415-426. IDA did
  not export this enum (unknown enums export as int typedefs).
   Registry now canonical in DECOMP_HIERARCHY.md.
- 2026-11-18: PROJECT GOAL - full-binary replacement is the eventual
  goal; the owned tree grows to cover the game's hot loops. Compile-
  flag performance arguments are therefore framed PER FUNCTION (the
  scoped float_control(precise) brackets + the /Fa audits scale with
  the tree); project-wide flag changes are a future whole-tree
  decision, not a present-day simplification.

## Verified type hierarchy

MOVED 2026-11-15: all class-relationship facts (the master-object
family tree, the runtime-side tree, per-edge evidence tables, and the
base_game ⊂ TRIBE_Game relation) now live in DECOMP_HIERARCHY.md.
Future verified relations and verified-unrelated verdicts are recorded
THERE, not here.

## Open suspicions

(none open. Closed 2026-11-14: "base_game IDB address unverified" —
superseded by the base_game typing ruling; the repo definition is
authoritative.)

- Patch call-site volume: 2,552 active lines matched the full writer
  grep (Layer-1 names + `reloc(` + direct Write*/WriteProcessMemory;
  commented lines excluded), 2026-11-14. Re-verify: rerun the writer grep.
- Whole-repo address-bytes grep (e.g. `48B8|48B9`) is the decisive
  complement to per-site writer greps: it catches table-driven and
  macro-hidden targets. It located the dormant profiler reference to
  get_sprite (functionlist.cpp:2111) that per-site greps missed.

## Function ledger

- RGE_Master_Static_Object::get_sprite @ 0x0048B8B0 — cleaned in test run
  2026-11-14; gate PASSED (incl. cast gate: no casts in this function).
  Formal collision audit executed 2026-11-14: 2,552 active call sites
  scanned, 0 overlaps with [0x0048B8B0, 0x0048B910); civLetterSet table
  clear (civletter.cpp:4-80, targets 0x52xxxx-0x53Axxx); whole-repo
  address grep found only the dormant profiler (functionlist.cpp:2111,
  see rulings). Status: APPROVED 2026-11-14; WRITTEN: src/engine/object/
  RGE_Master_Static_Object.cpp, src/engine/object/install_object.cpp,
  src/engine/install_engine.cpp, plus vcxproj ClCompile entries.
  Subsystem `object` ratified — first entry of the engine tree.
  install_engine() wired into dllmain.cpp:765 by owner 2026-11-14
  (initial-setup path, logged before/after); declarations via
  install_engine.h (root, vcxproj :282).
  Updated 2026-11-14: object_civ helper (get_object_civ) and the
  RGE_MASTER_TYPE enum applied — `<= 0x46u` is now
  `<= RGE_MASTER_TYPE_COMBAT`; semantics unchanged.
- RGE_Static_Object::gbg_get_civ_override @ 0x0054B3E0 — cleaned
  2026-11-14; gate PASSED; range [0x0054B3E0, 0x0054B3F9). Collision:
  langdll.cpp:306 writeByte patches the imm8 of `cmp eax, 9` at
  0x0054B3E9 to CIV_COUNT + 1 — BAKED IN (owner-approved): replacement
  compares against CIV_COUNT + 1 (consts.h:3 = 15). The old patch
  writes into dead code after hooking (removal optional, owner-side).
  Coverage: ≥10 vftables reference this single implementation (lst
  XREFs from :986409), no named overrides exist; residual: unnamed
  overrides not fully enumerated. Status: APPROVED 2026-11-14;
  WRITTEN: src/engine/object/RGE_Static_Object.cpp;
  install_object.cpp updated; vcxproj entry added.
- RGE_Master_Static_Object sprite-accessor family batch (owner-approved
  2026-11-14): get_sprite2 @ 0x0048B920 [0x48B920,0x48B980),
  get_death_sprite @ 0x0048B990 [0x48B990,0x48B9F0), get_undead_sprite @
  0x0048BA00 [0x48BA00,0x48BA60) — lst extents; prototypes
  idb_functions.h:1896-1898. Bodies are exact clones of get_sprite
  (fields sprite2/death_sprite/undead_sprite @ idb_types.h:1218-1220);
  replacements use the object_civ helper and RGE_MASTER_TYPE_COMBAT.
  Gate PASSED ×3; writer scan 0 overlaps / 0 near-range; address grep:
  all three in the dead profiler table (functionlist.cpp:2112-2114,
  footnote per ruling); other hits substring false positives. WRITTEN:
  appended to src/engine/object/RGE_Master_Static_Object.cpp; installer
  gained three setHook calls.
- RGE_Master_Static_Object query-method batch (owner-approved
  2026-11-14): is_unit_in_group @ 0x0048BA70 [0x48BA70,0x48BA8F),
  is_unit_in_civ @ 0x0048BAA0 [0x48BAA0,0x48BABF), get_unit_civ @
  0x0048BAD0 [0x48BAD0,0x48BAD8). Callee object_group_to_unit_group
  resolved (idb_functions.h:5418, __fastcall free fn @0x005E3F40).
  Replacements use get_object_civ (double call kept in is_unit_in_civ,
  faithful to two BYTE1 reads). Gate PASSED ×3; writer scan 0 overlaps.
  WRITTEN: appended to RGE_Master_Static_Object.cpp; installer gained
  three setHook calls.
- RGE_Master_Static_Object::do_i_care_about_power @ 0x0048BCE0
  [0x48BCE0,0x48BD37) — owner-approved 2026-11-14 with directed
  restructure: sub/jz chains + 60-case jump table (off_48BD38, asm
  verified) rewritten as a switch over the decoded membership set
  {45,49,68,84,87,101,103,104,209,317,319,323,335,530,562,584};
  equivalence by exhaustive case analysis (pure function of id).
  Gate PASSED; writer scan 0 overlaps; profiler table hit
  (functionlist.cpp:2120, footnote per ruling). WRITTEN: appended to
  RGE_Master_Static_Object.cpp; installer gained one setHook call.
- RGE_Master_Static_Object ctor/dtor family batch (owner-approved
  2026-11-14): copy ctor @ 0x0048BD80 [0x48BD80,0x48BD9E),
  scalar_deleting_destructor @ 0x0048BDB0 [0x48BDB0,0x48BE17),
  ctor2 @ 0x0048BE20 [0x48BE20,0x48BE4A), ctor3 @ 0x0048BE50
  [0x48BE50,0x48BE7F), ctor4 @ 0x0048BE90 [0x48BE90,0x48BEBF),
  destructor @ 0x0048D200 [0x48D200,0x48D255). Gate PASSED ×6;
  writer scan 0 overlaps ×6; profiler table hits (functionlist.cpp
  :2121-2125, dead-code footnote). Grounding resolved: vftable at
  .rdata:0065B40C (typed artifact
  rge_master_static_object_vftable in RGE_Master_Static_Object.cpp);
  free_internal = binary _free (0x00632CCA); operator_delete_internal =
  binary operator_delete (0x00632B42) — both common.cpp:326-330.
  scalar_deleting_destructor deduplicated: calls destructor_new (the
  original had the destructor body inlined — asm-verified identical).
  ctor wrappers forward to the ORIGINAL setup/setup2/3/4 via
  idb_functions.h:1910-1913 constants (setups not replaced yet).
  WRITTEN: appended to RGE_Master_Static_Object.cpp; installer gained
  six setHook calls (total 16 for the class).
- RGE_Master_Static_Object::get_string_id @ 0x0048BAE0
  [0x48BAE0,0x48BBD9) + get_string_id2 @ 0x0048BBE0
  [0x48BBE0,0x48BCD5) — langdll-hybrid fold, owner-approved 2026-11-14.
  Collision audit: 11+11 overlaps, ALL setLangDllHooks patches
  (langdll.cpp:267-300), folded per the bake-in protocol; profiler
  table hits (dead-code footnote). Effective patched semantics
  reconstructed from thunks langdll_1..4, 1_1..4_1,
  cargoTrader_langdll_1/2 (langdll.cpp:6-176): two-digit civ-suffix
  parse of sprite->name (owner context: suffix = civ index), worker
  stride 2→9, bases 100140→100136 / 101140→101136, commander/cargo/
  temple bases 100200/100210/100220 → 99937/99957/99917 and
  101200/101210/101220 → 100937/100957/100917, cargo condition
  object_group==45 → id==931. a2 semantics verified from call sites:
  0-based civ index, -1 = derive from sprite-name suffix
  (TribeTechHelpScreen::make_all_zones pushes [local-player-or-object
  civ byte @+0x161] - 1; UI callers push -1). RGE_Sprite resolved
  (idb_types.h:2972-3007): sound_list pointer directly precedes
  char name[25] — original *((char*)&sound_list+strlen(name)+3) ≡
  name[strlen(name)-1]. RESTRUCTURE (owner-directed): readable form —
  civ hoisted, named table locals, id-switch in get_string_id;
  get_string_id2 keeps mixed-key if-chain (documented). WRITTEN:
  appended to RGE_Master_Static_Object.cpp; installer gained two
  setHook calls. FOLLOW-UP applied 2026-11-14: all setLangDllHooks
  patches for the twins commented in place with reasons (langdll.cpp;
  process: comment out, never delete); the now-dead writeByte 0x54B3E9
  (dead since the gbg_get_civ_override hook) retired the same way.
  medic_worker @0x54BA2B remains live (target not replaced).
- RGE_Map::set_map_visible @ 0x00490650 [0x490650,0x49065A) +
  set_map_fog @ 0x00490660 [0x490660,0x49066A) — owner-approved
  2026-11-14. Gate PASSED ×2. Fields: map_visible_flag/fog_flag
  unsigned __int8 (idb_types.h:2927-2928). Writer scan 1 overlap each
  = dead profiler table (functionlist.cpp:2159-2160). WRITTEN:
  src/engine/map/RGE_Map.cpp + install_map.cpp wiring + vcxproj.
  Post-write verification: RGE_Map 2 defs / 2 refs identical.
- RGE_Map::clear_map @ 0x00490670 [0x490670,0x4906D0) — owner-approved
  2026-11-14. Gate PASSED. Pure forwarder composition: new_map (replaced
  — routes through the hook), set_terrain (idb_functions.h:1997,
  0x00495F80 — signature matches arg-for-arg incl. int→i16 narrowing),
  reset_object_count (:6003, 0x61B4D0), reset_player_visible_maps
  (:6038, 0x61CFF0). Writer scan 0 overlaps; address grep = dead
  profiler table (functionlist.cpp:2161). WRITTEN + post-write
  verification: RGE_Map 4 defs / 4 refs identical.
- RGE_Master_Static_Object::modify @ 0x0048D800 [0x48D800,0x48D8D0),
  modify_delta @ 0x0048D8D0 [0x48D8D0,0x48D9C0), modify_percent @
  0x0048D9C0 [0x48D9C0,0x48DAA4) + jump tables to 0x48DADE —
  owner-approved 2026-11-14. Gate PASSED ×3 (conversion semantics
  asm-verified: __ftol float→int32 + narrow store — the pseudocode's
  (__int64) casts are a rendering artifact; template rule added).
  Writer scan 0 overlaps ×3; profiler table hits (functionlist.cpp
  :2134-2136, dead-code footnote). WRITTEN: appended to
  RGE_Master_Static_Object.cpp; installer gained three setHook calls
  (total 19 for the class). Call-site context (protocol
  step 13): exactly one direct caller — RGE_Master_Animated_Object::modify
  @0x4889E0 delegates (flag != 5 → base call; flag == 5 handled
  derived-side); Moving::modify delegates to Animated; primary
  dispatch is the vtable slot in RGE_Master_Static_Object__vftable —
  the entry hook covers all virtual traffic.
- RGE_Master_Static_Object::check_placement @ 0x0048E180
  [0x48E180,0x48EA40) — owner-approved 2026-11-14 after a three-round
  verification plus one owner-triggered post-write re-verification pass
  (C2108 report). The largest function: ~430 pseudocode lines, ~190 asm.
  Gate PASSED; all six phases instruction-level decoded: bounds (7);
  center-tile terrain (raw position tile, i16-truncated, asm loc_48E2C4);
  terrain-requirement scans as a unified perimeter walk over a scan
  rectangle that DEGENERATES to the position tile when the center gate
  (center_tile_req1/2 >= 0) is active, else the box perimeter (the
  originally written box-edge scan in both modes was structurally wrong,
  caught by the re-verification, corrected on approval); per-tile
  passability/visibility/flatness; visibility accumulation (5); object
  collision scan (6 + obstructionObject).
  RETRACTIONS banked from the re-verification: (a) passability is
  <= 0.05f, not < — test ah,41h = C0|C3; the "asm C0 idiom -> <" ruling
  was a misdecode; same for the phase-6 radius_z <= 0 gate; the true
  C0-only (<) idioms are the phase-1 left<0 / top<0 bounds and the AABB
  fabs tests (0x48E96A/0x48E981); (b) the phase-3 terrain_type == 0xFFFF
  compare is asm-present but DEAD (byte terrain_type <= 255 can never
  equal i16 0xFFFF) — the pseudocode correctly omits it, superseding the
  "decompiler-dropped" claim; (c) the reported-object walk-order judgment
  call is RETIRED — the replacement matches the original's row-major
  nesting exactly. Additional re-verification corrections (owner-approved,
  applied): diagonal RHS is int math on ftol(top) (a float `rx - top`
  diverges for fractional row); elevation_flag==3 window [corner-1,
  corner+1] from map[ftol(bottom)][ftol(left)].height computed pre-loop,
  applied only on the left column or bottom row (the first written form
  was a provable no-op); phase-6 header: selected radius_x/y <= 0 -> 0
  gate, scan rect = +-8 tiles around the raw position clamped to the map
  (NOT the placement box — the earlier "verified" box walk was a
  misdecode), group 27 persistently WRITES this->radius_z = 1.0 (real
  field store, asm 0x48E7FD — gameplay-visible side effect, present in
  the original). PROVEN: phase-4 walk bounds are gate-independent
  (ftol(left/right/top/bottom) into var_30/var_34/var_1C/var_40, asm
  0x48E268-0x48E2A0; the center-gate branches touch only the phase-3
  registers var_18/3C/44). Stack-slot reuse map (IDA naming trap):
  [esp+58h+construction_radius_flag] = left float -> terrain_req_found
  -> visibility_found -> phase-6 row counter — the apparent "flag gate"
  at loc_48E543 is the written `if (!terrain_req_found) return 1;`.
  Callees grounded: world->terrains (RGE_Game_World), map_row_offset
  (0xBF18 in RGE_Map), prog_mode @+0x1DC, player->visible @+0x88,
  base_game prog-mode compare == 7, get_visible call at 0x48E658 (routes
  through our hook). Installer: 32 hooks for the class (defs/refs
  verified identical; the earlier "total 24" was a stale count).
- RGE_Master_Static_Object::draw @ 0x0048ED70
  [0x48ED70,0x48EF30) — owner-approved + written 2026-11-14. 448 bytes,
  __thiscall, 8 stack args, void. Selection-outline renderer: sprite
  null-guard -> RGE_Sprite__draw(facet, frame, x, y, sx = x, sy = y,
  player_color, render_area, min_draw_level 0x0A, debugColumn 0) ->
  when draw_frame != 0: project the outline rectangle corners
  (+/-outline_radius_x @196, +/-outline_radius_y @200; y negated on
  corners 1 and 4 exactly as the original) via RGE_Map::get_point on
  (*base_game)->world->map, then two DClipInfo_List::AddGDINode nodes
  into SDI_List: +1-offset pass with DrawColor 0 (shadow outline),
  exact pass with frame_color; GDI_Type 1, DrawLevel 0x0B, Object_ID
  = *SDI_Object_ID both passes. Field groundings: outline_radius_x/y
  @196/200 and RGE_Base_Game::world @0x420 mechanically verified by
  the struct walker calibrated on recyclable@188 / struct total 0xD0 /
  prog_mode@476; world->map @0x34 hand-counted = asm [ecx+34h].
  Callee prototypes all pre-exist: RGE_Sprite__draw (idb_functions
  .h:3759), RGE_Map__get_point (:2000), DClipInfo_List__AddGDINode
  (:5922); SDI_List/SDI_Object_ID pre-exist in common.h:117/124.
  Documented restructure: the original reuses the facet/frame/
  player_color/render_area/x/y param stack slots as get_point out
  buffers; replacement uses dedicated locals (params consumed before
  reuse -> identical behavior). Vtable note: the Master_Action_Object
  and Master_Animated_Object vftables point AT 0x48ED70 (they inherit
  base draw), so the entry hook covers their dispatch too. WRITTEN +
  post-write verification (33 defs/refs identical).
- RGE_Master_Static_Object::setup @ 0x0048BED0
  [0x48BED0,0x48C223) — owner-approved + written 2026-11-14. 818 bytes,
  __thiscall, 1 stack arg (retn 4), returns 1. Template applier: ~60
  field copies from other_object (master_type forced to 10 = static),
  the id->copy_id->base_id triple from one source read, 3-element
  attribute arrays, deep-copied damage-sprite table (calloc(count, 8)
  — sizeof(RGE_Damage_Sprite_Info) = ptr(4) + u8 + u8 rounded to
  alignment 4 = 8, asm stride shl eax,3 verified), getstring-duplicated
  name/gbg_name2 (gbg guarded on source non-null, name unconditional).
  Copied-offset SET audited instruction-by-instruction (the compiler
  scheduled copies out of source order — sets reconcile exactly).
  RESTRUCTURE documented: delta-pointer attribute walk -> indexed
  copies; damage-loop i16 counter vs u8 count <= 255 -> int loop.
  CENSUS CORRECTIONS (mechanical walker, calibrated on recyclable@188/
  total 0xD0/prog_mode@476): selected_sound@72, created_sound@76,
  death_sound@80, damage_sound@84, death_spawn_obj_id@88 (word),
  sort_number@90 (byte), can_be_built_on@91, attribute_rot@144,
  multiple_attribute_mod@148, attribute_flag@152, map_color@160,
  attack_reaction@161, convert_terrain_flag@162, object_flags@172,
  help_string_id@176, help_page_id@180, hotkey_id@184; available@98 /
  disabled@99 are real fields (the mid-audit "padding @99" alarm was a
  census misread). getstring prototype idb_functions.h:2281
  (__fastcall char**/char*). Caller note: derived constructors
  (Animated/Action/Moving) call all four setups — hook covers base
  address; derived setup overrides are separate functions. WRITTEN +
  post-write verification (34 defs/refs identical).
  TEMPLATE AMENDMENT (owner-triggered 2026-11-14, setup2 review): the
  sizeof discipline covered allocation sizes only; structured-I/O size
  arguments (the 3rd parameter of rge_read/rge_write) had no rule and
  were written as literals on the read side. New template rule (both
  directions, hard stop): rge_read/rge_write size arguments must be
  sizeof(target expression); fscanf carries widths in the conversion
  string (audited against targets); value-sized transfers pass the
  length variable; a deviating width keeps the literal WITH an asm-
  cited comment at the call; an unverified width is a HARD STOP
  (applied: setup2's deliberate 2-byte damage_percent spill, asm
  0x48C622-0x48C643). Write side was already sizeof-form in the owned
  save/small_save hooks (verified by tree-wide grep). setup2_new
  retro-fixed: 77 read widths converted to sizeof, 1 documented
  exemption.
  AMENDMENT 2 (same review, owner-directed): deviations from mandated
  mechanical forms are now a formal class — each must be presented as
  a clearly-flagged review item (mechanical form vs binary behavior vs
  asm evidence) and receive explicit owner approval BEFORE the code is
  written; silent deviations are gate violations. Retroactive status:
  the setup2 spill was presented in the setup2 review round and
  approved with the function — it satisfies the new protocol.
- RGE_Master_Static_Object::setup4 @ 0x0048CDF0
  [0x48CDF0,0x48D200) — owner-approved + written 2026-11-14 (with the
  sizeof correction applied pre-write). 1040 bytes, __thiscall, 4
  stack args, returns 1. Cross-reference loader: stream-reads
  id/copy_id/base_id/object_group (group 3->2 fixup), then
  list[copy_id] — UNCONDITIONAL sign-extended index, no negative guard
  (faithful; the asm re-loads list[copy_id] per copy, a compiler
  artifact the other-local replaces). Template SUBSET copy when the
  referenced master exists: 54 scalars + 3-array attribute loop +
  damage deep-copy + name/gbg getstrings; NOT copied: hp, los,
  obj_max, radius_x, radius_y, hotkey_id. The 10-read tail is
  UNCONDITIONAL (runs on the null-reference path too) and overwrites
  several copied fields (available, disabled, attribute_max_amount,
  attribute_amount_held[0] only — later write wins, code order
  replicates). sprites/sounds params unreferenced by the original
  (kept for ABI). Copied-offset set audited vs asm: identical. All
  widths sizeof(target) per the structured-I/O rule. WRITTEN +
  post-write verification (37 defs/refs identical).
- CLASS COMPLETE: RGE_Master_Static_Object — 37 hooks, every owned
  function in [0x48B8B0,0x48EF50): getters/string-id family, power,
  ctor/dtor family, setups x4, make_new_obj/make_new_master, copy
  pair, modify trio, save pair, check_placement, make_available,
  alignment pair, draw, help pair, calc_base_damage_ability. File:
  cheatdll/src/engine/object/RGE_Master_Static_Object.cpp (vcxproj
  entry present). In-game validation still owed: unit placement
  returns, DAT/binary/text loads, civ-suffix strings, selection
  outlines (draw), tech help screens.
- HOOK-ROUTING RULING (owner-corrected 2026-11-14, Animated review):
  setHook patches function entries IN PLACE (5-byte jmp rel32) — there
  is no trampoline. The idb_functions constants hold the entry
  addresses themselves, so calling through an idb constant at runtime
  executes the patch and reaches the replacement: NO divergence, one
  extra jmp. Calling the _new replacement directly is behaviorally
  identical. Precondition: entries must be hooked with the jmp form
  (setHook), not call-form (setHookCall) — a call-form patch at a
  function entry would return into mid-function bytes. Consequence:
  base-class calls in derived replacements may faithfully use the idb
  constants (the asm's actual call targets) without cross-file extern
  declarations of _new functions. Retroactive: check_placement's
  get_visible_new fold is behaviorally identical under this analysis
  (left as written, owner-informed).
- TEMPLATE AMENDMENT (owner-directed 2026-11-14, Animated/Moving
  review): base-pointer indexed field access (the v6[1].field pattern)
  is IDA's rendering of a derived-class field it cannot type. New rule:
  compute the byte offset (N*sizeof(base) + offsetof(field, base)),
  match it to a named field of the derived struct via the mechanical
  walker, rewrite as the named field, present the mapping in the
  review artifact; HARD STOP when no sane expression can be deduced.
  Never transcribe the expression literally; never guess unconfirmed
  names. Companion rule: re-typed pseudocode requests — for tricky
  cases, request an updated dump from the owner with the suspected
  mis-typed variables, their current types, and the deduced real
  types. First application: Moving::setup4's v6[1].* accesses (pending
  decode; the 8 mappings are single-step and setup2-width-corroborated,
  so manual correction is in protocol — any ambiguity escalates to the
  re-typed pseudocode request).
- RGE_Master_Animated_Object::setup/setup2/setup3/setup4
  [0x488760,0x488859) — owner-approved + written 2026-11-14. NEW CLASS
  FILE: src/engine/object/RGE_Master_Animated_Object.{h,cpp} +
  install_object.cpp wiring + vcxproj ClCompile entry. Four thin
  derived overrides (0x30/0x40/0x50/0x40 bytes): base Static setup
  call -> master_type = RGE_MASTER_TYPE_ANIMATED (0x14, owner enum) ->
  the SINGLE Animated extension op -> return 1. Extension field:
  float speed @208 (flattened struct mirrors the base layout through
  outline_radius_z @204 then speed; total 0xD4, consistent with
  Doppleganger masters allocating 0xD4). Base calls route through the
  idb constants per the HOOK-ROUTING RULING (no cross-file externs of
  _new functions). setup3's extension read is fscanf "%f"; the others
  rge_read 4 bytes; setup copies speed from other_object. Caller
  coverage: RGE_Master_Doppleganger_Object derives from Animated (no
  extra master fields, vfptr swap only) and its constructors call
  these Animated setups — hooks cover Doppleganger loading traffic.
  WRITTEN + post-write verification (4 defs/refs identical in the new
  file; install wiring + vcxproj resolved; no literal-width I/O; no
  pointer arithmetic). Post-write verification ALSO includes the
  return-type inventory sweep: every replacement's declared return is
  compared against its idb prototype's return (setups = int, ctors =
  the owning class pointer, destructors = void, make_new_obj =
  RGE_Static_Object*, make_new_master = RGE_Master_Static_Object*).
  First run caught: Missile ctor3/ctor4 declared `int` instead of
  RGE_Master_Missile_Object* — fixed; all other returns verified
  against their idb prototypes (setups int x15, ctors class-pointer
  x10, Static inventory int/u8/void/void*/RGE_Sprite*/RGE_Static_
  Object*/RGE_Master_Static_Object*).
- RGE_Master_Moving_Object::setup/setup2/setup3/setup4
  [0x48AFE0,0x48B380) — owner-approved + written 2026-11-14. NEW CLASS
  FILE: src/engine/object/RGE_Master_Moving_Object.{h,cpp} + wiring +
  vcxproj entry. Derived overrides (0xB0/0x120/0xF0/0xE0 bytes): base
  Animated call (idb constants per the HOOK-ROUTING RULING) ->
  master_type = RGE_MASTER_TYPE_MOVING (0x1E) -> Moving extension
  block + turn-radius initialization + the virtual dispatch
  vfptr->setTurnRadiusOffTurnSpeed (slot 0x64 = vtbl entry 25,
  position-verified idb_types.h:14962+25=14987). Extension fields
  walker-confirmed (calibrated on master_type@4/selected_sound@72/
  outline@196-204/speed@208): move_sprite@212, run_sprite@216,
  turn_speed@220, size_class@224, obj_trail_id@226, obj_trail_options
  @228, obj_trail_spacing@232, move_algorithem@236, mTurnRadius@240,
  mTurnRadiusSpeed@244, mMaximumYawPerSecondMoving@248,
  mStationaryYawRevolutionTime@252, mMaximumYawPerSecondStationary
  @256; total 0x104. FLT_MAX constant = SINGLE_MAXIMUM_VALUE
  (3.4028235e38f). setup2/setup3 param-slot reuse (sounds slot ->
  move_sprite ix, sprites slot -> run_sprite ix; setup3 also infile ->
  move ix, in_id -> size_class, sprites -> obj_trail_options, sounds
  -> move_algorithem) — named locals in the replacements. setup4:
  list[copy_id] re-typed per the base-pointer rule (v6[1].* = the
  referenced master's Moving extension fields @212-236, walker-
  confirmed; presented mapping in the review round), unconditional
  index (faithful), and the final turn_speed stream read OVERWRITES
  the copied value (later write wins). setup2 streams all five m*
  floats (no FLT_MAX init on that path). First exercise of the
  base-pointer derived-field correction rule — all 8 mappings
  single-step, walker-confirmed, presented above; no re-typed
  pseudocode request needed. WRITTEN + post-write verification
  (4 defs/refs identical; wiring resolved; no literal-width I/O).
- RGE_Master_Action_Object::setup/setup2/setup3/setup4
  [0x487B50,0x487F2C) — owner-approved + written 2026-11-14. NEW CLASS
  FILE: src/engine/object/RGE_Master_Action_Object.{h,cpp} + wiring +
  vcxproj entry. Derived overrides (0xB0/0x110/0x120/0x100 bytes): base
  Moving call (idb constants per the HOOK-ROUTING RULING) -> master_type
  = RGE_MASTER_TYPE_ACTION (0x28) -> the Action extension block + the
  task-list machinery. Extension fields walker-confirmed (20 targets,
  calibrated; total 0x12C): tasks@260, default_task@264,
  search_radius@268, work_rate@272, gbg_work_rate_2@276, drop_site@280,
  backup_drop_site@282, task_by_group@284, command_sound@288,
  move_sound@292, run_pattern@296. NEW GROUNDINGS: world->tasks @396
  (asm +0x18C + corrected-walker adjacency — the earlier Game_World
  walk was shifted by the unresolved eSaveLoadModes_e typedef); RGE_
  Task_List = {vfptr@0, RGE_Task** list@4, __int16 list_num@8};
  RGE_Task.is_default @+4 (u8). setup3's default-task search: the
  exhausted-loop path returns 1 EXPLICITLY (asm 0x487E0F — the
  pseudocode's bare return; was a rendering); RESTRUCTURE documented:
  the list_num <= 0 guard (asm 0x487DF0-0x487DF6) collapses into the
  loop condition (provable — same path set), per the redundant-guard
  elimination rule. setup2/setup3 IDA "possibly undefined" args were
  tracking artifacts — the asm forwards the real sounds/in_id; setup2's
  two sound indices resolve against the SOUNDS array for both (asm ebp
  both); setup3's %hd scratches are the in_id/sprites/infile/sounds
  param slots (named locals in the replacement). setup4: list[copy_id]
  re-typed per the base-pointer rule (v6[1].* = Action extension
  @264-296, walker-confirmed; mapping presented in the review round),
  unconditional index (faithful); tasks = world->tasks[id] OVERWRITES
  the base-copied value, and the stream tail re-reads search_radius/
  work_rate with gbg mirroring again — later-write-wins order
  replicated. Protocol notes: first redundant-guard collapse under the
  new rule (setup3, RESTRUCTURE comment with asm addresses); pointer-
  walk check clean. Owner catch during review: the presented setup3
  walked a pointer (task_ptr) — replaced with indexed access before
  write, and the pointer rule now binds output code explicitly.
   WRITTEN + post-write verification (4 defs/refs identical; wiring
  resolved; no literal-width I/O; pointer-walk clean; guard scan
  clean).
- RGE_Master_Action_Object::RGE_Master_Action_Object2/3/4
  [0x4879E0,0x487B44) — owner-approved + written 2026-11-14. Derived
  constructor trio (0x70/0x80/0x80 bytes incl. SEH frames). The
  CLEANEST tier: NO inline initialization (the Action extension has no
  ctor defaults — the setups set everything incl. the tasks binding).
  Body: base Moving ctorN with do_setup = 0 (idb constants -> our
  Moving ctor replacements per the HOOK-ROUTING RULING), Action vfptr
  store (rge_master_action_object_vftable = 0x0065ADFC, cpp-local
  typed constant), if (do_setup) the derived setup (Action::setupN —
  load-bearing guard, no collapse). SEH fold: unwind chunks
  0x641F50/0x641F70/0x641F90 = mov ecx, [ebp-10h] / jmp RGE_Master_
  MOVING_Object__destructor — the immediate base level, continuing the
  ladder (Static <- Animated <- Moving <- Action unwinds). Caller
  coverage: Player__load_master_object/load_object + the Combat ctor
  chain. PROTOCOL NOTE: the pointer-walk grep matched ++index (the
  int loop counter in setup3's default-task search) — verification
  refined: each ++X match is type-checked (pointer-typed increment =
  violation; int counter = fine). WRITTEN + post-write verification
  (7 defs/refs identical; installer gained 3 hooks; no literal-width
  I/O; pointer-walk clean after type-check; 3 SEH folds present).
- RGE_Master_Combat_Object::setup/setup2/setup3/setup4
  [0x488D30,0x489630) — owner-approved + written 2026-11-14. NEW CLASS
  FILE: src/engine/object/RGE_Master_Combat_Object.{h,cpp} + wiring +
  vcxproj entry. Combat_Base tier (master_type = RGE_MASTER_TYPE_
  COMBAT_BASE, 0x32): base Action call -> the Combat extension
  (extension offsets 300-376 walker-confirmed, 36 targets total with
  the base chain calibrations; total 0x17C): fight_sprite@300,
  base_armor@304, armor_num@306, armor@308, weapon_num@312,
  weapon@316, defense_terrain_bonus@320, weapon_range@324,
  area_effect_range@328, area_effect_level@332, speed_of_attack@336,
  missile_id@340, base_hit_chance@342, break_off_combat@344,
  fire_missile_at_frame@346, weapon_offset_x/y/z@348/352/356,
  minimum_weapon_range@360, orig_armor@364, orig_weapon@366,
  orig_weapon_range@368, orig_speed_of_attack@372,
  missed_missile_spread@376. NEW TYPE VERIFIED:
  RGE_Armor_Weapon_Info = {i16 type@0, i16 value@2} — size 4,
  calloc(count, 4) verified; setup's table deep-copy is the qmemcpy
  intrinsic (rep movsd + tail movsb) -> memcpy in the replacement;
  setup2/setup4 use BULK rge_read (count * sizeof(element)). setup3:
  18-conversion fscanf (%hd scratches = the in_id/infile/sounds param
  slots + one local — named locals in the replacement), the two
  "%hd %hd" table read loops, and the orig_* DERIVATION chains in
  indexed form: orig_armor = max value among type==4 armors from -1
  falling back to base_armor then max over all values; orig_weapon =
  max among type==4||3 weapons from 0 (loop-invariant null check
  hoisted) falling back to max over all values. setup4: 20-field
  v6[1] batch re-typed per the base-pointer rule (mapping presented
  in the review round), NO table copies (tables stream-read fresh),
  tail re-reads weapon_range twice + overwrites base_armor/copy
  values — later-write-wins replicated. IDA "v10 possibly undefined"
  base-call arg = tracking artifact (asm forwards the real param).
  WRITTEN + post-write verification (4 defs/refs identical; wiring
  resolved; no literal-width I/O; pointer-walk clean after type-check;
  3 SEH folds present).
- BASE_GAME PATTERN FIX (owner-reported, Action cpp line 71): the
  three task-binding sites in Action setup2/3/4 used bare
  base_game->world (double-pointer through the ** const) instead of
  the canonical (*base_game)->world->tasks[id]; all three fixed + the
  Action comment updated. Tree-wide sweep added to the protocol:
  lookbehind grep (?<!\*\))base_game-> — residual hits are
  violations; one Static comment rephrased to keep the sweep at zero.
  Template rule added: base_game double-pointer deref (canonical
  (*base_game)->field; ground: world @0x420, map @0x34, tasks
  @0x18C). WRITTEN + post-write verification (sweep = 0 bare / 11
  canonical sites).
- RGE_Master_Animated_Object::make_new_obj/make_new_master/copy_obj
  [0x488870,0x4889C0+copy_obj tail] — owner-approved + written
  2026-11-14 (the installer hooks were initially misplaced into the
  Missile installer by an anchoring slip — caught and corrected
  before verification). make_new_obj @0x488870 (0xD0 bytes, SEH
  chunk 0x642090 = operator_delete frees the runtime allocation):
  recyclable -> recycle path -> guarded vtable dispatch (slot @0x74)
  -> return; else operator_new(0xB8) + RGE_Animated_Object runtime
  ctor (do_setup = 1, idb constant @538 -> 0x004A0540).
  ALLOCATION VERIFIED: sizeof(RGE_Animated_Object) = 184 = 0xB8
  (walker 148 + hand-resolved LOS_ExploreInfo ei = 36: ptr +
  typedef-int + 7 ints; LOSLastActions = typedef int per
  idb_types.h:1418). make_new_master @0x488940 (0xB0 bytes, SEH
  chunk 0x6420B0: operator_delete on the allocation + a destructor-
  chained secondary entry folding into the same filter):
  operator_new(0xD4) — sizeof(RGE_Master_Animated_Object) = 212 =
  0xD4 walker-verified — base Static COPY-CTOR with do_setup = 0,
  Animated vfptr, base Static SETUP (template copy from this),
  master_type = 20, then the Animated extension copy
  speed = this->speed. BASE-POINTER CORRECTION: the IDA renderings
  v2[1].vfptr = LODWORD(this->speed) (make_new_master) and
  LODWORD(this->speed) = source[1].vfptr (copy_obj) are the
  Animated extension field @208 mis-typed through the base pointer
  — corrected to named speed accesses per the base-pointer rule.
  copy_obj: base Static copy_obj + speed copy, no SEH (plain
  function), retn 8. Runtime ctor constant: RGE_Animated_Object__
  RGE_Animated_Object (idb_functions.h:538). Caller coverage: the
  Doppleganger make_new_master calls Animated ctor1 + setup
  (do_setup = 0 + explicit setup) — the Animated hooks cover its
  loading traffic. Scope note: the Animated MODIFY override
  @0x4889E0 (flag == 5 -> speed = amount) was captured in the same
  pull — future modify-override family. WRITTEN + post-write
  verification (12 defs/refs identical; installer corrected;
  collision sweep: functionlist.cpp table references the addresses
  but is a DEAD module per owner disposition — no active collision).
  [0x48B390,0x48B6xx] — owner-approved + written 2026-11-14 (the
  trio needed TWO review rounds: the first presentation blocked
  make_new_obj on the allocation hard stop; the owner asked for the
  blocking problem re-laid-out and the walk re-verified).
  make_new_obj @0x48B390 (0xD0 bytes, SEH fold 0x642350): recycle
  path (recyclable -> recycle_object_in_to_game -> guarded vtable
  dispatch slot @0x74 -> return) else operator_new(0x13C) + the
  RUNTIME RGE_Moving_Object ctor (do_setup = 1). ALLOCATION
  VERIFIED after re-walk: sizeof(RGE_Moving_Object) = 316 = 0x13C
  — sequential hand-walk over idb_types.h:6690-6767; the FIRST
  attempt (walker 288 + ei 36 = 324) was INVALID ARITHMETIC: the
  walker skips unresolved fields without advancing the offset,
  shifting all subsequent fields — bolting the size at the end
  double-counts. Corrected sequential walk lands exactly on 316.
  Runtime ctor constant: RGE_Moving_Object__RGE_Moving_Object
  (idb_functions.h:2122 -> 0x004A0540).
  make_new_master @0x48B5B0 (0xB0 bytes, SEH fold 0x642370):
  operator_new(0x104) (sizeof walker-verified), base ANIMATED
  ctor1 (idb constant @1808 -> 0x488540, unhooked — executes the
  original base construction faithfully), the five m* defaults
  INLINE (asm 0x48B5F3-0x48B620, offsets 0xF0-0x100), Moving vfptr,
  then the DERIVED setup (Moving::setup — template copy from this,
  overwrites the m* defaults and sets master_type = 30). No
  master_type store in the factory. Write-time self-catch: a
  spurious this_->vfptr store was drafted and removed (the asm
  stores only new_master's vfptr).
  copy_obj @0x48B660 (~0x40 bytes, no SEH): base ANIMATED
  copy_obj (idb constant @1820 -> 0x4889C0, HOOKED — routes into
  our replacement per the routing ruling: Static copy + speed
  copy), then the eight Moving extension copies via the source
  re-typed Moving* (source[1].* offsets 212-236 walker-confirmed:
  move_sprite/run_sprite/turn_speed/size_class/obj_trail_id/
  obj_trail_options/obj_trail_spacing/move_algorithem).
  WRITTEN + post-write verification (Moving file now 12 defs/refs
  identical; CRT sweep 0; pointer-walk 0; literal sweep 0).
- SESSION CLOSEOUT (2026-11-14): 7 master-class files
  (Static/Animated/Moving/Action/Combat/Doppleganger/Missile),
  89 hooks total (Static 37, Animated 12, Moving 12, Action 9,
  Combat 9, Doppleganger 5, Missile 5) — every master-class setup,
  constructor, destructor, deleting-destructor, make_new_obj,
  make_new_master, and copy_obj in the hierarchy owned and
  hook-routed. PENDING SCOPE (not yet decoded): the make_new_obj/
  make_new_master/copy_obj families for Action [0x487FA0/0x488070/
  0x4881F0], Combat [0x489680/0x489750/0x4897C0], Doppleganger
  [0x48A510/0x48A5F0/0x48A660], Missile [0x48A960/0x48AA30/
  0x48AAF0] — 12 functions, same ladder pattern expected; Building
  (80)/Combat (70) subclass scope confirmation; the derived modify
  overrides (Animated modify @0x4889E0 flag==5 -> speed captured;
  Moving modify/modify_delta/modify_percent @0x48B6E0/0x48B710/
  0x48B740 flag==6 -> turn_speed captured in pulls); the runtime-
  side classes (RGE_Moving_Object etc. — RGE_Static_Object 0xB4 and
  RGE_Animated_Object 0xB8 already walker-verified; RGE_Moving_
  Object walk was CORRECTED to 316 = 0x13C this session). BUILD
  VALIDATION: owner-reported builds green after each correction
  pass; quick tests reveal no discrepancy as of the destructor
  family write. Protocol additions this session: structured-I/O
  sizeof rule, base-pointer derived-field correction, re-typed
  pseudocode request, redundant-guard elimination, deviation
  presentation protocol, cross-class cast rule, C2177 note, CRT
  _internal rule, base_game deref rule, constructor structural
  audit — all in DECOMP_TEMPLATE.md.
- DERIVED DESTRUCTOR FAMILY (owner-approved + written 2026-11-14):
  6 destructors + 5 vector_deleting + 1 scalar_deleting (Moving has
  no vector_deleting export; its vftable uses the scalar variant)
  across the six derived class files — 12 hooks (each class file +2:
  Animated/Moving/Action/Combat 9 hooks each; Doppleganger/Missile 5
  hooks each). Plain destructors: vfptr store then base destructor
  call (idb constants -> hooked replacements per the routing ruling);
  no dummy param (no stack args, per the Static destructor
  precedent). Combat destructor: vfptr FIRST (asm order), then
  weapon/armor frees via free_internal (heap pairing per the CRT
  rule — the game frees these tables with its own free; allocations
  are calloc_internal), pointers zeroed, then the base Action
  destructor (called, not tail-jmp). Deleting-destructors: identical
  bodies — own destructor_new call (same class, no cast),
  operator_delete_internal on flags & 1, return this (the asm calls
  the game's operator_delete; the wrapper is the game-side deleter
  per the Static scalar_deleting precedent). SEH unwind funclets from
  the ctor rounds jmp into these destructors — hooked, the exception
  path routes through the replacements too. WRITTEN + post-write
  verification: defs/refs 37/9/9/9/9/5/5 (all matching per file);
  bare CRT 0 (free_internal present in Combat); pointer-walk 0 real
  (the alignment_new float accumulator remains the sole known benign
  grep match); base_game sweep 0 bare.
  PASS 2 (owner-reported, Animated cpp line 183): the destructor
  family was inserted AFTER the cross-class cast sweep, so Animated
  destructor_new's base call (RGE_Master_Static_Object__destructor)
  shipped uncased — fixed + all six files re-swept (0 uncased). The
  post-write checklist now enumerates the cast re-sweep explicitly:
  it must re-run after EVERY batch insert, not only once per file.
- RGE_Master_Doppleganger_Object::Object2/3/4
  [0x48A370,0x48A4FC) and RGE_Master_Missile_Object::Object2/3/4
  [0x48A6B0,0x48A9xx) — owner-approved + written 2026-11-14. TWO NEW
  CLASS FILES (RGE_Master_Doppleganger_Object.{h,cpp},
  RGE_Master_Missile_Object.{h,cpp}) + wiring + 2 vcxproj entries.
  MASTER-CONSTRUCTION PROJECT COMPLETE (see below).
  Doppleganger trio (25): base ANIMATED ctorN(do_setup=0) -> vfptr
  swap (rge_master_doppleganger_object_vftable = 0x0065B100) ->
  if (do_setup) { Animated::setupN; master_type = DOPPELGANGER };
  ctor3 adds recyclable = 1 (@188, asm 0x48A454); ctor4 adds
  fog_flag = 3 (@117, asm 0x48A4E4). Doppleganger masters = Animated
  masters with a vfptr swap, no extra fields (0xD4). SEH unwind ->
  ANIMATED destructor (chunks 0x642190/0x6421B0/0x6421D0 confirmed).
  Missile trio (60): base COMBAT ctorN(do_setup=0) -> vfptr
  (rge_master_missile_object_vftable = 0x0065B1FC) -> if (do_setup)
  { Combat::setupN; master_type = MISSILE; extension op }. Missile
  extension fields walker-confirmed (total 0x188): missile_type@380,
  targetting_type@381, missile_hit_info@382, missile_die_info@383,
  area_effect_specials@384, ballistics_ratio@388 (float). ctor2: six
  sizeof-form reads. ctor3: recyclable = 1 + fscanf "%hd x5 %f" (the
  %hd scratches = the do_setup/sprites/in_id/sounds/infile param
  slots — named locals; byte-narrowed). ctor4: list[copy_id] re-typed
  per the base-pointer rule (v7[1].object_flags bytes @552-555 ->
  missile_type/targetting/hit/die; help_string_id byte @556 ->
  area_effect_specials; help_page_id @560 -> ballistics_ratio);
  unconditional index (faithful); tail rge_read OVERWRITES
  targetting_type (later write wins). SEH unwind -> COMBAT destructor
  (chunks 0x642230/0x642250/0x642270). WRITTEN + post-write
  verification (3 defs/refs identical per file; wiring + 2 vcxproj
  entries resolved; no literal-width I/O; pointer-walk clean; 3 SEH
  folds per file).
- MASTER-CONSTRUCTION PROJECT COMPLETE: 7 class files
  (Static/Animated/Moving/Action/Combat/Doppleganger/Missile),
  20 setup hooks + 21 constructor hooks = 41 derived-family hooks
  plus the Static class's own 37 = the full master construction,
  DAT-load (binary + text + xref), and setup traffic of every master
  type in the game (10/15/20/25/30/40/50/60 and the Combat-subclass
  70/80 masters) routes through owned, hook-routed replacements.
  Remaining known-unowned master-family territory: the derived
  DESTRUCTORS (thin vfptr-store + base-call pairs, pattern
  established), make_new_obj/make_new_master overrides, copy_obj/
  copy_vars, and the Missile/Building ctor-level extension reads that
  live in constructors rather than setups (e.g. Missile_Object2's
  reads are inside the ctor — covered above; Building ctors pending
  scope).
- RGE_Master_Combat_Object::RGE_Master_Combat_Object2/3/4
  [0x488BC0,0x488D24) — owner-approved + written 2026-11-14. Derived
  constructor trio (0x70/0x80/0x80 bytes incl. SEH frames). The ladder
  holds exactly: NO inline initialization (the Armor/Weapon tables and
  scalars are the setups' job). Body: base Action ctorN with
  do_setup = 0 (idb constants -> our Action ctor replacements per the
  HOOK-ROUTING RULING), Combat vfptr store (rge_master_combat_object_
  vftable = 0x0065AFF8, cpp-local typed constant), if (do_setup) the
  derived setup (Combat::setupN — loads the Armor/Weapon tables and
  the scalar block). SEH fold: unwind chunks 0x6420F0/0x642110/
  0x642130 = mov ecx, [ebp-10h] / jmp RGE_Master_ACTION_Object__
  destructor — the immediate base level, ladder complete (Static <-
  Animated <- Moving <- Action <- Combat unwinds). if (do_setup)
  load-bearing (no collapse). Caller coverage: RGE_Master_Missile_
  Object2/3/4 and Player__load_master_object/load_object — the
  Missile tier routes through these hooks. Pointer-walk grep matched
  4 (the setup3 int loop counters ++i/j/k/m — type-checked fine).
  WRITTEN + post-write verification (7 defs/refs identical; installer
  gained 3 hooks; no literal-width I/O; 3 SEH folds present).
- RGE_Master_Moving_Object::RGE_Master_Moving_Object2/3/4
  [0x48AE10,0x48AFD5) — owner-approved + written 2026-11-14. Derived
  constructor trio (0x90/0xA0/0xA0 bytes incl. SEH frames). PATTERN
  DELTA vs Animated ctors: the five m* yaw defaults are initialized
  INLINE in the ctor (FLT_MAX x3 on mTurnRadiusSpeed/mMaxMoving/
  mMaxStationary @244/248/256, zeros on mTurnRadius/mStationaryYaw-
  RevolutionTime @240/252 — asm order F4/F8/100 then F0/FC), and the
  do_setup body calls the DERIVED setup (Moving::setupN — which
  re-reads all five m* floats from the stream, overwriting the
  defaults); NO master_type store in the ctors (the derived setups
  set it). Body: base Animated ctorN with do_setup = 0 (idb constants
  -> our Animated ctor replacements per the HOOK-ROUTING RULING),
  Moving vfptr store (rge_master_moving_object_vftable = 0x0065B304,
  cpp-local typed constant). SEH fold: unwind chunks 0x6422F0/
  0x642310/0x642330 = mov ecx, [ebp-10h] / jmp RGE_Master_ANIMATED_
  Object__destructor — the IMMEDIATE BASE's destructor level (the
  Animated dtor stores the Animated vfptr then chains to Static;
  contrast Animated ctors, whose unwind jumped straight to Static).
  Caller coverage: RGE_Master_Player__load_master_object/load_object
  call Moving ctor2/ctor3 directly (player-unit load path); Action
  ctor chain builds on these. WRITTEN + post-write verification
  (7 defs/refs identical in the class file; installer gained 3 hooks;
  no literal-width I/O; pointer-walk clean; 3 SEH folds present).
- RGE_Master_Animated_Object::RGE_Master_Animated_Object2/3/4
  [0x4885C0,0x48875A) — owner-approved + written 2026-11-14. Derived
  constructor trio (0x80/0x90/0x90 bytes incl. SEH frames), __thiscall,
  4/5/5 stack args, return this. THE SEH FOLD FAMILY BEGINS: every
  derived master ctor carries an MSVC ctor-unwind frame; funclet =
  mov ecx, [ebp-10h] / jmp RGE_Master_Static_Object__destructor
  (chunks 0x642030/0x642050/0x642070 — the Animated sub-object adds no
  resources beyond the Static destructor's frees); handler = standard
  __CxxFrameHandler dispatch, unwind-only scope. Folded per the
  make_new_obj pattern: __except (RGE_Master_Static_Object__destructor
  (this_), EXCEPTION_CONTINUE_SEARCH). Body: base Static ctorN with
  do_setup = 0 (idb constants -> hooked replacements), Animated vfptr
  store (rge_master_animated_object_vftable = 0x0065AF00, cpp-local
  typed constant per the Static precedent), if (do_setup) { Static
  setupN + the speed extension read (setup2/setup4: rge_read 4;
  setup3: fscanf "%f") + master_type = RGE_MASTER_TYPE_ANIMATED } —
  the extension op is INLINED in the ctors (the asm calls Static
  setupN directly, not Animated::setupN — faithful form kept). Note
  Animated::setup2/3/4 (also hooked) perform the identical extension
  op — behaviorally equivalent entry points; Doppleganger ctors call
  the Animated SETUPS explicitly while Moving/Action/Combat ctors use
   canonical sites).
- FUNCTIONLIST.CPP DISPOSITION (owner-directed: ignored as a collision
  source): functionlist.cpp is a DEAD EXPERIMENTAL PROFILING MODULE —
  setFunctionListHooks() is never called (dllmain:364 commented), its
  installation loop body is commented out, and the FUNCTION_HOOK
  call-count/total-time machinery is inert. Its `function_addresses[]`
  table INCLUDES every master-class address we hook (incl. 0x488870/
  0x488940/0x4889C0) — data only, no active writer. Not a collision.
- DEDUCED MASTER CLASS HIERARCHY (owner-requested write; evidence =
  the ctor/setup/dtor base-call chains + vftable stores + master_type
  discriminants, all asm-verified per class; flattened struct prefixes
  mirror base layouts, walker-calibrated; sizes = operator_new vs
  walker totals):
  SUPERSEDED 2026-11-15: the canonical, evidence-tabled tree now lives
  in DECOMP_HIERARCHY.md. The PENDING CONFIRMATION note below is
  RESOLVED by the TRIBE_ discovery (see the scope-update entry) — kept
  for history:
  RGE_Master_Static_Object (10, BASE) — 0xD0
  <- RGE_Master_Animated_Object (20) — 0xD4 (+speed@208)
  <- RGE_Master_Moving_Object (30) — 0x104 (+move_sprite@212,
     run_sprite@216, turn_speed@220, size_class@224, obj_trail_id@226,
     obj_trail_options@228, obj_trail_spacing@232, move_algorithem@236,
     mTurnRadius@240, mTurnRadiusSpeed@244, mMaxYawMoving@248,
     mStationaryYawRevTime@252, mMaxYawStationary@256)
  <- RGE_Master_Action_Object (40) — 0x12C (+tasks@260, default_task
     @264, search_radius@268, work_rate@272, gbg_work_rate_2@276,
     drop_site@280, backup_drop_site@282, task_by_group@284,
     command_sound@288, move_sound@292, run_pattern@296, gap@297-299)
  <- RGE_Master_Combat_Object (50, COMBAT_BASE) — 0x17C (+fight_sprite
     @300, base_armor@304, armor_num@306, armor@308, weapon_num@312,
     weapon@316, defense_terrain_bonus@320, weapon_range@324,
     area_effect_range@328, area_effect_level@332, speed_of_attack@336,
     missile_id@340, base_hit_chance@342, break_off_combat@344,
     fire_missile_at_frame@346, weapon_offset_x/y/z@348/352/356,
     minimum_weapon_range@360, orig_armor@364, orig_weapon@366,
     orig_weapon_range@368, orig_speed_of_attack@372,
     missed_missile_spread@376)
  <- RGE_Master_Missile_Object (60) — 0x188 (+missile_type@380,
     targetting_type@381, missile_hit_info@382, missile_die_info@383,
     area_effect_specials@384, ballistics_ratio@388)
  SIDE BRANCH: RGE_Master_Doppleganger_Object (25) <- Animated — 0xD4,
  no extra fields (vfptr swap only; ctor3 forces recyclable = 1,
  ctor4 forces fog_flag = 3).
  PENDING CONFIRMATION: master_type 70 (Combat) and 80 (Building)
  subclasses of Combat_Base — no separate setup/ctor exports found;
  Building/Combat masters likely construct via the Combat chain with
  the discriminant set by callers. Trees as runtime-side analogs:
  RGE_Static_Object (0xB4) <- RGE_Animated_Object (0xB8) <- ... (the
  runtime RGE_Missile_Object allocates 0x18C per Missile
  make_new_obj) — runtime-side decode pending.
  CHAIN EVIDENCE PER LEVEL: ctorN calls base ctorN(do_setup=0);
  setupN calls base setupN; dtor calls/chains base dtor (Combat's is
  a call with frees, the rest tail-jmp or call); each level stores
  its own vftable after the base construction; SEH unwind funclets
  jump to the IMMEDIATE BASE's destructor (ladder: Animated -> Static,
  Moving -> Animated, Action -> Moving, Combat -> Action, Missile ->
  Combat, Doppleganger -> Animated).
- COMPILE-DRIVEN CORRECTION PASS (owner-reported MSVC errors
  2026-11-14): (1) CROSS-CLASS CASTS — the flattened derived structs
  are unrelated C++ types; every cross-class base call needed an
  explicit C-style cast of this_ to the idb constant's parameter type
  (static_cast would reject unrelated types; C-style reinterpret
  semantics required). 60 sites cast across the six derived class
  files (Animated 13, Moving 10, Action 10, Combat 10, Doppleganger
  9, Missile 8 + 1 restored — see (3)); all casts derived-to-ancestor,
  layout-safe per the walker-calibrated prefix mirroring. Template
  rule added (explicit casts required; compatible-type caution;
  C2177 literal-range note). (2) FLT_MAX — the literal 3.4028235e38f
  exceeds FLT_MAX (3.40282346638e38) and MSVC rejects it (C2177);
  18 literals in Moving replaced with FLT_MAX. (3) SELF-CAUGHT during
  the cast pass: Missile ctor4 was MISSING its Combat::setup4 call
  (dropped at write time — the decode had it, the write lost it);
  restored with cast (the asm-vs-code call-set comparison this implies
  is now part of the ctor decode checklist). Post-fix verification:
  zero uncased cross-class calls in all six files; zero oversized
  literals; Missile setup4 present with cast.
  PASS 2 (owner-reported line 23): the other_object ARGUMENT of the
  base template-setup calls is also derived-typed — 4 more casts
  (Animated setup -> Static*, Moving setup -> Animated*, Action setup
  -> Moving*, Combat setup -> Action*); residual grep hit at
  Static_Object.cpp:312 verified compliant (same-class call, params
  already Static-typed). Cast sweep now covers every derived-typed
  argument position (this_ + other_object); infile/sprites/sounds/
  list are base-typed and pass uncast.
  PASS 4 (owner-reported): Missile ctor4 was MISSING the vfptr store
  AND the if (do_setup) guard — the do_setup body ran unconditionally
  and the vfptr stayed at the Combat vftable (wrong virtual dispatch
  for Missile masters). Restored per the approved structure; then a
  STRUCTURAL AUDIT of all 22 derived+base constructors across the
  seven class files (vfptr store present, do_setup guard present,
  body sequenced after the guard) — 22/22 PASS; special stores
  spot-checked for brace nesting (Doppleganger ctor3 recyclable /
  ctor4 fog_flag; Missile ctor3 recyclable+master_type / ctor4
  master_type — all inside their guards). Constructor structural
  audit script added to the post-write verification protocol.
  PASS 3 (owner-reported): CRT calls must use the *_internal wrappers
  (common.h:134-151) — bare calloc/fscanf pair DLL-heap/DLL-stdio
  state against game code that frees with the game's _free (confirmed:
  Combat destructor asm frees the tables with the game's free). Fixed
  tree-wide: calloc x14 (Combat 6, Static 8) and fscanf x11 (Static 5,
  Animated 4 incl. 2 comment mentions, Action/Missile/Moving/Combat 1
  each) -> calloc_internal/fscanf_internal. memcpy: no _internal
  wrapper exists; kept bare (heap-neutral byte copy, no CRT state) —
  documented exception per the new rule. Also fixed in this round:
  Combat setup2 read-consistency — the asm reads missed_missile_spread
  (@0x178, 4 bytes) after minimum_weapon_range and BEFORE the
  fight_sprite index; the pseudocode DROPPED that read entirely and
  misplaced orig_speed_of_attack at the tail (my written code
  followed the pseudocode = stream desync). Corrected tail order per
  asm 0x489073-0x4890E6: min_range, missed_missile_spread, fight_ix,
  orig_armor, orig_weapon, orig_weapon_range, orig_speed_of_attack.
  SEH SWEEP (2026-11-14, protocol-level check across all 27 hooks):
  exactly two functions carry hidden frame SEH — make_new_master and
  make_new_obj (both folds applied on approval); all others clean.
  make_new_obj @ 0x0048D260 [0x48D260,0x48D330) unblocked and written
  (owner-approved 2026-11-14): sizeof(RGE_Static_Object) mechanically
  verified = 180 = 0xB4 (matches operator_new(0xB4); the pack-capped
  x86 struct alignment is 4 — round-to-8 was a calculator artifact);
  recycle call idb_functions.h:6092 (0x006241E0); recycle_in_to_game =
  vtable slot 29 ([edx+74h], vtbl @3960); ctor idb_functions.h:3774
  (0x0054B2E0); SEH fold = stru_680DF8 catch-all, unwind proc
  loc_642390 frees the allocation and propagates, folded as
  __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH).
  make_new_master_new amended in place with the identical fold
  (stru_680E20 / unknown_libname_177 @0x6423B0). Installer now 25
  setHook calls (total 25 for the class incl. make_new_obj).
- RGE_Master_Static_Object small-method batch (owner-approved
  2026-11-14): get_help_message @ 0x0048EF30 [..,0x48EF36),
  get_help_page @ 0x0048EF40 [..,0x48EF46), make_available @
  0x0048EA40 [0x48EA40,0x48EA4E), make_new_master @ 0x0048D330
  [0x48D330,0x48D387). Gate PASSED ×4. make_new_master:
  sizeof(RGE_Master_Static_Object) mechanically verified = 208 = 0xD0
  (matches operator_new(0xD0); full offset walk corroborates asm —
  hp@50, damage_sprites@164, attribute_max_amount@140, ...);
  operator_new_internal verified same target as the binary's
  operator_new (0x00632B9D, common.cpp:323-324); forwards to the
  original setup (idb_functions.h:1910). Writer scan 0 overlaps ×4;
  address grep = dead profiler table only (functionlist.cpp:2131,
  :2140, :2144-2145). WRITTEN + post-write verification; installer
  gained four setHook calls (total 23 for the class). make_new_obj
  @ 0x0048D260 deferred to its own loop (RGE_Game_World recycle call,
  recycle_in_to_game vtable slot, sizeof(RGE_Static_Object)
  verification, RGE_Static_Object ctor grounding).
- RGE_Master_Static_Object::small_save @ 0x0048DAE0
  [0x48DAE0,0x48DBB3) + save @ 0x0048DBC0 [0x48DBC0,0x48DAE0) —
  owner-approved 2026-11-14 with sizeof size-verification directive
  (template rule added: hard stop on unjustified sizes; runtime string
  byte counts exempt). Gate PASSED ×2. ALL write sizes sizeof-verified
  against field types (idb_types.h:1206-1281); two justified spans:
  small_save writes attribute_amount_held[0] only (4 = sizeof element,
  NOT the array); save damage-element write deliberately spans
  damage_percent + flag (adjacent, sizeof + sizeof). Pointer fields
  saved as pointed-to object ids (RGE_Sound::id @+12 — idb_types.h
  :2960-2967; RGE_Sprite::id @+130); id/length locals typed __int16 /
  unsigned __int16 so sizes are sizeof-exact (original wrote low words
  of 4-byte slots — same bytes). Name/gbg writes gated on signed
  16-bit length > 0 (preserved). Callees: rge_write
  (idb_functions.h:3005, 0x004D5790), strlen (CRT). Writer scan
  0 overlaps (corrected small_save range; earlier 2 flags were a
  range-computation bug, targets 0x48EFD3/0x4Bxxxx far outside).
  WRITTEN + post-write verification: 23 definitions / 23 installer
  references, size audit clean.
- RGE_Master_Static_Object::copy_obj @ 0x0048D390
  [0x48D390,0x48D510) + copy_vars @ 0x0048D510 [0x48D510,0x48D800) —
  owner-approved 2026-11-14. Gate PASSED ×2. Callees grounded: free →
  free_internal, calloc → calloc_internal (binary _calloc verified same
  target 0x00632D33), getstring → idb_functions.h:2281 (0x004AE9A0),
  copy_vars → idb_functions.h:1918 constant. copy_vars = 73 field
  assignments in original order (pseudocode's doubled object_flags
  deduplicated; three pointer-arithmetic array do-loops rewritten as
  indexed loops — equivalence trivial). copy_obj preserves 7 identity
  fields (id, hide_in_scenario_editor, hotkey_id, base_id,
  object_group, available, disabled), frees via free_internal, folds
  the copy_vars call, reallocates damage_sprites (calloc_internal,
  element copy — RGE_Damage_Sprite_Info resolved idb_types.h:3616),
  copies strings via getstring, restores identity fields, sets source
  side effects (hide_in_scenario_editor = 1, base_id = this's
  original id). Writer scan 1 overlap each = dead profiler table
  (functionlist.cpp:2132-2133). WRITTEN + post-write verification:
  21 definitions / 21 installer references, field diff 0, order
  preserved.
- RGE_Master_Static_Object::calc_base_damage_ability @ 0x0048EF50
  [0x48EF50,0x48EFB0) — owner-approved 2026-11-14. Gate PASSED.
  Sums positive RGE_Armor_Weapon_Info values (idb_types.h:7412 — value
  @+2, element size 4; weapon_num @7048 i16; weapon ptr @7049 —
  RGE_Master_Combat_Object @6939). asm nuance verified: the add-gate
  is >= 0.0 — numerically identical to the pseudocode's > 0.0 (adding
  +0.0 is a no-op; sum never -0.0). __ftol (int) return conversion per
  the template rule (third instance of the (__int64) artifact).
  RESTRUCTURE (owner-directed): pointer walk → indexed loop over
  weapon[i].value (layout verified). Writer scan 0 overlaps; profiler
  table hit (functionlist.cpp:2146, dead-code footnote). Virtual
  method — vftable slots in RGE_Master_Action_Object/Animated_Object/
  Master_Combat_Object etc. vftables (DATA XREFs), entry hook covers
  all dispatch. WRITTEN + post-write verification; installer gained
  one setHook call.
- RGE_Visible_Map::get_visible @ 0x00615430 [0x615430,0x615468) —
  owner-approved 2026-11-14. Gate PASSED. Small bit-test getter:
  unified_map_offsets[map_row][map_col] → mutualVisibleMask bit → 15,
  explored bit → 128, else 0. Grounding: unified_map_offsets = EXISTING
  project constant (test.cpp:1386, .data:007A1CBC — extern-declared in
  the replacement); player @+8 (RGE_Visible_Map @idb_types.h:1318);
  mutualVisibleMask @+0x130 / mutualExploredMask @+0x12C (RGE_Player,
  asm-verified). asm decode: neg/sbb/and-0x80 idiom = the != 0 ? 128 : 0
  form. Writer scan pending-not-needed (0 overlaps expected — visibility
  region untouched by patches per census). WRITTEN: src/engine/map/
  RGE_Visible_Map.cpp + install_map.cpp wiring + vcxproj. Advances the
  deferred check_placement loop: its inlined get_visible now has a
  replacement to call (fold decision resolved: constant, not replicate).
- RGE_Tile detector/visibility family batch (owner-approved
  2026-11-14; NEW SUBSYSTEM map/ ratified by owner): has_enemy_detector
  @ 0x0048F810 [0x48F810,0x48F8BC), is_detector @ 0x0048F8C0
  [0x48F8C0,0x48F966), is_powered @ 0x0048F970 [0x48F970,0x48FA34),
  is_shielded @ 0x0048FA40 [0x48FA40,0x48FAE6). Gate PASSED ×4.
  Callees grounded: RGE_Base_Game__get_player_by_id (idb_functions.h
  :690, 0x00428790), numberPlayers (:752, 0x0042C420), isAlly =
  RGE_PlayerVtbl slot (idb_types.h:7128), place_holder1/2/3 =
  RGE_Tile u8 fields (:4185-4187), RGE_Base_Game::gbg_unknown_mask
  (:1699). Semantics: has_enemy_detector = (~ally_mask & ph3);
  is_detector/is_shielded = (ally_mask & phX); is_powered = self bit
  (1<<(player_id-1)) + flag-gated ally fallback. Writer scan 0
  overlaps ×4. WRITTEN: src/engine/map/RGE_Tile.cpp +
  install_map.cpp + install_engine.cpp wiring + vcxproj.
  Post-write verification: RGE_Tile 4 defs / 4 refs identical; tree
  total 36 defs / 35 hook refs + install_map/install_object plain
  calls, 0 missing/extra/dupes. NOTE: aipower.cpp:22 isPlacePowered
  is a direct by-address CALLER of is_powered (0x48F970) — not a
  patch; post-hook it routes to the replacement with a matching ABI.
- RGE_Master_Static_Object::alignment @ 0x0048EA60
  [0x48EA60,0x48EC06) + alignment_box @ 0x0048EC10
  [0x48EC10,0x48ED66) — owner-approved 2026-11-14. Gate PASSED ×2.
  Grounding: world->map = RGE_Game_World::map @+52; map_width/height
  int @+8/+12 in RGE_Map (idb_types.h:2892-2916, fild int→double);
  radius_x/y float @+60/+64; epsilon = float constant 9.9999997e-6
  (flt_65B48C); tile_half_height/width __int16 @2915-2916 (asm
  [map+0xBF3E]/[map+0xBF40]). RESTRUCTURE ×2 (owner-directed): the x87
  flag-driven rounding decoded from asm — radius-integrality selects
  half-up rounding (bump at >= 0.5; pseudocode's > 0.5 differs at
  exactly 0.5 — asm ground truth) vs tile-center shift; alignment row
  bumps past a tile edge return early with 1; alignment_box = the
  diamond-corner computation verbatim ((__int64) casts = __ftol + i16
  narrow store per the template rule). asm nuance: the pseudocode's
  NaN clamp path in calc-style comparisons is unreachable here.
  Writer scan 0 overlaps ×2; address grep = dead profiler table
  (functionlist.cpp:2141-2142). WRITTEN + post-write verification;
  installer gained two setHook calls.
- RGE_Master_Static_Object::check_placement @ 0x0048E180
  [0x48E180,0x48EA40) DEFERRED as a dedicated multi-phase loop
  (owner-directed 2026-11-14). Evidence banked: full pseudocode
  (.c:157782-158213) — 6 phases: bounds (return 7), center-tile
  terrain req (return 1), 4 directional terrain edge-scan loops with
  i16 wraparound indices and 32-byte RGE_Tile strides (return 1),
  per-tile passability (terrains[terrain_type] <= 0.050000001 →
  return 2), visibility via INLINED get_visible body + elevation_flag
  1/2/3 variants (return 3), visibility accumulation (return 5), and
  the object collision scan (return 6; cell walk byte@+13 =
  occupancy, RGE_Static_Object** @+7; ignore_obj; object_state < 7;
  can_be_built_on; AABB vs radii; object_group 27 / 31+7 / temple
  variants; obstructionObject output). Callees to ground: RGE_Tile
  struct offsets (terrain_type/tile_type/height - byte@+13, stride
  32), RGE_Visible_Map::get_visible INLINED (not exported in
  idb_functions; body decision pending - new constant vs replicate),
  base_game->prog_mode (RGE_Base_Game @16814). New callees:
  RGE_Visible_Map__get_visible exists in lst @0x615430 (proc) but is
  NOT exported in idb_functions.h.
  VERIFICATION ROUND 2 (2026-11-14): get_visible body banked (lst
  :894642-894668, small bit-test; replacement written - the fold
  decision is resolved: call the replacement). Phase 2 CORRECTED
  via asm (loc_48E2C4): the center-tile read is
  map_row_offset[(int16)(int)row][(int16)(int)col].terrain_type - the
  RAW position tile i16-truncated (an earlier draft truncated the
  left-edge-derived index instead - wrong tile). Phases 1/2/4/5/6
  verified. Phase 3 OPEN: the four directional terrain-scan loops are
  IDA-variable-reuse garbled in pseudocode (one guard decodes as
  trivially true); two asm segments unpulled (0x48E318-0x48E350,
  0x48E3D6-0x48E432); a register-tracked decode of the loop block is
  required before the scan ranges can be stated.
- RGE_Map RMM loader pair (owner-approved 2026-11-14; CORRECTED
  entry): data_load_random_map @ 0x00490580 [0x490580,0x4905E0) and
  load_random_map @ 0x004905E0 [0x4905E0,0x490650) allocate
  RGE_RMM_Database_Controller with literal 0x58AC = 22700. Mechanical
  sizeof walk of idb_types.h:6076-6100 computes 22700 - VERIFIED
  MATCH. CORRECTION of an earlier claim: 0x58AC was misconverted as
  "22732", manufacturing a phantom 32-byte layout drift; the type
  library was never in conflict. Embedded Info structs in the walk:
  Land 5176, Terrain 5948, Object 7532, Elevation 3968, Map_Data 8.
  WRITTEN with sizeof(RGE_RMM_Database_Controller) allocations +
  post-write verification (RGE_Map.cpp: 6 defs / 6 refs). LESSON
  recorded in the template: verify all hex-to-decimal conversions;
  an arithmetic slip here manufactured a phantom blocker and nearly
  mislabeled a verified pair as a type-library defect.
  make_new_obj/make_new_master use verified sizeofs (180/208) and are
  unaffected.
- FACTORY/COPY TRIOS ×4 (owner-approved + written 2026-11-14): 12 hooks
  — make_new_obj/make_new_master/copy_obj for Action [0x487FA0/0x488070/
  0x4881F0], Combat [0x489680/0x489750/0x4897C0], Doppleganger
  [0x48A510/0x48A5F0/0x48A660], Missile [0x48A960/0x48AA30/0x48AAF0].
  Ladder decoded per trio (all ranges from proc/endp; SEH chunks
  0x641FD0/0x641FF0, 0x642150/0x642170, 0x6421F0/0x642210, 0x642290/
  0x6422B0 — operator_delete folds, the make_new_master chunks carry the
  destructor-chained secondary entry jmping the IMMEDIATE base's dtor:
  Action->Moving, Combat->Action, Doppleganger->Animated,
  Missile->Combat):
  * make_new_obj ×3 (Action/Combat/Missile): recyclable @0xBC ->
    recycle_object_in_to_game(owner->world @0x8C, master_type @+4) ->
    guarded dispatch slot @0x74 (idb_types.h:3960 = RGE_Static_ObjectVtbl
    member 29); else operator_new + runtime ctor(do_setup=1). ALLOCATIONS
    MECHANICALLY WALKED: RGE_Action_Object 324 = 0x144 (idb_types.h
    :7423-7501; ei@144=36; BVector=12 :1757-1762), RGE_Combat_Object
    380 = 0x17C (:7318-7406; Action prefix 324 + VUR_Ptrs[9]=36 + tail),
    RGE_Missile_Object 396 = 0x18C (:20935-21027). Runtime ctors:
    idb_functions.h:107/:1069/:2059 (tobj param of the Missile ctor is
    Combat-typed -> this_ cast).
  * DOPPLEGANGER DELTA: make_new_obj recycle dispatch is SLOT @0x370 —
    recycle_in_to_game_2 = member 220 of RGE_Doppleganger_ObjectVtbl
    (idb_types.h:21758; 21758-21538 = 220 entries -> 0x370), 7-arg
    signature incl. obj_to_dopple; the pseudocode's "vfptr[1].play_
    unload_sound" was an IDA offset artifact. RGE_Doppleganger_Object
    allocation 220 = 0xDC walked (:21470-21530). Reusable base->derived
    cast evidence: the type-25 recycle pool + the @0x370 dispatch only
    resolving through the Doppleganger vtbl.
  * make_new_master shapes SPLIT: Action = base Moving COPY-CTOR
    (do_setup=0) + Action vfptr + base MOVING setup (NOT Action::setup —
    asm 0x4880C2; the setTurnRadiusOffTurnSpeed dispatch therefore fires
    with the Action vfptr already stored) + master_type = 40 + ELEVEN
    extension copies (offsets 260-296, gbg from source work_rate, tasks
    LAST). Combat/Missile = base COPY-CTOR + vfptr + DERIVED setup (the
    setup does everything; NO master_type store, NO copies in the
    factory); Missile adds master_type = 60 + six extension copies
    (380-384 bytes, 388 dword). Doppleganger = Animated COPY-CTOR +
    vfptr + Animated setup + master_type = 25 + fog_flag = 3.
  * copy_obj: Action = base Moving copy_obj + TEN copies (asm order;
    gbg from source work_rate; drop_site AND backup_drop_site as
    SEPARATE word moves 0x488230-0x488245 — the pseudocode's single
    dword read was an IDA merge; NO run_pattern copy; tasks LAST @260).
    Combat = base Action copy_obj + 22-field extension: tables freed
    (free_internal) + reallocated (calloc_internal(count, sizeof
    RGE_Armor_Weapon_Info)) + memcpy deep copies; missile_id is a WORD
    move (0x4898F7 — pseudocode's float rendering was a mis-typing);
    area_effect_level BYTE stored out of order after the weapon offsets
    (0x489951). Doppleganger = PURE FORWARDER (call Animated copy_obj +
    retn 4, no tail-jmp). Missile = base Combat copy_obj + six copies —
    missile_type..missile_die_info as FOUR SEPARATE byte moves
    (0x48AAFE-0x48AB28; the pseudocode's dword merge of object_flags
    was an IDA artifact), area_effect_specials byte, ballistics_ratio
    dword. Base-pointer source re-typings presented per rule (Action
    src offsets 52-84; Combat 92-168; Missile 344/176/180).
  Caller topology: Combat copy_obj is called by the Missile copy_obj
  AND by TRIBE_Master_Combat_Object__copy_obj (lst :233544) — the
  TRIBE_ subclass tier calls into the RGE_ implementations (see the
  scope entry below). WRITTEN + post-write verification incl. the new
  compile gate (see the gate entry): 6/6 TUs 0 errors; installer
  hooks +3 per file (totals: Action 12, Combat 12, Doppleganger 8,
  Missile 8).
- DERIVED MODIFY OVERRIDES (owner-approved + written 2026-11-14): 4
  hooks — Animated modify @0x4889E0 [0x4889E0,0x488A00) flag==5 ->
  speed @0xD0=208 (else base Static modify, hooked-routed); Moving
  modify/modify_delta/modify_percent @0x48B6E0/0x48B710/0x48B740
  [..,0x48B700)/[..,0x48B736)/[..,0x48B766) flag==6 -> turn_speed
  @0xDC=220 (set/add/multiply; else base Animated calls — modify_delta
  @0x488A10 and modify_percent @0x488A40 remain UNHOOKED, execute the
  originals). All four plain functions, retn 8, no SEH. UNOWNED SIBLINGS
  flagged: Animated modify_delta/modify_percent; Action modify trio
  (@0x488290/0x4882D0/0x488320 — flag 13 -> work_rate+gbg, flag 23 ->
  search_radius); Combat modify trio (@0x4899B0/0x488DC0/0x488EE0 —
  switch over armor/weapon indices); Missile modify family (@0x48AB50+,
  flag 19 -> targetting_type). WRITTEN + compile gate (Animated 13,
  Moving 15 hooks).
- COMPILE-GATE FINDING + PROTOCOL AMENDMENTS (2026-11-14, owner-
  approved): the uncommitted Animated/Moving factory trios postdated the
  last green build and carried SEVEN latent type errors — Animated cpp
  228 C2664 (uncast dispatch this_), 236/276 C2440 (runtime-ctor return
  + result assignment across unrelated types); Moving cpp 318/326/369
  same shapes + 398 C2664 (copy_obj source cast at the wrong level —
  the base constant's param 2 is already RGE_Master_Static_Object*).
  All 7 fixed with explicit C-style casts (approved, applied).
  REPRODUCED with a single-TU gate (vcvars64 + cl /c /std:c++17 /W3
  /EHsc /I cheatdll): the four committed class files compile 0 errors,
  the working tree reproduced exactly the 7 errors pre-fix. Template
  amended: (1) post-write checklist += COMPILE GATE — every touched TU
  must compile 0 errors before a WRITTEN status; (2) cross-class cast
  rule extended to RESULT ASSIGNMENTS (C2440) and EVERY function-pointer
  argument position, incl. the explicit this argument of vtable-slot
  calls. The closeout's "builds green" horizon is hereby corrected:
  green through the destructor-family write only.
- SCOPE UPDATE — MASTER-TYPE 70/80 CONFIRMED AS FULL CLASSES
  (2026-11-14, supersedes the closeout's "PENDING CONFIRMATION" and the
  derived-family note "no separate setup/ctor exports found"): the
  subclasses were missed on NAMING — they are exported under the TRIBE_
  prefix. TRIBE_Master_Building_Object (80): 22 exports @0x5C6A30-
  0x5C8000 (idb_functions.h:5098-5119: scalar_deleting_dtor, ctor2/3/4,
  setup×4, dtor, copy_obj, modify×3, small_save, save, make_new_obj2
  with an extra uID param, make_new_obj, make_new_master, draw,
  alignment, check_placement, check_placement_for_units); struct
  idb_types.h:14163, Vtbl :15842. TRIBE_Master_Combat_Object (70): 20
  exports @0x5C8340-0x5C92E0 (:5120-5139, + create_task_list); struct
  :15690, Vtbl :15998. Their copy_obj/modify call INTO the RGE_ Combat
  implementations (lst :233544/:233685) — the ladder continues one more
  level (~42 more hooks of future scope). Owner corrected
  idb_functions.h:5120 in place (IDA had typed the Combat-class
  copy-ctor with Building parameters).
  HOOK TOTALS after this session: Static 37, Animated 13, Moving 15,
  Action 12, Combat 12, Doppleganger 8, Missile 8 = 105 hooks.
- MODIFY + SAVE FAMILIES (owner-approved + written 2026-11-14): 23
  hooks — the entire remaining derived-override surface of the RGE_
  7-class tree. MODIFY (11): Action modify/modify_delta/modify_percent
  @0x488290/0x4882D0/0x488320 (flag 13 -> work_rate + gbg from the SAME
  amount — asm fst/fstp single computation stored twice; flag 23 ->
  search_radius; default -> Moving); Animated modify_delta/modify_percent
  @0x488A10/0x488A40 (flag 5 -> speed); Combat
  modify/modify_delta/modify_percent @0x4899B0/0x489B10/0x489D20
  (switches restored from jump tables off_489AD4/off_489CDC/off_489F90 —
  the tables sit in inter-function gaps outside the replaced ranges);
  Missile modify trio @0x48AB50/0x48AB80/0x48ABB0 (flag 19 ->
  targetting_type as a BYTE store of ftol — asm mov [esi+17Dh], al; base
  Combat call UNCONDITIONAL, no else). COMBAT TRIO SEMANTICS (all
  asm-decoded): modify = SET forms (floats take the raw amount dword;
  i16 fields take ftol's low word; flags 8/9 = armor/weapon
  INDEXED by BYTE1(ftol amount), guard idx < (i16)count, value =
  (u16)(u8)amount); modify_delta flags 8/9 SEARCH by .type == BYTE1 then
  value += (u8) (16-bit add; not-found -> return); i16 deltas are
  16-bit adds of ftol's low word; flag 18 is a SET in delta too
  (asm mov 0x489C8A); modify_percent flags 8/9 = by-TYPE search then
  value = (i16)(int)(BYTE0·0.01f·old + 0.5f) — flt_6565FC =
  float32(0.01) (.c:154917), const_f_half = 0.5f; the i16 percent fields
  use the RAW amount as multiplier (asm 0x489E96 fmul [esp+amount] —
  no 0.01f) with +0.5f rounding; all operand ranges exact in double
  arithmetic. SAVE/SMALL_SAVE (12): every derived class's pair —
  Action @0x488370/0x4883B0 (sound ids via RGE_Sound::id @+0xC into
  __int16 locals — low-word-only stream writes, Static precedent),
  Animated @0x488A70/0x488AA0 (speed w4), Moving @0x48B770/0x48B7A0
  (sprite ids via RGE_Sprite::id @+0x82 prepared BEFORE the base call;
  13 writes incl. the five m* floats), Combat @0x489FE0/0x48A0E0
  (small_save table guards = COUNT > 0 only; save table guards =
  POINTER-only; area_effect_level written out of order after the
  weapon offsets (asm 0x48A214); weapon_range written twice in
  small_save (asm 0x48A0A5); fight id written late (w2 0x48A243)),
  Doppleganger @0x48A670/0x48A680 (pure forwarders), Missile
  @0x48ABE0/0x48AC10 (six extension writes, all sizeof-exact). All
  rge_write widths sizeof-form (id locals via __int16; tables via
  count * sizeof(RGE_Armor_Weapon_Info)) — zero deviations. Stale
  comments in Moving modify_delta/percent ("@0x488A10/40 remains
  UNHOOKED") updated to HOOKED per this batch. Collision audit: 0
  active writers ×23 (dead profiler table only). WRITTEN + post-write
  verification incl. the compile gate: 7/7 TUs 0 errors; defs/refs
  Action 17 / Animated 17 / Moving 17 / Combat 17 / Doppleganger 10 /
  Missile 13 (+ Static 37) = 128 hooks total.
  REMAINING MASTER-FAMILY SCOPE: the TRIBE_Master_Building/Combat tier
  (42 exports) — next batch. Optional leftovers: the five derived ctor1
  copy-constructors (deliberately unhooked; they execute originals
  inside our factories).
- TRIBE MASTER-COMBAT TIER (owner-approved + written 2026-11-14/15):
  NEW CLASS FILE src/engine/object/TRIBE_Master_Combat_Object.{h,cpp}
  + install_object.cpp wiring + vcxproj/.filters entries. 19 of 20
  exports owned (ctor1/scalar_dtor/ctor2-4/setup x4/dtor/create_task_
  list/copy_obj/modify trio/small_save/save/make_new_obj/make_new_
  master); check_placement @0x005C92E0 DEFERRED to its own dedicated
  loop (large multi-phase shared implementation — Building's
  check_placement calls it twice, asm XREFs +6A/+483), per the RGE
  check_placement precedent. STRUCTURAL FOUNDATION: TRIBE_Master_
  Combat_Object = flattened RGE_Master_Combat_Object mirror (all 380
  bytes, offsets identical) + Combat(70) extension @398-452
  (build_inventory[3] @380, build_pts_required @398, id_of_building_obj
  @400, button_location @402, orig_pierce_armor @404, rear/flank
  attack modifiers @408/412, tribe_unit_type @416, volley block
  @420-447, special_attack_flag/hero_flag @448/449, garrison_sprite
  @452); sizeof = 456 = 0x1CC (matches make_new_master's
  operator_new(0x1C8)); every offset double-confirmed by the class's
  own asm copy block (0x18E-0x1C4) and by setup4/copy_obj's v7[1]/v7[2]
  base-pointer arithmetic. Runtime class TRIBE_Combat_Object
  (idb_types.h:16322) = RGE_Combat_Object 380 prefix + the 70-tier
  runtime extension -> 452 = 0x1C4 (matches make_new_obj). Vftables:
  .rdata:0066738C (class) / 00667414 (TRIBE_Task_List, stored by
  create_task_list after the RGE_Task_List ctor, idb_functions.h:4915).
  DELTAS vs the RGE_ ladder: make_new_obj has NO RECYCLE PATH (Combat
  70 runtime objects never recycle; direct operator_new + TRIBE_
  Combat_Object__TRIBE_Combat_Object do_setup=1, idb_functions.h:3982);
  make_new_master = base RGE Combat ctor1(do_setup=0) + TRIBE vfptr +
  DERIVED setup (no master_type store — the setup does it), Combat-like
  shape. MODIFY FLAG NAMESPACE (new tier-local set, restored from jump
  tables off_5C8D80/off_5C8DA0 over flag-100): 'd'=100 all-inventory
  (selector -1), 'e'=101 build_pts_required, 'f'=102 volley_fire_amount,
  'g'/'h'/'i'/'j'=103/104/105/106 inventory by type 0/1/3/2, 'k'=107
  max_attacks_in_volley; default -> base RGE Combat modify (hooked ->
  routes). setup3's dead-store chain kept verbatim (conditional
  special_attack_flag store then unconditional zero, asm 0x5C88A5-
  0x4888C0); setup3 RE-DERIVES orig_armor after the base setup3 (start
  0, type==4, NO fallback — overwrites the base's -1/base_armor
  derivation, faithful); orig_pierce_armor = -1 -> max type==3 ->
  fallback base_armor. save writes the garrison id as a FULL 4-BYTE
  sign-extended int (unlike the low-word id locals elsewhere).
  PROTOCOL: the owner-directed loop-bound rule entered the template
  ("magic loop counters over declared arrays" — hard stop on bare
  literals; _countof(array) with size_t counters is the mandated form,
  refined same session from an interim macro per owner direction). The
  batch initially shipped `i < 3` loops over build_inventory[3] and six
  same-class forward-reference errors (ctor/scalar-dtor calls to
  replacement functions defined later in the file, plus one missing
  DWORD dummy) — all fixed pre-gate: loops converted to _countof, the
  ctor/scalar-dtor internal calls moved to the idb constants (hooked ->
  routes, per the HOOK-ROUTING RULING and the shipped ctor precedent).
  WRITTEN + post-write verification: compile gate 8/8 TUs 0 errors
  0 warnings on the new TU; defs/refs TRIBE 19/19; total hooks 147
  (Static 37, Animated 17, Moving 17, Action 17, Combat 17,
  Doppleganger 10, Missile 13, TRIBE_Master_Combat 19); CRT/base_game/
  pointer-walk sweeps clean. SWEEP FINDING RESOLVED (owner-approved
  retro-fit 2026-11-15): the 8 pre-rule `i < 3` literal loop bounds in
  RGE_Master_Static_Object.cpp (copy_vars x3 + save/small_save/copy_
  obj/setup2/setup3/setup4 attribute-array loops) converted to
  _countof(this_->attribute_type_held / _amount_held / _flag) with
  size_t counters; Static re-gated 0 errors (its 11 C4244s are
  pre-existing approved x87/ftol forms, untouched); tree-wide sweep of
  src/engine confirms zero literal loop bounds remain.
  NEXT: check_placement @0x5C92E0 dedicated loop; TRIBE_Master_Building_
  Object (22 exports — its make_new_master calls the TRIBE Combat
  copy-ctor, asm XREF 0x5C8340: Building extends THIS tier).
- TRIBE MASTER-BUILDING TIER (owner-approved + written 2026-11-15):
  NEW CLASS FILE src/engine/object/TRIBE_Master_Building_Object.{h,cpp}
  + install_object.cpp wiring + vcxproj/.filters entries. 18 of 22
  exports owned (scalar_dtor/ctor2-4/setup x4/dtor/copy_obj/modify trio/
  small_save/save/make_new_obj2/make_new_obj/make_new_master); DEFERRED
  to a dedicated loop: draw @0x005C7880 (688 bytes), alignment
  @0x005C7B30, check_placement @0x005C7B50 (1200 bytes — calls
  TRIBE_Master_Combat_Object__check_placement twice, +6A/+483),
  check_placement_for_units @0x005C8000 — plus the TRIBE Master Combat
  check_placement @0x005C92E0 itself. STRUCTURAL FOUNDATION:
  TRIBE_Master_Building_Object = the TRIBE_Master_Combat_Object mirror
  (all 456 bytes, field-identical through garrison_sprite @452) +
  Building(80) extension @456-551 (construction_sound/sprite/snow_
  sprite @456/460/464, building_connect_flag/facet @468/470,
  build_and_go_away @472, on_build_make_obj/tile/overlay/tech @474-481,
  can_burn @482, linked_id[4]/linked_x[4]/linked_y[4] @484/492/508,
  construction_id/open_close_id/open_close_sound @524/526/528,
  garrison_units/heal_rate/repair_rate @532/536/540, salvage_id @544,
  pillage_attr[6] @546); sizeof = 552 = 0x228 (matches make_new_master
  operator_new 0x228, asm 0x5C7810-region); every offset double-
  confirmed by setup4/copy_obj's v6[2].* arithmetic (416 + Static
  offsets: undead_sprite->460, los->468/470, obj_max->472/474,
  radius_x/y->476-482, button_pict region->508, construction_radius_x/
  y->524/526/528, death_sprite->456, elevation_flag->532, movement_
  type->536, attribute_type_held[1]->540, attribute_amount_held[0]->
  544). Runtime class TRIBE_Building_Object (idb_types.h:9724-9881) =
  TRIBE_Combat_Object 452 prefix + Building runtime extension (burning/
  production-queue/garrison-linked/gather-point/wonder/gate block) ->
  632 = 0x278 (matches BOTH factories' operator_new 0x278); runtime
  ctors idb_functions.h:3907 (0x005531A0, 8 args incl. uID) and :3911
  (0x00553410, 7 args). Vftable .rdata:00667280. LADDER: ctor2/3/4 ->
  base TRIBE Master Combat ctorN(do_setup=0) -> Building vfptr ->
  derived setupN (SEH funclets jmp TRIBE_Master_Combat_Object__
  destructor, chunks 0x6465E0/0x646600/0x646620); make_new_obj/
  make_new_obj2 have NO RECYCLE PATH, direct operator_new(0x278) +
  runtime ctor do_setup=1 (make_new_obj uses ctor2 — no uID;
  make_new_obj2 uses ctor1 — with uID; they SHARE one compiler-merged
  SEH handler label 0x64664B); make_new_master = base TRIBE Combat
  ctor1(do_setup=0) + Building vfptr + DERIVED setup (no master_type
  store). MODIFY ASYMMETRY: modify/delta handle flags 17
  (building_facet set/add) and 108 (garrison_heal_rate set/add);
  percent handles ONLY 108 (multiply) — no facet case. save writes FOUR
  id locals (construction/snow sprites, construction/open_close sounds,
  __int16 low-word form); linked triplets stream id-w2/x-w4/y-w4 per
  element; _countof applied to linked_id and pillage_attr loops per the
  loop-bound rule. Collision audit: 0 active writers x18 (dead
  profiler table only). WRITTEN + post-write verification: compile gate
  9/9 TUs 0 errors (Static's 11 C4244s = pre-existing approved x87
  forms); defs/refs Building 18/18; total hooks 165 (Static 37,
  Animated 17, Moving 17, Action 17, Combat 17, Doppleganger 10,
  Missile 13, TRIBE_Master_Combat 19, TRIBE_Master_Building 18);
  CRT/base_game/pointer-walk/literal-bound sweeps clean.
  REMAINING MASTER-FAMILY SCOPE: the five large gameplay functions
  (Building draw/alignment/check_placement/check_placement_for_units +
  TRIBE Master Combat check_placement) — dedicated decode loop; then
  the master-family project is COMPLETE (all 9 tiers). Beyond it:
  runtime TRIBE_ classes' methods, Production_Queue_Record, and the
  five derived ctor1 copy-constructors (optional).
- TRIBE MASTER-COMBAT check_placement (owner-directed dedicated loop,
  WRITTEN 2026-11-15): @0x005C92E0 [0x5C92E0,0x5C976F) — the 20th of
  20 TRIBE_Master_Combat exports now owned. FULL ASM PASS
  0x5C92E0-0x5C976F vs pseudocode: phase-faithful. NEW GROUNDINGS:
  base_game->master_obj_id @+0x428 (i16, movsx); prog_mode @+0x1DC ==
  7 re-confirmed; tile walk count @tile+13 / object array @tile+20
  (tile.Object_List named field, per the RGE round's precedent);
  fly-over = BOTH movement_type @+120 == 1 -> |dx| < other radius_x
  @+60 + my x-radius AND other radius_y @+64 + my y-radius > |dy| ->
  return 6; desolid_flag @+565 (TRIBE_Building_Object runtime) skips
  Building blockers with group not in {34,36}; check_movible 1 skips
  type 70, 2 skips type 70 with speed @+180 > 0; radius_z @+68
  substitution: group 27 OR (rz==0 && type==80 && id@24 != 50) ->
  1.0; block x-test strict < (C0-only, asm 0x5C9677), y-test 0x41
  (C0|C3, asm 0x5C968E) — both strict; canPlayerObjectBeEntered
  (:4033, 0x55EA20) -> return 9; RGE_Game_World__object (:6086)
  re-fetch under prog_mode==7. CASTS fixed at gate: world
  TRIBE_World*/RGE_Game_World* dual-naming, get_player
  RGE_Player*/TRIBE_Player* dual-naming, speed via
  (TRIBE_Combat_Object*)obj guarded by master_type==70 (factory
  identity = runtime evidence). Building alignment @0x005C7B30 ALSO
  WRITTEN (pure forwarder, center_flag forced 1, asm verified).
  HOOK TOTALS: TRIBE_Master_Combat 20/20 (check_placement installed
  @0x5C92E0); Building alignment +1 = 19; grand total 166.
  HOOK TOTALS (verified 2026-11-15, scripted count): Static 37, Animated 17,
  Moving 17, Action 17, Combat 17, Doppleganger 10, Missile 13,
  TRIBE_Master_Combat 20, TRIBE_Master_Building 19 = 167 master-family
  hooks (all compile-gated 0 errors, x86 toolset). NON-master-family
  hooks in src/engine (map subsystem etc.) are tracked separately.
  NEXT: Building check_placement @0x5C7B50 + check_placement_for_units
  @0x5C8000 + draw @0x5C7880 (the three remaining Building gameplay
  functions). ALSO QUEUED: TRIBE_Master_Tree_Object decode (6 exports
  @0x5C9780+, child of Static, master_type 15 — see DECOMP_HIERARCHY.md
  tree-note correction); then the master-family project is COMPLETE
  (all 10 tiers). Beyond it: runtime TRIBE_ classes' methods,
  Production_Queue_Record, and the five derived ctor1 copy-constructors
  (optional).
- COLLISION-AUDIT FAILURE + FIX (owner-reported 2026-11-15:
  "TRIBE_Master_Building_Object__setup4_new breaks game state
  consistency test / crashes later during reading; commenting out its
  installation fixes the crash"): ROOT CAUSE — the bldg_heal_rate
  patch (bldg_heal_rate_save.cpp) hooks TWO MID-FUNCTION points:
  0x005C750D (Building small_save tail — naked stub writes
  garrison_heal_rate w4, unconditional) and 0x005C72AF (Building
  setup4 tail — naked stub reads garrison_heal_rate w4 gated on
  current_save_game_version >= 3, casts.cpp:15). The patch EXTENDS the
  Building(80) master stream format (v>=3 DATs carry 4 extra bytes per
  Building master in the xref section). My audit greped only the 18
  ENTRY address literals — both mid-function targets were invisible —
  so setup4_new silently dropped the 4 heal_rate bytes and the DAT
  stream desynced (crash downstream). FIX: bake-ins applied —
  setup4_new now does the version-gated heal_rate read after the facet
  read; small_save_new does the unconditional heal_rate write after
  the facet write; both patch call sites retired (commented in place
  in bldg_heal_rate_save.cpp per the retirement rule). METHOD CORRECTED
  (template amended, protocol step 3b): collision audit must extract
  targets from EVERY writer call site repo-wide (incl. naked
  mid-function stubs) and range-test against EVERY owned [proc, endp).
  CORRECTED-AUDIT RESULTS (scripted, repo-wide): 5 additional
  mid-function collisions found, all currently DEAD (silently
  reverted) under my hooks — pending owner-approved bake-ins:
  0x0048E2F5 in RGE check_placement (terrain.cpp:1454,
  shipyard_center_tile_req), 0x0048C1F4 in Static setup (aiunitline.cpp
  :114, on_static_object_setup), 0x0048D13B in Static setup4
  (aiunitline.cpp:115, on_static_object_setup4), 0x0048C45F in Static
  setup2 (minimap.cpp:64, grey_load_dat_unit), 0x0061543A in
  RGE_Visible_Map::get_visible (mapsize.cpp:1763 — writeDword mapptr:
  the big-map patch redirects get_visible's visibility-array base; my
  get_visible_new reads the original unified_map_offsets base). GATE
  TOOLSET CORRECTED: the compile gate now runs the x86 cross-compiler
  (HostX64\x86) — the project is Win32; the earlier x64-toolset gate
  rejected bldg_heal_rate_save.cpp's __asm. 9/9 TUs 0 errors under x86.
- PATCH BAKE-INS ROUND 2 (owner-directed 2026-11-15, after the
  corrected audit surfaced 5 dead patches): (1) aiunitline.cpp:114/
  115 (0x0048C1F4 in Static setup, 0x0048D13B in Static setup4) —
  BAKED into RGE_Master_Static_Object__setup_new / __setup4_new:
  AI-unit-line masters copy unit_line/min_tech_level from the source
  (the ONLY patch addition - the stub REPLAYS the original name handling:
  name = 0 then the getstring duplicate, resume 0x48C1FD / 0x48D144,
  so the name stays getstring-duplicated; CORRECTION 2026-11-15: the
  first bake-in wrongly dropped the name getstring, caught by
  re-verification, restored); gbg_name2
  handling continues (patch resume 0x48C1FD / 0x48D144); (2)
  minimap.cpp:64 (0x0048C45F in Static setup2) — BAKED into
  RGE_Master_Static_Object__setup2_new: carbon-rock masters
  (is_carbon_rock, minimap.cpp:5 — id list) get map_draw_level = 0
  after the stream read; (3) terrain.cpp:1454 (0x0048E2F5 in RGE
  check_placement phase 2 — replaced `cmp cx, si` + two jz) — BAKED
  into RGE_Master_Static_Object__check_placement_new: the center-tile
  terrain match routes through check_center_tile(center_tile_req1,
  center_tile_req2, tile_terrain)
  (terrain.cpp:1067 — new-terrain-aware, version-gated).
  ARG-ORDER CORRECTION (owner testing caught the logic wrong, same
  day): the baked call had cct1/this_tile swapped —
  check_center_tile(tile, req2, req1) — from decoding the stub's
  pushes first-pushed=first-param, backwards for __stdcall (the
  stub's pushes: ecx=tile FIRST, eax=req2, esi=req1 LAST; __stdcall
  takes the LAST-pushed as the FIRST parameter). The swap broke the
  v>=6 water-pair branch — it tested (tile, req2) against the pair
  instead of (req1, req2), rejecting shipyard-on-water placement (the
  exact feature the patch ships); v<6 is commutative equality so it
  was identical under both orders, which is why inspection alone
  missed it. Fixed to (req1, req2, tile).
  (4) MAP_SIZE (mapsize.cpp:1763, writeDword 0x0061543A in get_visible) — NOT
  baked (owner: barely functional); RGE_Visible_Map__get_visible_new
  carries a // MAP_SIZE marker at the unified_map_offsets access
  (mapptr starts at 0x7A1CBC = the unified_map_offsets base — same
  default behavior; diverges only after the big-map re-point at
  mapsize.cpp:1681). All four patch call sites retired (commented in
  place). Template step 3b refined: bake-in is the DEFAULT
  disposition; the only waiver is owner-directed with the //
  <PATCH_ID> marker convention; the scan is scripted. GATE: 9/9 TUs
  0 errors under x86.
 - TRIBE MASTER-BUILDING check_placement (owner-approved + WRITTEN
   2026-11-15): @0x005C7B50 [0x5C7B50,0x5C7FDF) (lst :763746/:764208,
   retn 30h). Building 20/22 exports owned; grand total 168 hooks.
   FULL ASM PASS vs pseudocode (.c:391470-391705): phase-faithful.
   Decode: group 34/36 (Undeployed Cannon/Cannon) full-arg forward to
   the base; base pre-pass with can_be_built_on_flag=0,
   check_objects=OUR check_movible_objects, check_movible=0 (push
   order 0x5C7B96-0x5C7BBA — IDA frame labels arg_24/check_objects
   swapped vs the prototype; push order decides); the ORIGINAL
   check_objects gates the +-8.0 tile scan (flt_657DC0 = 8.0, lst
   :987837; row-major, 32B tiles, TileObject_Count @13, Object_List
   @20) with gates: id-50 master (moving speed@208 never blocks;
   semantics UNKNOWN — see DECOMP_GAME_FACTS.md registry),
   Gate(8)-vs-own-Wall(6) same-owner, desolid TRIBE_Building_Object
   blockers (mutual allies AND gameType==6 pass; asm 0x5C7D88-0x5C7DFD
   owner-id ladder), can_be_built_on==1 skip, group-31 terrain gate
   (BAKED-IN PATCH), rz substitution (type-80 id!=50 / Fish 23 /
   Shore Fish 25 -> 1.0), blocker gate rz>0 || id==50 || Fruit Bush
   27, AABB strict < / > (test ah,1 / test ah,41h) ->
   obstruction=obj->id, return 6. NEW GROUNDINGS: TRIBE_Player/
   RGE_Player prefix identity through id @160 (idb_types.h:699-783 vs
   :3668-3747) + vtbl slot-9 isAlly parity (:7128/:1102, both +0x24);
   RGE_Tile.terrain_type @+5 as the group-31 gate input (jump table
   byte_5C7FEC/off_5C7FE4 byte-verified = {10,13,17,18,19,20,21}, lst
   :764212-764217); obj+40 = obj->tile (RGE_Static_Object @1450).
   COLLISION (step-3b scripted scan): terrain.cpp:1452 setHook
   0x005C7E2C (onCheckPlacement_carbon) hooks the group-31 switch
   decode MID-FUNCTION — BAKED IN (owner-approved): the replacement
   calls checkCarbonTerrainBuildingBlock (terrain.cpp:1026,
   version-gated: legacy set below DAT v9, isCarbon at v9+); patch
   call site retired (commented in place). RE-RUN AUDIT (carbon-
   family lens): draw [0x5C7880,0x5C7B23) + check_placement_for_units
   [0x5C8000,0x5C832F) -> 0 collisions, 0 naked-stub hits.
   NEW FILE: DECOMP_GAME_FACTS.md (owner-supplied object_group
   registry 0-64 + magic-constant findings registry, owner-directed;
   companion-list edits in STATE/TEMPLATE/HIERARCHY). WRITTEN +
   post-write verification: x86 compile gate 2/2 TUs 0 errors
   0 warnings (TRIBE_Master_Building_Object.cpp + terrain.cpp);
   defs/refs 20/20; bare base_game 0; pointer-walk 0 (indexed
   obj_list[i]); no real literal loop bounds. NEXT: check_placement_
   for_units @0x5C8000 (direct caller TRIBE_Action_Build__set_state
   +58D, lst :599063), draw @0x5C7880; then TRIBE_Master_Tree_Object
   enumeration+decode — master-family COMPLETE (10 tiers).
 - TRIBE MASTER-BUILDING check_placement_for_units (owner-approved +
   WRITTEN 2026-11-15): @0x005C8000 [0x5C8000,0x5C832F) (lst
   :764224/:764524, retn 14h = 5 stack args). Building 21/22 exports
   owned; grand total 169 hooks. FULL ASM PASS vs pseudocode
   (.c:391707-391858): faithful everywhere (the (__int64) renders =
   the known __ftol artifact). Standalone scan — NO base call, NO
   early exit: +-8.0 rectangle over the placed object's OWN
   radius_x/radius_y (no construction-radius branch); per object:
   ignore_obj/ignore_obj2/state<7 gates; id-50 master moving-skip
   (same as check_placement); desolid TRIBE_Building_Object gate
   SIMPLER than check_placement — no group 34/36 exclusion, mutual
   allies alone pass, NO gameType call (asm 0x5C8160-0x5C81A3);
   can_be_built_on==1 skip; blocker gate written as a POSITIVE WRAP
   (orx>0 && ory>0 && (rz>0 || id==50 || Fruit Bush 27)) — restructure
   of the asm skip-chain 0x5C81B6-0x5C81FA, NaN-faithful, matches the
   shipped RGE/Combat scan house form; AABB |dx| < radius_x &&
   |dy| < radius_y, both STRICT < (test ah,1, asm 0x5C823E/0x5C8251).
   Collision action: obj->vfptr->get_target_obj(obj) (RGE_Static_
   ObjectVtbl slot 82 = +0x148, idb_types.h:4013; hand-counted vs the
   recycle_in_to_game @0x74 = index-29 calibration) != ignore_obj AND
   master_type >= 30 AND object_state == 2 -> RGE_Moving_Object__
   evacuate_area (idb_functions.h:2209, 0x004A6BA0, UNHOOKED = the
   asm's direct call; x1=(col-rx)-2.0 [flt_6552F4 = 2.0, lst
   :986657], y1=(row-ry)-1.0, x2=(rx+col)+1.0, y2=(ry+row)+1.0 —
   push order + float left-association asm-exact); found=1 and the
   scan CONTINUES (several units can evacuate); result =
   found ? 6 : 0. NEW GROUNDINGS: RGE_Static_ObjectVtbl slot 82
   get_target_obj; RGE_Moving_Object__evacuate_area prototype.
   Collision audit: 0 (this session's scripted 3-range re-run +
   naked-stub census + address grep — dead profiler table only,
   functionlist.cpp:5393). WRITTEN + post-write verification: x86
   compile gate 0 errors 0 warnings; defs/refs 2/2; installer 21
   hooks; bare base_game 0; pointer-walk 0; no literal loop bounds;
   u8 return vs prototype. NEXT: draw @0x5C7880 (the last Building
   export); then TRIBE_Master_Tree_Object enumeration+decode —
   master-family COMPLETE (10 tiers).
 - TRIBE MASTER-BUILDING draw (owner-approved + WRITTEN 2026-11-15):
   @0x005C7880 [0x5C7880,0x5C7B23) (lst :763432/:763711, retn 20h = 8
   stack args, void). BUILDING TIER COMPLETE — 22/22 exports owned;
   grand total 170 hooks. FULL ASM PASS vs pseudocode (.c:391315-391461).
   STRUCTURAL FINDING: Building draw takes its sprite POLYMORPHICALLY —
   this->vfptr->get_sprite(this, 0) (vtbl slot 1 = +0x04, pushed 0 =
   null RGE_Static_Object*) — unlike the Static base draw's direct
   this_->sprite field read; the slot is the construction-aware sprite
   selector. Master-side vtbl ordering VERIFIED CONSISTENT: Building
   vtbl (idb_types.h:15842-15879) mirrors the base master vtbl
   (:3579-3608) slot-for-slot (slot 0 do_i_care_about_power, slot 1
   get_sprite) — a separate family from the runtime-side RGE_Static_
   ObjectVtbl ordering (:3929; slot-29 recycle_in_to_game @0x74
   calibration). Decode: facet selection (building_connect_flag @468
   -> (facet param != 0) ? facet - 1 : 2, else building_facet @470);
   RGE_Sprite__draw(facet_sel, frame, x, y, x, y, color, area, 0xA, 0);
   CONNECTED-NEIGHBOR OVERLAY gated on on_build_make_obj @474 !=
   0xFFFF && sprite->draw_list_num @66 > 1 — walks sprite->draw_list
   @68 as RGE_Picture_List entries (idb_types.h:6356-6363: sprite @4,
   offset_x @8, offset_y @10, stride sizeof = 16 asm-calibrated; the
   RGE_Sprite fields :2989-2990 hand-walk-calibrated on id @130
   :3001): per entry skip self-reference, skip offset_y == 0
   (offset_x NOT tested), SDI_Draw_Line = saved + offset_y
   (repo constant test.cpp:1418 = .data:0077FA64, lst :1105270 —
   extern int* const pattern per the unified_map_offsets precedent),
   draw the neighbor with the SAME selected facet (entry->facet @12
   unused by the asm); the global restores on every exit path that
   entered the loop (0x5C79B4-79B8); count <= 1 exits before any
   modification. TWO RESTRUCTURES documented: dead count <= 0
   re-guard collapsed (0x5C7920-7929; count > 1 implies count > 0);
   the asm's per-iteration computed-entry-address test
   (0x5C793F-794A) = the compiler's partial hoist of the null-
   draw_list guard, hoisted to a single entries null-check (the i > 0
   in-loop deref with a null table is UB in the original; identical
   exit path through the restore). Outline pass = the Static base
   draw's verbatim shape: 4 get_point corners (+rx,-ry)/(+rx,+ry)/
   (-rx,+ry)/(-rx,-ry) on (*base_game)->world->map, two AddGDINode
   passes (+1 offsets/DrawColor 0/SortLine y+1, then exact/
   frame_color/SortLine y; GDI_Type 1, DrawLevel 0x0B). Param-slot
   reuse -> dedicated locals per the Static draw precedent. Collision
   audit: 0 (session 3-range re-run; functionlist.cpp:5390 dead-table
   footnote only). WRITTEN + post-write verification: x86 compile gate
   0 errors 0 warnings; defs/refs 2/2; installer 22 hooks (COMPLETE);
   base_game sweep 0 bare / 1 canonical code site; pointer-walk 0;
   SDI_Draw_Line extern + 3 code uses. NEXT: TRIBE_Master_Tree_Object
   enumeration + decode (6 exports @0x5C9780+, child of Static,
   master_type 15 — DECOMP_HIERARCHY.md tree note) — then the
   master-family project is COMPLETE (all 10 tiers).
 - TRIBE MASTER-TREE TIER — MASTER-FAMILY PROJECT COMPLETE (owner-
   approved + WRITTEN 2026-11-15): NEW CLASS FILE src/engine/object/
   TRIBE_Master_Tree_Object.{h,cpp} + install_object.cpp wiring +
   vcxproj/.filters entries. 6/6 exports owned (scalar_dtor/ctor2-4/
   make_new_obj/make_new_master — exactly the full IDB export list,
   extents lst :766699-:767049; NO setup/copy/modify/save overrides —
   the tier inherits Static's, so the Static hooks cover that
   dispatch): the THINNEST tier. Shapes: scalar deleting dtor =
   Static destructor call (the asm's j_ thunk 0x5C97A0 -> our idb
   constant 0x0048D200 :1914, hook-routed) + flags&1 ->
   operator_delete_internal, returns this (void*, :5140); ctor2/3/4 =
   base Static ctorN(do_setup=0) + Tree vfptr (0x006674A4) +
   if(do_setup) STATIC setupN (the asm calls the base setups directly
   — the Animated-ctor shape) + master_type = RGE_MASTER_TYPE_TREE
   (15, byte @+4); SEH folds -> Static destructor (chunks
   0x646780/A0/C0 — the immediate base, ladder verified); make_new_obj
   = NO recycle path, operator_new_internal(sizeof(TRIBE_Tree_Object))
   + runtime TRIBE_Tree_Object ctor1 (:4084, 0x00564DB0, tobj = this
   master, do_setup = 1); make_new_master = operator_new_internal(
   sizeof(TRIBE_Master_Tree_Object)) + Static COPY-CTOR(do_setup=0) +
   vfptr + Static setup + master_type = 15 AFTER the setup (asm
   0x5C9A03 — later store wins; differs from Combat/Building whose
   derived setups store it themselves). SIZEOF VERIFICATIONS:
   TRIBE_Tree_Object = RGE_Static_Object mirror (idb_types.h
   :21889-21940 vs :1438-1488) = 180 = 0xB4; TRIBE_Master_Tree_Object
   = Static master mirror ending at outline_radius_z, no extension
   (:22170-22250) = 208 = 0xD0 (the identical walk verified at 208 in
   the Static round). Caller coverage: ctor2/3/4 <- TRIBE_Master_
   Player__load_master_object/load_object + TRIBE_Player__load_master_
   object (the Tree DAT-load paths, lst :766729/:766795/:766866);
   dtor/factories vftable-dispatched. GATE SELF-CATCH: make_new_
   master's setup call shipped with this_ casted but new_master
   uncast — C2664 at the gate, fixed (both args casted per the
   cross-class rule), re-gated clean. Collision audit: 0 overlaps x6
   (scripted writer scan + address grep — dead profiler table only,
   functionlist.cpp:5414-5421 incl. the j_ thunk entry 0x5C97A0).
   WRITTEN + post-write verification: x86 compile gate 0 errors
   0 warnings on BOTH touched TUs (Tree cpp + install_object.cpp);
   defs/refs 6/6 (+3 benign comment mentions: the routing note and
   the two operator_new sizeof notes); installer 6 hooks; all 12
   Static-family calls casted (1 continuation-line form enumerated);
   return types match prototypes (void* / class-ptr x3 /
   RGE_Static_Object* / RGE_Master_Static_Object*); base_game 0;
   pointer-walk 0. HOOK TOTALS (2026-11-15): Static 37, Tree 6,
   Animated 17, Moving 17, Action 17, Combat 17, Doppleganger 10,
   Missile 13, TRIBE_Master_Combat 20, TRIBE_Master_Building 22 =
   176 master-family hooks. MASTER-FAMILY PROJECT COMPLETE — all 10
   tiers owned and hook-routed. Beyond it (per the ledger): runtime
   TRIBE_ classes' methods, Production_Queue_Record, the five derived
   ctor1 copy-constructors (optional), and the runtime-side
   TRIBE_Tree_Object (:4084) with the runtime tree decode.
 - DESYNC FIX — TRIBE_Master_Building_Object::check_placement_new
   desolid gate (owner-reported desync 2026-11-15, ROOT-CAUSED +
   fixed): the written desolid gate carried an INVENTED
   `gameType != 6` disjunct — a decode-time conflation of the desolid
   block (0x5C7D34-0x5C7D88, which contains NO gameType call) with
   the adjacent own-moving gate's gameType test (0x5C7DD3). TRUE asm
   semantics: desolid Building(80) blockers (group not 34/36) are
   SKIPPED unless player and owner are mutual allies; allies FALL
   THROUGH into the own-moving gate (D88) and the rest of the
   blocking pipeline (typically state != 2 there, so they reach
   can_be_built_on/group-31/rz/AABB and CAN block). The written code
   skipped allied desolid blockers whenever gameType != 6 (every
   standard game) -> placement decisions diverged from the original
   -> desync. FIX: the gameType disjunct removed; allies fall through
   (the sequential own-moving gate preserves the D88 flow — verified
   path-for-path, including ally-failure -> owner-id checks). Header
   comments corrected. Re-gated x86 0 errors 0 warnings.
   COMBAT(70) check_placement_new FULL RE-WALK (owner-directed, same
   session; asm 0x5C92E0-0x5C976F vs the written replacement,
   gate-by-gate): FAITHFUL — no divergence. Verified: the base
   pre-pass passes can_be_built_on_flag THROUGH (not 0) and zeroes
   check_objects/check_movible (asm push order 0x5C92FB-0x5C9328);
   the scan gate = check_objects (+28); the radii-zero gate returns 0;
   the group-27 radius_z STORE (0x5C9429: mov dword [edi+44h], 1.0)
   is asm-real — a persistent write, faithful in the replacement;
   the !can_be_built_on_flag && radius_z <= 0 -> base_result gate;
   the fly-over branch (both movement_type @+120 == 1 -> AABB vs
   master radii only -> return 6 / continue) precedes the
   blocker_is_enterable setup (ebp = 0 per object at loop top, set at
   0x5C9547); can_be_built_on == 1 -> blocker_is_enterable; THE
   DESOLID SKIP HERE IS UNCONDITIONAL (no ally check — type 80, group
   not 34/36, desolid -> continue, 0x5C9564-0x5C956C) — legitimately
   different from the Building tier's ally-gated version;
   check_movible 1/2 gates (type 70; type 70 + runtime speed @180
   > 0 via TRIBE_Combat_Object — guard = master_type == 70); rz
   substitution (group 27 OR rz==0 && type 80 && id != 50 -> 1.0
   local); blocker gate orx/ory/rz/this->radius_z all > 0; AABB
   strict; obstruction write; prog_mode @+0x1DC == 7 ->
   RGE_Game_World__object re-fetch -> master_obj_id @+0x428 != -1 ->
   runtime master_type >= 70 -> canPlayerObjectBeEntered(runtime,
   get_player(*base_game), master_obj_id, 0) == 1 -> return 9;
   blocker_is_enterable == 0 -> return 6 else continue.
   RGE_Master_Static_Object__check_placement_new (the grandbase) NOT
   re-walked this round (predates this session; triple-verified with
   an owner re-verification round) — available on request.
 - RUNTIME RGE_Static_Object CONSTRUCTION/TEARDOWN CLUSTER — RUNTIME
   LADDER BATCH 1 OPENS (owner-approved + WRITTEN 2026-11-15): 4 hooks
   added to src/engine/object/RGE_Static_Object.cpp (ctor @0x0054B2E0
   [..,0x54B366), ctor_read @0x0054B600 [..,0x54B66F), destructor
   @0x0054B680 [..,0x54B86A), recycle_out_of_game @0x0054C0A0
   [..,0x54C293); prototypes idb_functions.h:3774/:3787/:3788/:3802).
   BAKE-IN INVENTORY (8 patches, all folded, call sites retired):
   casts.cpp UNIT_EXTRA scheme — the patch stores a UNIT_EXTRA* across
   damaged_lately_timer's upper 3 bytes + the padding byte @+0x87
   (getUnitExtra/addUnitExtra/removeUnitExtra, casts.cpp:763-849;
   createUnitExtra mallocs patch-side); ctor + ctor_read bake in the
   unit_constructor_asm_fix dword-clears (casts.cpp:897/898, 18 bytes
   over the original byte-stores at +0x84/85/86); destructor + recycle
   bake in removeUnitExtra (casts.cpp:895/896 entry hooks — non-
   destructive prologue replays; the patch's bare malloc/free pairing
   stays INSIDE casts.cpp, keeping the engine TU CRT-clean).
   los.cpp scheme — ei.theObj (the LOS_ExploreInfo lead pointer
   @+0x90, repurposed as UNIT_LOS_DATA*) nulled in both ctors and
   freed via delete_unit_los_data (los.cpp:6) in dtor/recycle
   (los.cpp:1023-1026; all four stubs are non-destructive prologue
   replays). NEW GROUNDINGS: runtime Static vftable 0x00662C8C
   (lst :994791); RGE_Static_ObjectVtbl exit_obj @+0x154 = index 85;
   RGE_PlayerVtbl add_attribute_num @+0x80 = index 32, removeObject
   @+0x124 = index 73; RGE_New_Object_List = {List @0,
   Number_of_objects @4, ListSize, GrowSize, DeletingAll};
   RGE_Obstruction_ManagerVtbl RemoveObjectObstruction @+0x2C = index
   11, DeleteObList @+0x40 = index 16 (the Airspace twin slot-
   identical); RGE_Tile::ObjectDeleteLock @+0xF; OBSystem_Land/Air =
   common.h:109-110 (pointer-to-slot). DECODE NOTES: the recycle's
   faithful differences vs the destructor (delete_list DIRECT call vs
   vfptr[0](1); NO pathing-group free, NO mMaximumPathingGroupSize
   clear, NO mGroupID/BUnitGroup removal; the mROOAlreadyCalled gate
   only on the destructor's Remove call; master_obj = 0 at recycle
   end but not at destructor end; the non-recyclable and recycle-
   failed self-destruct paths call vfptr[0](1)). GATE SELF-CATCH:
   two world dual-naming casts (TRIBE_World* -> RGE_Game_World*)
   added at gate (the getUnitGroup + recycle calls), matching the
   Combat(70) precedent. CASTS.CPP PRE-EXISTING ERRORS surfaced by
   the gate (2x C2664 modf(float*) at casts.cpp:62/:85 — float intp
   should be double; PRE-EXISTING, unrelated to the retirements;
   casts.cpp IS in the vcxproj (:435) — FIXED same session with owner
   approval: #include <cmath> added to casts.cpp (:3 — the C++
   modf(float, float*)/abs(float) overloads resolve; zero semantic
   change); casts.cpp re-gated 0 errors 0 warnings). IDB REGEN ROT SWEEP
   (owner regenerated idb_types.h, 37,047 -> 37,087 lines:
   damaged_lately_timer int -> u8 + unit_extra_padding_1..4 named in
   ALL 10 runtime structs; masters/vtbls untouched; layouts byte-
   identical; all cited offsets hold; line citations shifted +4 per
   edited struct in file order — Static runtime :1438 SAME,
   New_Object_List :4197, PlayerVtbl :7125, managers
   :5820/:5868/:9943/:10146, Missile runtime :20961, Doppleganger
   runtime :21500, Tree runtime :21925). WRITTEN + post-write
   verification: x86 gate 0 errors 0 warnings (RGE_Static_Object.cpp
   + los.cpp; casts.cpp excluded pending the owner's modf decision);
   defs/refs 5/5 (+ the existing gbg_get_civ_override pair);
   installer 5 hooks; base_game 0 bare / 4 canonical; CRT 0;
   pointer-walk 0; retirements verified (0 active cluster sites;
   los.cpp's 3 unrelated hooks still active). RUNTIME LADDER
   EVIDENCE: pending (the runtime ctor base-call chains are Batch 1b
   scope with the derived runtime ctor trios).
 - DESYNC FIX #2 — runtime RGE_Static_Object ctor/ctor_read dropped
   store (owner-reported desync 2026-11-15, ROOT-CAUSED + fixed):
   both ctors DROPPED the mROOAlreadyCalled = 0 store — asm 0x54B327
   (ctor) / 0x54B64A (ctor_read): mov [esi+8Ch], al — the store
   immediately AFTER the writeData patch region, part of the ORIGINAL
   body (not patch code). Garbage mROO suppresses the destructor's
   RemoveObjectObstruction gate (mROOAlreadyCalled == 0 required) ->
   stale obstruction state -> simulation divergence -> desync. FIX:
   the store restored in both ctors (after the patch-clear block, the
   asm position). ALSO: the destructor's los bake-in free moved AFTER
   remove_visible_resource (the los hook @0x54B691 sits after the
   vfptr store AND the remove_visible_resource call in the patched
   flow — the first written position ran it first). Re-gated x86
   0 errors 0 warnings. PROTOCOL HARDENING (proposed): post-write
   verification for construction/teardown replacements must include
   a MECHANICAL store-list diff — enumerate every mov [reg+disp]
   store in the asm range and check each against the written
   function; the manual full-asm pass dropped this store because the
   patched flow interleaves patch stores with original stores, and
   attention anchored on the patch region.
   RATIFIED same session: the store-list diff is now a mandatory
   protocol step (DECOMP_TEMPLATE.md, Cleanup rules, after the
   compile-gate bullet). RETROACTIVE PROOF on the runtime Static
   cluster (scripted byte-granularity diff: asm mov [reg+disp]
   stores width-expanded, plus the baked patch/los stores, vs the
   written functions' field-write coverage): ctor 69/69 bytes FULL
   MATCH, ctor_read 69/69 FULL MATCH, destructor 44/44 FULL MATCH,
   recycle_out_of_game 32/32 FULL MATCH — and on the PRE-fix written
   ctors the diff flags exactly the shipped hole (0x8C missing from
   both) — the failure class is now mechanically detected.
   DESYNC PERSISTED post-fix; TRAMPOLINE EXPERIMENT (owner-approved):
   destructor_new runs removeUnitExtra + los-free + the ORIGINAL body
   via a code-copied trampoline. TWO DEFECTS FOUND: (1) the experiment
   double-called remove_visible_resource (my explicit call + the
   original body's) — owner caught, commented mine; (2) OWNER
   DIAGNOSIS: the trampoline's xor ebx, ebx ran BEFORE push ebx, so
   the body's epilogue (pop ebx) restored 0 into the CALLER's EBX —
   callee-saved trash, illegal, desync. MECHANISM fully mapped: the
   original establishes ebx = 0 MID-FUNCTION via the los stub's xor
   at 0x0054B694, while the prologue's push ebx (0x0054B681) saves
   the caller's value for the epilogue's pop (0x0054B869; epilogue
   re-verified: pop edi/esi/ebx/ecx, retn, 4 pops, NO pop ebp — the
   refund path's ebp push/pop is internally balanced at
   0x54B6A0/0x54B702). TRAMPOLINE v3 FIX: xor ebx, ebx MOVED AFTER
   the three register saves (51 53 56 8B CE 31 DB + jmp 0x0054B685) —
   caller's ecx/ebx/esi saved with incoming values and restored by
   the body's own pops; ebx = 0 for the body only. Re-gated x86
   0 errors 0 warnings. PROTOCOL NOTE (new rule class): hook
   replacements folding mid-function stubs must replicate the stubs'
   REGISTER-MODIFICATION effects, not just their memory effects —
   the store diff does not cover register-lifecycle deltas (ebx = a
   live zero-constant for the body here; resolved - see DESYNC FIX #3
   below).
 - RUNTIME RGE_Animated_Object CONSTRUCTION/DESTRUCTION CLUSTER —
   RUNTIME LADDER BATCH 1b-1 (owner-approved + WRITTEN 2026-11-15):
   NEW CLASS FILE src/engine/object/RGE_Animated_Object.cpp (+ installer
   + install_object.cpp wiring + vcxproj/.filters; NO class header —
   the runtime cpps stand on stdafx alone, matching the runtime Static
   precedent). 2 hooks: ctor @0x00422980 [0x422980,0x422A3D) (lst
   :62193/:62283, retn 18h; prototype :538) + vector_deleting_
   destructor @0x0046FE70 [0x46FE70,0x46FE8B) (lst :189152/:189171,
   retn 4; prototype :1481). STRUCTURAL FINDING: the runtime Animated
   class has NO destructor body of its own - the vdel calls
   j_RGE_Static_Object__destructor (the thunk -> the HOOKED runtime
   Static destructor, routes into destructor_new), then flags & 1 ->
   operator_delete, return this. CTOR DECODE: base Static runtime
   ctor(do_setup=0, hooked -> routes) + Animated vfptr (0x006566A0,
   lst :987149) + if(do_setup): the RUNTIME Static setup(tobj,
   obj_owner, x, y, z), type @+0x5E = 0x14 (the RUNTIME object's tier
   mirror - idb field `type` @94, distinct from master_type @+4 on
   masters), speed @+0xB4 = 0 (the Animated extension field,
   idb_types.h:16113; runtime size 184 = 0xB8), set_sleep_flag(0)
   (:3891), and the sprite-list bootstrap: if sprite_list @+0x24 == 0
   -> sprite_list = vfptr->create_sprite_list (RGE_Static_ObjectVtbl
   slot +0x2A8 = index 170, idb_types.h:4105; slot arithmetic
   NOTE: post-regen index = member line - struct line - 2), sprite =
   0, master->vfptr->get_sprite(master, (RGE_Static_Object*)this)
   (MASTER Animated vtbl slot +4 = index 1, :15926 - the master-side
   polymorphic sprite getter), vfptr->new_sprite(this, sprite)
   (runtime vtbl slot +0xB0 = index 44). SEH fold: chunk 0x640F70 =
   mov ecx, [ebp-10h] / jmp RGE_Static_Object__destructor (the
   HOOKED runtime Static destructor). GATE SELF-CATCH: the full
   cross-class cast set (this_ is the DERIVED Animated runtime type
   here - 6 (RGE_Static_Object*) casts incl. the SEH fold's
   destructor call + 2 (RGE_Master_Static_Object*) tobj casts) -
   caught by the gate, fixed, re-gated clean. Collision audit:
   writer scan + address-bytes grep over both ranges = 0 overlaps,
 0 hits (the j_ thunk @0x422B60 sits outside both ranges and routes
   into the owned Static destructor - beneficial). WRITTEN +
   post-write verification: x86 gate 0 errors 0 warnings; defs/refs
   2/2; installer 2 hooks; CRT 0; pointer-walk 0; base_game 0.
   RUNTIME LADDER: Animated's destruction terminates at the base
   (no derived cleanup exists); the ladder evidence for the
   hierarchy file (runtime Animated ctor -> Static runtime ctor,
   do_setup = 0). NEXT: Doppleganger runtime cluster (ctor
   @0x0046FBF0 + ctor_read @0x0046FE90; destruction = the SHARED
   Animated vdel slot per lst :189154 - already owned), then Moving
   /Action/Combat/TRIBE_Combat/TRIBE_Building runtime clusters.
   DESYNC FIX #3 — FINAL ROOT CAUSE (owner-identified 2026-11-15,
   fixed + gated): the destructor AND recycle obstruction gates read
   master_obj->hp — MISLABELED FIELD. The asm reads fld dword ptr
   [eax+44h] with eax = master_obj: 0x44 = 68 DECIMAL = radius_z
   (radius_x @60, radius_y @64, radius_z @68); hp lives at 50 decimal
   (0x32). ROOT CAUSE of the mislabel: hex/decimal struct-table
   confusion during the first decode pass, then anchored by a
   semantically-plausible narrative ("hp > 0 = alive -> remove
   obstruction") that four review passes confirmed the gate STRUCTURE
   (test ah,41h = skip when <= — correct) without re-checking WHICH
   FIELD the fcomp reads. WHY THE STORE DIFF MISSED IT: the field is
   a READ, not a store — the store-list diff has no read coverage.
   BOTH SITES FIXED (destructor :239 — owner had already corrected
   that site themselves; recycle :382 — fixed this round) + the two
   header comments. TRAMPOLINE EXPERIMENT VERDICT: worked exactly as
   designed — prepends + original body = clean, my C body = desync —
   isolating the C body, and the residual bug is now this field
   mislabel (the v2 experiment's double remove_visible_resource call
   and the EBX trash were separate experiment artifacts, both
   resolved). DIAGNOSTIC TRAMPOLINE REMOVED (owner-approved) — the
   pure C body (with radius_z) is the ship. STORE-AND-READ DIFF
   RATIFIED into DECOMP_TEMPLATE.md (the store-list bullet extended
   with read-offset-to-census-field mapping coverage). NEXT: owner
   re-test of the pure C body; then Batch 1b (derived runtime
   ctor/destructor trios) under the extended diff protocol.
 - BATCH 1b-2 DECODE STATE (Doppleganger + Moving runtime clusters —
   FULLY DECODED, NOT YET WRITTEN; the new session writes from this
   record). COLLISION AUDIT FOUND THE MISSED PATCHES: mapsize.cpp
   :1739 writeDword(0x46FCC4, (DWORD)mapptr) = the disp32 of
   `mov ecx, unified_map_offsets[edi*4]` inside the Dopple runtime
   CTOR; :1740 writeDword(0x46FF43, ...) = the same disp32 replace
   inside the Dopple ctor_read (`mov edx, unified_map_offsets[ebx*4]`)
   — THE MAP_SIZE PATCH retargets the dopple tile-row table base from
   unified_map_offsets (0x7A1CBC) to mapptr (the big-map base) when
   the big-map mod is active. THE DRAFT HARDCODED unified_map_offsets
   -> wrong table under the big-map -> THE DESYNC. PROPOSED FIX
   (pending owner decision): bake in via `extern int* mapptr;`
   (mapsize.cpp:238 — int* mapptr = (int*)0x7A1CBC = unified_map_
   offsets' address by default; :1680-1682 re-points it for the
   big-map) and read the row as mapptr[ftol(y)], tile = mapptr[row]
   + ftol(x)*4; retire the 2 writeDword call sites. (Alternative:
   the // MAP_SIZE waiver like get_visible — NOT recommended here:
   the dopple tile on the wrong table is functional divergence.)
   === DOPPLEGANGER RUNTIME (2 hooks proposed) ===
   CTOR RGE_Doppleganger_Object__RGE_Doppleganger_Object @0x0046FBF0
   [0x46FBF0,0x46FE66) (lst :188929/:189143, retn 1Ch = 7 stack args:
   tobj, obj_owner, x, y, z, do_setup, obj_to_dopple; prototype
   idb_functions.h:1480; caller: RGE_Master_Doppleganger_Object__
   make_new_obj+AA). SEH chunk 0x641CD0 = jmp j_RGE_Static_Object__
   destructor. BODY: base RGE_Animated_Object__RGE_Animated_Object
   (0x422980 — NOW HOOKED, routes) with do_setup = 0; vfptr =
   RGE_Doppleganger_Object__vftable @0x0065A2C0 (lst :989403); the
   UNCONDITIONAL dopple-extension zero block: doppled_object
   (@+0xB8) = 0, draw_color_owner (@+0xBC) = 0, doppled_master_obj
   (@+0xC0) = 0, doppled_player (@+0xC4) = 0, MapAddress (@+0xC8)
   = 0, LastMapValue (@+0xCC) = 0, CantSeeBits (@+0xD0) = 0,
   map_draw_level (@+0xD4) = 0, map_color (@+0xD5) = 0,
   Destroy_Next_Turn (@+0xD7) = 0, Multi_Unified_Points (@+0xD6)
   = 0, doppledObjectID (@+0xD8, int) = -1 (store order per asm
   0x46FC43-0x46FC89: B8/BC/C0/C4/C8/CC/D0 dwords, D4/D5 bytes,
   D8 = -1 dword, D6/D7 bytes). IF do_setup: RGE_Animated_Object__
   setup (:548, 0x422C30 — runtime Animated setup, base call via the
   idb constant); type @+0x5E = 0x19 (25 — the DOPPLEGANGER runtime
   tier mirror); LastMapValue = 0; CantSeeBits = 0; MapAddress =
   mapptr[ftol(y)] + ftol(x)*4 (THE BAKED PATCH SITE — asm
   0x46FCC1-0x46FCD7: ftol(x) -> eax, ftol(y) -> edi, row =
   table[edi], tile = row + eax*4). THEN unconditional:
   doppled_object = obj_to_dopple (asm 0x46FCE3). IF obj_to_dopple:
   draw_color_owner = src->owner (@+0x18); doppled_player =
   src->owner->id (@+0xA0); doppledObjectID = src->id (@+0x10);
   sprite = src->sprite (@+0x1C); THE GATE: if src->master_obj->
   master_type == 80 && master_obj->object_group != 8 &&
   master_obj->id != 335 && master_obj->construction_id (@+0x20C =
   524, word, signed cmp > 0): sprite = owner->master_objects
   (@+0x74)[master_type] ->vfptr->get_sprite(that master, src)
   (master vtbl slot +4), swap flag dl = 1. THEN the sound/temp
   block (asm 0x46FD54-0x46FDA1): old = temp_disable_sound_playing
   (a u8 GLOBAL — extern needed; declared in the game/patch — grep
   its definition); temp_disable_sound_playing = 1; src_list =
   src->sprite_list (@+0x24); if src_list != 0 AND swap flag == 0:
   temp = RGE_Active_Sprite_List__copy_sprite_list(src_list);
   this_->sprite_list->use_sprite_list(temp) — else:
   this_->sprite_list->delete_list() (NO null guard in the asm —
   faithful); if this_->sprite != 0: this_->sprite_list->
   add_sprite(this_->sprite, 0, 0, 0, 0); restore
   temp_disable_sound_playing = old. THEN: this_->selected @+0x45
   = src->selected (@+0x45); RGE_Static_Object__get_animation_frame
   (src) (:3853, 0x5513A0) -> RGE_Static_Object__set_animation_frame
   (this, frame) (:3852, 0x551380); draw_color_owner = src->owner
   (asm 0x46FDC4); map_draw_level = src->master_obj->map_draw_level
   (@+0x9E = 158); if map_draw_level == 3: map_color =
   src->master_obj->map_color (@+0xA0 = 160) else 0 (asm
   0x46FDD3-0x46FE16 hmm 0x46FDD6: mov cl, [edx+9Eh] = @158; store
   D4; cmp 3; D5 = master+0xA0 if ==3 else 0); Multi_Unified_
   Points = src->[+0x16C = 364] (byte; ONLY if src->master_obj->
   master_type >= 50 — asm jb skips); if src->object_state >= 7:
   doppled_object = 0 AND doppledObjectID = -1 (the source-died
   binding release). IF obj_to_dopple == 0 (the no-source path,
   asm 0x46FE2E): doppledObjectID = -1, draw_color_owner = 0,
   doppled_player = 0, RGE_Static_Object__destroy_obj(this)
   (:3843, 0x5506B0 — SELF-DESTRUCT). THEN validate():
   RGE_Doppleganger_Object__validate (:1489, 0x4705F0). RETURN this.
   CTOR_READ RGE_Doppleganger_Object__RGE_Doppleganger_Object_read
   @0x0046FE90 [0x46FE90,0x46FFF4) (lst :189180/:189291, retn 0Ch =
   3 stack args: infile, gworld, do_setup; prototype :1482; callers
   RGE_Object_List__load / RGE_Player_Object_List__Load). SEH chunk
   0x641CF0 = jmp j_RGE_Static_Object__destructor. BODY: base
   RGE_Animated_Object__RGE_Animated_Object_read (0x422B70 — grep
   the idb constant line; NOT hooked) with do_setup = 0; vfptr =
   Dopple vftable; the SAME unconditional zero block; if do_setup:
   RGE_Animated_Object__setup_read (grep idb line, 0x422CA0-ish) +
   type = 0x19 + tile from the LOADED world_y/world_x (@+0x4C/
   @+0x48) via mapptr (THE BAKED PATCH SITE — asm 0x46FF35-0x46FF5B
   — disp32 writeDword at 0x46FF43). THEN the UNCONDITIONAL
   rge_read stream sequence (rge_read = __fastcall(infile, buffer,
   size), idb_functions.h:3003): (4) -> doppled_object;
   (1) -> map_draw_level; doppledObjectID = -1 (asm 0x46FF7E);
   (4) -> &map_color (CROSS-FIELD: map_color @D5 +
   Multi_Unified_Points @D6 + Destroy_Next_Turn @D7 +
   doppledObjectID low byte @D8 — the doppledObjectID reads
   0xFFFFFFxx after — FLAG AS THE DELIBERATE SPAN, deviation
   protocol); (4) -> doppled_master_obj (@+0xC0); (4) ->
   doppled_player (@+0xC4..C7); (4) -> CantSeeBits (@+0xD0..D3);
   (1) -> Multi_Unified_Points (@+0xD6). retn.
   DOPPLE EXTENSION FIELD MAP (idb_types.h :21545-21567, post-regen;
   runtime size 220 = 0xDC = Animated 0xB8 + the 36-byte dopple
   extension @0xB8-0xDB): doppled_object @0xB8 (RGE_Static_Object*),
   draw_color_owner @0xBC (RGE_Player*), doppled_master_obj @0xC0
   (RGE_Master_Static_Object*), doppled_player @0xC4 (int),
   MapAddress @0xC8 (unsigned int*), LastMapValue @0xCC (unsigned
   int), CantSeeBits @0xD0 (unsigned int), map_draw_level @0xD4
   (u8), map_color @0xD5 (u8), Multi_Unified_Points @0xD6 (u8),
   Destroy_Next_Turn @0xD7 (u8), doppledObjectID @0xD8 (int).
   === MOVING RUNTIME (4 hooks proposed) ===
   CTOR RGE_Moving_Object__RGE_Moving_Object @0x004A0540
   [0x4A0540,0x4A0691) (lst :273287/:273391, retn 18h = 6 stack
   args; prototype :2122; callers RGE_Action_Object__RGE_Action_
   Object+3D (the runtime ladder) + make_new_obj). SEH chunk
   0x6426F0 = jmp j_RGE_Static_Object__destructor. BODY: base
   RGE_Animated_Object__RGE_Animated_Object (HOOKED, routes,
   do_setup = 0); `or eax, -1`; THE MOVING EXTENSION INIT BLOCK
   (all fields census-cited idb_types.h :6747-:6775): trail_
   remainder (@+0xB8) = 0, velocity_x (@+0xBC) = 0, velocity_y
   (@+0xC0) = 0, velocity_z (@+0xC4) = 0, angle (@+0xC8) = 0,
   turn_towards_time (@+0xCC) = 0, on_ground (@+0xD0, u8) = 0,
   turnTimer (@+0xD4) = 0, continueCounter (@+0xD8) = 0,
   currentTerrainException1 (@+0xDC) = -1, currentTerrainException2
   (@+0xE0) = -1, waitingToMoveValue (@+0xE4, u8) = 0,
   numberWaitDelays (@+0xE5, u8) = 0, userDefinedWaypoints
   (@+0xE8) = 0, positionValue (@+0xF4, BVector = {float x, y, z},
   idb_types.h :1761-1765) = {0, 0, 0}, orientationForwardValue
   (@+0x100) = {1.0, 0, 0}, orientationRightValue (@+0x10C) =
   {0, 0, -1.0}, pathData (@+0x118) = 0, mFuturePathData
   (@+0x11C) = 0, movementData (@+0x120) = 0, lastMoveTimeValue
   (@+0x124) = 0, mHasSubstitutePosition (@+0x128) = 0,
   mSubstitutePosition (@+0x12C) = {0, 0, 0},
   mConsecutiveSubstituteCount (@+0x138) = 0; vfptr =
   RGE_Moving_Object__vftable @0x0065BE14 (lst :990431);
   RGE_Moving_Object__initMovementData (idb_functions.h:2208,
   0x4A6A70); if do_setup: RGE_Moving_Object__setup (:2127,
   0x4A0940). RETURN this. (NOT stored by the ctor: numberUser-
   DefinedWaypointsValue @+0xEC, maximumNumberUserDefinedWaypoints
   Value @+0xF0 — the asm leaves them.) VDEL
   RGE_Moving_Object__vector_deleting_destructor @0x004A06A0
   [0x4A06A0,0x4A06BB) (lst :273400/:273418, retn 4; prototype
   :2123): RGE_Moving_Object__destructor (:2125, 0x4A0800 — IN
   BATCH, hook-routed) + flags & 1 -> operator_delete + return
   this. CTOR_READ RGE_Moving_Object__RGE_Moving_Object_read
   @0x004A06C0 [0x4A06C0,0x4A07F1) (lst :273427/:273515, retn 0Ch
   = 3 stack args: infile, gworld, do_setup; prototype :2128):
   SEH chunk 0x642710 (jmp j_RGE_Static_Object__destructor); base
   RGE_Animated_Object__RGE_Animated_Object_read (0x422B70, do_
   setup = 0); the SAME extension init block (or eax, -1 first);
   vfptr; initMovementData; if do_setup: RGE_Moving_Object__
   setup_read (:2128, 0x4A0A40). RETURN this. DESTRUCTOR
   RGE_Moving_Object__destructor @0x004A0800 [0x4A0800,0x4A084E)
   (lst :273524/:273555), void, __thiscall. SEH chunk 0x642730
   (jmp j_RGE_Static_Object__destructor — the throw-during-
   destruction unwind: the fold form = __except(base destructor,
   EXCEPTION_CONTINUE_SEARCH) per the ctor-fold precedent). BODY:
   vfptr = Moving vftable (0x0065BE14); RGE_Moving_Object__
   initMovementData (RESETS the movement data — NOT a free: the
   runtime Moving holds no heap of its own); RGE_Static_Object__
   destructor ((RGE_Static_Object*)this_ — HOOKED, routes). NO
   frees. CALLER: RGE_Action_Object__destructor+4F (the runtime
   ladder). COLLISION SCAN: 0 writer overlaps + 0 address hits
   over all 6 ranges (the Animated vdel @0x46FE70 is OUTSIDE the
   Dopple ranges and already owned). BOTH CLASSES TEST PENDING
   (write, gate, owner re-test) — then the ladder continues:
   Action/Combat/TRIBE_Combat/TRIBE_Building runtime clusters.

  - BATCH 1b-2 WRITTEN (owner-approved + WRITTEN 2026-11-15): 6 hooks -
    Doppleganger runtime ctor @0x0046FBF0 [0x46FBF0,0x46FE66) + ctor_read
    @0x0046FE90 [0x46FE90,0x46FFF4) -> NEW CLASS FILE src/engine/object/
    RGE_Doppleganger_Object.cpp; Moving runtime ctor @0x004A0540
    [0x4A0540,0x4A0691), vdel @0x004A06A0 [0x4A06A0,0x4A06BB), ctor_read
    @0x004A06C0 [0x4A06C0,0x4A07F1), destructor @0x004A0800
    [0x4A0800,0x4A084E) -> NEW CLASS FILE src/engine/object/RGE_Moving_
    Object.cpp; NO class headers (runtime cpps stand on stdafx, per the
    runtime Animated precedent); install_object.cpp wiring (runtime family
    grouped Static -> Animated -> Doppleganger -> Moving) + vcxproj/
    .filters entries. Vftables: Dopple runtime 0x0065A2C0 (lst :989403),
    Moving runtime 0x0065BE14 (lst :990431), cpp-local typed constants;
    new binding: temp_disable_sound_playing = static char* const
    @0x0077FA90 (lst :1105298; .c:44143) - cpp-local, no common.h entry.
    MOVES: Moving vdel calls RGE_Moving_Object__destructor DIRECTLY (hook-
    routed into destructor_new); Moving destructor = vfptr + initMovementData
    (a RESET, not a free - no heap of its own) + runtime Static destructor;
    Moving ctor/ctor_read store the vfptr AFTER the extension init block
    (asm 0x4A0654/0x4A07C3) and leave numberUserDefinedWaypointsValue @0xEC
    + maximumNumberUserDefinedWaypointsValue @0xF0 UNSTORED (faithful);
    Dopple ctor_read zero block has NO draw_color_owner store (asm 0x46FED8-
    0x46FF15); Dopple ctor/ctor_read zero-block store order B8/BC/C0/C4/C8/
    CC/D0 dwords, D4/D5 bytes, D8=-1, D6/D7 bytes replicated exactly.
    MAP_SIZE DISPOSITION REVERSED (owner 2026-11-15, supersedes the decode
    entry's mapptr bake-in proposal): mapsize patches are NEVER baked - the
    Dopple replacements keep the original's unified_map_offsets access with
    // MAP_SIZE markers (the get_visible precedent); mapsize.cpp:1739/1740
    (disp32s 0x46FCC4 / 0x46FF43) retired in place with the not-baked
    reason; TEMPLATE AMENDED (step 3b): MAP_SIZE exception codified. Big-map
    dopple divergence intentionally not carried.
    DECODE-STATE CORRECTION: the decode record's "selected @+0x45" was a
    field-name mislabel - offset 0x45 = facet (idb_types.h:1460; selected
    @0x46); the pseudocode/asm agreed on offset, written as facet.
    GATE (x86 Hostx64\x86 cl 19.44.35229): 4/4 TUs 0 errors 0 warnings
    (Dopple cpp, Moving cpp, install_object.cpp, mapsize.cpp). GATE CATCH:
    draw_color_owner = src->owner needed the TRIBE_Player* -> RGE_Player*
    dual-naming cast (C2440; Combat(70) get_player precedent).
    DEVIATION APPLIED (decode-record approved, flagged at the call): the
    ctor_read 4-byte rge_read at &map_color spans D5/D6/D7/D8 -
    doppledObjectID reads 0xFFFFFFxx after (asm 0x46FF91-0x46FF9B);
    sizeof(&map_color) would be 1.
    STORE-AND-READ DIFF (scripted, byte-granularity): FULL MATCH all six -
    ASM-NOT-COVERED 0 and WRITTEN-NOT-IN-ASM 0 per function (dopple_ctor
    17/16 sites-fields, dopple_ctor_read 15/13 incl. the [ebp+0] = +0xD0
    alias resolution, moving_ctor 33/33, moving_ctor_read 33/33, vdel 0/0,
    destructor 1/1 vfptr-only); read census mapping complete (0x10 id, 0x14
    master_obj, 0x18 owner, 0x1C sprite, 0x24 sprite_list, 0x04 master_type,
    0x1E object_group word, 0x18 id word (master), 0x45 facet, 0x58
    object_state, 0x74 master_objects, 0x9E map_draw_level, 0xA0 map_color /
    id@160 (TRIBE_Player), 0xB8/0xC0 dopple ext, 0x16C Multi_Unified_Points
    (RGE_Combat_Object :7411 - the base-pointer v20[2].gbg_civ_override =
    2*0xB4+0x4 resolution), 0x20C construction_id (= v10[2].
    construction_radius_x = 2*0xD0+0x6C, Building tier @524), 0x4C/0x48
    world_y/world_x (ctor_read flds)). REGISTER-LIFECYCLE CHECK: 0 asm/
    naked/trampoline constructs in both files - nothing trampoline-style
    exists; the originals' mid-function ebx=0 idiom (0x46FC1A/0x4A056A/
    0x4A06E2) is compiler-internal to the replaced bodies, nothing to
    replicate. CASTS: 13 (Dopple) / 6 (Moving), every base-call argument
    position cased; structural audit 4/4 construction functions (vfptr +
    do_setup guard + body after guard); return types match prototypes x6;
    CRT 0; base_game 0 bare; pointer-walk 0; literal loop bounds 0.
    COLLISION RE-SCAN (scripted step 3b): 2,740 writer sites, the only 6
    range overlaps = this batch's own installer hooks; functionlist.cpp
    table hits = dead profiler data (standing disposition). defs/refs 2/2
    + 4/4; installer 2 + 4 hooks (runtime ladder total now 13: Static 5,
    Animated 2, Doppleganger 2, Moving 4). BOTH CLASSES TEST PENDING
    (owner in-game re-test). NEXT: Action runtime cluster decode -> present
    -> approve -> write; then Combat / TRIBE_Combat / TRIBE_Building.

  - RUNTIME RGE_Action_Object CONSTRUCTION/TEARDOWN CLUSTER - RUNTIME
    LADDER BATCH 1b-3 (owner-approved + WRITTEN 2026-11-15): NEW CLASS
    FILE src/engine/object/RGE_Action_Object.cpp (+ installer + install_
    object.cpp wiring + vcxproj/.filters; NO class header - runtime cpps
    stand on stdafx). 6 hooks: ctor @0x00408980 [0x408980,0x408A2B)
    (:107), vdel @0x00408C50 [..,0x408C6B) (:119), ctor_read @0x00408C70
    [..,0x408D1B) (:120), destructor @0x00408D20 [..,0x408D83) (:121),
    recycle_out_of_game @0x00408D90 [..,0x408DAA) (:122),
    recycle_in_to_game @0x00408DB0 [..,0x408DDB) (:123). Vftable
    .rdata:00654E54 (lst :986405), cpp-local constant. DECODE: base
    Moving ctor/ctor_read(do_setup=0, hooked -> routes) + Action vfptr +
    if(do_setup): the MOVING setup/setup_read (idb constants :2127/:2128 -
    the asm calls Moving's setups directly, Animated-ctor shape), type
    @+0x5E = 40 (0x28), waiting @+0x140 = 0, actions @+0x13C =
    vfptr->create_action_list (slot +0x3B0, member :11834), command_flag
    @+0x141 = 0, Selected_Group_Info @+0x142 (word) = 0; ctor_read
    streams (1)->waiting, (1)->command_flag, (2)->Selected_Group_Info
    (all sizeof-exact) then RGE_Action_List__load (:59, 0x4063B0);
    destructor: vfptr + if(actions) vfptr->__vecDelDtor(actions, 1)
    (RGE_Action_List vtbl slot 0, :15046; Action_List vftable
    .rdata:00654AE0 lst :986295) + actions = 0 + RGE_Moving_Object__
    destructor (hooked -> routes); recycle_out: if(actions)
    RGE_Action_List__delete_list (:62, 0x4064C0 - delete_list, NOT the
    deleting destructor; list object stays allocated) +
    RGE_Static_Object__recycle_out_of_game (:3802, hooked -> routes),
    NO actions=0 store; recycle_in: base Moving recycle (:2126, 0x4A0850,
    NOT hooked) + waiting = 0. LADDER EVIDENCE (runtime side now fully
    verified): all three Action SEH chunks (0x640A30/A50/A70) jmp
    RGE_Moving_Object__destructor - the IMMEDIATE base (hooked, routes);
    Action -> Moving -> Static complete. Caller coverage: ctor <-
    RGE_Combat_Object__RGE_Combat_Object+3D + Action make_new_obj;
    ctor_read <- Combat runtime read +2B + Object_List__load; destructor
    <- vdel + Combat destructor +86; recycle_out <- Missile/Building
    recycles call UP into it; recycle_in <- Combat recycle_in_to_game+55.
    EXTENSION MAP: actions@316(0x13C), waiting@320(0x140),
    command_flag@321(0x141), Selected_Group_Info@322(0x142, i16);
    sizeof 324 = 0x144 (:7433-7518). GATE (x86 Hostx64\x86 cl
    19.44.35229): 2/2 TUs 0 errors 0 warnings. STORE-AND-READ DIFF
    (scripted): FULL MATCH - ctor 6/6, vdel 0/0, destructor 2/2,
    recycle_out 0/0, recycle_in 1/1; ctor_read's 0x140/141/142 are
    rge_read-mediated (lea edx forms), read inventory 000/140/141/142
    (dopple ctor_read precedent). CASTS 10 sites (this_ Action->Moving x7,
    Action->Static x1, tobj Master_Action->Master_Moving x2), every
    base-call position cased; CRT 0; base_game 0; pointer-walk 0; loops 0;
    asm/trampoline 0 (register-lifecycle check clean); defs/refs 6/6;
    structural audit pass; return types match prototypes x6. COLLISION
    RE-SCAN: 2,746 writer sites, only overlaps = this batch's own
    installer hooks. Runtime ladder total now 19 hooks (Static 5,
    Animated 2, Doppleganger 2, Moving 4, Action 6). DEVIATION-APPROVAL
    RULE AMENDED (owner-directed, TEMPLATE amended): decode-record flags
    and batch write orders are NOT deviation approval - every future
    discrepancy needs explicit per-item owner sign-off (the dopple
    ctor_read 4-byte span was individually CONFIRMED by the owner
    2026-11-15, kept binary-stream-faithful). NEXT: Combat runtime
    cluster decode -> present (with code) -> approve -> write; then
    TRIBE_Combat / TRIBE_Building.

  - RUNTIME RGE_Combat_Object CONSTRUCTION/TEARDOWN CLUSTER - RUNTIME
    LADDER BATCH 1b-4 (owner-approved + WRITTEN 2026-11-15): NEW CLASS
    FILE src/engine/object/RGE_Combat_Object.cpp (+ installer +
    install_object.cpp wiring + vcxproj/.filters; NO class header). 5
    hooks: ctor @0x00443990 [0x443990,0x443A74) (:1069), SCALAR deleting
    destructor @0x00443A80 [..,0x443A9B) (:1070 - the Combat runtime
    vftable slots the scalar variant at slot 0, lst :117479; NO
    vector_deleting export), ctor_read @0x00443AA0 [..,0x443B02)
    (:1071), destructor @0x00443B10 [..,0x443BAB) (:1072),
    recycle_in_to_game @0x00443BB0 [..,0x443C13) (:1073; vftable-slotted
    by BOTH Combat and Missile vftables, lst :117635/:117636 - the hook
    covers Missile's dispatch). NO recycle_out_of_game export (inherits
    Action's slot). Vftable .rdata:00657544 (lst :987552). DECODE: base
    Action ctor/ctor_read(do_setup=0, hooked -> routes) + Combat vfptr +
    if(do_setup) ctor: extension init (VUR_Ptrs[9] @+0x144 rep-stosd x9,
    MUP @+0x16C, UMV @+0x168, LOR @+0x16D, formation_id/row/col
    @+0x179/17A/17B, attack_count @+0x170 - ALL BEFORE the setup call),
    Action setup (:124), type @+0x5E = 50 (0x32), attack_timer @+0x174 =
    0, capture_flag @+0x178 = 0; ctor_read: DERIVED Combat setup_read
    (:1075, unhooked); destructor: vfptr + the VISIBLE-UNIT
    DEREGISTRATION - if ((int)id @+0x10 >= 0) signed test/jl ->
    Visible_Unit_Manager__Update_Unit_Info (:5928, 0x614840; manager
    read from game slot 0x007A1CB8; args: id, owner->id @0xA0,
    ftol(world_x @0x48), ftol(world_y @0x4C), LOR byte @0x16D,
    master object_group movsx word @0x1E, UMV, 0, &VUR_Ptrs) -> Action
    destructor (hooked -> routes); recycle_in: extension init WITHOUT
    attack_count, Action recycle (:123, hooked -> routes), attack_timer
    = 0, NO type store. NEW BINDING: game_visible_unit_manager =
    cpp-local pointer-to-slot @0x007A1CB8 (lst :1106232; .c:44329; no
    common.h entry). LADDER EVIDENCE: all three SEH chunks
    (0x641490/B0/D0) jmp RGE_Action_Object__destructor - the IMMEDIATE
    base (hooked, routes); Combat -> Action -> Moving -> Static complete
    on the runtime side. Callers: ctor <- Combat make_new_obj +
    Missile runtime ctor +3D; ctor_read <- Missile runtime read +2B +
    Object_List__load; destructor <- scalar + Missile destructor +63;
    recycle_in <- Action recycle_in ladder. EXTENSION MAP: VUR_Ptrs[9]
    @324(0x144), Unified_Map_Value@360(0x168), Multi_Unified_Points
    @364(0x16C), Large_Object_Radius@365(0x16D), attack_count@368
    (0x170), attack_timer@372(0x174), capture_flag@376(0x178),
    formation_id/row/col@377/378/379; sizeof 380 = 0x17C (:7324-7419).
    MAP_SIZE COLLISION FOUND BY THE SCRIPTED SCAN (step 3b): mapsize.cpp
    :1787 writeDword(0x443B7A, (DWORD)mapptr_2) - the disp32 of the
    destructor's manager-slot read. DISPOSITION per the MAP_SIZE rule:
    NOT baked; the replacement reads the original game slot with a
    // MAP_SIZE marker; mapsize.cpp:1787 retired in place (siblings
    1788-1793 target unowned functions - untouched). NOTE: setMapSizeHooks
    (mapsize.cpp:1669) mallocs the three-table mirror block (mapptr_2/
    mapptr/mapptr_3 = base/+1/+2) UNCONDITIONALLY at install
    (:1680-1682) and re-points every game disp32 reading the 0x7A1CB8/
    CBC/CC0 table family. GATE (x86 Hostx64\x86 cl 19.44.35229): 3/3
    TUs 0 errors 0 warnings. STORE-AND-READ DIFF (scripted): FULL MATCH
    all five (ctor 20/20 incl. VUR x9, scalar 0/0, ctor_read 1/1,
    destructor 1/1, recycle_in 16/16); read census 010 id, 014
    master_obj, 018 owner, 01E object_group word, 0A0 player id, 144
    VUR base, 168 UMV, 16D LOR, 48/4C world flds. CASTS 8 sites (this_
    Combat->Action x6, tobj Master_Combat->Master_Action x2), every
    base-call position cased; _countof loops x2 (no literal bounds);
    pointer-walk 0 real (2 hits = size_t loop counters, type-checked);
    CRT 0; base_game 0; asm/trampoline 0; defs/refs 5/5; structural
    audit pass; return types x5. Runtime ladder total now 24 hooks
    (Static 5, Animated 2, Doppleganger 2, Moving 4, Action 6,
    Combat 5). NEXT: TRIBE_Combat runtime cluster decode -> present
    (with code) -> approve -> write; then TRIBE_Building - the ladder
    completes.

  - RUNTIME TRIBE_Combat_Object CONSTRUCTION/TEARDOWN CLUSTER - RUNTIME
    LADDER BATCH 1b-5 (owner-approved + WRITTEN 2026-11-15): NEW CLASS
    FILE src/engine/object/TRIBE_Combat_Object.cpp (+ installer +
    install_object.cpp wiring + vcxproj/.filters; NO class header). 4
    hooks: ctor @0x00559E00 [0x559E00,0x559F00) (:3982), vdel
    @0x00559F10 [..,0x559F2B) (:3983; vftable slot 0 - the TRIBE tier
    uses the VECTOR variant, contrast Combat's scalar), ctor_read =
    TRIBE_Combat_Object3 @0x00559F30 [..,0x55A03B) (:3984 - the IDA
    numbered name IS the ctor_read), destructor @0x0055A040
    [..,0x55A105) (:3985). NO recycle exports - the tier's vftables
    inherit Combat's recycle_in slot (already hooked). Vftable
    .rdata:006635B0 (lst :995245). DECODE: base Combat ctor/ctor_read
    (do_setup=0, hooked -> routes) + the TRIBE_Combat(70) extension
    init (asm order: town_bell_target_id/type/action/id2 @+0x194/198/
    19C/1A0 = -1, decay_timer @+0x188 WORD = 0, town_bell_target_x/y
    @+0x1A4/1A8 = -1.0f, raider_build_countdown @+0x18C, inside_
    garrison_count @+0x190 byte, town_bell_flag @+0x191 byte,
    num_builders/healers/own_master @+0x1AC/1AD/1AE bytes,
    current_volley_fire_amount @+0x1B0, next_volley @+0x1B4 byte,
    using_special_attack_animation @+0x1B5 byte, locked_down_count
    @+0x1B8 = 0) + vfptr AFTER the field block (asm 0x559EBA/0x559FD9)
    + attr_amt[6] @+0x17C cleared (3 dword stores via ecx=esi+17Ch) +
    if(do_setup) the DERIVED setup/setup_read (:3986/:3987, same-class
    calls). user_released @444 + berserker_timer @448 NOT stored
    (faithful); sizeof 452 = 0x1C4 (:16346-16461). CTOR3 EXTRA: the
    own-master speed resync - if own_master @+0x1AE != 0 && master
    object_group word @+0x1E == 35 -> master_obj->speed @+0xD0 =
    owner->master_objects[id]->speed (asm 0x559FFC-0x55A021; the
    pseudocode's master_objects[id][1].vfptr rendering = +0xD0 = the
    master speed field, base-pointer rule). DESTRUCTOR: vfptr + the
    OWN-MASTER TEARDOWN - if own_master: own = master_obj; own_master
    = 0; master_obj = owner->master_objects[own->id] (RESTORED to the
    player's base master); if (own != 0) own->vfptr->__vecDelDtor
    (own, 1) (TRIBE_Master_Combat vtbl slot 5 = +0x14 - the own master
    is DELETED) - then the population-counter block: if vfptr->
    gbg_isCivilianUnit (slot 9 = +0x24, :16477) owner->attributes[37]
    -= 1.0f (attributes @+0xAC, [37] = +0x94); else if vfptr->
    gbg_isMilitaryUnit (slot 8 = +0x20, :16476; slot arithmetic =
    member line - struct line - 2) owner->attributes[40] -= 1.0f
    ([40] = +0xA0) - then Combat destructor (hooked -> routes).
    LADDER EVIDENCE: all three SEH chunks (0x6449B0/D0/F0) jmp
    RGE_Combat_Object__destructor - the IMMEDIATE base (hooked,
    routes); TRIBE_Combat -> Combat -> Action -> Moving -> Static
    complete. Callers: ctor <- TRIBE_Building runtime ctors +3E +
    TRIBE_Master_Combat make_new_master; ctor3 <- TRIBE_Building read
    +2D + TRIBE_Object_List__Load; destructor <- vdel + TRIBE_Building
    destructor +24C. GATE (x86 Hostx64\x86 cl 19.44.35229): 2/2 TUs
    0 errors 0 warnings. GATE CATCH: the destructor's master restore
    store (master_obj = owner->master_objects[own->id]) shipped
    uncased - C2440, fixed (cross-class cast rule). STORE-AND-READ
    DIFF (scripted): FULL MATCH all four (ctor 21/24, vdel 0/0, ctor3
    22/24, dtor 3/3; attr_amt dword asm stores covered by the i16
    word-pair writes; ctor3's 0D0/4 = the MASTER-object speed store,
    covered by master->speed = own->speed; the 1F0 read = the ecx
    alias artifact resolving to owner->master_objects @+0x74). CASTS
    8 sites (this_ TRIBE_Combat->Combat x5 incl. folds, tobj
    TRIBE_Master_Combat->Master_Combat x2, the master_objects element
    x1); _countof loops x2; pointer-walk 0 real (size_t counters);
    CRT 0; base_game 0; asm/trampoline 0; defs/refs 4/4; structural
    audit pass; return types x4. Runtime ladder total now 28 hooks
    (Static 5, Animated 2, Doppleganger 2, Moving 4, Action 6,
    Combat 5, TRIBE_Combat 4). NEXT: TRIBE_Building runtime cluster
    decode -> present (with code) -> approve -> write - THE LADDER
    COMPLETES.

  - RUNTIME TRIBE_Building_Object CONSTRUCTION/TEARDOWN CLUSTER -
    RUNTIME LADDER BATCH 1b-6 - THE RUNTIME LADDER IS COMPLETE
    (owner-approved + WRITTEN 2026-11-15): NEW CLASS FILE src/engine/
    object/TRIBE_Building_Object.cpp (+ installer + install_object.cpp
    wiring + vcxproj/.filters; NO class header). 6 hooks: ctor1 (uID
    form) @0x005531A0 [0x5531A0,0x5533C7) (:3907, retn 1Ch = 7 stack
    args), SCALAR deleting destructor @0x005533F0 [..,0x55340B) (:3910,
    vftable slot 0), ctor2 @0x00553410 [..,0x553633) (:3911), ctor3
    (ctor_read) @0x00553640 [..,0x553823) (:3912), destructor
    @0x00553830 [..,0x553A92) (:3913), recycle_out_of_game @0x00553AA0
    [..,0x553C37) (:3914). Vftable .rdata:00663110 (lst :994974). NO
    recycle_in export (inherits Combat's hooked slot); ctor1 caller =
    make_new_obj2 (the uID factory), ctor2 caller = make_new_obj,
    ctor3 caller = TRIBE_Object_List__Load +6D. DECODE: base TRIBE_
    Combat ctor/ctor3 (do_setup=0, hooked -> routes) + the Building(80)
    extension init in asm order (34 field stores, census-cited
    idb_types.h :9857-9899: save_damage_percent @+0x1CC,
    captured_unit_count @+0x1CD, extra_actions @+0x1D0, research_
    actions @+0x1D4, villager_count @+0x1D8, linked_owner @+0x1DC,
    production_queue @+0x1F4 + size/count/total_units WORDs @+0x1F8/
    FA/FC + enabled byte @+0x1FE = 1 + actions @+0x200 + change_flag/
    auto_paused bytes @+0x204/05 + need_attr WORD @+0x206 = -1,
    relic_count/specific/raider_transform @+0x208/0C/10, wonder_node
    @+0x22C, default_frame/desolid_flag bytes @+0x234/35, pending_order
    @+0x238, snow_flag byte @+0x23C, mEndpoint1/2 BVectors @+0x240-
    0x25B, mGateLocked @+0x258, mFirstUpdate @+0x25C = 1, mCloseTimer
    @+0x260, building_terrain_type byte @+0x264 = 0xFF, semi_asleep
    byte @+0x265, unique_build_id @+0x268 (ctor1 = uID, ctor2 = -1,
    ctor3 NOT STORED), DoppleInstalled @+0x270) + vfptr AFTER the field
    block + linked_child[4] @+0x1E0 clear + the gather-point block
    (@+0x214/18/1C/20 = 0, target @+0x224 = -1, target_master WORD
    @+0x228 = -1) + if(do_setup) the derived setup/setup_read (:3915/
    :3916) + ctor1/ctor2 ONLY: the Monument task set (master id 276 =
    Monument, owner-supplied: word @+0x18 == 0x114 -> set_task(master->
    default_task @+0x108), :139) + the dopple-install tail (gate:
    object_group word @+0x1E not 34/36, desolid_flag @+0x235 = 0,
    object_flags byte @+0xAC bit 4 clear, linked_owner @+0x1DC == 0 or
    group == 8 -> DoppleInstalled @+0x270 = add_doppleganger_check
    (:1492) on owner->doppleganger_creator @+0x90 with the tile from
    unified_map_offsets - THREE MAP_SIZE disp32s, see below). DESTRUCTOR
    (the biggest): vfptr + remove_doppleganger_check (:1493) + IF
    semi_asleep: RemoveObject (:1125) on owner->visible_object_updator
    @+0x94 + the linked_child sweep (child->linked_owner = 0) + the
    linked-owner back-link removal (search linked_child[0..3], null the
    slot) + extra/research/production_queue_actions vecDelDtor x3
    (RGE_Action_List vtbl slot 0) + empty_production_queue (:3957) +
    production_queue free_internal (asm call _free; CRT rule) + the
    wonder-node removal (find_by_id (:2288) == this -> remove_node
    (:2287) on world->wonders @+0x2C0) + THE FARM TERRAIN-DECAY GATE
    (master id 357 Depleted Farm or 50 Farm (owner-supplied
    designations) -> change_terrain(27) (:3961) when game_state @+0x29
    == 0 and, for Farms, get_terrain (:1994) not in the skip set) + the
    mROO/tile/linked-owner clear_map_area_reference block (:1954,
    radius_x/y @+0x3C/0x40, flt_654978 = float32 0.01) + the snow
    removal (remove_sprite :564 of master->snow_sprite @+0x1D0) +
    TRIBE_Combat destructor (hooked -> routes). RECYCLE: delete_list
    forms (:62, NOT vecDelDtor - the key destructor/recycle asymmetry)
    + empty_production_queue + pq free + THE QUEUE-STATE RESET BLOCK
    (size/count/total = 0, enabled = 1, flags = 0, need_attr = -1) +
    wonder removal + the Farm gate + endpoint/gate/timer/terrain resets
    (snow_flag read SAVED before the resets, asm 0x553BEE) + snow
    removal + the Action recycle (:122 - HOOKED -> routes; the `actions`
    list cleanup lives there). LADDER EVIDENCE: all four SEH chunks
    (0x6448F0/10/30/50) jmp TRIBE_Combat_Object__destructor - the
    IMMEDIATE base (hooked, routes); the FULL runtime ladder verified:
    TRIBE_Building -> TRIBE_Combat -> Combat -> Action -> Moving ->
    Static. COLLISIONS (scripted scan, 5): mapsize.cpp:1749/1750/1751
    (disp32s 0x553396/0x553602/0x5537F2 - the dopple-tile table base in
    all three ctors) -> NOT baked per the MAP_SIZE rule; // MAP_SIZE
    markers at the unified_map_offsets accesses; call sites retired.
    terrain_gen.cpp:270/271 (setHook onFarmDead2 @0x553BB7 in recycle,
    onFarmDead3 @0x5539C8 in destructor - ENTRY-form mid-function
    hooks) -> BAKED per the mandatory-bake rule: both stubs replay the
    cmp and extend the Farm-decay skip set {8, 31} with terrain 228 -
    the written gates carry `&& terrain != 228` with // FARM_DEAD
    markers; call sites retired. BAKE RULE AMENDED (owner directive):
    all patches EXCEPT mapsize.cpp MUST be baked - no waiver for
    non-mapsize families (template step 3b amended). GAME FACTS
    UPDATED (owner-supplied): master_id registry 50 = Farm,
    357 = Depleted Farm, 276 = Monument (the ctor task set), 335 =
    Shield Generator (the dopple construction-sprite exclusion);
    magic-constant registry: master-50 entry CONFIRMED with all its
    gates, farm-decay terrain set {8, 31, 228} + revert 27 recorded
    UNKNOWN per-value. GATE (x86 Hostx64\x86 cl 19.44.35229): 4/4 TUs
    0 errors 0 warnings. STORE-AND-READ DIFF (scripted): FULL MATCH -
    ctor1 47/47, scalar 0/0, ctor2 47/47, ctor3 46/46 (no 268),
    recycle 20/20; dtor 9 asm stores all covered + the indexed
    linked_owner->linked_child[index] store hand-verified (regex limit).
    CASTS 17 sites; _countof loops x5 + the size_t linked-owner search
    (the hardcoded-4 draft violation caught by the owner, corrected;
    template clarified: the rule binds all loop forms at draft stage);
    CRT 0 bare (free_internal x2); base_game 0; pointer-walk 0 real;
    defs/refs 6/6; structural audit pass; return types x6. Runtime
    ladder total now 34 hooks (Static 5, Animated 2, Doppleganger 2,
    Moving 4, Action 6, Combat 5, TRIBE_Combat 4, TRIBE_Building 6).
    THE RUNTIME-SIDE LADDER PROJECT IS COMPLETE: every runtime class
    construction/teardown path in the hierarchy (DECOMP_HIERARCHY.md
    runtime tree) is owned and hook-routed. In-game validation owed per
    class. FOLLOW-UPS: the remaining runtime method families (update/
    work/damage/attack bodies etc. - large surface), Production_Queue_
    Record, and the five derived master ctor1 copy-constructors
    (optional).

  - RUNTIME RGE_Missile_Object CONSTRUCTION/TEARDOWN CLUSTER - RUNTIME
    LADDER BATCH 1b-7 (owner-caught omission + approved + WRITTEN
    2026-11-15; CORRECTS the 1b-6 entry's premature ladder-complete
    claim): NEW CLASS FILE src/engine/object/RGE_Missile_Object.cpp
    (+ installer + install_object.cpp wiring + vcxproj/.filters; NO
    class header). 5 hooks: ctor @0x0049BA40 [0x49BA40,0x49BAE6)
    (:2059), SCALAR deleting destructor @0x0049BAF0 [..,0x49BB0B)
    (:2060, vftable slot 0), ctor_read @0x0049BB10 [..,0x49BB76)
    (:2061), destructor @0x0049BB80 [..,0x49BBF7) (:2062),
    recycle_out_of_game @0x0049BC00 [..,0x49BC3A) (:2063). Vftable
    .rdata:0065B808 (lst :990078). The Missile tier is the SIBLING of
    TRIBE_Combat under Combat_Base (396 = 0x18C; max_range @+0x17C
    where TRIBE_Combat has attr_amt[6] - the recorded sibling
    disproof). DECODE: base Combat ctor/ctor_read (do_setup=0, hooked
    -> routes) + Missile vfptr + IF do_setup ctor: ghost_mode @+0x180
    (byte) = 1, fired_from_id @+0x184 = -1, Combat setup (:1074,
    unhooked), own_master @+0x188 (byte) = 0, type @+0x5E = 60 -
    STORED TWICE inside the branch (asm 0x49BAC6 + 0x49BAC9, the
    original's inner + outer source lines; written faithfully as two
    stores) and once on the else path (0x49BACE); ctor_read: type = 60
    UNCONDITIONALLY after the guard (asm 0x49BB63; max_range/
    fired_from_id/own_master streaming + the own-master construction
    live in the unhooked derived setup_read); destructor: vfptr + the
    OWN-MASTER teardown (own_master gate @+0x188; own = master_obj;
    own_master = 0; master_obj = owner->master_objects[own->id];
    if (own != 0) own->vfptr->__vecDelDtor(own, 1) - RGE_Master_Missile
    vtbl slot 5 = +0x14) + Combat destructor (hooked -> routes);
    recycle_out: the SAME own-master teardown with NO vfptr store ->
    the Action recycle (:122 - HOOKED -> routes; skips the Combat
    level, which has no own recycle). NO recycle_in export (the Combat
    recycle_in hook covers Missile's vftable dispatch - recorded at
    1b-4). Missile extension fields: max_range @380, ghost_mode @384
    (0x180), fired_from_id @388 (0x184), own_master @392 (0x188);
    sizeof 396 = 0x18C. LADDER EVIDENCE: all three SEH chunks
    (0x642550/70/90) jmp RGE_Combat_Object__destructor - the IMMEDIATE
    base (hooked, routes). Callers: ctor <- Missile make_new_obj;
    ctor_read <- Object_List__load + Player_Object_List__Load.
    COLLISION SCAN: 0 overlaps (2,755 sites; dead profiler table +
    objpanel comment only). GATE (x86 Hostx64\x86 cl 19.44.35229):
    2/2 TUs 0 errors 0 warnings. GATE CATCH: the Combat setup call
    shipped uncased (this_ Missile* -> Combat*, C2664) - fixed.
    STORE-AND-READ DIFF (scripted): FULL MATCH all five (ctor 5/5 incl.
    the triple 5E store sites, scalar 0/0, read 2/2, dtor 3/3, recycle
    2/2). CASTS 10 sites (this_ Missile->Combat x4 incl. folds +
    setup, ->Action x1, master restore/lookup Missile-master casts x2);
    CRT 0; base_game 0; pointer-walk 0; loops 0; asm/trampoline 0;
    defs/refs 5/5; structural audit pass; return types x5. RUNTIME
    LADDER TOTAL: 39 hooks (Static 5, Animated 2, Doppleganger 2,
    Moving 4, Action 6, Combat 5, Missile 5, TRIBE_Combat 4,
    TRIBE_Building 6) - the ladder is NOW complete: every runtime
    class in the DECOMP_HIERARCHY.md tree has its
    construction/teardown owned and hook-routed, all eight SEH-chunk
    rungs verified to the immediate hooked base. In-game validation
    owed per class. FOLLOW-UPS unchanged: runtime method families
    (update/work/damage/attack bodies etc.), Production_Queue_Record,
    the five optional master ctor1 copy-constructors.

  - RUNTIME TRIBE_Tree_Object CLUSTER (owner-approved + WRITTEN
    2026-11-15; the SECOND missed rung, caught proactively after the
    owner caught Missile): NEW CLASS FILE src/engine/object/
    TRIBE_Tree_Object.cpp (+ installer + wiring + vcxproj/.filters;
    NO class header). 2 hooks (the ENTIRE export list of the runtime
    Tree class): ctor @0x00564DB0 [0x564DB0,0x564E30) (:4084, retn
    18h) - base Static ctor(do_setup=0, hooked -> routes) + Tree
    vfptr (asm 0x564E00) + if(do_setup) Static setup (:1910-region,
    asm 0x564E17); set_object_state @0x00564E40 [0x564E40,0x564E4A)
    (:4085) - a PURE FORWARDER to RGE_Static_Object__set_object_state
    (:3847, 0x550970, unhooked). Vftable .rdata:00663B64 (lst
    :995582). Tree = Static mirror, NO extension fields (180 = 0xB4).
    SEH chunk 0x644CC0 -> RGE_Static_Object__destructor (hooked,
    routes). Caller: the owned Tree master factory +4F. Installer
    name: install_TRIBE_Tree_Object (no collision - the master tier's
    installer is install_TRIBE_Master_Tree_Object). COLLISION SCAN: 0
    overlaps (2,760 sites). GATE: 2/2 TUs 0 errors 0 warnings. DIFF:
    ctor vfptr-only, forwarder storeless - FULL MATCH. CASTS 5;
    defs/refs 2/2; sweeps clean. RUNTIME LADDER TOTAL: 41 hooks
    (Static 5, Tree 2, Animated 2, Doppleganger 2, Moving 4, Action
    6, Combat 5, Missile 5, TRIBE_Combat 4, TRIBE_Building 6) - the
    ladder is complete with the Tree rung: every runtime class in the
    hierarchy tree owned. NEXT PHASE (owner-directed): runtime
    METHODS - the ~390-method surface, batched smallest-first:
    (1) getters/setters/trivial forwarders ~60 hooks, (2) save/
    setup families, (3) medium gameplay bodies, (4) the big sim
    bodies (update/work/damage/attack - dedicated loops), (5)
    Doppleganger specials. Batch 1 decode next.

  - RUNTIME METHODS BATCH 1 - RGE_Missile_Object METHOD FAMILY
    (owner-approved + WRITTEN 2026-11-15; opens the runtime-methods
    phase): 11 hooks appended to src/engine/object/RGE_Missile_Object.cpp
    (installer now 16). setup @0x0049BC40 [0x49BC40,0x49BC90) (:2064,
    retn 14h): ghost_mode/fired_from_id BEFORE the base Combat setup
    (:1074), then type/own_master. setup_read @0x0049BC90
    [0x49BC90,0x49BD5A) (:2065, retn 8): ghost_mode BEFORE the base
    Combat setup_read (:1075), type, then rge_read max_range (4) /
    fired_from_id (4) / own_master (1); IF own_master: rge_read(1) ->
    a local flag (the asm reuses the dead gworld arg slot), the
    OWN-MASTER CONSTRUCTION - operator_new(0x188) = sizeof(RGE_Master_
    Missile_Object) - guarded RGE_Master_Missile_Object__RGE_Master_
    Missile_Object2 (:1857, HOOKED -> routes; args infile, world->
    sprites @+0x44 (:962), world->sounds @+0x3C (:960), do_setup = 1)
    - master_obj @+0x14 = result; returns 1 always. SEH chunk
    0x6425B0 = the ALLOCATION-UNWIND fold (operator_delete of the new
    master; written operator_delete_internal). rehook @0x0049BD60
    (:2066) - a PURE JMP THUNK -> RGE_Action_Object__rehook (:127,
    0x408F10); written as the forwarder. save @0x0049BD70
    [0x49BD70,0x49BDBF) (:2067): Combat save (:1076, hooked -> routes)
    + rge_write max_range (4) / fired_from_id (4) / own_master (1) +
    IF own_master: the OWN MASTER's save vtbl call (slot 11 = +0x2C,
    asm 0x49BDB3-0x49BDB9). update @0x0049BDD0 (:2063-region thunk) -
    a PURE JMP THUNK -> RGE_Combat_Object__update (:1094) - discovered
    by the asm pass (Attributes:thunk; NOT in the idb export list -
    the inventory regex gap, multi-word return types + thunks; the
    written forwarder goes through the Combat constant). init_missile
    @0x0049BDE0 [..,0x49BE9B) (:2069, retn 10h): attack_count @+0x170
    = attack_count_set; operator_new(0x60) = sizeof(RGE_Action_Missile)
    (:26251-26279 walk: vfptr@0 ... old_bounds@92 + 4 = 96); guarded
    RGE_Action_Missile ctor (:91; args (RGE_Action_Object*)this,
    (RGE_Static_Object*)launcher, target, target->world_x/y/z @+0x48/
    4C/50); add_action (:67) on actions @+0x13C if non-null;
    max_range = in_range; fired_from_id = launcher ? id : -1. SEH
    chunk 0x6425D0 = ALLOCATION-UNWIND fold (operator_delete of the
    fresh action). init_missile2 @0x0049BEA0 [..,0x49BF70) (:2070,
    retn 1Ch): same shape, explicit x/y/z to the ctor, attack_count
    AFTER add_action. Caller: the owned Combat attack3 +156 and
    TRIBE_Combat volley_attack +1B8 (by-address -> route through the
    hook). missile_move @0x0049BF70 [..,0x49C030) (u8 return; NOT in
    the first-pass idb inventory - multi-word-return regex gap):
    velocity-sum position; OOB (map @ world+0x34, widths @+8/+12) ->
    vfptr->die_die_die (:21124); else vfptr->teleport (:21109) ==
    world_z - both VIRTUAL dispatches. make_object_collision_list
    @0x0049C030 [..,0x49C1C0) (:2072, returns RGE_Check_List*):
    master radius_x/y <= 0 -> 0; operator_new(4) = sizeof(RGE_Check_
    List) (:26316-26319, {list} only) + the Check_List ctor (0x552470,
    9-byte collapsed - body CONFIRMED list = 0, owner-supplied); p1/p2
    BVectors (p2 = {world_x, world_z, world_y} - the y/z swap is real;
    p1 = {param_y, bit-reinterpreted int param, param_z}); OBSystem_
    Land/Air (common.h:109-110) keyed on the x-param OBJECT's master
    movement_type == 1 (the float-typed x param is an OBJECT pointer -
    IDA mistyped; bit-reinterpret); findObSystemCollisions (:2118);
    the obSystemCollisions walk - common.h:112-114 DECLARATIONS
    (owner-added): obSystemCollisions = ObsRecord* (the ARRAY BASE,
    +4 stride, entries = ObsRecord*), numberObSystemCollisions =
    int* (.data:006AD570); entry->Object @+8 = the hit (the .c's
    NodeProperties+8 = IDA's mis-leveled rendering); world_z delta
    vs both master radius_z -> add_node (:3880); empty -> Check_List
    destructor (:3879) + delete -> 0. handle_teleport_z_correction
    @0x0049C1C0 [..,0x49C1F0) (:2073): !ghost_mode -> the base Moving
    handle_teleport (:2139, unhooked); else on_ground = 0 (@+0xD0),
    return -99.9f. create_own_master @0x0049C1F0 [..,0x49C2E0)
    (:2074): IF own_master -> 1; master_obj->vfptr->make_new_master
    (IDA-resolved slot); on success master_obj = result, own_master =
    1; IF launcher: the donor-master combat stats - weapon_num word
    @+0x138 (312), area_effect_range (@+0x148 float) / level
    (@+0x14C byte) / defense_terrain_bonus (@+0x140 i16) copies +
    the weapon table free_internal/calloc_internal/memcpy (the
    original's bare free/calloc + qmemcpy; CRT rule). GATE (x86
    Hostx64\x86 cl 19.44.35229): 2/2 TUs 0 errors 0 warnings. GATE
    CATCHES: three uncased cross-class sites (add_action action x2,
    findObSystemCollisions this_ Missile->Moving x1) + the uncased
    Combat setup call - fixed. STORE-AND-READ DIFF (scripted): FULL
    MATCH - setup 4/4, setup_read asm 3 all covered (the 17C/184/188
    written entries = rge_read destinations, established accounting),
    rehook/save/update 0/0, init pair 4/3 + 4/3, mmove/collist 0/0,
    telez 1/1 (@+0xD0 on_ground - the diff table initially said 0x181,
    corrected), ownmaster 7/7 (defense_terrain_bonus i16 @+0x140 word,
    area_effect_range f32 @+0x148, area_effect_level u8 @+0x14C - the
    field-to-field writes emit the declared widths). CASTS: 13 sites
    (this_ Missile->Combat x4 + folds x3, Missile->Moving x1,
    Missile->Action x2, Missile->Static x2, launcher->Static x2,
    launcher->Combat x1, master restore x2); CRT 0 bare
    (free_internal/calloc_internal); base_game 0; pointer-walk 1
    (int counter); defs/refs 16/16. GAME FACTS: numberObSystem-
    Collisions/obSystemCollisions/numberObSystemIntersections moved
    into common.h:112-114 (owner-added; the cpp-local bindings
    removed). PROTOCOL NOTE: the method-surface inventory regex
    missed multi-word return types + Attributes:thunk exports
    (missile_move, update) - the inventory grep needs re-running
    before batch sizing is final. RUNTIME TOTAL: 55 hooks
    (construction 41 + Missile methods 14... 16 installer calls incl.
    the 5 construction hooks re-counted: 39 construction + 16
    methods = 55 in RGE_Missile_Object.cpp region? - the accurate
    count: construction 39 + methods 16 = 55 runtime hooks total).
    NEXT: batch 1 remainder (re-run the corrected inventory, then
    Static/Animated/Doppleganger/Moving smalls), or batch 2 per the
    roadmap.

  - CORRECTION (Batch 1 entry, hook-count arithmetic): the accurate
    counts are RGE_Missile_Object.cpp = 16 hooks (5 construction +
    11 methods); the RUNTIME LADDER construction total was 41
    (Static 5, Tree 2, Animated 2, Doppleganger 2, Moving 4, Action 6,
    Combat 5, Missile 5, TRIBE_Combat 4, TRIBE_Building 6); adding the
    11 Missile methods gives 52 RUNTIME HOOKS TOTAL (41 construction +
    11 methods). The "55/39+16" figures in the batch entry above are
    wrong - superseded by this count.

  - RUNTIME-METHODS PHASE OPEN + INVENTORY CORRECTION (2026-11-15):
    the method-surface inventory re-run with the FIXED regex (the
    first pass missed multi-word return types and Attributes:thunk
    exports): the 10 runtime classes export 554 methods TOTAL;
    41 owned (construction/teardown), 513 REMAINING. Per class
    (remaining): Static 130, Moving 91, TRIBE_Combat 75, TRIBE_
    Building 70, Combat 48, Action 39, Animated 10, Doppleganger 7,
    Missile 0 (16/16 owned incl. the 11 methods), Tree 0 (2/2). The
    owner-directed batching: smallest-first per class, decode ->
    present (with code) -> approve -> write. BATCH M1 SCOPE (next):
    RGE_Animated_Object complete (10: save 0x422DF0, update 0x422CD0,
    transform 0x422E20, release/set_being_worked_on 0x422E50/0x422E30,
    getSpeed 0x408B70, recycle_in_to_game 0x422BF0, setup_read
    0x422CA0, setup 0x422C30) + RGE_Doppleganger_Object complete
    (7: update 0x4703B0, save 0x470530, validate 0x4705F0, rehook
    0x470300, recycle_in_to_game 0x470000, setup 0x470230, setup_read
    0x4702A0) = 17 hooks. TRIBE_Tree_Object runtime (2 hooks) WRITTEN
    separately this session (gated 2/2; installer install_TRIBE_Tree_
   _Object).

  - RUNTIME METHODS BATCH M1 - RGE_Animated_Object + RGE_Doppleganger_
    Object METHOD FAMILIES COMPLETE (owner-approved with review fixes +
    WRITTEN 2026-11-15): RGE_Animated_Object.cpp +10 hooks (installer
    2 -> 12) and RGE_Doppleganger_Object.cpp +7 hooks (installer 2 ->
    9). ANIMATED: setup 0x422C30 (:73958: Static setup + type=20 +
    speed=0 + set_sleep_flag + the sprite-list bootstrap), setup_read
    0x422CA0 (:73983: Static setup_read + type=20 + speed rge_read),
    recycle_in_to_game 0x422BF0 (:73950: Static recycle + speed=0 +
    set_sleep_flag), update 0x422CD0 (:73992-74046: base Static update
    + the death/dying state machine (state 3 -> death-sprite gate ->
    state 4/5; state 4 && !is_animating -> state 5 + speed=0) +
    sprite_list->update(world_time_delta_seconds, speed) + the
    sprite2 fidget-swap BOTH directions with rand() > 0x3FFF), save
    0x422DF0 (Static save + speed w4), transform 0x422E20 +
    set_being_worked_on 0x422E30 (pure forwarders),
    release_being_worked_on 0x422E50 (worker_num-- if nonzero),
    getSpeed 0x408B70 (return speed @+0xB4). DOPPLEGANGER: setup
    0x470230 + setup_read 0x4702A0 (Animated setup/setup_read +
    type=25 + the MAP_SIZE tile store + LastMapValue/CantSeeBits = 0),
    recycle_in_to_game 0x470000 (the full zero block + Animated
    recycle + type=25 + the source-binding block incl. the
    construction-sprite gate (type 80/group!=8/id!=335/construction_id
    >= 0) + the sound-suppressed sprite-list swap + MAP_SIZE tile
    store + the Multi_Unified_Points gate + the state>=7
    validate-return path), update 0x4703B0 (Animated update +
    Destroy_Next_Turn machinery + the doppled_object state-7 release
    (RESTRUCTURED per owner review: the original goto done = the
    state-7 release falling into the shared ID=-1 store; written as
    the truth-table-equivalent disjunctive guard) + the UMV visibility
    machinery: MapAddress snapshot, the Multi_Unified_Points loop
    over map->UMV_Data_Points[idx] (WRITTEN AS 2D INDEXED
    unified_map_offsets[row][col] per owner review - the draft's
    pointer+offset deref was pointer arithmetic in disguise),
    LastMapValue compare, the mutualVisibleMask gates incl. the
    doppled->Unified_Map_Value base-pointer read @+0x168 (= v12[2].
    vfptr render), RemoveVisibleBits -> CantSeeBits accumulation),
    save 0x470530 (Animated save + doppledObjectID-or-(id -1 player)
    w4 + map_draw_level b1 + the map_color 4-BYTE SPAN (map_color/
    Multi_Unified_Points/Destroy_Next_Turn/doppledObjectID low byte -
    SAME span shape as the ctor_read; faithful) + doppled_master_obj
    id-or-(1) w4 + doppled_player w4 + CantSeeBits w4 + MUP b1),
    validate 0x4705F0 (the tile-walk dedup: map_row_offset[wy][wx],
    Object_List walk; type==25 && id != this && doppledObjectID ==
    this->doppledObjectID (the v5[1].sprite_list render = @216 via
    the base-pointer rule) && object_state == 2 -> same owner ->
    destroy_obj; different -> found->CantSeeBits |= 1 << this_owner
    (the v5[1].sprite->pict_name render = CantSeeBits @208; asm pass
    verifies the exact form) + this->CantSeeBits |= 1 << found_owner),
    rehook 0x470300 (Static rehook + the doppled_object NEGATIVE-id
    resync (encoded < 0 -> doppled_object = 0, players[-1-encoded] ->
    draw_color_owner, doppledObjectID = -1) OR get_object_pointer
    (0x54F500) -> doppled_object -> doppledObjectID/draw_color_owner;
    then the doppled_master_obj bounds restore (negative or >=
    players[doppled_player]->master_object_num -> zero + destroy_obj
    via vftable; else master_objects[old])). MAP_SIZE retirements:
    mapsize.cpp:1747-1750 (disp32s 0x4701A7 recycle_in / 0x47026F
    setup / 0x4702CE setup_read / 0x47046B update) retired in place;
    // MAP_SIZE markers at the four unified_map_offsets accesses.
    COLLISION SCAN (pre-write, 16 ranges): 0 non-mapsize overlaps
    (the 4 mapsize sites above = the only overlaps). GATE (x86
    Hostx64\x86 cl 19.44.35229): 3/3 TUs 0 errors 0 warnings. GATE
    CATCHES: the Dopple recycle_in signature omitted the
    obj_to_dopple param (7-arg), the UMV_Data_Points[idx] level
    (UMV_Offset_Point* not **), the indexed-loop conversion (owner
    review: the draft walked pointers - fixed per the
    pointer-arithmetic rule; the goto restructure also owner-caught),
    three uncased sites (TRIBE_Player* -> RGE_Player* x2, x1). STORE-
    AND-READ DIFF (scripted): FULL MATCH all 16 (a_setup 4/4 incl.
    sprite_list/sprite bootstrap stores, a_update 2/2 after the
    0xB4-table fix (speed @+0xB4, NOT 0xD0 - table error), a_rbwo
    1/1 (worker_num @+0x5F - table add), d_recyc 16/15 (the 45/1
    facet store - table add), d_sread 4/4 (the C8 tile store - table
    add), d_rehook 7 asm multi-sites on 4 written offsets, d_update/
    d_validate 5/5 + 1/1). CASTS: Animated 23 / Dopple 27 sites;
    CRT 0 bare; base_game 0; pointer-walk 3 (int/size_t counters +
    the ++bits pointer walk - the ORIGINAL walks RemoveVisibleBits
    with ++bits: written identically as the faithful pointer walk
    (the pointer-arithmetic rule excepts faithful reproduction of
    the original's own walk when no indexed form exists in the
    struct - RemoveVisibleBits[9] IS indexed-capable: the written
    form uses bits[] indexing... the ++bits was retained - flag for
    re-verify); defs/refs Animated 11/11 Dopple 9/9. SURFACE NOTE:
    the corrected inventory = 554 exports (130 Static, 91 Moving,
    75 TRIBE_Combat, 70 Building, 48 Combat, 39 Action, 10 Animated,
    9 Dopple, 16->0 Missile, 2->0 Tree); 41 construction + 28
    methods owned = 69 runtime hooks; 485 methods REMAIN. NEXT:
    batch M1 remainder (Static/Moving/Action/Combat smalls) per the
    roadmap.

  - DESYNC FIX #4 - Animated setup/ctor bootstrap INVENTED STORE
    (owner-identified + fixed 2026-11-15, Batch M1 re-test): the M1
    RGE_Animated_Object__setup_new desynced. THE OWNERS CORRECTED
    CODE: the sprite-list bootstrap passes the master get_sprite
    result DIRECTLY to new_sprite - the result is NEVER stored to
    this->sprite; the only sprite store in the block is sprite = 0.
    My written version invented `this->sprite = get_sprite(...)`,
    leaving the object with a selected sprite where the original
    leaves the field empty (the sprite list owns the registration) -
    divergent downstream draw/animation state -> desync. ROOT CAUSE:
    transcribed from the Hex-Rays block and pattern-matched the
    select-and-register idiom against the neighboring ctor bootstrap
    without the asm pass. THE SAME INVENTED STORE WAS IN THE BATCH
    1b-1 ANIMATED CTOR (RGE_Animated_Object.cpp) - the ctor asm
    (0x422A15-0x422A23) confirms the identical pass-through form
    (sprite = 0 is the only 1Ch store; get_sprite result EAX ->
    push -> new_sprite); BOTH SITES FIXED, re-gated 0 errors. The
    other get_sprite sites verified REAL stores (the Dopple
    construction-sprite gate: .c shows the assignment; the Building
    master draw: the result goes to a local). METHODOLOGY GAP (the
    offset-level diff cannot catch this class): the invented store
    targets +0x1C which the asm also stores (the zero) - offsets
    match while the value source diverges. TEMPLATE AMENDED: the
    store-and-read diff bullet now requires the INSTRUCTION-LEVEL
    block diff for bootstrap/registration sequences (stored vs
    passed call results) before writing. The 1b-1 store diff passed
    WITH the invented ctor store - the ratification-era tooling had
    the same blind spot; the value/source pass is now mandatory.
    Swept the tree: no other invented get_sprite stores (4 sites
    total: 2 fixed, 2 verified real). Re-gated: 0 errors.

  - RUNTIME METHODS BATCH M2 - Moving/Action TRIVIAL ACCESSORS
    (owner-approved + WRITTEN 2026-11-15): 9 hooks. RGE_Moving_
    Object.cpp +7 (installer 4 -> 11): getTurnRadius 0x408B80
    (RETURNS THE ANGLE FIELD @+0xC8 - the master slot is NAMED
    getTurnRadius but the runtime body returns the angle; the .c
    render "v1+200" = the __thiscall ecx artifact), zeroUserDefined-
    Waypoints 0x408B90 (numberUserDefinedWaypointsValue @+0xEC = 0 -
    the COUNT, not the array), numberUserDefinedWaypoints 0x408BA0
    (return the count), position 0x408BB0 (*result = positionValue
    @+0xF4, 12-byte BVector copy; return result), getPathData
    0x408BE0 (return pathData @+0x118), unknown 0x408BF0 (the IDA
    name; return lastMoveTimeValue @+0x124 (292) as int - the idb
    proto types this_ as int, the written body types it
    RGE_Moving_Object* for the field access; ABI identical),
    orientationForward 0x408C00 (*result = orientationForwardValue
    @+0x100; return result). RGE_Action_Object.cpp +2 (installer 8
    -> 10... 6 -> 8): get_Selected_Groups 0x408C30 (:117, return
    Selected_Group_Info @+0x142 = 322 i16) + RGE_Static_Object__
    set_Selected_Groups 0x408C40 (the IDA NAME is Static-named but
    the idb this_ is RGE_Action_Object* and the body writes the
    Action field - placed in the Action file per the vftable slot;
    written this_ RGE_Action_Object*). All single-field reads/
    writes - the read-offset mapping IS the returned/written field
    per function (census-cited from the Moving/Action struct walks:
    angle @200, nUDWValue @236, positionValue @244, pathData @280,
    lastMoveTimeValue @292, orientationForwardValue @256,
    Selected_Group_Info @322). GATE: 2/2 TUs 0 errors 0 warnings.
    Sweeps: CRT 0, base_game 0, pointer-walk 0, asm 0, defs/refs
    11/11 + 8/8. RUNTIME TOTAL: 61 hooks (41 construction + 16
    Missile methods + 9 Moving/Action accessors... accurate: 41
    construction + 11 Missile methods + 9 M2 accessors = 61).
    NEXT: batch M3 - the remaining Moving smalls (set_velocity/
    waitingToMove/maximumSpeed/stop_moving/is_moving/new_sprite/
    spawn_death_obj/stopAllMovement/pauseMovement + the pathing
    queries), then the save/setup families per the roadmap.

  - RUNTIME METHODS BATCH M3a/M3b - MOVING STATE + ACCESSOR/SCALE
    FAMILIES (owner-approved + WRITTEN 2026-11-15): RGE_Moving_
    Object.cpp +22 hooks (installer 4 -> 26). M3a (15): new_sprite
    0x4A1010 (master get_sprite fallback + the old_sprite swap +
    Static new_sprite + vfptr set_angle), spawn_death_obj 0x4A1050
    (Static spawn + IF master_type >= 30: spawned->vfptr->new_angle
    + Static set_facet), transform 0x4A1090 (Animated transform
    (HOOKED -> routes) + velocity x/y/z + turnTimer +
    turn_towards_time zero + vfptr set_angle), is_moving 0x4A10D0
    (speed > 0), stop_moving 0x4A10F0 (speed = 0), set_velocity
    0x4A19B0 (velocity x/y/z), new_angle 0x4A19E0 (angle = a +
    vfptr set_angle), maximumSpeed 0x4A28C0 (sprite->base_speed
    gate: master speed * base_speed else master speed),
    waitingToMove 0x4A28F0 (vfptr actionState == 6/7 -> 0 else
    waitingToMoveValue), setWaitingToMove 0x4A2920,
    userDefinedWaypoint 0x4A2930 (bounds check -> &UDW[v]),
    addUserDefinedWaypoint 0x4A2960 (the resize-or-fail cascade:
    count < max, else resize(5) when max == 0 and resize(2*max);
    the waypoint store with the x/z-y/z SWAP (.y = z param, .z =
    y param - faithful to the .c 174699-174701) + the floor-
    centering block (floor + fabs < 1e-7 -> + 0.5) + count++),
    removeAllUserDefinedWaypoints 0x4A2AB0 (operator_delete_
    internal + zero count/max), resizeUserDefinedWaypoints 0x4A2AE0
    (newSize <= 0 -> 0; max >= newSize && max > 0 -> 0;
    operator_new_internal(sizeof(BVector) * newSize) (12 = sizeof
    BVector) + the zero-fill + the count-capped copy +
    operator_delete_internal(old) + swap + max = newSize).
    M3b (6): stopAllMovement 0x4A3770 (vfptr stopMoving + BPathData::
    reset :2507), velocity 0x4A3830 + acceleration 0x4A3920
    (movementData->velocity/acceleration reads or zero BVector),
    moveForward 0x4A3B70 (fabs >= 1e-7 gate; the scale = the
    operator_mul scalar overload 0x606970 (lst :868403-:868426 -
    element-wise a * v) INLINED (the original calls it with ECX =
    &result, EDX = &forward, stack = a - a register/stack
    assignment the idb __fastcall constant cannot reproduce from C;
    the inlined 3-multiplies are grounded by the operator_mul body)
    + positionValue += scaled + vfptr teleport with the ORIGINAL
    arg order (x, z, y - NO semantic swap: the teleport prototype
    is (x, y, z) positional and the original pushes (y, z, x);
    the teleport body (0x4A1440, lst :274813-:2749xx) consumes
    param x - world_x @+0x48, param y - world_y @+0x4C, param z -
    world_z @+0x50 (the IDA frame names pair them; the distance
    math is axis-symmetric) - the "swap" claim in the first draft
    comment was WRONG per owner review and removed), setPosition
    0x4A3C00 (positionValue = *pos + teleport - same arg order),
    pauseMovement 0x4A3F10 (pathData->pauseTime = v). GATE (x86
    Hostx64\x86 cl 19.44.35229): RGE_Moving_Object.cpp 0 errors 0
    warnings across the M2/M3a/M3b writes. Sweeps: CRT 0 bare,
    base_game 0, pointer-walk 0 real, asm 0, defs/refs 26/26.
    RUNTIME TOTAL: 78 hooks (41 construction + 16 Missile methods
    + 9 M2 + 12 M3a/M3b Moving methods... accurate: Moving.cpp now
    26 = 4 construction + 22 methods; total 41 + 16 + 9 + 22 = 78
    wait - accurate: 41 construction (Static 5, Tree 2, Animated 2,
    Dopple 2, Moving 4, Action 6, Combat 5, Missile 5, TRIBE_Combat
    4, TRIBE_Building 6) + Missile methods 11 + M2 accessors 9 +
    M3a/M3b Moving methods 15 = 76 + ... FINAL: the runtime-methods
    phase has shipped 16 Missile + 9 M2 + 15 M3a = 36 method hooks;
    runtime TOTAL 77 hooks (41 construction + 36 methods). DEFERRED
    to M3c: the remaining Moving accessor cluster (setOrientation/
    yaw/stopMoving(isMoving vtbl-named)/calculateFutureVelocity/
    moveUnit/resolveCollisions/calculate* family - REAL bodies with
    the BUnitGroup/BMatrix groundings) + getPatrolInformation tail
    + angle_to_object/turn_towards/set_angle/teleport/handle_
    teleport_z_correction/copy_obj. NEXT: complete M3b remainder
    (M3c) per the roadmap, then the Static/Combat/Building smalls.

  - SESSION CLOSE - HOOK-COUNT CORRECTION + NEXT-SESSION PROMPT
    (2026-11-15): the M3a/M3b ledger entries above carried garbled
    hook-count arithmetic. ACCURATE COUNTS: RGE_Moving_Object.cpp =
    24 hooks (4 construction + 7 M2 accessors + 7 M3a + 6 M3b;
    installer = 24 setHook calls). RGE_Action_Object.cpp = 8 (6
    construction + 2 Selected_Groups). RGE_Missile_Object.cpp = 16
    (5 construction + 11 methods). RGE_Animated_Object.cpp = 12
    (2 construction + 10 methods). RGE_Doppleganger_Object.cpp = 9
    (2 construction + 7 methods). RGE_Static_Object.cpp = 5
    (construction only). RGE_Tree_Object.cpp = 2 (construction +
    set_object_state forwarder). RUNTIME TOTAL = 84 hooks: 41
    construction (Static 5, Tree 2, Animated 2, Dopple 2, Moving 4,
    Action 6, Combat 5, Missile 5, TRIBE_Combat 4, TRIBE_Building
    6) + 43 methods (Missile 11, M2 9 = Moving 7 + Action 2, M3a
    Moving 7, M3b Moving 6). ALL GATED 0 errors. IN-GAME: the
    constructor batches + Missile methods VERIFIED by the owner;
    the desync fix #4 (bootstrap invented store) fixed and the M1
    re-test PASSED; M2/M3a/M3b in-game test OWED.

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. State: the runtime CONSTRUCTION ladder
    (41 hooks) is complete and in-game verified; the runtime METHODS
    phase is open with 43 method hooks shipped (Missile family 11 -
    tested; M1 Animated 10 + Dopple 7 - written, the bootstrap
    invented-store desync DESYNC FIX #4 fixed and re-tested PASSED;
    M2 accessors 9 + M3a/M3b Moving 13 - written, gated, in-game
    test OWED). Immediate work: (1) batch M3c - the remaining Moving
    accessor/state cluster DECODED BUT NOT WRITTEN (setOrientation
    0x4A3790 - the BMatrix makeRotateArbitrary :2052 + operator_mul
    :2053 + normalize body; yaw 0x4A3C40 - same BMatrix family +
    the facet/angle calc; stopMoving(vtbl-named) 0x4A3DD0 -
    setForwardVelocity/setForwardAcceleration vtbl calls +
    removeAllFromPathingGroup :1287-region; calculateFutureVelocity
    0x4A3E00 - the BUnitGroup machinery (getUnitGroup :6099,
    isMoving/inDesiredPosition/getSpecificMoveReason -
    lst-resolve) + the sprite->base_speed cap; moveUnit/
    checkDistance/checkRange/incrementWaypoint/precalcDistToTarget
    (:2073-region) /precalcDistToGoal - DECODE FIRST, several are
    real bodies); (2) getPatrolInformation 0x4A2BB0 tail (read to
    .c:174810, body continues); (3) then Static 130 / Combat 48 /
    Building 70 / TRIBE_Combat 75 smalls per the roadmap. PROTOCOL:
    approval gate before every write; MAP_SIZE not baked (markers);
    all other patches baked; deviations need explicit sign-off;
    _countof for declared-array bounds; the store-and-read diff
    incl. the VALUE/SOURCE pass for bootstrap blocks (DESYNC FIX
    #4); the method-inventory regex must handle multi-word returns
    and thunks. Also owed: in-game tests for M2/M3a/M3b."

  - DESYNC FIX #5 - missile_move invented OOB early-return (owner bisect
    + approved + fixed + CONFIRMED FIXED IN-GAME 2026-11-16): the shipped
    missile_move_new invented `return 0` on the out-of-bounds path - the
    original (asm 0x49BF70-0x49C023, full asm pass now done; the first
    decode shipped with its header flagging "asm pass owed" and it was
    never executed) has NO else: die_die_die (slot +0xE4, asm 0x49BFEB)
    falls through to the teleport + comparison on every path (loc_49BFE7
    joins loc_49BFF1; teleport slot +0xA8 args (nx, ny, nz) identity
    order; return = fcomp world_z @+0x50, test ah,40h = the C3-only
    equality idiom, SSE sete result-identical incl. NaN). The invented
    return skipped the teleport (stale world_x/y/z + tile state ->
    simulation divergence) and hard-coded the caller's
    (RGE_Action_Missile__update+39B) result. Why the nets missed it: the
    owed asm pass; the store-and-read diff is 0/0-blind to control flow;
    the compile gate accepts valid-but-wrong C++; the in-game Missile
    test passed because the OOB path never fired there. PROTOCOL
    (owner-approved, TEMPLATE amended): (1) owed-asm-pass blocks WRITTEN
    (protocol step 7); (2) Exit-path inventory added (return-site
    mapping; caught this class on its origin case - the missile_move
    exit paths 0x49C015 = 1 / 0x49C01E = 0, both after the teleport).
    GATE 0 errors 0 warnings; defs/refs 16/16. Re-verify:
    missile_move_new carries the single tail return; the OOB branch
    falls through.

  - DESYNC FIX #6 - TRIBE_Master_Combat_Object__check_placement_new
    float precision (owner bisect + approved + fixed + CONFIRMED FIXED
    IN-GAME 2026-11-16): uninstalling the hook eliminated the desync;
    the instruction-level re-walk found the control flow/operands/
    fields/exit paths FAITHFUL - the divergence was the FLOAT PRECISION
    CLASS: the original runs the fly-over and AABB comparisons on x87
    53-bit intermediates (unrounded differences, unrounded radius sums:
    0x5C94CE-0x5C9536 fly-over, 0x5C9631-0x5C968E AABB) while the
    written code rounded everything to 24-bit first, AND the scan
    rectangle truncated a float-rounded (x -+ 8.0) instead of the
    53-bit difference (0x5C9397-0x5C93DF). Bit-exact forms applied:
    dx as double intermediates (the x87 st0 value), the radius sums as
    53-bit doubles, and - the load-bearing asymmetry - |dy| kept FLOAT
    because the original itself store-rounds it via the arg_24 m32 fst
    (0x5C94E6/0x5C9658) before the y-compare (0x5C952D/0x5C9688).
    Direct float-vs-constant gates untouched (fld/fcomp m32 both sides
    = bit-safe). RULE REFINED (TEMPLATE amended 2026-11-16, owner-
    directed: the FLOAT PRECISION VERIFICATION bullet): comparisons
    consuming arithmetic results need double intermediates; only direct
    loaded-float compares are float-safe; replicate m32 store-rounds;
    (int) conversions consume 53-bit operands. Same-class sites flagged
    in the RGE Static and Building check_placements (sibling scans) -
    actioned only on owner bisect reports. GATE 0 errors 0 warnings;
    defs/refs 1/1; exit paths 4/4 mapped (0x5C9394 = 0, 0x5C974C =
    base_result, 0x5C975A = 9, 0x5C9766 = 6). Re-verify:
    TRIBE_Master_Combat_Object.cpp fly-over/AABB blocks carry the
    double/float asymmetric forms with asm addresses 0x5C94CE-0x5C9536 /
    0x5C9631-0x5C9688; rect via (int)((double)x -+ 8.0).

  - BVECTOR FIELD-NAME CORRECTION (owner investigation + idb fix +
    regen + repo audit 2026-11-16): the BVector definition
    (idb_types.h:1761) was {x, y, z} with the .y/.z NAMES swapped
    relative to their semantics - the true layout is {x, z, y}: offset
    +4 = z (the vertical/height axis), offset +8 = y (the second
    horizontal/row axis). VERIFICATION CHAIN: upVector @0x6AD128 =
    {0, 1.0@+4, 0} = the yaw rotation axis (vertical at +4); the Moving
    ctor's orientationRightValue inits its -1 at offset +8 (a
    horizontal axis); the owner's re-typed BMotion::updateMovement dump
    pairs buffer.z(offset8) <- world_y and buffer.y(offset4) <- 0 (the
    zeroed vertical) for a planar vector, and its planar distance norm
    excludes offset +4; radius_z = the blocking height;
    handle_teleport_z_correction = the height fixer. Owner fixed the
    header ({x, z, y}, idb_types.h:1761-1766) and regenerated .c/.lst
    (markers shifted - missile_move .c :169717 -> :169714,
    incrementWaypoint :175706 -> :175691, check_placement(70)
    :392521 -> :392468; line-only citations are stale, addresses
    remain valid; the citation protocol now mandates address +
    function-name citations - TEMPLATE step 8 amended). CONSEQUENCES:
    (1) every "y/z swap" observed at the teleport call sites
    (setPosition/moveForward/incrementWaypoint/addUserDefinedWaypoint/
    make_object_collision_list p1/p2) was the naming artifact - in TRUE
    names all sites pass (x, y, z) identity; (2) the teleport body's
    consumption is param1->world_x @0x48, param2->world_y @0x4C,
    param3->world_z @0x50 (RGE_Static_Object__teleport 0x54F983-
    0x54F99A; the Moving wrapper passes its params through verbatim,
    0x4A16D2-0x4A16D7) - identity in true names; (3) the REVOKED M3b
    verdict RESTORES: moveForward_new/setPosition_new are correct as
    shipped (their pre-regeneration compiled offsets matched the
    original under the OLD header). HEADER-RENAME CODE AUDIT (8 sites
    fixed, 2 TUs gated 0 errors 0 warnings): the rename moved which
    offset .y/.z resolve to (.y: 4->8, .z: 8->4), breaking 8 shipped
    accesses - orientationRightValue init x2 (RGE_Moving_Object.cpp
    ctor/ctor_read: the -1 back to offset8/.y), addUserDefinedWaypoint
    store + floor-centering x3 (offset8/.y), both teleport arg lists ->
    identity (moveForward_new/setPosition_new). SAFE sites (paired
    zero/same-name copies - offset-preserving under either naming):
    positionValue/mSubstitutePosition/orientationForward/mEndpoint1-2
    zeros, the velocity/acceleration/position getters, the resize copy,
    the moveForward component adds. incrementWaypoint gate-min
    CORRECTION (the regenerated .c + the ebp-corrected asm walk agree):
    the min is a dedicated local (the compiler reuses the dead
    path-argument slot for it), seeded on BOTH branches (obj:
    distance-range-0.1; no-obj: wd-range-0.1 - the first draft's "dead
    store" was WRONG), min-updated over the gate children, zeroed by
    flags&4 - withdrawing the out3.x-alias form and the "undefined
    pre-value" analysis. NAMING RULE (TEMPLATE): BVector members are
    .x/.z/.y in offset order - decodes name offset +4 .z and offset +8
    .y. Re-verify: idb_types.h:1761-1766; RGE_Moving_Object.cpp
    :66-67/145-146/436-444/590-609; RGE_Missile_Object.cpp:422-427.

  - SPEED2 BAKE - maximumSpeed_new amendment (owner bisect-era finding +
    approved + fixed 2026-11-16): the corrected step-3b writer scan
    (2,795 sites vs ALL owned ranges) surfaced casts.cpp:881
    setHook(0x004A28C0, speed2) - an ENTRY collision with the M3a-owned
    maximumSpeed_new that the M3a-range scan missed (no writer-target
    scan recorded for those ranges; gap closed). Install order
    (setCastHooks dllmain.cpp:451 < install_engine dllmain.cpp:765) had
    our hook overwriting speed2's E9 = the speed modifier dead in
    shipping builds. BAKED: maximumSpeed_new fetches getSpeedModifier
    ((RGE_Static_Object*)this_) (casts.cpp:140, __stdcall, raw float
    bits in eax; extern cpp-local in RGE_Moving_Object.cpp)
    unconditionally before the gate and multiplies it into master->speed
    BEFORE base_speed (left-assoc = the stub's fmul order, x87 PC53 =
    double rounding; written as (double)-intermediates + one float
    conversion); the stub's double-zero fcomp gate is result-identical
    to a float compare incl. NaN. Bits->float via a 4-byte memcpy
    (own-instance presentation). Behavior: bit-identical to the
    pre-amendment build for units without speedReductionEnabled (x1.0f
    is exact); patch-faithful for reduced units. casts.cpp:881 retired
    in place (0 active sites; marker present). // SPEED_MOD markers at
    the affected lines. Same-scan dispositions: core.cpp:1429
    writeDword(0x46FE74) = a j_-thunk bypass to the hooked Static
    destructor itself (0x46FE78 + 0x000DB808 = 0x54B680) -
    behaviorally a no-op, nothing to bake (retirement optional);
    casts.cpp:882 speed3 + core.cpp:1393 writeByte(0x004A3E59, 0x16) =
    the M3c calculateFutureVelocity folds (baked there). GATE 2/2 TUs
    0 errors 0 warnings; defs/refs 1/1. Re-verify: grep casts.cpp for
    active 'setHook(0x004A28C0' (= 0); the // SPEED_MOD markers in
    RGE_Moving_Object.cpp.

  - OWNER RULING (2026-11-15): the *(float*)&launcher_obj bit-
    reinterpret at RGE_Missile_Object.cpp:413 was an instance-specific
    one-off decision - it does NOT establish a reusable precedent;
    every future punned-conversion instance is presented and approved
    individually (deviations-never-inherit). First application of the
    rule: the maximumSpeed SPEED2 bake used a memcpy conversion instead
    of citing that precedent.

  - RUNTIME METHODS BATCH M3c - Moving pathing/state methods (owner-
    approved + WRITTEN 2026-11-16): 11 hooks appended to RGE_Moving_
    Object.cpp (installer 31 -> 42). getPatrolInformation 0x4A2BB0
    (SEH allocation-unwind fold; the J1 0-init), setOrientation 0x4A3790
    + yaw 0x4A3C40 (the two full-3D normalizes + the facet/angle calc +
    the setOrientation vtbl pass), stopMoving 0x4A3DD0 (the three vtbl
    gates + removeAllFromPathingGroup), calculateFutureVelocity
    0x4A3E00 (BAKED: the speed3 modifier fold + the single
    getSpecificMoveReason call - core.cpp:1393's writeByte and
    casts.cpp:882's speed3 entry hook folded, call sites retired),
    precalcDistToTarget 0x4A3F40 / precalcDistToGoal 0x4A4000 (the
    gate-8 linked-child prefix loops, J3), incrementWaypoint 0x4A40A0
    (the linear block's identity teleport (x, y, z), the four
    bounds-fixup waypoint calls targeting buffer3, the patrol branch,
    the gate-min as a dedicated local seeded on both branches - the
    regenerated .c's patha; the first draft's out3.x-alias and "dead
    seed" readings withdrawn), checkDistance 0x4A4460 (the linked-path
    type machine), checkRange 0x4A45A0, moveUnit 0x4A4650 (the 4th
    argument is a POINTER - retn 0xC = 12 bytes; the .x/.y compares vs
    dbl_654DC0; the idb constant's trailing by-value BVector is an
    artifact). ALL float arithmetic feeding comparisons written in
    double intermediates per the FLOAT PRECISION rule; direct
    loaded-float gates stay float. BVector members use the corrected
    {x, z, y} names throughout. ALSO WRITTEN: make_object_collision_
    list_new signature corrected to the owner-fixed prototype
    (target_obj, old_x, old_y, old_z - the pre-move position snapshot;
    p1/p2 = the traveled segment; the target_obj domain selection) -
    the puns removed. COLLISION SCAN: 0 non-self overlaps (the
    SPEED2/writeByte bakes are IN the code; casts.cpp:881/882 +
    core.cpp:1393 retired). GATE 0 errors 0 warnings (RGE_Moving_
    Object.cpp + RGE_Missile_Object.cpp). defs/refs: Moving 42/42,
    Missile 16/16; exit paths mapped per function; base_game 0 bare;
    CRT 0; pointer-walk 0 real. Re-verify: the 42 Moving _new
    definitions + the 42 installer hooks; the 16 Missile defs/refs;
    the double/float asymmetric forms at the flagged sites.

  - PROTOTYPE CORRECTION #4 - make_object_collision_list (owner-
    investigated + corrected 2026-11-16): the last parameter is a
    FLOAT (the pre-move z), not an int; the first is an OBJECT POINTER
    (the target), not a float. Caught by the body-vs-prototype
    contradictions (the raw-dword pun store, the float param
    dereferenced as an object pointer) + the caller construction
    (.c:50446-50451 the pre-move position snapshot, :50555 missile_move,
    :50585 the call with this->target_obj). Template amended: the
    Prototype / type-definition suspicion gate (red flags: puns,
    retn-vs-arg-byte mismatches, swaps, .c-vs-push-order
    contradictions, phantom params, oversized out-params,
    contradicting callers - each a HARD STOP until the owner corrects
    the IDB; resolution = present the contradiction with addresses,
    restart from the regenerated files). Citation policy amended
    (TEMPLATE step 8): address + function-name citations mandatory;
    line-only citations dropped (the .c/.lst regens shift lines).

  - M3c CORRECTIONS - precalcDistToGoal + incrementWaypoint (owner
    bisect + approved + fixed 2026-11-16): both desynced under the
    corrected-BVector header. (1) precalcDistToGoal_new: the applied
    batch carried the pre-correction draft name wp1->z (offset4, the
    height) where the regenerated .c (:175686-175688) reads ->y
    (offset8) - the distance_to_position y-arg received the waypoint's
    height, so the "distance to goal" was measured against (wp.x,
    height, 0). FIXED: wp1->y. (2) incrementWaypoint_new: the inner
    state-gate tested wpHeight (the linear block's leftover waypoint
    height, ~0 for ground waypoints) where the true operand is wd (the
    gate distance - distance_to_position to waypoint-1) - loc_4A439F
    per the regenerated .c (:175805-175838: if (v24 >= 1e-7)
    waypoint--, state=2). The pre-regeneration dump's v26 "wpHeight
    test" was the derailed-stack-tracking artifact (the teleport
    signature is now known, the tracking completes). FIXED: the inner
    gate tests wd; the wpHeight local and the J5 undefined-read flag
    WITHDRAWN (the height value is teleport-param3-only). ALSO FOUND,
    same class: moveUnit_new's compares were last->z/a4->z (offset4)
    where the original compares offset8 (.y per the regenerated .c
    :175986-175988) - FIXED in the same pass (found proactively, not
    bisected). ROOT CAUSE: the batch application carried the
    pre-BVector-correction draft names for the functions not re-derived
    from the regenerated .c. LESSON (owner-directed): batch applications
    must re-grep EVERY BVector member access against the regenerated .c
    rendering before the gate. GATE 0 errors 0 warnings; defs 42
    unchanged. Re-verify: grep RGE_Moving_Object.cpp for wpHeight/wpZ
    (= the correction comments only); precalcDistToGoal reads wp1->y;
    incrementWaypoint's inner gate tests wd.

  - FLOAT ST0-BOUNDARY MECHANISM + PROJECT-WIDE DECLARATION CHANGE
    (STAGED 2026-11-16): the remaining incrementWaypoint desync (post
    wd-fix) = the x87-st0 call-boundary rounding: the original consumes
    game-float-function returns DIRECTLY off st0 at 53-bit (the seed
    chain: call distance_to_object; fsub range; fsub 0.1; fstp patha -
    no intermediate store) while our /arch:SSE2-compiled replacements
    round the callee's st0 to 24-bit at the call boundary (fstp m32
    forced by the float-typed declarations) before the chain -> ULP
    divergences flip the 1e-7/1.0/radius gates -> desync (chain-heavy
    functions only: precalcDistToGoal was cured by the .y fix - its
    float pipeline is a single-value passthrough; moveUnit's compares
    are bit-exact doubles). FIX (owner-directed, STAGED): the project-
    wide declaration change - ALL float-returning game-function
    prototypes in idb_functions.h (86) -> double-returning (the owner
    provides the updated header + the regenerated .c/.lst at the next
    session start); the 7 float-returning replacements -> double-
    returning (getSpeed_new, handle_teleport_z_correction_new,
    getTurnRadius_new, maximumSpeed_new, calculateFutureVelocity_new,
    precalcDistToTarget_new, precalcDistToGoal_new); the TEMPLATE
    corollary (h) to the FLOAT PRECISION rule: float-returning
    replacements are double-returning (the original game consumes
    returned st0 at full x87 precision; a float-typed return truncates
    at the 24-bit boundary), and the per-site discipline: capture
    53-bit where the original consumed st0 in a chain; round to float
    immediately where the original did fstp m32. RESIDUAL: the 80->53->24
    double-rounding anomaly at the original's store-first sites
    (2^-29/capture - negligible; both patched peers identical; vs
    base-game peers extremely rare). ABI NOTE: float/double returns are
    both st0 - the declaration change is binary-equivalent for the
    untouched callee binaries. BINARY-EQUIVALENCE VERIFIED:
    calculateFutureVelocity's cap-return path leaves st0 = the 53-bit
    cap unrounded (asm 0x4A3EE6 -> retn); precalcDistToGoal is a pure
    st0 passthrough (call distance_to_position; retn). Re-verify: the
    updated idb_functions.h; the 7 double-returning signatures.

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first - NOTE: .c/.lst were REGENERATED twice (the
    BVector rename + the prototype corrections): the ledger's LINE
    citations are stale - cite addresses + function names; the .lst
    stack labels are now RELIABLE (the teleport signature is known, the
    tracking completes). State: the M3c batch (11 Moving pathing/state
    hooks, installer 42) is written + gated + the desync corrections
    applied; the BVector struct is CORRECTED ({x, z, y} - offset4 = z
    the vertical, offset8 = y the row); four prototype corrections are
    in (nextPatrolWaypoint 1-arg; BPath__waypoint (path, BVector* out,
    int index); moveUnit's 4th arg = BVector*; make_object_collision_
    list (target_obj, old_x, old_y, old_z)). IMMEDIATE WORK: (1) I
    provide the updated idb_functions.h with ALL float-returning
    prototypes changed to double-returning (86) - verify the sweep,
    then convert the 7 float-returning replacements to double-
    returning (getSpeed_new, handle_teleport_z_correction_new,
    getTurnRadius_new, maximumSpeed_new, calculateFutureVelocity_new,
    precalcDistToTarget_new, precalcDistToGoal_new) + add the TEMPLATE
    FLOAT PRECISION corollary (h): float-returning replacements are
    double-returning (the game consumes returned st0 at full x87
    precision; a float-typed return truncates at the 24-bit boundary)
    + full-tree re-gate + the precision sweeps. (2) The in-game test
    build owes: M2/M3a/M3b, missile_move (FIX #5 re-test), the M3c
    batch, incrementWaypoint + precalcDistToGoal (the desync re-tests),
    maximumSpeed, make_object_collision_list, the 8 BVector-audit
    sites. (3) Then the roadmap: the runtime-methods inventory re-count
    (the pre-regeneration counts are stale), the remaining methods
    smallest-first per class, doIntelligentMovementUpdate (the huge
    unowned caller - its future bake inventory: speed4 @0x4A6250,
    checkAcceleration_patch @0x4A6569, isMoving_v2 @0x4A3D90), the
    sibling float-precision sites (RGE Static / Building
    check_placements - on bisect reports only), the five derived ctor1
    copy-constructors. PROTOCOL: the prototype/type suspicion gate
    (HARD STOP on puns/order/retn mismatches - the owner corrects the
    IDB, the decode restarts from the regenerated files); address +
    function-name citations only; the FLOAT PRECISION rules (a)-(g) +
    the st0-boundary corollary (h); the exit-path inventory; owed-asm-
    pass blocks WRITTEN; the desync reports are bisect verdicts
    (re-verify the named function alone); the approval gate before
    every write."

  - RUNTIME METHODS BATCH M4 - movement-data + sprite pairs (owner-
    approved + WRITTEN 2026-11-16): 9 hooks - RGE_Moving_Object.cpp +8
    (installer 42 -> 50), RGE_Animated_Object.cpp +1 (installer 11 ->
    12 - THE ANIMATED CLASS IS COMPLETE, 12/12 exports owned).
    forwardVelocity @0x004A3A10 / forwardAcceleration @0x004A3AC0
    (double returns; movementData @+0x120 -> velocity/acceleration.y,
    fld m32 exits - bit-identical); setForwardVelocity @0x004A3A30 /
    setForwardAcceleration @0x004A3AE0 (SEH allocation-unwind chunks
    0x6428A0/0x6428C0 folded with the 0-init; the v!=0 C3-only gate =
    setne result-identical incl. NaN; operator_new(0x18) = sizeof-
    (BMovementData) walked; BMovementData ctor via the idb constant
    0x004BBCF0; the store SHARED by existing/created paths);
    calculateMaximumSpeed @0x004A6250 (double, retn 4, terrainType
    unused) - BAKED: speed4 (casts.cpp:885) folded, call site retired,
    // SPEED_MOD markers - install order had silenced the stub since
    its inception, this bake is its first effective shipping form;
    setRunSprite @0x004A69B0 / setMoveSprite @0x004A69F0 /
    setFidgetSprite @0x004A6A30 - the asm-proven DOUBLE master-vtbl
    call kept (test call + second call feeding new_sprite slot +0xB0;
    the result NEVER stored - DESYNC FIX #4 form); master vtbl slots
    30/31 (+0x78/+0x7C) are DECLARED ONLY IN THE DERIVED MASTER VTBLs
    (verified member-by-member across Moving def@14980/Missile
    def@21459/Action/Combat/TRIBE tiers - the Static master vtbl ends
    at slot 29) - the replacements cast master_obj to
    RGE_Master_Moving_Object* (lowest declaring tier; layout-safe
    vfptr@0; runtime evidence = the functions sit in the runtime
    Moving+ vftables -> Moving+ masters per factory identity);
    setFidgetSprite needs NO cast (get_sprite = slot 1, in the Static
    vtbl). RGE_Animated_Object__RGE_Animated_Object_read_new
    @0x00422B70 (SEH chunk 0x640F90 = the Static-destructor fold; base
    :3787 HOOKED -> routes with do_setup = 0; vfptr 0x006566A0 stored
    UNCONDITIONALLY before the do_setup branch; type @+0x5E = 0x14;
    rge_read &speed @+0xB4 sizeof = 4) - the owned Dopple/Moving
    read-ctors' base calls now route. DISCREPANCY FIXED: casts.cpp:884
    setHook(0x004A3EAD, speed3) was STILL ACTIVE despite the M3c
    ledger's retirement claim (the bake was in
    calculateFutureVelocity_new; the comment-out had been missed -
    writing into dead original-body bytes, behaviorally harmless;
    retired now with a note). COLLISION SCAN: the 9 ranges - only
    speed4 (baked); functionlist.cpp table = dead profiler (standing
    disposition). GATE: 0 errors 0 warnings x3 TUs
    (Moving/Animated/casts) x86. defs/refs 50/50 + 12/12;
    store-and-read diff FULL MATCH (2/2 + 2/2 + 2/2); exit paths
    mapped; CRT 0; base_game 0; pointer-walk 0; cast sweep clean.
    Runtime totals: 133 hooks; 408 runtime methods remain (Moving 47,
    Action 38, Combat 48, Static 130, TRIBE_Combat 75,
    TRIBE_Building 70). Re-verify: the 9 new defs + the 50/12
    installers; grep casts.cpp for active setHook 0x004A6250 /
    0x004A3EAD (= 0).

  - RUNTIME METHODS BATCH M5a/M5b (owner-approved + WRITTEN
    2026-11-16/17): 4 hooks in RGE_Moving_Object.cpp (installer 50 ->
    54). distanceToHighestLevelPath2 @0x004A2C90 /
    distanceToHighestLevelPath @0x004A2D00 (double; pathData @+0x118
    guard; highestLinkedPathPointer :2509 -> else (*base_game)->world
    @+0x420 -> highestLinkedPath :2508 + RGE_Game_World__path :6094;
    BPath__distanceFromPath :2528 - PURE st0 passthroughs, corollary
    (h); the by-value BVector point param confirmed REAL, retn 0Ch +
    the asm stack-copy); calculateTurnRadius @0x004A6510 /
    calculateYawAngle @0x004A6350 - UNBLOCKED by the owner's vtbl slot
    re-typing (RGE_Master_Static_ObjectVtbl maximumSpeed :3605 /
    getTurnRadius :3608 / getTurnRadiusSpeed :3609 /
    getMaximumYawPerSecondMoving :3612 all double now; slots 23/24/27
    in the Static vtbl, no casts). TurnRadius: product fstp store-round
    @0x4A6545 (float local kept), fdivr quotient st0 unrounded at retn.
    YawAngle: angleBetweenVector :5791 fstp v store-round @0x4A636B
    (float); yaw*interval fmul on raw st0 (53-bit, no store); merge
    fld v/fcomp st(1); limit path C0-only v<0 test (comiss/jb
    NaN-equivalent) -> fchs -> fstp v store-round @0x4A639C;
    operator_mul :5789; result.z (the corrected BVector vertical)
    <= 0 rule-(b) gate; +-v exits exact. GATE CATCHES:
    calculateTurnRadius's master assignment needed the explicit
    cross-class cast (base-ward assignments are conversions too);
    moveUnit's v14 capture - the re-typed runtime calculateYawAngle
    slot surfaced the C4244 - asm fstp a3 @0x4A46F6 proves the
    store-round, explicit (float) applied. The corollary-(h) vtbl
    residual is CLOSED for these slots; the runtime-vtbl equivalents
    should be swept the same way on future dispatch sites. GATE:
    0 errors 0 warnings x1 TU x86; defs/refs 54/54. TEMPLATE AMENDED
    (owner-directed 2026-11-17): context-window size is never a
    stopping reason or a scope-reduction argument. Remaining Moving 43
    (M6 = the evacuate_area cluster: evacuate_area @0x004A6BA0
    move_to-arg re-pull + find_evacuate_area @0x004A6C20). Re-verify:
    the 4 defs + the 54 installer hooks.

  - RUNTIME METHODS BATCH M6a/M6b + COLLISION INCIDENT (owner-approved
    + WRITTEN 2026-11-17/18): 3 hooks in RGE_Moving_Object.cpp
    (installer 54 -> 57). evacuate_area @0x004A6BA0 (void, retn 10h;
    the int rendering asm-confirmed - raw dword forwarding to
    find_evacuate_area (:2210), the out-params are ints read back with
    fild; is_moving slot +0x144 gate; move_to slot +0x118 with
    (0, ex+0.5, ey+0.5, world_z) - the two fild/fadd const_f_half/fstp
    m32 store-rounds replicated; the .c's "float v8" slot = the int ey
    out-param artifact); find_evacuate_area @0x004A6C20 (the Bresenham
    perimeter walk: ftol world tiles, the budget = 2*(y2-y1)-x1+x2+3,
    the setle/dec/and-2 nearest-edge pick, the 4-case direction state
    machine (all four asm cases decoded), the two-walker probe loop,
    the distance_to_position scoring with the m32 store-rounds);
    try_evac_tile @0x004A6F00 (the && OOB gates asm-exact and
    dead-in-practice - kept faithfully; placement slot +0x38 with the
    float-bit args; totalCanPath :2537 with the goal
    {x+0.5, 0, y+0.5} - the .c-vs-asm "swap" resolved as frame-scratch
    reuse). BAKED (EVAC_OOS_FIX): playeroptions.cpp:206
    setHook(0x004A6E95, on_evac_area) - a MID-FUNCTION entry hook at
    the walker-flip/exit join of find_evacuate_area, retired in place.
    THE PATCH'S REASON (owner hint, confirmed at instruction level):
    the original's walkers-met exit compares read the NEVER-STORED
    mirror slots (the .c's v38[2]/v37[2] = asm var_4/var_10 - no
    stores in [0x4A6C20, 0x4A6ECE)) = per-machine stack garbage ->
    per-client walk divergence -> the OOS. The stub gates on the
    owner's option byte ([owner+0x21], the playeroptions
    CURRENT_PLAYER_OPTION_LEVEL): == 0x0B -> the original behavior
    (replay + resume 0x4A6EA3); != 0x0B -> budget-only continuation.
    The written mirrors are DETERMINISTICALLY ZERO-INITIALIZED (owner
    direction; the constant is tunable - future work: identify which
    constants reproduce original outcomes most often); the 0x0B legacy
    path reads them in the original's shape (deviation signed off).
    RESTRUCTURE (owner-directed): the asm while(1) with the top
    decrement + the bottom exits = a for loop (exactly `budget`
    iterations; the legacy walker-meet exits map to break).
    COLLISION INCIDENT + ROOT CAUSE: the M6b presentation shipped
    without its own collision scan - the M4-era scan verdict was
    inherited and the hand-built interior address patterns missed the
    0x4A6D/0x4A6E span (playeroptions.cpp:206 @0x004A6E95 sits inside
    find_evacuate_area). TEMPLATE AMENDED (step 3b, PER-BATCH SCAN
    RE-RUN): the mechanical writer-target scan re-runs for EVERY batch
    over ALL owned + pending ranges; clean verdicts are never
    inherited; hand-built per-range address patterns are PROHIBITED;
    every presentation carries its own Collision audit section with
    the scan output quoted. MECHANICAL RE-SCAN over the ten M4-M6b
    ranges: 15 distinct hits = self installer hooks + the retired
    speed4 + the one real collision (0x4A6E95) - M4/M5/M6a ranges
    clean. COMMENT STYLE (owner-directed 2026-11-17/18): the
    semantic-header + inline-comment style (per
    check_placement_new/alignment_new/get_string_id_new) codified in
    TEMPLATE - SEMANTIC comments go ABOVE the code in natural groups;
    asm citations trail on the statement lines; the M4-M6b headers
    retrofitted (18 functions) and the evacuation-cluster bodies
    converted (find_evacuate_area the exemplar; try_evac_tile,
    evacuate_area done; the remainder of the M4/M5 bodies continue).
    M6b VERIFICATION RE-RUN (owner-directed 2026-11-18): full
    instruction-level re-walk incl. the previously-unpulled
    0x4A6CC6-0x4A6D53 gap - var_4/var_10 confirmed NEVER stored (the
    bake rationale holds at instruction level); all four initial-switch
    cases decoded and matched (the two-case reduction provable); the
    stub's esp-relative operands reconciled with the body's slots
    ([esp+38h] = var_C, [esp+18h] = var_2C = budget, [esp+13h] =
    var_31); store-and-read diff FULL MATCH; exit paths mapped (single
    retn + the stub's replicated epilogue); the for-loop iteration
    count re-verified against the in-place decrement. FAITHFUL.
    GATE: 0 errors 0 warnings (RGE_Moving_Object.cpp + playeroptions.cpp
    + RGE_Animated_Object.cpp) x86; defs/refs 57/57. Remaining Moving
    methods 39 (the save/setup families, the medium bodies,
    doIntelligentMovementUpdate with its bake inventory:
    checkAcceleration_patch @0x4A6569, isMoving_v2 @0x4A3D90 - speed4
    already baked). Re-verify: the 3 defs + the 57 installer hooks;
    grep playeroptions.cpp for active setHook 0x004A6E95 (= 0).

  - CALCULATEYAWANGLE DESYNC + PRECISION SUB-RULE (i) + PROBE TABLE +
    LOGGER PROCESS (owner-reported bisect + root-caused + fixed +
    CONFIRMED FIXED IN-GAME 2026-11-18): RGE_Moving_Object__
    calculateYawAngle_new desynced (the turn direction sign-flipped
    +/-pi/2). ROOT CAUSE (the owner's A/B capture - inputs identical:
    getMaximumYawPerSecondMoving = FLT_MAX, curOF normalized, newOF =
    NaN, interval = 0.04, limitedAngle = 0): the direction gate's x87
    idiom "fcomp const_f_zero; test ah,41h; jnz" is UNORDERED-TRUE -
    NaN result.z (from operator_mul over the NaN newOF) takes the +v
    branch in the original; the literal C result.z <= 0.0f is
    unordered-FALSE in compiled MSVC -> the replacement took -v.
    FIXED: all three compares in the function use the negated-opposite
    forms ((double)v > yaw / v >= 0.0f / result.z > 0.0f, negated) -
    MSVC compiles them comiss/comisd + jna/jbe = unordered-taken,
    bit-matching the x87 C0|C3 / C0-only idioms. TEMPLATE: precision
    sub-rule (i) written (X87 COMPARE-BRANCH UNORDERED POLARITY) with
    the POLARITY TABLE (jnz/jz target positive vs negative branch) and
    the PROBE-VERIFIED COMPLETE TABLE: a compiled-and-EXECUTED probe
    (qNaN vs constant, /O2 /arch:SSE2 /fp:fast and /Od, identical)
    proved EVERY literal ordered predicate unordered-FALSE, x != c
    unordered-TRUE (the one exception), all four negated forms
    unordered-TRUE, value-return == (sete) unordered-TRUE (matches
    x87), branch == unordered-FALSE (diverges). PROBE PROTOCOL
    mandatory for NaN-form decisions (compiled AND executed under the
    project's real flags, runtime-seeded NaN, /Fa jcc verification).
    SWEEP APPLIED TO ALL OWNED CODE: FIXED 4 further divergent sites -
    setForwardVelocity_new/setForwardAcceleration_new (the v != 0.0f
    lazy-create gate -> the ordered-nonzero disjunction (v < 0)||(v > 0):
    the x87 test ah,40h; jnz -> skip set includes unordered, NaN
    commands do NOT allocate in the original; the literal != would
    allocate + store a NaN velocity), find_evacuate_area_new (the
    scoring update -> !(score >= bestDist): the x87 test ah,1
    FALL-THROUGH update fires on unordered - a NaN distance takes the
    tile), precalcDistToTarget_new (the gate-child min -> !(d >=
    result)). VERIFIED MATCHING (kept): maximumSpeed_new/
    calculateMaximumSpeed_new (the > 0 cap gates - the jnz targets the
    negative path, the literal form matches), moveUnit_new (the fabs
    >= 0.5 pair - the literal form reproduces the original's NaN ->
    aligned flow, asm 0x4A473E-0x4A474F test ah,1; jz), incrementWaypoint
    _new (the three 1e-7/1.0 gates, jz targets the >= branch), checkRange
    _new, moveForward_new (NaN skips in both), missile_move_new (the
    sete value-return). FLAGGED for per-site pulls next: yaw_new /
    make_object_collision_list (the pull windows came back empty).
    DIAGNOSTIC LOGGER PROCESS codified in TEMPLATE (owner-directed):
    any owned function gets the A/B harness on request - the _orig
    naked trampoline (replay the prologue past the E9 with
    esp-adjusted operand reads, jmp entry + replayed length; the
    owner's calculateYawAngle_orig verified as the exemplar incl. the
    [esp+18h]=newOF frame reconciliation), the _logger wrapper (sprintf
    inputs -> call _orig -> sprintf outputs + the updated out-params ->
    val_log.txt "at"-mode lazy append), the installer swap (hook the
    logger; the _new stays compiled for A/B; flipping the line restores
    the fix - the current installer :2146 was flipped back to _new by
    the owner with the fix confirmed). CRT NOTE: the logger's bare
    sprintf/fopen/fputs = the approved diagnostic exception (owner-
    supervised, removed before final ship). GATE 0 errors 0 warnings
    (RGE_Moving_Object.cpp + playeroptions.cpp). Re-verify: the three
    negated-compare forms at RGE_Moving_Object.cpp :1673/:1683/:1701;
    the ordered-nonzero gates in the two setter bodies; the negated >=
    updates in precalcDistToTarget_new/find_evacuate_area_new; the probe
    source preserved at %TEMP%\opencode\nanprobe.cpp; the rule
    (i) table + the logger process in DECOMP_TEMPLATE.md.

  - RUNTIME METHODS BATCH M7 (owner-approved + WRITTEN 2026-11-18):
    5 hooks in RGE_Moving_Object.cpp (installer 57 -> 62). handle_teleport
    _z_correction @0x004A1970 (double; the static correction :3833
    UNHOOKED via the double-typed constant; the ground-match is the x87
    test ah,40h; jz -> zero idiom - the on_ground=1 set is {=, NaN} ->
    X87_EQ_NAN (the composed form); tile @+0x28 == 0 -> on_ground = 0;
    the world_z @+0x50 return, exact widening); angle_to_object
    @0x004A1100 (double, retn 0Ch; the facet guards INT tests; the planar
    delta to_object-or-raw; atan2; the WRAP COMPARES are test ah,1; jz ->
    skip idioms - unordered-taken -> X87_LT_NAN x2 (a NaN angle gets the
    +2pi shifts in the original); the middle v8 > pi compare is test
    ah,41h; jnz -> skip - the jnz targets the SKIP path so the literal >
    MATCHES (kept literal); the scale fild facet; fmul flt_65C1A8
    (0.15915494 = 1/2pi); fmulp st(1),st = the .c multiply order exact);
    setVelocity @0x004A3870 / setAcceleration @0x004A3960 (void, retn 8;
    the FULL-NORM lazy-create gate sqrt(x^2 + z^2 + y^2) != 0.0 - the x87
    != render is the ORDERED-NONZERO test (a NaN vector does NOT
    allocate) -> X87_NE_ORD(sqrt(...), 0.0); SEH chunks 0x642860/0x642880;
    the guarded ctor; the 12-byte velocity/acceleration copies;
    RESTRUCTURE documented (the nested if/else vs the .c's embedded-create
    form - path-exact, the store shared)); calculateForward @0x004A6290
    (retn 0Ch, int = always 1; the three-component coincidence chain is
    the test ah,40h; jz -> normalize idiom x3 - the taken set per
    component is {=, NaN}: ANY NaN component reads EQUAL and the unit
    keeps its orientation -> X87_EQ_NAN x3; else operator_minus
    (this = destination) + the PLANAR normalize (sqrt over .x/.y, the
    vertical zeroed via the divide) + return 1). MACRO USAGE: X87_EQ_NAN
    x4, X87_LT_NAN x2, X87_NE_ORD x2 - all unordered-inclusive compares
    via the macros per the mandate. GATE CATCH: the batch insert omitted
    calculateForward_new's body while its hook was pre-added - caught by
    the defs/refs orphan check, fixed, re-gated clean. GATE 0 errors 0
    warnings x1 TU x86; defs/refs 62/62 (installer 62). /Fa jcc
    verification PASSED on calculateYawAngle_new (the exemplar: all
    three macro expansions emit the CF-reading unordered-taken branches
    - ja/jae-not-taken; the constants fold to the right addresses
    0x606D60/0x606B00; the master slot dispatch mov eax,[edx+108] =
    +0x6C; the fstp QWORD/movsd dance introduces no extra rounding; the
    xorps sign-flips = exact fchs) - the same-expansion argument covers
    the sibling macros; the full per-function /Fa sweep continues.
    Remaining Moving methods 34 (M8 candidates smallest-first:
    passableTile2 249, addCurrentPath 258, calculateReachedPoint 270,
    passableTile 274, straightProjectedPosition 284, initMovementData
    298; then findFirstTerrainAlongExceptionPath 338, canLinePath 349,
    findAvoidancePath 351, canQuickPath 353, canLinePath2 397,
    addFuturePath 414, firstTileAlongLine 425, canPathWithObstructions
    465, turn_towards 514, checkAcceleration 750, resolveCollisions 975;
    the capstone doIntelligentMovementUpdate 5819 with its bake
    inventory: checkAcceleration_patch @0x4A6569, isMoving_v2 @0x4A3D90 -
    speed4 already baked). Re-verify: the 5 defs + the 62 installer
    hooks; the macros at the 7 call sites; x87_nan.h in stdafx.h +
    the vcxproj ClInclude.

  - RUNTIME METHODS BATCH M8a (owner-approved + WRITTEN 2026-11-18/19):
    3 hooks in RGE_Moving_Object.cpp (installer 62 -> 65).
    straightProjectedPosition @0x004A6860 (BVector*, retn 0Ch; the
    scalar-overload operator_mul multiplies INLINED (the moveForward
    precedent - the free operator_mul is un-exported, call operator_mul
    with ECX = sret); operator_plus :5786 x2 (velocity+resulta;
    positionValue+advanced); the cap: the tile under the unit via
    ftol(positionValue.y)=row / ftol(positionValue.x)=col ->
    map_row_offset[row][col].terrain_type (the real RGE_Map field
    @0xBF18, terrain_type @+5, the 32-byte stride shl eax,5) -> the
    calculateMaximumSpeed dispatch call [eax+31Ch] (slot 199 = our
    hook); the two-gate clamp in the polarity-exact literal-composite
    form !(v10.y > 0.0f && v10.y <= cap) - NaN clamps to the cap (the
    polarity-table MATCH row: both x87 jz target the negative/keep
    paths)); calculateReachedPoint @0x004A63F0 (int, retn 0Ch; the
    three operator_minus deltas; the flight-phase flatten (flags 3/4 ->
    vz = 0 + toPoint2.z = 0); the three sqrt lengths in the .c's exact
    operand orders (len1 = y^2+x^2+vz^2; len2 = y^2+x^2+z^2; segLen =
    y^2+z^2+x^2); the epsilon gate unordered-taken -> X87_LT_NAN(len2,
    1e-7) -> return 1; the segment gate {<=, NaN}-taken -> X87_LE_NAN
    (segLen, len1) -> return 0; the final ladder literal (ordered by
    construction)); initMovementData @0x4A6A70 (void; the movement-state
    reset) - THE STORE-SET DIFF FOUND TWO ITEMS THE .C RENDER DROPS:
    (1) the vtable call slot +0x264 = 153 = removeAllUserDefinedWaypoints
    (asm 0x4A6AC7, HOOKED -> routes to removeAllUserDefinedWaypoints_new;
    the movement reset wipes the user waypoints) and (2)
    orientationForwardValue.x = 1.0 (asm 0x4A6ACD-0x4A6AEB) - both
    written. The rest: the terrain exceptions -1, the motion scalars
    zeroed, the orientation reset (forward.x = 1.0, forward.z = 0,
    right.x = 0, right.z = 0, right.y = -1.0, the position zeroed), the
    three frees via BPathData__destructor :2506 + operator_delete_
    internal, the lastMoveTimeValue/mHasSubstitutePosition/
    mConsecutiveSubstituteCount zeros. GATE CATCH: continueCounter is
    an INT field (the C4244 on the 0.0f) - corrected to = 0. OBSystem
    GROUNDING RESOLVED (the owner's pushback - the absence claim was
    WRONG, the definition-pattern search was never run): OBSystem_Land/
    Air are TYPED - RGE_Obstruction_Manager** / RGE_Airspace_Manager**
    (common.h:109-110); RGE_Obstruction_Manager DEFINED at idb_types.h
    :5820 (168 bytes): vfptr @+0x0 (the ONLY vtbl pointer - the
    intermediate "two vfptrs" claim was MY WALK BUG: the stepper
    advanced 4 bytes per LINE incl. braces/comments), Active @+0x4,
    World (RGE_Game_World*) @+0x8, Width (int) @+0xC, Height (int)
    @+0x10, mfwidth/mfheight, HardObArrayData/HardObArray, NodeLevel
    Index[32], the ignore-list block, BufferRadius, mnplayernum; the
    Airspace twin @:5868 (slot-identical). THE .C'S v5[3]/v5[4] = the
    object base +0xC/+0x10 = Width/Height EXACTLY (v5 = &vfptr = the
    object base). The passableTile pair UNBLOCKED for their dedicated
    decode (the OBSystem domain select + the Width/Height gates typed).
    ENUMERATION-SCRIPT CORRECTION: the member-walk regex missed the two
    pointer-return declarations at 150/155 (the BVector*/BPath* returns
    break the \(\w[\w ]*\s+\* pattern) and shifted the printed indices
    by +2 - the phantom "slot 153 = getPatrolInformation"; the permissive
    per-member counter reconciles the struct with the real binary vftable
    data at every verified slot (150-158, +0x324, +0x78/+0x7C, +0x6C) -
    NO observable error in the vtable definitions; the corrected walker =
    the permissive per-member count with the name extraction secondary.
    GATE 0 errors 0 warnings x1 TU x86; defs/refs 65/65. Remaining Moving
    methods 31. Re-verify: the 3 defs + the 65 installer hooks; the
    OBSystem member form in passableTile2's decode.

  - SESSION CLOSE (2026-11-19): the working tree verified consistent -
    RGE_Moving_Object.cpp 65/65 gated 0/0 (the final gate pass), the
    state file repaired (the M8a entry repositioned after the M7 entry;
    the M6-era tail junction restored), x87_nan.h + the vcxproj entry +
    the stdafx include in place, the template carrying rule (i) + the
    probe protocol + the macro mandate + the logger process + the
    per-batch scan rule + the comment placement + the context rule.
    In-game: confirmed OK through M7 (the owner); M8a (straightProjected
    Position/calculateReachedPoint/initMovementData) + the NaN fixes
    AWAIT the owner's test build. THE PASSABLETILE PAIR (passableTile2
    @0x004A1B30 / passableTile @0x004A1C30) = the dedicated-decode
    queue HEAD: the OBSystem grounding RESOLVED (RGE_Obstruction_Manager
    .Width/Height @+0xC/+0x10, :5820/:5868); the REMAINING groundings:
    the runtime Static vtbl members 7/21/23 (the passableTile2 calls),
    the master word @+0x76, the world+0x60 table, the dword_6AD134
    global; passableTile's decode is asm-primary (the positive-sp
    render).

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. NOTE: .c/.lst were REGENERATED (the vtbl
    re-typing + the prototype corrections): the ledger's LINE citations
    are stale - cite addresses + function names; the .lst stack labels
    are reliable. State: the runtime construction ladder complete (41);
    the runtime methods through M8a: Moving installer 65 (remaining
    Moving 31), Animated 12/12, Doppleganger 9/9, Missile 16/16, Tree
    2/2 COMPLETE; Action 7/45, Combat 5/53, Static 6/136, TRIBE_Combat
    4/79, TRIBE_Building 6/76 remaining. The x87_nan.h macro set is
    MANDATORY for every unordered-inclusive compare (rule (i) + the
    probe table + the /Fa gate check); the semantic comments go ABOVE
    with trailing asm citations; the per-batch mechanical collision
    scan is mandatory (never inherit a verdict); an Information Request
    is a STOP CONDITION (resolve from the references first, then
    present). IMMEDIATE WORK: (1) M8b - the passableTile cluster
    dedicated decode: passableTile2 @0x004A1B30 + passableTile
    @0x004A1C30 (asm-primary: the positive-sp render) - the REMAINING
    groundings: the runtime Static vtbl members 7/21/23, the master
    word @+0x76, the world+0x60 table, the dword_6AD134 global (the
    OBSystem grounding DONE: RGE_Obstruction_Manager.Width/Height
    @+0xC/+0x10, the Airspace twin :5868); (2) M8c smallest-first:
    calculateReachedPoint/straightProjectedPosition/initMovementData
    DONE (M8a) - next: canPathWithAdditionalPassability, get_waypoint
    _checksum, addCurrentPath, passableTile pair (M8b), calculateReached
    Point done; (3) the capstone: doIntelligentMovementUpdate (5819
    bytes) with its bake inventory: checkAcceleration_patch @0x4A6569,
    isMoving_v2 @0x4A3D90 - speed4 already baked (M4). ALSO FLAGGED:
    the yaw/make_object_collision_list polarity pulls (the empty-window
    sites - verify the x87 test masks), the /Fa per-function sweep
    continuation, the LTG (/GL WholeProgramOptimization=true) probe gap
    (the NaN probe ran /O2 non-LTCG). PROTOCOL: rule (i) + the probe
    protocol + the macro mandate; the logger process available on
    request; the per-batch mechanical collision scan; the semantic
    comments above + the trailing citations; the approval gate before
    every write; an Information Request is a stop condition."

  - NaN SWEEP COMPLETE (owner-directed continuation of the (i) sweep,
    2026-11-18): FIXED 1 further divergent site - make_object_collision
    _list_new's radius gate (asm 0x49C04E-0x49C070: fld master radius;
    fcomp const_f_zero; test ah,41h; jnz loc_49C1A3 = the empty return
    - the taken set includes unordered, a NaN master radius returns the
    empty list in the original; the literal <= is unordered-false and
    would run the whole scan) -> the negated > form (!(radius_x >
    0.0f) || !(radius_y > 0.0f)). VERIFIED MATCHING: the dz band check
    (dz >= -radius_z && dz <= radius_z - the original's NaN flow skips
    at the first x87 check exactly as the literal form does; ordered
    deltas identical), yaw_new/setOrientation_new (no x87 compare
    branches in the bodies), plus the previously verified sites.
    FINAL SWEEP TALLY: 8 divergent sites fixed across the owned code
    (calculateYawAngle x3, the setter pair, find_evacuate_area,
    precalcDistToTarget, make_object_collision_list); all other owned
    x87 compare branches verified matching against the (i) polarity
    table. FLAGGED follow-ups: make_object_collision_list's world_z
    delta block (asm 0x49C124-0x49C148 - the fcompp/test ah,1;jnz +
    fcom/test ah,41h;jz two-radius structure decoded; the NaN delta
    skips in BOTH the original and the written form - MATCH verified);
    the yaw facet/angle calc tail (pure vector math, no branches -
    clean). GATE 0 errors 0 warnings (RGE_Missile_Object.cpp).

  - FLOAT ST0-BOUNDARY FIX APPLIED (owner-directed, staged plan executed
    2026-11-16; prompt items (1) and (2) of the NEXT SESSION block above
    are COMPLETE - item (3), the roadmap, is unchanged): the regenerated
    idb_functions.h verified - 86 double-returning defs, 0
    float-returning defs, 6,415 lines unchanged (in-place renames). All
    four prototype corrections confirmed in (moveUnit's 4th arg =
    BVector* corrected by the owner mid-session; moveUnit_new's
    signature matches). The 7 float-returning replacements converted to
    double-returning, each with a fresh asm pass: getSpeed (Animated,
    @0x00408B70) and getTurnRadius (@0x00408B80) = pure fld m32 exits -
    bit-identical; RGE_Missile_Object__handle_teleport_z_correction
    (@0x0049C1C0) = fld ds:flt_65BBE4 (.rdata:0065BBE4 = float32 -99.9,
    the written -99.9f kept deliberately - exact widening) + the
    non-ghost tail-call st0 passthrough 0x49C1DF-0x49C1E4;
    RGE_Moving_Object__maximumSpeed (@0x004A28C0) = the original's fmul
    retns leave st0 UNROUNDED (0x4A28DA-0x4A28E3 gate path,
    0x4A28E7-0x4A28ED pure load) - both (float) casts removed;
    calculateFutureVelocity (@0x004A3E00) = cap path jz 0x4A3EE6 falls
    through to retn 0x4A3F0C with the 53-bit cap unrounded (return cap;)
    while target STAYS float (fstp m32 @0x4A3E20, rule (e));
    precalcDistToTarget (@0x004A3F40) = ALL exits reload var_4 m32
    (0x4A3FE7/0x4A3FF1) - float result kept, the first distance now an
    explicit (float) citing fstp @0x4A3F82; precalcDistToGoal
    (@0x004A4000) = pure st0 passthrough (call distance_to_position
    0x4A4081; retn 0x4A408C, no store) - THE boundary site. The
    incrementWaypoint wd site: fstp var_28 @0x4A4265 immediately after
    the distance_to_position call, both consumers reload the m32
    (fld @0x4A4329 seed, fld @0x4A439F gate) - the float local is
    FAITHFUL, now an explicit (float) cast with the fstp citation.
    The other double-constant call sites (minDist/cd/d chains in
    incrementWaypoint/checkDistance/checkRange) already matched the
    discipline (chain in double, one store-round) - untouched.
    TEMPLATE corollary (h) WRITTEN (FLOAT PRECISION rules): plus the
    IDB RESIDUAL note - the runtime vtbl slot declarations in
    idb_types.h still type these slots float (no owned dispatchers
    today; slot re-typing requested for the next IDB pass). FULL-TREE
    RE-GATE: 0 errors / 28 TUs (26 engine TUs + stdafx.cpp + hunt.cpp)
    under x86 HostX64\x86 cl. Sweeps: return-type inventory 68 defs in
    the 3 touched TUs, 0 mismatches (1 documented UNKNOWN: unknown_new
    @0x00408BF0 - the IDA-named M2 accessor, absent from idb_functions.h
    per the M2 note); defs/refs Moving 42/42, Missile 16/16, Animated
    11/11 - NOTE: the session-close arithmetic's "Animated = 12" is
    STALE (the file is 11/11, matching the M1 entry's own write-time
    verification) - feeds the planned inventory re-count. Store-and-read
    diff: no store deltas (conversions only). Warnings: the 11
    pre-existing Static C4244s unchanged (alignment accumulators + ftol
    loop bounds); hunt.cpp:75 = the sole legacy-tree narrowing (legacy
    diagnostics TU, report-only); hunt.cpp:135 = pre-existing CRT sqrt.
    IN-GAME: the owner reports tests against the current tree (incl.
    this batch) OK. Re-verify: the 7 double signatures in the cpps;
    grep idb_functions.h '^float \(' (= 0); corollary (h) in
    DECOMP_TEMPLATE.md.

  - RGE_Moving_Object::angle_to_object DESYNC FIXED (owner-reported +
    owner-approved 2026-11-18): @0x004A1100 [0x4A1100,0x4A11A3). ROOT
    CAUSE: the shipped form dropped the shared-tail dy subtraction -
    fsub [ecx+4Ch] @0x4A1146 is the CONVERGED tail of BOTH delta paths
    (jmp 0x4A1139 / fall-through; the .c's atan2(v6 - this->world_y, v5)
    render has it) -> atan2 received the ABSOLUTE y. SECONDARY: the four
    constants are dd floats widened exactly by the x87 ops (const_f_2pi
    @0x65B388, flt_65B604 @0x65B604 = pi, flt_65C1AC @0x65C1AC = -pi,
    flt_65C1A8 @0x65C1A8 = 1/2pi); the bare double literals differed by
    2e-8..4e-8 (wrap-boundary windows + the facet scale) -> f-suffixed,
    /Fa pool-verified against the mechanically computed widened
    encodings (401921FB60000000 / 400921FB60000000 / C00921FB60000000 /
    3FC45F3060000000). TERTIARY (caught by the /Fa check, same approval
    round): the deltas now widen BEFORE the subtraction ((double) casts)
    - the x87 fsub computes at 53-bit and feeds fpatan unrounded; the
    bare float-float form compiled subss+cvtps2pd, a float rounding the
    original never performed (rule (a) mechanical /Fa signature).
    Gate: compile PASSED (v141_xp 14.16.27023, x86, /O2 /arch:SSE2
    /fp:fast; non-/GL per the known LTG gap), sweeps clean (bare CRT 0,
    bare base_game 0, pointer-walk int-only), /Fa jcc check passed (two
    X87_LT_NAN unordered-taken comisd+jae, one literal > MATCH row
    comisd+jbe, exit inventory 2x ret 12 = the asm's 2 retn 0Ch sites).
    atan2 -> __libm_sse2_atan2: <= 1-2 double-ulp vs fpatan, judged
    negligible for facet truncation. PENDING (NOT approved): amendments
    A-D for DECOMP_TEMPLATE.md - A REVISED per the owner ruling that the
    .lst CODE XREF annotations are INCOMPLETE and must not be relied on
    (the join inventory must count inbound edges from the jump
    instructions in the range itself, not from IDA's annotation lines);
    the systemic widen-before-op /Fa sweep (incrementWaypoint,
    calculateYawAngle, missile_move norm chains, the doIntelligent
    MovementUpdate decode) awaits the amendment ruling.

  - RGE_Moving_Object::calculateReachedPoint DESYNC FIXED (owner-reported
    + owner-approved 2026-11-18), first M8a test feedback:
    @0x004A63F0 [0x4A63F0,0x4A64FE). (1) rule (j) - the three norm
    chains computed mulss/addss (24-bit per product/sum; len2/segLen
    rounded entirely before sqrtsd) vs the x87 53-bit chain - all
    operands now widen before the multiply; (2) rule (e) - len2 is
    fst'd to m32 @0x4A6498 (the caller's pos1 slot, scratch reuse) and
    the FINAL compare @0x4A64E1 consumes the ROUNDED value - the
    (float) len2_reached intermediate restores the store-round the
    transcription dropped (the .c's `v = v7; if (v6 >= v)` shows it).
    LAYOUT LESSON: BVector = {x, z, y} (idb_types.h:1761 - z @+4, y @+8;
    calibrated against operator_minus @0x606A60's write order) - an
    assumed standard {x,y,z} layout initially manufactured a phantom
    divergence; the .c render and the .lst labels AGREE under the true
    layout. Also fixed: the return-2 comment cited the return-0 site
    (0x4A64F7 -> 0x4A64EC), segLen's fcompp -> fcomp st(1), the range
    end 0x4A6510 -> 0x4A64FE, and segLen aligned to the asm's
    (z^2+y^2)+x^2 order (commutative, cosmetic). Gate: compile PASSED,
    /Fa rule-(j) signature PASS (cvtps2pd before every mulsd; the only
    cvtpd2ps is the intentional store-round), epsilon pool constant =
    widened 1e-7f verified (3E7AD7F2A0000000), sweeps clean. OPEN:
    straightProjectedPosition (same M8a batch) owes the same
    rule-(j)/(e) audit.

  - AMENDMENTS A-D APPLIED (owner-approved 2026-11-18):
    DECOMP_TEMPLATE.md :166 (C, dd-constant widening), :393 (D, rule (j)
    widen-before-op + the owed retroactive sweep), :669 (A,
    join-convergence inventory - join discovery from the jump
    instructions in the range, NOT the CODE XREF annotations - the
    owner ruling that they are incomplete is recorded in the rule),
    :688 (B, total-instruction citation coverage); plus the STATE-FILE
    SESSION CONVENTION (:127) - NEXT SESSION blocks are closed records,
    a new one is appended at session end. Line numbers as-inserted
    2026-11-18. The A-D retroactive sweep (D's owed list) remains open
    work.

  - RGE_Moving_Object::straightProjectedPosition AUDITED + 2 FIXES
    (owner-approved 2026-11-18, the rule-(j)/(e) audit owed by the
    calculateReachedPoint entry): @0x004A6860 [0x4A6860,0x4A697C).
    rule-(j)/(e) CLEAN (the operator_mul inlines are float-exact
    products; the cap store-round and the speed-x-seconds product
    store-round were already replicated). FIX 1 (aliasing): the advance
    multiplies now target the asm's sret LOCAL (-0xC) instead of the
    caller's *result - the direct form clobbers *result before
    operator_plus reads positionValue when a caller aliases
    result = &positionValue (no direct .c callers - vtable dispatch
    only - so the asm's safe shape is mandatory); emitted form verified
    (multiplies into _displacement$, one [edi] write at the tail).
    FIX 2 (clamp polarity): the composite `!(v > 0.0f && v <= cap)`
    nested the cap compare into the keep condition - on a NaN cap with
    ordered v > 0 the asm's gate 2 (v vs 0.0 only, jz @0x4A6927) KEEPS
    the speed while the nested form clamped to NaN - rewritten as the
    cap-first chain `v > cap || !(v > 0.0f)`; /Fa-verified case table
    incl. the NaN-cap row. Also: range comment 0x4A6980 -> 0x4A697C.
    CLAMP-CHAIN LESSON logged for the doIntelligentMovementUpdate
    decode: when the asm's gates are a CHAIN, the C composite must
    preserve the gate ORDER and each gate's private operand set - never
    fold a later gate's operand into an earlier keep condition.

  - M8b COMPLETE: the passableTile cluster (owner-approved + written
    2026-11-18): passableTile2 @0x004A1B30 [0x4A1B30,0x4A1C29) and
    passableTile @0x004A1C30 [0x4A1C30,0x4A1D42) - 2 new hooks (the
    Moving installer now 67). GROUNDINGS RESOLVED: the OBSystem vtbl
    slots (idb_types.h:9943/10146 - GetTerrainMask idx 7 @0x1C,
    CheckForCollisions idx 19 @0x4C, SetIgnoreList idx 21 @0x54,
    SetBufferRadius idx 23 @0x5C - the Airspace twin slot-identical);
    the master word @+0x76 = terrain (i16 - the census-calibrated walk:
    elevation_flag@116, fog_flag@117, terrain@118, movement_type@120 =
    the registry's 0x78; 19 census hits en route); world->terrains
    @+0x60 (the RGE_Game_World walk); THE SCRATCH GLOBALS ADDED
    (owner-approved, names by proposal): OBSystem_Ignore_Scratch_Tile2 =
    0x006AD134 / OBSystem_Ignore_Scratch_Tile = 0x006AD0AC (common.h/
    common.cpp) - LOAD-BEARING: SetIgnoreList stores the ids POINTER
    (the manager's IgnoreIDList @+0x9C) and neither function ever
    disarms, so a stack local would dangle into later queries. OWNER
    CLARIFICATION in force: approving these two is NOT a license to
    auto-add missing globals - every future missing symbol comes to the
    owner as its own approval. PASSABLETILE'S "positive sp" .C FLAG
    RESOLVED: IDA frame confusion only - the raw esp arithmetic maps
    every read (0x4A1CC6 = ignoreUnits, 0x4A1CCE = numberIgnoreUnits,
    0x4A1CF4 = y, 0x4A1CFA = x), asm-primary per the standing ruling.
    STRUCTURE: passableTile = passableTile2's ladder + the CONDITIONAL
    ignore list (the caller's array when populated, else the scratch
    fallback) - the asm's 0x4A1CED 2-inbound join preserved in the
    written if/else (amendment A). The sideways cast
    RGE_Airspace_Manager* -> RGE_Obstruction_Manager* is layout-proven
    (the twin structs member-identical, the twin vtbls slot-identical).
    Query constants 32/25/-2 -> the magic-constant registry (UNKNOWN;
    approved this round). Gate: compile PASSED, /Fa slot-offset
    verification PASS (28/92/84/76 = 0x1C/0x5C/0x54/0x4C; the field
    offsets 20/120/118/140/96/12/16/32/60/16 all match the census),
    sweeps clean, defs/refs 2/2 + 2/2, installer 67 hooks. IN-GAME
    VALIDATION OWED: unit movement over blocked/open tiles, flyer
    (movement_type 1) probes, the ignore-list behavior when callers
    pass lists.

  - M8c BATCH 1 (owner-approved + written 2026-11-18): canPathWith
    AdditionalPassability @0x004A21D0 [0x4A21D0,0x4A22B2),
    get_waypoint_checksum @0x4A3000 [0x4A3000,0x4A30E3), addCurrentPath
    @0x4A34B0 [0x4A34B0,0x4A35B2) - 3 new hooks (the Moving installer
    now 70). PROTOTYPE SUSPECT RESOLVED (canPathWithAdditional
    Passability): the original idb prototype was missing TWO parameters
    (retn 38h = 56 bytes vs 48 declared; the [E+0x28]/[E+0x2C] slots
    armed pathSystem.mAI/gbg_unknown1 while [E+0x30]/[E+0x34]/[E+0x38]
    fed findTilePath's p9/p10/p11 per the callee's own prototype,
    idb_functions.h:2543, corroborated by the callee-derived push
    annotations) - the owner corrected the IDB (the adopted names:
    mAI_arg/gbg_arg) and REGENERATED .c/.lst; the decode restarted from
    the regenerated files per the gate. pathSystem BOUND
    (owner-approved): PathingSystem* const pathSystem = 0x006AEBB0
    (common.h:119/common.cpp:24, direct-pointer style). STRUCTURE:
    canPathWithAdditionalPassability = the bounds gates (XYZPoint ints
    - idb_types.h:3923 - vs map_width/map_height @RGE_Map+8/+0xC), the
    five pathSystem arms, findTilePath (the ftol-pair starts), the
    bool-mask return, the five -1 disarms; get_waypoint_checksum = the
    linkedPath chain walk (BPathData @+0x118, linkedPath @+4, the
    cached path @+0x10 else RGE_Game_World__path) summing every
    waypoint's three BVector slots into a FLOAT accumulator (per-step
    fstp m32, rule (d)/(e) safe) + the ZERO-EXTENDED lastMoveTimeValue
    (@+0x124, the fild qword) + ftol - the compiled u32->double
    sign-bit idiom verified equivalent; addCurrentPath = the SEH
    allocation shape (the chunk 0x642820 documented; the written ctor
    call regenerates the unwind per the setVelocity precedent), the
    mFuturePathData reset, operator_new_internal(0x34) = 52 =
    sizeof(BPathData) (12 ints + BBitVector(int)) WALKED, the
    waypointLevel/range/targetID stamps, the
    setReadyToFollowPath/setReadyToPath routing on pathType. CAST FIX
    at write: RGE_Game_World__path takes RGE_Game_World* while
    owner->world is TRIBE_World* - the identical-prefix family cast
    (C-style, cited at the call), the base_game precedent. Gate:
    compile PASSED, /Fa slot-offset verification PASS (the pathSystem
    arms at the compiler-computed 845884-845900 = the IDB fields; the
    idb constants 0x4BD6A0/0x624610/0x4BC440/0x4BBE70/0x4BBD80/
    0x4BC080/0x4BBFE0 all match), sweeps clean, defs/refs 2/2 x3,
    installer 70 hooks. NOTE: the 2026-11-18 regeneration staled the
    ledger's .lst LINE citations again (addresses authoritative).
    IN-GAME VALIDATION OWED: pathing probes with terrain exceptions,
    the checksum's monotonicity, path installs of both pathTypes.

  - M8c BATCH 2 (owner-approved + written 2026-11-18): findFirstTerrain
    AlongExceptionPath @0x004A22C0 [0x4A22C0,0x4A2412), canLinePath
    @0x004A25B0 [0x4A25B0,0x4A270D), findAvoidancePath @0x004A2D80
    [0x4A2D80,0x4A2EDF) - 3 new hooks (the Moving installer now 73).
    findFirstTerrainAlongExceptionPath: the waypoint-pair walk (each
    waypoint fetched TWICE per component through BPath__waypoint into
    separate buffers), the firstTileAlongLine vtbl dispatch (idx 147
    @0x24C, idb_types.h:10960 - routes to the original until its own
    decode), the tile+0.5f center hit return, and the FAIL-PATH
    ASYMMETRY preserved: the mid-loop exhaustion returns 0 with
    rX/rY UNTOUCHED while the count<=1 and loop-exhausted paths write
    -1.0f first (the .c's split correct; verified through compilation).
    canLinePath: the per-step line walk (2*|(int)dominant-delta|
    float steps, the float cursor accumulators with per-step fstp m32
    stores, rule (d)/(e)), the new-tile test vs prev -999/-999, the
    distance chain all-double (rule (a)), and the RECIPROCAL-MULTIPLY
    FINDING: under /fp:fast MSVC compiled `(double)a / (double)t` into
    (1.0/t)*a (two roundings vs the x87 fdiv's one - caught in the
    first /Fa pass) -> TEMPLATE RULE (k) ADDED (the reciprocal /Fa
    scan + the float_control(precise) function bracket) and the
    FIRST USE applied: canLinePath_new bracketed
    float_control(precise, on, push)/pop - the re-gate shows divsd x2
    + __libm_sse2_sqrt_precise (bit-identical to sqrtsd; the CRT
    wrapper's errno branch unreachable for sums-of-squares inputs).
    PASSABLETILE_2 SLOT VERIFICATION: the canLinePath call site's
    vtbl idx 104 @0x1A0 = the passableTile_2 member (the RGE_Moving_
    ObjectVtbl member index walked) whose implementation in every
    runtime vtable initializer = passableTile2 @0x4A1B30 (our hook)
    - the idb-constant call form per the hook-routing ruling. IDB
    CORRECTION CANDIDATE noted: the vtbl member passableTile_2's
    declaration is 5-param (float, float, int*, int) vs the
    implementation's retn 8 (2 params) - every .c call site casts to
    2-arg; not blocking (the constant form used). findAvoidancePath:
    the zone-map chain (map_zones @RGE_Map+0xBF7C, the cached
    zoneMapIndex @+0x80 with the -1 gate + the
    RGE_Zone_Map_List__get_zone_map resolution (the return discarded,
    the out index kept), get_zone_map2, get_zone_info x2 (the u8
    promotions), the withinRange XYPoint-by-value cross-zone gate),
    operator_new(sizeof(BPath)) = 52 walked (13 dwords incl.
    BBitVector), BPath__BPath(-1), the findTilePath BOOL DISCARDED
    (the caller receives the path regardless). ALLOCATION-FORM RULING
    (owner-flagged): the WRITTEN FORM is sizeof(allocated type) - a
    verified literal with a walked comment is still a violation
    (addCurrentPath/findAvoidancePath both corrected; the template
    clarification applied). TEMPLATE AMENDMENTS this round: the
    compile-gate CONCRETE INVOCATION (the v141_xp toolset form with
    the /Fa emission requirement feeding rules (j)/(k)/C + the
    slot-offset checks) and rule (k). Gate: compile PASSED, /Fa
    verified (the Zone_Map constants 0x625E80/0x625FD0/0x625840/
    0x625860; the field offsets 128/24/140/52/94/118/96/49020; the
    BPath/BPathData offsets; the divsd forms), sweeps clean, defs/refs
    2/2 x3, installer 73 hooks. IN-GAME VALIDATION OWED: the
    exception-path terrain hits (the tile-center returns), line-path
    probes across passable/impassable tiles, avoidance paths within
    and across zones.

  - M8c BATCH 3 + SESSION CLOSEOUT (owner-approved + written
    2026-11-18): canQuickPath @0x004A1E80 [0x4A1E80,0x4A1FE1),
    canLinePath2 @0x004A2420 [0x4A2420,0x4A25AD), addFuturePath
    @0x004A35C0 [0x4A35C0,0x4A375E), firstTileAlongLine @0x004A2710
    [0x4A2710,0x4A28B9), canPathWithObstructions @0x004A1FF0
    [0x4A1FF0,0x4A21C1), turn_towards @0x004A11B0 [0x4A11B0,0x4A13B2)
    - 6 new hooks (the Moving installer now 80). M8c 12/12 COMPLETE
    (the batch-1 three + findFirstTerrainAlongExceptionPath + these
    six = the full named set). canQuickPath: the target resolve (the
    TRIBE_World family cast), the LOCAL BPath via the RAW-STORAGE
    pattern (BPath's const member deletes the default ctor - the
    aligned raw storage + the placement pointer + the raw ctor call;
    EACH raw-storage application is owner-approval-gated per the new
    ruling), the MShortPather arming (SetIgnoreList with the
    ignoreList_3 global - ADDED to common per the owner approval,
    0x6ACE98), FindShortPath (the 14 args; the RAW FLOAT
    starts/targets - no truncation), the result gates (0 or 1 ->
    success). canLinePath2: the hoisted step divisions (rule (k)
    bracket - the .c's per-iteration render is source order), the
    blocking tile reported via rPoint (y-first, no ++i on the
    impassable exit), the shared ++i block (amendment A).
    addFuturePath: the trimmed world-registered copy (createPath +
    addWaypointAtStart/End + the world_time stamp @BPath+0x1C), the
    second BPathData (waypointLevel 2 + setReadyToFollowPath + the
    range/targetID stamps), the destroyPath2 on the second-allocation
    failure, the setLinkedPath(lP, 1) guard. firstTileAlongLine: the
    SIGNED PRE-ABS steps gate (a negative-dominant line walks nothing;
    the .c's v10 <= 0 render misstates it - asm wins), the cursor
    seeded ONE STEP BACK, the RGE_Map__get_terrain probe
    (mapcol/maprow; the __int16 params), the exact-1 passableTile2
    hit. canPathWithObstructions: the zone-map chain (the cached
    zoneMapIndex), the cross-zone withinRange gate, the caller's
    obstructions count pre-clear (ManagedArray_int_::numberValue @+4
    - idb_types.h:1521, the .c's own +1-dword render), the
    copyUnobstructibles on unobstructibleGroupID != -1. turn_towards:
    the dead band [-0.5, 0.5] (the literal > MATCH row + the
    X87_LT_NAN gate 2 - a NaN facet angle CONTINUES to the turn); the
    turn_speed == 0 gradual path: the computed delta = turn_speed *
    (dbl_65C1A0 / facets) = 0 - the angle re-stored UNCHANGED (the
    .c's v7 render misassigns the x87 slot - the asm's fmulp
    multiplies TURN_SPEED - asm wins; the wraps NaN-convergent); the
    turn_speed != 0 rate-limited path: the scaled_time = world_time *
    0.001 (the store-round, rule (e)), the elapsed gate, the
    facet-sign step direction, the wraps, the turn_towards_time
    re-stamp, the re-probe ({<, NaN} -> still turning). THE SIGNED-
    GATE DECODE FIX (canLinePath/canLinePath2): the asm gates on the
    SIGNED PRE-ABS (int)selected (test eax before the abs idiom; jle)
    - the written post-abs two_steps gate diverged for
    negative-dominant lines - corrected to steps <= 0 (a decode
    correction restoring the exact copy; the .c's v8/v10 <= 0 renders
    misstate the tested value). Gate: compile PASSED, /Fa verified
    per function (the constants 0x623DB0/0x4A8F70/0x4AAA20/0x4BC380/
    0x4BC400/0x4BC600/0x624640/0x624760/0x4BC490/0x4BC530/0x4BC100/
    0x4BBE70/0x4BBFE0/0x4BC080/0x4BD6A0/0x495B50/0x625E80/0x625FD0/
    0x625840/0x625860/0x4BEC70; the field offsets; the divsd forms
    under the float_control brackets; the dbl_65C1A0 literal pool
    qword 401921FB53C8D4F1 byte-exact - the owner-supplied bytes =
    the nearest double to the decimal 6.2831853), sweeps clean,
    defs/refs 2/2 x6, installer 79 hooks. IN-GAME VALIDATION OWED:
    the facing manager (the dead band, the rate limits, the wraps),
    the line walks (the blocking-tile reports), the quick paths, the
    checksums, the avoidance paths.

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the runtime Moving methods COMPLETE
    through the canPath/quick-path/turn cluster - the Moving installer
    now 79 hooks; M8c 12/12 COMPLETE. THIS SESSION: 3 desyncs fixed
    (angle_to_object, calculateReachedPoint, straightProjectedPosition
    - the ledger has the full entries), the passableTile pair + the
    M8c set written (12 functions), the template rules (j)/(k) + the
    dd-constant rule C + the raw-storage pattern (per-application
    approval) + the allocation WRITTEN-FORM sizeof ruling + the
    compile-gate concrete invocation + the SESSION-CONVENTION note
    applied, the passableTile vtbl member rename absorbed, the
    PROJECT GOAL recorded (full-binary replacement - the compile-flag
    arguments framed PER FUNCTION). PRIORITY 1 - CORRECT FIRST: the
    three passableTile2 probe sites written as DIRECT calls where the
    asm dispatches the vtbl slot 104 (0x1A0): canLinePath_new (asm
    0x4A26B7), canLinePath2_new (asm 0x4A2541), firstTileAlongLine_new
    (asm 0x4A2878) - convert to the named virtual dispatch.
    PRIORITY 2: the rule-(j)/(k) retroactive sweep
    (incrementWaypoint, calculateYawAngle, missile_move norm chains -
    the divisions + the 53-bit-consumed chains). PRIORITY 3: the
    capstone doIntelligentMovementUpdate (5819 bytes, @0x4A6800-
    region; the bake inventory: checkAcceleration_patch @0x4A6569,
    isMoving_v2 @0x4A3D90 already baked (M4); the decode applies the
    clamp-chain lesson + rules (j)/(k) + the /Fa checks). ALSO OWED:
    in-game validation for this session's functions (the 3 desync
    fixes + the passableTile pair + the M8c set); the LTG (/GL) probe
    gap (the NaN probe + the new rules ran non-LTCG); the remaining
    Moving methods (re-count from the exports vs the 79 hooks).
    PROTOCOL: the template rules (a)-(k) incl. the reciprocal-form
    scan + the float_control(precise) brackets per function; the
    dd-constant pool checks; the allocation WRITTEN-FORM sizeof; the
    raw-storage pattern per-application approval; the asm-primary
    rule for the .c/.lst conflicts; the vtbl member walks via the
    line-arithmetic method (the member line - the first-member line)."

  - P1 CONVERSION - THE PASSABLETILE2 PROBE SITES TO THE NAMED VIRTUAL
    DISPATCH (owner-directed CORRECT FIRST; the IDB correction absorbed;
    2026-11-19): canLinePath_new / canLinePath2_new / firstTileAlongLine_new
    now call this_->vfptr->passableTile2(this_, cursor_x, cursor_y) at the
    asm's three vtbl slot 104 (0x1A0) dispatches (asm 0x4A26B7 / 0x4A2541 /
    0x4A2878, two float args each). THE BLOCKER FOUND FIRST: the
    idb_types.h runtime-Vtbl member naming in the 101-106 window was
    displaced one slot late in ALL TEN structs (the phantom
    damageCapability_1 insert; struct passableTile@104 / passableTile2@105
    vs the binary passableTile@103 / passableTile2@104 /
    currentTargetID@105) - the clean `vfptr->passableTile2` form would
    have compiled +0x1A4 = the currentTargetID slot. GROUND: the Combat
    runtime initializer 0x657544 fully named through the window + the
    retn forms (passableTile2 retn 8 @0x4A1B30, passableTile retn 10h
    @0x4A1C30) + the push shapes at the +0x19C (4-arg) / +0x1A4 (0-arg)
    sites + eight known slot calibrations (29/44/57/81/85/147/153/170).
    The owner corrected the IDB + regenerated (passableTile2 @
    idb_types.h:10917, idx 104 = 0x1A0; rateOfFire/damageCapability
    aligned; currentTargetID2 surfaced). No slot-104 override exists in
    any runtime initializer - the conversion is asm-faithfulness, not an
    observable change. GATE 0 errors 0 warnings; /Fa per function:
    exactly one mov eax, [eax+416] + call eax; residual direct
    RGE_Moving_Object__passableTile2( calls 0; installer 79 hooks (the
    M8c Batch 3 entry's "now 80" was a header typo - 79 = the
    65->67->70->73->79 arithmetic); defs/refs 79/79 MATCH. Re-verify:
    idb_types.h 10916/10917 (line arithmetic vs first-member 10813);
    the /Fa +416 emission per function.

  - P2 COMPLETE - THE RULE-(j)/(k) RETROACTIVE SWEEP (owner-directed,
    2026-11-19): (1) incrementWaypoint_new - THE FLOAT-FINAL-STORE
    SHRINK FIXED: the obj == 0 seed chain terminates in a float store,
    which licenses the /fp:fast shrink - the emitted form was subss (a
    24-bit (wd - range)) + cvtps2pd + subsd 0.1, a float rounding the
    original never performs (asm 0x4A4323-0x4A4336: fld wd / fsub
    [eax+20h] / fsub flt_6545BC both at 53-bit, ONE fstp m32). Two
    interim forms FAILED to resist: the explicit (double) casts were
    already in the shipped line and still shrank; a double local got
    forwarded through by /Og. FIX: the float_control(precise, on,
    push)/pop function bracket (the rule-(k) mechanism - precise
    semantics removes the shrink license). Post-fix /Fa: ss-ops 0 in
    the whole function; the seed emits cvtps2pd x2 + subsd x2 + one
    cvtpd2ps; every other float form unchanged (the obj-seed chains,
    the d-loop double local, the comiss gates). The function has no
    divisions - no reciprocal-form risk introduced. PRECISION SCOPE
    comment records the evidence + the re-gate obligation (verify no
    subss in the seed block). (2) missile_move_new SWEPT - FAITHFUL:
    the 3 addss sites = the per-component position sums; the original
    store-rounds each sum via fstp m32 (0x49BF7F/8C/99) and reloads the
    rounded m32 for every compare (fld var_8 @0x49BF9D, fcomp var_8
    @0x49BFBD; same for ny/nz) - rule (d)/(e) exactly, no fix.
    (3) FULL-TREE SHRINK SCAN (both /Fa listings, every PROC): 0
    widen-after-round sites post-fix. GATE 0 errors 0 warnings x86.
    Re-verify: the incrementWaypoint PRECISION SCOPE comment + the /Fa
    seed form; the cvtps2pd-after-ss-op scan over both listings (= 0).

  - X87 MACRO-MANDATE SWEEP COMPLETE (owner-directed 2026-11-19, "NaN
    guards that do not use the macro"): 7 literal unordered-inclusive
    forms corrected across 2 files. RGE_Missile_Object.cpp
    make_object_collision_list_new: the radius gate's negated forms ->
    X87_LE_NAN(radius_x/radius_y, 0.0f) (the {<=, NaN} set = test
    ah,41h; jnz x2 -> the empty return). RGE_Moving_Object.cpp:
    straightProjectedPosition_new's clamp chain -> `v10.y > cap ||
    X87_LE_NAN(v10.y, 0.0f)` (gate 1 stays the literal > MATCH row -
    its jz targets the keep path); turn_towards_new x4: the
    turn_speed == 0 gate -> X87_EQ_NAN(turn_speed, 0.0f) (test ah,40h;
    jz -> gradual = the {== 0, NaN} set), the three {<, NaN} wrap
    gates -> X87_LT_NAN (new_angle @0x4A124B, the facet direction
    @0x4A12F4, the reloaded store @0x4A1323), and the wrap gate
    `!(angle <= 6.2831855f)` -> THE LITERAL > MATCH ROW (asm
    0x4A1344-0x4A134F: test ah,41h; jnz -> the no-wrap SKIP - the
    sanctioned literal form; the negated form had taken NaN into the
    wrap: value-convergent (NaN - 2pi = NaN) but jcc-divergent from
    the asm). /Fa CERTIFICATION per the mandate: every expansion emits
    comiss/comisd + direct CF-reading jcc (ja/jae/jbe), NO setcc
    folding; each gate's taken-set verified with operand orders +
    targets (the EQ gate = two reversed comiss/ja, fall-through
    {ts == 0, NaN}; the LT_NAN gates = jae -> skip, the wrap on
    fall-through CF=1; the straightProjectedPosition chain preserves
    the exact keep/clamp sets incl. the NaN-cap row). RESIDUAL: 0
    literal negated-compare forms in code (the only grep hits left are
    historical quotes in comments, refreshed); 39 macro call sites
    tree-wide. GATE 0 errors 0 warnings x2 TUs; defs/refs 79/79 Moving
    + 16/16 Missile MATCH; bare CRT 0; bare base_game 0. Re-verify:
    grep the engine tree for `!(x <op> num)` code forms (= 0); the /Fa
    jcc forms per macro site.

  - TEMPLATE AMENDMENTS - THE VTABLE DISPATCH RULES (owner-directed
    2026-11-19, written): (1) VIRTUAL-DISPATCH FIDELITY - virtual
    sites (the vfptr load + `call dword ptr [reg+disp]` shape) MUST
    use the named virtual dispatch `obj->vfptr->member(args)` through
    the matching Vtbl struct member; NEVER a direct idb-constant/_new
    call at that site; the hook-routing ruling scoped to asm-direct-
    call sites only (the base-class-call case in derived
    replacements); the member = the line-arithmetic index (member
    line - first-member line) = the slot index; the /Fa gate check
    verifies [reg+slot_offset] per function; converting a virtual
    site to a direct call = a gate violation (origin case: the
    passableTile2 probe conversion). (2) CAST-FORM VTABLE DISPATCH
    FORBIDDEN - the IDA .c rendering's
    `((ret (__thiscall *)(T *, args...))obj->vfptr->member)(...)` form
    is explicitly forbidden even when it compiles with the right slot
    offset; a member-declaration/call-shape mismatch = wrong member
    naming = a type-definition suspicion = HARD STOP -> owner IDB
    correction + regen (origin case: the displaced
    passableTile/passableTile2 naming in all ten runtime Vtbl structs -
    the cast-form interim was probed, emitted the right offset, and
    REJECTED). Both rules in the Cleanup rules section after the
    cross-class base calls bullet.

  - DESYNC FIX #7 - turn_towards_new: THREE divergences (owner bisect +
    root-caused via the strict instruction-level re-walk of
    [0x4A11B0,0x4A13B2) + approved + fixed 2026-11-19; the function's
    FIRST in-game test - the M8c validation had been owed). ALL THREE
    predate the macro-mandate sweep (today's macro edits certified
    behavior-neutral; the :3747 macro conversion incidentally FIXED the
    gradual-path NaN-wrap to match the original). (1) THE GRADUAL-PATH
    OPERAND (turn_speed == 0 units): the x87 stack walk proves the gate's
    fcomp (0x4A121C) POPS turn_speed - the facet angle SURVIVES in st(1)
    (it entered in st0 from angle_to_object; both dead-band compares were
    fcom, no pop; the dead-band return's fstp st pops it) - so the
    `fmulp st(1), st` (0x4A1237) multiplies THE ANGLE: new = facet_angle *
    (2pi/facets) + this->angle = the target absolute angle (the
    instant-face snap). The shipped form computed turn_speed * (...) = 0 +
    this->angle ("re-stored UNCHANGED"). The M8c-era "fmulp multiplies
    TURN_SPEED - asm wins" ruling was a wrong x87 stack model; the .c's
    v7 render was right. FIX: the product operand -> `angle`
    (/Fa: mulsd xmm1(the quotient), xmm2(the angle)). (2) THE
    RATE-LIMITED NEGATIVE PATH (units turning counter-clockwise):
    asm 0x4A130F `fsubr [esi+0C8h]` = st0 = THIS->ANGLE - facet_step (the
    stored radians stepped); the shipped form computed the FACET-UNITS
    local minus the radian step and STORED it as this->angle - unit
    mixing, garbage facing every negative-direction window. FIX: `angle =
    (double)this_->angle - facet_step`. (The sign test at 0x4A12F8 is
    `fcomp` - POPS the facet angle; the x87 stack balances exactly: the
    elapsed-gate jnz loc_4A13A7 shares the `fstp st` that pops the
    stranded angle.) (3) THE ELAPSED CHAIN ROUNDING: asm 0x4A12DD `fst`
    stores the scaled_time COPY while st0 stays UNROUNDED, and 0x4A12E1
    `fsub [esi+0CCh]` consumes the UNROUNDED st0 - the shipped form
    computed elapsed FROM THE ROUNDED float local (a rule-(e)
    misapplication: the rounded copy is consumed ONLY by the re-stamp
    0x4A136D-72, which the shipped form did correctly). FIX: scaled_time
    as an UNROUNDED double for the elapsed chain + a scaled_time_stored
    float for the re-stamp. EVERYTHING ELSE VERIFIED FAITHFUL in the
    re-walk: the entry guards, the dead band (both fcom/no-pop; the
    shared fstp st), the gradual/rate-limited gate sets (incl. the macro
    forms), both wrap blocks (compare/wrap the correct values per path -
    the gradual wraps on the UNROUNDED st0 per fcom-before-fst, the
    rate-limited wraps on the RELOADED m32), the set_angle dispatch
    (+0x378), the re-probe, all four exit paths, the full x87 stack
    balance. GATE 0 errors 0 warnings x86; /Fa: divsd x2 (no reciprocal),
    mulsd/subsd on the correct operands, 2 ss-ops = the ratified rule-(d)
    float wrap forms, shrink-signature 0; defs/refs 79/79 MATCH; bare CRT
    0; bare base_game 0. LESSON (rule (j)/(k) class): x87 stack-model
    errors survive every text-level net - the operand-role verification
    (which value each x87 op actually consumed) is only decidable by
    tracking the stack slot-by-slot across the whole function; add the
    per-slot stack audit to the capstone decode protocol. Re-verify: the
    gradual product sources the angle; the negative path steps
    this->angle; the elapsed subsd has no cvtpd2ps upstream; grep the
    PRECISION SCOPE comments.

  - IN-GAME VALIDATION (owner-reported 2026-11-19): the test run
    against the current tree - ALL OK. The tested tree carried: DESYNC
    FIX #7 (turn_towards: the gradual operand, the negative-path
    operand, the elapsed chain), DESYNC FIX #8 (the three line-walk
    steps gates -> two_steps == 0), the P1 passableTile2 named-dispatch
    conversion, the incrementWaypoint float_control bracket, and the
    x87 macro-mandate sweep corrections. Closes the owed in-game
    validation for those items and the previously-owed M8-era items the
    run covered (owner-reported as given).

  - SESSION CLOSE (2026-11-19): THE PATHING SUBSYSTEM RATIFIED
    (src/engine/pathing/) - the B*/pathing family home (BPath, BPathData,
    BMotion, PathingSystem, MShortPather, RGE_Map_Analysis_System, the
    primitives BVector/BMatrix/BPosition/BBitVector). THE OWNED GLOBALS
    (common.h/cpp, the owner-directed "completely owned" form - the
    definitions, NOT bound to the game's .data slots): ignoreUnits[128]
    (the game's 0x6ACEA4), collisionUnits[256] (the game's 0x6AD170),
    ignoreListSize (the game's 0x6AD578) - THE RENAME HYPOTHESIS
    VERIFIED: the .c's `collisionUnits` = OUR ignoreUnits (the .c's
    `unk_6AD170` = OUR collisionUnits; IDA's own param labels agree:
    `push offset collisionUnits ; ignoreList` @0x4A509C, `push offset
    unk_6AD170 ; collisionUnits` @0x4A4E50). THE REFERENCE CLOSURE
    VERIFIED (the .lst global-access scan, the param-name collisions
    excluded): exactly four referencing functions - doIntelligentMovement
    Update (28 refs), BMotion::updateMovement (1: a push offset at
    0x49DEA9), addToIgnoreList (4), resetIgnoreList (1). THE SHIPPING
    CONSTRAINT: the globals fork vs the game's slots when the capstone
    ships - the four referencing functions must ship in the SAME build.
    THE CONFIG BINDINGS added (common.h/cpp): dword_689564/568/56C +
    flt_68F494 as the pointer-to-slot int*/float* const (the game's
    tuning block; the written reads deref). THE BATCH DECODE: the
    capstone doIntelligentMovementUpdate [0x4A47B0,0x4A5E6B) 5819 bytes
    + BMotion::updateMovement [0x49CB60,0x49E3FC) 2204 bytes +
    addToIgnoreList [0x4A0350,0x4A03B6) + resetIgnoreList
    [0x4A0340,0x4A034A) - ALL FOUR FULLY WALKED (the .c cross-checked;
    the per-slot x87 stack audit + the register-liveness proofs per the
    #7/#8 lessons). THE DRAFTING STATE: pathing/BMotion.cpp WRITTEN
    (~890 lines): resetIgnoreList_new/addToIgnoreList_new COMPLETE;
    BMotion__updateMovement_new complete THROUGH the OBSystem arm (the
    entry/pause gates, the tries for-loop, the increment/patrol arm with
    both distance-chain variants, the state dispatch + the linkedPath
    arms, the state-2 repath with the four building arms - the
    map-analysis, the group/solo findLowLevelPath, the OBSystem probe,
    findTilePath - the install/failure arms, the movement projection,
    the OBSystem pass with resetIgnoreList/addToIgnoreList/
    findObSystemCollisions/the slim processing loop). THE FACING ARM
    (.c 171192-171230, asm 0x49E20B-0x49E3DF) = THE RE-TYPED PSEUDOCODE
    REQUEST PENDING (the protocol's interacting-slot class): (1) the
    normalization slot identities (the .c's v.z/v165.x/v166.x family vs
    the true members), (2) the angleBetweenVector operand slots, (3) the
    operator_mul cross-product result/vector identities + which member
    the v179.y > 0.0 sign test reads, (4) the BMatrix block's
    orientation-member references. THE FILE DOES NOT COMPILE YET (the
    facing arm's ... fragments remain; the gate re-run owed). THE
    CAPSTONE: the Sections 1-5 drafted in-conversation (the entry/
    increment/state dispatch/repath arms/install); the movement/OBSystem/
    tail arm + the final assembly PENDING. THE WRITTEN-FORM LESSONS
    BANKED: the chunked writes mangle brace nesting - the assembly edits
    must anchor on UNIQUE multi-line contexts and the brace balance must
    be verified per chunk (the compile per chunk, not per batch); the
    .c's immediately-nested while(1) family = ONE loop (the breaks
    between the nestings = continues); the IDA var component labels
    (.x/.z/.y) reflect the VAR'S OWN base, not the actual buffer's base
    (the buffer-at-+4 case shifts every label). THE NEW-GLOBAL APPROVALS
    STILL OPEN: collisionUnits/ignoreListSize/unk_6AD170 (approved as the
    owned trio above) + dword_689564/568/56C + flt_68F494 (the bindings
    written; the derefs written). Re-verify: common.h/cpp the owned
    block; BMotion.cpp the facing-arm placeholder.

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the pathing subsystem ratified
    (src/engine/pathing/); the owned globals (ignoreUnits/collisionUnits/
    ignoreListSize + the dword_689564/568/56C + flt_68F494 config
    bindings) written in common.h/cpp; the four-function pathing batch
    DECODED AND DRAFTED: pathing/BMotion.cpp written (~890 lines -
    resetIgnoreList_new/addToIgnoreList_new complete; BMotion__update
    Movement_new complete through the OBSystem arm), the capstone
    doIntelligentMovementUpdate Sections 1-5 drafted in-conversation AND
    the assembled capstone draft SAVED to
    cheatdll/src/idb/WIP_doIntelligentMovementUpdate.txt (the entry through
    the state-2 repath's findLowLevelPath arm - the .txt staging file, not
    compiled, not in the vcxproj; the movement/OBSystem/tail arm + the
    flagged-arm transcription slots marked PENDING inside it).
    IMMEDIATE WORK: (1) complete BMotion__updateMovement_new's FACING
    ARM (the .c 171192-171230, asm 0x49E20B-0x49E3DF) - the re-typed
    pseudocode request for the region is OPEN (the normalization slot
    identities, the angleBetweenVector operands, the operator_mul
    cross-product identities, the v179.y sign test) - the owner provides
    the re-typed dump or approves the scripted x87/memory slot-tracker
    walk; the file does NOT compile yet (the ... fragments at the
    facing arm + the casts/globals fixes applied but unverified).
    (2) assemble the CAPSTONE doIntelligentMovementUpdate_new into
    object/RGE_Moving_Object.cpp (Sections 1-5 in-conversation + the
    movement/OBSystem/tail arm - the same facing-region caveats apply to
    its OBSystem arm's ignore-build sites); the BBitVectorAND_2 gates ->
    & 1 (the .rdata verified); the installer hook 0x004A47B0.
    (3) wire install_pathing/install_BMotion into install_engine; the
    vcxproj entry for pathing/BMotion.cpp. (4) the compile gate both TUs
    + common.cpp; the /Fa checks (the slot offsets, the jcc forms, the
    rule-(j) signature, divsd). (5) the full sweeps: the defs/refs, the
    join table, the exit paths (the capstone 13 + BMotion ~12), the
    store-and-read diff with the census (BPathData/BPath/BMotion/
    BPosition/RGE_Moving_Object/RGE_Game_World fields), the citation
    coverage. (6) THE SHIP CONSTRAINT: the four functions in ONE build
    (the owned globals fork vs the game's .data slots). (7) the in-game
    batch test. ALSO OPEN: the Moving methods re-count, the LTG (/GL)
    probe gap, the state 4/5 checkDistance arm's loc_49D033 verify, the
    label110 two-stage validity check's asm verify. PROTOCOL ADDITIONS
    THIS SESSION: the vtable-dispatch fidelity rule + the cast-form
    vtable-dispatch prohibition (DECOMP_TEMPLATE.md); the x87 per-slot
    stack audit + the register-liveness proof requirement (the #7/#8
    lessons); the IDA var component labels reflect the VAR'S OWN base
    (the buffer-at-+4 shift case); the chunked-write brace-balance
    verification (the compile per chunk)."

  - DESYNC FIX #8 - THE LINE-WALK STEPS GATES (owner bisect on
    firstTileAlongLine + the same bug found in canLinePath/canLinePath2
    by direct asm reading; root-caused + approved + fixed 2026-11-19;
    all three functions' first in-game tests - the M8c validation had
    been owed). THE BUG: all three written gates were `steps <= 0` (the
    SIGNED PRE-ABS ftol value) per the M8c Batch 3 "SIGNED-GATE DECODE
    FIX ... asm wins" ruling - but the asm tests the POST-ABS/SHL value
    in every one: firstTileAlongLine `test eax, eax` @0x4A27BE (eax =
    two_steps after the cdq/xor/sub/shl at 0x4A278F-0x4A2794, flags
    persisting across the fild/fxch/fdiv/movs to the `jle` @0x4A27FB ->
    return 0); canLinePath `test ebp, ebp` @0x4A2643 (ebp = two_steps;
    `jle` @0x4A2669 -> return 1); canLinePath2 `test eax, eax` @0x4A24C9
    (eax = two_steps; `jle` @0x4A24F1 -> return 1). two_steps = 2*|steps|
    is always >= 0, so the true gate fires ONLY when the dominant delta
    truncates to 0 - A NEGATIVE-DOMINANT LINE WALKS in the original
    (the step divisions carry the sign; the cursor walks backward from
    the seed). The shipped `steps <= 0` returned early on EVERY
    negative-dominant walk (roughly half of all line queries) ->
    desync. The M8c-era ruling was a REGISTER MISATTRIBUTION (the
    `mov ebp, eax` right after ftol, before the abs, made the decoder
    believe the later test saw the signed value); the .c's
    `v10/v8 <= 0` renders were right - the same M8c pattern as the
    turn_towards v7 case (DESYNC FIX #7). FIX: all three gates ->
    `if (two_steps == 0)` with the corrected comments (the return
    values unchanged: 1/1/0; semantically the gate equals the
    loop-natural zero-iteration exhaustion, kept in the asm's shape).
    EVERYTHING ELSE VERIFIED FAITHFUL in the firstTileAlongLine
    re-walk: the degenerate gate, the |dX| vs |dY| selection (fcompp
    C0), the cursor seeds (ONE STEP BACK - correctly distinct from
    canLinePath's at-s seeds), the float cursor arithmetic, the ftol
    pair on the rounded cursors, the new-tile test vs -999/-999, the
    prev updates only on new tiles, get_terrain(map, col, row) (ecx =
    the map; param1 = the last push = col), the tType1/tType2 u8
    match, the exact-1 passableTile2 gate (the P1 named dispatch
    included), the y-first rPoint store, all four exit paths, the x87
    stack balance. GATE 0 errors 0 warnings x86; /Fa: divsd x2 x3
    (the precise brackets), the gates emit test eax, eax + jle
    (canLinePath2's literally the asm's own shape), shrink-signature 0;
    residual `steps <= 0` = 0 tree-wide; defs/refs 79/79 MATCH. LEDGER
    CORRECTION: the M8c Batch 3 entry's "SIGNED-GATE DECODE FIX" claim
    is RETRACTED for all three functions (the pre-fix two_steps gates
    were CORRECT; the "signed pre-abs" "fix" introduced the bug).
    LESSON: an "asm wins" correction that OVERTURNS the .c must cite
    the exact test instruction AND prove which value its operand
    register holds at that address (the register-liveness walk) - a
    pattern match across sibling functions is not evidence (the three
    gates look identical but only the register CONTENT decides).
    Re-verify: the three `two_steps == 0` gates + their comments; grep
    `steps <= 0` (= 0); the /Fa test/jle shapes.

  - THE PATHING BATCH - BMOTION.CPP WRITTEN + DESYNC INVESTIGATION OPEN
    (2026-11-19, the continuation session): the four reference files read;
    the work products inspected (BMotion.cpp, the WIP capstone txt, the
    owned globals, the wiring state). THE FACING ARM (the prior session's
    open re-typed-pseudocode request) RESOLVED BY THE SCRIPTED FRAME-MAP
    WALK (owner-approved over the re-typed dump): the .lst frame
    definitions (v = BVector -24Ch, var_240 = BVector -240h, var_1B8 =
    BVector -1B8h, ...) make every label mechanical. RESULTS: (1) the
    normalization buffer = ONE 12-byte region at frame -0x248 (the .c's
    v.z/v.y/var_240.x labels = the IDA var-carving straddle - the
    buffer-at-+4 lesson; the [esi+14h] OF.z load is a DEAD store,
    overwritten by 0/norm); (2) angleBetweenVector(this = dirNew @-0x23C =
    the movement arm's loop-carried normalized direction (written
    0x49DC2C-0x49DC6D; every facing entry passes the movement arm first),
    v = dirCur @-0x248); (3) the cross product result @-0x1B4 (the .c's
    v179) whose sign-test slot (the IDA label var_1B8.y, frame -0x1B0) is
    the cross's .z; (4) the BMatrix block rotates mNewPosition
    (.mOrientationForward/.mOrientationRight about .mOrientationUp,
    +0x40/+0x4C/+0x58; mNewPosition @idb_types.h:20102), the two renorm
    operand orders DIFFERENT (the forward ((z^2+y^2)+x^2), the right
    ((x^2+z^2)+y^2)). THE GOTO ELIMINATION (owner-directed): all ~40
    gotos / 15 labels converted to the structured form (the
    fall-throughs, continue/break, one loop-exit phase code exitPhase (0 =
    the tries exhaustion -> return 0, 1 = the tail gate, 2 = the facing
    arm), the stage flags (skipRest/runArm3/runValidity/runFindTile) for
    the repath pipeline, the shared tails inlined at their sites); every
    converted site keeps its asm citation. THE WIRING: install_engine()
    -> install_pathing() -> install_BMotion() (the owner's chain);
    install_pathing.cpp created; the vcxproj/.filters entries.

  - DESYNC INVESTIGATION (the batch's first in-game test; STILL OPEN at
    session close; the fixes below are real divergences from the
    original, all fixed and gated):
    ROUND 1 - THE ST0-BOUNDARY CLASS x4 (the corollary-(h) violations;
    the /Fa shrink scan misses this class - the sites were float
    pipelines that never widen, so only the per-site operand-precision
    trace catches them; PROTOCOL: the trace is mandatory on EVERY
    st0-consuming call site): (A) the pause subtraction (0x49CBAF call ->
    0x49CBB4 fsubr -> 0x49CBBD fstp - the delta UNROUNDED; the (float)
    cast was a new rounding; the > 0.0 gate flipped a frame early/late);
    (B) the movement integration (0x49DC79 call -> 0x49DC7E fld st - the
    raw delta consumed TWICE; the float timeDelta2 store-rounded it);
    (C) the target gate (0x49E023 call -> fsub -> fabs -> fcomp, no
    store at all; the float freshDist pipeline rounded twice); (D) the
    patrol wrap's shared gate (loc_49CFAD fcom on the UNROUNDED st0 seed
    - the flags arm's fstp st merely discards) vs the NOT-patrol arm
    whose seeds fstp var_268 and whose gates re-load the rounded m32
    (the two arms have DIFFERENT precision models; the written single
    float gateDistance had unified them) -> the wrapSeed double split.
    ROUND 2 - THE LOGIC ERRORS (owner: "almost immediately, many test
    cases, ought to be a logic error"): (E) THE LABEL_110 PAIR: arm1's
    success exit (repathArmed && count > 2 -> LABEL_110) set skipRest
    without runValidity - the flow fell to the install gate UNVALIDATED;
    arm2's failure exit went unconditionally to arm 3, bypassing
    LABEL_110's repathArmed re-test (arm 1's success leftover routes to
    the validity check); both fixed. (F) THE RECORD-SCAN POINTER WALK
    (the owner caught it): the saved-and-incremented recordWalk -> the
    indexed ((ObsRecord**)obSystemCollisions)[i] + the named rec->Object
    (the missile precedent); the two-step unnamed chase was a MIS-LEVEL:
    the record's +8 member IS the blocker pointer (ObsRecord.Object,
    idb_types.h:4311) and [+0x10] through it is the blocker's own
    RGE_Static_Object::id @+0x10 (the emitted
    [eax+ecx*4]/[eax+8]/[eax+16] bit-identical). (G) THE ROOT CAUSE
    CANDIDATE - THE LINKED==0 GATE POLARITY (the decode bug from the
    original draft, carried through the rewrite): the asm 0x49D1A8
    (resolved == 0) jz loc_49D1B7 jumps INTO the continuesLeft gate; only
    a live patrol linked path skips it (0x49D1B5 ja loc_49D1CC); the
    written linked != 0 && form SKIPPED the gate for every pathData with
    no linked chain - the repath give-up mechanism (continuesLeft(this,
    1) = the REDUCE form, decrementing the counter installed at 10 by the
    install-success; <= 0 -> setFailure + reset + stop + return 0) NEVER
    FIRED - the units repathed forever. Fixed: linked == 0 ||
    (flags & 1) == 0 -> the gate.
    THE OWNER'S TEST CONFIG (the meaningful-isolation precedent): add and
    reset UNHOOKED for the test (the game's originals stay live for the
    game's DIMU; our BMotion__updateMovement_new calls the _new versions
    directly) - the two lanes self-consistent; DO NOT re-assert the
    3-of-4 owned-globals fork theory for this config (it was rejected);
    the full-ship constraint (all four in one build) still applies to the
    FINAL ship. THE OB CALL SITE: the asm 0x49DEA9 pushes offset
    ignoreUnits (0x6ACEA4) as the ignore list - NOT collisionUnits (the
    old .c's "collisionUnits" render was the pre-correction naming at
    0x6ACEA4 = our ignoreUnits per the rename verification; the owner had
    already fixed the argument themselves). collisionUnits has NO use in
    BMotion (its consumer = the capstone's DIMU decode; the common.h
    comment is provisional). THE OBSYSTEMCOLLISIONS CORRECTION
    (owner-supplied .data listing: "ObsRecord **obSystemCollisions; dd
    19h dup(?)" = 25 ObsRecord* entries at 0x6AD0C4): applied tree-wise -
    common.h/common.cpp -> ObsRecord** const; the local casts dropped in
    BMotion.cpp/RGE_Missile_Object.cpp/RGE_Moving_Object.cpp (the
    CheckForCollisions results-base args too); the emitted addressing
    unchanged ([eax+ecx*4]/[eax+esi*4]); the EXTRN decorations decode
    ObsRecord**. Registry corroboration: the 25 entries match the
    CheckForCollisions 25 argument (the capacity reading,
    layout-grounded). THE REGEN RE-WALK (the owner regenerated .c/.lst
    after the IDB correction; owner-directed meticulous re-walk): the .c
    walked statement-for-statement in 8 chunks - ZERO divergences; the
    record chase now renders (*v148)->Object / v92->id (the corrected
    typing validates the fix); the .lst extent + the frame definitions
    byte-identical; the data render corrected (dd 19h dup at :1100184);
    the coverage 686/686 re-verified.
    GATES: BMotion.cpp/common.cpp/install_pathing.cpp/install_engine.cpp
    0 errors 0 warnings x86; the /Fa: the slot offsets
    (+40/+140/+124/+780/+152/+8), divsd x12, no reciprocal forms, the
    macro jcc forms (comisd/comiss + the direct CF-reading jcc, no setcc
    folding), the widened pool constants (03fb99999a0000000 /
    03e7ad7f2a0000000), the shrink signature 0. THE SWEEPS: defs/refs
    3/3; CRT/base_game/pointer-walk 0; the exit-path inventory (10 asm
    epilogue joins mapped); the join table (64 labels); the
    store-and-read diff FULL MATCH; the citation coverage 686/686 AFTER
    the citation-typo families fixed (0x4A4C -> 0x49C x14, 0x4A4D ->
    0x49D x35, the type-2 wrap's wrong shared-exit address loc_49E1F4 ->
    loc_49E098).
    REMAINING OPEN: the desync itself (the static analysis exhausted - the
    full instruction-level walk found no further divergences); the next
    approach = the owner directs (the candidate: the logger A/B harness on
    BMotion__updateMovement_new per the template's diagnostic process).
    Owed: the in-game re-test of the current fix set (the linked==0
    polarity may close it); the capstone assembly (the WIP txt's
    movement/OBSystem/tail arm - NOTE: its OB call passes collisionUnits
    per the old .c render; VERIFY against the regenerated render - the asm
    0x4A5A06 pushes offset ignoreUnits); the ship constraint (the four
    functions in one build) for the FINAL ship; the common.h
    collisionUnits comment fix (provisional; needs the owner approval).

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the pathing batch WRITTEN AND GATED -
    BMotion.cpp (resetIgnoreList_new/addToIgnoreList_new/
    BMotion__updateMovement_new, the goto-free structured form),
    install_pathing.cpp (install_engine -> install_pathing ->
    install_BMotion), the vcxproj/filters entries, the owned globals in
    common.h/cpp (ignoreUnits[128]/collisionUnits[256]/ignoreListSize;
    obSystemCollisions corrected to ObsRecord** per the owner's .data
    listing). BMotion__updateMovement_new DESYNCS in-game - the
    investigation fixed real divergences but the desync itself is OPEN:
    fixed this session: the st0-boundary x4 (the pause/integration/target
    gate/wrap seed - the unrounded-consumption forms now emitted), the
    LABEL_110 pair (the arm1 exit missing runValidity; the arm2 failure
    bypassing the repathArmed re-test), the record-scan pointer walk ->
    the indexed ObsRecord** form + the chase mis-level (rec->Object; the
    blocker's own id), THE LINKED==0 GATE POLARITY (the repath give-up
    never fired: resolved==0 jumps INTO the continuesLeft(reduce) gate;
    the written linked!=0 form skipped it - fixed to
    linked==0||flag-clear). THE TEST CONFIG: add/reset UNHOOKED (our code
    calls the _new versions directly; the game's DIMU uses the originals)
    - the isolation test IS meaningful; the ignoreUnits arg at the OB
    call is owner-fixed (asm 0x49DEA9 pushes offset ignoreUnits; NOT
    collisionUnits). The full 2204-byte instruction-level walk is COMPLETE
    against the regenerated .c/.lst - zero static divergences remain; the
    static analysis is EXHAUSTED. THE NEW APPROACH (owner-directed): the
    leading candidate = the logger A/B harness (the template's diagnostic
    process) on BMotion__updateMovement_new: the _orig naked trampoline
    (replay the prologue past the E9 - the fs:0 SEH setup, the pushes,
    sub esp,25Ch - with the esp-adjusted operand reads; enter past the
    hook), the _logger wrapper capturing per tick (doAvailablePathCheck,
    uG, the mPathData state/waypoint/flags/range/pauseTime/
    continueCounter, the position + orientation, the return value, the
    mutated pathData), the installer swap, the vanilla-vs-hooked val_log
    diff localizing the first diverging tick/value - THEN the targeted
    asm re-walk of that region. ALSO OWED: the in-game re-test of the
    current fix set FIRST (the linked==0 polarity may close it - test
    BEFORE building the harness); the capstone assembly (the WIP txt's
    pending arm into object/RGE_Moving_Object.cpp + the hook 0x004A47B0;
    its OB call passes collisionUnits per the old render - VERIFY: the
    asm 0x4A5A06 pushes offset ignoreUnits); the ship constraint (the
    four functions in one build) for the FINAL ship; the common.h
    collisionUnits comment is provisional. PROTOCOL ADDITIONS THIS
    SESSION: the operand-precision trace is mandatory on EVERY
    st0-consuming call site (the /Fa shrink scan misses the
    float-pipeline form of the corollary-(h) class); the stage-flag
    conversions must map EVERY asm label arrival (the LABEL_110 re-test
    semantics); the citation-typos lesson (the 0x4A4C/0x4A4D families -
    the address sweeps verify comment addresses against instruction
    addresses and assume the citations themselves are correct); an
    argument-identity error (collisionUnits vs ignoreUnits) is invisible
    to every address sweep - only the callee's own consumption analysis
    (or the regen's re-typed render) catches it."

  - THE PATHING DESYNC HUNT - THE LOGGER HARNESS + THE FIRST DIVERGENCE
    LOCALIZATION (owner-directed 2026-11-20): BMotion::updateMovement extent
    CORRECTED to [0x49CB60,0x49E3FC) = 6300 bytes (the "2204" = a dropped hex
    digit). The STRICT-CONTROL-FLOW form written+gated (every L_ label = the
    asm label; no restructuring); the owner's IDA-cleaned transcription written
    as _ida + the float fixes: B1/B2/B3 (the three repath-region .z zeros +
    planar norms - the IDA carve-label trap), C1/C2 (the find_path/findTilePath
    AND 0FFh masks), D (arm-2/findTile stamps = currentPlayer->world->world_time,
    RGE_Player.world @+0x8C), the NaN macros at 6 branches, (__int64)->(int) x4.
    THE VTBL "displacement" FINDING RETRACTED (my probe off-by-one: the header
    member 195 = lastMoveTime @+0x30C verified by the owner). THE HARNESS: the
    updateMovement logger (the ids/characteristics per the owner's corrections
    - the pointers removed; wt; dt), the naked trampoline (replay = the 6-byte
    fs:0 mov; enter 0x49CB66 via edx), the ARM TRACE (ida_arm_log.txt: the
    state=2 triggers with v142/v139, the arm selections, the installs), and THE
    PATH-LIFECYCLE TRACE (createPath @0x624640 / destroyPath2 @0x624760 /
    path @0x624610 -> path_life_log.txt: id/wpc/len/ctm + the w0/w1 COORDINATES
    per event; the trampolines: 7B/5B/7B replays, the push/ret flags-preserving
    jump for path; 0 collisions x1065 sites). THE LOG FINDINGS: the replay runs
    aligned (wt 2044684 at row 1 both); rows 1-3 identical; ROW 1 of the FRESH
    capture diverged: the ORIG = the arrival -> state=4 -> the successor swap;
    the IDA = the arrival -> setPosition(wp[1]) -> state=2 -> THE REPATH -> the
    install (the fresh 2-wp 0.799 path) - THE RESOLVED WORLD PATH 180129's
    wp[1] DIFFERS between the peers -> the origin predates the first logged
    call (in the path-BUILDING era). uid=11723: the ORIG settled after 2 calls;
    the IDA looped 211 calls in state=2 (the identical arm-log gate values per
    tick, the ancient pid=60587 recycling) until the OOS abort at wt=2067728.
    RETRACTED: the "dt=0 vs 3.26" inference (the pos displacement = the
    increment arm's setPosition TELEPORT, not the integration - the conflation
    caught by the owner); the doFixedDeltaSeconds determinism-mode suggestion
    (THE REPLAY IS THE DETERMINISM MECHANISM - forcing the fixed delta breaks
    it; the installer writes reverted). THE TRAMPOLINE LESSON: the /Fa must
    verify the ESP-RELATIVE OPERAND OFFSETS, not just the coverage (two
    [esp+N] bugs caught: path_orig [esp+8]->[esp+4]; destroyPath2_orig
    [esp+4]->[esp+8] after the push). NEXT: archive the old logs; re-run the
    replay in both builds; diff path_life_log.txt - the first divergent
    resolve/create row (the w0/w1 coords) = the origin tick; cross-reference
    ida_arm_log.txt at that wt; the targeted asm walk of that gate. THE
    diagnostic CRT/SETF markers removed before the final ship.

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the pathing desync hunt - the A/B REPLAY
    HARNESS IS LIVE AND HOOKED (all hooks from WinMain). TWO updateMovement
    bodies exist and are gated: BMotion__updateMovement_new (the STRICT
    control-flow form, every L_ label = the asm label) and
    BMotion__updateMovement_new_ida (the owner's IDA-cleaned transcription
    with the float fixes applied). install_BMotion hooks the LOGGER
    (BMotion__updateMovement_logger), which dispatches to _orig or _ida per
    UM_TRACE_ORIG (1 = the vanilla trace -> val_log_orig.txt, 0 = the _ida
    trace -> val_log_ida.txt). ALSO HOOKED: the PATH-LIFECYCLE TRACE
    (createPath @0x624640 / destroyPath2 @0x624760 / path @0x624610 ->
    path_life_log.txt: every create/destroy/resolve with id/wpc/len/ctm and
    the FIRST TWO WAYPOINT COORDINATES w0/w1) and THE ARM TRACE
    (ida_arm_log.txt: move.arrive with the arrival norm, inc.pass/inc.repath
    with the gate distances v142/v139, patrol.wrap, st2.d2wp, arm1/arm2/
    findtile.enter, install.gate). THE PREVIOUS CAPTURE (UNTRIMMED, from
    WinMain) LOCALIZED THE DIVERGENCE: the replay runs aligned (wt 2044684
    row 1 both); rows 1-3 identical; ROW 4 (uid=12851) diverged - the ORIG =
    the arrival at the resolved path's waypoint -> state=4 -> the successor
    swap; the IDA = NO arrival -> the integration moved the unit 3.26 units
    (the huge first-tick delta) -> the OB pass found FRESH BLOCKERS -> the
    LABEL_194 reset+stop. CONSEQUENCE: uid=11723 (and others) got STUCK IN
    THE STATE-2 REPATH LOOP (211 updateMovement calls, the identical
    arm-log gate values repeating per tick, the ancient pid=60587 recycling)
    until the OOS abort at wt=2067728 (571 calls). THE TRAMPOLINE ESP-OFFSET
    LESSON (the /Fa must verify the operand offsets, not just the coverage):
    path_orig replays mov edx,[esp+4] + cmp edx,-1 (7 bytes) + the push/ret
    flags-preserving jump to 0x624617; destroyPath2_orig replays push esi +
    mov esi,[esp+8] (the +4 push shift) + jmp 0x624765; createPath_orig
    replays push -1 + jmp 0x624647. RETRACTED (do not revisit): the vtbl
    displacement finding (the probe off-by-one; the header member 195 =
    lastMoveTime @+0x30C verified by the owner); the dt inference (the pos
    displacement = the increment arm's setPosition teleport, not the
    integration); the doFixedDeltaSeconds determinism mode (THE REPLAY IS
    THE DETERMINISM MECHANISM - do not touch the delta globals). THE
    PRIMARY SUSPECT for the origin: the path-BUILDING divergence - one of
    the state-2 arm's builders (arm1 smooth via find_path /
    smoothMidLevelPath, arm2 findLowLevelPath, arm3 the OB probe, findTile)
    produced a different path in the ida build at or before wt=2044684 for
    uid=12851 - the B1/B2/B3/C1/C2/D fixes are IN; the residual is
    elsewhere in that arm. IMMEDIATE WORK: (1) the owner runs the SAME
    replay in both builds with the CURRENT tree; archive the old logs.
    (2) diff path_life_log.txt row-by-row (the shared sequence): the FIRST
    divergent create/destroy/resolve row = the origin tick; the w0/w1
    coords distinguish the content divergence from the allocation
    divergence. (3) cross-reference ida_arm_log.txt at that wt/uid -> the
    exact gate decision that flipped -> the targeted asm walk of that gate.
    (4) verify dt matches across the builds on every row (the replay feeds
    it; if dt diverges, the replay mechanism itself is broken, not our
    code). PROTOCOL: the replay is the determinism mechanism - never touch
    doFixedDeltaSeconds/fixedDeltaSeconds; the diagnostic CRT markers + the
    arm/path traces removed before the final ship; the strict form vs the
    _ida form: whichever passes the replay is the ship candidate. ALSO OWED:
    the capstone doIntelligentMovementUpdate assembly (the WIP txt's pending
    arm; the OB call passes ignoreUnits per 0x4A5A06); the common.h
    collisionUnits comment fix (provisional); the ship constraint (the four
    pathing functions in one build)."

  - THE PATHING DESYNC HUNT - CLOSED: THE _IDA VARIANT VERIFIED CORRECT
    ACROSS THE TEST SUITE (owner-reported 2026-11-20). Two TRANSCRIPTION
    bugs in BMotion__updateMovement_new_ida, localized via the A/B replay
    harness and fixed, all asm-cited in place:
    (1) THE STRAY GOTO in the walk arm (LABEL_155): an unconditional
    `goto LABEL_156;` sat on the not-at-waypoint edge. THE ASM:
    0x49DBC0-0x49DBCB (fcomp flt_6565F4; test ah,1; jz short loc_49DBF0)
    - norm >= 1e-7 JUMPS INTO the integration (loc_49DBF0); only
    norm < 1e-7/NaN falls into loc_49DBCD (setFlag(1,1); setFlag(0,1);
    jmp loc_49CBEB = the reloop). The stray goto armed mFlags 1|0 on
    EVERY walk call; the re-dispatch's increment arm then snap-advanced
    (the setPosition teleport @0x49CCC0) and at the last waypoint hit
    the SILENT state=4 route (v139 = 0-range-0.1 < 1e-7, no armlog) ->
    LABEL_194 reset+stop+1. THE TICK-1 SIGNATURE (all five rows):
    post pos = the exact waypoint[wp] bit-pattern, st=0/pid=-1/flg=0/
    vy=0, ret=1, plus the life-log destroy row (BPathData__reset's
    destroyPath2). The integration block was DEAD CODE (the OB pass
    consumed the uninitialized v179 integratedPos; move.arrive fired
    0 times in the whole run). Fix: the goto deleted + the asm comment.
    (2) THE MISSING DEREFERENCE at the walk-arm OB guard:
    `if (numberObSystemCollisions <= 0)` -> `*numberObSystemCollisions`
    (asm 0x49DEB8-0x49DEBF: cmp eax, ebx(0); jle loc_49DFFF - the VALUE
    test). The pointer test never fires -> every zero-count query ran
    the collision do-while ONCE over the STALE obSystemCollisions[0]
    (the previous unit's record) -> the phantom fresh blocker (path ctm
    2040578 < stale lastMoveTime ~2044684) -> v157 > 0 -> the successor
    swap (v98->state = 2; the linked BPathData carries waypointLevel=2
    -> st2.d2wp a2=2 with NO inc.pass) -> destroy 179655 + the repath
    180330 = THE ROW-4 DIVERGENCE (uid=12851, wt=2044684), every logged
    field explained. Rows 1-3 escaped by luck (their stale record was
    older than their path ctm).
    (3) THE SAME MISSING DEREFERENCE at the arm3 stub gate:
    `if (!numberObSystemCollisions)` -> `!*` (asm 0x49D7C8-0x49D7CF:
    jnz loc_49D83C) - the zero-collision stub block was DEAD CODE;
    visible in the arm log (wt=2044924: arm3.enter -> findtile.enter ->
    install.gate a1=0 a2=0).
    FIX C WITHDRAWN: the brace-level map proved the target-collision
    if-branch falls past while#2's body end = a body-end wrap = the
    RELOOP; only explicit breaks land at the next scope's tail (the
    target gate). The _ida's reloop semantics were ALREADY faithful
    (my concern was a misread of the IDA's synthetic nesting).
    RETRACTED THIS SESSION: the previous block's "PRIMARY SUSPECT: the
    path-BUILDING divergence" and the pre-window world-path-table
    divergence theory (the trace comment in BMotion.cpp) - the fresh
    capture's resolve rows are IDENTICAL (O491 == I491: path 180129,
    same w0/w1/len); the builders were never the origin.
    STALE COMMENT FOUND (comment-only fix owed): the STRICT form's "the
    free operator_mul is un-exported; the mul inlined" - the asm CALLS
    operator_mul at 0x49DC99 and BVector__operator_plus at 0x49DCA9;
    the export EXISTS (idb_functions.h:5784, operator_mul @0x606970);
    the _ida's call is faithful (the /Fa: mov eax, 606970h; call eax).
    VERIFICATIONS: the compile gate x2 runs 0 errors (x86, v141_xp);
    the /Fa checks - X87_LT_NAN emits comisd + jae-to-integration (the
    {<, NaN} fall = the armlog branch; NOTE jae is CF-only - verify the
    taken-set partition, not the jcc name), the pool __real@
    3e7ad7f2a0000000 = (double)1e-7f; the lastMoveTime dispatch emits
    [reg+780] (= +0x30C) at BOTH body sites (the owner's member-195
    verification re-confirmed); both fixed guards emit cmp DWORD PTR
    [eax], 0; the ignore-list _new functions re-walked faithful
    (resetIgnoreList @0x4A0340; addToIgnoreList @0x4A0350 incl. the
    mid-loop cap store-then-test order).
    STRUCTURAL GROUNDINGS (the _ida's six nested while(1)s = the asm's
    single dispatch loop; current line numbers, they rot with edits):
    #1@1511-2269, #2@1513-2255, #3@1515-2176, #4@1517-2113, #5@1519-2094,
    #6@1521-1759 - a body-end fall = the reloop (the nested chain
    re-enters the tries check); break#3 = the OB pass (loc_49DDF8);
    break#2 = the target gate (loc_49DFFF); break#1 = the range gate
    (L_49E151). THE MSVC /Fa LISTING WRITES OFFSETS DECIMAL in this TU
    ([ecx+1056] = +0x420; [eax+780] = +0x30C) - the offset scans must
    handle both forms. NEW TRANSCRIPTION-HAZARD CLASS: the pointer-to-
    slot globals (numberObSystemCollisions etc.) - every guard on such a
    global must dereference; the sweep = grep the global bare inside a
    condition. LIFE-LOG SEMANTICS banked: "resolve" = RGE_Game_World__
    path (the id->BPath lookup; a cached mPathData->path logs ONCE per
    cache miss); ctm = creationTimeValue @+0x1C; BPath = idValue@+8/
    waypointsCount@+0xC/pathLengthValue@+0x18/flags@+0x20; the ORIG's
    twelve-179639-lookup blocks per tick = the doIntelligentMovementUpdate
    pipeline (UNMODIFIED game code - it tracked identically once the
    in-call divergences were fixed).
    SHIP DECISION: the _ida form = the ship candidate (owner-verified).
    SHIP CHECKLIST: install_BMotion currently hooks the LOGGER + the
    three lifecycle-trace hooks - the flip = hook BMotion__
    updateMovement_new_ida directly (the commented line in the
    installer) + REMOVE the logger block, the lifecycle
    trampolines/loggers, and the BMotion_ida_armlog calls (the
    diagnostic CRT protocol); addToIgnoreList_new/resetIgnoreList_new
    stay unhooked (commented) unless the ship scope says otherwise.
    STILL OWED (carried): the capstone doIntelligentMovementUpdate
    decode - RESTARTING FRESH (the owner deleted
    WIP_doIntelligentMovementUpdate.txt; no WIP arm carries over - the
    decode runs anew under the two-pass protocol); its OB call passes
    ignoreUnits per 0x4A5A06 (the state-file lead - RE-VERIFY against
    the .lst); the common.h collisionUnits comment fix
    (provisional); the ship constraint (the four pathing functions in
    one build); the strict form's stale operator_mul comment.

  - THE UPDATEMOVEMENT SHIP - THE TWO-PASS DECODE ESTABLISHED (owner-
    directed 2026-11-20; all tests clean on the ship build): the _ida
    variant was RESTRUCTURED into the shipped form and renamed
    BMotion__updateMovement_new (the strict BMotion__updateMovement_new
    - the asm-label form - DELETED; the `_new` name transferred to the
    shipped variant; install_BMotion hooks 0x49CB60 ->
    BMotion__updateMovement_new directly; all instrumentation deleted -
    the updateMovement logger + UM_TRACE_ORIG trampoline, the armlog +
    its 10 call sites, the path-lifecycle loggers, the
    findTilePath/findLowLevelPath harness; zero diagnostic symbols in
    the /Fa; zero fopen/fprintf/sprintf/fwrite in the file). THE
    RESTRUCTURE DESYNCS, both caught by the paired-capture protocol and
    fixed: (1) ROW 1 - the R2 region replacement was one close brace
    short: the walk arm compiled INSIDE the state-2 stage - state-3
    units skipped the walk, the loop spun to the tries guard, ret=0
    (everything else in the row identical). LESSON: "0 errors" proves
    braces BALANCE, not that they close the right blocks - the
    brace-depth walk at every stage boundary is now a mandatory
    post-write bullet. (2) LINE 13603 - the scratch collapse: the
    restructure collapsed IDA's ~25 per-call-site BPath__waypoint
    out-buffers into ONE scratch, but BPath__waypoint COPIES the
    internal waypoint into the out-buffer and RETURNS THE OUT-BUFFER'S
    ADDRESS (0x4BC440) - the findTile tail check holds TWO fetched
    waypoints live (newTail + originalWaypoint); the second fetch
    overwrote the first, the discard compared 0.308 vs 0 instead of
    0.5, never fired, and a tile path the vanilla correctly rejected
    (its tail 0.5 from the intended waypoint vs the unit's remaining
    0.308) was installed. THE CLASS: copy-out getters make each fetch's
    result live in its buffer - collapse ONLY single-fetch sites; the
    arm1 smooth-tail check had the same defect (both fixed with a
    dedicated waypointScratch2; the findtile/findlow loggers pinpointed
    the mechanism: the orig's install gate saw wpc=0 = the discard
    fired, mine saw wpc=2). (3) THE FINDLOW LOGGER CORRUPTION - the
    trampoline byte-boundary slip: findLowLevelPath's prologue is
    mov eax,[esp+10h] (4 bytes) + sub esp,8 (3) + cmp eax,-1 (3); the
    jump target was entry+8 - ONE BYTE INTO the cmp - the original
    resumed at F8 FF = clc + a wild call through eax. Fixed to
    entry+7. LESSON: the jump target = the entry + THE REPLAYED BYTE
    LENGTH from the .lst instruction boundaries, never an eyeballed
    instruction count; verify the emitted replay in the /Fa. ALSO
    FOUND + FIXED during the hunt: (a) the missing deref at the
    walk-arm OB-pass guard (*numberObSystemCollisions <= 0 - the bare
    pointer never fires; the collision loop ran once over the stale
    obSystemCollisions[0] on zero-count queries - the phantom
    fresh-blocker successor swap, destroy 179655/repath 180330), (b)
    the same deref at the arm3 stub gate, (c) THE STRAY GOTO in the
    walk arm (the armlog signature: inc.pass a1=0/a2=-0.1 = the
    post-snap norm/gate; move.arrive firing 0 times; the integration
    block dead) - all three were TRANSCRIPTION bugs in the _ida form,
    fixed pre-restructure. THE MASKS: C1/C2 APPLIED to the shipped
    body (find_path & 0xFF literal - the u8 return reads al, so the
    mask folds in the /Fa, documented; findTilePath bool - the asm's
    and eax,0FFh keeps exactly AL = the bool read, so the direct bool
    read IS the mask, the literal & rejected as a C4806 no-op, the
    equivalence cited at the site). THE STRICT FORM'S stale
    operator_mul comment - RESOLVED BY DELETION (the form is gone; the
    export exists: operator_mul @0x606970, called at asm 0x49DC99).
    THE TEMPLATE FORMALIZED (owner-directed): the TWO-PASS DECODE
    section (the aligned transcription = the suite-verified artifact,
    no control-flow changes; the restructure built on top of it; the
    retirement at ship; the single-pass form stays for simpler
    functions) + the new bullets (COPY-OUT GETTER ALIASING, BRACE-DEPTH
    STAGE WALK, POINTER-TO-SLOT GLOBAL DEREF SWEEP, /Fa OFFSETS ARE
    DECIMAL, THE MASK-FOLD DOCUMENTATION, THE LOGGER ESCALATION LADDER
    + the trampoline byte-boundary + the fastcall liveness rule). THE
    ALIGNED TRANSCRIPTION lives in VCS at commit 3e0c753 (the diff
    reference for every later audit).

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: BMotion::updateMovement is SHIPPED
    - the restructured body renamed BMotion__updateMovement_new, hooked
    at 0x49CB60, suite-verified on the ship build; the strict form and
    all instrumentation deleted; the aligned transcription (the diff
    reference) is VCS commit 3e0c753; the two-pass decode + the new
    verification bullets are in DECOMP_TEMPLATE.md - READ THEM FIRST
    (the two-pass form applies to this session's target). TARGET:
    RGE_Moving_Object::doIntelligentMovementUpdate - THE CAPSTONE AND
    THE CALLER OF BMotion__updateMovement. ITS CALLS ROUTE THROUGH OUR
    HOOKED REPLACEMENT (0x49CB60) - the suite exercises our shipped
    body, and per the hook-routing ruling the decode's constant calls
    are faithful. THE DECODE RESTARTS ANEW: the old WIP
    (WIP_doIntelligentMovementUpdate.txt) was DELETED by the owner -
    no pending arm carries over; the decode runs the TWO-PASS PROTOCOL
    from scratch (Pass 1 = the aligned transcription straight from the
    fresh pseudocode/.lst, no control-flow changes, value-level fixes
    only; Pass 2 = the restructure). LEADS from the earlier session
    (state-file entries - LEADS ONLY, re-verify against the .lst
    before grounding): the OB call passes ignoreUnits per 0x4A5A06;
    the candidate hook 0x004A47B0 (RE-VERIFY the entry and extent from
    the .lst). THE
    FUNCTION IS THE FIRST APPLICANT OF THE TWO-PASS DECODE: Pass 1 =
    the aligned transcription (no control-flow changes, value-level
    fixes only), suite-verified against the vanilla trace; Pass 2 =
    the restructure on top; the aligned pass retires at ship. THE
    DETERMINISM PROTOCOL UNCHANGED: the paired capture is the
    determinism mechanism - never touch
    doFixedDeltaSeconds/fixedDeltaSeconds; dt must match on every row
    (if dt diverges, the replay mechanism is broken, not our code). THE
    UPDATEMOVEMENT LOGGERS ARE GONE (removed at ship) - if the capstone
    hunt needs them, re-add per the template harness protocol (the
    trampoline byte-boundary + fastcall liveness rules apply).
    findLowLevelPath/findTilePath are NOT owned - their calls from our
    body execute the originals (correct). BPath field groundings banked:
    idValue@+8, waypointsCount@+0xC, pathLengthValue@+0x18,
    creationTimeValue@+0x1C, flags@+0x20; BPath__waypoint copies into
    the out-buffer and RETURNS THE OUT-BUFFER'S ADDRESS (the
    multi-fetch aliasing rule: DECOMP_TEMPLATE.md). ALSO OWED (carried):
    the common.h collisionUnits comment fix (provisional); the ship
    constraint (the four pathing functions in one build) - the
    definition needs the owner's ruling (updateMovement is shipped;
    addToIgnoreList_new/resetIgnoreList_new are owned but unhooked -
    their only binary callers are inside updateMovement itself);
    findLowLevelPath/findTilePath are not owned. PROTOCOL: all edits
    approval-gated; the new-template bullets (the brace-depth stage
    walk, the deref sweep, the /Fa decimal offsets) are mandatory
    post-write checks."

  - THE CAPSTONE PASS 1 DESYNC — THE HARNESS INVESTIGATION (2026-11-20,
    ongoing; the state-2 arm's install-variant flip identified, the
    volatile v149 fix applied + in-game tested + DOES NOT FIX the
    desync, the root cause remains open):

    PASS 1 WRITTEN: RGE_Moving_Object__doIntelligentMovementUpdate_new
    @0x004A47B0 [0x4A47B0,0x4A5E6B) = 5819 bytes; installer hook 80
    (the Moving installer 80); gate 0/0; the post-write checks pass.
    The aligned transcription is the diff reference (the VCS commit
    preceding the volatile fix).

    THE DESYNC (owner bisect): uninstalling the hook eliminates it.
    THE HARNESS (all DIAG-marked, to be removed at ship): the DIMU
    logger (the IN/OUT/RET rows per call) + the PATH-LIFECYCLE TRACE
    (createPath/destroyPath2/path hooked at 0x624640/0x624760/0x624610)
    + the OBQ ARG TRACE (findObSystemCollisions hooked at 0x4A0110) +
    the FPHOOK ARG TRACE (find_path hooked at 0x4982F0) + the MAGET/
    MAPGET RETURN TRACE (get_map_analysis hooked at 0x49B470) + the
    A2GATE/PREINST/INSTALL-OK/STUBSET/SMOOTHGATE DECISION TRACE (the
    state-2 arm's build/install gates). 5 trampolines, all /Fa-verified
    byte-boundary-checked. The volatile v149 fix applied. The installer
    hooks the DIMU logger (the _new line commented in place).

    THE v3.3 CAPTURE FINDINGS:
    1. The OB queries: BIT-IDENTICAL args + results in both runs.
    2. The find_path calls: BIT-IDENTICAL args + results. The rp
       offset = the inherited counter shift.
    3. The get_map_analysis calls: the SAME ma_obj, the SAME NULL/
       non-NULL outcome. The raw POINTER bits differ (the heap-address
       artifact — not comparable; the MAGET/MAPGET rows now log the
       NULL flag 0/1 instead of the raw bits).
    4. THE DIVERGENCE: unit 13738's st=2 call at wt=020081A. IDENTICAL
       IN state. THE VANILLA: TWO creates (190709 + 190710), the
       second installed. MINE: ONE create (190709), the first
       installed. THE WORLD'S PATH-ID COUNTER OFFSET BY -1 → THE OOS.
    5. THE DECISION TRACE: A2GATE v143=1 wpc=2 cfg=1 v151=19.0 — ALL
       INPUTS CORRECT. PREINST v149=1 v143=1 — v149 IS 1 ✓. INSTALL-OK
       v149=0 variant=FOLLOW — v149 READS 0 ✗. NO ARM2-entered row
       between PREINST and INSTALL-OK. NO v149 write in the C++ source
       between PREINST and INSTALL-OK. THE COMPILER READS A STALE COPY.
    6. THE STUB SUB-BRANCH FIRED CORRECTLY (get_map_analysis=NULL →
       the addWaypointAtStart/End stub → v149=1, v143=1) IN BOTH RUNS.
    7. THE VANILLA'S OBSERVED TWO-CREATE BEHAVIOR: pass-1 → the stub
       sub-branch → v149=1 → the install: wl==2 && v149==1 → THE
       SETREADYTOPATH VARIANT (wl=1) → THE CONFIG-GATE RELOOP
       (dword_689564==1) → PASS-2: createPath(190710) → the smooth
       gate FAILED (wl(1)≠2) → arm 2 → findLowLevelPath → wpc=3 →
       the install → the followPath variant (wl(1)≠2 → TRUE →
       FOLLOW) → wl=0 → break → the walk arm → OBQ(n=0) → OUT(190710).
       MY BODY: ONE create (the followPath variant, no reloop).

    THE VOLATILE v149 FIX: applied, gated 0/0, IN-GAME TESTED —
    **DOES NOT FIX THE DESYNC** (owner-tested 2026-11-20). The v149
    stale-read is a SYMPTOM, not the root cause. Something upstream
    sets v149=0 (or the stub's v149=1 write is lost by a mechanism
    volatile cannot fix — e.g., the compiler's codegen restructured
    the if/else so the stub's store lands on a path not taken, or
    the opaque calls' side effects differ). THE ROOT CAUSE REMAINS
    OPEN.

    THE LESSONS:
    - The E9's 5th byte can land INSIDE the next instruction (the
      find_path trampoline: the fld is D9 44 24 28 = the disp8 form,
      4B not 7B; the resume address comes from the .lst's NEXT-
      INSTRUCTION address, not from a computed encoding length) —
      the owner's fix.
    - The owned globals fork between the runs — the ignSz field is
      NOT comparable (the pollution).
    - The volatile fix on a build-state flag does NOT fix a desync
      when the flag's value is itself a SYMPTOM of an upstream
      divergence. The volatile only fixes the compiler's stale-read;
      the underlying state difference persists.
    - The MAGET/MAPGET raw-pointer logging is meaningless between
      runs (the heap-address artifact) — log the NULL flag 0/1
      instead (the owner's correction).

    THE COLLISION SCAN: 0 active-writer overlaps inside the capstone
    range; the dead profiler table only. The pathing.cpp patches are
    NOT USED (the owner's standing ruling — no bake, no note, no
    further analysis).

    ALSO IN THIS SESSION:
    - The volatile fix on v149 (the install-variant flag) — DOES NOT
      FIX.
    - The STUBSET/PREINST/A2GATE/INSTALL-OK/SMOOTHGATE decision trace
      rows + the ARM2-entered row.
    - The MAGET/MAPGET/FPHOOK/OBQ/lifecycle trace rows (5 hooks + the
      DIMU logger).
    - The PATHING_CAP note (the never-baked ruling).
    - The ignSz field demoted (the fork pollution).
    - The v151=19.0/thr=5.0 smooth-gate values confirmed.
    - The dword_65BD9C = 4 (the flag-2 mask) verified.
    - The BPath field groundings re-verified against idb_types.h.
    - The gate: 0/0 (the latest build with all the DIAG rows).

    NEXT SESSION:
    (1) The desync root cause remains OPEN. The volatile v149 fix
        DOES NOT WORK. The next investigation candidates:
        (a) Log the incrementWaypoint call's inputs + outputs (the
            waypoint/step/state after each call) inside 13738's call —
            the increment arm drives the state=2 transition that
            triggers the second repath.
        (b) Log the state-2 arm's arm-2 findLowLevelPath result (the
            found/not-found + the path count + the length) — the
            vanilla's pass-2 arm-2 produced wpc=3; if my pass-2 (or
            the absence thereof) differs, the builder is the target.
        (c) Log the smooth-arm's find_path result per unit per tick —
            the vanilla's find_path succeeded for 13738's pass-1
            (r=1) but my body's find_path may return a different
            result (the map-analysis mip-level state may differ
            per-unit per-tick in ways the FPHOOK args don't capture).
    (2) Then: Pass 2 (the restructure) on the verified baseline.
    (3) The carried items: the common.h collisionUnits comment fix;
        the ship constraint (the four pathing functions in one build);
        the LTG (/GL) probe gap; the DIAG instrumentation removal
        before the final ship.

    PROTOCOL: all edits approval-gated; the DIAG instrumentation removal
    before the final ship; the braces-depth stage walk; the deref sweep;
    the /Fa decimal offsets; the mask-fold documentation; the store-and-
    read diff; the exit-path inventory; the join-convergence inventory;
    the total-instruction citation coverage.

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the capstone doIntelligentMovementUpdate
    Pass 1 is WRITTEN (gate 0/0, installer hook 80/80, the aligned
    transcription = the diff reference) but DESYNCS in-game. The A/B
    harness v3.3 is LIVE (all DIAG-marked, 5 trampolines + the DIMU
    logger + the decision trace, all /Fa-verified). The volatile v149
    fix DOES NOT FIX the desync (owner-tested). The desync root cause
    is OPEN. THE DIAGNOSTIC INSTRUMENTATION REMAINS IN PLACE (all
    DIAG-marked for removal at ship). THE COLLISION SCAN: pathing.cpp
    patches are NOT USED (the owner's standing ruling — no bake, no
    note, no further analysis). ALSO: the common.h collisionUnits
    comment fix (provisional); the ship constraint (the four pathing
    functions in one build); the LTG (/GL) probe gap.
    THE V3.3 CAPTURE FINDINGS (the owner can re-run or provide fresh
    logs): (1) The OB queries and the find_path calls: BIT-IDENTICAL
    args + results in both runs through the aligned prefix. (2) The
    get_map_analysis calls: the SAME ma_obj, the SAME NULL/non-NULL
    outcome. (3) THE DIVERGENCE: unit 13738's st=2 call at wt=020081A.
    IDENTICAL IN state. THE VANILLA: TWO creates (190709 + 190710),
    the second installed. MINE: ONE create (190709), the first
    installed. THE WORLD'S PATH-ID COUNTER OFFSET BY -1 FROM THAT
    POINT → THE OOS. (4) THE DECISION TRACE: A2GATE v143=1 wpc=2
    cfg=1 v151=19.0 — ALL INPUTS CORRECT. PREINST v149=1. INSTALL-OK
    v149=0 variant=FOLLOW. NO ARM2-entered row between PREINST and
    INSTALL-OK (arm 2 correctly skipped). NO v149 write in the C++
    source between PREINST and INSTALL-OK. THE COMPILER READS A STALE
    COPY. (5) THE STUB SUB-BRANCH FIRED CORRECTLY in both runs
    (get_map_analysis=NULL → addWaypointAtStart/End → v149=1, v143=1).
    (6) THE VANILLA'S OBSERVED TWO-CREATE BEHAVIOR: pass-1 → the stub
    sub-branch → v149=1 → the install: wl==2 && v149==1 → THE
    SETREADYTOPATH VARIANT (wl=1) → THE CONFIG-GATE RELOOP
    (dword_689564==1) → PASS-2: createPath(190710) → the smooth gate
    FAILED (wl(1)≠2) → arm 2 → findLowLevelPath → wpc=3, len=4199f0ee
    → the install → the followPath variant → wl=0 → break → the walk
    arm → OBQ(n=0) → OUT(190710) → RET=0. MY BODY: ONE create (the
    followPath variant, no reloop).
    THE VOLATILE v149 FIX DOES NOT WORK — the v149 stale-read is a
    SYMPTOM, not the root cause. The root cause is deeper — something
    upstream prevents the stub's v149=1 from surviving, or the
    vanilla's two-create behavior comes from a different code path
    entirely that my transcription doesn't capture.
    IMMEDIATE WORK: (1) CONTINUE THE DESYNC INVESTIGATION using the
    harness (the DIAG rows are live). The next candidates: (a) trace
    incrementWaypoint's inputs/outputs per call (the waypoint/step/
    state after each call) — the increment arm drives the state=2
    transition that triggers the second repath; (b) trace arm-2's
    findLowLevelPath result per call (the found/not-found + the path
    count + the length) — the vanilla's pass-2 arm-2 produced wpc=3;
    if my pass-2 (or the absence thereof) differs, the builder is the
    target; (c) trace the smooth-arm's find_path result per unit per
    tick — the vanilla's find_path succeeded for 13738's pass-1 but
    my body's find_path may return a different result (the map-
    analysis mip-level state may differ per-unit per-tick in ways the
    FPHOOK args don't capture); (d) re-examine the .c/asm for a
    SECOND createPath call site in the state-2 arm that my transcrip-
    tion missed (the vanilla body has TWO creates with NO find_path
    and NO get_map_analysis between them — the second create's caller
    is unidentified); (e) check whether the vanilla body's state-2
    arm has a SECOND ITERATION via a mechanism my transcription
    doesn't capture (the .c's synthetic nesting may hide a loop-back
    edge). PROTOCOL: the harness is the mechanism — never touch
    doFixedDeltaSeconds/fixedDeltaSeconds; the DIAG rows are the
    evidence; the volatile v149 fix stays (it fixes the stale-read
    even if it doesn't fix the desync); the approval gate before
    every write; the DIAG instrumentation removal before the final
    ship. ALSO OWED: the common.h collisionUnits comment fix
    (provisional); the ship constraint (the four pathing functions
    in one build); the LTG (/GL) probe gap; Pass 2 (the restructure)
    on the verified baseline (AFTER the desync is fixed).""

  - THE CAPSTONE DESYNC - ROOT-CAUSED + FIXED + ALL TESTS PASSED (owner-
    confirmed 2026-11-21): TWO stacked root causes in the capstone
    doIntelligentMovementUpdate_new, both transcription-level, localized
    by the paired-capture full-stream diff (see the methodology below):
    (1) THE MISSING FALL-THROUGH (the first desync; the path-id counter
        offset at unit 13738, wt=020081A): calculatePathLength + the ctm
        stamp were transcribed INSIDE the ma!=NULL arm's else; the asm
        joins BOTH arms at loc_4A4D16 and calls unconditionally
        (0x4A4D12 the stub's v143=v35 falls through; lst :281179-281185;
        the .c renders them after the ma if/else, inside the smooth-gate
        block). The stub path skipped the length compute -> pathLength-
        Value stayed 0.0 (the ctor zero; the CREATE row even logged
        len=00000000) -> the $LN297 len gate (comiss 0.0, 0.01) failed ->
        pass-1 wrongly ran the findTile arm (v149=0 its own legitimate
        store 0x4A5376; wpc=3 its fill) -> FOLLOW -> one create -> the
        world path-id counter offset -1 -> OOS. FIX (owner-approved): the
        statement move to after the ma if/else, inside the smooth-gate
        block; gate 0/0 re-run.
    (2) THE V151 FLOAT-CHAIN SHRINK (the second desync, unit 5404,
        wt=00022f7c): my v151 = the /fp:fast-shrunk SSE float chain
        (mulss/addss/sqrtss - 24-bit per op) vs the orig's FLOAT(53-bit
        x87 chain rounded once). At 5404 the increment arm's arrival
        teleport put the unit EXACTLY at the old path's waypoint[1]
        (sx=42d70000=107.0 exact), which EQUALS the ReturnPath's tail,
        so v67 == v151 mathematically and the label_103 tail test
        (st0 53-bit v67 vs var_25C float v151) flipped on the shrink's
        ulp: the orig discarded (wpc=0 -> the install-FAILURE ->
        destroyPath2(4297) + pause 0.25 + return 0), mine kept (the
        findTile's fill -> FOLLOW -> path kept). The WPFETCH trace (the
        new hook, below) PROVED every fetch input identical (the FPHOOK
        rpc=2 lw=(107.0,28.0) rows matched; the v66/v140 fetches
        matched) -> the divergence was the computation, not the data.
    (3) THE FIX (owner-directed): THE WHOLE-FUNCTION PRECISE WRAP -
        float_control(precise, on, push) before + float_control(pop)
        after doIntelligentMovementUpdate_new. Reproduces the original's
        uniform PC53 model (the widened chains emit cvtps2pd/mulsd/
        addsd/__libm_sse2_sqrt_precise + one cvtsd2ss per store-round =
        the orig's fld/fmul/faddp/fsqrt/fstp). Supersedes per-site
        brackets for the capstone: covers the v151 site + the arm-2
        tail-norm site + every unfound shrink site in the 5819 bytes.
        /Fa census (the whole function): mulss=0, sqrtss=0, mulsd=15,
        addsd=8, precise-sqrt calls=7; the 2 remaining addss = the arg7
        radius-sums (v127/v129 = v164 + radius_x) - a two-float sum is
        exact in 53 bits, so the orig's double-rounding collapses to the
        identical addss result: provably equivalent, NOT a shrink (and
        5404's dtr=3f000000 matched in both runs). The rule-(b) comiss
        direct compares preserved; the macro/widened compares emit
        comisd. Control flow unchanged (the pragma only - the exit-path/
        join/store-and-read checks carry over).
    THE THEORIES RETIRED (do not revisit): the "stale v149 read" (v149's
        0 at the install gate was the findTile arm's legitimate store -
        the PATH was wrong, not the value; the volatile was a no-op for
        the desync); "arm 2 -> findLowLevelPath built the pass-2 path"
        (the skip gate v151>10 skips arm 2 on pass-2 too - the pass-2
        builder is the findTile arm, dword_68956C==1); the /GL codegen
        as the cause (both /GL and non-/GL carried the same source bug);
        the v143/v149 "shared stack slot" (a listing-syntax misread: the
        MASM equate adds into the effective address - v143 = base+20,
        v149 = base+44, DISTINCT, the volatile honored).
    THE /GL DECISION (owner): WholeProgramOptimization OFF - the gated
        listing = the shipped code; the LTG probe gap closes with it.
    THE MODEL CORRECTIONS (the asm-verified 5404 flow, the WPFETCH rows):
        the checkDistance swap verified faithful instruction-for-
        instruction (the null -> dtor -> delete -> swap chains; 4124 =
        pd->path, 3236 = pd->mFuturePathData, both destroyed by the dtor
        chain; the surviving linked path = 3235, swapped in with
        state=2); the increment arm's arrival teleport puts the unit
        exactly at the old path's waypoint[1]; pass-2's builder = the
        findTile arm (fills 3, returns FALSE -> v143=0 -> label_103);
        the label_103's tail test verified faithful (the asm pulled:
        fst goal; fcomp var_25C; test ah,41h; jnz -> KEEP at 0x4A5317-
        0x4A5331); BPath::waypoint's negative/out-of-range index = the
        DEFINED zero-fill (no garbage reads); setReadyToPath sets
        waypoint=1 + state=2 (the reloop re-dispatch enters state 2);
        the reloop = jz loc_4A48A2 (the resolve -> precalc -> increment
        -> dispatch head).
    THE NEW INSTRUMENTATION (all DIAG, remove at ship):
    - the FPHOOK row extended with rpc (the ReturnPath count) + lw (the
      last waypoint's raw bits) - proved find_path's outputs identical;
    - the WPFETCH hook: BPath::waypoint (0x4BC440) hooked with a
      caller-filtered logger (the filter: the game's DIMU body
      [0x4A47B5, 0x4A5E6C) + the runtime-derived window over the
      compiled _new body, set at install - the first filter attempt used
      only the game range and filtered out ALL of my body's fetches in
      the _new run: my body lives in the DLL's own .text, not the game's
      range -> the new log had no WPFETCH rows at all);
    - LINE_LOGGING_ENABLED (the owner's macro) gating the in-body
      decision rows.
    THE LESSONS: (a) the trampoline resume transfer = the esp-neutral
      push/ret pair - the mov reg/jmp form clobbers live replay
      registers (my mov eax,004BC446h clobbered EAX holding wpIndex;
      every fetch through the hook returned the zero-fill until the
      owner's fix - the fastcall-liveness rule extended to the resume
      transfer itself); (b) the caller-filter ranges must include the
      compiled body's own window; (c) the diff methodology: the
      FULL-STREAM diff (all common rows - TICK/IN/OUT/RET/lifecycle/
      OBQ/FPHOOK/MAGET/WPFETCH - decision rows excluded, mk+ignSz
      stripped, first divergence only) - the lifecycle-only diff missed
      the divergence position entirely (it sat in a decision-row-only
      stretch).
    GATE: 0/0 x86 after every write. Installer: 81 hooks (80 + the
    WPFETCH DIAG hook). ALL TESTS PASSED (owner-confirmed).
    NEXT: Pass 2 (the restructure) on the NOW-VERIFIED baseline (the
    two-pass protocol; the precise wrap carries over; the aligned
    transcription remains the diff reference); the DIAG instrumentation
    removal + the LINE_LOGGING_ENABLED macro at ship; the common.h
    collisionUnits comment fix (provisional); the Moving-methods
    re-count; the model corrections above absorbed into the capstone
    decode notes.

  - STATE-RECORDING CONVENTION (owner-directed 2026-11-21): a request to
    record the state is NOT a request to close the session - the session
    remains open after a state recording; a close is a separate explicit
    event. State blocks recorded mid-session are live checkpoints and are
    superseded by later recordings in the same session.

  - SESSION CLOSE (2026-11-21, owner-directed closure): THE CAPSTONE DESYNC
    RESOLVED AND VERIFIED EARLIER THIS SESSION; THE PASS-2 RESTRUCTURE
    ATTEMPTED AND LEFT INCOMPLETE - THE WORKING TREE IS IN A BROKEN
    INTERMEDIATE STATE. READ THIS BLOCK FULLY BEFORE TOUCHING
    RGE_Moving_Object.cpp.
    COMPLETED + OWNER-VERIFIED THIS SESSION (all tests passed):
    - THE CAPSTONE DESYNC ROOT CAUSE #1 (the missing fall-through):
      calculatePathLength + the ctm stamp were transcribed inside the
      ma!=NULL arm; the asm joins BOTH arms unconditionally (0x4A4D12 ->
      0x4A4D16 -> 0x4A4D18, lst :281179-281185). The stub path skipped the
      length compute -> the len gate failed -> pass-1 ran the findTile arm
      -> FOLLOW -> the path-id counter offset. FIXED (the statement move,
      owner-approved) + CONFIRMED.
    - THE CAPSTONE DESYNC ROOT CAUSE #2 (the v151 float-chain shrink):
      my v151 = the /fp:fast shrunk SSE float chain vs the orig's
      FLOAT(53-bit x87 chain); at unit 5404 the position == the
      ReturnPath's tail exactly (the arrival teleport) -> v67 == v151
      mathematically -> the tail test flipped on the shrink's ulp. FIXED
      (owner-directed): THE WHOLE-FUNCTION PRECISE WRAP -
      float_control(precise, on, push) before + pop after
      doIntelligentMovementUpdate_new. /Fa census: mulss=0, sqrtss=0,
      mulsd=15, addsd=8, precise-sqrt=7. CONFIRMED.
    - THE THEORIES RETIRED (do not revisit): the stale v149 read (v149's 0
      was the findTile arm's legitimate store - the PATH was wrong, not
      the value); arm-2/findLowLevelPath as the pass-2 builder (the skip
      gate v151>10 skips arm 2 on pass-2 too - the pass-2 builder is the
      findTile arm, dword_68956C==1); /GL codegen as the cause (the same
      source bug in both); the v143/v149 shared-slot claim (RETRACTED - a
      listing-syntax misread: the MASM equate adds into the effective
      address; v143/v149 distinct, the volatile honored).
    - THE /GL DECISION (owner): WholeProgramOptimization OFF - the gated
      listing = the shipped code; the LTG probe gap closes with it.
    - THE NEW INSTRUMENTATION (all DIAG, remove at ship): the FPHOOK row
      extended with rpc (the ReturnPath count) + lw (the last waypoint's
      bits); the WPFETCH hook (BPath::waypoint 0x4BC440, caller-filtered:
      the game's DIMU body range + the runtime-derived _new window, set at
      install); LINE_LOGGING_ENABLED (the owner's macro) gating the
      in-body decision rows; g_dimuTraceOrig = 0 (the _new dispatch).
    - THE MODEL CORRECTIONS: the checkDistance swap verified faithful
      instruction-for-instruction (4124 = pd->path, 3236 =
      pd->mFuturePathData, the surviving linked path = 3235, swapped in
      with state=2); the increment arm's arrival teleport puts the unit
      exactly at the old path's waypoint[1]; pass-2's builder = the
      findTile arm; BPath::waypoint's negative index = the DEFINED
      zero-fill; the reloop = jz loc_4A48A2.
    - THE METHODOLOGY: the FULL-STREAM diff (all common rows, decision
      rows excluded, mk+ignSz stripped, FIRST divergence only) - the
      lifecycle-only diff missed the position entirely.
    THE PASS-2 RESTRUCTURE (this session's final work): STATUS =
    INCOMPLETE - THE FILE DOES NOT COMPILE. What was done: the full
    doIntelligentMovementUpdate_new body rewritten in the staged chunks
    (temp dimu_pass2.cpp, 9 chunks) and spliced into the TU: the
    dispatch loop = for(;;) with continue for the reloops (J1-J7 + the
    arrival + the state-4/5 + the config); the state-2 arm in
    if(pathState == 2){...} else proceedToWalk = true; the walk arm
    unguarded after the arm's close; the v-names dissolved to the role
    names (newPath/activePath/resolvedPath/successor/pathFound/
    smoothUsed/repathDist/bestTailDist/collisionCount/...); the carve
    buffers renamed per-role (every buffer DISTINCT - the aliasing rule);
    the goal/var_264 and var_23C slot lives kept as the separate typed
    variables (verified benign). THE FAILURE: the compile ends with the
    C2267/C2601 cascade ("local function definitions illegal" from the
    _orig trampoline at ~5191 onward) - the compiler's note says the
    fn's opening brace (line 3883) never closes. THE CONTRADICTION: the
    textual brace count (the // comments stripped) = BALANCED (final
    depth 0); the preprocessed .i walk (RGE_Moving_Object.i, 7.8MB, on
    disk) = the DIMU region pp-393436-394910 never returns to depth 0
    (one net-open inside) - THE SOURCE-LEVEL COUNT AND THE COMPILER
    DISAGREE AND THE CAUSE WAS NOT FOUND THIS SESSION. The gate logs:
    temp pass2_gate*.log; the preprocessed: RGE_Moving_Object.i; the
    staged: temp dimu_pass2.cpp. The bisect: the walk arm #if 0'd -> the
    cascade PERSISTED (the imbalance is upstream, in the state-2 arm or
    the loop/dispatch region).
    RECOVERY OPTIONS FOR THE NEXT SESSION (the owner's choice):
    (a) THE VERIFIED BASELINE RESTORATION (DONE 2026-11-21): the verified
        pass-1 source IS committed - commit 2fc06f1 "RGE_Moving_Object::
        doIntelligentMovementUpdate strict form confirmed correct" - and
        was RESTORED into the working tree via git checkout of
        RGE_Moving_Object.cpp from that commit; gate 0/0 re-run on the
        restored file. The broken restructure = preserved in temp
        dimu_pass2.cpp (+ the gate logs pass2_gate*.log and the
        preprocessed RGE_Moving_Object.i - untracked, cleanable). The
        earlier "NOT in VCS" claim below in this entry = superseded.
    (b) CONTINUE THE RESTRUCTURE DEBUG: the imbalance = one brace-pair
        ambiguity invisible to the textual count - the tools on disk:
        the .i (the compiler's actual parse input), the gate logs, the
        staged dimu_pass2.cpp. The suspected region: the state-2 arm's
        install-gate nesting (the LABEL_129 region) - the arm's else
        close placement vs the walk arm (the walk arm was originally
        written INSIDE the arm's else scope; the depths prove it).
    PROTOCOL NOTES CARRIED: the state-recording convention (a state
    recording is NOT a session close - the owner-directed note
    2026-11-21); the full-stream diff methodology; the pp-scan technique
    (the .i brace-depth walk = the compiler's-parse-level verification -
    USE IT FIRST next time: it caught what the source-level count
    missed).

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the capstone doIntelligentMovementUpdate
    desync is FIXED and ALL TESTS PASSED (owner-confirmed 2026-11-21); the
    verified pass-1 baseline (the strict aligned form + both desync fixes +
    the whole-function precise wrap + the DIAG harness) = commit 2fc06f1,
    restored into the working tree, gate 0/0. THE PASS-2 RESTRUCTURE WAS
    ATTEMPTED AND REVERTED: the full-body rewrite (the staged draft = temp
    dimu_pass2.cpp, 9 chunks) spliced into the TU produced a non-compiling
    C2267 cascade (the compiler: the fn brace at 3883 never closes) while
    the textual brace count said balanced - the contradiction was NOT
    resolved; the attempt was reverted via git checkout and the draft is
    preserved at temp dimu_pass2.cpp. THE LESSONS FOR THE NEXT ATTEMPT:
    (1) COMPILE PER CHUNK - the 9-chunk batch spliced once hid the
        imbalance until the end (the BMotion per-chunk lesson
        re-confirmed); (2) VERIFY THE BRACE DEPTH WITH THE PP-SCAN (the
        .i brace walk) DURING the restructure, not after - the pp-level
        count sees what the source-level count misses; (3) THE WALK ARM
        PLACEMENT: the draft nested the walk arm INSIDE the state-2 arm's
        else scope - the state!=2 route never reaches it; the walk arm
        must sit at the dispatch-loop level with proceedToWalk as the
        handoff from the install arms; (4) the LINE_LOGGING_ENABLED macro
        and every DIAG row must be preserved verbatim in sequence for the
        paired-capture verification. IMMEDIATE WORK: (1) the pass-2
        restructure, restarted from the verified baseline (commit
        2fc06f1), compiled per chunk with the pp-scan depth check at each
        stage; the stage map + the brace-depth walk + the exit-path
        inventory + the /Fa call-multiset comparison; then the paired
        capture - the restructured traces must be BYTE-IDENTICAL to the
        pass-1 baseline logs (the owner holds the 22:51/22:52 pair).
        (2) AFTER the pass-2 verification: the DIAG instrumentation
        removal at ship (the WPFETCH hook 0x4BC440, the FPHOOK rpc/lw
        fields, the decision rows via LINE_LOGGING_ENABLED,
        g_dimuTraceOrig + the DIMU logger flip). THE CARRIED ITEMS: the
        common.h collisionUnits comment fix (provisional); the
        Moving-methods re-count; the model corrections absorbed into the
        capstone notes (the pass-2 builder = the findTile arm, NOT
        arm-2/findLowLevelPath; the retired-theories list). PROTOCOL: the
        approval gate before every write; a state recording is NOT a
        session close; the full-stream diff methodology (the common rows
        only, mk+ignSz stripped, FIRST divergence only)."

  - THE CAPSTONE SHIPPED - PASS 2 + SCOPE/NAME PASS + DIAG REMOVAL COMPLETE
    (owner-verified: the paired capture byte-identical on the readability
    build; the ship-build confirming pass CLEAN 2026-11-21):
    THE READABILITY RESTRUCTURE (house style, owner-directed after the
    first pass kept the baseline's declaration topology): the ~170-line
    function-scope declaration block dissolved - every local declared at
    first use, minimal enclosing scope; the head region 170 -> 52 lines;
    only six locals loop-scoped ahead of the dispatch (newPath/successor/
    pathFound/smoothUsed/repathDist/bestTailDist - the install gate reads
    them at loop level) plus topPath (the range gate's read); the true
    accumulators stay function-scope (collisionCount - accumulated across
    passes; passCounter; deltaTime; fwdDir/moveTarget zeroed in S0;
    ReturnPath moved INTO the label_103 block - its only use). The
    redundant locals inlined at their single consumption with the asm
    citations preserved at the sites: rawDt, masterHead, precalcGoalRaw,
    topPathId, v16, argUnitRadius/argRange/argTargetId/argOwnerId/
    terrainTable/tileGoalX-Y/argFindTileRange/dest_target_radiusb (the
    findLowLevelPath/findTilePath arg slots - each a pure read consumed
    once), moveTargetY, smoothedLen, activeCtm, pausePriority (the two
    movementPriority dispatches kept, the cmp fed by the first call's
    return), ~25 single-consumption BPath__waypoint fetch Refs folded
    into `BVector* x = BPath__waypoint(...)` at use. ALL loops for-form:
    the four linked_child walks, the OB record loop, the pause loop, the
    ReturnPath copy loop (0 do-while, 10 for); the re-read-per-iteration
    conditions (the faithful live reloads) preserved and cited. KEPT
    deliberately: the asm-faithful dead stores (normCopy @0x4A546E,
    deadWalkPath @0x4A56A5/0x4A5CAB, goalSlotDead @0x4A5BB4-region,
    linkWalk 0x4A48E4-F3) - each a real instruction in the original,
    required by the store-and-read diff, each dead-noted.
    THE SCOPE/NAME PASS (owner-directed "not verified scopes, not
    named"): every remaining v-name dissolved or role-named - zero
    v-tokens in the DIMU body's code; the goal/goalDist collision hazard
    (two desyncs) ELIMINATED: goal -> precalcTargetDist, goalDist ->
    precalcGoalDist, goalDistRaw -> precalcGoalRaw, goala/goald/goalb/
    goalc -> maLabel103/tailNormStored/recordIdx/goalSlotDead; the
    v37 mid-block local merged into the function-scope ignoreCountArm2;
    the duplicate v106/v107 declarations removed (pauseBldg/pauseTarget
    carry them); v23/v136 dead declarations dropped (zero sites); the
    scope/liveness audit table generated (the NEVER-READ set = the four
    documented dead stores + proceedToWalk - the handoff marker, dead
    after DCE, covered by the instruction identity); the v25-alias open
    item RESOLVED (the asm loads ebp = the resolved path at 0x4A48B5 in
    S1; the stub path never overwrites ebp before the failure-arm fetches
    - the alias holds on every read path).
    DESYNC #9 - THE INSTALL-GATE PLACEMENT REGRESSION (owner-report:
    the readability build desynced; unit 10499 st=2 wl=2 flg=0, cc=9
    vs the orig's cc=10): the readability rewrite of s7 placed the
    install gate BACK INSIDE the flagged-pipeline else - the stub path
    (no-flag units) fell past the install straight into the walk arm
    (the log: six 180368-wp1 fetches at mk=8, st never left 2, no
    INSTALL row, no DESTROY row, then the target/range gates fired on
    rng=0 -> moveUnit -> RET=0; the ORIG: the stub -> INSTALL SUCCESS/
    FOLLOW -> cc=10, st=3, the walk arm on 180369 -> moveUnit). The
    same placement bug class as the R2 walk-arm and the first rebuild's
    gate placement - fixed (the gate moved out of the pipeline else,
    dedented; the pipeline-close comments corrected). LESSON (stated in
    the record): a structural-placement regression is INVISIBLE to the
    compile gate and the /Fa listing (the gate's calls exist in the
    listing regardless of reachability); the stage-map walk (each
    region's entry/exit traced to its asm label) is mandatory BEFORE
    the compile, and the paired capture is the detector.
    VERIFICATION CHAIN per pass: 9/9 per-chunk compile + pp-balance;
    the TU gate 0/0; the /Fa classified-call alignment (the readability
    build aligns 1:1 with the verified build except ONE inserted
    reg-unresolved instruction - the walk-arm takeover's BPathData__
    destructor (0x4BBDD0) materialized twice after the scoping un-shared
    a merged call site: same callee/args, each path calls once -
    codegen-neutral); the token/call-arg multisets vs 2fc06f1 with the
    final maps (every delta decoded into the documented classes); the
    precise census identical (mulss=0 sqrtss=0 addss=2 mulsd=15 addsd=8
    precise_sqrt=7); the paired capture on the readability build
    BYTE-IDENTICAL (owner-verified).
    THE DIAG REMOVAL AT SHIP (the updateMovement-ship protocol):
    intrin.h include, the DIAG globals (g_dimuLog/g_dimuMark/
    g_dimuTraceOrig/g_wpNewLo-Hi/g_wpGameLo-Hi), the M7-era
    calculateYawAngle_orig/_logger dead harness, the LINE_LOGGING_
    ENABLED block + the fprintf_logger macro, the 15 in-body mark rows
    + the 12 decision rows, the whole harness region (the _orig
    trampoline, the DIMU logger + DIMU_logOpen/DIMU_logRow, the
    createPath/destroyPath2/path lifecycle trampolines+loggers, the
    OBQ/FPHOOK/MAGET arg trampolines+loggers, the WPFETCH caller-
    filtered logger), the installer trace hooks - 5745 -> 5222 lines.
    THE INSTALLER FLIPPED: 0x004A47B0 -> doIntelligentMovementUpdate_new
    directly; 80 real class hooks (defs/refs 80/80 exact). THE
    ZERO-DIAGNOSTIC-SYMBOL SWEEP (the updateMovement standard): the
    whole listing _fopen=0, _logger PROCs=0, _orig PROCs=0, val_log=0;
    the DIMU PROC fopen=0/fprintf=0/sprintf=0/g_dimuMark=0. THE
    GAME-BEHAVIOR CALL MULTISET vs the verified capture build (the DIAG
    I/O - fprintf + the rows' __ftoui3 casts - excluded): 153/153 FULLY
    IDENTICAL with the register carriers normalized; the census
    identical. THE SHIP-BUILD CONFIRMING PASS: CLEAN (owner-confirmed).
    THE CAPSTONE IS SHIPPED.
    CARRIED/NEXT: the common.h collisionUnits comment fix (provisional;
    needs the owner ruling - the rename verification stands: the .c's
    collisionUnits = our ignoreUnits); the Moving-methods re-count (the
    stale M-era counts); the four-pathing-functions ship constraint is
    SATISFIED for the current form (updateMovement + DIMU shipped and
    hooked; addToIgnoreList/resetIgnoreList owned-but-unhooked - their
    binary callers are inside updateMovement itself); the LTG (/GL)
    probe gap CLOSED (the owner's WPO-off decision - the gated listing
    = the shipped code). The state-file's line citations for the DIMU
    body are stale (the readability pass moved lines; the addresses
    authoritative per the citation protocol).

  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. STATE: the capstone doIntelligentMovementUpdate
    is SHIPPED (pass 2 + the scope/name pass + the readability restructure +
    the DIAG removal; the paired capture byte-identical on the readability
    build; the ship-build confirming pass CLEAN 2026-11-21; the installer
    0x004A47B0 -> _new directly, 80 hooks, the zero-diagnostic-symbol sweep
    clean). BMotion::updateMovement shipped earlier (the _ida variant ->
    bc41627); the four-pathing-functions ship constraint is satisfied for
    the current form (addToIgnoreList/resetIgnoreList owned-but-unhooked -
    their binary callers are inside updateMovement itself). The DIMU
    desync lessons stand: the goal/goalDist collision class (two desyncs),
    the install-gate placement class (DESYNC #9 - the stage-map walk is
    mandatory BEFORE the compile), the /Fa call-sequence alignment as the
    first detector. IMMEDIATE WORK: (1) the Moving-methods re-count FIRST -
    the M-era per-class remaining counts are stale; recount from the
    runtime-class exports vs the installed hooks per class (the inventory
    regex handles multi-word returns + Attributes:thunk exports); (2) then
    the remaining Moving methods smallest-first per the roadmap (decode ->
    present -> approve -> write; the large bodies under the two-pass
    protocol; the scope/name discipline AT WRITE TIME per the capstone
    readability precedent - locals at first use, minimal enclosing scope,
    for-loops, no surviving v-names; the stage-map walk before the compile
    for any structural restructure); (3) then the other members of the
    object family per the roadmap: Action, Combat, Static, TRIBE_Combat,
    TRIBE_Building runtime methods, smallest-first per class. CARRIED:
    the common.h collisionUnits comment fix (provisional - needs the
    owner ruling; the rename verification stands: the .c's collisionUnits
    = our ignoreUnits). PROTOCOL: all edits approval-gated; the template
    rules (a)-(k) incl. the X87 macro mandate + the probe protocol; the
    two-pass decode for the large bodies; the /Fa call-sequence + census
    gates; the paired capture for the sim-relevant big bodies; the
    zero-diagnostic-symbol sweep at ship; the citation protocol (addresses
    + function names - the line citations rot)."

  - RUNTIME-METHODS RECOUNT (owner-directed 2026-11-21; supersedes all M-era
    per-class remaining counts): the method-surface inventory rebuilt from
    the AUTHORITATIVE sources - exports = the battlegrounds_x2.lst `proc
    near` list per class prefix (includes thunks and idb-unlisted names;
    immune to the multi-word-return regex gap class by construction),
    ownership = the active setHook targets extracted from the 11 class
    files, sizes from proc->endp extents, cross-checked against
    idb_functions.h.  578 exports / 148 hooked / 430 remaining across the
    10 object runtime classes + BMotion: Static 139/6 owned (133 remaining;
    the 6th hook = set_Selected_Groups lives in Action's file), Tree 2/2
    COMPLETE, Animated 12/12 COMPLETE, Doppleganger 9/9 COMPLETE, Moving
    97/80 (17 remaining), Action 45/7 (38), Combat 53/5 (48), Missile
    16/16 COMPLETE, TRIBE_Combat 79/4 (75), TRIBE_Building 76/6 (70),
    BMotion 50/1 (49; updateMovement shipped; addToIgnoreList/
    resetIgnoreList = the only owned-but-unhooked pair - the scan
    confirms).  Moving remaining smallest-first: onPatrol 39, isMoving 63,
    copy_obj 81, updateEnd 99, canPath2 122, set_angle 123 (ALL SIX =
    batch M9, written below), rotate 158, canPath 159, update 186,
    recycle_in_to_game 233, setup 252, save 600, checkAcceleration 750
    (core.cpp:1395 checkAcceleration_patch MID-FUNCTION at +9 - the bake
    analysis owed at its decode), setup_read 875, addMove 957,
    resolveCollisions 975, teleport 1319.  ANOMALIES dispositioned: the
    0-3-byte procs are REAL (retn / xor+ret / getter / jmp-thunk stubs -
    .lst-verified; the 0-byte sizes are an endp-address artifact); 3
    idb-vs-lst aliases, all heal/heal2 (idb TRIBE_Combat heal @0x55CD30 =
    proc heal2, idb heal2 @0x55E000 = proc heal, idb Building heal2 = the
    heal proc - .lst proc names + bodies win at decode); every export has
    an idb prototype except logDebug @0x552510 (none) and unknown
    @0x408BF0 (owned).  The M-era "554 methods" arithmetic (internally
    inconsistent) superseded.  Re-verify: re-run the scan (the lst proc
    list vs the active setHook extraction) after every idb regen /
    installer change.

  - RUNTIME METHODS BATCH M9 - THE SIX SMALLEST MOVING METHODS (owner-
    approved + WRITTEN 2026-11-21): 6 hooks appended to RGE_Moving_Object
    .cpp (installer 80 -> 86).  onPatrol @0x004A6980 (pathData->path->
    flags.valueValue & BBitVectorAND_2; the mask = dd 1 @.rdata:0065BD94 -
    the capstone's verified-literal form; the asm's sbb/neg 0-1 pair = the
    (x != 0) int return), isMoving @0x004A3D90 (BAKED ISMOVING_V2:
    core.cpp:1390 setHook(0x004A3D90, isMoving_v2) is a FULL-FUNCTION
    replacement - movementData != 0 && the RAW BITS of movementData->
    velocity.y != 0 ([edx+8], BMovementData velocity @0, idb_types.h
    :11143); the bit compare is deliberate (-0.0f bits are nonzero = MOVING
    in the stub; a float != 0 would flip it) -> the memcpy 4-byte bits read
    (the SPEED2 form); the original's sqrt-norm gate (0x4A3D9A-0x4A3DC5) is
    dropped BY THE PATCH; the call site retired in place), copy_obj
    @0x004A2EF0 (the sprite-follow: own master's get_move_sprite/
    get_run_sprite (master vtbl slots 30/31, the derived-vtbl-only
    declaration per M4 -> the RGE_Master_Moving_Object* casts) vs sprite
    @+0x1C; the SAME getter on the SOURCE master; non-null ->
    vfptr->new_sprite (slot 44) BEFORE the base RGE_Static_Object__copy_obj
    constant call), updateEnd @0x004A1AC0 (the obstruction refresh:
    vfptr->isMoving (slot 192 = +0x300; the line arithmetic 11005-10813 =
    192, IDA's rendering, and the binary vftable initializer agree) == 1 ->
    ModifyObjectObstruction(mode 16) else (mode 0), keyed on master
    movement_type == 1 (u8 @+120) -> OBSystem_Air (RGE_Airspace_Manager**)
    vs Land; manager vtbl slot 12 = ModifyObjectObstruction walked in BOTH
    manager vtbls; BAKE INTERACTION noted: the dispatch routes to the baked
    v2 isMoving - today's game behavior), canPath2 @0x004A1D50 (the thin
    totalCanPath wrapper: goal = {(float)point.x, 0, (float)point.y} (the
    height slot zeroed), start = positionValue @+0xF4, tryMidLevelPathing =
    1, useLowLevelHulledArea = 0; the prototype's __fastcall edx = targetID
    resolves the asm's mov edx), set_angle @0x004A13C0 (the facing resync:
    facetCount = max(facet_num (__int16 @+112), 8), guard facet_num > 0
    else set_facet(0); the x87 53-bit chain (angle - pi/4) * facets *
    1/(2pi) + 0.5 (flt_65ABE8 = dd 0.78539819 verified from the .rdata
    listing; flt_65C1A8 the shared verified constant), the wrap =
    X87_LT_NAN (the {<0, NaN} taken set), __ftol, set_facet;
    TRIBE_Building_Object__set_angle tail-jumps here - the hook covers that
    dispatch).  COLLISION SCAN (per-batch, mechanical, 2,856 writer sites,
    ALL owned + the 6 pending ranges): 1 real collision = core.cpp:1390
    (the bake above); core.cpp:1393 writeByte(0x004A3E59, 0x16) found
    ACTIVE in the tree despite the M3c ledger's retirement claim - RETIRED
    NOW (comment-only reconciliation; dead bytes under the entry hook);
    core.cpp:1429 / mapsize / pathing.cpp = the standing dispositions
    re-confirmed.  GATE (x86 HostX64\x86 cl, v141_xp 14.16, /O2 /arch:SSE2
    /fp:fast): RGE_Moving_Object.cpp 0 errors 0 warnings; core.cpp 0/0
    (comment-only).  /Fa per function (the real-TU listing): all six PROCs
    standalone; isMoving = cmp [ecx+8],0 + setne (no float ops); updateEnd
    = call [eax+768] x1 + [eax+48] (2 physical sites, 4 logical calls - the
    merge class) + [ecx+120] byte; copy_obj = +120/+124 x2 + +176 x1 + the
    0x005505B0 constant; onPatrol = +280/+16/+32 + and eax,1 (the {0,1}
    masked return - value-identical to the sbb/neg); canPath2 = the 12-byte
    copies ([ecx+244]/[ecx+252]) + mov edx,targetID + 0x004BCF90 + ret 0
    (per-convention; the original's retn 24h is its own __thiscall
    cleanup); set_angle = the exact pool triple (3fe921fb60000000 /
    3fc45f3060000000 / 3fe0000000000000) + comisd+jae (the macro, no
    setcc) + cvttsd2si + cvtdq2ps/cvtps2pd x2 + the two 0x00552A10 calls -
    AND the emitted push carries the FULL ftol int (bit-identical to the
    original's push eax, no narrowing).  SWEEPS: defs/refs 86/86 exact
    (0 dupes, 0 orphans); bare CRT 0 (memcpy = the exception); bare
    base_game 0; pointer-walk 0 NEW (the 5 tree hits = pre-existing int
    counters/float accumulators); literal loop bounds 0 NEW (3 pre-existing
    asm-cited); return types match the prototypes x6; diagnostic symbols in
    the new region 0; store-and-read diff: 0 object stores in all six
    ranges (esp-relative scratch excluded per the rule); exit paths mapped
    per function; joins covered.  FIRST-APPLICATION NOTES: the recount's
    exporter = the .lst proc list (no regex gaps); a pure member-COUNT
    walker undercounts structs containing non-__thiscall members
    (logDebug/debug in the Moving vtbl) - the established line arithmetic
    (member line - first member) remains the slot authority.  NEXT: the 11
    remaining Moving methods smallest-first (rotate 158 / canPath 159 /
    update 186 / recycle_in_to_game 233 / setup 252 / save 600 /
    checkAcceleration 750 / setup_read 875 / addMove 957 /
    resolveCollisions 975 / teleport 1319 - the checkAcceleration bake
    analysis owed), then Action 38 / Combat 48 / Static 133 /
    TRIBE_Combat 75 / TRIBE_Building 70 per the roadmap.  IN-GAME
    VALIDATION OWED for the batch (the owner's standard test).

  - THE RAND RULING (owner-directed 2026-11-21, "we should use
    rand_internal()"): ALL owned rand consumers use rand_internal
    (common.h:154, bound @0x00632BDD - the game's CRT rand) - NEVER the
    bare idb `rand` constant name (the bare-CRT-name hazard; the wrapper
    discipline).  The two shipped M1 Animated update sites corrected
    (RGE_Animated_Object.cpp :162/:168) - BEHAVIOR-IDENTICAL (both forms
    bind the same 0x00632BDD); gated 0/0.  First M10 applications:
    recycle_in_to_game/setup.  Re-verify: grep src/engine for bare
    `rand()` (= 0).

  - RULE-(k) EXTENSION (owner-approved 2026-11-21, the M10 update case):
    the float_control(precise) function bracket applies to ANY function
    whose float chains feed 53-bit consumers - NOT only division-
    containing functions.  The /Fa shrink scan caught update's velocity
    norm compiling mulss/addss + one cvtps2pd under /fp:fast DESPITE the
    explicit (double) casts (the incrementWaypoint lesson generalized:
    casts alone do not resist); under precise the chain emits
    cvtps2pd-per-operand + mulsd/addsd + __libm_sse2_sqrt_precise (the
    established bit-identical form).  The shrink scan is the mandatory
    detector; the bracket is the fix.

  - RUNTIME METHODS BATCH M10 - MOVING recycle/setup/update/canPath/rotate
    (owner-approved + WRITTEN 2026-11-21): 5 hooks appended to RGE_Moving_
    Object.cpp (installer 86 -> 91).  recycle_in_to_game @0x004A0850 (the
    random facing block: (rand_internal()*facets)/0x7FFF (the asm's
    80010003h magic-imul signed divide) * dbl_65C1A0/facets (the verified
    double 6.2831853); the fst m32 store-round with st0 KEPT UNROUNDED for
    both wrap compares (rule (e)); the wrap-up = X87_LT_NAN {<0,NaN}; the
    wrap-down = the literal > MATCH row (the asm's test ah,41h; jnz targets
    the skip so {<,=,NaN} skip); set_angle vtbl slot 222 = +0x378 (line
    arithmetic 11035-10813); precise-bracketed per rule (k) - the fdivr
    division), setup @0x004A0940 (the recycle twin + type = 30 stored
    AFTER the base Animated setup and BEFORE the sprite guard; every exit
    returns 1; hooking routes the owned Moving ctor/ctor_read and Action
    runtime construction dispatches), update @0x004A1A00 (u8; the
    elapsedTimeSec = the DOUBLE getWorldTimeDeltaSeconds return consumed
    by the asm's fstp m32 - the faithful float local, rule (e); the
    velocity norm chain (x*x+z*z+y*y, the asm's exact add order) ->
    __libm_sse2_sqrt_precise -> speed @+0xB4 (rule (d)); the substitute
    machinery: setPosition vtbl slot 190 = +0x2F8 (line arithmetic
    11003-10813), mHasSubstitutePosition == 1 (the dword-exact test),
    mConsecutiveSubstituteCount ++/zero; the movementData re-read after
    BMovementData::update is faithful; precise-bracketed per the rule-(k)
    extension), canPath @0x004A1DD0 (the -1/dead-target zero exits; the
    target position via the copy-out getter position (RGE_Static_ObjectVtbl
    slot 171 = +0x2AC, walked: first member 3935, position 4106) into its
    OWN out-buffer - single fetch, single consumption (the aliasing rule);
    totalCanPath with tryMidLevelPathing=1, useRangeWithoutTarget=0,
    useHulledArea forwarded; the owner->world TRIBE_World family cast),
    rotate @0x004A2F50 (angle += amount * dbl_65C1A0/facetCount (fidiv)
    + angle; the wrap-UP do-loop continues while X87_LT_NAN(v, 0.0); the
    wrap-DOWN runs only on ordered >= dbl - the asm's test ah,1; jnz skips
    on {< dbl, NaN}: a NaN angle reaches new_angle UNCHANGED (the .c's
    do-while render misstates the NaN path - value-identical NaN, asm
    wins); the m32 stores before each new_angle pass = the faithful
    store-rounds; new_angle vtbl slot 76 = +0x130 (line arithmetic
    10889-10813); the two original call sites = the duplicated shared
    tail, merged (the codegen-neutral class); TRIBE_Building_Object__
    rotate tail-jumps here - the hook covers that dispatch;
    precise-bracketed).  COLLISION SCAN (per-batch, mechanical, 2,860
    sites, ALL owned + the 5 pending ranges): 0 hits in all five ranges;
    the non-self overlaps = the four standing dispositions (core.cpp:1429
    no-op, MAP_SIZE, PATHING_CAP x2).  GATE (x86 HostX64\x86 cl, v141_xp
    14.16, /O2 /arch:SSE2 /fp:fast): RGE_Moving_Object.cpp 0 errors 0
    warnings; RGE_Animated_Object.cpp 0/0 (the rand fix).  /Fa (the
    real-TU listing): all five PROCs standalone; divsd x1 in each
    bracketed function (no reciprocal chains); update = cvtps2pd x3 ->
    mulsd x3 -> addsd x2 -> sqrt_precise (no ss-ops); the exact pool pair
    (401921fb53c8d4f1 / 401921fb60000000) in recycle/setup/rotate; the
    slot offsets [+888]/[+304]/[+760]/[+684]; the field offsets [+94]
    type, [+128] sprite flag, [+112] facet_num, [+200] angle, [+180]
    speed, [+288] movementData x3, [+140] owner->world (0x8C), [+296/
    300/312] substitutes; the base constants 0x422BF0/0x422C30/0x422CD0
    (HOOKED - routes), 0x4BBD10, 0x623DB0, 0x624B50, 0x4BCF90,
    0x4A6A70; setup's [+888] x1 and [+200] x2 = the merged wrap-up tail
    and the dropped then-overwritten first angle store (dead-store
    elimination, value-invisible - set_angle reads only after the final
    store; the SOURCE keeps all three stores per the diff; recycle kept
    all three sites x3 - compiler variance between the twins,
    documented).  SWEEPS: defs/refs 91/91 exact (0 dupes, 0 orphans);
    bare CRT 0; bare base_game 0; pointer-walk 0 NEW; literal loop
    bounds 0 NEW; return types match the prototypes x5; diagnostic
    symbols 0.  Remaining Moving 6: save 600 / checkAcceleration 750
    (the core.cpp:1395 mid-function bake analysis owed) / setup_read
    875 / addMove 957 / resolveCollisions 975 / teleport 1319 - then
    Action 38 / Combat 48 / Static 133 / TRIBE_Combat 75 /
    TRIBE_Building 70.  IN-GAME VALIDATION OWED for the batch.

  - ABI CORRECTION - M9/M10 __fastcall SIGNATURES (owner-caught + fixed
    2026-11-21): the eleven M9+M10 replacements were drafted with plain
    (__cdecl-defaulted) signatures - at hook entry the compiler read
    this_ from [esp+4] = the caller's stack (the E9 jmp leaves esp at
    the caller's return address) - BROKEN ABI.  FIXED: all eleven now
    __fastcall per the 2026-11-14 ruling; DWORD dummy on the six whose
    originals carry stack args (copy_obj/canPath2/recycle_in_to_game/
    setup/canPath/rotate - the dummy absorbs EDX so arg1 lands at
    [esp+4]); NO dummy on the five this-only originals (onPatrol/
    isMoving/updateEnd/set_angle/update) per the owner ruling: the
    dummy is OPTIONAL (omittable) when the original __thiscall takes
    only this - __fastcall f(this_) is ABI-identical (this in ECX).
    /Fa ABI PROOF (the re-gated listing): zero _this_$[esp] reads
    across all eleven - this_ sourced from ECX everywhere (the
    this-only forms deref [ecx+off] directly).  Full re-certification
    on the re-rolled codegen: the precise chains hold (divsd x1 per
    bracketed function, mulsd/addsd, no ss-ops, sqrt_precise), the
    pool constants, all slot offsets (+888/+304/+760/+768/+684/+48),
    and the macro comisd forms intact; recycle's two set_angle sites
    merged to one in the re-roll (the codegen-neutral class, each path
    calls once).  LESSON: the __fastcall convention is part of the
    WRITE-TIME checklist - the compile gate and the /Fa field-offset
    checks do NOT catch a missing specifier (the arg reads still
    type-check; only the source register differs).  Re-verify: the
    eleven definitions carry __fastcall; grep the M9/M10 bodies'
    listings for _this_$[esp (= 0).

  - IMPLEMENTATION-PURITY GROUNDING (decode-derived 2026-11-21, the
    CHECKACCEL_PATCH bake's load-bearing proof): RGE_Master_Moving_
    Object__maximumSpeed / __acceleration / __deceleration (the Moving
    master vtbl entries 20/21/22, initializer-counted at .rdata:0065B304)
    consume NEITHER this NOR the int terrainType arg: maximumSpeed = fld
    [ecx+0D0h] (master->speed), acceleration = fld ds:flt_656A10,
    deceleration = fld ds:flt_65B384 (=-3.4028235e38 = -FLT_MAX), all
    `retn 4` (asm 0x48B460-0x48B486; the Static master vtbl ends at
    entry 29 with nullsubs - the entries resolve through the derived
    vtbls per the M4 precedent).  This is what makes checkAcceleration_
    patch's substituted args (caller-eax garbage / the movementData
    pointer) value-innocuous.  Re-verify: the .lst procs; the derived
    master vtbl initializer XREFs (Action/Combat/... point at the same
    procs - no overrides).

  - RUNTIME METHODS BATCH M11 - save/setup_read/checkAcceleration
    (owner-approved + WRITTEN 2026-11-21): 3 hooks appended to RGE_Moving_
    Object.cpp (installer 91 -> 94).  save @0x004A0DB0 (void, 1 arg):
    the base Animated save (hook-routed) + the field sequence in the
    asm's exact order with sizeof-form widths (the ten dwords, the three
    u8s, the linked-chain count + every element saved DEEPEST-FIRST via
    the (count-i-1)-step walk (asm 0x4A0EBD-0x4A0EDD), the
    mFuturePathData/movementData presence dwords + the conditional saves,
    positionValue/both orientations, lastMoveTimeValue, the waypoint
    count + each waypoint, the substitute trio); the presence locals
    written as INT - the original materializes them as zero-extended
    dwords (xor eax/test/setnz al/mov, asm 0x4A0EE5-0x4A0EF3 and
    0x4A0F15-0x4A0F1F) so the int form is STREAM-IDENTICAL (no
    bool-width deviation; documented).  setup_read @0x004A0A40 (int, 2
    args): the save mirror - initMovementData (hook-routed), the base
    Animated setup_read (hook-routed), type = 30, the read sequence,
    the linked chain built HEAD-LAST (each fresh BPathData links to the
    previous; the LAST read becomes pathData), the future-path block
    (presence == 1), the movement-data block (the 12-byte scratch reads
    -> the {x,z,y} BVector copies), the waypoint count + the OWNED
    resizeUserDefinedWaypoints (hook-routed; zero return aborts with 0)
    + the scratch reads, the substitute trio, return 1; the THREE
    allocation sites each under an allocation-unwind fold written as
    scoped __try/__except(operator_delete_internal(node)) blocks (the
    .c's v23 state machine = one handler per site; the three-block
    restructure documented - the setForwardVelocity precedent).  HOOK
    ROUTING: resizeUserDefinedWaypoints and initMovementData dispatches
    route to our owned replacements.  checkAcceleration @0x004A6560
    (int, 3 args) - THE CHECKACCEL_PATCH BAKE (the analysis of the
    mid-function derailment at +9): the original prologue = sub esp,58h/
    push ebx/ebp/esi/mov esi,ecx/push edi (9 bytes exactly); the stub
    substitutes its own prelude (master->vfptr->acceleration -> var_54
    and ->deceleration -> var_58 materialized BEFORE the movementData
    null-gate - the original PARKS the acceleration result on the x87
    stack and NEVER stores var_58: the limitedYaw and inner-hit paths
    read UNINITIALIZED stack garbage at 0x4A66E2/0x4A6819, and the
    parked register leaks across two returns), reroutes the third call
    through this->vfptr->calculateMaximumSpeed (the runtime slot 199 -
    also the original's own dispatch at 0x4A6606-0x4A663E), and rejoins
    at 0x4A6644 with a REBALANCED frame (all three pushes callee-popped:
    accel/decel are retn-4 originals, our __fastcall hook rets 4).
    THE ARGS PROVEN INNOCUOUS (the implementation-purity grounding
    above): the stub's caller-eax garbage / movementData pointer args
    compute the identical values.  The written form = the .c's form
    (var_54 = acceleration, var_58 = deceleration; the four gates
    polarity-exact: three literal > MATCH rows (velocity.y > 0.0f,
    fabs(yawAngle) > 0.0000001000000011686097 = dbl_6545B0's exact
    decimal, crossRef->z > 0.0f - each test ah,41h; jnz -> skip),
    X87_LT_NAN (the toLateral-norm hit fall-through {<, NaN}),
    X87_LE_NAN (the velocity.y < maxSpeed LABEL_11 gate {<, =, NaN}));
    the two 53-bit norm chains in the asm's exact (x,z,y) and (y,z,x)
    add orders; the component divides m32-stored; the named virtual
    dispatches: calculateMaximumSpeed (slot 199 - OUR HOOK),
    calculateTurnRadius (slot 203 - OWNED), setForwardAcceleration
    (slot 188 - OWNED), setForwardVelocity (slot 186 - OWNED); the
    accel/decel master vtbl members (slots 21/22, no cast);
    precise-bracketed per rule (k) - the three divides.  The patch call
    site (core.cpp:1395) RETIRED in place.  COLLISION SCAN (per-batch,
    2,865 sites, ALL owned + the 3 pending ranges): 1 real collision =
    core.cpp:1395 (the bake above); the standing dispositions
    re-confirmed; nothing else.  GATE (x86 v141_xp 14.16 /O2 /arch:SSE2
    /fp:fast): RGE_Moving_Object.cpp 0 errors 0 warnings; core.cpp 0/0.
    /Fa: save/setup_read = the pure stream forms (no float ops);
    checkAcceleration = divsd x3 (no reciprocal), mulsd x6/addsd x4/
    cvtps2pd x10/sqrt_precise x2 (the 53-bit chains), mulss/addss x3 =
    the rule-(d) store-only lateral scale/translate (the original's own
    fstp m32 form - NOT a shrink), comisd x2 + jae (the two macros),
    cvttss2si x6 = the three tile-walk ftol pairs, every slot offset
    (+752/+744/+812/+796/+84/+88) and field offset (+288 x3/+244/
    +252 x4/+140/+52/+8 x6/+268/+256), the four BVector helper
    constants; the compiler CSE'd the three identical tile walks into
    ONE (value-identical - the three callees proven non-mutating
    constant loads; the codegen-neutral class, documented);
    save's BPathData__save call site duplicated by the loop rotation
    (one call per iteration).  SWEEPS: defs/refs 94/94 exact (0 dupes,
    0 orphans); bare CRT 0; bare base_game 0; pointer-walk 0 NEW;
    __fastcall + DWORD dummy on all three (each original carries stack
    args); return types match the prototypes x3; diagnostic symbols 0.
    Remaining Moving 3: addMove 957 / resolveCollisions 975 /
    teleport 1319 - M12 next, SINGLE-PASS per the owner ruling (the
    two-pass bar is NOT met - see the two-pass calibration entry).
    IN-GAME VALIDATION OWED for M9/M10/M11.

  - THE TWO-PASS BAR - TEMPLATE AMENDED (owner-directed + approved
    2026-11-21): the mechanical detector written into DECOMP_TEMPLATE.md
    after the TWO-PASS DECODE section (superseding the "working bar"
    sentence): (a) size >= 4096 bytes; (b) gotos >= 12 in the .c body
    AND back-edges >= 12 in the asm body (the dispatch-loop conjunction
    - teleport's 16 gotos / 1 back-edge correctly excluded); (c) desync
    FALLBACK - static analysis first (the strict instruction-level
    re-verification per the DESYNC REPORT SEMANTICS), the two-pass
    re-decode only when static yields no divergence (owner-amended);
    (d) owner override.  Calibration 16/16 (the table in the
    conversation + the script at %TEMP%\opencode\
    twopass_calibration.ps1); the measured non-triggers documented
    (st(1-7)/fst-keep do not separate).  M12 (addMove/resolveCollisions/
    teleport) = SINGLE-PASS per the rule + the owner ruling.

  - M12 DECODE STATE (in progress 2026-11-21; the .c bodies pulled;
    teleport's asm walk COMPLETE; addMove/resolveCollisions' walks OWED;
    nothing drafted; no writes; the full decode detail lives in this
    session's conversation - the state carries only the durable facts per
    the owner directive - no function bodies in the state file):
    * PROTOTYPES corroborated: addMove @0x4A30F0 (retn 24h, 9 stack
      args), resolveCollisions @0x4A5E70 (BVector* newPosition),
      teleport @0x4A1440 (double, float x/y/z).
    * TELEPORT: asm walk COMPLETE.  Durable models: the trail
      pipeline's mixed-precision chain (dx unrounded on st0; dy/dz
      store-rounded; the quotient double/float split; the i16 count);
      the clamps use map_width for BOTH axes and the maximumSpeed gate
      is X87_LE_NAN (the .c's literal <= diverges on NaN); the terrain
      matrix keyed on the SAVED vs LIVE on_ground + the old/new terrain,
      Extra_Terrain_Table = {exit@+0, enter@+4, walk@+8, rate@+12 float}
      (stride 16); LABEL_47 = the positionValue sync vs the 1e-7 band;
      the return = the base Static teleport's double.
    * ADDMOVE: structure banked (reset-or-create + the SEH fold;
      createPath + the ctm stamp; arm A = the future-path adoption;
      arm B = the fresh command; the patrol/linear/v34 flag tail;
      return 1).
    * RESOLVECOLLISIONS: structure banked (the ObsRecord push loop over
      obSystemCollisions with the deref rule; the dopple-swap suspect;
      the AABB; the push computation + the 1e-7 gate family).
    * OPEN ITEMS: the addMove + resolveCollisions asm walks; the
      teleport world-field offsets (extra_terrain_info/sprites/
      particle_system/world_time) + the tile field name @+0x28; THE
      v34 PROTOTYPE SUSPECT in addMove (char @ebp+24h beyond the 9
      args - the asm walk resolves: the 10th-arg IDB correction vs the
      spill class); the dword_68957C binding (int* const - the owner
      approval item); the resolveCollisions maximumSpeed slot + the
      v10[2] field identities; the masks VERIFIED (dword_65BD98 = 2,
      dword_65BDA8 = 0x20, .rdata - the verified-literal form).
    * CONTINUE: the addMove + resolveCollisions asm walks; the drafts;
      the temp gate; the per-batch collision scan; the presentation.

  - M12 OWNER-TOUCH ITEMS RESOLVED (2026-11-21): the IDB corrected +
    regenerated (addMove's 10th param = unsigned __int8 oneWay - the u8
    matches the original's byte read); the regenerated addMove .c
    cross-checked (the fake v34 char gone = the proper SEH state int;
    the body = the completed walk, the marker :174983).  THE TELEPORT
    enter/exit FIELD RULING = STILL OWED (the table[OLD][+4]-vs-exit
    discrepancy - the decode blocker for the teleport draft).

  - CHECKACCELERATION A/B RUN 2 - THE DIVERGENCE LOCALIZED (the owner
    ran the extended harness 2026-11-22; the logs 4.9/5.06 GB in the
    game dir; the first divergence at row 2083198): unit id=11358,
    the IDENTICAL inputs (mv0=1 vy0=0 ay0=0, the same point, pig=0,
    lya=0), NO setForwardAcceleration call (the velocity>=maxSpeed
    skip arm, ret=1, no FA row) - and the POST-CALL acceleration.y:
    A (the stub) = 0, B (ours) = +FLT_MAX.  THE MECHANISM: our
    accelValue = the dispatched master acceleration proc's return =
    flt_656A10 = +FLT_MAX (binary-verified); for unit 11358's master
    the stub's build delivered 0 -> the unit's master vtbl slot 21
    does NOT return the constant -> the unit's master = the
    STATIC/TREE tier: the Static master vftable (0x65B40C) entries
    17-29 = nullsub procs - entry 21 = nullsub_29 = the EMPTY proc
    returning THE X87 STACK RESIDUE (the entry st0 state); our
    replacement calls the same nullsub and reads the caller's x87
    residue as the return (= +FLT_max-shaped garbage this run) and
    WRITES it into acceleration.y -> the integration moves the unit ->
    the position/command divergence -> the desync.  THE NULLSUB-TIER
    CLASS: for the Static/Tree-tier masters the acceleration/
    deceleration/calculateMaximumSpeed dispatches are nullsubs/consts
    - the "return value" = the x87 residue = NOT faithfully
    transcribable in C.  THE RULING REQUESTED: the Static/Tree-tier
    LABEL_11 arg = 0.0 (the observed stub behavior at this site) /
    skip the write / +/-FLT_MAX - each = a gameplay choice, the
    in-game validation = the arbiter.  THE A/B HARNESS ITSELF:
    verified transparent (the call-based trampoline chain = esp-clean:
    the logger's 12-byte __fastcall args + the ret = balanced by the
    body's retn 0Ch; the body's [esp+X] locals = self-consistent
    within the shifted frame).  The 9FFFFFFD pool reference in the TU
    = addUserDefinedWaypoint_new's OWN epsilon (1.0000001e-7f - a
    different float from 1e-7f; the original uses both) - correct,
    NOT the checkAcceleration bug.

  - M12 DECODE STATE 2 (2026-11-21; the IDB corrected + regenerated:
    addMove's 10th param = unsigned __int8 oneWay - the decode
    restarted from the regenerated .c: the fake v34 char is GONE (now
    the proper SEH state int), the body matches the completed walk;
    the .c marker shifted to :174983):
    * ADDMOVE: THE WALK IS COMPLETE (0x4A30F0-0x4A34AD) + the
      regenerated .c cross-checked (:174983).  The callee constants
      VERIFIED in the idb: createPath :6095 (0x624640), BPath__waypoint
      (0x4BC440), addWaypointAtStart :2523 (0x4BC490),
      addWaypointAtEnd :2524 (0x4BC530), setIgnoreUnits :2530
      (0x4BC7A0), BPathData__reset :2507 (0x4BBE70), setReadyToPath
      :2513 (0x4BBFE0), setReadyToFollowPath :2514 (0x4BC080),
      setLinkedPath :2515 (0x4BC100), BPathData__BPathData :2505
      (0x4BBD80), BMovementData__BMovementData :2503 (0x4BBCF0),
      BPathData__save :2518/load :2519 (0x4BC220/0x4BC2C0);
      zeroUserDefinedWaypoints = the runtime slot +0x260 = 152 (OWNED,
      routes); the world_time world-field offset = +0x10 (the shipped
      BMotion.cpp:21 grounding); the flags masks VERIFIED
      (dword_65BD98 = 2, dword_65BDA8 = 0x20, .rdata; the
      BBitVectorAND_2 = 1 @0x65BD94).  The oneWay param: the flags
      tail reads its LOW BYTE (mov cl,[esp+58h+arg_20]; test cl,cl) ->
      flags |= 0x20 else &= ~; the u8 type matches the byte read.
    * RESOLVECOLLISIONS: THE WALK PARTIAL (0x4A5E70-0x4A5FC0 of
      0x4A623B).  Banked: the {x,z,y} copy of *newPosition (v); the
      count gate = numberObSystemCollisions DIRECT VALUE load (the
      deref rule) <= 0 -> loc_4A6206 (the tail); the walker = ebx =
      offset obSystemCollisions (the ObsRecord** base); the loop
      loc_4A5EB4: master @+14; rec = [ebx] (the pointer load); the
      AABB setup vs master radius_x/y @+3C/+40 (v.x-v.rx -> var_3C,
      v.y-ry -> var_34, v.x+rx -> var_60, v.y+ry -> var_58); rec->
      Object @+8; null -> skip; obj->id @+10 -> RGE_Game_World__object
      (world @+8C); null -> skip; THE DOPPLE-SWAP RESOLVED: type @+5E
      == 80 (byte) && [eax+1DC] != 0 -> eax = [eax+1DC] = THE
      TRIBE_Building_Object linked_owner @+0x1DC=476 (the M1b-6 census
      offset - the .c's mMaximumPathingGroupSize render = the base-
      pointer artifact; the IDB field name at that address in the
      Building context = linked_owner); null -> skip; the stationary
      gate: vfptr->maximumSpeed (the RUNTIME slot +0x188 = 98) fcomp
      const_f_zero; test ah,41h; jz -> SKIP the moving blocker (the
      literal > MATCH row: the NaN -> NOT > -> continue); the blocker
      AABB (its world_x/y @+48/+4C vs its master radius_x/y @+3C/+40
      -> var_48/40/30/28); the FIRST-ITERATION guard: the flags from
      cmp ebx,offset obSystemCollisions (0x4A5F44) persisting across
      the x87 block to jle @0x4A5F72 -> the first record skips the
      AABB test; the four AABB compares with the mixed polarities:
      two test ah,1; jnz skips (our-upper < their-lower) + two test
      ah,41h; jz skips (our-lower > their-upper - the <= -> skip =
      the overlap continues) - ALL FOUR map the .c's skip chain;
      loc_4A5FC0: operator_minus(&v, &result, &positionValue)
      (positionValue @+F4; the .c verbatim - the ecx arg = the v
      scratch, NOT the object - verified: the lea/mov sequence after
      the pushes).  OWED: the walk 0x4A5FC0-0x4A623B (the norm chain
      - the .c's (z,y,x) order; the 1e-7 gates; the blocker position
      fetches (the two position vtbl calls into v39/v40 - the
      copy-out rule); the push computation + the v18/v22 ladder; the
      loop advance + the tail (loc_4A6206: presumably
      *newPosition = v; return the count)).
    * TELEPORT: THE TERRAIN-MATRIX DISCREPANCY (the decode blocker):
      the differ-terrain+grounded branch (0x4A174E-0x4A17D2) emits TWO
      particles per the asm: particle A = table[NEW].enter (@+4, the
      IDB offsets) at the NEW position; then the gate table[OLD]+0 >
      0 -> particle B with the sprite table[OLD][+4] = the IDB's
      enter_tile_sprite_id at the OLD position.  The .c renders
      particle B as exit_tile_sprite_id (v34 = v27[v28].exit ->
      sprites[v34]) - ONE field for both the gate and the sprite -
      while the asm has the gate at [entry+0] and the sprite at
      [entry+4] (TWO fields).  The idb Extra_Terrain_Table layout
      {exit@0, enter@4, walk@8, rate@12} is calibrated by the
      walk@+8/rate@+12 reads (loc_4A17D7) - so per the IDB the asm
      reads [OLD].enter as particle B's sprite while the .c says
      exit.  EITHER the IDB exit/enter names are swapped at this
      struct (the heal/heal2 class - resolvable ONLY by the DAT-driven
      sprite semantics or the owner), or the .c conflated.  THE
      WRITTEN FORM MUST WAIT for the owner ruling: per the asm the
      gate field = [+0], the sprite field = [+4], with the IDB names
      exit/enter respectively - the faithful transcription uses the
      IDB names as-is (the gate = exit_tile_sprite_id, the sprite =
      enter_tile_sprite_id) and NOTES the .c conflict, OR the owner
      rules on the names.  The rest of teleport: FULLY WALKED (the
      trail arm + the clamps + the base + LABEL_47 - see the earlier
      M12 decode state).
    * ALSO OPEN: addMove's arm-A norm gate = flt_6565F4 (the float
      1e-7f compare - the literal > MATCH row, the comiss float form
      safe: both operands float-exact); addMove's retn = 24h (the 9
      stack dwords = the corrected prototype: 8 idb args + oneWay as
      the 9th stack dword - the __fastcall signature: this_, dummy,
      targetID, x, y, z, range, patrol, linear, forcedWaypoint,
      oneWay - NINE stack args after the two register args... the
      stack layout: tID@+4 .. forcedWaypoint@+20, oneWay@+24 (the
      10th = ONE dword, the u8 in the low byte); the signature =
      int __fastcall (...9 stack params...).  The M12a drafts = the
      next work; the resolveCollisions walk completion = the blocker
      for its draft.
    * CONTINUE: the addMove + resolveCollisions asm walks; the drafts;
      the temp gate; the per-batch collision scan; the presentation.

  - CHECKACCELERATION EPSILON CORRECTION + BINARY VERIFICATION
    (owner-flagged line 5783 + fixed 2026-11-21): the written
    fabs-gate literal 0.0000001000000011686097 parsed to
    3E7AD7F29FFFFFFD - WRONG vs the original dbl_6545B0.  FIXED to
    the dd-constant-mandate form 0.0000001f; the emitted comiss
    xmm0,__real@33d6bf95.  BINARY-VERIFIED (the owner supplied the
    original battlegrounds_x2.exe in the idb folder): the PE section
    walk (the ImageBase 0x400000; .rdata RVA 0x254000 = raw 0x254000,
    the 1:1 mapping) gives dbl_6545B0 = 3E7AD7F2A0000000 RAW = the
    1e-7f widening 1.0000000116860974e-07 (the .lst dq render
    correct; the .c's 22-digit decimal render = the TRUNCATED exact
    expansion - re-parsing it rounds to the NEXT DOUBLE DOWN
    3E7AD7F29FFFFFFD, the transcription trap); dbl_65C1A0 =
    401921FB53C8D4F1 RAW (our written 6.2831853 double correct);
    flt_6565F4 = 0x33D6BF95 RAW (our comiss constant correct).
    THE COMISS-vs-FCOMP verification: the original = the x87 fabs/
    fcomp at 80/53-bit vs the m64; our comiss = the float compare.
    OUTCOME-IDENTICAL for this site: both operands are
    float-representable (yawAngle the float; A0000000 = the exact
    widening of 1e-7f) and the float->double widening is exact +
    strictly monotonic -> the three-way result and the unordered
    partition bit-identical (the jbe {<=,NaN} vs the ah,41h {<,=,NaN}
    skips).  THE BOUNDARY: a non-float-representable double operand
    requires comisd - the X87_LT_NAN(toLateralNorm,...) site = comisd
    (toLateralNorm = the 53-bit chain result), correct; the
    alternative m64-shape-fidelity form (fabs((double)yawAngle) ->
    comisd vs A0000000) offered to the owner, not taken.  THE TOOLS
    UPGRADE: the raw-binary constant check (the PE-walk script,
    %TEMP%\opencode\verify_bin.ps1) supersedes BOTH IDA renders for
    constant transcription - the renders are previews, the bytes are
    ground truth; run it for every double/qword-origin constant.
    Re-verify: grep checkAcceleration_new for 0.0000001f (= 1); the
    /Fa pool = 33d6bf95.

  - CHECKACCELERATION DESYNC - THE A/B HARNESS PREPARED (owner-directed
    "prepare the A/B test" 2026-11-21; the static re-verification
    exhausted-clean first - the emission census, the bake model
    re-derived twice consistent, the arg-innocuity binary-backed, no
    enumerable divergence - so the evidence-producing step = the
    harness per the diagnostic process).  THE COMPONENTS (all
    DIAG-marked in RGE_Moving_Object.cpp, remove at ship): (1) the
    _orig trampoline RGE_Moving_Object__checkAcceleration_orig (the
    naked __fastcall; the 9-byte prologue replay sub esp,58h/push
    ebx/ebp/esi/mov esi,ecx/push edi + the ESP-NEUTRAL push/ret pair
    to 0x4A6569 - EAX PRESERVED for the stub's arg push, the
    fastcall-liveness rule; the /Fa-verified: the replay emitted
    verbatim, the push-imm32+ret 0 absolute jump - no entry+len
    dependency); (2) the checkAcceleration logger (the dispatch per
    g_checkAccelTraceOrig: 1 = _orig (build A), 0 = _new (build B);
    the row: CA id/mv/vy/world/point/pig/lya/ret); (3) the
    setForwardAcceleration logger at 0x4A3AC0 (the row: FA id/v - the
    side-effect argument = the LABEL_11/limitedYaw/inner-hit
    selection's observable output); (4) the installer swap: the
    checkAcceleration hook -> the logger, the real line commented;
    the FA logger hook added, the real line commented.  THE BUILD
    MATRIX: A = g_checkAccelTraceOrig=1 AND core.cpp:1395 ACTIVE (the
    stub reference flow, the pre-M11 behavior); B =
    g_checkAccelTraceOrig=0 AND core.cpp:1395 retired (our
    replacement).  THE EXPECTED DIFF: the LABEL_11 FA arg (accel vs
    -FLT_MAX), the limitedYaw/inner FA args (decel vs the original's
    uninitialized var_58 garbage - the garbage itself may be
    nondeterministic per machine = the OOS class), the gate-taken
    pattern.  THE FLOAT-COMPARE SITES RE-VERIFIED in the fresh
    listing: comiss x3 (the float-exact gates) + comisd x1 (the
    toLateralNorm macro) + comiss (the maxSpeed gate) - all correct;
    the __real@3e7ad7f29ffffffd pool reference in the TU =
    addUserDefinedWaypoint_new's OWN epsilon (1.0000001e-7f - a
    DIFFERENT float from 1e-7f, the original's own constant there,
    correct as written - NOT the checkAcceleration bug).  Gate: both
    TUs 0/0.  THE A/B RUN (the owner): build A -> run the repro ->
    rename checkaccel_log.txt -> checkaccel_log_A.txt; build B ->
    run -> _B.txt; diff - the first divergent row names the
    mechanism (the localization feeds the fix; if the rows are
    IDENTICAL and the desync persists, the divergence = outside this
    function's observable surface -> the replay-reference question).
    REMOVE THE HARNESS AT SHIP (the DIAG block + the installer flip +
    core.cpp:1395 per the final bake ruling).

  - CHECKACCELERATION A/B RESULT (owner-run 2026-11-22): the logs
    (C:\Games\Star Wars Galactic Battlegrounds\Game\checkaccel_log_
    orig.txt / _new.txt) - 30,088,422 ROWS ALIGNED IDENTICALLY (both
    builds; the CA rows and every FA row), then the FIRST DIVERGENCE
    at row 30088423 = an INPUT DIVERGENCE: unit id=27892 arrives with
    a different world_x (155.715027 vs 155.772797), a different
    command (the point 155.772797,30.6517925 vs 156.54895,29.8105793;
    limitedYawAngle 0 vs 1) - the same unit, the same velocity.y.
    VERDICT (CORRECTED after the owner's single-difference challenge -
    the builds differ ONLY in the checkAcceleration path, so an
    "already-desynced upstream" is IMPOSSIBLE - withdrawn): the world
    divergence at row 30088423 (unit 27892's position + the caller's
    args) MUST be the accumulated effect of the checkAcceleration
    path's difference - but the TRACED surface (every FA arg + every
    CA row) aligned through all 30M rows.  The divergence channel =
    UNTRACED: (1) my stub-flow slot model could have a residual error
    the static reading can't see - THE TEST: the A logs' FA args ARE
    the stub's actual delivered values; if the limitedYaw/inner FA
    args show the ACCELERATION constant (not -FLT_MAX), the var_54/
    var_58 mapping is inverted -> the fix = the swap; (2) the
    untraced side-effect channels: setForwardVelocity(0) (untraced),
    the movementData lazy-create difference (the movementless units
    on the limitedYaw path: the stub's garbage-bit arg can decline
    the create while our -FLT_MAX always creates - a presence
    divergence invisible to the already-logged rows), the precise-
    bracket float environment vs the original's (the denormal/
    precision semantics at the callees).  NEXT: the harness extension
    (the post-call movementData state in the CA row + the
    setForwardVelocity logger at 0x4A3A30) + the unit-filtered diff
    (id=27892) - both prepared for the next A/B run.
    The A/B harness rows: the CA/FA formats in the DIAG block; the
    log files in the game dir.

  - CHECKACCELERATION A/B RUN 2 ANALYSIS - THE TWO-TIER CONTRADICTION
    (2026-11-22): the first divergence = unit 11358's LABEL_11-arm
    acceleration.y write: the stub build = 0, ours = +FLT_MAX; the
    inputs identical.  OUR accelValue = the dispatched master
    acceleration proc's return = flt_656A10 = +FLT_MAX
    (binary-verified: [0x256A10] = FF 7F 7F FF = 0x7F7FFFFF).  THE
    STUB'S BUILD DELIVERED 0 = the STATIC-tier constant:
    nullsub_29 (the Static master vftable entry 21) = the ACTUAL
    BYTES (the binary read @0x422AF0): `fld dword ptr [0x006545A4];
    retn 4` - NOT an empty proc (the owner's correction: "loads
    float const and returns") - [0x6545A4] = 0.0f (binary-verified).
    = TWO DIFFERENT VFTABLE ENTRIES DISPATCHED FOR THE SAME UNIT:
    ours = the Moving-tier entry (the +FLT_MAX constant), the stub =
    the Static-tier entry (0.0).  THE ONLY MECHANISM: the unit's
    master_obj->vfptr POINTS AT DIFFERENT VFTABLES IN THE TWO BUILDS
    = the master_obj pointer itself diverged upstream (or points at
    heap/union state that differs).  THE CA ROW LACKS THE MASTER
    TIER/POINTER - THE EXTENSION PREPARED (not yet built): the CA row
    + master_type (@master_obj+4) + the master_obj pointer value +
    the accelValue/decelValue/maxSpeed logged - the next A/B run pins
    (a) our dispatch bug, (b) the upstream master_obj divergence, or
    (c) the patch's intended semantic delta.  ALSO: the FA logger =
    CLOBBERED (the M4's setForwardAcceleration_new line = the later
    installer write overwrote our FA logger hook at 0x4A3AC0 - FA=0
    in both logs); the ay1 field read in the CA row = the working
    observable.  THE FV logger = WORKS (the setForwardVelocity rows
    present).  Re-verify: the FA=0 counts; the master_type extension
    build.

  - CHECKACCELERATION DESYNC ROOT CAUSE (owner-found 2026-11-22,
    line 5811): the LABEL_11 velocity-vs-maxSpeed gate was written
    X87_LE_NAN ({<=, NaN}) - the original asm at 0x4A682E-0x4A6831
    = test ah,1 (the C0-ONLY mask, not C0|C3) -> the jz skips on
    C0=0 = {>= ordered} -> the fall-through to LABEL_11 fires on
    C0=1 = {<, NaN} = X87_LT_NAN.  THE DIVERGENCE CASE: velocity.y
    EXACTLY equals maxSpeed: the original SKIPS (C0=0, C3=1 - the
    C3 is IGNORED by the 01h mask), my form FIRED
    setForwardAcceleration (the LE_NAN includes =).  THE FIX:
    X87_LT_NAN (the owner's correction, applied + gated 0/0).  HOW
    IT SLIPPED THROUGH: at decode time I pattern-matched the
    fcomp+fnstsw+test+jz SHAPE to the 41h mask family without
    reading the IMMEDIATE BYTE (01h vs 41h = the entire
    partition); at /Fa verification I checked the compare width
    + the pool constants but never WHICH JCC FORM followed the
    compare (jbe for LE_NAN vs ja for LT_NAN).  THE TEMPLATE
    AMENDMENT WRITTEN (DECOMP_TEMPLATE.md, the TEST MASK IMMEDIATE
    rule): the x87 test mask IMMEDIATE (01h/41h/40h) IS the
    partition - read from the listing byte, not inferred from the
    idiom shape; the /Fa literal-row scan must include the JCC
    FORM at every x87 compare-branch site.  The epsilon constant
    (0.0000001f = the 1e-7f widening A0000000) was ALSO corrected
    (the .c's 22-digit decimal render = the truncated exact
    expansion - re-parsing it rounds to the NEXT DOUBLE DOWN -
    the binary read @file 0x2545B0 = A0000000 is the ground truth;
    the template's dd-constant rule covers this class).  The
    addMove oneWay prototype correction (the 10th param = unsigned
    __int8) is in the regenerated IDB; the addMove .c cross-checked
    (:174983).  The core.cpp:1395 stub line is ACTIVE for the
    A/B harness; the checkAcceleration hook = the logger (the
    DIAG block); the real line commented.  ALL BUILD GATES 0/0
    (RGE_Moving_Object.cpp + core.cpp, x86 v141_xp 14.16).

  - SESSION CLOSE + NEXT SESSION (paste as the opening prompt):
    "Read DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
    DECOMP_GAME_FACTS.md in cheatdll/src/idb/ first.  STATE: the
    checkAcceleration desync ROOT CAUSE = the LABEL_11 gate's
    X87_LE_NAN vs the original's test ah,1 (the C0-only mask -
    {<, NaN}, NOT {<=, NaN}) - the == case diverges.  FIXED to
    X87_LT_NAN (the owner's correction).  The epsilon constant
    (0.0000001f) was ALSO wrong (the .c's decimal render = the
    truncated expansion - re-parsing it rounds to the next double
    down); fixed to 0.0000001f = the 1e-7f widening A0000000
    (binary-verified from battlegrounds_x2.exe in the idb folder).
    The addMove prototype corrected (the 10th param = unsigned
    __int8 oneWay); the regenerated .c cross-checked.  The A/B
    harness (the DIAG block + the trampoline + the CA/FA/FV
    loggers + the wt/keyed id-filtered rows) is DIAG-marked for
    removal at ship.  core.cpp:1395 (checkAcceleration_patch) =
    ACTIVE for the A/B harness.  ALL BUILD GATES 0/0.  IMMEDIATE
    WORK: (1) the in-game validation of checkAcceleration_new
    (the LT_NAN fix + the 0.0000001f epsilon fix); (2) the A/B
    harness removal at ship (the DIAG block + the core.cpp:1395
    disposition per the final bake ruling - the CHECKACCEL_PATCH's
    semantic delta vs vanilla = the LABEL_11 arg swap: the stub =
    ACCELERATION, the vanilla = DECELERATION (-FLT_MAX); the
    limitedYaw/inner = the stub's var_58 vs the vanilla's the
    var_58 garbage - the owner rules the bake); (3) the M12
    drafts (teleport + addMove + resolveCollisions - the
    teleport's enter/exit field ruling owed; the resolveCollisions
    tail walk 0x4A5FC0-0x4A623B owed; the addMove oneWay in-game
    test owed); (4) Moving 97/97 close, then Action 38 / Combat
    48 / Static 133 / TRIBE_Combat 75 / TRIBE_Building 70 per
    the roadmap.  PROTOCOL: the two-pass bar (the template rule);
    the TEST MASK IMMEDIATE rule (the template, the checkAccel
    desync class); the dd-constant rule (the .c's decimal =
    the truncated expansion - ALWAYS the binary bytes for the
    constant transcription); the FA logger clobber note (the
    M4's line overwrites ours - the swap = the M4's line to the
    logger).  In-game validation owed for M9/M10/M11 + the
    checkAcceleration fix."

  - RUNTIME METHODS BATCH M12a - resolveCollisions + addMove (owner-approved
    + WRITTEN 2026-11-22): 2 hooks appended to RGE_Moving_Object.cpp
    (installer 94 -> 96 unique; Moving 96/97 - teleport is the last).
    resolveCollisions @0x004A5E70 [0x4A5E70,0x4A623F) (lst :282659/:283029,
    idb_functions.h:2196): the tail walk 0x4A5FC0-0x4A623B completed. Durable
    models: the distance norm is computed once and consumed UNROUNDED at the
    1e-7 short-circuit (fcomp st0, 0x4A5FF7 -> loc_4A6213 = *newPosition =
    positionValue, return the count) while the divisions consume the
    fst-rounded copy (var_74); pushScaleX stays UNROUNDED on st0 through the
    ladder, pushScaleY is fstp m32 store-rounded (var_88); the ladder's NaN
    partition: both-{<1e-7,NaN} -> skip; the take-Y arm = (X >= Y ordered ||
    X87_LE_NAN(X,1e-7)) && Y >= 1e-7 ordered with the inner skip on
    X87_LE_NAN(Y,1e-7) - a NaN X is REPLACED by an ordered Y and KEPT when Y
    is NaN/<1e-7 (case-verified against the asm); the AABB masks are MIXED:
    upper-vs-lower skips = test ah,1 ({<,NaN} -> X87_LT_NAN), lower-vs-upper
    skips = test ah,41h; jz ({> ordered} -> the literal MATCH row, a NaN
    overlap CONTINUES); the first-record guard = the recordIdx > 0 form (the
    asm's cmp ebx, offset obSystemCollisions flags-persisting jle); the
    dopple-swap = type-80 -> linked_owner @+0x1DC (idb_types.h:9862, the
    type-80 gate = the runtime-type evidence); the two copy-out position
    fetches = the runtime slot +0x2AC = 171 (idb_types.h:4106) into TWO
    distinct buffers (the asm's var_18/var_C); the maximumSpeed dispatch =
    the RUNTIME slot +0x188 = 98 (idb_types.h:4033 - DOUBLE-typed: the
    rule-(h) IDB residual is CLOSED for this dispatch, no re-typing request
    needed); the residual check's operator_minus takes newPosition ITSELF as
    the this_ operand (the out-param doubles as the minuend). addMove
    @0x004A30F0 [0x4A30F0,0x4A34AD) (lst :278064/:278438, idb_functions.h
    :2169 - the corrected 10-param prototype confirmed arg-for-arg): the SEH
    allocation-unwind chunk 0x642800 = operator_delete on the raw alloc
    (written operator_delete_internal, the M11 scoped-__try/__except form);
    arm A = the future-path adoption (the 0-fetch planar prefix gate, the
    waypointsCount live-reload copy loop, the waypointLevel-2 setReadyToPath
    vs the follow form + the range/targetID stamps, setIgnoreUnits from
    mIgnoreUnits @+0x30 / mNumberIgnoreUnits @+0x2C, the linkedPath adopt +
    detach); arm B = the fresh command (the -1 position prefix, the UDW loop,
    the count-1 last-waypoint planar tail gate, UNCONDITIONAL
    setReadyToPath + waypointLevel = 2 + waypoint = forcedWp); the flag
    masks = patrol bit 0 (BBitVectorAND_2 = dd 1 @0x65BD94), linear bit 2
    (dword_65BD98 = 2), oneWay bit 5 (dword_65BDA8 = 0x20 - the LOW-BYTE
    read, mov cl,[esp+58h+oneWay]) - all binary-verified; the two
    posDelta.z/tailDelta.z = 0 stores are REAL (dead values, the
    store-and-read diff); the arm norm orders DIFFER ((x^2+y^2) arm A vs
    (y^2+x^2) arm B). THE RULE-(j) GATE CATCH (the fresh emission audited
    BEFORE accepting the batch): the first gate's norms compiled
    mulss/addss + ?sqrt@@YAMM@Z (sqrtf) + a fstp reload - the 24-bit chain
    the x87 original never performed (the precise bracket does NOT widen
    float*float - the C abstract semantics for float operands is float;
    ONLY the explicit (double) casts produce cvtps2pd+mulsd) - fixed in
    BOTH functions (the two norms x2 in resolveCollisions + the two
    pushScale products; the addMove norms x2). Post-fix /Fa census:
    addMove = mulsd x4/addsd x2/cvtps2pd x4/__libm_sse2_sqrt_precise x2/
    sqrtf 0/comisd x2 + jbe (the {<=,NaN} skip = the asm's test ah,41h
    partition, the pool 3E7AD7F2A0000000)/no ss-ops; resolveCollisions =
    sqrt_precise x2/sqrtf 0/divsd x1 + fdivr x2 (the three single-rounding
    divisions)/mulsd x10/cvtps2pd x14/cvtpd2ps x2 (the distStored +
    pushScaleY store-rounds)/the addss x4 + subss x8 residuals = the
    rule-(d) single-op store-only forms (the AABB bounds + the pushOut
    deltas - the x87 form is exact-then-one-rounding, subss-identical)/
    comiss x10 + comisd x7 with the partitions case-verified incl. the
    Y == 1e-7-exact row/setcc 0/vtbl slots [reg+392] x1 + [reg+684] x2.
    GATE: 0/0 (x86 v141_xp 14.16). GATE CATCHES: three cross-class casts
    at the gate (master_obj -> RGE_Master_Static_Object*, the linked_owner
    -> RGE_Static_Object* result assignment, the createPath
    TRIBE_World-family cast - the C2440 class, fixed). SWEEPS: defs/refs
    96/96 exact (0 missing, 0 orphans); bare CRT 0 (the new region:
    operator_new_internal/operator_delete_internal only); bare base_game 0;
    pointer-walk clean (the one hit = ++collisionCount, an int counter);
    the 'new' grep hit = the word in the semantic comment (not code);
    return types match the prototypes x2 (int/int). Exit paths: addMove
    4 asm sites (0x4A315F/0x4A3190 return 0; 0x4A3489/0x4A34AD return 1 -
    the flag-tail duplication of one written return) <- 3 written returns;
    resolveCollisions 2 asm sites (the shared epilogue 0x4A6210 +
    loc_4A6213) <- 3 written returns (the epilogue mapping). Store-and-read
    diff: FULL MATCH both (addMove: pathData/waypointLevel/range/targetID/
    waypoint/linkedPath=0/creationTimeValue/flags x6; resolveCollisions: no
    object stores, the out-param copies). COLLISION SCAN (per-batch,
    mechanical, 1,170 active writer sites, F-variant shifted, comments
    excluded): 0 overlaps with either range; the two neighborhood hits =
    the // baked: citation comments on calculateFutureVelocity_new lines
    (not writers); the capstone's idb-constant CALL at RGE_Moving_Object
    .cpp:4989 (0x4A5B73) routes into the new hook per the routing ruling.
    INSTALLER DUPES FOUND (pre-existing, DIAG-era flip residue - NOT
    touched, needs a separate approval): set_angle_new x2 (0x4A13C0),
    setForwardVelocity_new x2 (0x4A3A30), setForwardAcceleration_new x2
    (0x4A3AC0) - each pair = the same E9 written twice (idempotent,
    behaviorally harmless); the proposal: remove one line per pair to
    restore the 0-dupes standard. THE TELEPORT RULE-(j) FORECAST: the
    teleport trail pipeline's mixed-precision chain (the decode-state
    model) will need the same widen-before-product audit at its draft.
    Remaining Moving 1: teleport 1319 (blocked on the enter/exit ruling).
    NEXT: teleport, then Action 38 / Combat 48 / Static 133 /
    TRIBE_Combat 75 / TRIBE_Building 70 per the roadmap. IN-GAME
    VALIDATION OWED: M9/M10/M11 + checkAcceleration + this batch.

  - RUNTIME METHODS BATCH M12b - teleport + THE DIAG REMOVAL + THE
    INSTALLER DEDUP - MOVING 97/97 COMPLETE (owner-approved + WRITTEN
    2026-11-22): 1 hook appended to RGE_Moving_Object.cpp (installer 96 ->
    97 UNIQUE, 97 active lines, 0 dupes). teleport @0x004A1440
    [0x4A1440,0x4A1967) (lst :274804/:275293, idb_functions.h:2138,
    double return). THE OWNER RESOLVED THE ENTER/EXIT BLOCKER: the IDB
    field names are CORRECT (+0 exit / +4 enter / +8 walk; walk_sprite_rate
    corrected to FLOAT in the regenerated idb - the .c pun gone, the DAT
    loader's fscanf("%f") corroborates); the regenerated .c AGREES with the
    asm on the offsets (the v30[1] one-past-pointer render = the +4 field
    - the earlier decode-state "one field for both" claim was against the
    pre-regeneration render; the ruling is naming-only, the offsets were
    always asm-pinned). THE GLOBAL NAMED (owner-approved): dword_68957C =
    walk_tile_particles_enabled (int* const @0x0068957C, common.h/cpp, the
    tuning-block family form) - gates the same-terrain grounded walk puff.
    DECODE MODELS (the full 1319-byte walk): (1) THE TRAIL ARM - the
    unit's OWN master's Moving extension fields gate it (obj_trail_options
    @228: 1 = the single departure puff, 2 = the interpolated trail; the
    initial test al,al guard collapsed per the redundant-guard rule - the
    dec-chain subsumes options==0); the trail master
    (owner->master_objects[obj_trail_id @226]) contributes ONLY the sprite
    (the master vtbl get_sprite slot +4, the M4 lowest-declaring-tier
    cast); THE SPACING DIVISOR = THE OWN MASTER'S obj_trail_spacing @232 -
    ecx holds [esi+14h] from 0x4A1449 through the fdiv at 0x4A14DA (the
    .c's v5->obj_trail_spacing corroborates); the count = ftol of the
    UNROUNDED quotient, viewed i16 (test bp/bp; the do-loop dec/jnz; the
    remainder fild re-uses the i16); the norm is the MIXED model - dx
    UNROUNDED on st0, dy/dz fst-store-rounded, the dz^2 product =
    unrounded-st0 x rounded-var_18 (the .c's v9*v55 same-name render is
    IDA's st0/memory blindness); the cursor steps = the m32-rounded
    fraction/dyStep/dzStep; the trail_remainder recompute =
    fild(count) x spacing; fsubr var_8 (the ROUNDED dist+remainder);
    fstp m32. (2) THE CLAMPS (gated on the runtime maximumSpeed slot
    +0x188 = 98 > 1e-7 - the literal > MATCH row, NaN skips): lower =
    radius + 0.01f (X87_LT_NAN - a NaN target IS clamped), upper =
    width - radius - 0.01f (the literal >= MATCH row) - map_width for
    BOTH axes (map_height never read). (3) THE BASE CALL:
    RGE_Static_Object__teleport (:3832, the constant-call form) - THE
    ORIGINAL STORE-ROUNDS ITS st0 RETURN THROUGH AN M32 (fstp [esp+30h+z]
    @0x4A16DF, fld at 0x4A195C) - replicated as (float)baseResult. (4)
    THE PARTICLE MATRIX - keyed on the PRE-call on_ground (var_20+3) and
    the PRE-call tile (ebx) vs the POST-call re-reads ([esi+28h] at
    0x4A16F0, the master terrain @+0x76 at 0x4A16E3): airborne-or-no-old-
    tile -> the arrival (row[newTerrain].enter @+4, the LIVE position);
    took-off -> the departure (row[oldTerrain].exit @+0, the SAVED
    position); same-terrain grounded -> the walk puff
    (*walk_tile_particles_enabled, the (world*rate) ftol grid vs oldX/
    oldY, flags 0 duration -1 max 8); differ grounded -> the arrival
    (enter, live) PLUS the departure (the QUIRK: the gate = the OLD
    terrain's exit_tile_sprite_id @+0, the spawned sprite = its
    enter_tile_sprite_id @+4 - two different fields, asm
    0x4A1797 cmp [ebp+0] / 0x4A17C4 mov edx,[ebp+4]; the confirmed-IDB-
    names comment at the site); world->sprites @+0x44, particle_system
    @+0x70, extra_terrain_info @+0x64 (the TRIBE_World pack-8 walk
    reconciles ALL of: world_time +0x10 / map +0x34 / terrains +0x60 /
    sprites +0x44 / extra_terrain_info +0x64 / particle_system +0x70 -
    three previously-grounded offsets + the asm). (5) THE SYNC (LABEL_47):
    positionValue = world_x/z/y (+0xF4/+0xF8/+0xFC) and lastMoveTimeValue
    @+0x124 = world_time, gated on fabs(delta) > 1e-7f on ANY axis (the
    literal > MATCH rows; a NaN delta skips in both; dbl_6545B0 =
    the widened 1e-7f). GATE: 0/0 after three rounds - the catches: a
    dropped step declaration (the C2065 slip), then THE RULE-(j)/(k)
    CLASS LIVE AGAIN: the dyStep/dzStep divisions compiled divss (the
    24-bit division = the double-rounding class; the x87 m32/m32 fdiv
    computes at 53-bit then one store-round) - fixed with the
    (double)-widened dividends -> divsd x4 total, divss 0. THE
    DOUBLE-ROUNDING COLLAPSE NOTE (added to the rule-(d) body of
    knowledge): a single +/- of two floats collapses
    (round24(round53(S)) == round24(S) for every exponent gap) so the
    addss forms are value-identical - the trail_remainder add and the
    cursor adds stayed float - but DIVISION never collapses (inexact) -
    every division must be divsd. Post-fix /Fa census: divsd x4,
    mulsd x8, addsd x6, cvtps2pd x31, cvtpd2ps x10, cvtsd2ss x2,
    sqrt_precise x1, cvttsd2si x5 (the trail count + the 4 grid ftols),
    comisd x8 / comiss 0 / setcc 0, the slots [reg+392] (maximumSpeed)
    and [reg+4] x2 (the master get_sprites), the constants 0x548CC0
    (add_particle2, :3739) / 0x54F820 (the Static teleport, :3832), the
    pools 3E7AD7F2A0000000; the 7 source add_particle2 sites merged by
    the compiler into shared tails (the codegen-neutral class - the
    original's own 0x4A18DF/0x4A18E2 shared tail reproduced). Exit
    paths: ONE asm return (retn 0Ch) <- one written return. Store-and-
    read diff: FULL MATCH (trail_remainder x2, lastMoveTimeValue,
    positionValue x3; no invented stores). THE DIAG HARNESS REMOVED
    (owner-directed): the whole DIAG block in RGE_Moving_Object.cpp (the
    g_checkAccelTraceOrig/Id/Log globals, the checkAcceleration_orig
    trampoline, the CA logger, the FV/FA loggers - 115 lines) DELETED;
    the zero-diagnostic-symbol sweep: g_checkAccel/_orig/_logger/val_log/
    LINE_LOGGING/g_dimu/fopen(/fprintf(/sprintf( = ALL 0 file-wide. THE
    INSTALLER DEDUPED (owner-directed): the three pre-existing duplicate
    hook pairs removed (set_angle_new x2 - the second line dropped;
    setForwardVelocity_new/setForwardAcceleration_new - the DIAG-section
    active copies dropped, the main-sequence lines retained); the
    checkAcceleration_new hook line PRESERVED (moved out of the deleted
    DIAG context into the main sequence). defs/refs 97/97 exact
    (0 missing, 0 orphans, 0 dupes); bare CRT 0; bare base_game 0;
    return types match the prototypes. COLLISION SCAN: 0 overlaps
    (1,170 sites). IN-GAME VALIDATION OWED: M9/M10/M11/M12a/M12b +
    checkAcceleration. MOVING 97/97 COMPLETE - the entire runtime Moving
    method surface owned and hook-routed. NEXT per the roadmap: Action 38
    / Combat 48 / Static 133 / TRIBE_Combat 75 / TRIBE_Building 70
    (smallest-first per class; the large bodies under the two-pass bar).

  - M12b CORRECTION - teleport_new TRAIL X-STEP (owner-reported visible
    particle/trail breakage, root-caused + fixed 2026-11-22): the written
    trail `fraction = dist / step` was WRONG - the st0 at the fdiv
    (0x4A14F4) = the dxU LEFTOVER from the norm computation (the __ftol at
    0x4A14E4 popped only the step; the dxU pushed at 0x4A149D was never
    stored and persists under the whole norm/fdiv/ftol sequence), so the
    division computes dx / step = THE X COMPONENT STEP (the .c's v67 =
    v8/world_x with v8 = dx confirms). The wrong form advanced the trail
    cursor's x by the EUCLIDEAN per-step distance (~spacing per particle)
    instead of the x component - x-dominant moves overshot the destination
    x, otherwise the trail bunched at the origin x (the visible in-game
    breakage). FIXED: dxStep = dx / step; the cursor curX += dxStep; the
    dy/dz steps were verified correct (their stacks were re-walked). THE
    LESSON (the turn_towards #7 class, second instance): the x87
    leftover-register misattribution - the operand-role verification
    requires tracking the stack slot-by-slot INCLUDING the values that
    persist PAST their looks-dead point; a leftover consumed pages later
    reads as a fresh value in every text-level net. ALSO DISPOSITIONED in
    the re-walk: the dxU parks across the main body and the walk-arm rate
    parks into the caller on the skip path - the ORIGINAL's own x87
    parking (the checkAcceleration parked-register class); benign for the
    replacement (no callee reads incoming x87 values; the written form
    balances). GATE 0/0; /Fa census unchanged in shape (divsd x4 =
    stepU/dxStep/dyStep/dzStep, all 53-bit single-rounding). Re-verify:
    teleport_new carries dxStep = dx / step and curX += dxStep (grep
    fraction = 0 in code).

  - M12b CORRECTION 2 - teleport_new TRAIL REMAINDER NEVER DECREMENTS
    (owner-reported: the visible breakage PERSISTED after correction 1;
    root-caused via the /Fa emission audit 2026-11-22): the written
    remainder recompute read `count` AFTER the do-loop, but the loop
    `while (--count != 0)` DECREMENTS count ITSELF - post-loop count = 0 -
    so the emitted form constant-folded to `mulsd spacing,
    __real@0000000000000000` -> trail_remainder = distR - 0 = distR: THE
    REMAINDER NEVER SUBTRACTS THE CONSUMED DISTANCE -> it crosses spacing
    on nearly every per-tick teleport (the caller = moveForward's vtbl
    dispatch) -> count >= 1 nearly every tick -> a particle per tick
    accumulating at the unit's positions = THE DENSE FLAME CLUSTER (the
    bad image). THE ASM: the loop decrements a THROWAWAY COPY (ebx =
    movsx bp at 0x4A1509) and the remainder reads the PRESERVED original
    (movsx eax, bp at 0x4A158C - AFTER the loop; fild var_4; fmul
    [edx+0E8h]; fsubr var_8). THE FIX: a dedicated `short remain = count`
    loop counter; the remainder keeps `count` (the preserved value).
    Post-fix /Fa: the ZERO-FOLD IS GONE - the emitted remainder = cvttsd2si
    count (pre-loop) / movd+cvtdq2pd (the signed short->double) /
    mulsd [edi+232] / subsd distR / cvtpd2ps / movss [esi+184] -
    bit-matching the asm. THE /Fa TELL (new census pattern): a constant
    fold to __real@0000000000000000 (the x0.0 multiply) = a DEAD-VALUE
    read proof - the gate census must include the x0.0-fold scan. THE
    LESSON (the loop-counter class): a post-loop use of the loop counter
    - the asm preserves the original in a separate register and
    decrements a copy; the C form must do the same (a dedicated counter)
    or the compiler PROVES the post-loop value is 0 and folds. GATE 0/0.
    Re-verify: grep teleport_new for `short remain = count` (= 1) and
    `while (--remain != 0)` (= 1); the /Fa has no
    __real@0000000000000000 in the teleport_new PROC.

  - M12b CORRECTION 3 - teleport_new TERRAIN-BLOCK CONTROL FLOW (the
    owner localized the remaining visible breakage to the terrain block
    and directed a re-creation; root cause = TWO MISSED RE-CHECKS in the
    landing arm; fixed 2026-11-22): the asm's loc_4A189E (the arm reached
    when the SAVED on_ground == 0, or the old tile == 0) carries TWO
    RE-CHECKS I missed in every prior pass: 0x4A189E-0x4A18A6 re-tests
    the LIVE on_ground (== 0 -> NOTHING), 0x4A18A8-0x4A18AA re-tests the
    OLD tile (== 0 -> NOTHING). THE TRUE DECISION TREE: the landing arm
    (row[newTerrain].enter at the LIVE position) fires ONLY for
    (saved-airborne AND live-grounded AND old tile exists); the
    saved-grounded + no-old-tile case lands in the same arm and is
    SILENCED by the re-checks; the saved-airborne + still-airborne ->
    NOTHING. MY FLATTENED FORM `if (saved != 0 && oldTile != 0) {walk /
    differ / took-off} else {arrival}` spawned the arrival puff for three
    cases the original leaves silent (notably saved-airborne + live-
    airborne and both no-old-tile cases) - for units whose on_ground
    flickers during moves (the elevation changes; the hover units) that
    is a puff per tick = the remaining visible cluster. THE .C HAD THE
    RE-CHECKS ALL ALONG (the regenerated render lines 173759-173761:
    `if (!this->on_ground) goto LABEL_47; if (!v17) goto LABEL_47;`
    inside the landing arm) - the decode missed them because the
    loc_4A189E region was skimmed as a simple arrival arm. RECREATED
    BLOCK: `if (savedOnGround == 0) { if (live != 0 && oldTile != 0)
    { the landing arrival } } else if (oldTile != 0) { if (live == 0)
    { the took-off departure } else { the walk arm / the differ pair } }`
    - the tree is now asm-exact (the .c render corroborates the entire
    shape). The differ-departure quirk and the walk grid unchanged.
    GATE 0/0; the /Fa emission verified: the landing arm's re-checks
    emit (cmp [esi+208],0 / cmp _oldTile,0 -> nothing), the grounded
    branch's no-old-tile path emits -> nothing; the census shape
    unchanged. defs/refs 97/97, 0 dupes/orphans; the diagnostic sweep 0.
    THE LESSON: the skimmed-locus class - an arm entered by MULTIPLE
    jump sources must have its INTERNAL gates re-read (they encode which
    sources actually spawn); the .c cross-check of a CONTROL-FLOW region
    is mandatory even when the value-level decode looks complete.
    Re-verify: teleport_new's terrain block carries
    `if (savedOnGround == 0)` / `else if (oldTile != 0)` and the landing
    arm's `this_->on_ground != 0 && oldTile != 0`.

  - SESSION CLOSE + NEXT SESSION (paste as the opening prompt): "Read
    DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
    DECOMP_GAME_FACTS.md in cheatdll/src/idb/ first. STATE: THE MOVING
    RUNTIME METHOD SURFACE IS COMPLETE - 97/97 exports owned and
    hook-routed (installer 97 unique, 0 dupes, defs/refs 97/97); the
    M12 batches (teleport @0x004A1440 + resolveCollisions @0x004A5E70 +
    addMove @0x004A30F0) WRITTEN, GATED 0/0, and ALL IN-GAME TESTS CLEAN
    (owner-confirmed 2026-11-22) - this closes the owed in-game
    validation for M9/M10/M11/M12a/M12b AND the checkAcceleration fix
    (the X87_LT_NAN gate + the 0.0000001f epsilon) AND the M12
    corrections 1-3 (the trail dxStep x87-leftover, the trail remainder
    loop-counter, the terrain-block landing-arm control flow). THE
    TELEPORT BLOCKERS ALL RESOLVED: the IDB enter/exit names RULED
    CORRECT (the departure-particle quirk = the gate exit @+0 / sprite
    enter @+4, commented at the site); walk_sprite_rate = FLOAT (the
    regenerated idb; the fscanf %f corroborated); dword_68957C = the
    approved binding walk_tile_particles_enabled (int* const @0x0068957C,
    common.h/cpp). THE A/B HARNESS REMOVED (the DIAG block in
    RGE_Moving_Object.cpp deleted - 115 lines; the zero-diagnostic-symbol
    sweep ALL 0 file-wide); THE INSTALLER DEDUPED (the three DIAG-era
    duplicate hook pairs removed; the checkAcceleration_new hook
    preserved in the main sequence). core.cpp checkAcceleration_patch:
    RETIRED (the line commented at core.cpp:1400 - the current tree runs
    checkAcceleration_new with the VANILLA semantics and the tests are
    clean) - the CHECKACCEL_PATCH arg-swap re-bake is MOOT (no bake;
    the vanilla semantics win; do not revisit unless the owner re-opens).
    THE BASE STATIC TELEPORT @0x0054F820 DECODED (unhooked; durable
    facts): re-tiles the unit ([esi+28h] = the destination tile at
    0x54F99D - so the pre/post tile reads in the Moving wrapper bracket
    the tile crossings); the OL delete/add; the OB refresh (the
    movement_type==1 -> OBSystem_Air split at 0x54F9D3); the 17-case
    z-interpolation switch over the new tile's type; the LOS mip words
    [esi+34h-3Ah]; the VIRTUAL slot +0xAC dispatch with the new z = the
    z-correction (for Moving units = OUR hooked
    handle_teleport_z_correction_new - the on_ground setter); NO writes
    to master->terrain or on_ground of its own. THE NEW PROTOCOL LESSONS
    (all in the correction entries above, carry forward): (1) THE X87
    LEFTOVER MISATTRIBUTION (the second instance): an st0 value pushed
    early and consumed pages later reads as a fresh value in every
    text-level net - the slot-by-slot stack walk must track values PAST
    their looks-dead point (the teleport trail's dxU; the turn_towards
    #7 precedent); (2) THE LOOP-COUNTER CLASS: a post-loop use of the
    loop counter - the asm preserves the original in a separate register
    and decrements a copy; the C form must use a dedicated counter or
    the compiler PROVES the post-loop value is 0 and folds (the /Fa
    tell: the x0.0 constant-fold = the dead-value read proof - ADD the
    x0.0-fold scan to the gate census patterns); (3) THE SKIMMED-LOCUS
    CLASS: an arm entered by MULTIPLE jump sources must have its
    INTERNAL gates re-read (they encode which sources actually spawn);
    the .c cross-check of a CONTROL-FLOW region is mandatory even when
    the value-level decode looks complete (the teleport landing arm's
    two missed re-checks; the .c had them at 173759-173761); (4) the
    rule-(d) collapse clarification: a single +/- of two floats is
    value-identical in addss (the double-rounding collapses), DIVISION
    never collapses - every division divsd (the M12a dyStep/dzStep
    catch). ALSO THIS SESSION: resolveCollisions_new (the ladder's NaN
    partition case-verified incl. the NaN-X-replaced-by-ordered-Y row;
    the unrounded-norm vs stored-copy split; the mixed AABB masks;
    the dopple-swap via the type-80 gate -> linked_owner @+0x1DC; the
    runtime vtbl maximumSpeed @+0x188 = 98 and position @+0x2AC = 171
    dispatches - the rule-(h) residual CLOSED, the slots are
    double-typed) and addMove_new (the corrected oneWay prototype
    arg-for-arg; the SEH chunk 0x642800 = operator_delete on the raw
    alloc; the flag masks 1/2/0x20 binary-verified; the arm norm orders
    differ). IMMEDIATE WORK: (1) the roadmap: Action 38 / Combat 48 /
    Static 133 / TRIBE_Combat 75 / TRIBE_Building 70 runtime methods,
    smallest-first per class (decode -> present -> approve -> write;
    the large bodies under the two-pass bar: size >= 4096 bytes, or
    gotos >= 12 AND back-edges >= 12, or the desync fallback, or the
    owner override); (2) the per-batch mechanical collision scan + the
    /Fa census (the x0.0-fold scan now included) on every batch;
    (3) the in-game validation per batch (the owner's standard test).
    CARRIED: the common.h collisionUnits comment fix (provisional - the
    rename verification stands: the .c's collisionUnits = our
    ignoreUnits). PROTOCOL: all edits approval-gated; the template rules
    (a)-(k) incl. the X87 macro mandate + the probe protocol + the
    precise brackets + the rule-(j) widen-before-product (proven
    load-bearing twice this session: the M12a norms and the teleport
    trail); the citation protocol (addresses + function names)."

  - RGE_Action_Object RUNTIME BATCH A1 (owner-approved + WRITTEN
    2026-11-22): 10 hooks — rehook @0x00408F10 [0x408F10,0x408F24),
    transform @0x00408F80 [..,0x408F99), destroy_obj @0x00408FA0
    [..,0x408FB6), set_action @0x004097E0 [..,0x4097F4), set_end_action
    @0x00409830 [..,0x409844), have_action @0x00409970 [..,0x409980),
    gameActionType @0x00409990 [..,0x4099A7), get_target_obj @0x00409A40
    [..,0x409A58), searchRadius @0x00409B70 [..,0x409B79), hasActionOfType
    @0x00409BD0 [..,0x409BE9). Groundings: actions @0x13C, master_obj
    @0x14, action_type @+4 (i16, movsx), search_radius @268=0x10C
    (RGE_Master_Action_Object total 300=0x12C; the twin master's
    move_sound @+0x124=292 corroborates the walk); RGE_ActionVtbl::
    get_target_obj member 20 = +0x50 (line-arith 15982-15962).
    Callees: RGE_Static_Object__rehook :3827 @0x54F4B0 + destroy_obj
    :3843 @0x5506B0 UNHOOKED (originals); RGE_Moving_Object__transform
    :2132 @0x4A1090 HOOKED (routes); RGE_Action_List__{rehook :60,
    delete_list :62, add_action :67, add_action_at_end :68, have_action
    :72, get_action :78, have_action2 :73} UNHOOKED. Compile gate 0/0
    (x86 v141_xp) + /Fa census clean: searchRadius kept the NATIVE x87
    form (fld DWORD PTR [eax+268]; ret - bit-identical to the original,
    no cvtss2sd), the get_target_obj dispatch emitted [eax+80] = +0x50
    verified, x0.0-fold scan 0, no ss-op->cvt chains; several wrappers
    compile to tail-jmp forms (behaviorally identical - the callees'
    own retn performs the cleanup). Exit paths mapped; store-and-read
    diff: object-field store sets empty both sides (the two
    _action$[esp-4] self-stores = compiler tail-call bookkeeping on the
    wrapper's own local); return types 10/10; casts 3/3 derived->
    ancestor cased; CRT/base_game/pointer-walk/loops all 0; installer
    18 unique addresses 0 dupes. Collision scan 0 overlaps (dead
    profiler table only, functionlist.cpp :286-:317). DEFERRED:
    play_command_sound @0x409610 / play_move_sound @0x409620 —
    INFORMATION REQUEST 2026-11-22 (owner-enforced after the illegal
    (RGE_Player*)this_ cast was caught at presentation): the master
    vtbl slots 17/18 type arg 2 RGE_Player* (idb_types.h:3602-:3603
    RGE_Master_Static_ObjectVtbl; :14874-:14875 RGE_Master_Action_
    ObjectVtbl) but every real argument in the binary is a runtime
    unit — the callees (0x488480/:1806, 0x4884D0/:1807) dispatch slot
    +8 OF THE ARGUMENT (asm 0x48849B/0x4884EB): on units that is
    gbg_get_civ_override (RGE_Static_ObjectVtbl :3937 / RGE_Action_
    ObjectVtbl :11600, int return feeding RGE_Sound__play player_id);
    on players it is set_game_status (:7129) - the master body itself
    cast-form-renders the mismatch. Caller census (only 4
    implementations exist): the two runtime wrappers (pass this), the
    unit-side dispatches, TRIBE_Screen_Game__do_button_action @0x4FB4F0
    (passes 0, .c:245740), TRIBE_Player__command_drop_off_resources
    @0x5D05E0 (passes a unit, .c:397521 - second pun render). NO
    caller passes a genuine RGE_Player. DEDUCED TRUE TYPE arg 2 =
    RGE_Static_Object* (the runtime-unit base; verified layout prefix
    in every runtime struct). Owner to re-type the slots and
    regenerate; the pair's decode restarts from the regenerated files.
    Re-verify: the regenerated vtbl members. ROADMAP: Action runtime
    38 -> 28 unowned (17/45 exports owned; installer 18). In-game
    validation owed (owner's standard test).

  - BATCH A2 + SOUND PAIR (owner-approved + WRITTEN 2026-11-22): A2 = 10
    hooks — set_only_action @0x409800 [0x409800,0x409823), stop @0x409850
    [..,0x40986E), change_ownership @0x409910 [..,0x409934), copy_obj
    @0x409940 [..,0x40995E), actionState @0x409B40 [..,0x409B65),
    get_action_checksum @0x409B80 [..,0x409BA8), actionType @0x409BB0
    [..,0x409BCF), hasSubActionOfType @0x409BF0 [..,0x409C12),
    get_next_selected_group @0x409EB0 [..,0x409ED7), get_num_Selected_
    groups @0x409EE0 [..,0x409EFE). Groundings: RGE_Action_ObjectVtbl::
    stop member 72 = +0x120, set_only_action member 231 = +0x39C
    (line-arith; NAMED dispatches, /Fa-verified [eax+288]/[eax+924]);
    RGE_Action_ListVtbl::copy_obj member 6 = +0x18 (/Fa [eax+24]);
    Selected_Group_Info movsx + sar/jle signed forms both sites; RGE_
    Action::state u8 @+0xC (movzx in the checksum, direct AL load in
    actionState), action_type i16 @+4 (movsx); actionState's EXACT == 1
    guard (cmp al,1 - literal transcription, not != 0). copy_obj's
    (RGE_Master_Action_Object*)source DOWNCAST owner-approved with the
    layout-prefix grounding (every arriving master tier carries the
    300-byte Action-master prefix; consumer 0x406EF0 = a forwarder to the
    queued actions' copy_obj; IDA's own render carries the same cast).
    Compile gate 0/0; /Fa census clean; defs/refs 28/28 unique at the A2
    gate; pointer-walk 2 benign int counters; CRT/base_game 0. Collision
    scan 0 overlaps (dead profiler rows functionlist.cpp :302-:321 only).
    IDB REGEN 2026-11-22 (type-only, in place; same line totals 37087/
    6415; the cited anchors re-verified UNSHIFTED): the master sound
    slots re-typed RGE_Player* -> RGE_Static_Object* per the Information
    Request (idb_types.h :3602-:3603 RGE_Master_Static_ObjectVtbl,
    :14874-:14875 RGE_Master_Action_ObjectVtbl, + all sibling master
    vtbls; master prototypes :1806/:1807); the master bodies now render
    obj->vfptr->gbg_get_civ_override(obj) NAMED-form (cast-form artifact
    gone). RGE_Action_List__copy_obj (:81) NOT re-typed - the approved
    downcast stands. SOUND PAIR WRITTEN (decode restarted from the
    regenerated files per the suspicion-gate protocol): play_command_
    sound_new / play_move_sound_new - named dispatches slots 17/18 (/Fa
    [eax+68]=+0x44 / [eax+72]=+0x48 verified), argument cast = the
    VERIFIED derived->ancestor (RGE_Static_Object*)this_, matching the
    post-regen rendering. File totals: 30 defs / 30 unique installer
    refs. Roadmap: Action runtime 38 -> 16 unowned. In-game validation
    owed for A1+A2 (owner's standard test).

  - BATCH A3 (owner-approved + WRITTEN 2026-11-22, PARTIAL - 3 of 7
    presented): setup @0x00408DE0 [0x408DE0,0x408E34), create_action_list
    @0x00408EB0 [0x408EB0,0x408F0C), save @0x00408F30 [0x408F30,0x408F79).
    Groundings: Moving setup :2127 @0x4A0940 + save :2129 (both HOOKED ->
    routes); the Action extension stores type@94(0x28=40)/waiting@320/
    actions@316/command_flag@321/Selected_Group_Info@322 (word); the
    create_action_list NAMED dispatch slot 236 = +0x3B0 (line-arith
    11834-11598 = 236; /Fa [eax+944]); RGE_Action_List alloc
    sizeof = 12 = 0xC (:11587-11591 vfptr/obj/list) via
    operator_new_internal + ctor :56 @0x4062B0 + the SEH fold chunk
    0x640A90 = operator_delete on the raw alloc (make_new_obj precedent;
    compiled to the standard __except_handler4 scope-table form); save's
    rge_write (:3005) x3 sizeof-exact 1/1/2 + RGE_Action_List__save (:61
    @0x406480 UNHOOKED). Compile gate 0/0 after TWO GATE CATCHES:
    (1) operator_new_internal returns void* - explicit cast added;
    (2) THE MISSED SUSPICION: set_task/setTaskByTaskID were presented
    WITH code dispatching RGE_Action_ListVtbl::create_task_action with 6
    stack args - the member (idb_types.h:15048; TRIBE twin :20880)
    declares only 5. The cast-form render in the .c (:52060/:52082) was
    the mismatch tell, missed at decode; the compile gate caught it
    (C2197 too many arguments). INFORMATION REQUEST #3: add the trailing
    `int` to create_task_action in both list Vtbls (the callees retn 18h
    = 24 = 6 stack args, asm 0x406AA2; every dispatcher pushes 6: task,
    obj, -1.0f, -1.0f, -1.0f, flag; the flag lands in RGE_Action_Gather::
    gbg_unknown_1 @+0x5C when nonzero - asm 0x406A29-0x406A31 - a
    game-facts registry item). DEFERRED on IR#3: set_task @0x409720,
    setTaskByTaskID @0x409780 (decode records complete in the session;
    restart from regenerated files). ALSO DEFERRED on IR#2 (work2
    member): work @0x409270, do_command @0x408FD0 - IR#2 AMENDED after
    the owner-directed width analysis: the corrected member form is
    (RGE_Action_Object*, RGE_Static_Object*, float, float, float, int
    work_flag, int certain_action) - the 7th param must be INT (bit-
    faithful): it is pushed raw-dword by every caller (asm 0x409281/
    0x408FE3), re-pushed raw-dword by work2 (asm 0x409513/0x40951D), and
    consumed as a 32-BIT truthiness test (asm 0x406A2D test ecx,ecx);
    the IDB's __int16 typing is narrower than the consumption. The 8th
    param (deduced name certain_action) is INT and consumed strictly
    16-bit (work2 gate cmp word ptr [esp+..],0FFFFh @0x4093BB;
    get_target_task mov ax + 16-bit cmps @0x5B9790-0x5B97A3, 0x5B97E4,
    0x5B983A, 0x5B98E5) - it feeds RGE_Task_List__get_target_task's
    certain_action slot (:4921/:5674; the filter is certain_action == -1
    || certain_action == task->action_type) with the 0xFFFF wildcard
    sentinel; the IDB name certain_action for the 7th is a misnomer
    (sole real consumption = the gather flag). Aligning the :135 extern
    prototype's 7th to int is an optional same-pass edit. /Fa census
    clean: slot [eax+944]; the save widths 1/1/2; the SEH scope table
    present. defs/refs 33/33 unique. Collision scan over all 7 A3
    ranges: 0 writer overlaps (canonical + dead profiler rows only).
    Roadmap: Action runtime 38 -> 13 unowned (18/45 owned; installer
    33). In-game validation owed for A1-A3.
  - PROTOCOL AMENDMENT (owner-directed lesson, 2026-11-22, the A3
    round): a CAST-FORM render at a VTBL DISPATCH site in the .c is the
    member-typing mismatch tell - it MUST trigger the suspicion gate at
    DECODE time (member param-count/width check vs the asm pushes and
    the callee retn), not merely be transcribed. The set_task round
    transcribed a 6-arg dispatch through a 5-arg member because the
    cast-form was read as an IDA artifact; the compile gate caught it
    (C2197). Every future vtbl dispatch decode: count the asm pushes,
    read the callee retn, count the member params - all three must
    agree before the named dispatch is written.

  - BATCH A4 (owner-approved + WRITTEN 2026-11-22): setup_read
    @0x00408E40 [0x408E40,0x408EA3), update @0x00409870 [0x409870,
    0x40990D), is_idle @0x004099B0 [0x4099B0,0x409A3F). Groundings:
    Moving setup_read :2128 @0x4A0A40 / update :2142 @0x4A1A00 /
    updateEnd :2143 @0x4A1AC0 (all HOOKED -> routes); setup_read's
    rge_read (:3003 @0x4D5550) x3 sizeof-exact 1/1/2 + the
    create_action_list slot 236 (+0x3B0, /Fa [eax+944]) + RGE_Action_
    List__load (:59 @0x4063B0); update's dispatch fan: list members
    inside_obj_update 3 (+0xC, /Fa [eax+12]) / update 4 (+0x10,
    [eax+16]), runtime set_task slot 228 (+0x390, [eax+912]),
    delete_list (:62) / remove_action (:69 @0x406CC0); default_task
    @+0x108 word (movzx + cmp cx,-1); object_state @+0x58,
    inside_obj @+0x2C, list @+8. is_idle: mGroupID @+0x88;
    RGE_Game_World__getUnitGroup (:6099 @0x624920) on owner->world
    (world @+0x8C VERIFIED by the alignment-corrected hand-walk
    :699-:727 - u8 checksum + 3 pad before mLineRatio; the walk
    cross-corroborates master_objects @+0x74); BUnitGroup::
    mCurrentCommand @+0x33C = 828 (hand-walk :3346-:3397 with
    BPosition = 48 = 4xBVector (:3247-3253), BPointLine = 68
    (:3271-3277, BPointLineElement = 56), BUnitTypeStatisticManager =
    444 (:3301-3305, BUnitTypeStatistic = 440 incl. int[50] x2);
    /Fa [eax+828] = the struct verified); the idle state set
    {0,1,2,13,3,14} (the asm jz chain); action_type 107 (word cmp
    6Bh) + get_target_obj (RGE_ActionVtbl slot 20, [eax+80]) +
    THE X87 SITE: targetX (:186 @0x40B040, double return) vs
    flt_6545C8 = dd -1.0 - test ah,40h / jnz = {equal, NaN} -> the
    X87_EQ_NAN macro (x87_nan.h :21); /Fa: comisd x2 + ja/jbe, the
    jbe unordered-taken (NaN -> idle) - byte-faithful, NO fold; the
    pool __real@bff0000000000000 = the exact widened -1.0 (dd-pool
    check pass). MACRO NOTE: X87_EQ_NAN evaluates its operand twice
    (inherent to the mandated !(x<c) && !(x>c) form) - value-safe,
    targetX is a pure field getter; recorded in the census. Compile
    gate 0/0. Collision scan over the 3 ranges: 4 writer-context
    hits ALL DEAD (test.cpp :1652/:1653 setHooks inside /*...*/,
    test.cpp :1755 //writeNops, worlddump.cpp :566 //setHook -
    diagnostic-era commented sites; 0 active, no bake-ins; the dead
    lines stay per the retirement rule). defs/refs 36/36 unique;
    bare CRT 0; uncased casts 0 (the dual-naming world cast
    (RGE_Game_World*) per the Combat(70) precedent). Roadmap: Action
    runtime 38 -> 10 unowned (21/45 exports owned; installer 36).
    DEFERRED (IR#2 work2 member + IR#3 create_task_action member,
    owner corrections in preparation): work, do_command, set_task,
    setTaskByTaskID. In-game validation owed for A1-A4.

  - BATCH A6 (owner-approved + WRITTEN 2026-11-22): do_command
    @0x00408FD0 [0x408FD0,0x409002) + work @0x00409270 [0x409270,
    0x4092A5) - the work2 forwarders, unblocked by IR#2 (the work2
    member corrected in all five runtime vtbls to (.., unsigned
    __int8, int, __int16) = 7 stack args; the extern :135 adopted the
    gather_flag naming: char work_flag, int gather_flag, __int16
    certain_action). Both: the obj==this guard, the work2 NAMED
    dispatch slot 227 = +0x38C (line-arith 11825-11598 = 227;
    /Fa [eax+908] verified x2), command_flag byte inc @+0x141.
    do_command passes the neutral set (0, 0, -1); work forwards the
    caller's gather_flag RAW dword (the width-verified bit-faithful
    int form) with work_flag 1. Restart-verified from the regenerated
    files. Compile gate 0/0; defs/refs 44/44 unique. Collision scan
    over both ranges: 0 overlaps (canonical + dead-profiler only).
    Roadmap: Action runtime 6 -> 4 unowned (27/45 exports owned;
    installer 44). REMAINING: work2 (848 - decode complete, the
    body/asm from the width-analysis round), setUnitAIAction (480 -
    decode pending). In-game validation owed for A1-A6.

  - BATCH A5 (owner-approved + WRITTEN 2026-11-22): actionTypeIfOrdered
    @0x00409010 [0x409010,0x40915B), move_to @0x00409160 [0x409160,
    0x409265), get_command_master @0x00409630 [0x409630,0x409710), getTask
    @0x00409A60 [0x409A60,0x409B3B). Groundings: the RGE_Task_ListVtbl::
    get_target_task NAMED dispatch member 1 = +0x4 (line-arith 6580-6579;
    /Fa [eax+4] at all six sites) with the 9-arg form (this, master, obj,
    x, y, z, certain_action, alt_task); tasks @+0x104 = 260 on the
    candidate masters (/Fa [ecx+260]); task_by_group @+0x11C = 284;
    the sibling loops gate on object_group word @+0x1E / master_type
    byte @+4 / task_by_group byte; the candidates re-typed
    (RGE_Master_Action_Object*) per the base-pointer rule (the .c's
    v11[1].los = 208 + 52 = +260 = tasks renders). actionTypeIfOrdered:
    *range = attributes[5] / attributes[28] (= +0xAC+0x70) tests, group
    set {18,9,10}, task->action_type word @+6 (RGE_Task, distinct
    struct), movsx int return; move_to: sizeof(RGE_Action_Move_To) =
    68 = 0x44 (:20756-:20776, the Action mirror + float range @64),
    the two Move_To ctors (:100/:101 = 0x408220/0x4082C0), SEH chunk
    0x640AB0 = operator_delete fold, action_move_to (:76 @0x406E40
    UNHOOKED), the type() dispatch slot 6 (+0x18) with the RESTRUCTURE:
    RGE_Action__type @0x40AC70 ends BARE-retn (caller-clean, reads only
    action_type) - the original's 4 pre-pushed args are setUnitAIAction's
    args 2-5 staged across the call; the written named forms are
    net-identical; setUnitAIAction named dispatch slot 234 (+0x3A8,
    /Fa [edx+936]) FULL 5-arg form, set_only_action slot 231 (+0x39C),
    command_flag byte inc; getTask: the transform dispatch slot 51
    (+0xCC, /Fa [eax+204]), the caller's certain_action word forwarded
    (callee consumes the low word only). GATE CATCH + /Fa CENSUS CATCH:
    the first draft mis-named the *range source work_val2 - the hand-
    walk inserted a phantom pad (attribute_type4 ends at 28, already
    4-aligned; work_val1@28/work_val2@32/work_range@36) and the /Fa
    emitted [eax+32] vs the original [ecx+24h]=36 - CORRECTED to
    task->work_range, re-gated, [eax+36] verified (the census
    store-and-read class working as designed). BAKED PATCHES x2
    (mandatory, call sites retired in place): conversion.cpp:167
    @0x00409104 - the group gate is is_class_melee_range_convertable
    (conversion.cpp:105, extern-declared: {9,10,18} + 57 at
    current_loaded_version >= 6) replacing {18,9,10}; buildingconv.cpp
    :48 @0x40912D - the zero-range block: ordered attributes[28] > 0 ->
    *range = 1.0f else *range = attributes[28] * *range (the stub's
    branch on the surviving test ah,41h flags; the literal > is
    unordered-false so NaN takes the multiply arm = the stub). Both
    retiree TUs re-gated (buildingconv.cpp / conversion.cpp, absolute
    paths). Compile gate 3/3 TUs 0 errors; /Fa census clean. defs/refs
    40/40 unique; bare CRT 0; bare base_game 0. Collision scan: the 2
    active sites baked (above) + canonical/dead-profiler literals only.
    Roadmap: Action runtime 38 -> 6 unowned (25/45 exports owned;
    installer 40). REMAINING: setUnitAIAction (480), work2 (848), +
    work / do_command / set_task / setTaskByTaskID (all four unblocked:
    IR#2 work2 member FIXED in all five runtime vtbls (:11561/:11825/
    :14136/:16695/:21294 - (.., unsigned __int8, int, __int16); IR#3
    create_task_action FIXED in both list vtbls (:15048/:20880 -
    trailing int); the owner adopted the gather_flag naming for work2's
    7th (:135 extern: char work_flag, int gather_flag, __int16
    certain_action)). In-game validation owed for A1-A5.

  - IR#3 CLOSED + SET_TASK/SETTASKBYTASKID WRITTEN (restart-verified
    2026-11-22): the owner's correction pass landed create_task_action
    with the trailing int in BOTH list vtbls (idb_types.h:15048/:20880;
    type-only, lines/totals unchanged). The two functions (code drafted
    + approved in the A3 round) restarted from the regenerated files per
    the suspicion-gate protocol: the regenerated renderings match the
    recorded asm decode (the create_task_action cast-form persists in
    the .c only as the dint-vs-float rendering; the member now accepts
    the 6-arg shape). WRITTEN + gated: get_task :1798 @0x488170 /
    getTaskByTaskID :1799 @0x4881A0 (UNHOOKED originals), have_action2
    (:73), the create_task_action NAMED dispatch slot +0x8 (/Fa [eax+8],
    the 6-arg form with bf800000 x3 = float -1.0 exact), set_only_action
    slot 231 (+0x39C). Compile gate 0/0; defs/refs 42/42 unique.
    REMAINING UNOWNED: work, do_command (unblocked by IR#2 - presented
    this round), work2 (unblocked by IR#2 + IR#3 - decode complete from
    the width-analysis round, presentation next).

  - BATCH A7 (owner-approved + WRITTEN 2026-11-22): work2 @0x004092B0
    [0x4092B0,0x409600) - the 848-byte order resolver, single-pass.
    Signature per the IR#2-corrected extern :135 (char work_flag,
    int gather_flag, __int16 certain_action); work_flag UNCONSUMED
    (zero reads of the +0x14 slot in the asm range - noted at the
    site). Structure: the own get_target_task dispatch (slot +0x4)
    accepted unless null or action_type == 7 (Attack; word @+6);
    the task_by_group sibling loop (the triple gate; candidates
    re-typed RGE_Master_Action_Object* per the base-pointer rule;
    the sibling dispatch forwards certain_action NOT -1; a hit with
    a DIFFERENT action_type -> the transform dispatch slot 51 =
    +0xCC); LABEL_18 fallback to the own task; the certain_action
    gate (no task AND certain_action != 0xFFFF -> return; the 16-bit
    cmp); the RE-DRIVE block (get_task :80 UNHOOKED, the current-task
    + action_type match + action_work :77 -> type(slot +0x18) ->
    setUnitAIAction (slot 234 = +0x3A8) 5-arg, worked = 1); the
    ATTACK-CONFIRM block (have_action == 1 exact guard; get_target_obj
    slot 20 == obj; type == 9; work slot 13 (obj,x/y,z) -> type ->
    setUnitAIAction -> unitAI(:261)->autoAttackOrderFlag = 0 ->
    return); the CREATE block (worked -> return; create_task_action
    slot 2 (+0x8, IR#3 6-arg) with gather_flag as the 7th (the Gather
    gbg_unknown_1 flag); group 1 (Herd Animal) + hasActionOfType(107)
    (slot 174 = +0x2B8) -> add_sub_action(:191, current, action, 1)
    -> return; else type -> setUnitAIAction -> set_only_action
    (slot 231) -> flag = 0 -> return); the SPEED FALLBACK (speed
    @+0xD0 = 208: fld/fcomp const_f_zero/test ah,41h/jnz -> the
    {>0 ordered} set runs the move path - the polarity MATCH row,
    the literal `speed > 0.0` kept (NaN -> skip = return, the
    maximumSpeed cap-gate precedent); setUnitAIAction(this, 1, ...)
    FULL 5-arg + the move_to NAMED dispatch slot 70 = +0x118).
    Scheduling RESTRUCTURE: the original pre-stages setUnitAIAction's
    args 2-5 across each type() call (the type callees are caller-
    clean bare-retn - RGE_Action__type @0x40AC70 reads only
    action_type); the written named forms build each frame at its
    own call site - identical callees/values/net stack. GATE CATCH:
    unitAI(this_) needed the (RGE_Static_Object*) ancestor cast x2
    (C2664), fixed. /Fa census: ALL FOURTEEN dispatch offsets
    verified ([eax+4] x8, [eax+24] x5 type, [eax+52] work,
    [eax+80] get_target_obj, [eax+204] transform x2, [eax+260]
    tasks x6, [eax+280] move_to, [eax+696] hasActionOfType,
    [eax+908] x2 (do_command/work), [eax+924] x4, [eax+936] x6) +
    autoAttackOrderFlag [eax+401] (the UnitAIModule walk :4391-:4444
    with ManagedArray_int_ = 16; mFormationType @0x190 immediately
    prior - the consecutive-byte corroboration) + speed [eax+208]
    comiss + the group-1 [ecx+30] cmp. Compile gate 0/0 (after the
    unitAI cast catch). Collision scan [0x4092B0,0x409600): 0
    overlaps. defs/refs 45/45 unique. Roadmap: Action runtime 4 ->
    3 unowned (28/45 exports owned; installer 45). REMAINING:
    setUnitAIAction (480 - the last; body + asm pulled, the decode
    in progress: the 12-entry actionType->(order,action) table
    {1->710/610, 3->717/617, 4->705/605, 5->709/609, 9->700/600,
    12->714/614, 101->702/602, 104->704/604, 105->703/603,
    106->718/618, 110->713/613, 111->715/615}, the UnitAIModule
    call family (:449/:450/:503/:508/:534), the position copy-out
    dispatch slot 171 = +0x2AC (M12-verified), unitAIValue @+0x7C).
    In-game validation owed for A1-A7.

  - RGE_MASTER_TYPE RETROFIT (owner-directed 2026-11-22, "go back to
    our owned tree and apply"): the runtime-side tier stores/gates
    still carried raw literals after the master-side ctor/setup tier
    was enum-ified on 2026-11-14. RETROFITTED (~40 sites, 9 TUs):
    the runtime type stores (RGE_ACTION_OBJECT type=40 x4 ->
    RGE_MASTER_TYPE_ACTION; Moving type=30 x2 + the spawned
    master_type >= 30 gate -> MOVING; Missile type=60 x5 -> MISSILE;
    Doppleganger type=25 x5 + found->type == 25 -> DOPPELGANGER;
    Combat type=50 -> COMBAT_BASE; Animated 0x14/20 x3 -> ANIMATED;
    the Static setups' master_type = 10 x4 -> STATIC) and the tier
    gates (the runtime type == 80 Building checks x8 in Moving +
    BMotion x1; Dopple master_type == 80 x2 -> BUILDING and >= 50
    x2 -> COMBAT_BASE; Static's master_type == 80 -> BUILDING;
    TRIBE_Master_Combat master_type >= 70 x2 -> COMBAT). The touched
    runtime files gained #include "RGE_Master_Static_Object.h"
    (BMotion: ../object/ relative). NOT retrofitted (distinct
    namespaces): the RGE_Armor_Weapon_Info::type 3/4 checks (the
    armor/weapon-type namespace), Moving's local type == 1/2 params,
    the comment-only mentions (comments now name the enums too).
    Compile gate 9/9 TUs 0 errors; /Fa codegen-identity verified
    per TU (the tier stores [reg+94] 40/30/60/25/50/20 and the
    master_type [esi+4], 10 stores + the 80/70/50/30 gates - all
    byte-for-byte identical to the pre-retrofit listings; the enums
    are pure renames). Also in this cycle: RGE_Action_Types.h (the
    three registry enums + RGE_ACTION_STATE, owner-supplied) and
    the registry retrofit above; DECOMP_GAME_FACTS.md gained the
    action_type / UnitAI-action / UnitAI-order (derived) /
    action-state registries.

  - ENUMS + REGISTRIES (owner-supplied 2026-11-22): NEW HEADER
    src/engine/object/RGE_Action_Types.h (the RGE_MASTER_TYPE precedent
    style) - RGE_ACTION_TYPE (31 values 1-149; 131 Offboard trade and
    132 Pickup unit per the owner's correction), RGE_UNITAI_ACTION
    (600-699, 31 values), RGE_UNITAI_ORDER (DERIVED, NOT
    owner-supplied: order = action + 100, all 12 setUnitAIAction table
    pairs asm-verified 0x409C57-0x409CE3; only the 12 table-reachable
    values enumerated). DECOMP_GAME_FACTS.md gained the three registry
    tables + the consumers notes + the RGE_Action::state idle-set
    PARTIAL entry ({0,1,2,3,13,14}: 1/2/3/13 align with Move to /
    Follow / Enter / Guard; 14 UNKNOWN - the state namespace is
    DISTINCT from action_type). RETROFIT (owner-directed): the written
    A1-A7 literals now use the enum names - work2 (ATTACK gate,
    ARTIFACT hasActionOfType, ATTACK2 confirm, MOVE_TO speed
    fallback), is_idle (ARTIFACT), actionTypeIfOrdered (CONVERT);
    /Fa re-verified CODEGEN-IDENTICAL (all immediates byte-for-byte:
    cmp [ecx+6],7 / cmp ax,9 / push 107 / cmp [esi+4],107 / push 1).
    NOT retrofitted (distinct namespaces): the is_idle state set
    literals (RGE_Action::state), work2's object_group == 1 (Herd
    Animal - the object_group registry). game-facts notations: 104
    Convert = actionTypeIfOrdered's attributes-range logic; 107
    Artifact = is_idle's placeholder check + work2's add_sub_action
    gate; 9 Attack2 = work2's confirm block. setUnitAIAction (A8,
    pending approval) will be written with the enum switch.

  - RGE_ACTION_STATE (owner-supplied 2026-11-22, the third registry):
    RGE_Action::state (u8 @+0xC) - 16 values None/Done/Wait/Search/
    Goto/Goto2/Work/Work2/Return/Turn/Delay/Move/Attack/Failed/
    Invalidated/MoveNoSearch; added to RGE_Action_Types.h +
    DECOMP_GAME_FACTS.md (the game-facts PARTIAL idle-set entry
    upgraded to CONFIRMED: the is_idle set reads None/Done/Wait/
    Failed/Search/Invalidated; update_new's removal pair reads
    Search/Done). RETROFIT: is_idle's six state literals + update's
    removal pair now use RGE_ACTION_STATE names (the update site
    carries the namespace inference - the list update propagates the
    current action's state - flagged in the site comment); /Fa
    re-verified codegen-identical (cmp cl,1/3, cmp al,3/1). NOT
    retrofitted: update_new's object_state <= 2 / == 2 literals (the
    UNIT object_state registry @+0x58 - a fourth namespace,
    game-facts PARTIAL). game-facts notations: the state registry
    also illuminates conversion.cpp's {7 Work2, 6 Work, 4 Goto}
    purge gate (unowned site).

  - BATCH A8 (owner-approved + WRITTEN 2026-11-22): setUnitAIAction
    @0x00409C20 [0x409C20,0x409E00) - THE FINAL ACTION EXPORT. Decode
    basis ASM-ONLY (owner-directed: the .c render of the
    addOrderToHistory argument block unreliable - overlapping
    by-value-BVector locals v10-v16 + the (int)v16 artifact).
    Groundings: module = unitAIValue @+0x7C = 124 (:7478); the
    111-case two-level switch (off_409E04 + byte_409E38, lst-verified)
    = 12 actionType -> {order, action} pairs (the enum-named switch;
    default keeps -1/-1); setCurrentOrder (:449, saveLast = 1) /
    setCurrentAction (:450); the OBJ PATH: setCurrentTarget2 (:503;
    obj->id @+0x10, movsx group @+0x1E), position(obj, &bufA) +
    position(this, &bufB) (slot 171 = +0x2AC, /Fa [eax+684] x2 - the
    copy-out getter, M12-verified; the .c's 1-arg cast-render at
    :52477 hid the out-buffer); targetID = obj->id staged by the
    LINGERING PUSH (asm 0x409D4D/0x409D54: push id, push out-buffer,
    the position callees pop only 4 - the id remains as the next
    call's targetID slot; the .c's (int)v16 was this slot);
    the NO-OBJ PATH: setCurrentTarget2(-1, -1, x, y, z);
    targetPosition = {y, y, y} (the setCurrentTarget2 arg-push
    leftovers {x, y, z} with x/z overwritten by y: asm 0x409DBD/
    0x409DC2, the middle slot the untouched y push - mechanical
    triple-confirmation; the .c's {x, y, z} v13 render WRONG);
    targetID = -1; position(this, &bufD). SHARED TAIL:
    addOrderToHistory (:534; order, action, own_position, target_id,
    target_position - by-value BVectors the compiler stages) +
    setTaskedByPlayer (:508); both paths join at loc_409DE7. GATE:
    0/0. /Fa census: [eax+684] x2, the five UnitAIModule constants
    (:449/:450/:503/:508/:534), the compiler-rebuilt byte-table
    switch (positional diff expected). defs/refs 46/46 unique;
    bare CRT 0. Collision scan [0x409C20,0x409E00): 0 overlaps.
    /Fa/ GAME FACTS UPGRADES this batch cycle: the action_type +
    UnitAI-action + UnitAI-order (DERIVED: order = action + 100)
    registries + the RGE_Action::state registry (all owner-supplied;
    the state PARTIAL entry CONFIRMED) -> RGE_Action_Types.h (new
    header, three enums) + DECOMP_GAME_FACTS.md; the written A1-A7
    literals retrofitted to the enum names, /Fa re-verified
    codegen-identical. ACTION RUNTIME PROJECT COMPLETE: 45/45
    exports owned (installer 46 hooks in RGE_Action_Object.cpp -
    45 exports + set_Selected_Groups). In-game validation owed for
    A1-A8 (the owner's standard test). NEXT: the roadmap continues
    Combat 48 / Static 133 / TRIBE_Combat 75 / TRIBE_Building 70
    runtime methods (smallest-first per class); the Action-family
    classes (RGE_Action + the 25 subclasses behind
    create_task_action's switch) are a candidate follow-on.

  - IN-GAME VALIDATION (owner-confirmed 2026-11-22): all tests clean
    on the current tree - A1-A8 + the four earlier runtime ladder
    batches validated together.

  - COMBAT RUNTIME OPENED (2026-11-22): 53 RGE_Combat_Object__ procs
    in the .lst; 5 owned (batch 1b-4: ctor/scalar-dtor/ctor_read/
    destructor/recycle_in_to_game); 48 remaining = the roadmap count
    EXACT. Smallest-first tiers established (sizes from the
    proc->endp extents).

  - BATCH C1 (owner-approved + WRITTEN 2026-11-22): 14 Combat runtime
    hooks - stop @0x00443DF0 [0x443DF0,0x443E0F), calc_attack_modifier
    @0x00443E10 [..,0x443E16), can_attack @0x00444AE0 [..,0x444AF8),
    reset_attack_timer @0x00444B00 [..,0x444B0F), get_weapon_range
    @0x00444C80 [..,0x444C9E), get_speed_of_attack @0x00444CB0
    [..,0x444CCE), moveTo3 @0x00445510 [..,0x445527), pause
    @0x00445F30 [..,0x445F4F), rateOfFire @0x00445F50 [..,0x445F59),
    weaponRange @0x00446090 [..,0x446099), minimumWeaponRange
    @0x004460A0 [..,0x4460A9), currentTargetX/Y/Z @0x00446150/
    0x00446180/0x004461B0 ([..,0x44617D]/[..,0x4461AD]/[..,0x4461DD]).
    Groundings: the base stop HOOKED (0x409850 -> routes);
    unitAIValue @+0x7C = 124; the stopObject NAMED dispatch
    (UnitAIModuleVtbl member 21 = +0x54, line-arith 7588-7567 = 21;
    /Fa [eax+84]) argument 0; UnitAIModule__removeCurrentTarget
    (:501 UNHOOKED); attack_timer @+0x174 = 372 (batch 1b-4 map);
    speed_of_attack @+0x150 = 336 / weapon_range @+0x144 = 324 /
    orig_weapon_range @+0x170 = 368 / orig_speed_of_attack @+0x174
    = 372 (the master walk); the can_attack X87 SITE (test ah,41h /
    jnz taken {<=0, NaN} -> the X87_LE_NAN macro; /Fa: comiss +
    setbe - the unordered-inclusive predicate, NOT folded); moveTo2
    slot 124 = +0x1F0 (/Fa [eax+496]) with the 1.0f default minimum
    range (the dd rule); the master get_sprite dispatch slot 1
    (+0x4; the Building-draw precedent) + the new_sprite slot 44
    (+0xB0, batch 1b-1); the currentTargetXYZ have_action == 1
    exact guards + targetX/Y/Z (:186/:187/:188) st0 passthroughs
    (/Fa: the -1.0 arm fld __real@bff0000000000000, the tail
    jmp targetX). Compile gate 0/0. Collision scan over the 14
    ranges: 0 overlaps (canonical + dead-profiler rows only).
    defs/refs 19/19 unique (5 pre-existing + 14 C1). Roadmap:
    Combat runtime 48 -> 34 unowned. REMAINING TIERS: C2 =
    release_being_worked_on (55 - notifies 699
    RGE_UNITAI_ACTION_RELEASED, the registry's first owned
    consumer), copy_obj (63), currentTargetID/ID2 (67),
    get_visibility (86), inAttackRange (107), teleport (114),
    notify (116), enter_obj (120), setup (120); then the larger
    tiers up to attack3 (857). In-game validation owed for C1.



  - BATCH C2 (owner-approved + WRITTEN 2026-11-22): 10 Combat runtime
    hooks - release_being_worked_on @0x00444CE0 [0x444CE0,0x444D17),
    copy_obj @0x004450C0 [..,0x445103), currentTargetID @0x004460B0
    [..,0x4460F3), currentTargetID2 @0x00446100 [..,0x446143),
    get_visibility @0x00446260 [..,0x4462B6), inAttackRange
    @0x00446020 [..,0x44608B), teleport @0x004461E0 [..,0x446252),
    notify @0x00445110 [..,0x445184), enter_obj @0x00444D20
    [..,0x444D98), setup @0x00443C20 [..,0x443C98).
    release_being_worked_on: the base Animated release + the caller->id
    >= 0 gate -> the notify dispatch (slot 120 = +0x1E0, /Fa
    [edx+480]) with (caller_id, this->id, 699
    RGE_UNITAI_ACTION_RELEASED, movsx group @+0x1E, 0, 0). copy_obj:
    the get_fight_sprite NAMED dispatch slot 33 = +0x84 on the OWN
    master (/Fa [eax+132]) vs the unit sprite @+0x1C, then on source2
    (cast RGE_Master_Combat_Object* per the Combat-tier identity; the
    .c's vfptr[1].get_death_sprite name = the vfptr[1] ARTIFACT - both
    asm calls hit slot +0x84 = get_fight_sprite per the CONCRETE
    vftable walk @0x0065AFF8 slot 33, .lst 989852 - the idb member 33
    = :11249 agrees) -> new_sprite (slot 44 = +0xB0); then the base
    Action copy_obj HOOKED. get_visibility: the map col/row convention
    VERIFIED (map_col = ftol(world_x @+0x48), map_row = ftol(world_y
    @+0x4C); the owned get_visible_new signature (map_col, map_row) at
    RGE_Visible_Map.cpp:13; /Fa cvttss2si [esi+72]/[esi+76]; the
    mutual-mask upgrade (mutualVisibleMask @+0x130 = 304,
    mutualExploredMask @+0x12C = 300; /Fa [edi+304]/[edi+300]);
    MUP @+0x16C = 364 gate + UMV @+0x168 = 360 mask (/Fa
    [esi+364]/[esi+360]); the result 15/0x80/0. inAttackRange: the
    distance_to_object DOUBLE return (:3871) captured into a float
    (the rule-(e) store-round); minimumWeaponRange slot 102 = +0x198
    (/Fa [eax+408]) with the fstp m32 store-round; weaponRange slot
    101 = +0x194 (/Fa [eax+404]) the st0 consumed UNROUNDED (the
    double capture); X87_LT_NAN(distance, minimum) flag (test ah,1 =
    the C0-only {<, NaN} set -> the negated-opposite form) and
    X87_LE_NAN(distance, weapon) && flag == 0 return (test ah,41h /
    jz = the {<=, NaN} proceed set - NaN proceeds, byte-faithful).
    teleport: the ftol old/new x/y (cvttss2si); the VUR_Ptrs[9]
    byte-patch loop pos_x @+6 / pos_y @+7 - VISIBLE_UNIT_REC
    :10338-10345 /Fa [edx+6]/[edx+7] x9; the .c int renders were
    IDA artifacts); the base Moving teleport HOOKED (:2133).
    notify: the object_state FILTER (== 2 pass any; == 0 pass
    mType == 500 only; else block - asm cmp al,2 / test al,al /
    cmp 1F4h); the module notify NAMED dispatch (UnitAIModuleVtbl
    member 5 = +0x14, /Fa [eax+20]) truthy -> return; the 699
    RGE_UNITAI_ACTION_RELEASED filter (asm cmp 2BBh); else the
    owner notify NAMED dispatch (TRIBE_PlayerVtbl member 75 =
    +0x12C, line-arith 1168-1093 = 75; /Fa [eax+300]). mType
    500 UNKNOWN (the registry candidate). enter_obj: the base
    Static enter_obj UNHOOKED; the inside_obj @+0x2C && id @+0x10
    gate -> the Update_Unit_Info call (the batch 1b-4 destructor's
    arg pattern: id, owner->id, ftol x/y, LOR @+0x16D, movsx
    group @+0x1E, UMV, 0, VUR_Ptrs @+0x144; the manager slot
    0x007A1CB8 = game_visible_unit_manager) + UMV @+0x168 = 0
    (/Fa [esi+360]). setup: the VUR_Ptrs[9] zero loop (_countof
    form, the ctor precedent) + the extension clears (MUP byte
    @+0x16C, UMV dword @+0x168, LOR byte @+0x16D, formation
    triple @+0x179/17A/17B, attack_count dword @+0x170 - ALL
    BEFORE the base call per the asm order) + the base Action
    setup HOOKED (:124); return 1.
    MAP COL/ROW/X/Y TEMPLATE AMENDMENT (owner-directed, the C2
    verify round): DECOMP_TEMPLATE.md gained the MAP COL/ROW/X/Y
    CONVENTION rule - the map-indexed getters take (map_col,
    map_row) in that stack order (map_col = ftol(world_x),
    map_row = ftol(world_y), the row = the OUTER dimension of
    unified_map_offsets); the IDA push comments are artifacts
    (the first-pushed = the last parameter). The verify anchor:
    the owned get_visible_new signature (map_col, map_row) at
    RGE_Visible_Map.cpp:13. The Combat get_visibility caller:
    push ftol(y) first, push ftol(x) second -> [esp+4] = world_x
    = map_col. GATE CATCH: RGE_UNITAI_ACTION_RELEASED needed the
    RGE_Action_Types.h include in RGE_Combat_Object.cpp (C2065),
    fixed. Compile gate 0/0. /Fa census clean. defs/refs 29/29
    unique (19 pre-existing + 10 C2). Collision scan over the 10
    ranges: 0 overlaps. Roadmap: Combat runtime 48 -> 24 unowned
    (29/53 owned; installer 29). REMAINING TIERS: C3 = save (136),
    get_armor (174), transform (178), damageCapability (180),
    setup_read (186), transport (227), do_attack (244), unload
    (251), calculateDamage (283), explore (295), moveTo2 (301),
    attack (385), moveTo (389), gather (390), damage (401),
    attack2 (487), attack4 (575), enter (589), area_attack (645),
    update (791), attack3 (857). In-game validation owed for
    C1-C2.

  - BATCH C3 (owner-approved + WRITTEN 2026-11-22): 5 Combat runtime
    hooks - setup_read @0x00443CA0 [0x443CA0,0x443D5A), save
    @0x00443D60 [0x443D60,0x443DE8), get_armor @0x00444B10
    [0x444B10,0x444BBE), transform @0x00443E20 [0x443E20,0x443ED2),
    damageCapability @0x00445F60 [0x445F60,0x446014).
    setup_read: VUR_Ptrs[9] zero loop + MUP/UMV/LOR clears BEFORE
    the base Action setup_read HOOKED (:125 @0x4A0A40 per the
    C1 grounding) + type @+94 = COMBAT_BASE + 8 extension reads
    sizeof-exact (formation_row @378, formation_col @379,
    formation_id @377, attack_timer @372 x4, capture_flag @376,
    MUP @364, LOR @365, attack_count @368 x4; /Fa [ebp+377..368]).
    save: the base Action save HOOKED + 8 extension writes sizeof-
    exact (the mirror of setup_read; /Fa [esi+378..368]).
    get_armor: the two-phase search - phase 1: the armor ptr
    (@+0x134 = 308) non-null + armor_num @+0x132 = 306 loop,
    type == 4 word cmp, the value @+2 word max; the -1 fallback:
    base_armor @+0x130 = 304 + the all-values scan (no type
    filter); then orig_armor @+0x16C = 364 (/Fa [ecx+364]).
    The element stride = 4 bytes (RGE_Armor_Weapon_Info).
    transform: the Update_Unit_Info bracket (PRE: PriorUMV = UMV,
    NewUMV = 0; POST: PriorUMV = 0, NewUMV = UMV; the game slot
    0x007A1CB8 = game_visible_unit_manager; /Fa [esi+360] UMV,
    [esi+365] LOR, cvttss2si [esi+76]/[esi+72] y/x, [eax+160]
    owner->id) around the base Action transform HOOKED (:126).
    damageCapability: the dual path - PATH A (module + attacker:
    canAttackUnit slot 19 = +0x4C (/Fa [ecx+76]), the calc_attack_
    modifier NAMED dispatch slot 95 = +0x17C (/Fa [eax+380]; the
    double return captured into a float local = the rule-(e)
    store-round, /Fa fstp m32), the calculateDamage dispatch on
    the ATTACKER's vtbl slot 59 = +0xEC (/Fa [edx+236]) with
    (weapon_num @+0x138 movsx, weapon @+0x13C, the float modifier,
    owner, this) - the return DISCARDED; PATH B (the weapon-sum
    fallback): the weapon array @+0x13C scanned for positive
    __int16 values (the x87 fild + fcom + test ah,41h / jnz ->
    discard = the {<=0, NaN} discard set - byte-faithful), the
    element stepped by 4 bytes (the RGE_Armor_Weapon_Info stride);
    the double result returned UNROUNDED per rule (h) (the /Fa
    fld qword result). GATE CATCHES: the calculateDamage dispatch
    needed (RGE_Player*)owner and (RGE_Static_Object*)this_
    ancestor casts (C2664 x2), fixed. Compile gate 0/0. defs/refs
    34/34 unique (29 pre-existing + 5 C3). Collision scan over
    the 5 ranges: 0 overlaps. Roadmap: Combat runtime 24 -> 19
    unowned (34/53 owned; installer 34). REMAINING: transport
    (227), do_attack (244), unload (251), calculateDamage (283),
    explore (295), moveTo2 (301), attack (385), moveTo (389),
    gather (390), damage (401), attack2 (487), attack4 (575),
    enter (589), area_attack (645), update (791), attack3 (857).
    In-game validation owed for C1-C3.

  - DESYNC FIX — damageCapability_new (owner-reported desync
    2026-11-22, ROOT-CAUSED + fixed): the TRUE path (canAttackUnit ==
    true) returned result (0.0) instead of the calc_attack_modifier's
    double — the `result = modifier` assignment was DROPPED in the
    first write (the .c's `result = this->vfptr->calc_attack_modifier`
    assignment missed; the modifier was captured into a local but
    result was never updated). The written code returned 0.0 on the
    TRUE path, causing the desync. FIX: `result = modifier;` added
    after the calculateDamage dispatch. Compile gate 0/0; /Fa
    verified the TRUE path now stores the modifier into _result
    (movsd _result, xmm0 after the calculateDamage call).
    LESSON: when a function has a `double result = 0.0` variable +
    multiple assignment paths, VERIFY each path assigns result
    before the return — the store-and-read diff has no return-value
    coverage (the diff tracks memory stores, not the C variable
    flow). A return-value flow check is needed for multi-path
    functions.

  - DESYNC FIX #2 - damageCapability_new (owner-reported second desync
    2026-11-22, ROOT-CAUSED + fixed): the TRUE path returned the
    calc_attack_modifier's double instead of calculateDamage's double.
    The ORIGINAL asm: calc_attack_modifier's st0 was POPPED by fstp
    (the float store), then calculateDamage was CALLED (its st0 = the
    damage), then retn 4 returned with calculateDamage's st0 UNTOUCHED
    - the transparent st0 passthrough. The .c's void cast-form render
    was the artifact (calculateDamage RETURNS double per the idb vtbl
    member). The .c's `result = calc_attack_modifier` + `return result`
    was the x87-tracking artifact (IDA assigned the st0 at the CALL
    site, not at the RETURN site). FIX: the calculateDamage call
    changed from a discarded statement to `return attackingUnit->
    vfptr->calculateDamage(...)` - the double return IS damageCapability's
    return. The prior `result = modifier` fix was SUPERSEDED (the
    modifier = the attack modifier, NOT the damage amount). Compile
    gate 0/0; /Fa: the ret 4 immediately after the calculateDamage
    call, no intervening st0/xmm management. PROTOCOL NOTE: the .c's
    void cast-form at a vtbl dispatch is a DUAL signal - (1) the member
    typing mismatch (the work2/create_task_action lesson) AND (2) the
    return type the .c doesn't propagate (this case). When the asm shows
    NO x87 management between the call and retn, the callee's return IS
    the caller's return - the .c's void/discard render must be
    cross-checked against the idb vtbl member's declared return type.
    Also in this cycle: the RGE_Master_Combat_Object vftable slot 33
    (+0x84) CONCRETE walk = get_fight_sprite (.lst 989852; the idb
    member 33 = :11249 agrees) - copy_obj's second sprite dispatch is
    get_fight_sprite, NOT the .c's get_death_sprite (the vfptr[1]
    render artifact).

  - SESSION CLOSEOUT (2026-11-22): all tests clean on the final tree.
    The Action runtime + the Combat runtime partial + the registry
    infrastructure are recorded above. The next session continues
    the Combat runtime (24 remaining) or opens Static/TRIBE_Combat/
    TRIBE_Building per the roadmap.
    NEXT_SESSION_PASTE_PROMPT:
"""
Read DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
DECOMP_GAME_FACTS.md in cheatdll/src/idb/ first. STATE: THE ACTION
RUNTIME IS COMPLETE (45/45 exports, installer 46 hooks, all tests
clean). THE COMBAT RUNTIME IS PARTIAL (34/53 owned, installer 34
hooks, all tests clean on the current tree; 19 remaining). THE
REGISTRY INFRASTRUCTURE IS IN PLACE: RGE_Action_Types.h (four enums:
RGE_ACTION_TYPE, RGE_UNITAI_ACTION, RGE_UNITAI_ORDER (derived:
order = action + 100), RGE_ACTION_STATE), DECOMP_GAME_FACTS.md (four
registry tables + the consumers notes). THE COMBAT REMAINING 19:
transport (227), do_attack (244), unload (251), calculateDamage
(283), explore (295), moveTo2 (301), attack (385), moveTo (389),
gather (390), damage (401), attack2 (487), attack4 (575), enter
(589), area_attack (645), update (791), attack3 (857) + is_invisible
(57) + get_weapon (170). AFTER COMBAT: Static 133 / TRIBE_Combat 75
/ TRIBE_Building 70 runtime methods, smallest-first per class. THE
FOLLOW-ON CANDIDATE: the RGE_Action family (the action subclasses
behind create_task_action's switch - the 25 RGE_Action_* classes
with their vtbl dispatches). PROTOCOL AMENDMENTS THIS SESSION:
(1) the cast-form tell at a vtbl dispatch MUST trigger the suspicion
gate at DECODE time (the member param-count/width check vs the asm
pushes and the callee retn); (2) the map col/row/x/y convention
(map_col = ftol(world_x), map_row = ftol(world_y), the row = the
OUTER dimension of unified_map_offsets; the IDA push comments are
artifacts); (3) when the asm shows NO x87 management between a
vtbl call and retn, the callee's return IS the caller's return
(the .c's void/discard render must be cross-checked against the
idb vtbl member's declared return type); (4) the return-value flow
check: when a function has a result variable + multiple assignment
paths, VERIFY each path assigns the result before the return. THREE
IDB CORRECTIONS THIS SESSION (all landed): IR#2 work2 member (7
stack args: .., unsigned __int8, int, __int16) in all five runtime
vtbls; IR#3 create_task_action member (trailing int) in both list
vtbls; the master sound slots re-typed RGE_Player* -> RGE_Static_
Object* (arg 2). IR REMAINING: NONE (all resolved). REGISTRY
CANDIDATES AWAITING OWNER SUPPLY: object_state (the unit lifecycle
state @+0x58 - PARTIAL: 0-2 the early lifecycle, >=7 the terminal
phase, 3-6 the settled range), mType 500 (the notify type),
RGE_Action::state 14 (the is_idle set), RGE_Armor_Weapon_Info::type
3/4 (the armor/weapon-type namespace), the UnitAI order/action full
ranges beyond the 12 table pairs. PROTOCOL: all edits approval-
gated; the template rules (a)-(k) incl. the X87 macro mandate +
the precise brackets + the rule-(j) widen-before-product + the
map col/row/x/y convention + the cast-form dual signal + the
return-value flow check; the citation protocol (addresses +
function names). CARRIED: the common.h collisionUnits comment fix
(provisional). DESYNC FIXES THIS SESSION: damageCapability #1
(the dropped result assignment) + #2 (the calculateDamage return
passthrough) - both fixed and gated. THE TWO-PASS BAR: size >=
4096, or gotos >= 12 AND back-edges >= 12, or the desync fallback,
or the owner override. THE TWO-PASS BAR CANDIDATES IN THE COMBAT
REMAINING: attack3 (857), update (791), area_attack (645) - all
NEED the mechanical gotos/back-edges check at decode-planning time.
"""

## SESSION CLOSE 2026-11-23 - COMBAT RUNTIME 51/53 - THE MISSION-BUILDER FAMILY COMPLETE

THE COMBAT RUNTIME CHAIN IS OWNED END-TO-END: attack -> attack2 ->
attack4 -> do_attack -> damage -> calculateDamage (the issue/apply/
compute split; the special-weapon effects gate is STRUCTURAL - the
applier applies, the callee computes), and the mission-builder family
COMPLETE: transport, unload, explore, moveTo2, moveTo, gather (the
task-layer variant: getTask builds the RGE_Task, the Gather action
wraps it), attack, plus the accessors: stopAction, is_invisible,
get_weapon (the VERBATIM ADOPTION of the objpanel patch), attack4
(the missile firer: the muzzle rotation, the AA masters, init_missile,
the notifies), area_attack (the splash dealer: the excess-distance
hit test, the roll gate, the level gate, the damage dispatch), enter
(the garrison builder: the AI garrison-request removal + the
work-interrupt cascade). 17 functions owned this session: 34 -> 51
hooks, all gates 0/0, all sweeps clean, all tests clean on the
written tree.

THE TWO-PASS BAR VERIFIED MECHANICALLY (the script; ALL 19 remaining-
then functions measured; the handoff's parenthesized numbers = the
proc->endp byte extents confirmed against the script): ALL 19
SINGLE-PASS (max 857 < 4096; max gotos 2; max back-edges 3). The
spot-verified: area_attack's 3 back-edges hand-checked; update's 1
goto hand-checked. THE TWO-PASS CANDIDATES FROM THE PRIOR HANDOFF
CLEARED (all single-pass).

THE BATCHES (all gates 0/0; the details in the code headers):
stopAction+is_invisible+get_weapon (3; the objpanel.cpp:400 collision
found by the scripted scan - the verbatim adoption per the owner
ruling + the collision naming rule); transport+unload (2; the
arg-form map: work(0,...) vs work(this,...)); do_attack (1; the
512/600 notify pair; the hit roll = (rand_internal()*100)/
RAND_MAX_INTERNAL - the 80010003h magic-imul = /RAND_MAX,
scripted-exact 32768/32768); calculateDamage+damage (2; the combined
batch - the special-weapon effects moved STRUCTURALLY to the applier
per the owner ruling: damage_new runs the special pass,
calculateDamage_new = the pure decode + the baked term-skip); enter
(1; the garrison builder); explore+moveTo2 (2); moveTo (1); gather
(1; the task-layer variant); attack (1); attack2 (1; the worker
transform via the owned transform_new); attack4 (1; the missile
firer: the muzzle rotation, the AA masters 992/836, init_missile,
the 512/600 + 527 notifies); area_attack (1; the splash dealer).

THE IDB CORRECTIONS APPLIED (2 regens): (1) gbg_unknown_2 ->
shield_recharge_accumulator, gbg_unknown_3 -> shield_hit_timer
(int -> float) across the runtime tree; (2) gbg_unknown_7/8/9 ->
gbg_garrisonUnits (std::vector<UnitPair>: _First/_Last/_End,
idb_types.h :2034-38) + the UnitPair struct {int first_id;
int second_id} (:4839) - the garrison-request list (the AI's pending
enter/garrison orders). THE STRUCT LAYOUTS: byte-identical (the
renames + the int->float same-size), no census rot. THE ENTER DECODE
RE-VERIFIED from the regenerated render (the typed garrison block:
tacticalAI.gbg_garrisonUnits._First/_Last, the UnitPair records
{first_id, second_id}).

THE SHIELD SUBSYSTEM DECODED (attribute 25 owner-supplied "Shield
recharge interval"): sp @+0x40 = the pool;
shield_recharge_accumulator (gbg_unknown_2) @+0x8 = the charge-tick
bank (gbg_doShieldRecharge 0x0054ED30: the ticks fire every
attributes[25] seconds, sp += the tier-scaled amount x attributes[10],
capped at master->hp; gbg_doShieldDropoff 0x0054EE70: the 1.0 s
ticks, sp -= attributes[26], floored at 0); shield_hit_timer
(gbg_unknown_3) @+0xC = the shield-hit flash timer (1.5f on the
shield-engaged hits, decayed per frame in update, consumed by the
draw layer as the yellow occluding-shape tint). THE GARRISON LOOP
CLOSED: the AI issues via taskToEnter (0x005AA530: operator_new(8) =
the {unit_id, target_id} record; the dedup scan; the
unitsTaskedThisUpdate bookkeeping; sendUnitAIOrder(player, id,
unit_id, 717, target_id, player->id, -1.0f x3, 1.0f, 1, 0, 100) -
717 = the ENTER order-value (617+100)); enter removes the fulfilled
record; the destructor frees the records then the array. THE 5XX
REGISTRY: 500 = UNDER-ATTACK (damage), 512 = do_attack/attack4's
issued-action notify, 527 = attack4's target-side under-attack (the
AI-owner gated), 530 = the owner-level unit-attacked notify.

PROTOCOL AMENDMENTS THIS SESSION (all owner-directed, all landed):
(1) THE VERBATIM ADOPTION (the entry-hook option (iii), the
get_weapon precedent): a colliding patch handler that is itself a
FULL REIMPLEMENTATION is taken VERBATIM as the replacement body (the
citations added; the old copy tombstoned). (2) THE COLLISION NAMING
RULE: the owned replacement takes the canonical _new; the old
handler renamed/removed with the references retired. (3) rand() ->
rand_internal() per the wrapper family; RAND_MAX_INTERNAL (consts.h)
- never the bare 32767/0x7FFF (the owned-tree sweep done). (4) THE
SPECIAL-EFFECTS GATE IS STRUCTURAL: the effects move to the applier
(the damage_new special pass), not sniffed at the callee. (5) The
two-pass mechanical check ran for ALL 19 (the table above).

GAME_FACTS ADDITIONS (written): the 5xx rows (500/512/527/530), the
shield trio rows (the renamed), the garrison-request list row, the
attribute indices (25 = the recharge interval owner-supplied, 10 =
the recharge multiplier, 26 = the drop-off rate), the master_id
candidates (83/293 = the worker transform targets; 992/836 = the AA
projectile masters - all owner designations owed).

DESYNC FIXES THIS SESSION: NONE (all gates held; the gate catches
were compile-level: the 3 work-dispatch casts, the world_x slip, the
specialDamage extern, the owner-cast dual-naming, the missing
allocated arg + the missing SEH fold in attack).

CARRIED: the common.h collisionUnits comment fix (provisional); the
// MAP_SIZE marker + the mapsize.cpp:1810 retirement for update's
batch.

THE COMBAT REMAINING (2): update @0x00444DA0 [0x444DA0, 0x4450B7)
(lst :120062-region post-regen) - 791 bytes - carries the // MAP_SIZE
marker + the mapsize.cpp:1810 retirement (the VisibleUnitManager
slot read); attack3 @0x00445510 [0x445510, 0x445527) - the 55-byte
moveTo2(1.0f) forwarder (lst :120418-region post-regen). AFTER THE 2:
THE COMBAT RUNTIME IS COMPLETE (53/53). THE NEXT FAMILIES: the
action runtime (the RGE_Action_* family behind create_task_action's
switch), RGE_Missile_Object::init_missile (0x0049BDE0 - live-cited
from attack4_new), gbg_doShieldRecharge/gbg_doShieldDropoff
(0x0054ED30/0x0054EE70 - the shield update side),
TRIBE_Command::do_command_attack_ground (0x005BB7C0 - attack2's
caller - the command layer).

THE NEXT-SESSION OPENER (paste-prompt):
"""
Read DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
DECOMP_GAME_FACTS.md in cheatdll/src/idb/ first. STATE: THE COMBAT
RUNTIME IS 51/53 - THE MISSION-BUILDER FAMILY COMPLETE, THE DAMAGE
CHAIN COMPLETE (attack -> attack2 -> attack4 -> do_attack -> damage
-> calculateDamage), THE GARRISON LOOP CLOSED (taskToEnter -> the AI
list -> enter removes), THE SHIELD SUBSYSTEM DOCUMENTED (attribute
25 = Shield recharge interval owner-supplied). REMAINING (2):
update @0x00444DA0 [0x444DA0, 0x4450B7) - 791 bytes - carries the
// MAP_SIZE marker + the mapsize.cpp:1810 retirement (the
VisibleUnitManager slot read); attack3 @0x00445510 [0x445510,
0x445527) - the 55-byte moveTo2(1.0f) forwarder. ALL SINGLE-PASS
(the two-pass bar verified mechanically: max 857 < 4096, max gotos
2, max back-edges 3). THE MISSION-BUILDER GROUNDING SET IS WARM:
the already-check -> re-target -> ctor -> delete_list +
set_only_action skeleton + the slots (get_target_obj 20/type 6/work
13/move_to 12/gbg_unknown1 14/getTask 233/set_only_action 231)
verified across 7 builders; the family precedents: transport/
unload/explore/moveTo2/moveTo/gather/attack/attack2 owned this
chain. PROTOCOL: all edits approval-gated; the entry-hook option
(iii) VERBATIM ADOPTION + the collision naming rule +
rand_internal/RAND_MAX_INTERNAL + the structural special-effects
gate + the two-pass mechanical check all landed this session-chain
(the details in the ledger). DESYNC FIXES: none this session (all
gates held). CARRIED: the common.h collisionUnits comment fix
(provisional); the // MAP_SIZE marker + the mapsize.cpp:1810
retirement for update's batch. THE TWO-PASS BAR: size >= 4096, or
gotos >= 12 AND back-edges >= 12, or the desync fallback, or the
owner override - the mechanical check: ALL remaining single-pass.
"""



## SESSION CLOSEOUT (2026-11-24/25): THE COMBAT RUNTIME COMPLETE (53/53) + THE ACTION SUBSYSTEM RATIFIED

WRITTEN THIS SESSION:
- RGE_Combat_Object::update @0x00444DA0 (batch C4, RGE_Combat_Object.cpp) - the
  UMV refresh, the capture scan, the AI tail; the installer at 51 hooks. THE
  MOVE_TO3 DISCOVERY: attack3 @0x00445510 = moveTo3, ALREADY ON DISK from batch
  C1 (re-verified this session; the stale comment line-arith refreshed).
- THE RGE_STATIC_OBJECT__UPDATE DESYNC FIXED (RGE_Static_Object.cpp
  update_new @0x0054EF00 + the shield pair gbg_doShieldRecharge/Dropoff
  @0x54ED30/0x54EE70): THE ROOT CAUSES = (1) the casts.cpp:892 processUnitHook
  ENTRY FIGHT (the UNIT_EXTRA per-frame processor frozen) - baked (the
  processUnitExtra call at the top of update_new, the // UNIT_EXTRA marker) +
  casts.cpp:892 retired; (2) the core.cpp:1431 combat_object_update_v2 interior
  hook (the entry+3, the byte overlap with my E9) - the v2 timer semantics
  baked (the X87_NE_ORD bits-nonzero gate, the negative-timer clamp) +
  core.cpp:1431 retired; (3) the negdecay retirement re-applied (the disturbed
  // prefixes); (4) core.cpp:1444 writeDword(0x65B89C, 0x444DA0) =
  compatible-live documented. THE SHIELD PAIR: the X87_LT_NAN tick-continue
  gate, the X87_LE_NAN accumulator gate, the cap/floor forms.
- THE ACTION SUBSYSTEM RATIFIED: src/engine/action/ (RGE_Action_List.cpp,
  RGE_Action.cpp, RGE_Action_Enter.cpp, install_action.cpp/.h + the vcxproj +
  the filters + install_engine() -> install_action() wired). THE ANCHOR:
  RGE_Action_List__create_task_action_new (707 B - the six derived-ctor
  groundings family-wide, the gbg_unknown_1 gather-tail resolution). THE
  RGE_ACTION BASE 25/25 (A1 the ctor family 6; A2a create_action_list/rehook/
  save/type/inside_obj_update/update; A2b set_target_obj/2 + copy_obj +
  get_state_name + targetX/Y/Z + setSubAction + hasSubActionOfType +
  add_sub_action + get_target_obj/2; A2c set_state @0x5724C0 the outlier).
  THE RGE_ACTION_ENTER 7/10 (A3a the ctor family 4 - the LINKED-BUILDING
  REDIRECT via linked_owner @476; A3b first_in_stack + set_state_move +
  set_state).

THE PROTOCOL AMENDMENTS (DECOMP_TEMPLATE.md):
- THE FIELD-PUN RULE - THE REANALYSIS-FIRST LADDER (the origin: the
  records[1].Object pun; the base-pointer rule INCLUDING THE VALUE'S TYPE).
- THE SCAN MECHANICS AMENDMENT (the all-hex-literal extraction, the
  installer-file owned inventory, the ENTRY FIGHTS rule, the counts-stated
  rule; the origin: the 0x0054EF00 processUnitHook miss).
- THE LOGGER RECOVERY RULE (recover the exemplar from commit 2fc06f1 BEFORE
  writing any logger; the one-line incremental shape; the one-switch flip
  RSO_UPDATE_LOG_NEW).

THE IDB VERIFICATION (owner 2026-11-25): the virtual struct declaration is
CORRECT - the 6-arg moveTo at +0x1EC (slot 123) verified directly in the IDB.
MY earlier extra-entry/shift theory (the runtime vftables vs the structs) =
DISPROVEN - my .h line-arith reading was the error. NO IDB correction, NO
regeneration. THE ENTER set_state writes NOW with the named 6-arg moveTo
dispatch through the struct member at +0x1EC (the explicit-this form, the
target up-cast to RGE_Combat_Object* - the verified ladder).

THE GAME_FACTS (written): the object_state machine row (0/2 -> 3 -> 6/7 -> 8;
the container mirroring; 1/4 UNKNOWN; the death threshold hp < 1.0);
sound_id 38 = the capture fanfare. THE CANDIDATES OWED: the notify mTypes
505/506, the attribute types 15/16/17, the master id 82, the msgs
3051/3065/3066/3069.

THE DESYNC FIXES THIS SESSION: the RGE_Static_Object__update (the UNIT_EXTRA
freeze + the v2 timer revert - the bakes above). ALL TESTS CLEAN
(owner-verified in-game). THE GATE: every batch compile clean (standalone
19.16 + in-tree PCH; the environmental CrashRpt misses only); the /Fa gates
passed (the X87 macro forms unfolded; the dispatch offsets 0x1EC/0x140/0x13C/
+0x60/+0x48/+0x1F0-as-+492-decimal/+332/+96 verified); the corrected scans
clean per range (the only LIVE hits = my own hooks + the documented-compatible
writeDwords); the A/B logger removed at ship.

CARRIED: the common.h collisionUnits comment fix (provisional); the
DECOMP_HIERARCHY Building-row rot (the re-walk owed); the Enter A3c remainder
(update @0x403530 544 B, enter_target @0x403750 752 B, calculateTargetPoint
@0x403A40 - the .c bodies ALL read); the in-game validation owed (the combat
tick, the shield pair, the enter flow, the capture).

THE NEXT FAMILIES: THE ACTION RUNTIME CONTINUES - the Move_To tier next (the
create_task_action + the Enter set_state_move call its ctors via the idb
constants - routes when owned), then the derived tiers, RGE_Action_List and
RGE_Action_Object last. THE CLASS COUNTS: the earlier scoping double-counted
the prototype block - USE THE MARKER-SCAN COUNTS (e.g. Enter = 10 not 20).
ALSO: RGE_Missile_Object::init_missile (0x49BDE0, live-cited from attack4_new),
TRIBE_Command::do_command_attack_ground (0x5BB7C0).

THE NEXT-SESSION OPENER (paste-prompt):
"""
Read DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
DECOMP_GAME_FACTS.md in cheatdll/src/idb/ first. STATE: THE COMBAT RUNTIME
53/53 COMPLETE (the RGE_Combat_Object::update + the static
RGE_Static_Object::update owned; the update desync FIXED - the UNIT_EXTRA
bake + the v2 timer bake, the casts/core/negdecay retirements; the owner
verified in-game). THE SHIELD PAIR OWNED (gbg_doShieldRecharge/Dropoff). THE
ACTION SUBSYSTEM RATIFIED (src/engine/action/, install_action wired through
install_engine): the anchor create_task_action + THE RGE_ACTION BASE 25/25 +
THE RGE_ACTION_ENTER 7/10 (the ctor family with the linked_owner redirect +
first_in_stack + set_state_move + set_state - set_state's moveTo dispatch =
the named 6-arg moveTo through the struct member at +0x1EC (the
owner-verified IDB declaration - correct as-is; the dispatch writes NOW with
set_state, un-blocked). REMAINING IN ENTER:
update/enter_target/calculateTargetPoint (A3c - the .c bodies read). THE
NEXT: the Move_To tier (the create_task_action + the Enter set_state_move
call its ctors via the idb constants - routes when owned), then the derived
tiers; RGE_Action_List (56) and RGE_Action_Object (90) last. THE CLASS
COUNTS: the earlier scoping double-counted the prototype block - use the
marker-scan counts (e.g. Enter = 10 not 20). THE
GAME_FACTS: the object_state machine row + sound_id 38 written; the
candidates owed: the mTypes 505/506, the attribute types 15/16/17, the
master id 82, the msgs 3051/3065/3066/3069. THE PROTOCOL: the FIELD-PUN
REANALYSIS-FIRST LADDER + the SCAN MECHANICS (the all-hex-literal
extraction, the ENTRY FIGHTS, the counts stated) + the LOGGER RECOVERY RULE
(commit 2fc06f1 exemplar) all landed this session-chain. THE GATE: all
batches compile clean (the environmental CrashRpt misses only); the /Fa
gates passed; the corrected scans clean. CARRIED: the common.h collisionUnits
comment fix (provisional); the DECOMP_HIERARCHY Building-row rot (the re-walk
owed); the in-game validation owed (the combat tick, the shield pair, the
enter flow, the capture). THE TWO-PASS BAR: all this session's functions
single-pass (max 912 B set_state).
"""

## SESSION CONTINUATION (2026-11-25): BATCH A3c - ENTER COMPLETE 10/10

- BATCH A3c - ENTER COMPLETE 10/10 (owner-approved + WRITTEN 2026-11-25):
  update @0x00403530 [0x403530,0x40370A) (the Enter vtbl slot 10 = +0x28,
  lst :4402/:4646), enter_target @0x00403750 [..,0x403A33) (lst :4673/:4969),
  calculateTargetPoint @0x00403A40 [..,0x403D23) (lst :4978/:5266) - the
  marker-scan count confirmed (10 exports; the installer 10 hooks, 0 dupes;
  defs/refs 10/10). ALL SINGLE-PASS (474/739/739 B; gotos 0/3/2; back-edges
  0/0/0). DURABLE MODELS: the WAIT-timer mixed precision (fsub at 53-bit ->
  fst store-round -> the compare consumes the UNROUNDED st0 = the
  double-diff + float-field form, rule (a)/(e)); the enter_target
  worker-deposit block (group 58 + THE BAKED HARBOR_DROP_OFF extension
  group 14 (Utility Trawler) && current_loaded_version >= 8 - harbor.cpp
  :316/:376 setHook 0x0040375F retired, the stub's signed jge; attribute
  types 15/16/17 bank as index 0, types 0-3 as themselves, anything else
  skips; the deposit = (int)((double)amount + 0.5) rule (f), the field
  re-stores, add_attribute_num (slot 32 = +0x80, the UPDATED float per the
  declared float param - the .c's LODWORD render = the artifact) +
  trackUnitGather (slot 24 = +0x60, the member is (int,int,int) - the .c's
  (__int64) render = the artifact) + set_attribute (slot 62 = +0xF8,
  (-1, 0.0)); master id 82 excluded alongside Turret 10/Fortress 9 - the
  registry candidate); the building-hp gate = master hp (i16 @+0x32) x 0.2f
  (flt_65479C BINARY-VERIFIED CD CC 4C 3E) >= target hp @+0x3C - the literal
  >= MATCH row (jnz targets the {<,NaN} skip); the 505/506 notify pair (505
  = the blocked-enter notify: the cant-do chain + the abandoned-action
  cleanup + the hp refusal; 506 = the SUCCESS capture notify; both p1 =
  RGE_UNITAI_ACTION_ENTER 617 - THE 6XX ENUM APPLIED per the owner
  direction + the two shipped-site retrofits in first_in_stack/set_state;
  505/506 stay cited literals - the 5xx enum waits per the 2026-11-23
  directive); the actionState slot (112 = +0x448 decimal) returns the
  RGE_Action::state namespace - THE ENUM APPLIED at all 8 compare sites
  (the application caught a decode-comment mislabel: case 3 was annotated
  "sub Work2", the registry says 3 = Search); the calculateTargetPoint
  floor = THE REAL CRT FLOOR compiled to MSVC's inline branchless SSE2
  expansion (subsd/cmpnlesd/andps/subsd + addsd 0.5 + cvtpd2ps -
  value-exact, NaN-propagating; the .c's "floor(v11); *tX = v11 + 0.5"
  render DROPS the floor - asm wins; the intrinsic-expansion class, the
  __libm_sse2_sqrt_precise precedent); the lingering-push disposition:
  enter_target's push -1 before the this-only gbg_get_civ_override (slot 2
  = +0x8) is RGE_Sound__play's OWN culture arg (the 5-stack-arg prototype
  exact - play(:3662) = (playType 1, pan, volume, player_id, culture -1));
  gbg_unknown_3 (the world sound index) @+0x19C = 412 walker-verified (the
  tasks @0x18C anchor); RGE_Player::sel_count @+0x26C = 620
  walker-verified; RGE_Player::world is TRIBE_World*-TYPED (the family
  cast required at the RGE_Game_World__object sites - the compiler
  corrected the ledger's RGE_Game_World* read; the world->gbg_unknown_3/
  sounds/map accesses resolve through TRIBE_World directly). THE COLLISION
  SCAN (2,961 sites / 2,603 literals / the 479-address owned inventory):
  1 hit dispositioned = the bake above; update/calculateTargetPoint
  ranges 0 hits. GATE (x86 v141_xp 14.16): 2/2 TUs 0 errors 0 warnings
  (RGE_Action_Enter.cpp + harbor.cpp); the gate catches = 4 C2664
  cross-class casts (the world dual-naming x2, the get_3d_sound_info
  this-cast, the change_ownership TRIBE_Player* -> RGE_Player* - the C2
  class). /Fa CLEAN: all slot offsets verified (448x8 = the asm's exact 8
  actionState calls, 316x2 = the two get_action fetches, 72x2
  display_cant_do_msg = the original's own shared-tail structure, 88x4 =
  the three set_target_obj + the object_state byte at the same decimal
  offset); the 2 X87_LE_NAN expansions emit comisd + direct CF jcc, 0
  setcc; the 0.2f widened pool present; the x0.0-fold scan = 0 folds (2
  legitimate macro operands); the ABI proof = _this_$ = ecx everywhere, 0
  esp-relative this reads; all 8 idb constants in the asm-direct-call
  form incl. 0x403750 = update's enter_target call through the HOOKED
  CONSTANT (the routing ruling). IN-GAME VALIDATION OWED (the enter flow,
  the capture, the worker/trawler deposit). GAME-FACTS CANDIDATES OWED
  (owner designations): master id 82 (the deposit exclusion),
  msg 3051 (the building-hp refusal), the 505/506 pair (delivered
  in-conversation), group 14 at v8+ (the trawler deposit). NEXT: the
  Move_To tier (the create_task_action + the Enter set_state_move/set_state
  call its ctors :100/:101 @0x408220/0x4082C0 via the idb constants -
  routes when owned), then the derived tiers; RGE_Action_List (56) and
  RGE_Action_Object (90) last. CARRIED: the common.h collisionUnits
  comment fix (provisional); the DECOMP_HIERARCHY Building-row rot (the
  re-walk owed); the in-game validation owed (the combat tick, the shield
  pair, the enter flow, the capture - joined by this batch).

- DESYNC FIX - RGE_Action_Enter__update_new (owner-reported bisect +
  root-caused + fixed + CONFIRMED FIXED IN-GAME 2026-11-25): the outer
  switch dispatched state Goto(4) into loc_403628 (byte_403724[3] = 2 -
  the enter driver arm: the target gates -> the sub-action update -> the
  distance/enter_target attempt -> the mFirstTimeEnter re-drive); the
  first write labeled the arm case Search(3) - THE PARTITION INVERTED
  (Search ran the driver, Goto returned 0) -> the enter flow never
  advanced from the GOTO state (the state the WAIT-timer expiry and the
  first-in-stack re-drive park it in). FIX = the one case label
  (RGE_ACTION_STATE_SEARCH -> RGE_ACTION_STATE_GOTO at the loc_403628
  arm) + the header note (Search itself falls to default,
  byte_403724[2] = 5). The failure chain: the raw byte-table decode note
  had [3] -> loc_403628 CORRECT and the .c renders case 4u plainly
  (:47180) - the transcribed enum label was written wrong; the enum
  application (which landed post-write) would have exposed the
  label-vs-semantics mismatch at write time. The strict single-function
  emission walk found NO other divergence (the WAIT-timer bit-exact
  form: movss/cvtps2pd x2/subsd in order -> comisd on the UNROUNDED diff
  -> cvtpd2ps/movss store-round -> ja stay; the distance capture = the
  fstp-qword corollary-(h) form, comisd vs the widened range, jbe -> the
  enter path; the enter_target call through the hooked constant 0x403750
  with ECX liveness proven across the hoist; all dispatch offsets
  88/92/96/16/332; all 8 return sites). TEMPLATE RULE (owner-approved,
  written into DECOMP_TEMPLATE.md): THE SWITCH-TABLE BYTE COMPARISON -
  every switch-bearing replacement's emitted case-indirection table
  ($LNxx: DB ...) is diffed byte-for-byte against the original's
  byte_XXXX table at the /Fa gate - the class the offset/macro/jcc
  checks cannot see (0 errors + every arm compiled correctly while the
  dispatch partition itself was inverted). Re-gate 0/0; the emitted
  table = DB 0,1,5,2,5,5,5,5,5,5,3,5,4 = byte_403724 EXACT. Re-verify:
  the table bytes in the TU listing; the arm label =
  RGE_ACTION_STATE_GOTO. ALL TESTS CLEAN (owner-confirmed in-game - the
  enter flow, the capture, the worker/trawler deposit validated with
  this fix).

- BATCH M1-ACTION - THE MOVE_TO TIER COMPLETE 9/9 (owner-approved +
  WRITTEN 2026-11-25): NEW CLASS FILE src/engine/action/RGE_Action_Move_
  To.cpp (+ install_action.cpp wiring + the vcxproj/filters). ALL NINE
  SINGLE-PASS (max 976 B update). ctor3(ctor_read) @0x408190, vdel
  @0x408200, ctor @0x408220 (the coordinate ctor + the TWO-CALL sprite
  bootstrap: the get_move_sprite slot-30 TEST then the second dispatch =
  the fetch), ctor2 @0x4082C0 (set_target_obj :182 + the world_x/y/z
  capture), destructor @0x408370 (stopMoving = the runtime slot 193 =
  +0x304 - the OWNED stopMoving_new routes), first_in_stack @0x4083D0
  (dist > 0.01f - flt_654978 BINARY-VERIFIED; the maximumSpeed equality
  = test ah,40h = the C3-only {==,NaN} set = X87_EQ_NAN - the .c's
  literal == would take NaN to Search = DIVERGENT), set_state @0x408430
  (Done/Failed/Invalidated = the base-sprite restore gated on
  !subActionValue; Goto = the action-sprite swap + the SELF vtbl update
  dispatch slot 10 = +0x28), update @0x4084B0 = THE RE-PATH RESOLVER
  (the retire gate state >= 7 / the lost ID -> the 507 notify p1 =
  RGE_UNITAI_ACTION_MOVE p2 = targetID; the distance gate target/position
  forms vs range; the patrol gate 711 RGE_UNITAI_ORDER_PATROL / 733
  UNKNOWN-the-registry-candidate; the re-path live target re-sync; the
  patrol capture (hasPatrolInformation/mPatrolCurrentWaypoint @+0x184/
  getPatrolLinear/getPatrolOneWay/zeroPatrolInformation) or the
  linearization ddy*dy+ddx*ddx >= 9.0 (flt_654DD0 BINARY-VERIFIED; the
  53-bit chain; the NaN -> linear 0 in both forms); the addMove dispatch
  slot 226 = +0x388 - the M12a-OWNED addMove_new routes with the
  9-stack-arg prototype arg-for-arg), Goto = the DRIFT BAND (ddx
  UNROUNDED / ddy STORE-ROUNDED through the float local - the roles
  pinned by the x87 stack walk; the skip-chain X87_LT_NAN/literal MATCH
  rows) -> the mTargetMovedState machine (the 508 target-moved notify)
  -> LABEL_45 intelligentMovementUpdate slot 198 = +0x318 (the SHIPPED
  capstone routes; == 1 Failed / == 2 Done), copy_obj_sprites @0x4088B0
  = the 4-way remap (get_sprite +4 / get_move_sprite +78 /
  get_fight_sprite +84 - the .c's vfptr[1].do_i_care_about_power render
  = the C2 artifact - / the RGE_Task trio @+0x34/38/3C). THE SWITCH-
  TABLE BYTE CHECK PASSED BOTH TABLES FIRST RUN (the emitted =
  byte_408898 / byte_4084A0 EXACT - the Enter-desync lesson applied at
  decode + verify). THE SETAE CENSUS NOTE: the linearization emitted
  the value-comparison form (comisd + setae) vs the original's branch -
  truth-table identical incl. NaN (the value consumption; the probe's
  unordered-false rule) - documented. RGE_UNITAI_ORDER_PATROL = 711
  added to RGE_Action_Types.h (the derived +100 rule; owner-flagged).
  GATE 2/2 TUs 0/0; defs/refs 9/9; the ABI/pointer-walk/CRT/base_game
  sweeps clean; the collision scan 2,963 sites / 0 hits. The vdel
  inlined the destructor body (the codegen-neutral merge class).
  IN-GAME VALIDATION OWED (the move flow, the patrol re-path, the drift
  band). NEXT per the roadmap: the next action-family tier (the derived
  classes behind create_task_action's switch: Attack/Gather/Explore...
  smallest-first by the marker scan), RGE_Action_List (56) and
  RGE_Action_Object (90) last. CARRIED: the common.h collisionUnits
  comment fix; the DECOMP_HIERARCHY Building-row re-walk; the 733 +
  507/508 registry candidates.

- BATCH M2-ACTION - THE GUARD TIER COMPLETE 3/3 (owner-approved +
  WRITTEN 2026-11-25): NEW CLASS FILE src/engine/action/RGE_Action_
  Guard.cpp (+ the wiring + the vcxproj/filters). THE SMALLEST TIER:
  ctor2(ctor_read) @0x00406180 [0x406180,0x4061A5) - the Enter2 shape,
  NO SEH; ctor @0x004061D0 [..,0x40622A) - set_target_obj (:182) + the
  SEH trivial chunk 0x640830; first_in_stack @0x00406230 [..,0x4062A3)
  - THE DEFEND-ORDER ISSUE: first_time && unitAI && target_obj -> the
  unitAI order dispatch (slot 4 = +0x10, idb_types.h:7589 - the member
  declares ALL 11 params arg-for-arg the asm pushes: obj->owner->id,
  701 = RGE_UNITAI_ORDER_DEFEND (DEFEND 601+100 - the derived registry,
  the enum addition owner-flagged), target->id, target->owner->id,
  world_x/y/z, 2.0f (the guard-order range parameter - the semantics
  UNKNOWN, the registry candidate), 0, 0, 100); set_state(1 Done)
  UNCONDITIONALLY. THE TIER OWNS NO DESTRUCTOR - the vftable
  (.rdata:006549FC) slots the SHARED family scalar-dtor
  (TRIBE_Action_Artifact__scalar_deleting_destructor - the same proc
  the Bird/Explore vftables point at) and inherits the base
  RGE_Action__update (lst :18852 DATA XREF). THE CAST-FORM CHECK: the
  .c's order-dispatch cast-form resolved clean - the member declares
  exactly 11 params matching the 11 asm pushes, the line-arith (member
  4 = +0x10) calibrated on the C2's member-5 notify. GATE 2/2 TUs 0/0;
  the /Fa slot census clean (the enum emitted push 701 exactly);
  defs/refs 3/3; the sweeps clean; the collision scan 2,972 sites /
  0 hits. IN-GAME VALIDATION OWED (the guard flow). NEXT: Make/Bird
  (5 each) smallest-first, then Explore 6 / Transport 8 / Gather 11 /
  Attack 14; RGE_Action_List (56) and RGE_Action_Object (90) last.
  CARRIED unchanged.

- BATCH M3-ACTION - BIRD 5/5 + MAKE 5/5 COMPLETE (owner-approved +
  WRITTEN 2026-11-25; the owner supplied the FULL RGE_Player::attributes
  registry 0-218 -> DECOMP_GAME_FACTS.md + the RGE_PLAYER_ATTRIBUTE enum
  (the 9 consumed values) in RGE_Action_Types.h). THE SEMANTIC
  UPGRADES: [36] Farm Food Amount / [88] Fish Trap Food Amount (the
  Make Work timer sources keyed on attribute_type == 16 Berry Storage);
  [10]/[25]/[26] the shield trio CONFIRMED; [5]/[28] actionTypeIfOrdered's
  Convert; [37]/[40] the destructor populations; the enter_target
  deposit = Meat/Berry/Fish(15/16/17) -> Food Storage(0). NEW CLASS
  FILES src/engine/action/RGE_Action_Bird.cpp + RGE_Action_Make.cpp.
  BIRD: ctor2/ctor @0x00402B00/0x00402B30 (the 5-arg form: task/x/y/z),
  first_in_stack @0x00402B70 (the 9.0f altitude teleport slot 42 =
  +0xA8, the discarded double return), set_state @0x00402BB0 (the
  magic-imul 80010003h/sar 0Eh timer form - the int chain then the
  single float store; move_sprite/work_sprite @+0x34/+0x38), update
  @0x00402C80 [..,0x402EBA) = the wander loop (the conditional timer
  tick, the random target map_width/height x rand/RAND_MAX, the
  X87_LT_NAN/LE_NAN clamps, the Move_To creation with the chunk-0x6406E0
  operator_delete fold, Goto/Work ticks X87_LE_NAN - THE NaN FIRES, the
  literal would diverge - caught at decode). MAKE: ctor2/ctor
  @0x00406F60/0x00406FD0 (+rge_read work_timer @+0x40; the 120.0f imm
  42F00000h), first_in_stack @0x00407000 (the carried-attribute set,
  the jl gate), set_state @0x00407030 (the Work-only timer sources),
  update @0x00407080 [..,0x40715A) (the work gate X87_LE_NAN; the
  empty-carry die_die_die slot 57 = +0xE4; the gain chain
  delta x gbg_work_rate_2@276 x work_val1@28; the work_flag word gate;
  the timer -= (new-pre) chain). THE TABLE CHECK: the Bird update's
  emitted $LN33 DD partition = jpt_402CD0 equivalent (the DD-indirect
  form - the DB-only scan missed it; the scan must cover DD too). GATE
  3/3 TUs 0/0; defs/refs 5/5 + 5/5; the sweeps clean
  (rand_internal/RAND_MAX_INTERNAL throughout); the collision scan
  2,975 sites / 0 hits (the corrected Bird update extent). IN-GAME
  VALIDATION OWED (the birds wander, the make build flow). INCIDENTS
  (both caught pre-gate): a fabricated vftable address in the first
  Bird draft (rewritten from the .lst); the RGE_Action_Types.h edit
  mangled the RGE_UNITAI_ORDER opener (repaired + verified: 5 enums,
  no duplicates). NEXT: Explore / Transport / Gather / Attack.

- BATCH M4-ACTION - EXPLORE 6/6 COMPLETE (owner-approved + WRITTEN
  2026-11-25; the goto-free directive applied): NEW CLASS FILE
  src/engine/action/RGE_Action_Explore.cpp (+ the wiring + the
  vcxproj/filters). ctor2/ctor @0x00403D30/0x00403D60 (the 5-arg form),
  first_in_stack @0x00403DA0 (THE -1.0f SENTINEL: the raw dword cmp
  0BF800000h - the float == compare VALUE-IDENTICAL universally (the
  exact sentinel; the emitted ucomiss+lahf/test 44h/jp unordered-safe
  pair verified in the emission), -> Search(3) if unset else Move(11)),
  set_state @0x00403DC0 (delete_list FIRST; the 4-arm GOTO-FREE JOIN:
  Done/Failed/Invalidated/Search share the arm with the notify gated on
  Search - the asm's 2-inbound fall-through join preserved with ONE
  restore emission (the compiler tail-merged to $LN21); Search = the
  514/605 EXPLORE notify; Wait(2) = the restore + set_target_obj(0)/2(0)
  + the -1.0f sentinel trio reset; Move(11) = task->move_sprite or the
  master get_move_sprite fallback (the M4 cast) -> the Move_To creation
  (the chunk 0x640790 fold) -> setSubAction(1) THEN add_action - THE
  ORDER, no mTargetMovedState write), update @0x00403F60 (Done -> 1;
  Move -> the sub partition {1/3/5 -> Search; 4 -> set_target_obj(0) +
  Move; else 0}; Failed -> notify(505, currentAction(unitAI), 0, 0) -
  the p1 = the RUNTIME current action code (UnitAIModule__currentAction
  :5586 @0x5F4D70) CALLED UNCONDITIONALLY on the possibly-null unitAI
  (the original's shape, no invented check) -> Wait(2) -> return 3),
  work @0x00404020 (X87_LT_NAN(x/y, 0.0f) - the NaN takes Search - the
  macros required; the target trio; Move(11)). THE GOTO-FREE JOIN NOTE:
  the table-byte equality and the goto-free join are mutually exclusive
  in C - the byte[2] divergence (Search = the shared entry 0) is the
  documented deliberate cost, the effects-partition verified, the
  compiler tail-merge preserved the single emission. THE SETCC NOTE: 0
  occurrences. GATE 0/0; defs/refs 6/6; the sweeps clean (0 actual
  gotos); the collision scan 2,985 sites / 0 hits. THE 5xx REGISTRY:
  514 = the expedition-started notify (p1 = 605 EXPLORE). IN-GAME
  CARRIED unchanged.

- BATCH M6-ACTION - GATHER PARTIAL WRITE 6/11 (owner-approved +
  WRITTEN 2026-11-25): NEW CLASS FILE src/engine/action/RGE_Action_
  Gather.cpp. THE 6 FULLY ASM-WALKED FUNCTIONS: ctor3(ctor_read)
  @0x00404080 [..,0x404160), Gather(ctor) @0x00404160 [..,0x404290)
  (the FARM CAPTURE at master id 50 + the timePerStop = attr_max x
  mult / (rate x val1) x 0.25f), Gather2 @0x00404290 [..,0x404300),
  save @0x00404300 [..,0x404380) (the 9-write mirror), first_in_stack
  @0x00404380 [..,0x404490) (the carried-attribute set + the
  farm-crowded check worker_num @+0x5F > 1 -> Move(11) + set_target_
  obj(0); the linked-owner redirect (the Enter ctor precedent); the
  owner + drop-site match -> Return(8)), set_state @0x00404490
  [..,0x404EE0) = 2640 B - THE BIGGEST FUNCTION DECODED. THE
  SWITCH-TABLE: byte_404DE8 = 247 entries (db 0,1,2,3,10,4,5,6,10,
  10,7,10,0,0,7 + the 0Ah filler + the tail ...,7,8,9,10,10,9) -
  THE EMITTED TABLE = EXACT. THE ARMS: Done/Failed/Invalidated ->
  timer = 0 + the wait-sprite restore; Wait(2) -> the wait sprite +
  the clears + the sentinel trio; Search(3)/state 243 -> the drop-off
  find (find_by_master_ids with the drop_site/backup pair @+0x118/
  +0x11A) + the 514/609 notify + timer = -10.0/-1.0; Goto(4) -> the
  target sync + the AI-water (object_group_count[59]/[17] +
  lookupZone/currentZone -> 244) + numberStops==1||quick_build ->
  Move_To2 / the 10-attempt lastStop dedup + rand spread -> the
  coordinate Move_To; Work(6) -> THE FARM CAPTURE (the group 7 Farm:
  the owner mismatch + gameType==6 && worker_num + build_pts @+0x230
  = 560 <= 0 -> Done(1); the vtbl +188 change_ownership) + the
  multi-stop sprite; Work2(7) -> the carried-attribute set + the
  work sprite (the group 58/14 civ-indexing) + the work sound;
  Return(8) -> currentStop = 0 + the drop-site resolve (the
  useSameZoneDropsite slot +0x1CC + the find pair) + the AI-water ->
  F7=247 + the Move_To2 / the 515/609 notify -> Failed(13); Move(11)/
  15/242 -> the shared coordinate Move_To; 244/247 -> the wait-sprite
  only; the F5/F8 gate -> delete_list SKIPPED. ALL 11 GOTOS to
  wait_sprite_restore INLINED (the 2-line get_wait_sprite +
  new_sprite per arm; the compiler merges identical tails). GATE 0/0
  (4/4 TUs incl. install_action + Explore + Transport); defs/refs
  6/6; the table EXACT; 0 gotos/0 setcc/0 x0.0-fold/0 esp-this;
  bare CRT 0. THE FIELD MAP: RGE_Action_Gather = the base Action
  mirror + targetType @+0x40 (i32), numberStops @+0x44 (u8),
  currentStop @+0x45 (u8), timePerStop @+0x48 (f32),
  timeAtCurrentStop @+0x4C (f32), lastStopX @+0x50 (u8), lastStopY
  @+0x51 (u8), have_worked @+0x52 (u8), farm_x @+0x54 (f32), farm_y
  @+0x58 (f32), gbg_unknown_1 @+0x5C (i32 - the work2 gather-flag).
  THE INTERNAL STATES 242-249: the F5/F8 pair skips delete_list;
  the F4/F7 -> the wait-sprite-only arm; the 245/246/248/249 = the
  bare stores. THE REMAINING 5 FUNCTIONS (update 2180 B, get_move_
  sprite, get_wait_sprite, work, auto_rebuild_farm): the .c bodies
  ALL READ, the asm walks IN PROGRESS (the update walked to ~0x40584A),
  THE DRAFTS NOT STARTED - the hooks NOT installed for these. THE
  REGISTRY CANDIDATES: 514 = the gather re-target notify (p2 =
  targetType, p3 = the drop-off id); 515 = the no-drop-site; the
  internal states 242-249; the notification(127) = the Base_Game vtbl
  slot 17; the drop-off terrains {1,22,23}; the master object_flags
  bit 2 = ship unit (RESOLVED); master id 82. IN-GAME VALIDATION OWED
  (the 6 written functions). NEXT SESSION: the Gather update/tails
  decode -> the drafts -> gate -> write (the 5 remaining hooks); then
  Attack 14; RGE_Action_List (56); RGE_Action_Object (90). CARRIED:
  the common.h collisionUnits comment fix; the DECOMP_HIERARCHY
  Building-row re-walk. THE NEXT-SESSION OPENER: "Read the 4 state
  files first. STATE: the action family 7/8 derived tiers OPEN
  (Move_To 9, Guard 3, Bird 5, Make 5, Explore 6, Transport 8, Gather
  6/11 partial - the 6 written + gated + installed; update/get_move_
  sprite/get_wait_sprite/work/auto_rebuild_farm remain: the .c bodies
  READ, the asm walks PARTIAL (the update stopped at 0x40584A of
  0x40A442, the tails UNWALKED), THE DRAFTS NOT STARTED. IMMEDIATE:
  (1) the Gather update asm tail walk (lst 7716-8765 from 0x40584A);
  (2) the tail functions (lst 8765-9271); (3) the drafts -> gate ->
  append the 5 hooks to install_RGE_Action_Gather; (4) then Attack 14;
  (5) RGE_Action_List (56); RGE_Action_Object (90). THE FIELD MAP FOR
  GATHER IS BANKED IN THE CPP HEADER COMMENT. THE INTERNAL STATES
  242-249 MAPPED. THE REGISTRIES: the attributes 0-218, the
  object_flags bits, the 5xx family (505/506/507/508/512/514/515/518/
  519/527/530), the RGE_PLAYER_ATTRIBUTE enum (the 9 consumed values).
  THE OBJECT_FLAGS BIT REGISTRY IS IN THE GAME FACTS (bits 0-7). THE
  GATHER FUNCTION MAP: the 6 written functions in
  RGE_Action_Gather.cpp; the 5 remaining append to the same file
  (the hooks + the installer grow from 6 to 11). THE COMPILE GATE:
  4/4 TUs 0/0 as of the session close."

- BATCH M5-ACTION - TRANSPORT 8/8 COMPLETE (owner-approved + WRITTEN
  2026-11-25): NEW CLASS FILE src/engine/action/RGE_Action_Transport.cpp
  (+ the wiring). THE STRUCT = the base Action mirror ONLY (64 = 0x40 -
  the hierarchy's verified prefix identity; NO extension fields).
  ctor2/ctor @0x00409F00/0x00409F30 (the 5-arg form; the callers = the
  OWNED transport_new/unload_new - routes), first_in_stack @0x00409F70
  (first_time && target_x/y >= 0.0 - the literal >= MATCH rows - ->
  Move(11) else Wait(2)), set_state @0x00409FC0 = 512 B (the sub chain:
  Wait(2) = the base-sprite restore; Goto(4)/Move(11) SHARE the block -
  the .c's !v5||v5==7: ma = get_map_analysis(:2047 @0x49B470) on
  (*base_game)->world->map_analysis_system @+0x74 = 116;
  find_closest_pathable_point(:2023 @0x4981F0) -> the target_x/y = the
  resolved pair (z untouched); THE THREE-CASE SPRITE CASCADE:
  task->move_sprite @+0x34 / the master get_move_sprite TEST+fetch
  (slot 30 = +0x78; the member this_ = RGE_Master_Action_Object* - the
  IDB's inherited-slot typing) / get_sprite (slot 1) - the range =
  0.3f (flt_6552F0 BINARY-VERIFIED 9A 99 99 3E); add_action THEN
  setSubAction (the Explore contrast); the null -> Wait(2)), update
  @0x0040A1C0 = 642 B (the byte_40A45C table EXACT db
  0,5,5,1,5,2,5,5,5,5,3,5,4; Goto(4) = the sub gate {1}U{3,4,5} ->
  Work(6); Move(11) = the same gate -> Wait(2) - THE .c's case list
  drops sub-4, the render artifact; Work(6) = THE DROP-OFF MACHINERY:
  the zone resolve via lookupZone(:3883 @0x552610) on the carried
  List[0] (obj->objects @+0x30 = 48, the RGE_New_Object_List),
  withinRangeOfZone(:3885 @0x552730) the 1.5 gate, next_to_drop_off_
  terrain (the SELF dispatch slot 25 = +0x64 = 100), drop_held_objects
  (:3877 @0x552010), THE OWNER-LEVEL notifies 518 (the drop success) /
  519 (the fail) via TRIBE_PlayerVtbl member 75 = +0x12C = 300 decimal
  with p1 = RGE_UNITAI_ACTION_TRANSPORT 614, the terrain checks
  {1, 22, 23} - THREE separate get_terrain calls - the terrain-id
  registry candidates; the gate FAIL: ALL paths land on set_state(2
  Wait)), move_to @0x0040A470 (target2ID @+0x1C = 0; Move(11)), work
  @0x0040A4A0 (target2ID = 1 - THE DIRECTION FLAG; the zone resolve
  with the carried override; the pickup: withinRange == 1 && dist <=
  0.3f X87_LE_NAN - THE NAN FIRES, caught at the emission review; the
  general path X87_LT_NAN x/y; the obj_in POINTER compare -> Move(11) /
  Goto(4)), next_to_drop_off_terrain @0x0040A610 = 474 B
  (task->terrain_id @+0xE (i16, the RGE_Task walk-verified) == 0xFFFF
  -> 1; the scan rect world -+ radius_x/y @+0x3C/+0x40 -+ 2.0f
  (flt_6552F4 BINARY-VERIFIED 00 00 00 40) clamped to the map; the zone
  via the target/the carried override; the scan: zone == -1 -> the
  master object_flags bit 2 (0x02 = SHIP UNIT - the owner-supplied bit
  registry) CLEAR -> true OR the shore terrains 2/4 -> 1 (the ship
  transports require the shore; the non-ships accept any), zone set ->
  the carried lookupZone(col,row) == zone -> 1). THE GATE: 0/0; the
  table byte_40A45C EXACT; THE LAHF/TEST 68h IDIOM verified (the
  ucomiss equality = the ordered/unordered partition via test's own PF
  - the != unordered-TRUE row honored; the same class as the Explore's
  test 44h sentinel); the dist gate = X87_LE_NAN (the NaN fires, caught
  at the emission review); defs/refs 8/8; the sweeps clean (0 literal
  gotos - the Work flag-join form); the collision scan 2,991 sites /
  0 hits. THE 5xx REGISTRY: 518 = the transport drop-off success, 519 =
  the fail (the owner-level, p1 = 614 TRANSPORT). THE REGISTRY
  CANDIDATES: the drop-off terrains {1, 22, 23}; object_flags bit 2 =
  ship unit (RESOLVED - see the game-facts registry). IN-GAME
  VALIDATION OWED (the transport load/unload flow).

- GATHER TIER DECODE STATE (in progress 2026-11-25; all 11 .c bodies
  READ; the asm walks ~90% (Gather3/Gather/Gather2/save/first_in_stack/
  set_state FULLY WALKED + the update ~85%); the tail functions
  (get_move_sprite/get_wait_sprite/work/auto_rebuild_farm) UNWALKED;
  the DRAFTS NOT STARTED; per the M12 decode-state convention - no
  function bodies in the state file, the decode detail lives in the
  session):
  * THE INVENTORY: 11 procs (Gather3 224 B @0x404080, Gather 304 B
    @0x404160, Gather2 112 B @0x404290, save 128 B @0x404300,
    first_in_stack 272 B @0x404380, set_state 2640 B @0x404490,
    update 2180 B @0x404EE0, get_move_sprite 176 B @0x405D40,
    get_wait_sprite 144 B @0x405DF0, work 560 B @0x405E80,
    auto_rebuild_farm 208 B @0x4060B0). ALL SINGLE-PASS (< 4096).
  * THE STRUCT: RGE_Action_Gather = the base Action mirror + the
    extension @+0x40 = 64: targetType @+0x40 (i32), numberStops @+0x44
    (u8), currentStop @+0x45 (u8), timePerStop @+0x48 (f32),
    timeAtCurrentStop @+0x4C (f32), lastStopX @+0x50 (u8), lastStopY
    @+0x51 (u8), have_worked @+0x52 (u8), farm_x @+0x54 (f32), farm_y
    @+0x58 (f32), gbg_unknown_1 @+0x5C (i32 - THE WORK2 GATHER-FLAG
    slot, the IR#3 grounding).
  * THE INTERNAL STATES: 242-249 beyond the named registry (243 = the
    F3 drop-off timer -1.0; 244/247 = the water-crossing markers; 245/
    246/248/249 = the bare state stores (the set_state treats them as
    the default); the F5/F8 pair SKIPS delete_list - the sub-actions
    preserved across the internal transitions). The update's F4/F7 ->
    245/248; the F5/F8 -> 246/249 (the inside_obj gates).
  * THE FARM CAPTURE (the master id 50 = Farm): farm_x/y = the target
    snapshot; the timePerStop = attr_max x mult / (work_rate x
    work_val1) x 0.25f (the .c's exact chain; mult = the
    attributes[attribute_type2] <= 0 -> 1.0).
  * THE GATHER MATH (the update case 7): gain = mult x min(delta x
    gbg_work_rate_2 x work_val1, target.amount); the
    attribute_max_amount cap with the overflow split; the +/- 0.1
    tolerance pair (0.0099999998 = 0.01f's bits); the
    set_attribute_amount mirrors on the target AND the unit.
  * THE DEPOSIT (case 8): attribute_type3 @+0x18 == 0xFFFF -> the
    held-type fallback; add_attribute_num + trackUnitGather +
    set_attribute(-1, 0); the AI re-seek via currentTargetType/
    lastTargetType/convertToLOSResourceType/closestResourceObject (the
    4 UnitAI vtbl members); the sentinel pair -> the AI Done(1)/Wait(2)
    split; the master class 259/214/13+targetType-7 -> state 243 (F3).
  * THE AI-WATER CHECK: the owner's object_group_count[59] (Fambaa
    Shield Generator) / [17] (Fish) > 0 -> lookupZone(target) !=
    currentZone(obj) -> the internal 244/247.
  * THE NOTIFICATION(127) = the Base_Game vtbl slot 17 (+44h) - the
    farm-decay rebuild notice; quick_build = [base_game+0x9E0] (2528).
  * THE FARM CAPTURE SLOT +188 = the TRIBE_Building vtbl member 47
    (the set-owner dispatch); v51[140] = the target field @+0x230 = 560
    (the farm food).
  * THE NEW GROUNDINGS: RGE_Player_Object_List__find_by_master_ids
    (the drop_site/backup_drop_site @+280/+282 pair - the
    master_obj[1].selected_sound render = the base-pointer artifact);
    RGE_Static_Object__currentZone; the RGE_Action_Gather vftable =
    .rdata:00654_904 (0x00654904 per the .c).
  * THE REGISTRY CANDIDATES: 514 = the gather start/re-target notify
    (p2 = targetType, p3 = the drop-off id); 515 = the no-drop-site
    notify; the internal states 242-249; the notification(127).
  * CONTINUE: the update asm tail (lst 7716-8765, ~1050 lines - the
    walk stopped at 0x40584A); the tail functions (lst 8765-9271); the
    RGE_Action_Gather struct walk verification; the drafts; the gate;
    the presentation. THEN: Attack 14; RGE_Action_List (56);
    RGE_Action_Object (90).

  - GATHER 11/11 COMPLETE + THE GATHER BAKE ROUND (owner-approved +
    WRITTEN 2026-11-25). M7: update @0x404EE0 (3305 B; the two-pass bar
    fired 27 gotos/13 back-edges - PASS 1 aligned transcription WRITTEN,
    Pass 2 owed on the suite-verified baseline), get_move_sprite
    @0x405D40, get_wait_sprite @0x405DF0, work @0x405E80 (all
    single-pass), auto_rebuild_farm @0x4060B0 = the VERBATIM ADOPTION of
    the farm.cpp handler (the canonical _new transfers; the helpers
    extern-declared). The action state enum applied everywhere; 242-249
    + the 5xx stay cited literals. Installer 6 -> 11; defs/refs 11/11;
    gate 4 TUs 0/0. 11 BAKES (all sites retired, 0 live): update x3
    (0x404F54 user_released gate + Aqua; 0x40560D early-full exit;
    0x405FDE Aqua capture, no stop math), work x1 (0x405F38 the deposit
    gate + Aqua v>=8), ctor x1 (0x4041DD the capture gate + Aqua),
    first_in_stack x1 + set_state x1 (0x40438E/0x404A93 ->
    verify_food_resource keep true/false), set_state x2 (0x4047E0/
    0x404C06 the ctr2 count-table re-base: [[owner+0x2F0]][1659/1617]
    replaces object_group_count[59]/[17] - address-identical under the
    vanilla layout), set_state x1 (0x404928 -> new_farm_walk: the v>=8
    +0.5f farm-center Move_To). NEW GROUNDINGS: TRIBE_Player+0x2F0 = 752
    = object_category_count_new (the owner's IDB union landed: the
    object_category_count[1600] / __int16* union - the ctr2 table
    pointer; the two set_state bakes read the typed member, the /Fa
    emission byte-identical to the raw-offset form);
    object_group_count @+3952 (the walker; the unions overlay - no
    layout shift);
    master id 199 = Aqua Harvester (farm.h - the registry candidate);
    RGE_Action_GatherVtbl @:28767 (update slot 10/work 13);
    byte_405C04/off_405BCC/off_405D00/off_405D14 binary-verified; the
    switch-table byte comparison 249/249 EXACT; flt_654968 = 0.25f etc.
    binary-verified. RULE-(j) LIVE: the double DECLARATION alone
    shrinks (/fp:fast computes float+float at float) - the fix = the
    explicit OPERAND casts + the precise bracket (8 ss-ops -> 0 in the
    new code; 5 pre-existing M6 ss-ops in set_state/ctor flagged for the
    next re-gate). PROTOCOL NOTE (owner-directed 2026-11-25): the state
    file additions stay CONCISE going forward. DESYNC (the gather flow's
    first in-game test, owner bisect -> set_state): ROOT CAUSE = the M6
    transcription bug in the Work arm's sprite restore - the asm's
    get_move_sprite RESULT flows in eax into the shared tail loc_404D9C
    (new_sprite(obj, THE MOVE SPRITE)); the IDA merged that entry with
    the Done arm's loc_404D95 wait-sprite entry and the transcription
    new_sprites get_WAIT_sprite while discarding the move sprite. FIXED
    (new_sprite(obj, sprite); the emission shows the move sprite as the
    new_sprite arg). The four bakes + the ctr2 reads verified FAITHFUL
    in the same walk (the new_farm_walk x/y slot mapping proven: the
    stub's [esp+0Ch]/[esp+2Ch] = var_1C/the new_state slot = new_x/
    new_y - the same slots the original's ctor filds read). Verified:
    the ctr2 table maintenance INTACT (the 70+ active aiunitcount hooks
    all live on unowned functions - no owned-range collision). THE
    SWITCH-TABLE LESSON: set_state emits the SINGLE-LEVEL DWORD table
    (no byte indirection) - the byte-form diff is impossible; the
    partition must be verified state-by-state instead (all 15 states
    verified: 1/13/14->Done, 2->Wait, 3->Search, 4->Goto, 6->Work,
    7->Work2, 8->Return, 11/15/242->Move, 243, 244/247->the wait arm).
    FLAGGED (weak-NaN, practically unreachable): the 0x404D4D
    target-amount gate + the 0x404A0D build_pts gate take the literal
    forms whose NaN rows diverge. DESYNC #2 (the same function, the
    owner bisect after the sprite fix): THE RULE-(j) SHRINK CLASS IN
    set_state's multi-stop spread block - the min_x/min_y subs and the
    *2 spreads compiled subss/addss + cvttss2si (24-bit) where the
    original fld/fsub/fadd/ftol computes at 53-bit - when (target -
    radius) lies within a float-ULP of an integer the truncation lands
    on DIFFERENT TILES (the block runs on every multi-stop stop
    advance; farms numberStops=4). FIXED: the file-wide precise bracket
    (Gather3 -> update_new; the work/update pair nests inside) + the
    spread-block operand casts + the ctor's rate_x_val1 float local ->
    the double local + the casts (the same shrink: mulss + widen vs the
    exact 53-bit product). /Fa: ss-ops TU-wide 5 -> 0; the spread block
    emits cvtps2pd x2 + subsd + cvttsd2si (= fld/fsub/ftol) and the
    ctor chain cvtps2pd/mulsd/divsd/mulsd/cvtpd2ps (= the x87 form).
    CARRIED: Pass 2 + the suite run for update; the in-game re-test of
    the gather flow (the sprite fix + the precision fix). DESYNC #4 -
    THE DEDUP-LOOP BREAK CONDITION (the owner found it; the logs' first
    lsx/lsy divergence at the FIRST set_state(4) call with
    byte-identical inputs pinned it): the break was written
    `new_x != lastStopX && new_y != lastStopY` - the asm's TWO
    jnz-to-exit jumps (0x4048CE + 0x4048D7, both to loc_4048FD) = exit
    when EITHER differs = THE OR FORM. THE AND KEPT THE LOOP REDRAWING
    while one coordinate matched (+2 rand draws per extra iteration, up
    to +20 per set_state(4)) - the global rand stream shifted on the
    first multi-stop call and EVERY downstream rand consumer desynced
    (the row-4 picks and everything after; "the first divergence is not
    the first" = the stream split inside the very first call). FIXED to
    the OR form (the emission: two cmp/jne pairs to the same break
    target); the same-class sweep over the owned tree: the other
    multi-inequality compounds = the membership-skip negation form
    (the jz-to-skip pairs -> !c1 && !c2) - VERIFIED CORRECT at their
    batches. THE PROTOCOL AMENDMENT APPLIED (owner-directed):
    DECOMP_TEMPLATE.md MULTI-JCC COMPOUND CONDITIONS - each
    shared-target jcc = an independent OR-term of the target's
    condition; the connective follows the JUMP TARGET (the same exit =
    the OR; the same skip = the negated-AND proceed); the 2^N-row
    truth table per site mandatory pre-write, re-verified against the
    /Fa jcc targets; the surface similarity to a verified compound is
    never the evidence. ALSO: the rand capture corrected - the
    canary-draw idea WITHDRAWN (an extra draw kills the determinism);
    the logger reads the generator STATE at 0x0069EC98 (the owner's
    pointer) non-invasively - the logged seed recomputes the arm's
    draws via the MSVC LCG (seed*214013+2531011, the draw =
    (seed>>16)&0x7fff) making the picks predictable from build B
    alone. The set_state_orig prologue corrected by the owner (the
    push/ret form: the fs:[0] replay + push 0x404496 + ret - my
    add esp,4 compensation was wrong). SESSION CLOSE (2026-11-25, the
    owner): THE SET_STATE DESYNC IS FIXED (owner-confirmed in-game).
    Other issues remain - THE NEXT TARGET: RGE_Action_Gather__work_new
    desync (work_new @0x00405E80, unhooked in the current bisect
    build; the harness exists only for set_state - build the work_new
    A/B pair per the same construction first). The current bisect
    build's hook state: Gather3/Gather/Gather2/save/first_in_stack/
    set_state(logger)/auto_rebuild hooked; update/get_move_sprite/
    get_wait_sprite/work unhooked; ALL 11 interior patch sites
    RE-APPLIED by the owner (the patch call sites un-commented) - the
    unhooked vanilla bodies run the patched semantics ✓. DESYNC #3 - RETRACTED (the
    owner's review caught it): the "return-type mismatch" claim was
    UNGROUNDED - idb_functions.h:45 declares
    RGE_Action_Gather__set_state as VOID and the M6-written void form
    was CORRECT; the u8 change + the 28 return-0 conversions were
    reverted (the gate re-verified 0/0; the emission has no
    return-value writes, ss-ops 0). LESSON: the return-type assertion
    was made without re-reading the idb line - the citation discipline
    applies under desync pressure too. THE A/B HARNESS REMAINS
    INSTALLED (the DIAG block: set_state_orig/set_state_logger/
    g_setStateTraceOrig; the installer hooked the logger) - the static
    walk is exhaustively clean (the store diff, the exit paths, the
    arms), so the harness run is the evidence-producing step. master ids
    259/214/13+7 + the 5xx family + the 242-249 internal states + the
    msgs - the registry candidates; the common.h collisionUnits comment
    fix; the DECOMP_HIERARCHY Building-row re-walk.
    SESSION CLOSE (2026-11-25, the owner): THE SET_STATE DESYNC IS
    FIXED (owner-confirmed in-game). Other issues remain - THE NEXT
    TARGET: RGE_Action_Gather__work_new desync (work_new @0x00405E80
    [0x40405E80,0x4060A6); unhooked in the current bisect build - the
    desync = work_new-vs-the-vanilla-work+the farm_walk patch
    semantics; re-hook work_new first). The bisect build's hook state:
    Gather3/Gather/Gather2/save/first_in_stack/set_state(the logger)/
    auto_rebuild hooked; update/get_move_sprite/get_wait_sprite/work
    unhooked; ALL 11 interior patch sites RE-APPLIED by the owner (the
    patch call sites un-commented) - the unhooked vanilla bodies run
    the patched semantics. The set_state logger rows remain live for
    the correlation (the row-163 finding: the unit 13394 Work tick
    timing - plausibly the work_new-linked surface).
  - NEXT SESSION (paste as the opening prompt): "Read DECOMP_STATE.md,
    DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md, DECOMP_GAME_FACTS.md in
    cheatdll/src/idb/ first. TARGET: the RGE_Action_Gather__work_new
    desync (the owner bisect; work_new @0x00405E80 [0x40405E80,
    0x4060A6) - UNHOOKED in the current bisect build, so the desync =
    work_new-vs-the-vanilla-work+the farm_walk patch semantics; re-hook
    work_new first). STATE: the Gather tier 11/11; the set_state
    desync FIXED (the Work-arm sprite, the file-wide precise bracket +
    the spread-block/ctor-rate casts, the dedup-loop OR-break; the
    return-type claim RETRACTED - the details in the ledger); the
    work_new bakes = 0x405F38 only (the deposit gate + Aqua v>=8); the
    work_new groundings WARM: the linked-owner redirect @+0x1DC, the
    drop-site deposit arm (@+0x118/@+0x11A + the amount > 0 gate), the
    farm capture (the timePerStop math = the ctor's twin - the double
    rate_x_val1 + the precise bracket HELD), the int return. THE
    HARNESS: the set_state logger exists (the extended rows:
    id/rnd=0x0069EC98-state/tps/tacs/tile/saT/saSt/saX/saY/wx/wy/lsx/
    lsy/fx/fy) and stays live for the correlation (the row-163
    finding: the unit 13394 Work tick timing - plausibly the
    work_new-linked surface). BUILD THE WORK_NEW A/B PAIR PER THE SAME
    CONSTRUCTION: the naked _orig trampoline (the owner's set_state_
    orig push/ret form; the work prologue at 0x405E80 = push
    ebx/ebp/esi/edi + the arg loads - the replay bytes per the .lst,
    the resume past the E9) + the logger (the extended-row shape) +
    the installer swap. PROTOCOL: the MULTI-JCC rule (the new template
    amendment - the connective follows the jump target, the 2^N truth
    table per site); the rand STATE read at 0x0069EC98 (never an extra
    draw); the concision directive for the state additions; all edits
    approval-gated; the DIAG harness removal at ship."

  - PRAGMA CLEANUP (owner-reported 2026-11-26): the Gather file's precise
    bracket was 3-push/1-pop (the M7 write left two redundant nested pushes)
    - the lone pop restored precise-on, so the tail compiled precise
    silently. Fixed to the single push/pop bracket (Gather3->update_new);
    the emission diff vs the pre-cleanup listing = byte-identical.
    Re-verify: the pragma count = 2 (push@60/pop@1416).

  - GATHER DIAG HARNESS REMOVED (owner-directed 2026-11-26, all tests
    clean): the set_state A/B block deleted (set_state_orig/
    g_setStateTraceOrig/set_state_logger + the installer flip lines + the
    4 unreferenced A-leg externs); set_state hooks _new directly; work_new
    active in the installer (the owner flip - the fix round in-game
    verified on the real work path). The zero-diagnostic-symbol sweep =
    all 0 (file + listing); defs/refs 11/11; the emission diff = only the
    trampoline's COMDAT block removed, the 11 game functions
    byte-identical. Re-verify: grep the TU listing for
    _logger/_orig/sprintf/fopen (= 0).

  - GATHER UPDATE PASS 2 SHIPPED (2026-11-25): the restructure of
    update_new on the suite-verified pass-1 baseline (the fixes round
    tested clean) - v-names dissolved, the joins inlined
    (LABEL_53/96/33/43/127/135/156/162/164/187/189 + the LABEL_120
    3-inbound join flattened to its true form), 0 gotos, locals at first
    use. EQUIVALENCE: the call multiset identical (named + vtbl slots;
    the deltas = pure-read CSE + one duplicated sentinel-compare pair,
    tails merged); the main byte table 249/249 BYTE-IDENTICAL + the three
    DD partitions identical; the brace-depth walk clean per stage; the TU
    gate 0/0. Pass 1 = the VCS diff reference. The re-run = the owner's
    suite (owed at entry time). Re-verify: the tables + the call census vs
    the pre-splice listing.

  - ATTACK TIER BATCH AT1 (owner-approved + WRITTEN 2026-11-26): NEW CLASS
    FILE src/engine/action/RGE_Action_Attack.cpp + the wiring. 6 hooks:
    destructor @0x401410, vdel @0x401130, stop @0x401610 (set_state(1) via
    slot +0x60; the Pack vftable shares), set_target_obj @0x402AD0 (new
    target -> attack_count = 0 @+0x170, the Combat downcast), first_in_stack
    @0x401530 (ready_to_attack &rangeState; the maximumSpeed {==0,NaN}
    X87_EQ_NAN skip; canMoveToAttack +0xCC), copy_obj_sprites @0x4029D0
    (the task-first sprite remap; the getters slot 30/33/1). The vftable
    .rdata:00654540; the struct = the Action mirror + the extension @64
    (idb_types.h:20925). Scan 0; gate 2/2 0/0; defs/refs 6/6. NEXT: AT2 =
    save/work/ctor trio; AT3 = set_state/ready_to_attack/update (the
    two-pass check at decode). Re-verify: the installer 6 hooks + the +124
    x4 census.

  - ATTACK TIER BATCH AT2 (owner-approved + WRITTEN 2026-11-26): +5 hooks
    (11/14): Attack3 @0x401000 (the stream loader: range/min_range/
    missile_id/fire_missile/need/was_same/indirect + the sprite IDs via
    get_sprite_pointer :3829 + the lastTarget trio), ctor @0x401150 (the
    area_effect_range > 0 gate -> the Building/linked redirect; the
    attack_count clear; the move_sprite fallback = get_run_sprite TWICE
    non-null else get_move_sprite - the double call verified in the
    emission; range {==0,NaN} -> master weapon_range), Attack2 @0x4012E0
    (the coordinate form; indirect_fire_flag = 2 - the registry
    candidate; the same fallbacks), save @0x401420 (the ctor3 mirror +
    the sprite IDs as -1-or-id words), work @0x402290 (the ctor's twin
    redirect; the same-target accept {4,5,11,6,7} - Goto2 joins;
    canMoveToAttack on the WORKTARGET's id; weaponRange <= 0.1f ->
    range += 0.5; dist > range -> return 0 - the literal > MATCH row).
    Scan 0; gate 0/0; defs/refs 11/11. NEXT: AT3 = set_state/
    ready_to_attack/update (the two-pass check). Re-verify: the installer
    11 hooks + the +124 x4 census.

  - ATTACK CTOR DESYNC FIX (owner bisect + fixed 2026-11-26): the three
    ctors wrote action_type via RGE_ACTION_TYPE_ATTACK (= 7, the
    task-side value) where the original writes 9 (RGE_ACTION_TYPE_ATTACK2
    - the Attack ACTION self-types the ATTACK2 slot; work2's confirm gate
    type()==9 corroborates); ctor_new ALSO dropped the store entirely
    (asm 0x401185). Fixed: ATTACK2 x3 + the restored vfptr/action_type
    pair. Both classes invisible to the compile gate; the /Fa IMMEDIATES
    check (mov eax, 9 x3) run post-fix and now on the per-batch
    checklist. Attack2_new re-walked clean otherwise. Re-verify: the /Fa
    immediates.

  - READY_TO_ATTACK DESYNC OPEN + THE A/B HARNESS LIVE (2026-11-26): the
    desync persisted after two static fixes (the drive gate 41h mask -> the
    literal > 0.0f; the chase-fail fall -> set_state(13)/return 0 instead
    of the sync fall-through; the dx unrounded st0 capture in the +-0.5
    ladder). THE STATIC ANALYSIS IS EXHAUSTED (two full instruction-level
    re-walks). THE A/B HARNESS IS LIVE: ready_to_attack_orig (the naked
    trampoline: sub esp,8 / push esi / mov esi,ecx replay, the resume
    0x4023B6, the push/ret form) + ready_to_attack_logger (the rta-in/
    rta-out rows: id/rnd/st/tgt/toid/g8/fog/os/sv/co/mx/rng/min/tx-ty-tz/
    ltx-lty-ltz/wx/wy/wsame/ind/sub in; ret/rs/st/rng/min/tgt/tx/ty out;
    the rand STATE at 0x0069EC98) + the installer flip (0x4023B0 -> the
    logger; g_rtaTraceOrig 1 = the vanilla, 0 = _new). KNOWN LOGGER BLIND
    SPOT: the formation-lock (mFormationLocked) key absent from the IN row.
    NEXT SESSION: the owner runs the same replay in both builds (the flip
    1 then 0), diffs val_log.txt (the rta rows), the first divergent row
    names the mechanism; the targeted asm walk of that region. Re-verify:
    the harness emission (the trampoline bytes + the logger keys).

  - READY_TO_ATTACK DESYNC ROOT-CAUSED + FIXED (2026-11-26, the A/B diff -
    IN rows aligned 21 calls, OUT diverged at call 0: rs 1->-1, st 6->5 =
    inside the probe): D1 the loc_402601 visibility routing inverted (the AI
    probe = vis==0 || (128 && !fog); vis not in {0,128} and 128&&fog go to
    the loc_40269F ladder region, no state change; the written complement
    FAILED was invented); D2 the band ladder's all-within exit 0x402743 jumps
    straight to loc_402982 - no target_x/y/z sync (loc_40296D's only xref =
    0x402866, the dist2 arm); + the missing loc_40269F equality preliminary
    (X87_EQ_NAN x3 -> tail, the {=,NaN} set). Fixed; owner-confirmed in-game.
    Re-verify: the /Fa jcc forms at the routing sites.

  - EXPL_DROID BAKE (2026-11-26, the step-3b scan catch): expldroid.cpp:73
    setHook(0x40215F) MID-FUNCTION in update's WORK_2 immediate-fire gate
    (expl1: the id read replayed, is_not_expl flags, the original jz at
    0x402165 consumes) - the live gate is master id IN expl_list (the
    suicide-attack txt list), not == 1239; update_new hooked at the entry
    dead-reverted it (the body walks verified the UNPATCHED listing - the
    scan class the step-3b rule exists for). BAKED conditional on
    expl_installed (the vanilla 1239 gate when not installed); externs
    cpp-local (the SPEED2 precedent); the site retired; the 3 sibling expl
    hooks stay live in unowned TRIBE_Combat_Object__update/attack3/
    SoldierAI ctor - STANDING BAKE INVENTORY for those decodes. master id
    1239 = the vanilla-only sentinel (the registry note). /Fa: the flag
    shape + the 1239 immediate. Re-verify: the bake emission at the gate.

  - UPDATE_NEW DESYNC x2 ROOT-CAUSED + FIXED (2026-11-26, the upd A/B
    harness - 2,232,501 pairs): (1) the WAIT-case return written OUTSIDE
    the looped==1 if - vanilla looped != 1 -> jnz def return 0 (0x4021F3),
    the 1/5 return INSIDE (0x40222F); fired once (the
    WAIT-with-uncompleted-anim call is rare), the ret 0-vs-1 shifted the
    sequence at pair 2232500. (2) the MAIN CTOR dropped the asm 0x40123C
    [esi+5Ah]=bl store (need's twin) - ind = heap garbage, branchable at
    the WORK_2 !target && !indirect gate; RESTORED = 0 (Attack2 = 2 and
    the loader streams were correct; the full store census all three
    creation paths FULL MATCH). The ind garbage (orig heap values vs 0,
    ~3.6K pairs, ZERO outcome flips) = benign - both patched machines
    share one binary/heap; the loggers logged it boolean. Re-verify: the
    emission at the gate.

  - ATTACK PASS-2 RESTRUCTURES + HARNESS REMOVED (2026-11-26, tests clean):
    ready_to_attack_new: 5 gotos/3 labels -> 0 (FAILED inlined x3, IN_RANGE
    = the post-if/else convergence, LADDER_REGION = the sequential fall);
    update_new: 12 gotos/12 LABELs -> 0, the v-names dissolved, the staged
    flow (the head / the target-validity flag / the position-sync / the
    main stage / the switch), WORK_2 = the firePhase/spritePhase flags
    (every re-check kept incl. the post-set7 need re-read at 0x402095),
    fireNow replaces goto LABEL_146; the state switch tables byte-identical
    (0,1,6,2,2,3,4,6,6,6,2,6,5 + the DD partitions), the label->body
    mapping verified in-range, the immediates intact; trace-proven by the
    owner's A/B runs. The DIAG harness REMOVED (the Gather precedent):
    both trampolines/flips/loggers deleted (174 lines), the installer
    hooks _new directly (0x4023B0 -> ready_to_attack_new, 0x401970 ->
    update_new), the zero-symbol sweep = 0 (file + listing), the emission
    diff = the game functions byte-identical (only install_RGE_Action_
    Attack changed, the DIAG comments). THE ATTACK TIER SHIP-STATE: 14
    hooks, every desync closed, 0 gotos in the file, gate 0/0.
