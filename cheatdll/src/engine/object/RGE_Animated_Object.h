#pragma once
// ============================================================================
// RGE_Animated_Object.h - the converted runtime Animated tier (the ladder
// conversion, 2026-11-28; the forwarder-first route - the base = the
// converted RGE_Static_Object).
// ============================================================================
// THE RECOVERY: the class behind the binary's runtime Animated tier
// (sizeof 184 = 0xB8 = the Static 180 + the Animated extension float speed
// @+0xB4 = 180; the vftable .rdata:006566A0 = 220 entries
// (0x6566A0..0x656A10) = the Static's 219 + ONE new appended virtual).
//
// THE VTABLE STRUCTURE (the DD-list walk vs the Static table):
// - the 9 overrides: slot 0 vector_deleting_destructor (the Animated vdel
//   body = j_RGE_Static_Object__destructor + flags&1 -> delete - the same
//   Static dtor body + the same game-heap delete; the Animated layer adds
//   nothing), 29 recycle_in_to_game, 37 update, 40 save, 51 transform,
//   79 set_being_worked_on, 80 release_being_worked_on, 96 getSpeed
//   (the Static's = nullsub_28 - the Animated supplies the real getter),
//   167 setup_read.
// - THE HIDE-AND-APPEND (the vtable evidence): the Animated's setup proc
//   @0x422C30 is NOT the slot-168 override - the Animated slot 168 stays
//   the Static setup proc (inherited), and RGE_Animated_Object__setup is
//   APPENDED as the NEW virtual at slot 219. The C++ semantics: the
//   Animated setup's tobj parameter is RGE_Master_Animated_Object* (the
//   base's = RGE_Master_Static_Object*) - the different parameter type
//   HIDES the base virtual and the new virtual appends. The binary
//   structure validates the original class's declaration exactly.
// - the setup proc is invoked DIRECTLY by the ctors and the derived tiers
//   (the idb constant :548), never through slot 168.
//
// THE LAYOUT CONTRACT: the member order = the inherited verbatim + the
// extension; sizeof == 184; no RTTI (/GR-; the vcxproj RuntimeTypeInfo=
// false). The static_asserts below are the derived-layout gate compiled
// into every TU that names the class.
// ============================================================================

class RGE_Animated_Object : public RGE_Static_Object
{
public:
    float speed;                                       // +0xB4 = 180 (the Animated extension; sizeof 184)

    // ---- the constructors (the placement-new trampoline targets) ----
    RGE_Animated_Object(RGE_Master_Animated_Object* tobj, TRIBE_Player* obj_owner,
                        float x, float y, float z, int do_setup);   // 0x00422980 (:538)
    RGE_Animated_Object(int infile, RGE_Game_World* gworld, int do_setup);   // 0x00422B70 (the stream ctor)

    // ---- the slot overrides (the signatures inherit the base declarations) ----
    virtual void* vector_deleting_destructor(unsigned int flags) override;   // slot 0
    virtual void recycle_in_to_game(RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
                                    float x, float y, float z) override;      // slot 29
    virtual unsigned __int8 update() override;                               // slot 37
    virtual void save(int outfile) override;                                 // slot 40
    virtual void transform(RGE_Master_Static_Object* obj) override;          // slot 51
    virtual void set_being_worked_on(RGE_Action_Object* work_obj, __int16 action_type,
                                     unsigned __int8 attack_flag) override;  // slot 79
    virtual void release_being_worked_on(RGE_Static_Object* caller) override;  // slot 80
    virtual double getSpeed() override;                                      // slot 96
    virtual int setup_read(int infile, RGE_Game_World* gworld) override;     // slot 167

    // ---- the NEW appended virtual (the slot 219; the hide-and-append - see
    // ---- the header comment; deliberately NO override keyword)
    virtual int setup(RGE_Master_Animated_Object* tobj, RGE_Player* obj_owner,
                      float x, float y, float z);
};

// ---- the derived-layout gate ----
static_assert(sizeof(RGE_Animated_Object) == 184, "RGE_Animated_Object: the size drifted from the binary (0xB8)");
static_assert(offsetof(RGE_Animated_Object, speed) == 0xB4, "RGE_Animated_Object: the speed extension drifted (the binary: +0xB4 = 180)");
