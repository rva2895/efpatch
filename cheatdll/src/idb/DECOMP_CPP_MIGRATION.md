# DECOMP_CPP_MIGRATION.md — the C-style → C++ class conversion process

Companion to DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
DECOMP_GAME_FACTS.md. This file documents the plan for converting the
decompiled C-style struct definitions to a proper C++ class hierarchy,
along the way (incrementally, not after the entire engine is
decompiled). Owner-directed 2026-11-27. AMENDED 2026-11-28: the
measured deletion protocol (section 8 - the probe verdict: approach A
disqualified on the free routing, the explicit thunk form certified),
the conversion mechanics (section 3b - the name swap, the vfptr
sweep), the trampoline/dispatch rules (section 5), the forwarder form
(section 6), the pilot corrections (sections 10/12 - the runtime
vftable 0x00662C8C, STATIC 139/139).

Rules (same as the state file):
- Entries are LEADS plus recorded verification results. Anything used
  in code must be re-verified against the reference files and cited.
- The conversion is applied per class, approval-gated, and verified
  per the gates below.

## 0. The reframe: recovery, not redesign

The original game was C++ compiled by MSVC. The binary's vtables are
MSVC vtables; the flattened IDB structs are IDA's rendering of real
class layouts; the "ctor2/ctor3/ctor4" families are real overloaded
constructors; the "vtbl struct + vfptr->slot()" pattern is IDA's
rendering of real virtual dispatch. The conversion is RECOVERING the
original class declarations - names, member order, virtual order,
inheritance - from evidence we already hold. The decompiled C is the
original C++ with the classes stripped off.

This means there is a hard correctness gate available: if the
reconstructed class compiles to a vftable whose structure and
per-slot semantic mapping match the binary's .rdata vftable, the
reconstruction is proven ABI-identical. That is the per-class test.

## 1. The evidence inventory

| recovered element | source |
|---|---|
| the member lists + declaration order | the IDB struct definitions |
| the inheritance trees | DECOMP_HIERARCHY (the ctor-ladder + prefix-layout evidence) |
| the virtual tables in slot order | the *Vtbl structs (walked slot-by-slot with line-arith) |
| the method bodies | the .c functions (already decompiled and cleaned) |
| the constructors/destructors | the ctor families, the SEH unwind folds |
| the field offsets | the asm pins + the censuses |
| the global bindings | common.h/cpp |

## 2. The conversion states

Each class is in one of three states at any moment:

- C-STYLE: unconverted. The IDB struct + the vtbl struct + the owned
  free functions (or unowned).
- C++-ABI: converted. A real class with virtuals in binary slot
  order, binary-exact layout, real ctors (placement-new in the
  hooks), real methods. THE MIXED PHASE STATE.
- C++-NATIVE: post-sole-producer. The layout can diverge from the
  binary's.

The transition C-STYLE -> C++-ABI per class is atomic: the class
flips as a unit (no half-converted classes).

## 3. The ABI-preserving constraints (the mixed-phase rules)

During the mixed phase (converted + unconverted code coexisting),
every converted class must be binary-layout-identical:

| frozen | why |
|---|---|
| member order + types + sizes | unconverted code reads fields at binary offsets |
| the vtable slot order | unconverted code dispatches through the object's vfptr |
| the allocation size | the game's operator_new / Object_List machinery |
| the padding | unit_extra_padding_* / the UNIT_EXTRA scheme lives there |

These are the VERIFICATION GATE, not just constraints: compile the
class, read the emitted vftable, walk it slot-by-slot against the
original's .lst DD list. Per slot: our function is either our
decompiled replacement (name-mapped via the idb constant) or a
redirector thunk jumping to the original address. Slot count, order,
and the redirector targets must match exactly.

### 3b. The conversion mechanics (measured 2026-11-28)

THE NAME SWAP - the stdafx macro-order form (regen-proof: idb_types.h
is IDA-regenerated, so nothing project-owned lives inside it, and the
struct STAYS in the IDB - every future .c decode of the unconverted
classes dereferences Static-inherited members and needs the
rendering):
    // stdafx.h - the include order is the mechanism
    #include "engine/object/RGE_Static_Object.h"      // 1. the class, its real name
    #define RGE_Static_Object RGE_Static_Object_idb    // 2. retire the generated struct
    #include "idb/idb_types.h"                         // 3. the regen output lands renamed
    #undef RGE_Static_Object                           // 4. project code sees the class
Token-safe: RGE_Static_ObjectVtbl / RGE_Static_Object__method /
RGE_Static_Object_RGE_Static_Object are distinct preprocessor tokens.
The idb struct survives as RGE_Static_Object_idb (alive in the decode
references, dead in the build). THE LAYOUT GATE IN-TYPE:
static_assert(sizeof(RGE_Static_Object) == 180) plus the offsetof
asserts on the census anchors (master_obj 0x14, world_x 0x48,
object_state 0x58, worker_num 0x5F, mGroupID 0x88, damaged_lately_
timer 0x84 - the full set per the S2a census) compile into every TU.
The class header is self-contained: it forward-declares the ~10
pointer-referenced types and needs no idb_types.h ordering.

THE VFptr SITE SWEEP (the real blast radius, measured): the named
vfptr member is GONE once the class has virtuals (the implicit vtable
pointer cannot be named). The measured vfptr radius: 1,291 tokens in
32 engine files + 243 in the legacy patch tree; the true break-set =
only the sites whose base expression is RGE_Static_Object*-typed
(the derived flattened structs keep their own named vfptr fields
until their own flips; the master-typed code is untouched). The
compiler enumerates the set: each broken site = one fix, and the fix
= the named virtual call (the dispatch through the object's current
vfptr - byte-identical to the old form, already governed by the
dispatch fidelity rules). The explicit vfptr STORES in our ctor/dtor
bodies die by design (the C++ ctors store the vtable implicitly -
ours; the mixed-phase neutrality holds iff the vtable gate passes).

THE FLIP'S ATOMICITY: the class header + the stdafx swap + the vfptr
site sweep + the method moves = ONE commit (the tree compiles only as
a unit); git revert = the rollback to the verified tree.

## 4. The conversion order = the ladder order

C++ inheritance flows base-first, so the conversion order is the
verified runtime ladder:

    Static (139/139) -> Animated (12/12) -> Moving (97/97) -> Action (45/45)
                                                -> Combat (53/53) -> Missile (16/16)
                                                                -> TRIBE_Combat -> TRIBE_Building
            Static -> Tree (2/2)
            Animated -> Doppleganger (9/9)

The masters convert in their own parallel ladder with the same
base-first order. A class converts WHEN ITS METHOD SURFACE IS
COMPLETE (all exports owned) - not before. The pilot exception: the
thinnest complete class converts first to prove the pipeline.

## 5. The per-class conversion checklist

1. THE HEADER: the base class, the members in IDB declaration order,
   the virtuals in binary slot order (the vtbl struct members ->
   virtual declarations one-for-one). THE MEMBERS ARE PUBLIC (the
   mixed phase: the unconverted TUs access the fields directly; the
   recovered original was public-member style). NO DESTRUCTOR VIRTUAL:
   the class's slot-0 thunk = an ordinary virtual declared at its
   measured slot (§8; the runtime classes: first; the masters: fifth),
   and the dtor body = a plain non-virtual destructor. Un-owned
   slots: declare virtual with a FORWARDER body calling the original
   address through a cpp-local typed constant (the SPEED2 extern
   precedent; /O2 emits the tail-jmp; the gate follows it). NEVER
   reimplement a nullsub slot as an empty body (the retn-width and
   the x87-residue classes - nullsub_29 is a constant-loading proc).
2. THE METHOD BODIES: the existing decompiled .c bodies move into
   the class, this_ -> this, the class prefix drops. Near-1:1.
   INTERNAL-CALL FIDELITY: asm VTABLE dispatches -> the named virtual
   calls (the compiler emits the slot offset); asm DIRECT calls ->
   the QUALIFIED member calls (this->Class::method(...) - the direct
   implementation, never the unqualified dispatch, which would invoke
   derived overrides the original never called).
3. THE HOOKS: the existing free functions become one-line
   trampolines using the QUALIFIED call:
       static int __fastcall RGE_Static_Object_play_unload_sound_trampoline(
           RGE_Static_Object* this_)
           { return this_->RGE_Static_Object::play_unload_sound(); }
   The qualified call is LOAD-BEARING: the unqualified form is a
   virtual dispatch - for a derived instance still carrying the game's
   derived vftable it lands on the original entry, which is E9-patched
   to this very trampoline -> infinite recursion. The qualified form
   also carries the correct semantics (the hook sits on the base
   implementation's entry). The installer model unchanged - same
   setHook calls, same addresses, same E9 patches.
4. THE CONSTRUCTORS: the ctor hooks switch to placement-new
   (REQUIRES #include <new> - the standard placement-new declaration):
        static RGE_Static_Object* __fastcall RGE_Static_Object_ctor_trampoline(
            RGE_Static_Object* this_, DWORD dummy,
            RGE_Master_Static_Object* tobj, TRIBE_Player* obj_owner,
            float x, float y, float z, int do_setup)
        { return new (this_) RGE_Static_Object(tobj, obj_owner, x, y, z, do_setup); }
   The placement-new runs the real C++ constructor chain, which
   stores the compiler-emitted vftable (ours - the mixed-phase
   neutrality holds iff the vtable gate passes: every slot's effective
   target = the original's, and the hooks live at the original entries
   regardless of which table the object carries). THE CTOR READS ARE
   SECOND CTOR OVERLOADS (e.g. RGE_Static_Object(int infile,
   RGE_Game_World*, int do_setup) @0x0054B600) - each with its own
   placement-new trampoline. NO operator delete/delete[] overloads;
   the class declares the PRIVATE undefined delete tripwire + the
   deleted copies/moves (§8).

5. THE VTABLE VERIFICATION: compile -> read the emitted vftable ->
   walk it slot-by-slot against the original's .lst DD list ->
   per-slot assert (our method for owned / the forwarder for
   un-owned - follow one jmp/call to the exact original address).
   Plus: slot count, the delete-thunk slot per section 8, /GR-
   (no COL at [-1]).
6. THE GATE + THE LEG RUNS (section 14) + THE IN-GAME TEST.

## 6. The vtable verification model

The pointer VALUES can never match (ours live in the DLL, the
originals in the binary). The correct model is structural +
semantic:

Two vtables are equivalent iff:
1. the slot count matches (the DD list length in .rdata),
2. every slot resolves to a function with the same observable
   behavior - where "resolves" means:
   (a) our decompiled replacement of the original slot-i function,
       OR
   (b) a redirector thunk that jumps to the original slot-i
       address.

The original slot map is free from the .lst (each vftable renders
as a DD list with the target names). Our slot map comes from the
/Fa listing (the CONST-segment vftable). The verification = walk
both tables side by side, slot i vs slot i.

The redirector trick makes the mapping total: for any slot not yet
decompiled, the vtable entry is a FORWARDER VIRTUAL calling the
original address through a cpp-local typed constant (the SPEED2
extern-constant precedent; /O2 emits the tail-jmp - never a naked
thunk, never an empty body: the nullsub targets are not uniformly
empty, nullsub_29 is a constant-loading proc, and the retn widths
vary). Every slot has a provable effective target: decompiled -> our
verified body; else -> the exact original address (the gate follows
one jmp/call). As decompilation progresses, forwarders graduate into
real methods. The /Fa census note: the idb-constant calls fold to
IMMEDIATES in the emission (the namespace-scope const pointers have
internal linkage - e.g. mov eax, 4830320 ; 0049b470H) - the
constant-call census form is the folded immediate, not the symbol
name.

The runtime self-test: after install_engine(), per converted class,
walk the original .rdata vftable vs our object's vfptr:
  - slot count equality
  - per slot: resolve our effective target (follow one jmp for
    redirectors) -> compare with the original's slot address
  - for decompiled slots: verify by the install-table name mapping
Log the full table: slot, orig addr, our addr, effective target,
MATCH. Any regression in slot order, count, or mapping trips
immediately.

## 7. The structural checks (byte-comparable)

| check | how |
|---|---|
| slot count | the DD list length in the original .lst vs the /Fa vftable length |
| dtor pair placement | no compiler dtor slots exist in this binary - the delete-thunk is an ordinary virtual at its declaration position (runtime: slot 0; masters: slot 5) - read off the DD list per class |
| RTTI | if the original has CompleteObjectLocator pointers at [-1], the build used /GR; match the flag (check for ??_R0 symbols in the lst) |
| multiple-vftable structure | the count and the base offsets must match (our single-inheritance hierarchy = one vftable per class) |

## 8. THE DELETION PROTOCOL - MEASURED (the probe verdict 2026-11-28;
   cl 19.16, x86, /std:c++17, /O2 /arch:SSE2 /fp:fast /GR-; artifacts
   temp\opencode\dtorprobe*.asm. Supersedes the earlier unknowns - the
   dtor-pair merging and the __vecDelDtor declaration questions dissolved.)

THE BINARY HAS NO COMPILER-GENERATED DTOR SLOTS. The vtables'
"destructors" are hand-rolled, scalar-only delete-thunks - ORDINARY
VIRTUAL FUNCTIONS (IDA names them vector_deleting_destructor /
scalar_deleting_destructor; same thunk shape). The recovery declares
each thunk as an ordinary virtual at its measured slot with its
verbatim body; the dtor BODY is a plain non-virtual destructor (no
vtable slot), invoked by the thunk via the qualified pseudo-dtor and
by the derived chains. The base dtor being non-virtual makes
virtual-dtor inheritance impossible down the whole ladder - the ABI
is structurally self-defending.

- THE DTOR SLOT = ITS DECLARATION POSITION among the virtuals
  (measured: dtor first -> slot 0; fifth -> slot 4; last -> slot 5).
  The runtime classes declared ~T first (the measured slot 0s); the
  masters declared it fifth (the master Static DD walk + the 1b-5 /
  1b-7 __vecDelDtor +0x14 dispatches). The "always slot 0" convention
  = just the common declaration order.
- THE COMPILER-GENERATED DTOR CANNOT ROUTE THE FREE to the game's
  deallocator - approach A (a plain virtual dtor + compiler dispatch)
  is DISQUALIFIED on the measured free side: no class-scoped overload
  (unsized, sized, /std:c++14 /Zc:sizedDeallocations-) is honored -
  the generated thunk's free = the CRT's sized global operator delete
  (the strong ??3@YAXPAXI@Z reference - the wrong-heap class) or, with
  a member declared, the WEAK __global_delete defaulting to
  __empty_global_delete (ret 0 - the leak class). The emitted vtable's
  dtor slot references ??_E as an UNDEFINED WEAK EXTERNAL (default =
  the scalar ??_G) - the unowned-emission class. The slot side A got
  right (the declaration position) is inherited by the explicit form.
- B IS CLEAN END-TO-END (measured): the vtable slot 0 = a strong
  reference to our authored member; the body authored verbatim
  against the original's thunk (runtime Static: 18 lines, scalar-only,
  lst :551850-:551868 - call the dtor body; flags & 1 ->
  operator_delete_internal(this); return this).
- NO operator delete / operator delete[] OVERLOADS exist on the
  converted classes. The free happens exactly where the original puts
  it: the thunk's operator_delete_internal call (the game's 0x632B42,
  common.h:152). The heap pairing stays operator_new_internal <->
  operator_delete_internal, the original's.
- THE DELETE TRIPWIRE (hard gate, owner choice): the class declares
  PRIVATE undefined operator delete / operator delete[] - any
  delete-expression on the type = C2248 at the site (measured). The
  "= delete" form is MEASURED INERT: MSVC ignores deleted deallocation
  functions in delete-expressions and falls through to the global CRT
  delete (the trip silently absent, the listing showing ??3). The
  deleted copies stand (a plain deleted-function mechanism, unaffected).
  The members are deliberately undefined; the first full gate run
  verifies the link (the unreferenced ??_G COMDAT is discarded before
  resolution under /Gy) - a stray LNK2019 flips to the functional
  private form (the bodies routing to operator_delete_internal).
- THE RTTI: the original vftables carry NO COL at [-1] (runtime Static:
  the preceding dword = the damage 1.5f constant) -> the conversion
  TUs compile /GR-.
- THE PROBE PROTOCOL EXTENSION: any compiler-emission-dependent form
  decision (dtor shapes, delete routing) runs the compiled-and-emission-
  read probe under the project's real gate flags first - the NaN-probe
  precedent extended to the ABI forms.

## 9. The field classification (per struct, before writing the class)

| bucket | examples | fate |
|---|---|---|
| runtime state | hp, world_x, selected, mGroupID | class members |
| cache/derived | screen_x_offset, shadow_*, MapAddress | keep as members or recompute - decide per field |
| stream mirrors | the setup/setup_read/save ladders | the serialization schema - decoupled once sole-producer |
| vtbl pointers | vfptr | deleted (real virtual functions) |
| padding | unit_extra_padding_* | reclaimed; BUT the UNIT_EXTRA/LOS patches that repurpose padding become real members |

## 10. The migration order (the safe sequence)

C++ inheritance flows base-first, so the ROOT converts first:

1. STATIC (the root, 139/139 owned - find_closest_resource shipped
   2026-11-28 as batch S12, the two-pass protocol): the header (the
   members + the 219 virtuals in the DD-list order), the 139 owned
   method bodies, the ctor placement-new trampolines, the
   delete-thunk virtual at its measured slot (slot 0 - the original
   declared it first), the vtable verification against the RUNTIME
   vftable .rdata:00662C8C (lst :994786; 219 slots / 127 distinct
   targets / 119 nullsub-target slots / no RTTI COL).
2. TREE immediately after: the derived-class proof (2 exports, zero
   extension fields - the Static mirror). The base ctor chain, the
   Tree vfptr, the overrides, the emitted vftable vs
   .rdata:00663B64.
3. UP THE RUNTIME LADDER: Animated -> Moving -> Action -> Combat ->
   TRIBE_Combat -> TRIBE_Building (+ Doppleganger off Animated).
   Each conversion runs the full per-method battery (the
   store-and-read diff, the exit-path inventory, the emission
   census, the paired-capture replay - the decompiled original =
   the reference, the new method = the candidate).
4. THE MASTER FAMILY IN PARALLEL (the same base-first order: the
   RGE_Master_Static_Object root, then the derived tiers).
5. THE GLOBALS LAST: base_game, OBSystem_*, SDI_List stay
   pointer-to-slot until the engine context object exists; then
   collapse into an Engine aggregate (or keep the slots - the
   game's own init writes them either way).
6. THE ABI RELAXATION ONLY AT THE END: once a class is 100%
   constructed, dispatched, and destroyed by owned code, its
   layout can legally diverge. In practice little reason - the
   recovered layout is the original's.

## 11. The risks, honestly ranked

1. THE PARTIAL-VTABLE TRAP (highest): converting a class whose vtbl
   slots are only partially decompiled. Every un-decompiled slot
   needs a redirector virtual. Mitigation: the slot census per
   vtbl (already done for the runtime Static vtbl - 219 slots).
2. THE INVISIBLE MUTATOR (medium): the original re-reads fields
   because virtual calls and OB machinery mutate them (the
   capture's own selected re-reads). The clean C++ model must
   preserve the re-read semantics - no compiler CSE across the
   dispatches. The .c bodies already encode the re-reads; keep
   them.
3. THE FLOAT MODEL (medium): the decompiled bodies encode the x87
   53-bit semantics. Pin per subsystem - pathing/combat keep the
   precise brackets; UI accepts the modern model. The
   paired-capture replay is the gate.
4. THE HEAP PAIRING (low): the *_internal wrapper discipline
   disappears once sole-producer (one allocator, RAII). Until
   then, unchanged.
5. THE SAVE/DAT FORMATS (constant): frozen regardless. The
   serialization layer is the one place bit-exactness is
   permanent, not a migration phase.

## 12. What to do first: STATIC + TREE

THE PILOT = STATIC + TREE as a combined milestone:

STEP 1 - STATIC: the root class (139/139 owned - S12 shipped
2026-11-28; no redirectors needed for any owned slot). The header
(the members in IDB order + the 219 virtuals in the binary slot
order), the 139 owned method bodies from the .c, the ctor
placement-new trampolines (both ctor overloads - the primary
:3774 @0x0054B2E0 and the stream ctor :3787 @0x0054B600), the
delete-thunk virtual at slot 0, the vtable verification against the
binary's RUNTIME .rdata:00662C8C vftable (NOT the master-side
0x0065B40C - that table belongs to the parallel master ladder, whose
thunks sit at slot 5). This proves the BASE-CLASS conversion: the
header scale, the forwarder pattern for the un-owned slots, the ctor
chain, the vtable gate.

STEP 2 - TREE immediately after: the derived-class proof (2
exports, zero extension fields - the Static mirror). `class
TRIBE_Tree_Object : public RGE_Static_Object` with zero new
members, the 2 overrides, the ctor chain (the base ctor + the
Tree vfptr), the emitted vftable vs .rdata:00663B64. This proves
the DERIVED-CLASS conversion: the extension pattern, the override
plumbing, the ctor chain through a converted base.

If both pass (the vtable gates + the in-game tree test), the
pipeline is proven for the base and derived patterns and the
ladder conversion runs as a batch operation per tier.

## 13. The compounding payoff

Each converted class improves every subsequent decode:
- the new batches write this->world_x not this_->world_x - the
  field-offset transcription risk drops to zero (the compiler
  computes the offsets from the IDB struct)
- the vtbl dispatches become named virtual calls - the slot-arith
  transcription risk drops to zero (the compiler computes the
  slot offsets)
- the wrong-field-name class (the medic_worker / var_8=+0x88
  mislabels) disappears - the compiler catches unknown members at
  compile time
- the store-and-read diff narrows to the body (the field offsets
  are compiler-guaranteed)

On the decode side: the unconverted classes' decodes keep working
unchanged - the IDB structs and the free-function form are still
valid for reading the asm. The two forms coexist per the
ABI-preserving constraints.

## 14. THE CONVERSION A/B - THE LEG-GATED VERIFICATION (2026-11-28)

THE MODEL: the setHook targets are FIXED at conversion time (one
stable qualified-call trampoline per hooked address, installed once,
never flipped). The A/B flip lives INSIDE the method bodies:

    bool g_origStatic = false;                    // the class leg
    // per-method switches: added ON DEMAND during a hunt, never pre-built
    bool g_origStaticTeleport = false;

    double RGE_Static_Object::teleport(float x, float y, float z)
    {
        if (g_origStatic || g_origStaticTeleport)
            return RGE_Static_Object__teleport_orig(this, x, y, z);  // replay+jmp past the E9
        ...the new implementation...
    }

WHY THE INSTALLER-FLIP MODEL FAILS AFTER CONVERSION: the converted
class creates dispatch paths that never pass through the hook. Five
path classes reach the owned surface:
  1. unowned code via the idb constants -> the original entry -> E9
     -> the trampoline;
  2. unowned code via the GAME vftables (derived instances) -> the
     original entry -> E9 -> the trampoline;
  3. exact-Static instances via OUR emitted vftable -> our member
     DIRECTLY (no E9, no hook);
  4. converted bodies' internal QUALIFIED calls -> our member
     directly;
  5. cross-class owned code -> paths 1 or 4.
The setHook flip rewires only 1-2; 3-4 stay on the new implementation
- an "original" leg would run a MIXED call graph and the A/B
attribution would be void. The member-body flip covers all five:
every path funnels through the members, and each member's flip
selects the leg.

THE TWO DISPATCH KINDS:
- DIRECT CALLS (paths 1/2 and 5's constant form): E9 -> trampoline ->
  member -> per the flip -> the per-method _orig replay+jmp past the
  E9. The _orig forms are esp-adjusted for the CALL-FROM-MEMBER depth
  (the DIMU-era logger-calls-_orig precedent: the loggers called the
  _orig trampolines as functions, the replayed prologue operands
  written for that frame depth). THE CONSTRUCTION = DECOMP_TEMPLATE.md's
  TRAMPOLINE CONSTRUCTION PROCEDURE, mandatory per form: the replay
  set derived from the .lst walk (the minimal instruction prefix
  whose accumulated end >= entry+5; the resume = the first .lst
  boundary at or after it - never computed), the transfer = ALWAYS
  push addr / ret (no register-held jump targets), the replay bytes
  binary-verified, the /Fa walk per form.
- VIRTUAL DISPATCHES: derived instances (game vftables) hit the
  original entry -> E9 -> member -> flip; exact-Static instances (our
  vftable) hit our member -> flip. THE ORIG LEG STAYS FULLY ORIGINAL
  INTERNALLY: the original body's own virtual calls dispatch through
  the object's current vfptr and land back in a member, see ORIG, and
  reach the original callee; the NEW leg symmetrically reaches the new
  callees. THE WHOLE CONVERTED SURFACE FLIPS ATOMICALLY - the original
  call graph in one leg, the new call graph in the other, never mixed.

THE TWO-LEVEL SWITCH HIERARCHY (all previous desyncs were found with
per-method switches; the per-method apparatus is applied ON DEMAND,
never pre-built):
- DAY ONE: every converted member carries only the class-global check
  (g_orig<Class>) - the coarse legs (fully-original / fully-new) align
  here first.
- ON A DESYNC: the method under suspicion gets its _orig replay form +
  its own global (g_orig<Class><Method>) + the extended condition
  (g_orig<Class> || g_orig<Class><Method>) - one DIAG-marked, gated
  edit per addition (the naked replay from the .lst instruction
  boundaries, the esp-adjusted operands for the call-from-member
  depth, the liveness-safe transfer, the /Fa verification).
- THE ATTRIBUTION SEMANTICS: with a method flipped the graph is mixed
  BY DESIGN - M runs original while its callees run new; if the desync
  follows M's flip, M's body is the divergence; if it persists, the
  callees. The coarse class leg is the sanity frame (both coarse legs
  align before any per-method bisection starts).
- NAMING: g_orig<Class> / g_orig<Class><Method> (the
  g_checkAccelTraceOrig house style).
- THE STARTUP-CONSTANT COHERENCE: the globals are fixed before ANY
  installer runs (compiled defaults for the two leg builds, or env-read
  at the DLL's earliest init) - the bake gating below depends on it.
- SHIP: the class globals land on NEW permanently and the entire
  apparatus (the globals + every _orig form) is deleted under the
  zero-diagnostic-symbol sweep (the updateMovement protocol).

THE BAKE GATING (the retired patch call sites under the conversion):
the retirement as-is makes every ORIG leg VANILLA at the baked sites -
the shipped behavior those functions must reproduce is original +
patch, and the retired sites mean the replayed original runs without
the patch (every bake = a false divergence, masking real ones). THE
THREE PATCH CLASSES:
1. INTERIOR/BYTE PATCHES (the majority): UN-RETIRED, gated on the
   OR-form of the owning method:
       // los.cpp - the site, gated on the class leg OR its owning method:
       if (g_origStatic || g_origStaticSave)
           setHook((void*)0x0054F766, los_write_save2_hook);
   The ORIG run installs the patches -> the replayed originals execute
   them (the interior stubs' esp adjustments are upstream-transparent
   to our entry E9); the NEW run does not install them (the interior
   code is dead under the conversion; the bodies' bakes carry the
   behavior - no double-apply).
2. ENTRY-FIGHT / VERBATIM-ADOPTION SITES: STAY RETIRED PERMANENTLY
   (re-enabling overwrites our stable E9 and breaks the conversion and
   the flip). Their behavior reaches the ORIG leg via the
   FOLD-BEFORE-REPLAY in the member's ORIG path (processUnitExtra(this);
   return update_orig(...)), mirroring the NEW body's inline bake -
   both legs apply the patch effect exactly once, no external gate
   (the fold lives inside the flipped bodies and follows the method's
   own switch automatically).
3. MAP_SIZE: the never-bake exception - live in both legs unchanged
   (the intentional mod divergence; both legs carry it, the A/B stays
   consistent).
THE SITE->METHOD TABLE: a STEP A deliverable per conversion - per
retired site: the address, the owning method (the [proc, endp)
mapping the collision audits already produce), the patch form, the
gate expression. THE RE-ENABLE AUDIT: each un-retired interior site
is range-checked against our entry E9 span (a site inside the first 5
bytes = the entry-fight class = stays retired).

CHECKLIST ITEM 6 references this section: the gate = the vtable walk
(section 5 item 5) + the leg runs (this section) + the in-game test.

## 15. THE CONVERSION-GATE ADDENDA (measured 2026-11-28, the Static pilot; owner-approved)

These bind every subsequent conversion in the ladder (Animated next).

**15.1 THE BOUNDARY-CAST SWEEP (a per-conversion checklist item).** Each
class's conversion DELETES every boundary cast referencing its renamed
struct, compiler-enumerated: after the flip, the full-tree gate surfaces
the class<->idb mismatches (C2664/C2440/C2446/C2039); each fix removes a
cast (never adds a second), and the removals audit-log per site (file,
line, code, before/after). The two directions: the class->idb ARG casts
(`(RGE_Static_Object_idb*)` on the idb-vtbl/field positions) and the
idb->class RESULT casts (`(RGE_Static_Object *)` on the constant-call
positions - the idb constants type-check against the class forever, so
those casts die when the VALUE's holder converts). THE LEVEL RULE: never
stack casts - a `**` position takes the `**`-level cast exactly once
(the stacked single-star chains broke advcheat sel_units et al.).
THE TERMINAL STATE: at the ladder's completion every boundary cast, the
bridge operator, and the `_idb` rendering itself are deleted (the
zero-trace sweep).

**15.2 THE MASTER-TIER DISPATCH RULE.** The named-virtual conversion of a
`base->vfptr->method(base, ...)` site is valid ONLY when the base object's
RUNTIME vftable shares the converted class's table (the runtime tier -
Static+ its derived ladder). A MASTER-tier instance carries the master
vftable (~30 slots, different indices) - converting that site to the named
virtual reads the WRONG SLOT (the effectUnitVar catch). The decision
procedure per site: identify the base expression's true tier (the
declaration type + the factory identity) BEFORE converting; the
master-tier sites keep the vtbl-dispatch form (self-consistent under the
rename) and take only the boundary casts.

**15.3 THE PLACEMENT-NEW FORM: `::new` MANDATORY.** The ctor trampolines
use the GLOBAL-scope placement form `::new (this_) T(...)`: the
class-scope `new` triggers the throwing-ctor cleanup lookup of the CLASS
deallocation function - with the private-undefined operator-delete
tripwire that is an unconditional LNK2001 (the measured catch). `::new`
restricts the cleanup lookup to the global placement delete; the tripwire
stays armed for real delete-expressions.

**15.4 THE OPERATOR-DELETE LNK FLIP (the sec-8 contingency, now
measured).** The compiler emits the scalar-deleting-destructor COMDAT
($LN*@scalar) for every polymorphic class; its body calls the class
operator delete even when nothing references the COMDAT. The first
real-config link therefore hits LNK2019 on the private-undefined
tripwire - the documented flip: give both operator delete/delete[]
BODIES routing to operator_delete_internal (the tripwire stays private;
the external delete-expressions still fire C2248; the game-heap pairing
holds). Applied and shipped in the Static pilot.

**15.5 THE NO-IDB-PROTOTYPE ABI ADJUDICATION.** The exports ABSENT from
idb_functions.h (the Static pilot: get_group_number_mask, fill_pathable_
map, find_closest_resource, logDebug) have NO header authority for their
ABI - the classification (member vs __fastcall free) must be adjudicated
from the asm BEFORE the transform: the retn width (the stack-arg count),
the ECX semantics (the object deref vs the plain-arg read), and the
caller arg construction. The get_group_number_mask lesson: the S1 decode
knew (`__fastcall int(int) - no this`, its own comment) and the
transform still mis-filed it as a member - the hooked entry read the
group number as this and garbage as the arg. The owner catch. The
adjudication artifact (the asm citation + the caller census) goes in the
header/class comment at the definition site.

**15.6 THE /GR- CONFIG COHERENCE.** The conversion TUs compile /GR- AND
the project configuration must agree: RuntimeTypeInfo=false in all four
vcxproj configs (landed 2026-11-28). With /GR anywhere, the emitted
vftable carries the ??_R4 COL at the head - the slot layout shifts and
the game-side dispatch reads garbage. The zero-RTTI claim was verified
(the 13 typeid-token hits = the typeID field names, no dynamic_cast).