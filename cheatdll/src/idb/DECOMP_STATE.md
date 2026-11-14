# DECOMP_STATE — session state for the decompilation workflow

Read this file FIRST, at session start. Companion to DECOMP_TEMPLATE.md.

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
  prefix, `world` at :16827/:17517 — see Verified type hierarchy). A
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

## Verified type hierarchy

Relations verified by mechanical comparison of field/slot sequences
(vfptr-normalized: the vfptr field's named vtable type legitimately
differs per class). Re-verify after any idb_types.h regeneration.
Verified-unrelated verdicts are recorded here too — they justify future
cast hard stops.

- RGE_Base_Game ⊂ TRIBE_Game — derived extends by 53 fields
  (117 base + 53 = 170); vtbls 99/99 slots, names identical in order.
  Evidence: struct prefix idb_types.h:16759-16876 vs :17449-17619;
  vtbl slot names :16887-16985 vs :17627-17725. Verified 2026-11-14.
  Runtime note: the base_game global is IDB-typed `TRIBE_Game*`
  (battlegrounds_x2.c:43930) — IDA's own runtime-type evidence for
  base→derived access on this chain; repo slot type `RGE_Base_Game**`
  (common.h:102).

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
   the do_setup-gated ctor chain. WRITTEN + post-write verification
  (7 defs/refs identical in the class file; installer gained 3 hooks;
  no literal-width I/O; pointer-walk clean; 3 SEH folds present).
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
