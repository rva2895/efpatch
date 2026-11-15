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
