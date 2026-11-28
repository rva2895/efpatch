#pragma once
// ============================================================================
// TRIBE_Tree_Object.h - the derived-class proof (the TREE conversion step,
// 2026-11-28). The first DERIVED conversion in the ladder: it proves the
// extension pattern (zero new members), the override plumbing, and the
// ctor chain through a converted base.
// ============================================================================
// THE RECOVERY: the class behind the binary's runtime Tree tier
// (sizeof 180 = 0xB4 - the RGE_Static_Object mirror with ZERO extension
// fields; the vftable .rdata:00663B64 = 219 entries (0x663B64..0x663ED0),
// identical to the Static table except:
//   slot 0:  RGE_Animated_Object__vector_deleting_destructor (0x0046FE70)
//            - the BEHAVIORALLY-IDENTICAL form: the Animated vdel body =
//            j_RGE_Static_Object__destructor + flags&1 -> delete, i.e. the
//            same Static dtor body + the same game-heap delete (the Animated
//            layer carries no Tree state). Our inherited slot 0 (the Static
//            vdel member) is the exact observable equivalent.
//   slot 53: TRIBE_Tree_Object__set_object_state (0x00564E40) - the pure
//            forwarder to RGE_Static_Object__set_object_state (0x550970);
//            our override reproduces it with the qualified base call.
//
// THE CTOR CHAIN (the asm 0x564DB0-0x564E30): the base Static ctor with
// do_setup = 0 ALWAYS, the Tree vftable store, then IF do_setup the base
// setup. The C++ form: the mem-init passes 0; the compiler stores the Tree
// vftable before the derived body (the standard construction order = the
// original's order); the body calls the base setup through the qualified
// member form (the asm direct call to the base implementation). The
// original ctor's SEH fold (chunk 0x644CC0 -> the Static destructor) is
// subsumed by the compiler-generated ctor-unwind of the C++ chain (the
// throwing base ctor destroys the base subobject - the same destructor).
//
// THE LAYOUT CONTRACT: the member order = inherited verbatim (zero new
// members); sizeof == 180; no RTTI (/GR-). The static_asserts below are
// the derived-layout gate compiled into every TU that names the class.
// ============================================================================

class TRIBE_Tree_Object : public RGE_Static_Object
{
public:
    // ---- the constructor (the placement-new trampoline target) ----
    TRIBE_Tree_Object(TRIBE_Master_Tree_Object* tobj, TRIBE_Player* obj_owner,
                      float x, float y, float z, int do_setup);   // 0x00564DB0 (:4084)

    // ---- the slot-53 override: the pure forwarder to the base
    // ---- implementation (the original proc tail-calls the base proc)
    virtual void set_object_state(char);
};

// ---- the derived-layout gate ----
static_assert(sizeof(TRIBE_Tree_Object) == 180, "TRIBE_Tree_Object: the size drifted from the binary (0xB4)");
static_assert(sizeof(TRIBE_Tree_Object) == sizeof(RGE_Static_Object), "TRIBE_Tree_Object: the Tree = the Static mirror - zero extension fields");
