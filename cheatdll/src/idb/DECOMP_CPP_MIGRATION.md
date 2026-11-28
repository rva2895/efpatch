# DECOMP_CPP_MIGRATION.md — the C-style → C++ class conversion process

Companion to DECOMP_STATE.md, DECOMP_TEMPLATE.md, DECOMP_HIERARCHY.md,
DECOMP_GAME_FACTS.md. This file documents the plan for converting the
decompiled C-style struct definitions to a proper C++ class hierarchy,
along the way (incrementally, not after the entire engine is
decompiled). Owner-directed 2026-11-27.

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

## 4. The conversion order = the ladder order

C++ inheritance flows base-first, so the conversion order is the
verified runtime ladder:

    Static (138/139) -> Animated (12/12) -> Moving (97/97) -> Action (45/45)
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
   virtual declarations one-for-one, including __vecDelDtor and the
   dtor pair at their binary slots). Un-owned slots: declare virtual
   with a redirector body (jmp to the idb constant / the original
   address).
2. THE METHOD BODIES: the existing decompiled .c bodies move into
   the class, this_ -> this, the class prefix drops. Near-1:1.
3. THE HOOKS: the existing free functions become one-line
   trampolines:
       static int __fastcall RGE_Static_Object_play_unload_sound_trampoline(
           RGE_Static_Object* this_)
           { return this_->play_unload_sound(); }
   The installer model unchanged - same setHook calls, same
   addresses, same E9 patches.
4. THE CONSTRUCTORS: the ctor hooks switch to placement-new:
       static void __fastcall RGE_Static_Object_ctor_trampoline(
           RGE_Static_Object* this_, DWORD dummy,
           RGE_Player* owner, float x, float y, float z)
       { new (this_) RGE_Static_Object(owner, x, y, z); }
   The placement-new runs the real C++ constructor chain, which
   stores the compiler-emitted vftable.
5. THE VTABLE VERIFICATION: compile -> read the emitted vftable ->
   walk it slot-by-slot against the original's .lst DD list ->
   per-slot assert (our method for owned / the redirector for
   un-owned). Plus: slot count, the dtor-pair placement, RTTI
   presence.
6. THE GATE + THE IN-GAME TEST.

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
decompiled, the vtable entry points at a naked thunk:
    __declspec(naked) void __fastcall slot_N_redirect()
        { __asm jmp dword ptr [original_addr] }
Every slot has a provable effective target: decompiled -> our
verified body; else -> the exact original address. As
decompilation progresses, redirectors graduate into real methods.

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
| dtor pair placement | MSVC puts the deleting/scalar dtor slots at fixed positions from the declaration |
| RTTI | if the original has CompleteObjectLocator pointers at [-1], the build used /GR; match the flag (check for ??_R0 symbols in the lst) |
| multiple-vftable structure | the count and the base offsets must match (our single-inheritance hierarchy = one vftable per class) |

## 8. The honest unknowns (the gate finds them)

1. THE DTOR-PAIR MERGING: the binary has a single __vecDelDtor at
   slot 0, not the two-entry dtor pair MSVC normally emits. The
   original's /O2 merged the identical dtor bodies into one entry.
   Our /O2 should merge identically IF our dtor bodies are
   equivalent - the gate proves it or flags it. If MSVC won't
   merge: the fallback = /O1 for the dtor-emitting TU, or a
   #pragma-managed form.
2. THE __vecDelDtor DECLARATION: void* (this, unsigned int flags).
   The exact C++ declaration to produce that slot signature at
   that slot position is a one-iteration fix per the vtable gate.
3. THE RTTI COL CONTENTS: even with /GR matched, the type_info
   names differ. Harmless (nothing reads them - dynamic_cast is
   never used in the decompiled code), but document it.

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

1. STATIC (the root, 138/139 owned): the header (the members + the
   ~219 virtuals in slot order), the 138 owned method bodies, the
   ctor placement-new trampolines, the vtable verification.
   find_closest_resource (the one un-owned export) = a redirector
   virtual until its decode completes.
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

STEP 1 - STATIC: the root class (138/139 owned, the one gap =
find_closest_resource as a redirector virtual). The header (the
members in IDB order + the ~219 virtuals in the binary slot order),
the 138 owned method bodies from the .c, the ctor placement-new
trampolines, the vtable verification against the binary's
.rdata:0065B40C vftable. This proves the BASE-CLASS conversion:
the header scale, the redirector pattern for un-owned slots, the
ctor chain, the vtable gate.

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
