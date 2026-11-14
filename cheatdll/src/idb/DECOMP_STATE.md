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
