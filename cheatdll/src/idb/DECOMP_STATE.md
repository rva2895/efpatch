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

- Reference file sizes (2026-11-14): idb_types.h 37,047 lines;
  idb_functions.h 6,415 lines (6,403 definitions); battlegrounds_x2.c
  15,217,425 bytes / 464,838 lines; battlegrounds_x2.lst 64,337,921 bytes /
  1,111,401 lines; common.h 109 lines; common.cpp 314 lines.
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
  setHook calls (total 19 for the class). Call-site context (protocol
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
  WRITTEN + post-write verification (Moving file now 12 defs/refs
  identical; CRT sweep 0; pointer-walk 0; literal sweep 0).
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
