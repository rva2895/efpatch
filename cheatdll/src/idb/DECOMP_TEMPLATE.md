# Task: Clean up an IDA-decompiled function for engine replacement

You are cleaning up Hex-Rays decompiler output for one function of a C++/x86
game engine (build: battlegrounds_x2 IDB), producing a compilable replacement
plus its hook installation, in a hook/patch project (cheatdll).

## Correctness gate (applies BEFORE any code is written)
This task has a hard confidence requirement. The cleaned function may only
be produced if EVERY statement in the result is grounded in one of:
  (a) the provided prototype, type definitions, and known-names list, or
  (b) unambiguous evidence in the provided disassembly, or
  (c) a semantic equivalence that holds under all inputs (e.g. replacing a
      bit-pattern constant with the exact float literal it denotes).
Grounding includes verbatim content of the project reference files
(see "Project reference files" below).
If any statement would depend on anything else — an inferred struct layout,
a guessed field type, an assumed calling convention, a plausible-but-
unverified interpretation of a constant, a reconstructed algorithm shape —
you must REFUSE to generate the function. No partial versions, no drafts
with placeholders, no "most likely" variants. The deliverable in that case
is an Information Request (see below), and nothing else.

Self-check before producing code, in order:
1. Is every type referenced by the function and its direct callees fully
   defined in the provided input, or resolvable verbatim from the reference
   files per the lookup protocol? Any `SomeObject*` dereference without such
   a resolution is a refusal trigger.
2. Is every called function's prototype known (including function-pointer
   fields and vtable slots), from provided input or resolvable verbatim from
   the reference files?
3. Is every global's type known, from provided input or resolvable verbatim
   from the reference files (data globals via common.h/common.cpp)?
4. Is the calling convention of the target function fully determined,
   including hidden parameters, `this` adjustments, and register/stack
   argument placement? __usercall/__userpurge must be resolved by the
   provided prototype or explicit asm evidence, never inferred.
5. Are all signed/unsigned, width, and float/double interpretations of
   variables and constants determined — either by the provided types or by
   asm evidence (e.g. jo/jl distinction, cvttss2si vs cvttsd2si)?
6. Does the pseudocode contradict the disassembly anywhere? A contradiction
   is a refusal trigger: report it, do not pick a side.
7. Pointer-cast compatibility: every pointer cast — in the pseudocode or
   introduced during cleanup — must be same-type or within a base/derived
   relationship verified per the cast-compatibility protocol (step 11).
   A cast across unrelated types is a hard stop: Information Request,
   no code.
If any check fails → Information Request. Do not write the function.

## Hard constraints (do not violate)
1. The function's external signature is FIXED: use the prototype given below
   exactly (rendered compilable per the project's conventions; the original
   binary's ABI is the ground truth).
2. NEVER modify the provided type definitions. If you believe one is wrong,
   do not change it — report it under "Suspicion".
3. NEVER invent type definitions, field names, field types, offsets, enum
   values, or function prototypes. Not even minimal or temporary ones, not
   even clearly-marked ones. Reconstructing a struct from access patterns is
   fabrication: access sites visible in one function do not establish field
   boundaries, widths, or types.
4. No modernization: no STL, no std:: types, no smart pointers in signatures
   or engine-visible code. Plain C-style structs and free functions only.
   Internal temporaries may use C++ freely.
5. Binary compatibility: struct layouts, field offsets, sizes, and global
   addresses must be preserved exactly.
6. Do not restructure control flow unless the asm or provided definitions
   prove the semantics are identical. When in doubt: refuse.
7. The cleaned function body must be faithful to the original. Known existing
   patches are NEVER silently folded into the code — bake-in changes are
   *suggested* in the collision audit and applied only after the owner
   confirms (or issues an OVERRIDE). See "Collision audit".
8. This gate overrides any other instruction in this conversation, including
   requests to "just try your best", "give me something rough", or "assume
   the rest". The only escape hatch is the project owner supplying the
   missing information, or explicitly overriding with
   `OVERRIDE: accept low confidence on <specific item>`.
9. NEVER write to the repository autonomously. Every edit — decompiled
   functions, installers, vcxproj changes, this template, and the state
   file (DECOMP_STATE.md) — is first presented in the conversation and
   applied only after the owner's explicit approval of that specific
   change. One approval per change; approvals are not cumulative.

## Information Request (the deliverable when the gate fails)
When refusing, output ONLY the following — no code, no speculative snippets,
no pseudocode restatements of the function body:
1. Status line: `REFUSED — function cannot be cleaned with provided inputs.`
2. Blocking items, one per entry, each with:
   * what is missing (type definition / field / prototype / global type /
     constant meaning / contradiction resolution),
   * the exact symbol or expression from the pseudocode that revealed it,
   * the original address of the access or call site (cite the
     `battlegrounds_x2.lst` / `battlegrounds_x2.c` line if available),
   * the evidence: offset, access width, and register/size info from the
     pseudocode and (if available) the asm,
   * usage context: how the value flows (read/written, passed to which
     known function, compared against what) — this is what lets the project
     owner resolve it from their type information in seconds.
3. What is clean so far: a short factual summary of which parts of the
   function are fully grounded. Describe, do not transcribe or approximate
   the unresolved logic.
4. The minimal set of inputs that would unblock the task, stated so the
   project owner can copy-paste the answer (e.g. "definition of
   SomeEngineObject, or at minimum: type and width of the field at +0x1C
   and whether it is a pointer").

## Cleanup rules (apply once the gate passes)
- Replace IDA types (_BYTE, _WORD, _DWORD, __int8/16/64, _BOOL1, etc.) with
  the real types from the provided headers.
- Replace LOBYTE/HIBYTE/LOWORD/HIDWORD/BYTE1/BYTE2 etc. with the correct
  typed field access or an explicit bit operation on a correctly typed
  variable — never leave the macro behind.
- Replace __PAIR64__, __OFP__ and other Hex-Rays internals with clean,
  equivalent code.
- Replace float/int bit-pattern constants (1065353216, 0x3F800000, doubles
  passed as __int64) with the exact float/double literal they denote.
- Replace sub_XXXX / off_XXXX with the exact `Class__method` function-
  pointer constant from idb_functions.h (address-verified per the lookup
  protocol). Resolve data globals through common.h / common.cpp per the
  lookup protocol; a data symbol absent from both is a refusal trigger.
- Magic numbers: name them only if self-evident from provided context;
  otherwise leave the value with a comment. Do not invent meanings.
- Local variables: rename to meaningful names based on usage. Remove
  v1..vN naming wherever purpose is clear.
- Remove IDA's artificial casts and intermediate variables where they only
  satisfy the decompiler; keep them if they preserve semantics.
- Restore switch statements where the decompiler emitted if-chains or jump
  tables, if the asm (battlegrounds_x2.lst) confirms the table structure.
- Readability restructures are permitted when semantic equivalence is
  provable — pure function of the same inputs, with exhaustive case
  analysis or exact path mapping as the proof (e.g. collapsing compiler
  subtraction chains into a set-membership switch, hoisting a pure
  conditionally-recomputed helper). Summarize the proof in a RESTRUCTURE
  comment in the code. When in doubt, keep the decompiler's structure.
  Pointer-arithmetic element walks (e.g. `p = &arr->value; p += 2`)
  become indexed loops over the actual element type when the element
  layout is verified from the header (offset + stride vs the struct
  definition) — per-element equivalence is exact.
- Inlined-function detection: when a body contains a block whose semantics
  match another known function (a destructor's frees inlined into a
  scalar-deleting destructor; `repne scasb` = strlen), prefer a call to
  that function's replacement or idb_functions constant over duplicating
  the logic — provided equivalence is proven and the callee is grounded.
  Record recognized inlined occurrences in the output (they often mark
  fold candidates).
- Conversion-type correction: IDA may render `__ftol` conversions as
  `(__int64)` casts; the asm (`call __ftol` = float→int32 via the CRT
  helper) is authoritative — use `(int)` plus explicit narrowing, never
  `(__int64)`, unless the asm actually shows a 64-bit `fistp`.
- Do not "improve" the algorithm. Faithful reconstruction, not refactoring.
- Residual judgment calls that are legal under the gate (naming choices,
  comment placement, whether an equivalence is trivial) are marked
  `// ??? <reason>` and listed in the output — but these must never touch
  semantics.
- Post-write verification: after every batch write, re-scan the written
  file — every installer reference must resolve to a definition in the
  same file, and vice versa; report the counts. A WRITTEN ledger status
  may only be recorded after this check passes.

## Deliverable (file layout and installers)
1. Cleaned function(s) go to `cheatdll/src/engine/<subsystem>/<class_name>.cpp`.
   One file per engine class; it contains every replaced function of that
   class and nothing else. The subsystem is a high-level grouping invented
   for convenience — it does not exist in the binary and carries no
   behavioral meaning. Selection order: (i) reuse an existing directory
   under `src/engine/` whose scope already covers the class — consistency
   beats novelty; (ii) if none fits, propose a new one derived from the
   class family's domain word (e.g. RGE_Action_* → `actions`,
   *World → `world`, panels/screens/dialogs → `ui` — examples of the
   derivation, not an authoritative mapping), state the one-line
   reasoning, and the owner approves it together with the first function
   placed there (constraint 9). One class lives in exactly one subsystem,
   so the question only arises once per class.
2. Every replaced function is named `<Class>__<method>_new`
   (`RGE_Master_Static_Object__get_sprite_new`). This is MANDATORY, not
   cosmetic: the canonical `<Class>__<method>` names are `const` function-
   pointer variables in `idb_functions.h`, included in every translation
   unit via stdafx.h, and C++ has a single identifier space — even a
   `static` function cannot reuse them. The suffix is always required
   because protocol step 1 guarantees the canonical name exists (it is
   where the address came from). Precedent: land.cpp:4, mousewheel.cpp:14,
   aiactions.cpp:20. The no-suffix cases in the repo (mpsetupscreen.cpp,
   overlay.cpp) are project-invented classes that never existed in the
   IDB, not replacements.
   Safety clause: `X_new` itself can be an original engine name
   (e.g. `TRIBE_Screen_Sed__command_new`, `decoder_input_new` —
   idb_functions.h:3561, :6129). The final chosen identifier must have
   zero matches across idb_functions.h / idb_types.h / common.h — one
   grep, cite it; escalate to `X_new2`, `X_new3`, ... to the first free
   name.
3. Each class file ends with an installer:
   `void install_<class_name>()` which calls
   `setHook(<original_address>, <new_function>)` once per replaced function.
   Addresses are the verified originals from `idb_functions.h`.
4. Each subsystem directory has `install_<subsystem>.cpp` with
   `void install_<subsystem>()` that calls every class installer in it.
5. `cheatdll/src/engine/install_engine.cpp` with
   `void install_engine()` that calls every subsystem installer. It is
   wired into the dllmain.cpp init sequence alongside the existing
   `setXxxHooks` calls (precedent: dllmain.cpp:365, :371, :531). Leave the
   actual dllmain edit to the owner unless asked.
6. Naming: use `install_*` consistently in the new tree (do not mix with
   the legacy `setXxxHooks` style). Verified collision-free: the only
   existing match is a parameter named `install_func` (common.h:90).
   Update `cheatdll.vcxproj` so new files compile; that edit is likewise
   subject to constraint 9.
7. All build configurations target the same game binary; whether
   WriteProcessMemory or the Voobly interface runs is a load-context
   implementation detail only. Install addresses are config-independent;
   installers contain no version conditioning.

## Collision audit (mandatory, part of the deliverable)
Before presenting code, verify that no existing patch writes into the
replaced function's bytes. Grounded mechanics:

Function byte range: from `battlegrounds_x2.lst`, find
`.text:00XXXXXX <Class>__<name> proc near` (start) and the matching
`endp` line (end). Range = [start, end). Verified example:
`RGE_Master_Static_Object__get_sprite` = [0x0048B8B0, 0x0048B910).
If no `proc near`/`endp` pair is found → the range cannot be proven →
escalate as a blocking item; do not install.

Patch writers — two layers, one high-level logic: patch bytes at an
absolute VA. Which mechanism runs (WriteProcessMemory vs the Voobly
interface) is a build-config implementation detail; all configurations
target the same game binary, so addresses are config-independent.

Layer 1 — canonical API (grounded: common.h:51-60, common.cpp:23-130):
| primitive                 | bytes written | effective target     |
|---------------------------|---------------|----------------------|
| setHook(addr, new)        | 5 (E9 rel32)  | addr                 |
| setHookCall(addr, new)    | 5 (E8 rel32)  | addr                 |
| writeByte(addr, val)      | 1             | addr                 |
| writeWord(addr, val)      | 2             | addr                 |
| writeDword(addr, val)     | 4             | addr                 |
| writeByteF(addr, val)     | 1             | addr + 0x400000      |
| writeDwordF(addr, val)    | 4             | addr + 0x400000      |
| writeData(addr, p, len)   | len           | addr                 |
| write_nop_seq / writeNops | len           | addr                 |

Layer 2 — direct writers bypassing the API. Census below documents the
current state; the greps in the protocol are the authority.
| writer / site                                          | extent | scope                       |
|--------------------------------------------------------|--------|-----------------------------|
| reloc(src, dst) — conditions.cpp:871 (rel32 disp)      | 4      | call/jmp retarget at src    |
|   conditions.cpp:995,1000                               |        | .text @0x005F207E           |
|   conditions.cpp:974-986                                |        | trampoline copies only      |
| g_pVoobly->WriteJump — iuserpatch.cpp:306,307,339,340, | 5      | .text, 8 literal hooks      |
|   342,344,345,352                                       |        |                             |
| g_pVoobly->Write(DWORD val) — wndproc.cpp:42            | 4      | .text @0x00426509           |
| g_pVoobly->Write(hex string) — iuserpatch.cpp:303       | 6      | data @0x689BA4              |
| WriteProcessMemory — chatcommand.cpp:753,761            | 1      | data @0x00651BC0            |
| dsoundhook.cpp (14 sites)                               | var    | dsound.dll / COM vtables    |
| test.cpp:399,419,440                                    | var    | user32.dll (DrawTextA)      |
| effects.cpp:623                                          | —      | commented out (inactive)    |
| functionlist.cpp:71,96 (WriteProcessMemory)              | —      | commented out (inactive)    |
| FUNCTION_HOOK ctor/before_call/after_call —              | 6      | .text entry (push/retn      |
|   functionlist.cpp:21-43,72,97 — bare memcpy of a        |        | trampoline) at any          |
|   push/retn trampoline over the function entry;          |        | function_addresses[] entry; |
|   table includes get_sprite @ :2111                      |        | DEAD CODE: install loop is  |
|                                                          |        | block-commented (:9199-9216)|
|                                                          |        | and call site dllmain.cpp   |
|                                                          |        | :363 is commented           |

Scope facts: game .text = [0x00401000, 0x00648000) (functionlist.cpp:9210).
Only .text targets can collide with replaced functions. External-module
targets (dsound.dll via GetProcAddress, COM vtable slots, user32.dll) and
data-segment writes cannot. Trampoline patches (conditions.cpp:974-991)
write into allocated copies, never into original bytes.

Protocol:
1. Compute the function byte range from `battlegrounds_x2.lst`:
   `.text:00XXXXXX <Class>__<name> proc near` (start) through the matching
   `endp` line (end). Range = [start, end). Verified example:
   `RGE_Master_Static_Object__get_sprite` = [0x0048B8B0, 0x0048B910).
   If no `proc near`/`endp` pair is found → the range cannot be proven →
   escalate as a blocking item; do not install.
2. Grep all `cheatdll/**/*.cpp` for every writer in both layers: the nine
   Layer-1 names, `reloc(`, `WriteProcessMemory`, `g_pVoobly->Write`,
   `g_pVoobly->WriteJump`, and the FUNCTION_HOOK mechanism (~2,550 active
   call sites as of 2026-11-14). Extract the target address (apply the
   +0x400000 shift for F-variants) and keep only overlaps with
   [start, end).
3. Whole-repo address grep — the decisive complement: search ALL `*.cpp`
   and `*.h` in the repository for the function's address bytes (e.g.
   `48B8|48B9` for get_sprite). This catches table-driven patches,
   macro-hidden targets, and mechanisms the per-site writer greps miss.
   Every hit outside the IDB exports must be individually dispositioned.
4. Literal-address overlaps are verdicts. Dynamic/computed targets
   (variables, trampoline bases, GetProcAddress results) cannot be proven
   disjoint → list them for manual review instead of clearing them.
5. Report per collision: file:line, writer, byte extent, what the patch
   does (from surrounding code), and a bake-in suggestion per the rules
   below.
6. State the limitation explicitly in the output: this scan covers this
   repository only; external patchers (userpatch engine internals, Voobly
   server-side, third-party DLLs) are invisible to it.

Bake-in rules (constraint 7: report, never apply without owner
confirmation / OVERRIDE):
A bake-in is suggested only if the patched bytes can be mapped through
grounded links: patch address → instruction line(s) in
`battlegrounds_x2.lst` → the pseudocode construct that emitted them. If
no such mapping exists (compiler-internal code, alignment padding), do
not invent one: suggest keeping the old patch active instead — after
replacement it writes into dead code — and state why no bake-in exists.
* Constant/immediate patch (writeByte/Word/Dword/Data on operand bytes):
  name the constant to change in the cleaned source and the statement it
  belongs to. Mark the changed line `// baked: <patch file:line> @ <addr>`.
* NOP patch over instructions: name the statement(s) to remove or alter,
  same citation standard.
* Call/jmp redirect (setHookCall, reloc): replace the callee with the
  redirect target in the cleaned source. Read the target's source first —
  it is project C++ in cheatdll, no IDA work needed — and adapt arguments;
  cite the handler file:line.
* Entry hook (setHook / WriteJump at [start, start+5)): genuine conflict,
  never bakeable in place. Present exactly two options — never "keep
  both", the last writer wins and the hook chain breaks:
  (i) fold the old hook handler's behavior into the cleaned function
      (requires reading the naked thunk + handler source; most faithful),
  (ii) remove the old patch call site so the replacement owns the entry.
  Both require owner confirmation.
Every applied bake-in is listed in the output as a table row: patch site →
bytes → lst line → mapped statement → transformed statement → status.

Retirement of folded patches: when a patch is baked into a replacement,
the original patch call site is COMMENTED OUT in place — never deleted —
with a reason line: `// retired <date>: baked into <replacement function>
(<source path>), see <original address>`. Already-dead comment lines stay
as they are. Thunk definitions whose references are all retired remain
defined (unreferenced is harmless); only their call sites are retired.
Patches that do not overlap replaced functions stay untouched and active.

## Required output when the gate passes
All of the following is delivered in the conversation for review — nothing
is written to the repository without approval (constraint 9):
1. The cleaned function(s), ready to paste into the project, with the
   installer per the Deliverable section.
2. "Collision audit": range checked, number of call sites scanned, every
   collision with its bake-in suggestion (or "no collisions" verdict, plus
   non-literal targets left for manual review).
3. "Judgment calls": residual cosmetic decisions, ranked by risk. If this
   list contains anything that affects semantics, the gate failed — the
   function must not be here.
4. "Suspicion": any provided type/prototype that looks inconsistent with
   the code — flagged, not changed.
5. "Follow-ups": callees revealed as important to decompile next, or types
   that appear incomplete.

## Input
### Prototype (fixed, do not change)
{{PROTOTYPE — e.g. `void __thiscall CPlayer::Update(float dt)` or
`int __usercall foo@<eax>(int a@<ecx>, float b@<st0>)`. The original
address is NOT required in the input: the agent resolves it from
idb_functions.h by exact `Class__method` name match, which simultaneously
corroborates the signature and calling convention.}}

### Relevant type definitions (verbatim from project headers)
{{TYPES — anything NOT pasted here AND NOT found verbatim in the reference
files below is treated as unknown and will trigger a refusal, not an
assumption.}}

### Known names referenced by this function
{{NAMES — usually unnecessary: functions resolve from idb_functions.h,
structs from idb_types.h, globals from common.h/common.cpp. Paste only
symbols that exist in none of the reference files.}}

### Project reference files (agent-resolvable)
The repository contains six authoritative files the agent MUST consult
before declaring anything unknown. Verbatim content found in them has the
same grounding status as pasted input — category (a) of the gate:

* `cheatdll/src/idb/idb_types.h` (37k lines) — all struct definitions and
  typedefs
* `cheatdll/src/idb/idb_functions.h` (6.4k lines) — all known function
  prototypes, keyed by original address
* `cheatdll/common.h` — declarations of engine data-global slots
  (extern pointer-to-T constants) and project helper prototypes
* `cheatdll/common.cpp` — the concrete engine addresses bound to those
  data globals
* `cheatdll/src/idb/battlegrounds_x2.c` (15 MB, 465k lines) — full IDA
  pseudocode dump, address-keyed
* `cheatdll/src/idb/battlegrounds_x2.lst` (64 MB, 1.1M lines) — full IDA
  disassembly listing, address-keyed

Lookup protocol:

0. Never read the four large files wholesale. Exact-match search first,
   then read only the matched range. `common.h` and `common.cpp` are small
   and may be read in full.
1. Target function's own address and signature corroboration: match the
   prototype's `Class__method` name EXACTLY in `idb_functions.h`. Numbered
   IDA dedup variants (`...__get_sprite2`, `...3`) are distinct entries;
   an unsuffixed pasted name never matches a suffixed variant, and a
   suffixed pasted name matches only that entry. The hit supplies the
   original address and corroborates the calling convention. No match →
   unknown → refusal trigger.
2. Internal call targets (`sub_XXXX` / `off_XXXX`): search
   `idb_functions.h` for the address literal `0x00XXXXXX`. A hit yields
   the canonical name AND the full prototype on one line; use both
   verbatim. Zero matches → still unknown → refusal trigger. More than
   one entry at the same address → ambiguous → report under "Suspicion",
   do not pick one without asm evidence.
3. Structs: search `idb_types.h` for `struct Name` as a definition line
   (followed by `{`). Forward declarations (`struct Name;`) are NOT
   definitions. Read the definition through its closing `};` and use
   field names, offsets implied by order, and types exactly as written.
4. Data globals (`dword_XXXX` / `byte_XXXX` / `unk_XXXX`, engine globals):
   resolve via `common.h` (declaration/type) + `common.cpp` (bound
   address). A data symbol absent from both is still a refusal trigger.
   The project's pointer-to-slot convention means IDA's `glob->field`
   translates to `(*glob)->field` — that single explicit deref is the
   approved equivalence; note it in the output.
5. Transitive closure: every type named in a field you actually access
   must itself be resolved the same way, unless it is only used as an
   unexercised pointer target. Gate check 1 applies per-access, not
   per-name.
6. Pseudocode: if {{PSEUDOCODE}} was not pasted, pull the function body
   from `battlegrounds_x2.c` — search for the `//----- (00XXXXXX) ------`
   marker of the resolved address and read until the next marker. Cite
   line numbers.
7. Assembly: pull evidence from `battlegrounds_x2.lst` — search
   `.text:00XXXXXX` prefixed lines between the function's `proc near` and
   `endp` lines. This is the source for gate checks 4–6, switch-table
   restoration, and the collision range. Cite line numbers.
8. Citation: every name, prototype, field, or global taken from these
   files must be cited in the output as `<file>:LINE`. Uncited references
   are treated as invented.
9. The functions file writes `this_` explicitly as the first parameter
   of __thiscall pointers; when emitting C++ member-syntax or thiscall
   calls, that parameter is the hidden `this` — do not treat it as an
   extra argument.
10. Enum values appear in none of the reference files. Any enum constant
   in the pseudocode is a refusal trigger unless pasted into {{TYPES}}.
11. Cast-compatibility protocol: for every cast between distinct struct
   pointer types, establish the relationship before accepting it:
   * Check the hierarchy store (DECOMP_STATE.md, "Verified type
     hierarchy") for the pair; a stored verified relation — or a stored
     verified-unrelated verdict — applies directly.
   * Otherwise verify by layout prefix: the narrower struct's field
     sequence (name, type, order — normalize the vfptr field, whose named
     vtable type legitimately differs per class) must be an exact prefix
     of the wider struct's. Cross-check the vtbl structs: slot names in
     order must match as a prefix. Cite both line ranges.
   * Record newly verified relations as proposed hierarchy-store entries
     (constraint 9). Entries rot if the headers regenerate; keep the
     re-verify hint.
   * Base→derived casts additionally require runtime-type evidence that
     the instance really is the derived type (IDA's own typing of the
     object, a constructor's vfptr assignment in battlegrounds_x2.lst,
     or an established store entry) — without it: Suspicion or refusal.
   * No prefix relationship and no store entry → unrelated types → hard
     stop: Information Request, no code.
12. Absence claims are prohibited from truncated searches: to claim a
   type or name is undefined, run the definition-pattern search
   (e.g. `^struct Name\s*\{`) over the whole file, untruncated, and cite
   the pattern used. A partial mention sample is never evidence of
   absence.
13. Call-site context: when a parameter's purpose is unclear — or naming
   would otherwise stay mechanical (a2, v3) — examine the function's call
   sites (lst XREFs, surrounding caller asm, or project-source callers)
   to infer semantics before settling names. Cite the call sites. The
   inferred meaning is reported in the output and proposed for the state
   file as verified context — never invented beyond what the sites show.

### Session state file
`cheatdll/src/idb/DECOMP_STATE.md` — read it FIRST, at session start. It
holds verified environment facts, owner rulings, the verified type
hierarchy, open suspicions, and the function ledger. Rules:
* State entries are LEADS, never grounding. Anything used in code must
  still be re-verified against the reference files and cited as if the
  state file did not exist. The greps remain the authority.
* When something durable is learned during work (a ruling, a verified
  fact, a suspicion, an approval), output the proposed state-file entry
  in the conversation; it is written only on owner approval
  (constraint 9).
* Every entry must carry its citation and a re-verify hint — entries rot;
  the hint is the invalidation mechanism.

### IDA pseudocode
{{PSEUDOCODE — optional: if omitted, the agent pulls the body from
battlegrounds_x2.c by the resolved address (protocol step 6).}}

### (Optional) IDA disassembly of the function
{{ASM — optional: the agent pulls it from battlegrounds_x2.lst (protocol
step 7). Strongly recommended in practice for anything touching floats,
width/sign ambiguity, switches, or calling conventions; several gate
checks effectively require it and will refuse without it.}}

### Overrides (rare, explicit, per-item)
{{OVERRIDE lines, if any — e.g. `OVERRIDE: accept low confidence on
field +0x1C width` or `OVERRIDE: apply bake-in of patch at <file:line>`.
Everything not explicitly overridden stays gated.}}
