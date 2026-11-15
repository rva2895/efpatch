#include "stdafx.h"

// .rdata vftable of RGE_Moving_Object (lst :990431)
static RGE_Moving_ObjectVtbl* const rge_moving_object_vftable =
    (RGE_Moving_ObjectVtbl*)0x0065BE14;

// Replacement for RGE_Moving_Object::RGE_Moving_Object (runtime ctor).
// Original: @ 0x004A0540 (idb_functions.h:2122). Range [0x4A0540, 0x4A0691)
// (lst :273287/:273391, retn 18h = 6 stack args), returns this. ABI:
// __thiscall - this_ in ECX. Callers: RGE_Action_Object__RGE_Action_Object
// +3D (the runtime ladder) and RGE_Master_Moving_Object__make_new_obj (both
// owned, hook-routed; lst :273288/:273289).
// Decode (asm 0x4A0540-0x4A0691): base runtime Animated ctor with
// do_setup = 0 (hooked -> routes), then the MOVING extension init block in
// asm order 0x4A0586-0x4A064E (all fields census-cited idb_types.h
// :6747-6775): trail_remainder @0xB8, velocity_x/y/z @0xBC/C0/C4, angle
// @0xC8, turn_towards_time @0xCC, on_ground @0xD0 (BYTE), turnTimer @0xD4,
// continueCounter @0xD8, currentTerrainException1/2 @0xDC/E0 = -1,
// waitingToMoveValue @0xE4 (BYTE), numberWaitDelays @0xE5 (BYTE),
// userDefinedWaypoints @0xE8, positionValue @0xF4 = {0,0,0} (BVector =
// {float x,y,z}, idb_types.h:1761-1765), orientationForwardValue @0x100 =
// {1.0,0,0}, orientationRightValue @0x10C = {0,0,-1.0}, pathData @0x118,
// mFuturePathData @0x11C, movementData @0x120, lastMoveTimeValue @0x124,
// mHasSubstitutePosition @0x128, mSubstitutePosition @0x12C = {0,0,0},
// mConsecutiveSubstituteCount @0x138. The vfptr store comes AFTER the field
// block (asm 0x4A0654). numberUserDefinedWaypointsValue @0xEC and maximum-
// NumberUserDefinedWaypointsValue @0xF0 are NOT stored by the original
// (asm leaves them - faithful). Then initMovementData (:2208), and IF
// do_setup the derived Moving setup (:2127, same-class call).
RGE_Moving_Object* __fastcall RGE_Moving_Object__RGE_Moving_Object_new(
    RGE_Moving_Object* this_, DWORD dummy,
    RGE_Master_Moving_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Moving_Object* result = this_;
    __try
    {
        RGE_Animated_Object__RGE_Animated_Object(
            (RGE_Animated_Object*)this_, (RGE_Master_Animated_Object*)tobj, obj_owner, x, y, z, 0);
        this_->trail_remainder = 0.0f;                   // asm 0x4A0586 (@0xB8)
        this_->velocity_x = 0.0f;                        // asm 0x4A058C
        this_->velocity_y = 0.0f;                        // asm 0x4A0592
        this_->velocity_z = 0.0f;                        // asm 0x4A0598
        this_->angle = 0.0f;                             // asm 0x4A059E
        this_->turn_towards_time = 0.0f;                 // asm 0x4A05A4
        this_->on_ground = 0;                            // asm 0x4A05AA (BYTE store)
        this_->turnTimer = 0.0f;                         // asm 0x4A05B0
        this_->continueCounter = 0;                      // asm 0x4A05B6
        this_->currentTerrainException1 = -1;            // asm 0x4A05BC
        this_->currentTerrainException2 = -1;            // asm 0x4A05C2
        this_->waitingToMoveValue = 0;                   // asm 0x4A05C8 (BYTE store)
        this_->numberWaitDelays = 0;                     // asm 0x4A05CE (BYTE store)
        this_->userDefinedWaypoints = 0;                 // asm 0x4A05D4
        this_->positionValue.x = 0.0f;                   // asm 0x4A05DA
        this_->positionValue.y = 0.0f;                   // asm 0x4A05E0
        this_->positionValue.z = 0.0f;                   // asm 0x4A05E6
        this_->orientationForwardValue.x = 1.0f;         // asm 0x4A05EC
        this_->orientationForwardValue.y = 0.0f;         // asm 0x4A05F6
        this_->orientationForwardValue.z = 0.0f;         // asm 0x4A05FC
        this_->orientationRightValue.x = 0.0f;           // asm 0x4A0602
        this_->orientationRightValue.y = 0.0f;           // asm 0x4A0608
        this_->orientationRightValue.z = -1.0f;          // asm 0x4A060E
        this_->pathData = 0;                             // asm 0x4A0618
        this_->mFuturePathData = 0;                      // asm 0x4A061E
        this_->movementData = 0;                         // asm 0x4A0624
        this_->lastMoveTimeValue = 0;                    // asm 0x4A062A
        this_->mHasSubstitutePosition = 0;               // asm 0x4A0630
        this_->mSubstitutePosition.x = 0.0f;             // asm 0x4A0636
        this_->mSubstitutePosition.y = 0.0f;             // asm 0x4A063C
        this_->mSubstitutePosition.z = 0.0f;             // asm 0x4A0642
        this_->mConsecutiveSubstituteCount = 0;          // asm 0x4A064E
        this_->vfptr = rge_moving_object_vftable;        // asm 0x4A0654 (after the field block)
        RGE_Moving_Object__initMovementData(this_);      // asm 0x4A065A (:2208)
        if (do_setup != 0)
            RGE_Moving_Object__setup(this_, tobj, obj_owner, x, y, z);   // asm 0x4A0678 (:2127)
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6426F0
    {
    }
    return result;
}

// Replacement for RGE_Moving_Object::vector_deleting_destructor.
// Original: @ 0x004A06A0 (idb_functions.h:2123). Range [0x4A06A0, 0x4A06BB)
// (lst :273400/:273418, retn 4), void* result. ABI: __thiscall. Virtual:
// vftable DATA XREF (lst :273401). Body: the Moving destructor (IN BATCH -
// the direct call routes into RGE_Moving_Object__destructor_new per the
// hook-routing ruling), then flags & 1 -> operator_delete, return this
// (asm 0x4A06A0-0x4A06BB; operator_delete = the wrapper's target per the
// Static scalar_deleting precedent).
void* __fastcall RGE_Moving_Object__vector_deleting_destructor_new(
    RGE_Moving_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Moving_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Moving_Object::RGE_Moving_Object_read (runtime
// ctor_read). Original: @ 0x004A06C0 (idb_functions.h:2124). Range
// [0x4A06C0, 0x4A07F1) (lst :273427/:273515, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: RGE_Action_Object__
// RGE_Action_Object_read +2B (the runtime ladder) and the RGE_Object_List__
// load paths (lst :273428/:273429).
// Decode (asm 0x4A06C0-0x4A07F1): base runtime Animated ctor_read with
// do_setup = 0 (idb constant :546 - NOT hooked, executes the original),
// then the SAME extension init block (asm 0x4A06F5-0x4A07BD, identical
// store set), vfptr, initMovementData, and IF do_setup the derived Moving
// setup_read (:2128, same-class call).
RGE_Moving_Object* __fastcall RGE_Moving_Object__RGE_Moving_Object_read_new(
    RGE_Moving_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    RGE_Moving_Object* result = this_;
    __try
    {
        RGE_Animated_Object__RGE_Animated_Object_read((RGE_Animated_Object*)this_, infile, gworld, 0);
        this_->trail_remainder = 0.0f;                   // asm 0x4A06F5
        this_->velocity_x = 0.0f;                        // asm 0x4A06FB
        this_->velocity_y = 0.0f;                        // asm 0x4A0701
        this_->velocity_z = 0.0f;                        // asm 0x4A0707
        this_->angle = 0.0f;                             // asm 0x4A070D
        this_->turn_towards_time = 0.0f;                 // asm 0x4A0713
        this_->on_ground = 0;                            // asm 0x4A0719 (BYTE store)
        this_->turnTimer = 0.0f;                         // asm 0x4A071F
        this_->continueCounter = 0;                      // asm 0x4A0725
        this_->currentTerrainException1 = -1;            // asm 0x4A072B
        this_->currentTerrainException2 = -1;            // asm 0x4A0731
        this_->waitingToMoveValue = 0;                   // asm 0x4A0737 (BYTE store)
        this_->numberWaitDelays = 0;                     // asm 0x4A073D (BYTE store)
        this_->userDefinedWaypoints = 0;                 // asm 0x4A0743
        this_->positionValue.x = 0.0f;                   // asm 0x4A0749
        this_->positionValue.y = 0.0f;                   // asm 0x4A074F
        this_->positionValue.z = 0.0f;                   // asm 0x4A0755
        this_->orientationForwardValue.x = 1.0f;         // asm 0x4A075B
        this_->orientationForwardValue.y = 0.0f;         // asm 0x4A0765
        this_->orientationForwardValue.z = 0.0f;         // asm 0x4A076B
        this_->orientationRightValue.x = 0.0f;           // asm 0x4A0771
        this_->orientationRightValue.y = 0.0f;           // asm 0x4A0777
        this_->orientationRightValue.z = -1.0f;          // asm 0x4A077D
        this_->pathData = 0;                             // asm 0x4A0787
        this_->mFuturePathData = 0;                      // asm 0x4A078D
        this_->movementData = 0;                         // asm 0x4A0793
        this_->lastMoveTimeValue = 0;                    // asm 0x4A0799
        this_->mHasSubstitutePosition = 0;               // asm 0x4A079F
        this_->mSubstitutePosition.x = 0.0f;             // asm 0x4A07A5
        this_->mSubstitutePosition.y = 0.0f;             // asm 0x4A07AB
        this_->mSubstitutePosition.z = 0.0f;             // asm 0x4A07B1
        this_->mConsecutiveSubstituteCount = 0;          // asm 0x4A07BD
        this_->vfptr = rge_moving_object_vftable;        // asm 0x4A07C3 (after the field block)
        RGE_Moving_Object__initMovementData(this_);      // asm 0x4A07C9 (:2208)
        if (do_setup != 0)
            RGE_Moving_Object__setup_read(this_, infile, gworld);   // asm 0x4A07D8 (:2128)
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642710
    {
    }
    return result;
}

// Replacement for RGE_Moving_Object::destructor. Original: @ 0x004A0800
// (idb_functions.h:2125). Range [0x4A0800, 0x4A084E) (lst :273524/:273555),
// void, no stack args. ABI: __thiscall. Caller: RGE_Action_Object__
// destructor +4F (the runtime ladder, hook-routed) and the vdel above.
// Decode (asm 0x4A0800-0x4A084E): vfptr store, initMovementData (RESETS the
// movement data - NOT a free: the runtime Moving holds no heap of its own),
// then the runtime Static destructor (HOOKED -> routes). NO frees. The
// throw-during-destruction unwind (SEH chunk 0x642730) jumps the BASE
// destructor - folded per the ctor-fold precedent.
void __fastcall RGE_Moving_Object__destructor_new(RGE_Moving_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_moving_object_vftable;        // asm 0x4A081D
        RGE_Moving_Object__initMovementData(this_);      // asm 0x4A082B (:2208)
        RGE_Static_Object__destructor((RGE_Static_Object*)this_);   // asm 0x4A083A - HOOKED, routes
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642730
    {
    }
}

void install_RGE_Moving_Object()
{
    setHook((void*)0x004A0540, RGE_Moving_Object__RGE_Moving_Object_new);
    setHook((void*)0x004A06A0, RGE_Moving_Object__vector_deleting_destructor_new);
    setHook((void*)0x004A06C0, RGE_Moving_Object__RGE_Moving_Object_read_new);
    setHook((void*)0x004A0800, RGE_Moving_Object__destructor_new);
}
