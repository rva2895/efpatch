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
If any statement would depend on anything else вЂ” an inferred struct layout,
a guessed field type, an assumed calling convention, a plausible-but-
unverified interpretation of a constant, a reconstructed algorithm shape вЂ”
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
   variables and constants determined вЂ” either by the provided types or by
   asm evidence (e.g. jo/jl distinction, cvttss2si vs cvttsd2si)?
6. Does the pseudocode contradict the disassembly anywhere? A contradiction
   is a refusal trigger: report it, do not pick a side.
7. Pointer-cast compatibility: every pointer cast вЂ” in the pseudocode or
   introduced during cleanup вЂ” must be same-type or within a base/derived
   relationship verified per the cast-compatibility protocol (step 11).
   A cast across unrelated types is a hard stop: Information Request,
   no code.
If any check fails в†’ Information Request. Do not write the function.

## Prototype / type-definition suspicion gate (owner-directed 2026-11-16)

The IDB prototypes and type definitions are usually accurate but
occasionally wrong - a single session caught four prototype mistakes
plus the BVector field-name swap (BPath__nextPatrolWaypoint: a phantom
second argument; BPath__waypoint: the out-copy buffer declared "int"
and the index unnamed; RGE_Moving_Object::moveUnit: a trailing
by-value BVector where the binary passes a pointer;
RGE_Missile_Object::make_object_collision_list: an object pointer
typed float and a float typed int). The following are POINTS OF
EXTREME SUSPICION - each is a HARD STOP: Information Request, the
owner corrects the IDB and regenerates, the decode restarts from the
regenerated files. Never write a replacement that needs a pun, a
guessed signature, or a dead store to live with a suspect prototype;
never resolve a contradiction by trusting either side - present it.

Red flags (each observed this session):
1. A parameter consumed through a pun or exclusively as another type
   (*(float*)&int_param; a float param dereferenced as an object
   pointer: LODWORD(x) + 20 -> master_obj -> movement_type).
2. The retn immediate contradicts the declared stack-argument byte
   total (moveUnit: retn 0xC = 12 bytes vs a by-value-BVector
   prototype implying 20).
3. A "swap" (y/z, x/z, argument order) only explainable by misnamed
   fields or parameters (the BVector .y/.z; the
   make_object_collision_list p1 assignment pattern).
4. The .c argument rendering vs the instruction stream: resolve the
   parameter order via the ABI (right-to-left pushes, last push =
   param1) and the callee's own consumption; a mismatch with the
   declared prototype is a prototype suspect.
5. A phantom or extra parameter (nextPatrolWaypoint's "modifyPath" -
   the leftover-slot rendering; retn 4 proves one stack argument).
6. An out-parameter declared as a scalar but written larger
   (BPath__waypoint's "int wpIndex": 12 bytes copied into it).
7. A caller whose push/argument construction contradicts the
   prototype (make_object_collision_list's caller passes an object
   pointer and position floats).

Resolution protocol: present the contradiction with addresses (the
body's consumption sites, a caller's pushes, the retn immediate, the
regenerated rendering), state the deduced true signature/definition,
and request the owner correct the IDB; after the fix + regeneration,
restart the decode from the regenerated files. Approved corrections
become state-file entries.

## THE TWO-PASS DECODE FOR LARGE FUNCTIONS (owner-directed 2026-11-20, from the BMotion::updateMovement ship)
DECODING APPROACH BY FUNCTION SIZE/COMPLEXITY - the 0x49CB60 movement
driver (6300 bytes, the 13-back-edge dispatch loop, the four path-building
arms) established the two-pass form:

PASS 1 - THE ALIGNED TRANSCRIPTION (the suite-verified artifact): the IDA
rendering transcribed 1:1 into compilable code with NO control-flow
changes - the IDA nesting (its synthetic multi-while loop nests, its
label/goto rendering, its variable reuse v4..vNNN) preserved verbatim;
only value-level fixes land in this pass (the named-finding classes: the
float .z zeroing/carve slots, the AND 0FFh masks, the timestamp sources,
the X87 NaN macros, the (__int64)->(int) conversions). The aligned
transcription is THE test artifact: it is suite-verified against the
vanilla trace first, and every later change is verified against IT.
Rationale: a single-pass rewrite of a large function changes control
flow, naming, AND value semantics simultaneously - a desync then has
three suspect classes at once. The aligned pass isolates the value
semantics: the suite run proves the transcription correct BEFORE any
readability work exists.

PASS 2 - THE RESTRUCTURE (the shipped artifact): built ON TOP of the
verified aligned body - named locals (the v-names dissolved), staged
control flow (gotos/labels dissolved into sequential stages plus
`continue` for the asm back-edges), the IDA synthetic nests flattened.
THE EQUIVALENCE BAR: the restructured body must be behaviorally identical
to the verified baseline - proven by (a) the stage map (each stage = an
asm dispatch region, each `continue` = a cited asm back-edge), (b) the
brace-depth walk at every stage boundary, (c) the /Fa structural
comparison (same call multiset; positional diffs expected - the compiler
re-decides inlining and layout), and (d) THE RE-RUN: the deterministic
replay must reproduce byte-identical traces. NOTE: "0 errors" proves the
braces BALANCE, not that they close the right blocks - the brace-depth
walk at each stage boundary is mandatory (a missing stage close silently
nests the following stage inside the previous one's gate and passes every
compile - the wt=2185372 lesson).

THE RETIREMENT: the aligned transcription is the verification artifact,
NOT a shipped body - at ship it is deleted and the restructured form is
hooked (the `_new` name transfers to the shipped variant). Keep the
aligned body in VCS history: the commit preceding the restructure is the
diff reference for every later audit.

SIMPLER FUNCTIONS (the majority): the single-pass approach per this
template's main flow (asm-grounded decode -> the cleaned named form ->
gate -> suite) remains sufficient and faster - the two-pass form pays for
itself only when the function is large enough that a single-pass rewrite
carries more than one suspect class at once (the working bar: multi-arm
dispatch loops, several hundred decoded lines, or any function whose
first suite run desynced).

## Hard constraints (do not violate)
1. The function's external signature is FIXED: use the prototype given below
   exactly (rendered compilable per the project's conventions; the original
   binary's ABI is the ground truth).
2. NEVER modify the provided type definitions. If you believe one is wrong,
   do not change it вЂ” report it under "Suspicion".
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
   patches are NEVER silently folded into the code вЂ” bake-in changes are
   *suggested* in the collision audit and applied only after the owner
   confirms (or issues an OVERRIDE). See "Collision audit".
8. This gate overrides any other instruction in this conversation, including
   requests to "just try your best", "give me something rough", or "assume
   the rest". The only escape hatch is the project owner supplying the
   missing information, or explicitly overriding with
   `OVERRIDE: accept low confidence on <specific item>`.
9. NEVER write to the repository autonomously. Every edit вЂ” decompiled
   functions, installers, vcxproj changes, this template, and the state
   file (DECOMP_STATE.md) вЂ” is first presented in the conversation and
   applied only after the owner's explicit approval of that specific
   change. One approval per change; approvals are not cumulative.

   STATE-FILE SESSION CONVENTION (owner-directed 2026-11-18): the NEXT
   SESSION paste-prompt blocks in DECOMP_STATE.md are the CLOSED
   handoff records of PREVIOUS sessions - they are never updated after
   their session closes. A NEW block is appended at the END of a
   session, summarizing that session's state for the next one.

## Information Request (the deliverable when the gate fails)
When refusing, output ONLY the following вЂ” no code, no speculative snippets,
no pseudocode restatements of the function body:
1. Status line: `REFUSED вЂ” function cannot be cleaned with provided inputs.`
2. Blocking items, one per entry, each with:
   * what is missing (type definition / field / prototype / global type /
     constant meaning / contradiction resolution),
   * the exact symbol or expression from the pseudocode that revealed it,
   * the original address of the access or call site (cite the
     `battlegrounds_x2.lst` / `battlegrounds_x2.c` line if available),
   * the evidence: offset, access width, and register/size info from the
     pseudocode and (if available) the asm,
   * usage context: how the value flows (read/written, passed to which
     known function, compared against what) вЂ” this is what lets the project
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
  variable вЂ” never leave the macro behind.
- Replace __PAIR64__, __OFP__ and other Hex-Rays internals with clean,
  equivalent code.
- Replace float/int bit-pattern constants (1065353216, 0x3F800000, doubles
  passed as __int64) with the exact float/double literal they denote.
  DD-CONSTANT WIDENING MANDATE (owner-approved 2026-11-18, after the
  angle_to_object desync): every constant consumed from a dd (float)
  memory operand - arithmetic operand, compare operand, or X87_* macro
  argument - is written with the f suffix; a bare double literal is a
  DIFFERENT number (the binary's 6.2831855f = 6.2831854820251464844
  widened; the bare double literal 6.2831855 sits 2e-8..4e-8 away -
  every wrap-add, wrap-compare window, and the facet scale diverged).
  Gate: mechanically compute [double](float)literal as hex and diff
  against the /Fa literal pool __real@ names - the emitted pool must
  carry the widened float's exact bit pattern (angle_to_object's pool:
  401921FB60000000 / 400921FB60000000 / C00921FB60000000 /
  3FC45F3060000000, verified 2026-11-18). In-file exemplar:
  RGE_Moving_Object.cpp:775, (double)6.2831855f.
- Replace sub_XXXX / off_XXXX with the exact `Class__method` function-
  pointer constant from idb_functions.h (address-verified per the lookup
  protocol). Resolve data globals through common.h / common.cpp per the
  lookup protocol; a data symbol absent from both is a refusal trigger.
- Magic numbers: name them only if self-evident from provided context;
  otherwise leave the value with a comment. Do not invent meanings.
- Magic loop counters over declared arrays (hard stop): a loop bound
  over a struct array member must be derived from the declared type вЂ”
  `_countof(array)` (MSVC, stdlib.h; loop counters typed size_t to keep
  the comparison unsigned-clean) вЂ” never a bare numeric literal, and no
  bespoke macros or local-constant substitutes. The dimension comes
  verbatim from the provided type definition, so the derived form is
  grounded (no invention). A hardcoded literal count in replacement
  code is a violation, and failing to convert it before the code is
  written is a HARD STOP: Information Request, no code. (Owner-directed
  amendment 2026-11-14 after the TRIBE_Master_Combat_Object batch
  drafted `i < 3` loops over build_inventory[3]; refined to _countof
  syntax per owner direction вЂ” supersedes the interim macro form.)
  Owner clarification 2026-11-15 (TRIBE_Building linked_owner search):
  the rule binds EVERY loop form (for / while / do-while) and binds the
  DRAFT + presentation stage, not only the post-write sweep - a bound
  derived from a declared array dimension (e.g. a search limit over
  linked_child[4]) is a loop counter over a declared array and must be
  _countof(array) with a size_t counter. Presentations carry the
  _countof forms; a hardcoded dimension literal in a presented draft
  is a protocol violation caught at review.
- Magic-constant game-facts check (owner-directed 2026-11-15): every
  magic constant surfaced by a decode (master_id, object_group, terrain
  id, mode/flag values, flag bits) is checked against
  DECOMP_GAME_FACTS.md before the code is written. Known values
  justify comments and identifier choices (cited to the registry);
  unknown values keep the literal with a comment, and their status
  (UNKNOWN / PARTIAL + asm context) is recorded in the game-facts
  magic-constant registry so the semantics can be deduced later as
  information accumulates. Registry gaps in gating positions (e.g. an
  unknown master_id steering control flow) are raised to the owner as
  an Information Request at presentation time - never silently passed
  over.
- Local variables: rename to meaningful names based on usage. Remove
  v1..vN naming wherever purpose is clear.
- Remove IDA's artificial casts and intermediate variables where they only
  satisfy the decompiler; keep them if they preserve semantics.
- Restore switch statements where the decompiler emitted if-chains or jump
  tables, if the asm (battlegrounds_x2.lst) confirms the table structure.
- Readability restructures are permitted when semantic equivalence is
  provable вЂ” pure function of the same inputs, with exhaustive case
  analysis or exact path mapping as the proof (e.g. collapsing compiler
  subtraction chains into a set-membership switch, hoisting a pure
  conditionally-recomputed helper). Summarize the proof in a RESTRUCTURE
  comment in the code. When in doubt, keep the decompiler's structure.
  Pointer-arithmetic element walks (e.g. `p = &arr->value; p += 2`)
  become indexed loops over the actual element type when the element
  layout is verified from the header (offset + stride vs the struct
  definition) вЂ” per-element equivalence is exact.
- COPY-OUT GETTER ALIASING (owner-directed 2026-11-20, the BPath__waypoint
   lesson): a getter of the form `T* f(this, T* out, index)` that COPIES
   the internal element into `out` and RETURNS `out` (BPath__waypoint
   @0x4BC440: mov ecx,[esp+outVector]; mov [ecx],esi; ...; retn 8 - eax =
   the out-buffer's address) makes EACH FETCH'S RESULT LIVE IN ITS OWN
   BUFFER until the next fetch overwrites it. NEVER collapse per-call-site
   out-buffers into one shared scratch: any site holding TWO fetched
   waypoints live together (the tail-quality checks - newTail from the
   candidate path AND the original path's waypoint in one expression)
   silently compares the second fetch against ITSELF and the discard
   dies. The verified aligned transcription kept IDA's ~25 distinct carve
   slots 1:1 - they were load-bearing. Collapse ONLY single-fetch sites
   (fetch -> consume -> dead before the next fetch), and audit every fetch
   site's liveness window when a scratch is shared.
- Redundant-guard elimination: when a guard is provably subsumed by a
  later condition вЂ” an outer `if (count > 0)` around a loop whose own
  condition `i < count` already fails for count <= 0, a repeated test,
  or a bounds test the index arithmetic implies вЂ” write the collapsed
  form (loop condition only), mark it with a RESTRUCTURE comment
  citing the equivalence (guard condition vs loop/body condition truth
  table, with the asm address of the original guard), and list the
  collapse in the review artifact. When the collapse is not provable,
  keep the original guard. Verification protocol: after drafting, and
  again at post-write, re-scan every new loop and conditional for
  guards subsumed by their own body conditions and report each collapse
  (or state that none was found).
- Inlined-function detection: when a body contains a block whose semantics
  match another known function (a destructor's frees inlined into a
  scalar-deleting destructor; `repne scasb` = strlen), prefer a call to
  that function's replacement or idb_functions constant over duplicating
  the logic вЂ” provided equivalence is proven and the callee is grounded.
  Record recognized inlined occurrences in the output (they often mark
  fold candidates).
- Conversion-type correction: IDA may render `__ftol` conversions as
  `(__int64)` casts; the asm (`call __ftol` = floatв†’int32 via the CRT
  helper) is authoritative вЂ” use `(int)` plus explicit narrowing, never
  `(__int64)`, unless the asm actually shows a 64-bit `fistp`.
- FLOAT PRECISION VERIFICATION (mandatory from 2026-11-16, owner-directed
   after the check_placement desync; THE rule class behind several
   desyncs): the original binary computes float arithmetic on the x87
   stack at 53-bit (double) intermediates (MSVC CRT control word) -
   differences, sums, products, quotients, and sqrt results stay
   UNROUNDED on the register stack until an `fst`/`fstp m32` store or a
   comparison consumes them. Rules:
   (a) Any arithmetic result that feeds a COMPARISON must be computed in
       double intermediates - the asm compares the unrounded 53-bit
       value; a float-rounded operand can flip the comparison. (This
       supersedes the "single + - * is float-safe" shorthand: that held
       only for stored RESULTS, not for comparison-consumed
       intermediates.)
   (b) Direct loaded-float vs loaded-float/constant comparisons
       (fld/fcomp m32 shapes, no intervening arithmetic) are bit-safe in
       float form.
   (c) Division and sqrt ALWAYS need double intermediates (inexact
       operations: the 53-bit rounding followed by an m32 rounding is a
       double-rounding vs the single SSE rounding).
   (d) A single + - * whose result is only STORED (fstp m32) is
       float-safe (two float operands produce an exact 53-bit
       product/sum/difference; the store rounding is the only effective
       one).
   (e) REPLICATE STORE-ROUNDS: when the original rounds an intermediate
       to float via an m32 store and later consumes the rounded value
       (e.g. the check_placement/incrementWaypoint arg_24 |dy|:
       0x5C94E6/0x5C9658 fst, compared at 0x5C952D/0x5C9688), the
       replacement must keep that value float - writing double there is
       a NEW divergence. Read the operand PRECISION per side before
       writing a comparison.
   (f) (int)/__ftol conversions consume the 53-bit operand: the feeding
       arithmetic must be double, e.g. (int)((double)x - 8.0).
   (g) The project compiles /arch:SSE2 + /fp:fast: float expressions
       emit SSE (24-bit rounding per op), double expressions emit
       double ops (bit-identical to x87 PC53). /fp:fast licenses
       reassociation - keep expression trees small and explicit;
       contractually pinned association requires /fp:precise (an
       owner-level project decision: changes all shipped codegen).
   (h) FLOAT-RETURNING REPLACEMENTS ARE DOUBLE-RETURNING (owner-directed
       2026-11-16, the st0 call-boundary corollary): the original game
       consumes game-float-function returns DIRECTLY off st0 at full
       x87 precision - frequently with NO intermediate store (chains
       like call; fsub range; fsub 0.1; fstp - one rounding at the
       end, or a pure passthrough: call; retn). A float-typed return
       in a replacement forces an fstp m32 at the call boundary - a
       24-bit truncation the original never performed; ULP deltas
       flip downstream gates (the incrementWaypoint desync class).
       ALL float-returning game-function prototypes are
       double-returning in idb_functions.h (86 defs, verified
       2026-11-16); every replacement with a float return declares
       double. float and double both return in st0: the declaration
       change is binary-equivalent for the untouched callee binaries;
       it removes the compiler-inserted rounding in OUR callees and
       callers. Per-site discipline: capture 53-bit where the
       original consumed st0 in a chain; round to float immediately
       (explicit (float)) where the original did fstp m32 - cite the
       fstp address at the cast. IDB RESIDUAL: the vtbl slot
       declarations in idb_types.h still type these slots float
       (getSpeed/getTurnRadius/maximumSpeed/handle_teleport_z_
       correction/calculateFutureVelocity across the runtime vtbls);
       no owned code dispatches them today - any future replacement
       calling one of those slots must request the slot re-typing
       from the owner first (a stale float slot would reintroduce the
       call-boundary rounding).
   Post-write sweep: for every float comparison and every (int)
   conversion in the written function, trace the asm operand - if the
   operand is an x87 stack value with arithmetic upstream, the written
   form must be double; if it is a direct memory load, float is
   bit-safe. Store-and-read diff blindness note: this class is
   invisible to the store diff and the compile gate (check_placement
   desynced with a faithful control flow); only this operand-precision
   trace catches it.
   (i) X87 COMPARE-BRANCH UNORDERED POLARITY (owner-directed
       2026-11-18, after the calculateYawAngle desync - NaN newOF made
       result.z NaN and the C <= took the wrong turn direction,
       +/-pi/2): the x87 idiom "fcomp; fnstsw ax; test ah,41h; jnz"
       (the C0|C3 <= / >= shapes) is UNORDERED-TRUE - NaN takes the
       branch - because unordered sets C0=C2=C3=1. The same holds for
       the C0-only "test ah,1" shapes (the taken set includes
       unordered whenever the jcc fires on C0 alone). MSVC's SSE
       codegen for C's <= / >= / < / > is UNORDERED-FALSE (the IEEE
       predicate), so a literal transcription of those branches
       DIVERGES on NaN inputs. THE FAITHFUL FORMS: for an x87
       C0|C3-taken branch write the NEGATED OPPOSITE, !(x > c) /
       !(x < c) (compiles comiss/comisd + jna/jbe = unordered-taken);
       for a C0-only-taken branch write !(x >= c) / !(x <= c)
       (jb/ja). Verify the emitted jcc at the gate (jna/jbe/jb/ja -
       NOT setcc+boolean folding). POLARITY TABLE: when the x87
       jnz/jz targets the POSITIVE branch (<= / >= taken), the literal
       C form diverges - use the negated form; when it targets the
       NEGATIVE branch (the > / < skip, e.g. maximumSpeed's
       base_speed > 0 cap gate whose jnz leads to the plain-speed
       path), the literal C form (NaN-false -> the skip) already
       matches - keep it. Every owned x87 compare branch is swept
       against this table (the sweep found calculateYawAngle's three
       sites divergent; maximumSpeed/calculateMaximumSpeed's cap gates
       match; moveUnit's fabs >= 0.5 pair and incrementWaypoint/
       checkRange's 1e-7 gates are per-site polarity-checked at their
       decode). MACRO MANDATE (owner-approved 2026-11-18): the
       unordered-inclusive compares are written ONLY via the
       X87_LE_NAN / X87_LT_NAN / X87_GE_NAN / X87_GT_NAN /
       X87_EQ_NAN / X87_NE_ORD macros (cheatdll/x87_nan.h, included
       from stdafx.h); the literal operators are reserved for the
       polarity-table MATCH rows (the x87 jnz/jz targeting the
       negative/skip path). Each macro call still carries its asm
       citation; the /Fa gate check verifies the emitted jcc per
       macro-using function (the expansion must not fold).
       PROBE-VERIFIED COMPLETE TABLE (owner-directed 2026-11-18; a
       compiled-and-EXECUTED probe under the project's real Release
       flags /O2 /arch:SSE2 /fp:fast - and /Od, results identical -
       qNaN vs constant): x <= c FALSE; x < c FALSE; x > c FALSE;
       x >= c FALSE; x == c FALSE; x != c **TRUE** (the ONE literal
       that is unordered-TRUE); !(x > c) TRUE; !(x >= c) TRUE;
       !(x < c) TRUE; !(x <= c) TRUE; double-typed forms and fabsf
       forms identical. RULES DERIVED: (1) every literal ordered
       predicate (<, <=, >, >=, ==) is unordered-FALSE in compiled
       MSVC - NEVER assume an x87 unordered-true idiom survives a
       literal transcription; (2) `!=` is unordered-TRUE - the x87
       "test ah,40h; jnz -> SKIP" gates (equality-with-skip, e.g.
       setForwardVelocity/Acceleration's lazy-create gate) must be
       written as the ORDERED-NONZERO disjunction
       ((v < c) || (v > c)), not `v != c`; (3) x87 branch-FALL-THROUGH
       update idioms ("fcom mem; test ah,1; jz -> discard", e.g. the
       min-scoring updates in precalcDistToTarget/find_evacuate_area)
       fire on unordered - write the negated >= (the score is NaN ->
       the update happens in the original); (4) value-return equality
       (`return a == b`, compiled sete: unordered -> ZF=1 -> 1)
       MATCHES the x87 test ah,40h idiom - safe (missile_move);
       (5) branch equality (`if (a == b)`) is IEEE-false on NaN and
       DIVERGES from the x87 C3-taken set - use the composed form
       (!(x < c) && !(x > c)) when a x87 ah,40h branch is taken-set
       {=, NaN}. PROBE PROTOCOL (mandatory for NaN-form decisions):
       the probe is compiled AND EXECUTED under the project's real
       flags (the vcxproj Release: /O2 /arch:SSE2 /fp:fast + /GL) with
       a runtime-seeded NaN (unfolding-able), both /O2 and /Od; asm
       listings (/Fa) verify the emitted jcc per form. The probe
       source is the reference implementation of this table.
   (j) WIDEN-BEFORE-OP /Fa SIGNATURE (owner-approved 2026-11-18; the
       /Fa check that caught angle_to_object's second desync class):
       rule (a) is not satisfied by a double-DECLARED variable - the
       written form `double d = floatA - floatB;` compiles under
       /fp:fast to subss/addss/mulss + cvtps2pd AFTERWARD, a float
       rounding the x87 original never performed (~6e-8 relative on
       far-apart game coordinates, where the 53-bit exact difference
       spans > 24 mantissa bits). The faithful x87 fsub/fadd/fmul of
       two m32 floats feeding a 53-bit consumer (atan2/sqrt/cos
       arguments, comparison operands, chains) must emit
       cvtps2pd/cvtps2pd + subsd/addsd/mulsd. Every function's /Fa
       gate check now includes this pattern scan: any ss op whose
       result is cvtps2pd'd afterward and consumed at double is a
       DIVERGENCE - write the operands with explicit (double) casts.
       Owed retroactive sweep: every owned function consuming a
       float-float arithmetic result at 53-bit (incrementWaypoint,
       calculateYawAngle, missile_move's norm chains, the doIntelligent
       MovementUpdate decode; angle_to_object fixed 2026-11-18).
   (k) RECIPROCAL-MULTIPLY /Fa SCAN + THE float_control(precise)
       FUNCTION BRACKET (owner-directed 2026-11-18; the /Fa check that
       caught canLinePath's step divisions): under /fp:fast MSVC
       transforms `(double)a / (double)t` into (1.0/t)*a - the
       reciprocal pre-rounded (rounding 1), the product re-rounded
       (rounding 2) - diverging from the original's single-rounding
       fdiv + fstp m32 whenever the reciprocal is inexact. EVERY owned
       function containing a division on precision-relevant operands is
       /Fa-scanned for the reciprocal form (a 1.0-constant + divsd +
       mulsd chain instead of a plain divsd); when the transform
       appears AND the divided value feeds a boundary-sensitive
       consumer (tile truncations, compares, the rule-(e) store-
       rounds), the function is bracketed at file scope:
       #pragma float_control(precise, on, push) before it and
       #pragma float_control(pop) after - pinning the divisions as
       divisions (divsd). The project's first use: canLinePath_new,
       2026-11-18 (the /Fa re-gate showed divsd x2 and
       __libm_sse2_sqrt_precise replacing the inline sqrtsd -
       bit-identical results: IEEE sqrt is correctly-rounded in both,
       the CRT wrapper only adds a negative-input/errno branch
       unreachable for sums-of-squares inputs; first-use performance:
       two direct divsd + one precise-sqrt call per invocation -
       immeasurable at pathing call rates). The /Fa gate check for
       every bracketed function on every re-gate: verify divsd (no
       reciprocal chain). The owed retroactive division sweep:
       incrementWaypoint, findAvoidancePath (pre-write), the
       doIntelligentMovementUpdate decode.
- Allocation size verification (hard stop): every operator_new /
  operator_new_internal literal size must be mechanically verified as
  sizeof(allocated type) вЂ” recursive struct walk over idb_types.h with
  pack-capped x86 rules (member align = min(natural, pack); struct
  align = max member align, capped; size rounded to struct align).
  A mismatch, or an unverifiable allocation (unresolved embedded types),
  is a HARD STOP: Information Request, no code вЂ” keeping the unverified
  literal is not permitted. Runtime-size allocations (arrays sized by
  parameters) are exempt from sizeof but must match the original's
  allocation expression. THE WRITTEN FORM is sizeof(allocated type)
  - a verified literal with a walked-comment justification is still a
  violation (owner ruling 2026-11-18, the addCurrentPath/
  findAvoidancePath allocations); sizeof lets the compiler maintain
  the verification across IDB struct changes.
- Structured-I/O size verification (hard stop): every rge_read /
  rge_write / fread / fwrite-style size argument (the 3rd parameter)
  must be written as sizeof(target) вЂ” sizeof of the exact destination
  expression, never a numeric literal вЂ” so the transferred width is
  mechanically type-checked against the target (single-element arrays
  use the element expression, e.g. arr[0], not the whole-array sizeof).
  Format-driven readers (fscanf) carry widths in the conversion string
  instead вЂ” audit every conversion against its target. Value-sized
  transfers (length-prefixed strings) pass the length variable, not a
  size. A width that intentionally differs from the target's sizeof
  (partial-field or deliberate-overwrite transfers) is a DEVIATION and
  follows the deviation protocol (see its bullet below): the literal
  stays, an asm-cited comment goes at the call, and the deviation must
  be presented as a clearly-flagged item in the review artifact вЂ”
  target expression, literal width, the sizeof it replaces, and the
  asm evidence вЂ” with explicit owner approval obtained BEFORE the code
  is written. Writing a deviation into a file without that approval is
  a gate violation. An unverified width вЂ” a bare literal without the
  asm citation and approval, or a sizeof that disagrees with the
  asm-pushed size вЂ” is a HARD STOP: Information Request, no code.
- Cross-class base calls (explicit casts required): the flattened
  derived structs are unrelated C++ types вЂ” no declared inheritance вЂ”
  so a call through a base-class idb constant rejects the implicit
  derived-to-base pointer conversion (MSVC C2664) вЂ” for EVERY
  derived-typed argument, not just this_: forwarded pointers such as
  other_object need the same treatment (caught at Action setup, line
  23: the second argument other_object is Action-typed where the
  constant declares Moving-typed). Every cross-class
  base call must carry an explicit C-style cast of each derived-typed
  argument to the constant's parameter pointer type; static_cast would
  reject unrelated types, so the C-style form is required. The rule
  binds EVERY cross-type pointer site, not only call arguments:
  (a) RESULT ASSIGNMENTS and initializations of a base-typed variable
  from a derived-typed value are the same conversion and need the same
  explicit cast (MSVC C2440 вЂ” caught at the Animated/Moving factory
  trios' `result = new_master` / runtime-ctor returns, 2026-11-14);
  (b) vtable-slot calls cast the explicit `this` argument to the slot's
  declared first-parameter type, and any argument wider than the base
  type (e.g. a Master_Static_Object* parameter fed from a derived
  this_) exactly like idb-constant calls.
  Compatibility:
  cast only to types whose accessed fields lie within the real
  object's layout prefix вЂ” each flattened struct mirrors its base
  prefix exactly (walker-calibrated), so a derived-to-ancestor cast is
  layout-safe for the ancestor's fields. Deeper casts (base ->
  derived, or sideways) require the access-offset proof per the
  base-pointer rule. Floating literals beyond the type range (e.g.
  3.4028235e38 > FLT_MAX) are rejected by MSVC (C2177) вЂ” use FLT_MAX
  or an exactly-representable literal.
- VIRTUAL-DISPATCH FIDELITY (owner-directed 2026-11-19): when the asm
   dispatches a call through a vtable slot (the vfptr load + `call
   dword ptr [reg+disp]` shape), the replacement MUST use the NAMED
   VIRTUAL DISPATCH `obj->vfptr->member(args)` through the matching
   Vtbl struct member - NEVER a direct call to the idb-constant
   function or to the `_new` replacement at that site. The
   hook-routing ruling's "calling through an idb constant is
   behaviorally identical" applies ONLY where the original asm itself
   calls the constant directly (the base-class-call case in derived
   replacements); a virtual site written as a direct call hard-codes
   one implementation where the original resolved dynamically (a
   derived override - present or future - would be bypassed) and the
   /Fa record desyncs from the original's dispatch shape. The member
   must be the struct member whose line-arithmetic index (member line
   - first-member line) equals the slot index; the /Fa gate check
   verifies the emitted offset ([reg+slot_offset]) per function.
   Converting a virtual site to a direct call is a gate violation.
   (Origin case: the passableTile2 probe sites in canLinePath /
   canLinePath2 / firstTileAlongLine - the direct idb-constant calls
   converted to the named dispatch, 2026-11-19.)
- CAST-FORM VTABLE DISPATCH FORBIDDEN (owner-directed 2026-11-19):
   NEVER call a vtbl member through a function-pointer cast of the
   member pointer - the IDA .c rendering's
   `((ret (__thiscall *)(T *, args...))obj->vfptr->member)(...)` form -
   even when it compiles and the /Fa shows the right slot offset. If
   the member's declaration does not match the call shape the asm
   shows (argument count/types), the member naming is WRONG: that is a
   type-definition suspicion - HARD STOP, Information Request, the
   owner corrects the IDB and regenerates, and the decode/conversion
   restarts from the regenerated files. (Origin case: the displaced
   passableTile/passableTile2 vtbl member naming in all ten runtime
   Vtbl structs - the cast-form interim was probed, emitted the right
   offset, and REJECTED; the owner's IDB correction restored the clean
   named form.)
- CRT calls via *_internal wrappers (hard stop for bare CRT): the
  common.h/cpp wrapper family (malloc/calloc/operator_new/operator_
  delete/free/read/write/fopen/fscanf/fread/fwrite/fgets_internal...)
  routes CRT through the GAME's runtime вЂ” required because game code
  frees what replacements allocate (heap pairing; e.g. the Combat
  destructor frees the Armor/Weapon tables with the game's _free) and
  because FILE* handles carry the game's stdio state. Bare calloc/
  fscanf/malloc/free calls in replacements are violations. memcpy is
  the documented exception: no _internal wrapper exists and a copy is
  heap-neutral (safe on game-heap memory). Post-write sweep: bare
  calloc(/fscanf(/malloc(/free( must be 0.
- base_game double-pointer deref: the base_game global is declared
  RGE_Base_Game** const (common.cpp:7); IDB types it TRIBE_Game*. The
  canonical access form is (*base_game)->field вЂ” a bare
  base_game->field is a double-pointer bug (reads the pointer-slot
  region of the Game struct as if it were the Game pointer). Verified
  ground: (*base_game)->world @0x420, world->map @0x34, world->tasks
  @0x18C. Post-write sweep: grep for base_game-> not preceded by *)
  (lookbehind `(?<!\*\))base_game->`) вЂ” any hit is a violation.
- Numeric conversion verification: every hex-to-decimal (and reverse)
  conversion cited in output or ledger must be recomputed mechanically
  before use вЂ” an arithmetic slip here can manufacture phantom blockers
  or hide real ones. Prefer scripted conversion over mental arithmetic
  for any constant that gates a decision.
- Pointer arithmetic elimination (hard stop): `pointer += constant`,
  `pointer -= constant`, and `pointer + constant` expressions in the
  decompilation must be converted to indexed array access, with every
  byte offset verified against the element size and the containing
  struct layout. `pointer += constant` statements must never appear in
  output code. This binds the replacement code too: output must not
  introduce walked pointers вЂ” no `++p` / `p += n` / saved-and-
  incremented pointer variables; all array iteration is by index over
  the verified element type (`arr[index]`). An offset that cannot be
  verified against the layout is a hard stop: Information Request, no
  code. Post-write verification includes a pointer-walk pattern check
  (increments and constant-arithmetic on pointer variables) alongside
  the defs/refs count.
- Deviation presentation protocol: whenever a reconstruction must
  deviate from a mechanical form this template mandates (sizeof widths,
  indexed access, faithful operator order, any literal the rules would
  otherwise rewrite), the deviation is not an implementation detail вЂ”
  it is a review item. Present each deviation explicitly in the review
  artifact: what the mechanical form would be, what the binary actually
  does, the asm evidence, and why the deviation is required. Explicit
  owner approval must be obtained before the deviating code is written.
  Silent deviations вЂ” however well-commented in the code вЂ” are gate
  violations.
   Owner clarification 2026-11-15 (dopple ctor_read 4-byte span round):
   decode-record flags and batch-level write orders do NOT constitute
   that approval - every deviation needs explicit per-item owner sign-off
   in the conversation, even when pre-flagged in a decode record; future
   discrepancies are presented and approved individually, never by
   inheritance.
- Base-pointer derived-field correction (the v6[1].field pattern): IDA
  pseudocode that indexes a base-typed pointer (v6[N].field) is its
  rendering of an access to a field of a DERIVED class вЂ” IDA lacks the
  class hierarchy and falls back to stepping the pointer by the base
  size. Never transcribe the expression literally (it computes
  v6 + N*sizeof(base) + offsetof(field), an address in the derived
  extension region the base type cannot name). Instead: compute the
  target byte offset (N * sizeof(base) + offsetof(field within the
  base)), locate the matching field in the derived struct via the
  mechanical walker over idb_types.h, and rewrite the access as that
  named field. Present the mapping (original pseudocode expression ->
  byte offset -> derived field name) in the review artifact. If the
  offset cannot be matched to a named field of a known derived struct
  at the right width вЂ” i.e., no sane expression can be deduced вЂ” it is
  a HARD STOP: Information Request, no code. Do not guess names the
  walker does not confirm, and do not accept a field on semantic
  plausibility alone. When the correction chain is non-trivial
  (multiple interacting mis-typed expressions), prefer the re-typed
  pseudocode request (see its rule) over manual rewriting.
- Re-typed pseudocode requests: IDA recalibrates pseudocode well when
  variable types are set manually. When a case is tricky вЂ” ambiguous
  base-pointer corrections, interacting mis-typed variables, artifact
  chains (param slots reused as unrelated locals, pointers typed as
  integers driving arithmetic, byte fields indexed with word reads) вЂ”
  do not fight the rendering: issue an Information Request asking the
  owner for an updated pseudocode dump, stating the suspicions
  concretely: which variables are incorrectly typed, their current IDA
  types, and the deduced real types, each with its evidence. Prefer
  this over manual reinterpretation whenever the manual chain runs
  longer than a couple of steps or its result gates a semantic
  decision. A manual reinterpretation may still be used when it is
  short, mechanical, and walker-confirmed вЂ” it must be presented in
  the review artifact either way.
- DIAGNOSTIC LOGGER PROCESS (owner-directed 2026-11-18, after the
   calculateYawAngle A/B harness): on owner request, ANY owned function
   gets the A/B harness - three pieces, appended to the class file:
   (1) THE ORIG TRAMPOLINE: `__declspec(naked) <rettype> __fastcall
   <Class>__<method>_orig(...same params...)` - read the original's
   prologue from the .lst, REPLAY in inline asm enough instructions to
   pass the 5-byte setHook E9 (adjust every [esp+X] operand for the
   trampoline's own frame: the trampoline is entered via a CALL, so its
   esp = the original's entry esp - 4 - the trampoline's own pushes;
   the replayed instructions' stack reads must resolve to the SAME
   slots the original's did), then `mov eax, <original entry + replayed
   length>; jmp eax` (enter PAST the E9 - calling the hooked entry
   would recurse). Verified exemplar: RGE_Moving_Object__calculateYaw
   Angle_orig (replays sub esp,0Ch / push ebx / mov ebx,[esp+18h]=newOF,
   enters 0x4A6358). (2) THE LOGGER WRAPPER:
   `<rettype> __fastcall <Class>__<method>_logger(...)` - sprintf the
   inputs, call _orig, sprintf the outputs + the updated out-params,
   append to val_log.txt ("at" mode, lazy fopen, one handle per
   session). (3) THE INSTALLER SWAP: setHook(<addr>, _logger) - the
   _new stays compiled but unhooked; flipping the line back restores
   the fix. THE CRT NOTE: the logger's bare sprintf/fopen/fputs are
   the APPROVED DIAGNOSTIC EXCEPTION to the CRT-wrapper rule
   (owner-supervised, removed before final ship). VERIFICATION: the
   trampoline's replayed instructions must cover EVERY byte the E9
   overwrote; the register/stack lifecycle must balance (the exemplar's
   retn 10h pops the logger's arg block and returns to the logger).
   A/B OUTPUT: run both the hooked build and the unhooked build, diff
   val_log.txt - the first diverging value names the exact stage.
   THE ESCALATION LADDER + THE MULTI-TRACE PAIRED CAPTURE (owner-directed
   2026-11-20, from the updateMovement hunt): when the val_log diff
   localizes a stage but the stage has multiple internal decisions,
   escalate INWARD - hook the functions the stage calls (the pathfinder
   loggers: the args, the return, the resulting rPath content) and the
   world-object lifecycle (createPath/destroyPath2/path-resolve with the
   resulting BPath characteristics). ALL TRACE PAIRS (val + arm +
   lifecycle + pathfinder) diff line-by-line; the first divergent line
   names the call. A matched OUTCOME does not prove a matched internal
   call history (stateful callees - the tile/low-level pathfinders -
   evolve per call), so the stateful callees' own logs are the evidence.
   THE TRAMPOLINE BYTE-BOUNDARY LESSON (0x4BCDA8-vs-0x4BCDA7): the jump
   target = the entry + THE REPLAYED BYTE LENGTH derived from the .lst
   INSTRUCTION BOUNDARIES - an eyeballed instruction count is not a byte
   count (mov eax,[esp+10h] is 4 bytes, not 5); a one-byte slip
   mid-prologue resumes execution INSIDE a broken instruction and
   corrupts the call. Verify the emitted replay + the jump address in the
   /Fa. THE FASTCALL LIVENESS RULE: the jump-address register must be
   DEAD at the jump target - if the convention puts a live arg in edx
   (findLowLevelPath's start), use the esp-neutral push/ret pair instead
   of mov edx/jmp. THE SHIP REMOVAL: at ship, delete the harness pieces
   and hook the replacement directly (the updateMovement precedent);
   re-run the suite once on the ship build - only the observer I/O
   differs, but the project standard is one confirming pass.
- SEMANTIC COMMENT HEADER (owner-directed 2026-11-17, style per
   check_placement_new / alignment_new / get_string_id_new): every
   replacement's decode comment OPENS with a short semantic summary -
   what the function DOES in gameplay terms (its role, then the
   narrative: what is read, what is decided, what side effects land) -
   before the address-by-address decode mechanics. The semantic
   paragraph justifies identifier choices and comments; the asm
   citations remain the grounding. COMMENT PLACEMENT (owner-directed
   2026-11-18): SEMANTIC comments go ABOVE the code they explain, in
   natural statement groups separated by blank lines; the asm CITATIONS
   trail on the statement lines (the "// asm 0x..." form) - never the
   reverse, and never a semantic novel appended after a statement.
   Retrofits: batches written before
   this rule carry the semantic headers too (comment-only edits,
   re-gated).
- Do not "improve" the algorithm. Faithful reconstruction, not refactoring.
- Residual judgment calls that are legal under the gate (naming choices,
  comment placement, whether an equivalence is trivial) are marked
  `// ??? <reason>` and listed in the output вЂ” but these must never touch
  semantics.
- Post-write verification: after every batch write, re-scan the written
  file вЂ” every installer reference must resolve to a definition in the
  same file, and vice versa; report the counts. Also re-run the
  pointer-walk pattern check (type-checked: pointer-typed increments are
  violations, int loop counters are fine), the redundant-guard scan (see
  their rules), and the return-type inventory sweep вЂ” every
  replacement's declared return compared against its idb prototype's
  return (setups = int, ctors = the owning class pointer, destructors =
  void, make_new_obj = RGE_Static_Object*, make_new_master =
  RGE_Master_Static_Object*) вЂ” and report the results. Also re-run the
  cross-class cast sweep (every base-class call carries an explicit
  cast) вЂ” re-run it after EVERY batch insert, including destructor
  batches (the Animated destructor insertion postdated the original
  cast sweep and shipped one uncased base call, caught by the owner).
  For constructor replacements: the structural audit вЂ” each ctor
  carries its class vfptr store, the do_setup guard, and the do_setup
  body sequenced after the guard (script: per-ctor body extraction +
  membership checks; a first-version probe that searched 'setup'
  matched the guard's own 'do_setup' substring вЂ” probe after the
  guard text). A WRITTEN ledger
  status may only be recorded after this check passes.
- Compile gate (hard requirement): after every batch write, each
   touched translation unit must pass a syntax-only compile (vcvars +
   `cl /c /std:c++17`, 0 errors) before the WRITTEN status is recorded
   THE CONCRETE INVOCATION (owner-directed 2026-11-18 - the practiced
   form, reproduce exactly): cmd /c '"<VS>\VC\Auxiliary\Build\
   vcvarsall.bat" x86 -vcvars_ver=14.16 && cl /c /nologo /std:c++17
   /O2 /arch:SSE2 /fp:fast /EHsc /DWIN32 /D_CRT_SECURE_NO_WARNINGS
   /DEFPATCH_CURRENT_CONFIG=Release /DNDEBUG /D_WINDOWS /D_USRDLL
   /DCHEATDLL_EXPORTS /I"<repo>\DoctorDumpSDK\include" /I"<Cpp
   libs>\zlib" /I"<Cpp libs>\CImg" /I"<Cpp
   libs>\zlib\contrib\minizip" /I"<repo>\cheatdll" /Fa<temp>\<TU>.asm
   /Fo<temp>\<TU>.obj <TU>.cpp' (<VS> = the vswhere-located install;
   -vcvars_ver=14.16 = the project's v141_xp toolset; non-/GL per the
   known LTG gap; the "Cpp libs" env var = the include root). The /Fa
   listing is NOT optional: it feeds the rule-(j) widen-before-op
   check, the X87 macro /Fa gate check, the dd-constant pool check
   (rule C), the vtbl slot-offset verification, and the reciprocal-
   form scan (rule (k)).
   вЂ” the defs/refs sweep does not catch type errors (the Animated/
   Moving factory trios shipped 7 latent C2664/C2440 sites on
   2026-11-14, caught by the gate).
- BRACE-DEPTH STAGE WALK (mandatory for restructured bodies, owner-
   directed 2026-11-20): after any multi-stage rewrite, mechanically walk
   the brace depth from the function open through the end and assert the
   depth at EVERY stage boundary comment (the dispatch loop body, each
   sequential stage, each nested arm). "0 errors" proves the braces
   BALANCE, not that they close the right blocks - a missing stage close
   silently nests the following stage inside the previous one's gate (the
   walk arm compiled inside the state-2 stage: state-3 units skipped the
   walk, the loop spun to the tries guard, ret=0 - caught only by the
   replay).
- POINTER-TO-SLOT GLOBAL DEREF SWEEP (mandatory, owner-directed
   2026-11-20): globals declared as pointer-to-slot (int*
   numberObSystemCollisions etc.) must be DEREFERENCED in every guard and
   condition - a bare pointer compare (`ptr <= 0` / `!ptr`) never fires
   and silently disables the branch (the OB-pass guard ran the collision
   loop once over the stale obSystemCollisions[0] on every zero-count
   query - the phantom fresh-blocker successor swap). Sweep: grep the
   global bare inside a condition; every hit is a stop-and-fix.
- /Fa OFFSETS ARE DECIMAL in MSVC listings ([ecx+1056] = +0x420,
   [eax+780] = +0x30C; small offsets render without any suffix): the
   vtbl-slot and struct-offset verifications must convert before
   comparing.
- THE MASK-FOLD DOCUMENTATION (the C1/C2 class, owner-directed
   2026-11-20): where the asm narrows a raw return (and eax, 0FFh at
   0x49D3BF / 0x49D8CA) and the idb prototype's narrow return (u8/bool)
   already reads AL, the source-level mask compiles as a FOLDED NO-OP -
   cite the asm address, state the equivalence (the AL read IS the mask's
   result: and eax,0FFh keeps exactly bits 0-7), and do not force
   machinery to make the mask literally appear in the listing.
- Store-and-read diff (mandatory for construction/teardown and store-heavy
   replacements): after drafting and again post-write, mechanically
   enumerate every memory store in the asm range вЂ” `mov [reg+disp]`
   forms with object-pointer bases (esi/edi in these frames),
   excluding [esp] locals вЂ” and extract each target offset and width.
   Diff against the written function's field-write set (field name ->
   offset via the struct; dword fields cover 4 bytes, byte fields 1).
   Every asm store must have a written counterpart covering the same
   bytes, and every written store must trace to an asm store or a
   cited bake-in (patch dword-stores count at their dword width).
   EVERY READ OFFSET ON OBJECT-POINTER BASES (fld/cmp/mov reads like
   `[eax+44h]`) must be mapped to its census-verified field name and
   the mapping cited вЂ” a read whose offset-vs-name mapping cannot be
   cited from the census is a stop-and-fix before the write. (Shipped
   lesson: the runtime destructor's `[master+44h]` read was labeled
   `hp` instead of `radius_z` вЂ” a hex/decimal census slip that four
   reviews confirmed via a plausible narrative and that caused a
   shipped desync.) Report the diff table. Rationale: the runtime
   Static ctor batch shipped with a dropped `mov [esi+8Ch], al`
   (mROOAlreadyCalled) despite a full-asm pass вЂ” the writeData patch
   store interleaving with original stores anchored attention on the
   patch region, and the following original store fell out of the
   transcription. Manual reading is insufficient; this diff is
   scripted.
- Exit-path inventory (mandatory from 2026-11-16, owner-directed after
   the missile_move desync): after drafting and again post-write,
   enumerate the asm range's return sites (address + returned
   value/register) and diff against the written function's return
   statements — every asm return site must be accounted for by a
   written return, and every written return must trace to at least
   one asm return site (mapping, not counting: a single C return
   legitimately compiles to multiple asm sites — missile_move's one
   written return maps to the 0x49C015/0x49C01E value-split pair). Covers the control-flow class the
   store-and-read diff cannot see: an invented early-return on an
   unexercised branch (missile_move shipped `return 0` on the OOB
   path where the original fell through to the shared teleport tail -
   asm loc_49BFE7 joins loc_49BFF1, exactly two return sites, both
   after the teleport). Script it alongside the store diff.
- Join-convergence inventory (mandatory from 2026-11-18, owner-directed
   after the angle_to_object desync): before drafting, mechanically
   enumerate inbound edges per label from the asm range's own
   instructions: every jcc/jmp instruction whose operand is a label
   contributes one inbound edge to that label, and each label
   additionally has one fall-through inbound edge if its immediately
   preceding line is not an unconditional control transfer (jmp / ret /
   retn / jmp reg). Any label with >= 2 inbound edges is a JOIN. The
   .lst's CODE XREF annotations are hints only - NEVER the count (they
   do not list all xrefs; owner ruling 2026-11-18). Every instruction
   in a joined block is attributed to EVERY reaching path, and a value
   produced before a join and consumed after it appears in the written
   code once, positioned so all paths pass through it (duplicated only
   with a path-exact proof). The join table (label -> inbound sites ->
   the shared instructions -> the written statement covering them) goes
   in the review artifact. The failure this closes: angle_to_object's
   to-object path jmp'd over the fallback path into the shared
   fsub [ecx+4Ch] tail @0x4A1146 - the fld was transcribed, the
   converged fsub was not, and atan2 received the absolute y.
- Total-instruction citation coverage (mandatory from 2026-11-18, the
   same desync): upgrades the store-and-read diff's read-mapping to a
   scripted, bidirectional ADDRESS-COVERAGE check: (1) enumerate every
   memory-operand instruction address in the asm range (mov/cmp/fld/
   fadd/fsub/fcom/... [reg+disp] and the [esp+...] parameter reads);
   (2) every such address must be CITED in >= 1 written-code comment;
   (3) every written-code citation must resolve to an address in the
   range. An address in (1) absent from (2) is a stop-and-fix before
   the write; a citation that resolves to nothing is removed. Scripted
   alongside the store-and-read diff and the exit-path inventory.

## Deliverable (file layout and installers)
1. Cleaned function(s) go to `cheatdll/src/engine/<subsystem>/<class_name>.cpp`.
   One file per engine class; it contains every replaced function of that
   class and nothing else. The subsystem is a high-level grouping invented
   for convenience вЂ” it does not exist in the binary and carries no
   behavioral meaning. Selection order: (i) reuse an existing directory
   under `src/engine/` whose scope already covers the class вЂ” consistency
   beats novelty; (ii) if none fits, propose a new one derived from the
   class family's domain word (e.g. RGE_Action_* в†’ `actions`,
   *World в†’ `world`, panels/screens/dialogs в†’ `ui` вЂ” examples of the
   derivation, not an authoritative mapping), state the one-line
   reasoning, and the owner approves it together with the first function
   placed there (constraint 9). One class lives in exactly one subsystem,
   so the question only arises once per class.
2. Every replaced function is named `<Class>__<method>_new`
   (`RGE_Master_Static_Object__get_sprite_new`). This is MANDATORY, not
   cosmetic: the canonical `<Class>__<method>` names are `const` function-
   pointer variables in `idb_functions.h`, included in every translation
   unit via stdafx.h, and C++ has a single identifier space вЂ” even a
   `static` function cannot reuse them. The suffix is always required
   because protocol step 1 guarantees the canonical name exists (it is
   where the address came from). Precedent: land.cpp:4, mousewheel.cpp:14,
   aiactions.cpp:20. The no-suffix cases in the repo (mpsetupscreen.cpp,
   overlay.cpp) are project-invented classes that never existed in the
   IDB, not replacements.
   Safety clause: `X_new` itself can be an original engine name
   (e.g. `TRIBE_Screen_Sed__command_new`, `decoder_input_new` вЂ”
   idb_functions.h:3561, :6129). The final chosen identifier must have
   zero matches across idb_functions.h / idb_types.h / common.h вЂ” one
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
If no `proc near`/`endp` pair is found в†’ the range cannot be proven в†’
escalate as a blocking item; do not install.

Patch writers вЂ” two layers, one high-level logic: patch bytes at an
absolute VA. Which mechanism runs (WriteProcessMemory vs the Voobly
interface) is a build-config implementation detail; all configurations
target the same game binary, so addresses are config-independent.

Layer 1 вЂ” canonical API (grounded: common.h:51-60, common.cpp:23-130):
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

Layer 2 вЂ” direct writers bypassing the API. Census below documents the
current state; the greps in the protocol are the authority.
| writer / site                                          | extent | scope                       |
|--------------------------------------------------------|--------|-----------------------------|
| reloc(src, dst) вЂ” conditions.cpp:871 (rel32 disp)      | 4      | call/jmp retarget at src    |
|   conditions.cpp:995,1000                               |        | .text @0x005F207E           |
|   conditions.cpp:974-986                                |        | trampoline copies only      |
| g_pVoobly->WriteJump вЂ” iuserpatch.cpp:306,307,339,340, | 5      | .text, 8 literal hooks      |
|   342,344,345,352                                       |        |                             |
| g_pVoobly->Write(DWORD val) вЂ” wndproc.cpp:42            | 4      | .text @0x00426509           |
| g_pVoobly->Write(hex string) вЂ” iuserpatch.cpp:303       | 6      | data @0x689BA4              |
| WriteProcessMemory вЂ” chatcommand.cpp:753,761            | 1      | data @0x00651BC0            |
| dsoundhook.cpp (14 sites)                               | var    | dsound.dll / COM vtables    |
| test.cpp:399,419,440                                    | var    | user32.dll (DrawTextA)      |
| effects.cpp:623                                          | вЂ”      | commented out (inactive)    |
| functionlist.cpp:71,96 (WriteProcessMemory)              | вЂ”      | commented out (inactive)    |
| FUNCTION_HOOK ctor/before_call/after_call вЂ”              | 6      | .text entry (push/retn      |
|   functionlist.cpp:21-43,72,97 вЂ” bare memcpy of a        |        | trampoline) at any          |
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
   If no `proc near`/`endp` pair is found в†’ the range cannot be proven в†’
   escalate as a blocking item; do not install.
2. Grep all `cheatdll/**/*.cpp` for every writer in both layers: the nine
   Layer-1 names, `reloc(`, `WriteProcessMemory`, `g_pVoobly->Write`,
   `g_pVoobly->WriteJump`, and the FUNCTION_HOOK mechanism (~2,550 active
   call sites as of 2026-11-14). Extract the target address (apply the
   +0x400000 shift for F-variants) and keep only overlaps with
   [start, end).
3. Whole-repo address grep вЂ” the decisive complement: search ALL `*.cpp`
   and `*.h` in the repository for the function's address bytes (e.g.
   `48B8|48B9` for get_sprite). This catches table-driven patches,
   macro-hidden targets, and mechanisms the per-site writer greps miss.
   Every hit outside the IDB exports must be individually dispositioned.
   3b. Writer-target enumeration against FULL ranges (mandatory, scripted): extract the target address from EVERY writer call site in the repository - all Layer-1/2 names AND this project's __declspec(naked) mid-function hook stubs (the patch style hooks function INTERIORS, e.g. bldg_heal_rate_save.cpp) - and test each target against EVERY owned [proc, endp) range. Entry-address literal greps DO NOT catch interior targets: the 2026-11-15 Building batch shipped two such misses (0x005C750D inside small_save, 0x005C72AF inside setup4 - the bldg_heal_rate save-format hooks) that silently reverted a shipped stream-format extension and crashed master reading. A colliding patch's behavior must be BAKED into the replacement (owner-approved) and the patch call site retired - a replacement silently reverts the patch and desyncs whatever state the patch changed. Bake-in is the DEFAULT disposition for every colliding patch; the only waiver is owner-directed, and the affected replacement lines then carry a // <PATCH_ID> marker comment (e.g. // MAP_SIZE in RGE_Visible_Map__get_visible_new) so the revert stays visible at the code site. The scan is scripted: extract targets from all writer call sites, range-test against every owned [proc, endp), report. MAP_SIZE EXCEPTION (owner-directed 2026-11-15): mapsize.cpp patches are NEVER baked regardless of the default rule - the replacement keeps the original's table/address access (e.g. unified_map_offsets), the affected replacement lines carry the // MAP_SIZE marker documenting the intentional divergence when the big-map mod re-points the patched locations, and the colliding mapsize.cpp call sites are retired in place (commented, dead-after-hook) with the not-baked reason.
  BAKE MANDATORY PER FAMILY (owner directive 2026-11-15): every
   PER-BATCH SCAN RE-RUN (owner-directed 2026-11-17, after the missed
   on_evac_area collision): the scripted writer-target scan re-runs for
   EVERY batch, over ALL owned ranges PLUS every pending batch range -
   a clean verdict from a previous batch is NEVER inherited, and any
   "covered by an earlier scan" claim is void. The scan is the
   MECHANICAL FORM ONLY: extract every literal .text target from every
   writer call site repo-wide (Layer-1 names, reloc, Voobly
   Write*/WriteJump, WriteProcessMemory, and every __declspec(naked)
   stub's setHook/Write*), then range-test each target against every
   [proc,endp) range. Hand-built per-range address patterns are
   PROHIBITED - they are how the M6b miss happened (the hand pattern
   covered 4A6C/4A6F but not the 4A6D/4A6E interior, missing
   playeroptions.cpp:206 setHook(0x004A6E95, on_evac_area) inside
   find_evacuate_area). Every presentation carries its own
   "Collision audit" section with the scan output quoted; a
   presentation without it is incomplete and must not be acted on.
   BAKE MANDATORY PER FAMILY (owner directive 2026-11-15): every
  STORE-AND-READ DIFF LIMITATION (owner-reported desync 2026-11-15,
  Animated setup bootstrap): the offset-level diff cannot catch an
  INVENTED store that targets an offset the asm also legitimately
  stores - the offsets match while the VALUE/SOURCE diverges. For
  bootstrap/registration blocks (create-X + select-Y + register-Y
  sequences), diff the written block against the asm at INSTRUCTION
  level before writing: determine per call result whether it is
  STORED to a field or PASSED to the next call. Pattern-matching
  familiar idioms across similar-looking blocks without the asm pass
  is prohibited - the Animated setup/ctor bootstrap invented
  `sprite = get_sprite(...)` where the asm passes the result directly
  to new_sprite (the field stays 0).
  colliding patch EXCEPT mapsize.cpp patches MUST be baked into the
  replacement - there is no waiver for non-mapsize families. Entry-form
  mid-function hooks fold the handler semantics (the naked stub is read,
  its replayed instructions and resume targets are transcribed into the
  written gate/flow) and the setHook call site retires; constant/NOP
  patches fold the constant or the altered statement. mapsize.cpp
  patches remain never-baked per the exception above.
4. Literal-address overlaps are verdicts. Dynamic/computed targets
   (variables, trampoline bases, GetProcAddress results) cannot be proven
   disjoint в†’ list them for manual review instead of clearing them.
5. Report per collision: file:line, writer, byte extent, what the patch
   does (from surrounding code), and a bake-in suggestion per the rules
   below.
6. State the limitation explicitly in the output: this scan covers this
   repository only; external patchers (userpatch engine internals, Voobly
   server-side, third-party DLLs) are invisible to it.

Bake-in rules (constraint 7: report, never apply without owner
confirmation / OVERRIDE):
A bake-in is suggested only if the patched bytes can be mapped through
grounded links: patch address в†’ instruction line(s) in
`battlegrounds_x2.lst` в†’ the pseudocode construct that emitted them. If
no such mapping exists (compiler-internal code, alignment padding), do
not invent one: suggest keeping the old patch active instead вЂ” after
replacement it writes into dead code вЂ” and state why no bake-in exists.
* Constant/immediate patch (writeByte/Word/Dword/Data on operand bytes):
  name the constant to change in the cleaned source and the statement it
  belongs to. Mark the changed line `// baked: <patch file:line> @ <addr>`.
* NOP patch over instructions: name the statement(s) to remove or alter,
  same citation standard.
* Call/jmp redirect (setHookCall, reloc): replace the callee with the
  redirect target in the cleaned source. Read the target's source first вЂ”
  it is project C++ in cheatdll, no IDA work needed вЂ” and adapt arguments;
  cite the handler file:line.
* Entry hook (setHook / WriteJump at [start, start+5)): genuine conflict,
  never bakeable in place. Present exactly two options вЂ” never "keep
  both", the last writer wins and the hook chain breaks:
  (i) fold the old hook handler's behavior into the cleaned function
      (requires reading the naked thunk + handler source; most faithful),
  (ii) remove the old patch call site so the replacement owns the entry.
  Both require owner confirmation.
Every applied bake-in is listed in the output as a table row: patch site в†’
bytes в†’ lst line в†’ mapped statement в†’ transformed statement в†’ status.

Retirement of folded patches: when a patch is baked into a replacement,
the original patch call site is COMMENTED OUT in place вЂ” never deleted вЂ”
with a reason line: `// retired <date>: baked into <replacement function>
(<source path>), see <original address>`. Already-dead comment lines stay
as they are. Thunk definitions whose references are all retired remain
defined (unreferenced is harmless); only their call sites are retired.
Patches that do not overlap replaced functions stay untouched and active.

## Required output when the gate passes
All of the following is delivered in the conversation for review вЂ” nothing
is written to the repository without approval (constraint 9):
1. The cleaned function(s), ready to paste into the project, with the
   installer per the Deliverable section.
2. "Collision audit": range checked, number of call sites scanned, every
   collision with its bake-in suggestion (or "no collisions" verdict, plus
   non-literal targets left for manual review).
3. "Judgment calls": residual cosmetic decisions, ranked by risk. If this
   list contains anything that affects semantics, the gate failed вЂ” the
   function must not be here.
4. "Suspicion": any provided type/prototype that looks inconsistent with
   the code вЂ” flagged, not changed.
5. "Follow-ups": callees revealed as important to decompile next, or types
   that appear incomplete.

## Input
### Prototype (fixed, do not change)
{{PROTOTYPE вЂ” e.g. `void __thiscall CPlayer::Update(float dt)` or
`int __usercall foo@<eax>(int a@<ecx>, float b@<st0>)`. The original
address is NOT required in the input: the agent resolves it from
idb_functions.h by exact `Class__method` name match, which simultaneously
corroborates the signature and calling convention.}}

### Relevant type definitions (verbatim from project headers)
{{TYPES вЂ” anything NOT pasted here AND NOT found verbatim in the reference
files below is treated as unknown and will trigger a refusal, not an
assumption.}}

### Known names referenced by this function
{{NAMES вЂ” usually unnecessary: functions resolve from idb_functions.h,
structs from idb_types.h, globals from common.h/common.cpp. Paste only
symbols that exist in none of the reference files.}}

### Project reference files (agent-resolvable)
The repository contains six authoritative files the agent MUST consult
before declaring anything unknown. Verbatim content found in them has the
same grounding status as pasted input вЂ” category (a) of the gate:

* `cheatdll/src/idb/idb_types.h` (37k lines) вЂ” all struct definitions and
  typedefs
* `cheatdll/src/idb/idb_functions.h` (6.4k lines) вЂ” all known function
  prototypes, keyed by original address
* `cheatdll/common.h` вЂ” declarations of engine data-global slots
  (extern pointer-to-T constants) and project helper prototypes
* `cheatdll/common.cpp` вЂ” the concrete engine addresses bound to those
  data globals
* `cheatdll/src/idb/battlegrounds_x2.c` (15 MB, 465k lines) вЂ” full IDA
  pseudocode dump, address-keyed
* `cheatdll/src/idb/battlegrounds_x2.lst` (64 MB, 1.1M lines) вЂ” full IDA
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
   original address and corroborates the calling convention. No match в†’
   unknown в†’ refusal trigger.
2. Internal call targets (`sub_XXXX` / `off_XXXX`): search
   `idb_functions.h` for the address literal `0x00XXXXXX`. A hit yields
   the canonical name AND the full prototype on one line; use both
   verbatim. Zero matches в†’ still unknown в†’ refusal trigger. More than
   one entry at the same address в†’ ambiguous в†’ report under "Suspicion",
   do not pick one without asm evidence.
3. Structs: search `idb_types.h` for `struct Name` as a definition line
   (followed by `{`). Forward declarations (`struct Name;`) are NOT
   definitions. Read the definition through its closing `};` and use
   field names, offsets implied by order, and types exactly as written.
4. Data globals (`dword_XXXX` / `byte_XXXX` / `unk_XXXX`, engine globals):
   resolve via `common.h` (declaration/type) + `common.cpp` (bound
   address). A data symbol absent from both is still a refusal trigger.
   The project's pointer-to-slot convention means IDA's `glob->field`
   translates to `(*glob)->field` вЂ” that single explicit deref is the
   approved equivalence; note it in the output.
5. Transitive closure: every type named in a field you actually access
   must itself be resolved the same way, unless it is only used as an
   unexercised pointer target. Gate check 1 applies per-access, not
   per-name.
6. Pseudocode: if {{PSEUDOCODE}} was not pasted, pull the function body
   from `battlegrounds_x2.c` вЂ” search for the `//----- (00XXXXXX) ------`
   marker of the resolved address and read until the next marker. Cite
   line numbers.
7. Assembly: pull evidence from `battlegrounds_x2.lst` вЂ” search
   `.text:00XXXXXX` prefixed lines between the function's `proc near` and
   `endp` lines. This is the source for gate checks 4вЂ“6, switch-table
   restoration, and the collision range. Cite line numbers.
   A function whose decode record flags its asm pass as OWED is
   decode-incomplete: it may not be presented as ready nor recorded
   WRITTEN (owner-directed 2026-11-16 after the missile_move desync).
   Batch entries record per-function asm-pass status.
   DESYNC REPORT SEMANTICS (owner-directed 2026-11-16): "function X
   desyncs" is a BISECTION VERDICT - the owner has already proven that
   uninstalling X's hook eliminates the desync. It is NOT an invitation
   to investigate callers, dependencies, or the patch ecosystem: the
   divergence is inside X's replacement. The workflow is a strict
   instruction-level re-verification of X alone against its own asm
   range (gates, operand order, float precision forms, exit paths,
   store/read sets, ABI), treating every prior verification pass of X
   as suspect.
8. Citation: every name, prototype, field, or global taken from these
   files must be cited by its ADDRESS (and the enclosing function name) —
   e.g. `setPosition @ 0x4A3C00`, `the store at 0x54F98F`. Line numbers
   into battlegrounds_x2.c/.lst are NOT stable citations: the owner
   regenerates both files (line numbers shift, pseudocode re-renders with
   corrected type names), so line-only citations rot (owner-directed
   2026-11-16). Pull function bodies by NAME/address; cite addresses.
9. The functions file writes `this_` explicitly as the first parameter
   of __thiscall pointers; when emitting C++ member-syntax or thiscall
   calls, that parameter is the hidden `this` вЂ” do not treat it as an
   extra argument.
10. Enum values appear in none of the reference files. Any enum constant
   in the pseudocode is a refusal trigger unless pasted into {{TYPES}}.
11. Cast-compatibility protocol: for every cast between distinct struct
   pointer types, establish the relationship before accepting it:
   * Check the hierarchy store (DECOMP_HIERARCHY.md) for the pair; a
     stored verified relation - or a stored verified-unrelated verdict -
     applies directly.
   * Otherwise verify by layout prefix: the narrower struct's field
     sequence (name, type, order вЂ” normalize the vfptr field, whose named
     vtable type legitimately differs per class) must be an exact prefix
     of the wider struct's. Cross-check the vtbl structs: slot names in
     order must match as a prefix. Cite both line ranges.
   * Record newly verified relations in DECOMP_HIERARCHY.md (proposed in
     the conversation, written on owner approval). Entries rot if the
     headers regenerate; keep the re-verify hint.
   * Baseв†’derived casts additionally require runtime-type evidence that
     the instance really is the derived type (IDA's own typing of the
     object, a constructor's vfptr assignment in battlegrounds_x2.lst,
     or an established store entry) вЂ” without it: Suspicion or refusal.
   * No prefix relationship and no store entry в†’ unrelated types в†’ hard
     stop: Information Request, no code.
12. Absence claims are prohibited from truncated searches: to claim a
   type or name is undefined, run the definition-pattern search
   (e.g. `^struct Name\s*\{`) over the whole file, untruncated, and cite
   the pattern used. A partial mention sample is never evidence of
   absence.
13. Call-site context: when a parameter's purpose is unclear вЂ” or naming
   would otherwise stay mechanical (a2, v3) вЂ” examine the function's call
   sites (lst XREFs, surrounding caller asm, or project-source callers)
   to infer semantics before settling names. Cite the call sites. The
   inferred meaning is reported in the output and proposed for the state
   file as verified context вЂ” never invented beyond what the sites show.

### Session state file
`cheatdll/src/idb/DECOMP_STATE.md` вЂ” read it FIRST, at session start. It
holds verified environment facts, owner rulings, the verified type
hierarchy, open suspicions, and the function ledger. Rules:
* State entries are LEADS, never grounding. Anything used in code must
  still be re-verified against the reference files and cited as if the
  state file did not exist. The greps remain the authority.
* When something durable is learned during work (a ruling, a verified
  fact, a suspicion, an approval), output the proposed state-file entry
  in the conversation; it is written only on owner approval
  (constraint 9).
* Every entry must carry its citation and a re-verify hint вЂ” entries rot;
  the hint is the invalidation mechanism.
* Companion: `cheatdll/src/idb/DECOMP_GAME_FACTS.md` вЂ” owner-supplied
  game-level semantics (object_group registry, magic-constant findings
  registry). Context for naming/comments only; never grounding.

### IDA pseudocode
{{PSEUDOCODE вЂ” optional: if omitted, the agent pulls the body from
battlegrounds_x2.c by the resolved address (protocol step 6).}}

### (Optional) IDA disassembly of the function
{{ASM вЂ” optional: the agent pulls it from battlegrounds_x2.lst (protocol
step 7). Strongly recommended in practice for anything touching floats,
width/sign ambiguity, switches, or calling conventions; several gate
checks effectively require it and will refuse without it.}}

### Overrides (rare, explicit, per-item)
{{OVERRIDE lines, if any вЂ” e.g. `OVERRIDE: accept low confidence on
field +0x1C width` or `OVERRIDE: apply bake-in of patch at <file:line>`.
Everything not explicitly overridden stays gated.}}

## Session continuity (owner-directed 2026-11-15)

Assume sufficient context for every task, always. Work autonomously
through decodes, verification passes, gates, and ledgers without
stopping at internal checkpoints; the only stop is a protocol gate
(batch approval before writing, deviation sign-offs, hard stops).
The owner will explicitly request a session close when the context is
running out - never preemptively wrap up, truncate a batch, or defer
work citing context. CONTEXT-WINDOW SIZE IS NEVER A STOPPING REASON OR
A SCOPE-REDUCTION ARGUMENT (owner-directed 2026-11-17): never cite
remaining context, token budgets, or output length when deciding what
to decode, verify, present, or write - every session point has
sufficient context for the full protocol; deferrals must be justified
by protocol gates or decode blockers only, never by size.
