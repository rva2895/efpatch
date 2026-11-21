#include "stdafx.h"
#include <intrin.h>                                                   // DIAG: _ReturnAddress (the waypoint-fetch caller filter; remove with the harness)

// .rdata vftable of RGE_Moving_Object (lst :990431)
static RGE_Moving_ObjectVtbl* const rge_moving_object_vftable =
    (RGE_Moving_ObjectVtbl*)0x0065BE14;

// casts.cpp:140 - the speed-reduction patch helper (__stdcall, raw float bits
// in eax). BAKE SOURCE for maximumSpeed_new (speed2) and, in the M3c batch,
// calculateFutureVelocity_new (speed3).
extern uint32_t __stdcall getSpeedModifier(RGE_Static_Object* unit);

// .data BVector upVector @0x006AD128 (lst :272773-region; initialized
// {0, 1.0, 0} at 0x4A00F0-0x4A0104) - the yaw rotation axis (offset +4
// = the vertical per the corrected BVector).
static BVector* const upVector = (BVector*)0x006AD128;

// .data BVector stru_6AE6A0 @0x006AE6A0 (lst :1102398-region) -
// moveUnit's fewer-than-2-waypoints fallback vector (read by address;
// the content is the game's).
static BVector* const fallbackWaypointVector = (BVector*)0x006AE6A0;

// DIAG (remove at ship): the capstone A/B harness globals - the shared log
// handle and the breadcrumb mark (written by the capstone body's regions,
// logged by the DIMU/lifecycle rows). Declared here because the breadcrumb
// sites sit inside the capstone body above the harness block.
static FILE* g_dimuLog = 0;                                               // DIAG
static int g_dimuMark = 0;                                                // DIAG

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
        this_->orientationRightValue.z = 0.0f;           // asm 0x4A0608 (offset4 = .z per the corrected BVector)
        this_->orientationRightValue.y = -1.0f;          // asm 0x4A060E (offset8 = .y)
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
        this_->orientationRightValue.z = 0.0f;           // asm 0x4A0777 (offset4 = .z per the corrected BVector)
        this_->orientationRightValue.y = -1.0f;          // asm 0x4A077D (offset8 = .y)
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

// ============ RUNTIME METHODS (batch M2) ============
// The master-side query slots (getTurnRadius/position/getPathData/
// orientationForward/zero+numberUserDefinedWaypoints/unknown/
// get+set_Selected_Groups) dispatch to these runtime bodies.

// Replacement for RGE_Moving_Object::getTurnRadius. Original:
// SEMANTIC: returns the unit's current facing ANGLE (the master slot's name notwithstanding - the runtime body reads the angle field, not a radius).
// @ 0x00408B80 (idb_functions.h). Decode (.c:51402-51407): return
// this->angle (float @+0xC8 = 200; the .c's "v1+200" with v1 = the
// __thiscall ecx this - the "possibly undefined" render is the
// classic ecx artifact; the Moving ctor's extension init (batch
// 1b-2 store map) confirms angle @+0xC8). The master vtbl slot is
// NAMED getTurnRadius but the runtime body returns the angle field.
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): the original is
// fld dword ptr [ecx+0C8h]; retn (asm 0x408B80-0x408B86) - the m32
// load is exact; the double return is bit-identical for this body.
double __fastcall RGE_Moving_Object__getAngle_new(RGE_Moving_Object* this_, DWORD dummy)
{
    return this_->angle;
}

// Replacement for RGE_Moving_Object::zeroUserDefinedWaypoints.
// Original: @ 0x00408B90 (idb_functions.h). Decode (.c:51411-51414):
// numberUserDefinedWaypointsValue @+0xEC (236) = 0 - sets the COUNT
// zero, not the waypoint array.
void __fastcall RGE_Moving_Object__zeroUserDefinedWaypoints_new(RGE_Moving_Object* this_, DWORD dummy)
{
    this_->numberUserDefinedWaypointsValue = 0;
}

// Replacement for RGE_Moving_Object::numberUserDefinedWaypoints.
// Original: @ 0x00408BA0 (idb_functions.h). Decode (.c:51417-51420):
// return numberUserDefinedWaypointsValue.
int __fastcall RGE_Moving_Object__numberUserDefinedWaypoints_new(RGE_Moving_Object* this_, DWORD dummy)
{
    return this_->numberUserDefinedWaypointsValue;
}

// Replacement for RGE_Moving_Object::position. Original:
// @ 0x00408BB0 (idb_functions.h). Decode (.c:51423-51430):
// *result = this->positionValue (the 12-byte BVector struct copy);
// return result.
BVector* __fastcall RGE_Moving_Object__position_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* result)
{
    *result = this_->positionValue;
    return result;
}

// Replacement for RGE_Moving_Object::getPathData. Original:
// @ 0x00408BE0 (idb_functions.h). Decode (.c:51433-51436): return
// this->pathData (@+0x118 = 280).
BPathData* __fastcall RGE_Moving_Object__getPathData_new(RGE_Moving_Object* this_, DWORD dummy)
{
    return this_->pathData;
}

// Replacement for RGE_Moving_Object::unknown (the IDA name; the body
// reads lastMoveTimeValue). Original: @ 0x00408BF0 (idb_functions.h).
// Decode (.c:51439-51442): return *(dword*)(this + 292) =
// lastMoveTimeValue @+0x124 (292) - the unsigned int movement
// timestamp.
int __fastcall RGE_Moving_Object__unknown_new(RGE_Moving_Object* this_, DWORD dummy)
{
    return (int)this_->lastMoveTimeValue;
}

// Replacement for RGE_Moving_Object::orientationForward. Original:
// @ 0x00408C00 (idb_functions.h). Decode (.c:51445-51452):
// *result = this->orientationForwardValue (@+0x100 = 256); return
// result.
BVector* __fastcall RGE_Moving_Object__orientationForward_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* result)
{
    *result = this_->orientationForwardValue;
    return result;
}

// ============ RUNTIME METHODS (batch M3a) ============

// Replacement for RGE_Moving_Object::new_sprite. Original:
// @ 0x004A1010 (idb_functions.h). Range [0x4A1010, 0x4A1050). Decode
// (.c:173426-173441): if (!nsprite) nsprite = master get_sprite
// (master vtbl slot +4); if (this->sprite @+0x1C != nsprite):
// old_sprite @+0x20 = the old sprite, Static new_sprite (:1178-
// region), vfptr->set_angle(this) (Moving vtbl).
void __fastcall RGE_Moving_Object__new_sprite_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Sprite* nsprite)
{
    RGE_Sprite* target = nsprite;
    if (target == 0)                                     // .c:173432
        target = this_->master_obj->vfptr->get_sprite(this_->master_obj, (RGE_Static_Object*)this_);   // .c:173433
    if (this_->sprite != target)                         // .c:173435
    {
        this_->old_sprite = this_->sprite;               // .c:173437 (@+0x20)
        RGE_Static_Object__new_sprite((RGE_Static_Object*)this_, target);   // .c:173438
        this_->vfptr->set_angle(this_);                  // .c:173439 (Moving vtbl)
    }
}

// Replacement for RGE_Moving_Object::spawn_death_obj. Original:
// @ 0x004A1050 (idb_functions.h). Range [0x4A1050, 0x4A1090),
// returns RGE_Static_Object*. Decode (.c:173444-173457): Static
// spawn_death_obj; IF the spawned object && its master_type >= 30
// (0x1E - the MOVING tier): spawned->vfptr->new_angle(spawned,
// this->angle) (Static vtbl member, asm-confirmed .c render) +
// Static set_facet (:1166-region) with this->facet.
RGE_Static_Object* __fastcall RGE_Moving_Object__spawn_death_obj_new(
    RGE_Moving_Object* this_, DWORD dummy)
{
    RGE_Static_Object* spawned = RGE_Static_Object__spawn_death_obj((RGE_Static_Object*)this_);   // .c:173449
    if (spawned != 0 && spawned->master_obj->master_type >= 30)   // .c:173451 (0x1E)
    {
        spawned->vfptr->new_angle(spawned, this_->angle);   // .c:173453 (Static vtbl new_angle)
        RGE_Static_Object__set_facet(spawned, this_->facet);   // .c:173454
    }
    return spawned;
}

// Replacement for RGE_Moving_Object::transform. Original:
// @ 0x004A1090. Range [0x4A1090, 0x4A10D0). Decode (.c:173460-
// 173472): Animated transform (HOOKED -> routes) + velocity_x/y/z
// = 0 + turnTimer = 0 + turn_towards_time = 0 + vfptr->set_angle.
void __fastcall RGE_Moving_Object__transform_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Master_Static_Object* obj)
{
    RGE_Animated_Object__transform((RGE_Animated_Object*)this_, obj);   // .c:173464 (HOOKED, routes)
    this_->velocity_x = 0.0f;                        // .c:173466 (@+0xBC)
    this_->velocity_y = 0.0f;                        // .c:173467 (@+0xC0)
    this_->velocity_z = 0.0f;                        // .c:173468 (@+0xC4)
    this_->turnTimer = 0.0f;                         // .c:173469 (@+0xD4)
    this_->turn_towards_time = 0.0f;                 // .c:173470 (@+0xCC)
    this_->vfptr->set_angle(this_);                  // .c:173471 (Moving vtbl)
}

// Replacement for RGE_Moving_Object::is_moving. Original:
// @ 0x004A10D0. Range [0x4A10D0, 0x4A10F0), u8 return. Decode
// (.c:173475-173478): return speed @+0xB4 > 0.0.
unsigned __int8 __fastcall RGE_Moving_Object__is_moving_new(RGE_Moving_Object* this_, DWORD dummy)
{
    return this_->speed > 0.0f;
}

// Replacement for RGE_Moving_Object::stop_moving. Original:
// @ 0x004A10F0. Range [0x4A10F0, 0x4A1100). Decode (.c:173481-
// 173484): speed @+0xB4 = 0.
void __fastcall RGE_Moving_Object__stop_moving_new(RGE_Moving_Object* this_, DWORD dummy)
{
    this_->speed = 0.0f;
}

// Replacement for RGE_Moving_Object::set_velocity. Original:
// @ 0x004A19B0. Range [0x4A19B0, 0x4A19E0). Decode (.c:173943-
// 173948): velocity_x/y/z @+0xBC/0xC0/0xC4 = the params.
void __fastcall RGE_Moving_Object__set_velocity_new(
    RGE_Moving_Object* this_, DWORD dummy, float x, float y, float z)
{
    this_->velocity_x = x;                           // .c:173945 (@+0xBC)
    this_->velocity_y = y;                           // .c:173946 (@+0xC0)
    this_->velocity_z = z;                           // .c:173947 (@+0xC4)
}

// Replacement for RGE_Moving_Object::new_angle. Original:
// @ 0x004A19E0. Range [0x4A19E0, 0x4A1A00). Decode (.c:173951-
// 173958): angle @+0xC8 = new_angle, then vfptr->set_angle(this)
// (Moving vtbl).
void __fastcall RGE_Moving_Object__new_angle_new(
    RGE_Moving_Object* this_, DWORD dummy, float new_angle)
{
    this_->angle = new_angle;                        // .c:173956 (@+0xC8)
    this_->vfptr->set_angle(this_);                  // .c:173957 (Moving vtbl)
}

// Replacement for RGE_Moving_Object::maximumSpeed. Original:
// @ 0x004A28C0 (idb_functions.h; Attributes: thunk per the .c - no SEH).
// Range [0x4A28C0, 0x4A28F0), returns double. ABI: __thiscall, no stack args.
// Base decode (.c:174638-174649): IF sprite @+0x1C && sprite->base_speed
// @+0x74 > 0: master_obj->speed @+0xB4 * base_speed; ELSE master_obj->speed.
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): the original exits are
// fld [eax+0D0h]; fmul [edx+74h]; retn (asm 0x4A28DA-0x4A28E3, the product
// stays UNROUNDED 53-bit on the x87 stack) and fld [ecx+0D0h]; retn (asm
// 0x4A28E7-0x4A28ED, exact m32 load) - no m32 store before consumption;
// the former (float) casts were the st0-boundary truncation, removed.
// BAKED (SPEED_MOD): casts.cpp:881 setHook(0x004A28C0, speed2) - the ENTRY of
// this function, silently reverted by install order since the M3a write
// (setCastHooks dllmain:451 < install_engine dllmain:765). speed2 multiplies
// getSpeedModifier(this) into master->speed BEFORE the base_speed multiply
// (left-assoc = the stub's fmul order, x87 PC53 = double rounding per op);
// modifier fetched unconditionally before the gate; the gate is the base
// decode's (the stub's double-zero fcomp is result-identical to a float
// compare under all inputs). Behavior-neutral for units without
// speedReductionEnabled (speed*1.0f is exact), patch-faithful for reduced
// units. Call site retired in casts.cpp (:881, commented in place).
double __fastcall RGE_Moving_Object__maximumSpeed_new(RGE_Moving_Object* this_, DWORD dummy)
{
    uint32_t modifier_bits = getSpeedModifier((RGE_Static_Object*)this_);   // baked SPEED_MOD: casts.cpp:882-region stub (unconditional fetch before the gate)
    float modifier;
    memcpy(&modifier, &modifier_bits, sizeof(modifier));   // the helper returns raw float bits (casts.cpp:140 "returns 32 bit float in eax") - 4-byte value copy, aliasing-clean
    if (this_->sprite != 0 && this_->sprite->base_speed > 0.0)
        return (double)this_->master_obj->speed * (double)modifier * (double)this_->sprite->base_speed;   // asm stub: fmul x2 at 53-bit, left-assoc - st0 unrounded at retn (0x4A28DA-0x4A28E3 form)
    return (double)this_->master_obj->speed * (double)modifier;       // asm stub loc_4A28E4
}

// Replacement for RGE_Moving_Object::waitingToMove. Original:
// @ 0x004A28F0. Range [0x4A28F0, 0x4A2920), u8 return. Decode
// (.c:174652-174663): v = vfptr->actionState(this) (Moving vtbl);
// v == 6 || v == 7 -> 0; else waitingToMoveValue @+0xEC.
unsigned __int8 __fastcall RGE_Moving_Object__waitingToMove_new(RGE_Moving_Object* this_, DWORD dummy)
{
    int state = this_->vfptr->actionState(this_);    // .c:174657 (Moving vtbl)
    if (state == 6 || state == 7)
        return 0;
    return this_->waitingToMoveValue;                // @+0xEC
}

// Replacement for RGE_Moving_Object::setWaitingToMove. Original:
// @ 0x004A2920. Range [0x4A2920, 0x4A2930). Decode (.c:174666-
// 174669): waitingToMoveValue @+0xEC = w.
void __fastcall RGE_Moving_Object__setWaitingToMove_new(
    RGE_Moving_Object* this_, DWORD dummy, unsigned __int8 w)
{
    this_->waitingToMoveValue = w;
}

// Replacement for RGE_Moving_Object::userDefinedWaypoint. Original:
// @ 0x004A2930. Range [0x4A2930, 0x4A2960), returns BVector*.
// Decode (.c:174672-174681): v < 0 || v >= count @+0xEC -> 0;
// else &userDefinedWaypoints[v] (@+0x1E0... the BVector* array
// @+0xE8? ? userDefinedWaypoints @232 = 0xE8 per the Moving walk).
BVector* __fastcall RGE_Moving_Object__userDefinedWaypoint_new(
    RGE_Moving_Object* this_, DWORD dummy, int v)
{
    if (v < 0 || v >= this_->numberUserDefinedWaypointsValue)
        return 0;
    return &this_->userDefinedWaypoints[v];
}

// Replacement for RGE_Moving_Object::addUserDefinedWaypoint.
// Original: @ 0x004A2960. Range [0x4A2960, 0x4A2AB0), returns int.
// Decode (.c:174684-174717): the resize-or-fail gate (count <
// maximum, else the resize(5)/resize(2*maximum) cascade per the
// .c conditional), then the waypoint store with the x/z SWAP
// (.y = z param, .z = y param), the floor-centering block
// (floor + fabs < 1e-7 -> field = floor + 0.5), count++ -> 1.
int __fastcall RGE_Moving_Object__addUserDefinedWaypoint_new(
    RGE_Moving_Object* this_, DWORD dummy, float x, float y, float z)
{
    int result = 0;
    if (this_->numberUserDefinedWaypointsValue < this_->maximumNumberUserDefinedWaypointsValue
        || ((this_->maximumNumberUserDefinedWaypointsValue != 0
            || RGE_Moving_Object__resizeUserDefinedWaypoints(this_, 5) != 0)
        && RGE_Moving_Object__resizeUserDefinedWaypoints(this_,
            2 * this_->maximumNumberUserDefinedWaypointsValue) != 0))
    {
        BVector* wp = &this_->userDefinedWaypoints[this_->numberUserDefinedWaypointsValue];
        wp->x = x;                                   // new .c:174684
        wp->z = z;                                   // new .c:174685 (identity in the corrected BVector names)
        wp->y = y;                                   // new .c:174686
        double fx = floor((double)wp->x);            // new .c:174687-174689
        double fy = floor((double)wp->y);            // new .c:174690-174691
        if (fabs((double)wp->x - fx) < 0.0000001000000011686097)   // new .c:174693
            wp->x = (float)(fx + 0.5);               // new .c:174694
        if (fabs((double)wp->y - fy) < 0.0000001000000011686097)   // new .c:174696
            wp->y = (float)(fy + 0.5);               // new .c:174697
        ++this_->numberUserDefinedWaypointsValue;    // .c:174713
        result = 1;
    }
    return result;
}

// Replacement for RGE_Moving_Object::removeAllUserDefinedWaypoints.
// Original: @ 0x004A2AB0. Range [0x4A2AB0, 0x4A2AE0). Decode
// (.c:174720-174729): IF userDefinedWaypoints: operator_delete
// (CRT rule: operator_delete_internal) + = 0; count @+0xEC = 0;
// maximum @+0xF0 = 0.
void __fastcall RGE_Moving_Object__removeAllUserDefinedWaypoints_new(
    RGE_Moving_Object* this_, DWORD dummy)
{
    if (this_->userDefinedWaypoints != 0)                // .c:174722
    {
        operator_delete_internal(this_->userDefinedWaypoints);   // .c:174724 (CRT rule)
        this_->userDefinedWaypoints = 0;                 // .c:174725
    }
    this_->numberUserDefinedWaypointsValue = 0;          // .c:174727
    this_->maximumNumberUserDefinedWaypointsValue = 0;   // .c:174728
}

// Replacement for RGE_Moving_Object::resizeUserDefinedWaypoints.
// Original: @ 0x004A2AE0. Range [0x4A2AE0, 0x4A2BB0), returns int.
// Decode (.c:174732-174793): newSize <= 0 -> 0; maximum >= newSize
// && maximum > 0 -> 0; operator_new(12 * newSize) (12 = sizeof
// (BVector) ?) + the zero-fill loop (12 bytes per element, asm
// 174757-174767); IF old array: the count-capped copy loop
// (x/y/z dwords), operator_delete of the old (CRT rule); swap;
// maximum = newSize; return 1.
int __fastcall RGE_Moving_Object__resizeUserDefinedWaypoints_new(
    RGE_Moving_Object* this_, DWORD dummy, int newSize)
{
    if (newSize <= 0)                                // .c:174746
        return 0;
    if (this_->maximumNumberUserDefinedWaypointsValue >= newSize
        && this_->maximumNumberUserDefinedWaypointsValue > 0)   // .c:174748-174750
        return 0;
    BVector* fresh = (BVector*)operator_new_internal(sizeof(BVector) * newSize);   // .c:174751 (12 = sizeof(BVector))
    if (fresh == 0)                                  // .c:174753
        return 0;
    for (int i = 0; i < newSize; ++i)                // .c:174755-174768 (the zero-fill)
    {
        fresh[i].x = 0.0f;
        fresh[i].y = 0.0f;
        fresh[i].z = 0.0f;
    }
    if (this_->userDefinedWaypoints != 0)            // .c:174769
    {
        int copied = 0;
        if (this_->numberUserDefinedWaypointsValue > 0)   // .c:174772
        {
            for (int i = 0; i < this_->numberUserDefinedWaypointsValue; ++i)   // .c:174775-174785
            {
                fresh[i].x = this_->userDefinedWaypoints[i].x;   // .c:174781
                fresh[i].y = this_->userDefinedWaypoints[i].y;   // .c:174782
                fresh[i].z = this_->userDefinedWaypoints[i].z;   // .c:174783
            }
        }
        operator_delete_internal(this_->userDefinedWaypoints);   // .c:174788 (CRT rule)
    }
    this_->userDefinedWaypoints = fresh;             // .c:174790
    this_->maximumNumberUserDefinedWaypointsValue = newSize;   // .c:174791
    return 1;
}

// ============ RUNTIME METHODS (batch M3b) ============
// BMovementData lazy-init setters (setVelocity/setAcceleration/
// setForwardVelocity/setForwardAcceleration) are DEFERRED to M3c:
// their operator_new(0x18) allocation requires the BMovementData
// ctor body (no idb export; lst-resolve + struct walk owed).

// Replacement for RGE_Moving_Object::stopAllMovement. Original:
// @ 0x004A3770. Range [0x4A3770, 0x4A3790). Decode (.c:175336-
// 175344): vfptr->stopMoving(this) (Moving vtbl) + IF pathData
// @+0x118: BPathData::reset (:2507).
void __fastcall RGE_Moving_Object__stopAllMovement_new(RGE_Moving_Object* this_, DWORD dummy)
{
    this_->vfptr->stopMoving(this_);                 // .c:175340
    if (this_->pathData != 0)                        // .c:175342
        BPathData__reset(this_->pathData);           // .c:175343 (:2507)
}

// Replacement for RGE_Moving_Object::velocity. Original:
// @ 0x004A3830. Range [0x4A3830, 0x4A3870), returns BVector*.
// Decode (.c:175367-175385): IF movementData @+0x120: *result =
// movementData->velocity; ELSE zero result; return result.
BVector* __fastcall RGE_Moving_Object__velocity_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* result)
{
    if (this_->movementData != 0)                    // .c:175374
        *result = this_->movementData->velocity;     // .c:175376
    else
    {
        result->x = 0.0f;                            // .c:175380
        result->y = 0.0f;                            // .c:175381
        result->z = 0.0f;                            // .c:175382
    }
    return result;
}

// Replacement for RGE_Moving_Object::acceleration. Original:
// @ 0x004A3920. Range [0x4A3920, 0x4A3960), returns BVector*.
// Decode (.c:175403-175424): same shape as velocity with
// movementData->acceleration.
BVector* __fastcall RGE_Moving_Object__acceleration_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* result)
{
    if (this_->movementData != 0)                    // .c:175410
        *result = this_->movementData->acceleration;   // .c:175414
    else
    {
        result->x = 0.0f;
        result->y = 0.0f;
        result->z = 0.0f;
    }
    return result;
}

// Replacement for RGE_Moving_Object::moveForward. Original:
// @ 0x004A3B70. Range [0x4A3B70, 0x4A3BFA), retn 4. Decode (asm
// 0x4A3B70-0x4A3BFA): IF fabs(v) >= 1e-7: positionValue snapshot +
// BVector__operator_mul (:5789: result = v * orientationForward-
// Value - the scalar-vector scale) + positionValue += the scaled
// vector + vfptr->teleport(this, x, y, z) - the identity order in the
// corrected BVector names (the pre-regeneration "(x, Z, Y) swapped"
// reading was the misnamed-struct artifact; the pushes at 0x4A3BDD/
// 0x4A3BEA/0x4A3BEB carry offsets 4/8/0 -> params (x, y, z); the
// teleport body stores param1->world_x @0x48, param2->world_y @0x4C,
// param3->world_z @0x50, 0x54F983-0x54F99A).
void __fastcall RGE_Moving_Object__moveForward_new(
    RGE_Moving_Object* this_, DWORD dummy, float v)
{
    if (fabs((double)v) >= 0.0000001)                // .c:175506
    {
        BVector pos = this_->positionValue;          // .c:175508
        BVector scaled;
        // The scale = BVector__operator_mul's scalar overload (0x606970,
        // lst :868403-:868426 - element-wise a * v; the original calls it
        // with ECX = &result, EDX = &orientationForwardValue, stack = a -
        // a register/stack assignment the idb __fastcall constant cannot
        // reproduce from C; the inlined element-wise scale is the same
        // arithmetic, grounded by the operator_mul body).
        scaled.x = v * this_->orientationForwardValue.x;   // .c:175509
        scaled.y = v * this_->orientationForwardValue.y;
        scaled.z = v * this_->orientationForwardValue.z;
        pos.x = pos.x + scaled.x;                    // .c:175510
        pos.y = pos.y + scaled.y;                    // .c:175511
        pos.z = pos.z + scaled.z;                    // .c:175512
        this_->vfptr->teleport(this_, pos.x, pos.y, pos.z);   // asm 0x4A3BDD-0x4A3BEE: pushes offset4/offset8/offset0 -> params (x, y, z)
    }
}

// Replacement for RGE_Moving_Object::setPosition. Original:
// @ 0x004A3C00. Range [0x4A3C00, 0x4A3C35), retn 4. Decode (asm
// 0x4A3C00-0x4A3C35): positionValue @+0xF4 = *pos + vfptr->teleport(
// this, x, y, z) - the identity order in the corrected BVector names
// (pushes 0x4A3C23/0x4A3C29/0x4A3C2A carry offsets 4/8/0 -> params
// (x, y, z)).
void __fastcall RGE_Moving_Object__setPosition_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* pos)
{
    this_->positionValue = *pos;                     // asm 0x4A3C12-0x4A3C1B
    this_->vfptr->teleport(this_, pos->x, pos->y, pos->z);   // asm 0x4A3C23-0x4A3C2B: pushes offset4/offset8/offset0 -> params (x, y, z)
}

// Replacement for RGE_Moving_Object::pauseMovement. Original:
// @ 0x004A3F10. Range [0x4A3F10, 0x4A3F40). Decode (.c:175630-
// 175637): IF pathData @+0x118: pathData->pauseTime = v (the
// BPathData member).
void __fastcall RGE_Moving_Object__pauseMovement_new(
    RGE_Moving_Object* this_, DWORD dummy, float v)
{
    if (this_->pathData != 0)                        // .c:175635
        this_->pathData->pauseTime = v;              // .c:175636
}

// ===========================================================================
// BATCH M3c - Moving pathing/state methods (11 hooks). BVector note: the
// corrected field order is {x, z, y} (offsets 0/4/8) - all member accesses
// below use the corrected names; the "y/z swaps" in the pre-regeneration
// .c were the naming artifact (DECOMP_STATE.md, BVECTOR FIELD-NAME
// CORRECTION). Float-precision note: arithmetic feeding comparisons uses
// double intermediates; direct loaded-float compares stay float
// (DECOMP_TEMPLATE.md, FLOAT PRECISION VERIFICATION).
// ===========================================================================

// Replacement for RGE_Moving_Object::getPatrolInformation. Original:
// @ 0x004A2BB0. Range [0x4A2BB0, 0x4A2C81) (retn 4), returns BPath*.
// Vftable-slotted (DATA XREF :277434-region). SEH chunk 0x6427C0 = the
// allocation-unwind fold (funclet operator_delete's the allocation;
// written per the Missile setup_read precedent, 0-init so the
// pre-allocation window is a null-free no-op - judgment call J1).
// Decode (asm 0x4A2BB0-0x4A2C81): *currentWaypoint = 0; pathData @+0x118
// null -> 0; source = highestLinkedPathPointer (:2509) else
// (*base_game)->world (+0x420) + highestLinkedPath (:2508) -> path (:6094)
// -> null -> 0; operator_new(0x34) = sizeof(BPath) (52, walk :921-936);
// guarded BPath ctor (:2520, args -1); the result is the ctor's return =
// the allocation (asm 0x4A2C37 mov edi, eax); copyGameData (:2531); the
// linkedPath (+4) walk to the deepest node; *currentWaypoint = waypoint
// (@+0x18); return the new BPath.
BPath* __fastcall RGE_Moving_Object__getPatrolInformation_new(
    RGE_Moving_Object* this_, DWORD dummy, int* currentWaypoint)
{
    BPath* newPath = 0;                                              // judgment call J1 (0-init)
    BPath* result;
    __try
    {
        *currentWaypoint = 0;                                        // asm 0x4A2BD1
        BPathData* pd = this_->pathData;                             // asm 0x4A2BD4 (@+0x118)
        if (pd == 0)
            return 0;                                                // asm 0x4A2BDC-0x4A2BE0
        BPath* source = BPathData__highestLinkedPathPointer(pd);     // asm 0x4A2BE5 (:2509)
        if (source == 0)
        {
            int pathID = BPathData__highestLinkedPath(pd);           // asm 0x4A2C01 (:2508)
            source = RGE_Game_World__path(
                (RGE_Game_World*)(*base_game)->world, pathID);       // asm 0x4A2BF0-0x4A2C09 (:6094; world @+0x420)
            if (source == 0)
                return 0;                                            // asm 0x4A2C10-0x4A2C16
        }
        newPath = (BPath*)operator_new_internal(sizeof(BPath));      // asm 0x4A2C18 (0x34 = sizeof walk)
        if (newPath != 0)
            BPath__BPath(newPath, -1);                               // asm 0x4A2C2E-0x4A2C32 (:2520)
        if (newPath == 0)
            return 0;                                                // asm 0x4A2C39-0x4A2C45
        BPath__copyGameData(source, newPath);                        // asm 0x4A2C49-0x4A2C4C (:2531)
        BPathData* deepest = this_->pathData;                        // asm 0x4A2C51
        while (deepest->linkedPath != 0)                             // asm 0x4A2C57-0x4A2C65 (@+4)
            deepest = deepest->linkedPath;
        *currentWaypoint = deepest->waypoint;                        // asm 0x4A2C67-0x4A2C6C (@+0x18)
        result = newPath;                                            // asm 0x4A2C6A
    }
    __except (operator_delete_internal(newPath), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6427C0
    {
    }
    return result;
}

// Replacement for RGE_Moving_Object::setOrientation. Original:
// @ 0x004A3790 (idb_functions.h:2173). Range [0x4A3790, 0x4A3822)
// (retn 8), void. Vftable slot 180 (+0x2D0).
// Decode (asm 0x4A3790-0x4A3822): base Static setOrientation (:3895,
// unhooked - executes the original); orientationForwardValue @+0x100 =
// *newForward (3 dword copies, asm 0x4A37A9-0x4A37C8);
// orientationRightValue @+0x10C = BVector::operator_mul(newUp,
// newForward) (:5789 - ecx = newUp, asm 0x4A37CB-0x4A37E2); normalize
// orientationRightValue (x87 53-bit: sqrt over the three squares, three
// divides - asm 0x4A37E5-0x4A3818; the divisions consume the unrounded
// 53-bit len - double form per the FLOAT PRECISION rule (a)/(c)).
void __fastcall RGE_Moving_Object__setOrientation_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* newForward, BVector* newUp)
{
    RGE_Static_Object__setOrientation(
        (RGE_Static_Object*)this_, newForward, newUp);               // asm 0x4A37A2 (:3895)
    this_->orientationForwardValue = *newForward;                    // asm 0x4A37A9-0x4A37C8 (@+0x100)
    BVector right;
    this_->orientationRightValue =
        *BVector__operator_mul(newUp, &right, newForward);           // asm 0x4A37CB-0x4A37CD (:5789; ecx = newUp)
    double len = sqrt((double)right.x * (double)right.x
        + (double)right.y * (double)right.y
        + (double)right.z * (double)right.z);                        // asm 0x4A37E5-0x4A3803
    right.x = (float)((double)right.x / len);                        // asm 0x4A3805-0x4A3807
    right.y = (float)((double)right.y / len);                        // asm 0x4A380B-0x4A380E
    right.z = (float)((double)right.z / len);                        // asm 0x4A3813-0x4A3816
    this_->orientationRightValue = right;
}

// Replacement for RGE_Moving_Object::yaw. Original: @ 0x004A3C40
// (idb_functions.h:2184). Range [0x4A3C40, 0x4A3D81) (retn 4), void.
// Vftable slot 191 (+0x2FC).
// Decode (asm 0x4A3C40-0x4A3D81): m = BMatrix::makeRotateArbitrary(v,
// &upVector) (:2052, asm 0x4A3C4C-0x4A3C56); orientationForwardValue
// @+0x100 = m * forward (BMatrix__operator_mul :2053, asm
// 0x4A3C5B-0x4A3C6B); orientationRightValue @+0x10C = m * right (asm
// 0x4A3C7F-0x4A3C92); normalize BOTH (two full-3D x87 blocks, asm
// 0x4A3CA9-0x4A3CDA forward, 0x4A3CE1-0x4A3D12 right); vfptr->
// setOrientation(this, &forward, &upVector) (slot +0x2D0, asm 0x4A3D23);
// facet/angle: n = sprite ? max(sprite->facet_num @+0x70, 8) : 8 (asm
// 0x4A3D29-0x4A3D3D); if facet @+0x45 < n: angle @+0xC8 = const_f_2pi
// (6.2831855f, lst :989982) / n * (facet + 1) (fild/fdivr/fimul, asm
// 0x4A3D62-0x4A3D77; the fdivr divides the 53-bit way - double form
// per the FLOAT PRECISION rule (a)/(f)) else angle = 0 (asm 0x4A3D50).
void __fastcall RGE_Moving_Object__yaw_new(RGE_Moving_Object* this_, DWORD dummy, float v)
{
    BMatrix m;
    BMatrix__makeRotateArbitrary(&m, v, upVector);                   // asm 0x4A3C4C-0x4A3C56 (:2052)
    BVector f;
    this_->orientationForwardValue =
        *BMatrix__operator_mul(&m, &f, &this_->orientationForwardValue);   // asm 0x4A3C5B-0x4A3C6B (:2053)
    BVector r;
    this_->orientationRightValue =
        *BMatrix__operator_mul(&m, &r, &this_->orientationRightValue);     // asm 0x4A3C7F-0x4A3C92 (:2053)
    double fl = sqrt((double)f.x * (double)f.x
        + (double)f.y * (double)f.y + (double)f.z * (double)f.z);    // asm 0x4A3CA9-0x4A3CC7
    f.x = (float)((double)f.x / fl);                                 // asm 0x4A3CC9-0x4A3CCD
    f.y = (float)((double)f.y / fl);                                 // asm 0x4A3CCF-0x4A3CD2
    f.z = (float)((double)f.z / fl);                                 // asm 0x4A3CD7-0x4A3CDA
    this_->orientationForwardValue = f;
    double rl = sqrt((double)r.x * (double)r.x
        + (double)r.y * (double)r.y + (double)r.z * (double)r.z);    // asm 0x4A3CE1-0x4A3CFF
    r.x = (float)((double)r.x / rl);                                 // asm 0x4A3D01-0x4A3D03
    r.y = (float)((double)r.y / rl);                                 // asm 0x4A3D07-0x4A3D0A
    r.z = (float)((double)r.z / rl);                                 // asm 0x4A3D0F-0x4A3D12
    this_->orientationRightValue = r;
    this_->vfptr->setOrientation(this_, &this_->orientationForwardValue, upVector);   // asm 0x4A3D14-0x4A3D23 (slot +0x2D0)
    int n = 8;                                                       // asm 0x4A3D3D
    if (this_->sprite != 0 && this_->sprite->facet_num > 8)          // asm 0x4A3D29-0x4A3D3B (@+0x70)
        n = this_->sprite->facet_num;
    if (this_->facet < n)                                            // asm 0x4A3D48-0x4A3D4D (facet byte @+0x45)
        this_->angle = (float)((double)6.2831855f / (double)n * (double)(this_->facet + 1));   // asm 0x4A3D62-0x4A3D77 (const_f_2pi; fild/fdivr/fimul)
    else
        this_->angle = 0.0f;                                         // asm 0x4A3D50
}

// Replacement for RGE_Moving_Object::stopMoving. Original: @ 0x004A3DD0
// (idb_functions.h:2186). Range [0x4A3DD0, 0x4A3DFD) (retn, no stack
// args), void. Vftable slot 193 (+0x304 - the slot the state-6 branches
// in incrementWaypoint/checkDistance dispatch through; after this hook
// those routes here).
// Decode (asm 0x4A3DD0-0x4A3DFD): vfptr->setForwardVelocity(this, 0)
// (slot +0x2E8, asm 0x4A3DD7); vfptr->setForwardAcceleration(this, 0)
// (slot +0x2F0, asm 0x4A3DE3); vfptr->setWaitingToMove(this, 0)
// (slot +0x1B8, asm 0x4A3DEF); RGE_Static_Object::
// removeAllFromPathingGroup (:3889, direct call, asm 0x4A3DF7).
void __fastcall RGE_Moving_Object__stopMoving_new(RGE_Moving_Object* this_, DWORD dummy)
{
    this_->vfptr->setForwardVelocity(this_, 0.0f);                   // asm 0x4A3DD3-0x4A3DD7 (slot +0x2E8)
    this_->vfptr->setForwardAcceleration(this_, 0.0f);               // asm 0x4A3DDD-0x4A3DE3 (slot +0x2F0)
    this_->vfptr->setWaitingToMove(this_, 0);                        // asm 0x4A3DE9-0x4A3DEF (slot +0x1B8)
    RGE_Static_Object__removeAllFromPathingGroup((RGE_Static_Object*)this_);   // asm 0x4A3DF5-0x4A3DF7 (:3889)
}

// Replacement for RGE_Moving_Object::calculateFutureVelocity. Original:
// @ 0x004A3E00 (idb_functions.h:2187). Range [0x4A3E00, 0x4A3F0C)
// (retn 10h = 4 stack args), returns double. Vftable-slotted (DATA XREF
// :279612-region).
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): the cap-return exit
// leaves st0 = the 53-bit cap UNROUNDED (asm 0x4A3EE6 jz falls through
// to retn 0x4A3F0C with cap still on the x87 stack) - the former
// (float) cast was the st0-boundary truncation, removed. All other
// exits load/reload m32 values (const_f_zero, mMaximumVelocityForward,
// the stored target) - exact as doubles.
// Decode (asm 0x4A3E00-0x4A3F0C):
//   target = acceleration * secondsInTheFuture + currentVelocity (the
//     x87 chain stores the float-rounded result - asm 0x4A3E20; written
//     with double intermediates + one float store = bit-exact);
//   group = RGE_Game_World::getUnitGroup(owner->world @+0x8C, mGroupID
//     @+0x88) (:6099, asm 0x4A3E24);
//   if (group && (group->vfptr->isMoving(group) == 1 (BUnitGroup vtbl
//     slot +0x78, asm 0x4A3E33) || !BUnitGroup::inDesiredPosition(group,
//     id @+0x10) (:1702, asm 0x4A3E41))):
//       reason = BUnitGroup::getSpecificMoveReason(group, id) (:1706,
//         asm 0x4A3E50) - ONE call (BAKED: core.cpp:1393
//         writeByte(0x004A3E59, 0x16) retargets the jnz to skip the
//         original's second call; the patched binary reuses this result
//         for both tests);
//       reason == 2 -> return 0.0f (asm 0x4A3E5A);
//       reason != -1 -> return group->mMaximumVelocityForward @+0xB0
//         (asm 0x4A3E75; walker-confirmed :3362);
//       target < 10.0f (flt_6565F0, lst :987125, asm 0x4A3E84): target
//         < 0 -> 0.0f else target (asm 0x4A3E91-0x4A3EAA);
//   cap (BAKED: casts.cpp:882 setHook(0x004A3EAD, speed3) - the
//     entry-form hook replays the sprite test and multiplies
//     getSpeedModifier(this) into master->speed BEFORE the base_speed
//     multiply; its gate is the same double-zero fcomp; the float
//     return of the helper is consumed raw (asm fld [esp])):
//       sprite && base_speed > 0 -> cap = master->speed * modifier *
//         base_speed; else cap = master->speed * modifier;
//   merge (asm 0x4A3EDB): the cap is compared at 53-bit BEFORE any
//     float store (fcomp st(1), asm 0x4A3EDF) - written as a double
//     compare; target <= cap ? (target < 0 ? 0 : target) : cap.
double __fastcall RGE_Moving_Object__calculateFutureVelocity_new(
    RGE_Moving_Object* this_, DWORD dummy, float currentVelocity,
    float acceleration, float secondsInTheFuture, BVector* futurePos)
{
    float target = (float)((double)acceleration * (double)secondsInTheFuture
        + (double)currentVelocity);                                  // asm 0x4A3E20
    BUnitGroup* group = RGE_Game_World__getUnitGroup(
        (RGE_Game_World*)this_->owner->world, this_->mGroupID);      // asm 0x4A3E24 (:6099; owner +0x18, world +0x8C, mGroupID +0x88)
    if (group != 0
        && (group->vfptr->isMoving(group) == 1                        // asm 0x4A3E33 (BUnitGroup vtbl slot +0x78)
            || !BUnitGroup__inDesiredPosition(group, this_->id)))     // asm 0x4A3E41 (:1702)
    {
        int reason = BUnitGroup__getSpecificMoveReason(group, this_->id);   // asm 0x4A3E50 (:1706) // baked: core.cpp:1393 writeByte(0x004A3E59, 0x16)
        if (reason == 2)
            return 0.0f;                                             // asm 0x4A3E5A
        if (reason != -1)
            return group->mMaximumVelocityForward;                   // asm 0x4A3E75 (@+0xB0)
        if (target < 10.0f)                                          // asm 0x4A3E84 (flt_6565F0)
        {
            if (target < 0.0f)
                return 0.0f;                                         // asm 0x4A3EA2
            return target;                                           // asm 0x4A3F06
        }
    }
    double cap;
    uint32_t modifier_bits = getSpeedModifier((RGE_Static_Object*)this_);   // baked: casts.cpp:882 setHook(0x004A3EAD, speed3) - fetched unconditionally (asm stub entry)
    float modifier = *(float*)&modifier_bits;                        // the helper returns raw float bits (casts.cpp:140 "returns 32 bit float in eax")
    if (this_->sprite != 0 && this_->sprite->base_speed > 0.0)       // asm stub (double-zero fcomp = the original's const_d_zero gate)
        cap = (double)this_->master_obj->speed * (double)modifier * (double)this_->sprite->base_speed;   // left-assoc = the stub's fmul order
    else
        cap = (double)this_->master_obj->speed * (double)modifier;   // asm stub loc_4A3ED2 form
    if ((double)target <= cap)
    {
        if (target < 0.0f)
            return 0.0f;                                             // asm 0x4A3EFB
        return target;                                               // asm 0x4A3F06
    }
    return cap;                                                      // asm 0x4A3EE6 jz path: st0 = the 53-bit cap UNROUNDED at retn (0x4A3F0C) - corollary (h), no float conversion
}

// Replacement for RGE_Moving_Object::precalcDistToTarget. Original:
// @ 0x004A3F40 (idb_functions.h:2189). Range [0x4A3F40, 0x4A3FF1)
// (retn 4), returns double. By-address caller:
// doIntelligentMovementUpdate+154.
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): every original exit
// reloads an m32-rounded value - fld [esp+var_4]; retn (asm 0x4A3FE7 /
// 0x4A3FF1) - so the float `result` local is kept (rule (e)); the
// double return widens the rounded value exactly.
// Decode (asm 0x4A3F40-0x4A3FF1): result = SINGLE_MAXIMUM_VALUE (asm
// 0x4A3F45); pathData->targetID @+0x24 <= -1 -> FLT_MAX; obj =
// world->object(targetID) (:6086; owner->world @+0x8C) -> *targetObj =
// obj, null -> FLT_MAX; result = distance_to_object (:3871; the call
// result is STORE-ROUNDED to var_4 immediately, asm 0x4A3F82 fstp -
// the explicit (float) replicates the rounding point); if obj->
// master_obj->object_group (word @+0x1E) == 8 (Gate): for i in [0, 2)
// (the original's literal 2, asm 0x4A3FD9 - a virtual getLinkedChild
// loop, J3): child = world->object(obj->vfptr->getLinkedChild(obj, i))
// (slot +0x358; (*base_game)->world @+0x420) -> non-null:
// result = min(result, distance - 0.1f (flt_6545BC, lst :986135, asm
// 0x4A3FBF; the subtraction feeds the compare -> double per the FLOAT
// PRECISION rule (a); the update store-rounds, asm 0x4A3FD0 fstp)).
double __fastcall RGE_Moving_Object__precalcDistToTarget_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Static_Object** targetObj)
{
    float result = FLT_MAX;                                          // asm 0x4A3F45 (SINGLE_MAXIMUM_VALUE)
    int targetID = this_->pathData->targetID;                        // asm 0x4A3F4D-0x4A3F53 (@+0x24)
    if (targetID <= -1)
        return result;                                               // asm 0x4A3F56-0x4A3F59
    RGE_Static_Object* obj = RGE_Game_World__object(
        (RGE_Game_World*)this_->owner->world, targetID);             // asm 0x4A3F5F-0x4A3F69 (:6086)
    *targetObj = obj;                                                // asm 0x4A3F72-0x4A3F74
    if (obj == 0)
        return result;                                               // asm 0x4A3F76
    result = (float)RGE_Static_Object__distance_to_object(
        (RGE_Static_Object*)this_, obj);                             // asm 0x4A3F78-0x4A3F82 (:3871; fstp var_4 store-round @0x4A3F82 - the explicit cast keeps the rounding at the original's point)
    if (obj->master_obj->object_group != 8)                          // asm 0x4A3F86-0x4A3F8E (word @+0x1E; 8 = Gate)
        return result;                                               // asm 0x4A3F8E
    for (int i = 0; i < 2; ++i)                                      // asm 0x4A3F94-0x4A3FDC (J3)
    {
        RGE_Game_World* world = (RGE_Game_World*)(*base_game)->world;   // asm 0x4A3F94-0x4A3F9B (@+0x420)
        int childID = obj->vfptr->getLinkedChild(obj, i);            // asm 0x4A3FA1-0x4A3FA5 (slot +0x358, :4149)
        RGE_Static_Object* child = RGE_Game_World__object(world, childID);   // asm 0x4A3FAB-0x4A3FAE (:6086)
        if (child != 0)
        {
            // the nearer gate-child wins; the x87 update branch is test ah,1
            // FALL-THROUGH = "< OR unordered" - a NaN distance UPDATES in the
            // original; the literal C < is unordered-false and would skip
            // (precision sub-rule (i))
            double d = (double)RGE_Static_Object__distance_to_object(
                (RGE_Static_Object*)this_, child) - 0.1;             // asm 0x4A3FB7-0x4A3FBF (flt_6545BC)
            if (X87_LT_NAN(d, (double)result))               // asm 0x4A3FC5-0x4A3FD0 (fcom var_4; test ah,1; jz -> the fstp-st discard - {>=, NaN}: a NaN distance UPDATES, as the original)
                result = (float)d;                                   // asm 0x4A3FD0 (fstp var_4 store-round)
        }
    }
    return result;                                                   // asm 0x4A3FDE
}

// Replacement for RGE_Moving_Object::precalcDistToGoal. Original:
// @ 0x4A4000 (idb_functions.h:2190). Range [0x4A4000, 0x4A4098)
// (retn 8), returns double.
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): the main path is a
// PURE st0 PASSTHROUGH - call distance_to_position (asm 0x4A4081);
// retn (asm 0x4A408C) with NO intermediate store - the caller consumes
// the 53-bit value at full x87 precision; the double-typed idb
// constant now captures it (the st0-boundary fix: the former
// float-typed function truncated at the 24-bit call boundary). The
// other exits load m32 values (goal, the FLT_MAX local) - exact.
// Decode (asm 0x4A4000-0x4A4098): targetObj != 0 -> return goal (asm
// 0x4A400A-0x4A4020); result = SINGLE_MAXIMUM_VALUE; pd =
// highestLinkedPathDataPointer(pathData @+0x118) (:2510, asm 0x4A4029)
// -> null -> FLT_MAX; path = pd->path @+0x10 else (*base_game)->world ->
// path(pd->pathID @+0x14) (:6094) -> null -> FLT_MAX; two BPath::
// waypoint(path, buffer, count-1) calls (:2522 corrected prototype; two
// 12-byte out-copy buffers, asm 0x4A405A/0x4A406F - verbatim re-call);
// return distance_to_position(this, wp.x, wp.z, 0.0) (:3873; asm
// 0x4A407C reads .x from CALL 2's result, 0x4A406A read .z from CALL
// 1's result, z pushed first = 0.0). No float arithmetic - the
// precision rule does not apply.
double __fastcall RGE_Moving_Object__precalcDistToGoal_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Static_Object* targetObj, float goal)
{
    if (targetObj != 0)
        return goal;                                                 // asm 0x4A400A-0x4A4020
    BPathData* pd = BPathData__highestLinkedPathDataPointer(this_->pathData);   // asm 0x4A4023-0x4A4029 (:2510)
    if (pd == 0)
        return FLT_MAX;                                              // asm 0x4A408F
    BPath* path = pd->path;                                          // asm 0x4A4032 (@+0x10)
    if (path == 0)
    {
        path = RGE_Game_World__path(
            (RGE_Game_World*)(*base_game)->world, pd->pathID);       // asm 0x4A4039-0x4A4049 (:6094; pathID @+0x14)
        if (path == 0)
            return FLT_MAX;                                          // asm 0x4A408F
    }
    int count = path->waypointsCount;                                // asm 0x4A4055 (@+0xC)
    BVector out1;                                                    // waypoint() out-copy buffer 1 (asm wpIndex slot)
    BVector* wp1 = BPath__waypoint(path, &out1, count - 1);          // asm 0x4A405A-0x4A4065 (:2522)
    float wpY = wp1->y;                                              // asm 0x4A406A ([eax+8]; new .c :175686 ->y)
    BVector out2;                                                    // out-copy buffer 2 (asm var_C slot)
    BVector* wp2 = BPath__waypoint(path, &out2, count - 1);          // asm 0x4A406F-0x4A4077 (verbatim re-call)
    return RGE_Static_Object__distance_to_position(
        (RGE_Static_Object*)this_, wp2->x, wpY, 0.0f);               // asm 0x4A407C-0x4A4081 (:3873)
}

// Replacement for RGE_Moving_Object::incrementWaypoint. Original:
// @ 0x4A40A0 (idb_functions.h:2191). Range [0x4A40A0, 0x4A445D)
// (retn 8 = 2 stack args), returns u8. By-address caller:
// doIntelligentMovementUpdate+18B. IDA's "sp-analysis failed" (in the
// pre-regeneration dump) = tracking derailment at the virtual teleport;
// the regenerated dump resolves the stack and this decode follows it.
// Decode (asm 0x4A40A0-0x4A445D):
//   LINEAR BLOCK (pathData->mFlags @+0x30 & dword_65BD98 (= 2, lst
//   :990397), asm 0x4A40A8-0x4A40BB): if waypoint @+0x18 < path->
//   waypointsCount @+0xC: three waypoint calls (:2522 corrected
//   prototype; 12-byte out-copies into three stack buffers, asm
//   0x4A40CB/0x4A40EA-lea/0x4A4106-lea -> buffers 1/2/3); CALL 1's .z
//   (offset4, asm 0x4A40D8) is teleport param3; teleport(
//   wp.x, wp.y, wp.z) (slot +0xA8 = 42, asm 0x4A4111; the push order
//   offset4/offset8/offset0 lands params (x, y, z) - identity in the
//   corrected names; float return discarded, asm 0x4A4117 fstp st).
//   setFlag(pd, 1, 0). setFlag(pd, 0, 0); ++pd->waypoint (asm
//   0x4A413D-0x4A4141).
//   BOUNDS FIXUP (waypoint < waypointsCount, asm 0x4A4144-0x4A420F):
//   four waypoint(path, buffer3, waypoint) calls (asm 0x4A4162/0x4A418B/
//   0x41B1/0x41E5 - all target buffer3, displacement-derived);
//   ++waypoint when wp.x < master radius_x @+0x3C, wp.y < radius_y
//   @+0x40, map_width - radius_x < wp.x, map_width - radius_y < wp.y
//   (owner @+0x18 -> world @+0x8C -> map @+0x34, map_width @+8 as fild
//   int; BOTH width tests read map+8 - asm-faithful; the fild/fsub
//   arithmetic feeds compares -> double per the FLOAT PRECISION rule
//   (a)/(f); the wp.x/radius_x and wp.y/radius_y pairs are direct
//   loaded-float compares - float form bit-safe, rule (b)).
//   PATROL (path->flags @+0x20 & BBitVectorAND_2 (= 1, lst :990395),
//   asm 0x4A4212-0x4A421D): waypoint = nextPatrolWaypoint(pd->path
//   @+0x10, waypoint - 1) (:2527, the corrected 1-arg form); state
//   @+0x1C = 2; waypoint == -1 -> state = 4; return 1.
//   GATE DISTANCE (asm 0x4A4223-0x4A4336): waypoint(path, buffer3,
//   waypoint - 1) (asm 0x4A422B - buffer3 refilled); waypoint(path,
//   buffer2, waypoint - 1) (asm 0x4A424A); wd = distance_to_position(
//   this, wp.x, wp.y, 0.0) (:3873, asm 0x4A425C). If obj: seed =
//   distance_to_object(obj) - range @+0x20 - 0.1 (flt_6545BC, lst
//   :986135) -> the MIN (asm 0x4A4290; the regenerated .c resolves the
//   storage as the dedicated patha local - the compiler reuses the dead
//   path-argument slot). group-8 blockers (word @+0x1E == 8): for i in
//   [0, 2) (J3): child = world->object(getLinkedChild(obj, i)); d =
//   distance_to_object(child) - range - 0.1 (53-bit chain, asm
//   0x4A42C6-0x4A42D7); if (d < min) min = (float)d (fcom patha / fstp
//   patha, asm 0x4A42DD-0x4A42E8). mFlags & dword_65BD9C (= 4) -> min =
//   0.0f + setFlag(pd, 2, 0) (asm 0x4A42FF-0x4A431C). The obj == 0
//   branch seeds min = wd - range - 0.1 (asm 0x4A4323-0x4A4336 - the
//   first draft's "dead store" was WRONG; the regenerated .c confirms
//   the seed flows into the tests).
//   STATE MACHINE (asm 0x4A433A-0x4A4416): waypoint < waypointsCount ->
//   waypointLevel @+0xC > 0 -> state = 2 (both branches return 1).
//   min < 1e-7 (flt_6565F4, lst :987127) -> state = 4, return 1. obj ==
//   0 || min >= 1.0f (const_f_one) || waypointLevel == 0 -> the wd test
//   (loc_4A439F: the operand = wd/v24 - the regenerated .c; the
//   pre-regeneration dump's "wpHeight test" was the derailed-tracking
//   artifact). wd < 1e-7: waypointLevel == 2 -> state = 6 + vfptr->
//   stopMoving (slot +0x304 - routes into stopMoving_new) + return 0;
//   else state = 4, return 1. wd >= 1e-7 -> waypoint = waypoint - 1,
//   state = 2, return 1.
// PRECISION SCOPE (owner-directed 2026-11-19, the rule-(j) retroactive
// sweep FIND): the obj == 0 seed's chain terminates in a float store
// (minDist), which licenses the /fp:fast shrink - the emitted form was
// subss (24-bit (wd - range)) + cvtps2pd + subsd 0.1, a float rounding
// the original never performs (asm 0x4A4329-0x4A4336: fld wd / fsub
// range / fsub flt_6545BC both at 53-bit, ONE fstp m32). The explicit
// (double) casts and a double local both failed to resist (/Og forwards
// through the local; /Fa evidence in the sweep record); under precise
// semantics the shrink transform is unavailable and the emitted form is
// cvtps2pd x2 + subsd x2 + one cvtpd2ps. The bracket pins the whole
// function precise - the /Fa gate check for this function must verify
// the seed form (no subss in the Line-1116 block) on every re-gate; the
// function has no divisions, so no reciprocal-form risk is introduced.
#pragma float_control(precise, on, push)
unsigned __int8 __fastcall RGE_Moving_Object__incrementWaypoint_new(
    RGE_Moving_Object* this_, DWORD dummy, BPath* path, RGE_Static_Object* obj)
{
    BPathData* pd = this_->pathData;                                 // asm 0x4A40B2 (@+0x118)
    BVector out1;                                                    // waypoint out-copy buffer 1
    BVector out2;                                                    // out-copy buffer 2
    BVector out3;                                                    // out-copy buffer 3
    if ((pd->mFlags.valueValue & 2) != 0)                            // asm 0x4A40A8-0x4A40BB (dword_65BD98 = 2)
    {
        int wp = pd->waypoint;                                       // asm 0x4A40BD (@+0x18)
        if (wp < path->waypointsCount)                               // asm 0x4A40C0-0x4A40C5 (@+0xC)
        {
            BVector* wp1 = BPath__waypoint(path, &out1, wp);         // asm 0x4A40CB-0x4A40D3 (:2522)
            BVector* wp2 = BPath__waypoint(path, &out2, pd->waypoint);   // asm 0x4A40E6-0x4A40ED
            BVector* wp3 = BPath__waypoint(path, &out3, pd->waypoint);   // asm 0x4A40FC-0x4A4107
            this_->vfptr->teleport(this_, wp3->x, wp2->y, wp1->z);   // asm 0x4A410C-0x4A4111 (slot +0xA8; params (x, y, z) identity)
        }
        BPathData__setFlag(pd, 1, 0);                                // asm 0x4A4119-0x4A4123 (:2517)
    }
    BPathData__setFlag(pd, 0, 0);                                    // asm 0x4A4128-0x4A4132 (:2517)
    ++pd->waypoint;                                                  // asm 0x4A413D-0x4A4141
    if (pd->waypoint < path->waypointsCount)                         // asm 0x4A4144-0x4A4152
    {
        BVector* wp = BPath__waypoint(path, &out3, pd->waypoint);    // asm 0x4A415A-0x4A4162 (buffer3)
        if (wp->x < this_->master_obj->radius_x                      // asm 0x4A4167-0x4A4174 (direct float compare - bit-safe, rule (b))
            || BPath__waypoint(path, &out3, pd->waypoint)->y < this_->master_obj->radius_y   // asm 0x4A417A-0x4A419E (@+0x40; verbatim re-call)
            || (double)this_->owner->world->map->map_width - (double)this_->master_obj->radius_x < (double)BPath__waypoint(path, &out3, pd->waypoint)->x   // asm 0x4A41A0-0x4A41D2 (fild/fsub 53-bit - double form, rule (a)/(f))
            || (double)this_->owner->world->map->map_width - (double)this_->master_obj->radius_y < (double)BPath__waypoint(path, &out3, pd->waypoint)->y)  // asm 0x4A41D4-0x4A4207 (BOTH width tests read map+8 - asm-faithful)
        {
            ++pd->waypoint;                                          // asm 0x4A4209-0x4A420F
        }
    }
    if ((path->flags.valueValue & 1) != 0)                           // asm 0x4A4212-0x4A421D (BBitVectorAND_2 = 1)
    {
        pd->waypoint = BPath__nextPatrolWaypoint(pd->path, pd->waypoint - 1);   // asm 0x4A4419-0x4A4432 (:2527)
        pd->state = 2;                                               // asm 0x4A443B (@+0x1C)
        if (pd->waypoint == -1)
            pd->state = 4;                                           // asm 0x4A444E
        return 1;                                                    // asm 0x4A4457
    }
    BVector* wpG1 = BPath__waypoint(path, &out3, pd->waypoint - 1);  // asm 0x4A422B-0x4A4237 (buffer3 refill)
    float wpGy = wpG1->y;                                            // asm 0x4A4242 ([eax+8])
    BVector* wpG2 = BPath__waypoint(path, &out2, pd->waypoint - 1);  // asm 0x4A4246-0x4A4252 (buffer2, verbatim)
    float wd = (float)RGE_Static_Object__distance_to_position(
        (RGE_Static_Object*)this_, wpG2->x, wpGy, 0.0f);             // asm 0x4A425A-0x4A4265 (:3873; fstp var_28 STORE-ROUND @0x4A4265 - wd IS float in the original; both consumers reload the m32: the no-obj seed fld @0x4A4329, the inner gate fld @0x4A439F. The explicit cast keeps the rounding at the original's point under the double-returning constant - corollary (h) per-site discipline)
    float minDist;
    RGE_Static_Object* objLocal = obj;                               // asm 0x4A4261 (ebx)
    if (objLocal != 0)
    {
        minDist = (float)((double)RGE_Static_Object__distance_to_object(
            (RGE_Static_Object*)this_, objLocal)
            - (double)pd->range - 0.1);                              // asm 0x4A4271-0x4A4290 (the seed - flt_6545BC)
        if (objLocal->master_obj->object_group == 8)                 // asm 0x4A4285 (word @+0x1E)
        {
            for (int i = 0; i < 2; ++i)                              // asm 0x4A429F-0x4A42FC (J3)
            {
                RGE_Game_World* world = (RGE_Game_World*)(*base_game)->world;   // asm 0x4A429F-0x4A42AC (@+0x420)
                int childID = objLocal->vfptr->getLinkedChild(objLocal, i);   // asm 0x4A42B4 (slot +0x358)
                RGE_Static_Object* child = RGE_Game_World__object(world, childID);   // asm 0x4A42BA-0x4A42BD (:6086)
                if (child != 0)
                {
                    double d = (double)RGE_Static_Object__distance_to_object(
                        (RGE_Static_Object*)this_, child)
                        - (double)pd->range - 0.1;                   // asm 0x4A42C6-0x4A42D7 (53-bit chain - double form)
                    if (d < (double)minDist)                         // asm 0x4A42DD (fcom minDist - the stored float, exact-extended)
                        minDist = (float)d;                          // asm 0x4A42E8 (fstp m32 - store-round, rule (e))
                }
            }
        }
        if ((pd->mFlags.valueValue & 4) != 0)                        // asm 0x4A42FF-0x4A430E (dword_65BD9C = 4)
        {
            minDist = 0.0f;                                          // asm 0x4A4314 (int-zero store = 0.0f)
            BPathData__setFlag(pd, 2, 0);                            // asm 0x4A4310-0x4A431C (:2517)
        }
    }
    else
    {
        double seed = (double)wd - (double)pd->range - 0.1;          // asm 0x4A4323-0x4A4336 (the obj == 0 seed - fld wd / fsub range / fsub flt_6545BC, both subtractions 53-bit)
        minDist = (float)seed;                                       // asm 0x4A4336 (fstp m32 - the single store-round; the double local forces the /Fa widen-before-op - rule (j))
    }
    if (pd->waypoint < path->waypointsCount)                         // asm 0x4A433A-0x4A4348
    {
        if (pd->waypointLevel > 0)                                   // asm 0x4A4400-0x4A4405 (@+0xC)
            pd->state = 2;                                           // asm 0x4A4409 (@+0x1C)
        return 1;                                                    // asm 0x4A4410 / 0x4A4455
    }
    if (minDist >= 1.0e-7f)                                          // asm 0x4A434E-0x4A435D (direct float compare - bit-safe)
    {
        if (objLocal != 0 || minDist >= 1.0f || pd->waypointLevel == 0)   // asm 0x4A4371-0x4A438B (const_f_one; direct compares)
        {
            if (wd >= 1.0e-7f)                                       // asm loc_4A439F (fld [esp]; the operand = wd/v24 per the regenerated .c - the pre-regeneration "wpHeight test" was the derailed-tracking artifact)
            {
                pd->waypoint = pd->waypoint - 1;                     // asm 0x4A43E4-0x4A43E6
                pd->state = 2;                                       // asm 0x4A43F1
                return 1;                                            // asm 0x4A43F8
            }
            if (pd->waypointLevel == 2)                              // asm 0x4A43B0-0x4A43B4
            {
                pd->state = 6;                                       // asm 0x4A43C8
                this_->vfptr->stopMoving(this_);                     // asm 0x4A43D3 (slot +0x304 -> stopMoving_new)
                return 0;                                            // asm 0x4A43DB
            }
        }
        pd->state = 4;                                               // asm 0x4A438F / 0x4A43B8
        return 1;                                                    // asm 0x4A4396 / 0x4A43BF
    }
    pd->state = 4;                                                   // asm 0x4A4361
    return 1;                                                        // asm 0x4A4368
}
#pragma float_control(pop)

// Replacement for RGE_Moving_Object::checkDistance. Original:
// @ 0x4A4460 (idb_functions.h:2192). Range [0x4A4460, 0x4A4590)
// (retn 4), returns u8. By-address caller:
// doIntelligentMovementUpdate+1523. IDA's "sp-analysis failed" (in the
// pre-regeneration dump) = a tracking artifact - the pushes/pops
// balance.
// Decode (asm 0x4A4460-0x4A4590): pd = pathData; linked = pd->linkedPath
// @+4; the reset+stopMoving path when linked == 0 OR a2 <= pd->range
// @+0x20 (test ah,41h) OR type not in {1,2,3}; linkedPathType @+8 == 1:
// pd->linkedPath = 0, destroy old (BPathData::destructor :2506 +
// operator_delete - written operator_delete_internal per the CRT rule),
// pathData = linked, setFlag(linked, 0, 1), return 0; == 2: same swap +
// linked->state = 2; p = pd->path @+0x10 null -> 0; p->flags & 1 == 0
// -> 0; pd->waypoint = nextPatrolWaypoint(p, pd->waypoint) (:2527);
// waypoint == -1 -> the reset path (on the NEW pathData); else 0;
// == 3: same swap + state = 2, return 0. Reset path: BPathData::reset
// (:2507) + vfptr->stopMoving (slot +0x304 -> stopMoving_new),
// return 1.
unsigned __int8 __fastcall RGE_Moving_Object__checkDistance_new(
    RGE_Moving_Object* this_, DWORD dummy, float a2)
{
    BPathData* pd = this_->pathData;                                 // asm 0x4A4465-0x4A4467 (@+0x118)
    BPathData* linked = pd->linkedPath;                              // asm 0x4A446D (@+4)
    if (linked == 0 || a2 <= pd->range)                              // asm 0x4A4470-0x4A4484 (@+0x20; test ah,41h)
        goto reset_stop;
    int type = pd->linkedPathType;                                   // asm 0x4A448A (@+8)
    if (type == 1)                                                   // asm 0x4A448D-0x4A4490
    {
        pd->linkedPath = 0;                                          // asm 0x4A4494
        if (pd != 0)                                                 // asm 0x4A449D-0x4A449F
        {
            BPathData__destructor(pd);                               // asm 0x4A44A3 (:2506)
            operator_delete_internal(pd);                            // asm 0x44A8-region
        }
        this_->pathData = linked;                                    // asm 0x44B7 (@+0x118)
        BPathData__setFlag(linked, 0, 1);                            // asm 0x4A44B1-0x4A44BD (:2517)
        return 0;                                                    // asm 0x4A44C4
    }
    if (type == 2)                                                   // asm 0x4A44CA-0x4A44CD
    {
        pd->linkedPath = 0;                                          // asm 0x4A44D5
        if (pd != 0)
        {
            BPathData__destructor(pd);                               // asm 0x4A44E4 (:2506)
            operator_delete_internal(pd);                            // asm 0x4A44EA
        }
        this_->pathData = linked;                                    // asm 0x4A44F8
        linked->state = 2;                                           // asm 0x4A44FF (@+0x1C)
        BPath* p = this_->pathData->path;                            // asm 0x4A4505-0x4A4508 (@+0x10)
        if (p == 0)
            return 0;                                                // asm 0x4A450A
        if ((p->flags.valueValue & 1) == 0)                          // asm 0x4A450C-0x4A4517 (BBitVectorAND_2 = 1)
            return 0;                                                // asm 0x4A4517
        this_->pathData->waypoint = BPath__nextPatrolWaypoint(p, this_->pathData->waypoint);   // asm 0x4A4519-0x4A4528 (:2527, 1-arg)
        if (this_->pathData->waypoint == -1)                         // asm 0x4A452B-0x4A4535
            goto reset_stop;                                         // asm 0x4A4535 (ecx = the NEW pathData)
        return 0;                                                    // asm 0x4A4539
    }
    if (type != 3)                                                   // asm 0x4A453F-0x4A4542
        goto reset_stop;
    pd->linkedPath = 0;                                              // asm 0x4A454A
    if (pd != 0)
    {
        BPathData__destructor(pd);                                   // asm 0x4A4559 (:2506)
        operator_delete_internal(pd);                                // asm 0x4A455F
    }
    this_->pathData = linked;                                        // asm 0x4A4567
    linked->state = 2;                                               // asm 0x4A456D (@+0x1C)
    return 0;                                                        // asm 0x4A4576
reset_stop:
    BPathData__reset(this_->pathData);                               // asm 0x4A457C (:2507; ecx = pathData on every entry path)
    this_->vfptr->stopMoving(this_);                                 // asm 0x4A4581-0x4A4585 (slot +0x304 -> stopMoving_new)
    return 1;                                                        // asm 0x4A458D
}

// Replacement for RGE_Moving_Object::checkRange. Original: @ 0x4A45A0
// (idb_functions.h:2193). Range [0x4A45A0, 0x4A4643) (retn 4), returns
// u8. By-address caller: doIntelligentMovementUpdate+150E.
// Decode (asm 0x4A45A0-0x4A4643): d = distance_to_object(obj) -
// pathData->range @+0x20 (the subtraction is stored float, asm
// 0x4A45BF - single-op float store, bit-safe per rule (d)); if obj->
// master_obj->object_group == 8: for i in [0, 2) (J3): child =
// world->object(getLinkedChild(obj, i)) ((*base_game)->world @+0x420) ->
// d = min(d, child distance - range) (the subtraction feeds the
// compare -> double per rule (a)); d < 1e-7 (flt_6565F4, lst :987127,
// direct float-vs-constant compare, asm 0x4A461C) -> setFlag(pd, 0, 1),
// return 1; else return 0.
unsigned __int8 __fastcall RGE_Moving_Object__checkRange_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    float d = (float)((double)RGE_Static_Object__distance_to_object(
        (RGE_Static_Object*)this_, obj) - (double)this_->pathData->range);   // asm 0x4A45A9-0x4A45BF (:3871; range @+0x20)
    if (obj->master_obj->object_group == 8)                          // asm 0x4A45B4-0x4A45C3 (word @+0x1E)
    {
        for (int i = 0; i < 2; ++i)                                  // asm 0x4A45C9-0x4A4614 (J3)
        {
            RGE_Game_World* world = (RGE_Game_World*)(*base_game)->world;   // asm 0x4A45C9-0x4A45D4 (@+0x420)
            int childID = obj->vfptr->getLinkedChild(obj, i);        // asm 0x4A45DA (slot +0x358, :4149)
            RGE_Static_Object* child = RGE_Game_World__object(world, childID);   // asm 0x4A45E0-0x4A45E3 (:6086)
            if (child != 0)
            {
                double cd = (double)RGE_Static_Object__distance_to_object(
                    (RGE_Static_Object*)this_, child)
                    - (double)this_->pathData->range;                // asm 0x4A45EF-0x4A45FA (53-bit chain - double form)
                if (cd < (double)d)
                    d = (float)cd;                                   // asm 0x4A45FD-0x4A4608
            }
        }
    }
    if (d >= 1.0e-7f)                                                // asm 0x4A4618-0x4A4627 (flt_6565F4; direct compare - bit-safe)
        return 0;                                                    // asm 0x4A463F
    BPathData__setFlag(this_->pathData, 0, 1);                       // asm 0x4A4629-0x4A4633 (:2517)
    return 1;                                                        // asm 0x4A4639
}

// Replacement for RGE_Moving_Object::moveUnit. Original: @ 0x4A4650
// (idb_functions.h:2194 - the constant's trailing by-value BVector is
// an IDA artifact; retn 0xC = 12 stack bytes = the 4th argument is a
// POINTER, confirmed by the caller (doIntelligentMovementUpdate+169C
// passes lea eax, [local BVector]) and the body (a4 used as BVector*
// for the checkAcceleration call and the compares)). Range
// [0x4A4650, 0x4A47AD) (retn 0xC), void.
// Decode (asm 0x4A4650-0x4A47AD): fwd = orientationForwardValue @+0x100
// (local copy); if isFlagSet(this, 3) (:3897) || isFlagSet(this, 4):
// normalize to the horizontal plane (sqrt over .y and .x - the two
// horizontals per the corrected BVector; .z (offset4, the vertical) =
// 0/len; asm 0x4A468F-0x4A46C3); yawArg = 0; v14 = vfptr->
// calculateYawAngle(this, &fwd, a2, a3, &yawArg) (slot +0x324, asm
// 0x4A46E5); if !(pathData->mFlags @+0x30 & 1 (BBitVectorAND_2 = 1)):
// vfptr->yaw(this, v14) (slot +0x2FC, asm 0x4A4708); hp =
// highestLinkedPathPointer(pathData) (:2509); if hp && |wp.x - a4->x| <
// 0.5 (dbl_654DC0 double-compare, asm 0x4A4744) && |wp.y - a4->y| <
// 0.5 (asm 0x4A476F) - the waypoint = &path->waypoints[count-1]
// (@+0x14/@+0xC) when count >= 2 else &fallbackWaypointVector
// (stru_6AE6A0, asm 0x4A4739/0x4A4762): checkAcceleration(this, a4, 1,
// yawArg) (slot +0x330) else checkAcceleration(this, a4, 0, yawArg).
// The compares consume the 53-bit subtraction results - double form
// per the FLOAT PRECISION rule (a).
void __fastcall RGE_Moving_Object__moveUnit_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* a2, float a3, BVector* a4)
{
    BVector fwd = this_->orientationForwardValue;                    // asm 0x4A4659-0x4A4675 (@+0x100)
    if (RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 3)   // asm 0x4A4679 (:3897)
        || RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 4))   // asm 0x4A4682-0x4A468D (:3897)
    {
        double len = sqrt((double)fwd.y * (double)fwd.y
            + (double)fwd.x * (double)fwd.x);                        // asm 0x4A468F-0x4A46A1 (the horizontal norm over .y/.x)
        fwd.x = (float)((double)fwd.x / len);                        // asm 0x4A46A3-0x4A46A9
        fwd.z = (float)(0.0 / len);                                  // asm 0x4A46AD-0x4A46B5 (const_f_zero / len - the vertical zeroed)
        fwd.y = (float)((double)fwd.y / len);                        // asm 0x4A46B9-0x4A46BF
    }
    int yawArg = 0;                                                  // asm 0x4A46CF (var_10)
    float v14 = (float)this_->vfptr->calculateYawAngle(this_, &fwd, a2, a3, &yawArg);   // asm 0x4A46D7-0x4A46F6 (slot +0x324; fstp a3 STORE-ROUND @0x4A46F6 - v14 IS float in the original; the explicit cast keeps the rounding point under the double-typed vtbl slot - corollary (h))
    if ((this_->pathData->mFlags.valueValue & 1) == 0)               // asm 0x4A46EB-0x4A46FD (BBitVectorAND_2 = 1; ja = taken when != 0)
        this_->vfptr->yaw(this_, v14);                               // asm 0x4A46FF-0x4A4708 (slot +0x2FC)
    BPath* hp = BPathData__highestLinkedPathPointer(this_->pathData);   // asm 0x4A470E-0x4A4714 (:2509)
    BVector* last;
    if (hp != 0)
    {
        int count = hp->waypointsCount;                              // asm 0x4A4723 (@+0xC)
        if (count >= 2)
            last = &hp->waypoints[count - 1];                        // asm 0x4A472B-0x4A4732 (waypoints @+0x14, 12-byte stride)
        else
            last = fallbackWaypointVector;                           // asm 0x4A4739 (stru_6AE6A0)
        if (fabs((double)last->x - (double)a4->x) >= 0.5
            || fabs((double)last->y - (double)a4->y) >= 0.5)         // asm 0x4A473E-0x4A477A (dbl_654DC0 double-compare; test ah,1 = strict <; .x/.y = offsets 0/8)
        {
            this_->vfptr->checkAcceleration(this_, a4, 0, yawArg);   // asm 0x4A4796-0x4A47A2 (slot +0x330)
            return;                                                  // asm 0x4A47AD
        }
        this_->vfptr->checkAcceleration(this_, a4, 1, yawArg);       // asm 0x4A477C-0x4A4788 (slot +0x330)
        return;
    }
    this_->vfptr->checkAcceleration(this_, a4, 0, yawArg);           // asm 0x4A4796-0x4A47A2 (slot +0x330; hp == 0)
}

// ============ RUNTIME METHODS (batch M4 - movement-data + sprite pairs) ============

// Replacement for RGE_Moving_Object::forwardVelocity. Original:
// SEMANTIC: the forward-axis movement readout - the forward component (BVector .y, the planar axis) of the live BMotionData velocity; 0 when the unit has no movement data (idle).
// @ 0x004A3A10 (idb_functions.h). Range [0x4A3A10, 0x4A3A24), returns
// double. Decode (asm 0x4A3A10-0x4A3A24): movementData @+0x120 != 0 ->
// return movementData->velocity.y (fld [eax+8]; BVector.y = offset 8,
// velocity @+0 of BMovementData {velocity, acceleration}); else
// 0.0 (fld const_f_zero). Both exits are exact m32 loads - the double
// return is bit-identical (FLOAT PRECISION corollary (h)).
double __fastcall RGE_Moving_Object__forwardVelocity_new(RGE_Moving_Object* this_, DWORD dummy)
{
    // the forward-axis readout: the live BMotionData's forward velocity
    // component (BVector .y = the planar axis); 0 when the unit has never
    // been commanded
    if (this_->movementData != 0)                            // asm 0x4A3A16-0x4A3A18
        return this_->movementData->velocity.y;              // asm 0x4A3A1A (fld [eax+8])
    return 0.0f;                                             // asm 0x4A3A1E (fld const_f_zero)
}

// Replacement for RGE_Moving_Object::forwardAcceleration. Original:
// SEMANTIC: the forward-axis acceleration readout - the forward component of the live BMotionData acceleration; 0 when idle.
// @ 0x004A3AC0 (idb_functions.h). Range [0x4A3AC0, 0x4A3AD4), returns
// double. Decode (asm 0x4A3AC0-0x4A3AD4): movementData != 0 -> return
// movementData->acceleration.y (fld [eax+14h]; acceleration @+0xC of
// BMovementData + .y @+8); else 0.0. Exact m32 loads - bit-identical
// double return.
double __fastcall RGE_Moving_Object__forwardAcceleration_new(RGE_Moving_Object* this_, DWORD dummy)
{
    // the forward-axis acceleration readout: the live BMotionData's forward
    // acceleration component; 0 when the unit has never been commanded
    if (this_->movementData != 0)                            // asm 0x4A3AC6-0x4A3AC8
        return this_->movementData->acceleration.y;          // asm 0x4A3ACA (fld [eax+14h])
    return 0.0f;                                             // asm 0x4A3ACE (fld const_f_zero)
}

// Replacement for RGE_Moving_Object::setForwardVelocity. Original:
// SEMANTIC: the forward-velocity command - lazily creates the unit's BMotionData on the first NONZERO forward command, then stores the forward component; a zero command on an idle unit is a no-op (no allocation).
// @ 0x004A3A30 (idb_functions.h). Range [0x4A3A30, 0x4A3AAF), void,
// retn 4. SEH allocation-unwind frame (chunk 0x6428A0: mov eax,
// [ebp-10h]; push eax; call operator_delete - the fresh BMovementData
// is freed if the ctor throws; folded as the __except below, with the
// 0-init per the getPatrolInformation J1 precedent).
// Decode (asm 0x4A3A30-0x4A3AAF): IF movementData @+0x120 == 0 AND
// v != 0.0 (fcomp const_f_zero; test ah,40h; jnz end - the C3-only
// equality idiom; SSE setne reads the same ZF and is result-identical
// incl. NaN): operator_new(0x18) (0x18 = sizeof(BMovementData) = 2 x
// BVector(12) = 24, walked); non-null -> BMovementData ctor
// (idb constant, 0x004BBCF0); movementData = result; then IF
// movementData != 0: velocity.y = v (raw dword move, asm
// 0x4A3A93-0x4A3A9D). The store is SHARED by the existing-data path
// and the created-data path.
void __fastcall RGE_Moving_Object__setForwardVelocity_new(
    RGE_Moving_Object* this_, DWORD dummy, float v)
{
    BMovementData* new_data = 0;   // asm var_10 - the slot the SEH funclet frees if the ctor throws (0-init per the getPatrolInformation J1 precedent)
    __try
    {
        // no movement data AND an ordered-nonzero command: lazily create it -
        // the x87 gate is test ah,40h; jnz -> skip (the skip set includes
        // UNORDERED: NaN commands do NOT create the block); the literal C
        // v != 0.0f is unordered-TRUE (probe: ne_f = 10) and would allocate
        // on NaN - the faithful form is the ordered-nonzero disjunction
        // (precision sub-rule (i))
        if (this_->movementData == 0 && X87_NE_ORD(v, 0.0f))   // asm 0x4A3A4F-0x4A3A62 (test ah,40h; jnz -> the skip; the skip set includes unordered - NaN commands do NOT create; the macro = the ordered-nonzero set)
        {
            new_data = (BMovementData*)operator_new_internal(sizeof(BMovementData));   // the 24-byte velocity+acceleration block (asm push 18h; same target as the binary's operator_new)
            if (new_data != 0)                               // guarded ctor - a null propagates as "still no data"
                new_data = BMovementData__BMovementData(new_data);   // asm 0x4A3A80 (ctor 0x004BBCF0)
            this_->movementData = new_data;                  // asm 0x4A3A8B - the unit now carries live movement state
            if (this_->movementData != 0)                    // asm 0x4A3A89-0x4A3A91 - the command lands only if the block exists
                this_->movementData->velocity.y = v;         // asm 0x4A3A93-0x4A3A9D - the forward speed (BVector .y = the planar forward axis; raw dword move)
        }
        else if (this_->movementData != 0)                   // asm 0x4A3A49-0x4A3A51 - already moving: just update the forward speed
        {
            this_->movementData->velocity.y = v;             // asm 0x4A3A93-0x4A3A9D (the store is shared with the just-created path)
        }
    }
    __except (operator_delete_internal(new_data), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6428A0 - the fresh block is freed if the ctor throws
    {
    }
}

// Replacement for RGE_Moving_Object::setForwardAcceleration. Original:
// SEMANTIC: the forward-acceleration command - the same lazy-create-then-store shape as setForwardVelocity, for the acceleration component.
// @ 0x004A3AE0 (idb_functions.h). Range [0x4A3AE0, 0x4A3B6B), void,
// retn 4. SEH allocation-unwind frame (chunk 0x6428C0, identical
// 5-instruction funclet). Decode (asm 0x4A3AE0-0x4A3B55): the
// setForwardVelocity shape with acceleration.y ([eax+14h],
// asm 0x4A3B43-0x4A3B4D).
void __fastcall RGE_Moving_Object__setForwardAcceleration_new(
    RGE_Moving_Object* this_, DWORD dummy, float v)
{
    BMovementData* new_data = 0;   // asm var_10 (the SEH-freed slot; 0-init)
    __try
    {
        // lazy create, same rule as setForwardVelocity: only an ordered-nonzero
        // command wakes the block up (the x87 test ah,40h skip set includes
        // unordered - see the setForwardVelocity note, sub-rule (i))
        if (this_->movementData == 0 && X87_NE_ORD(v, 0.0f))   // asm 0x4A3AFF-0x4A3B12 (test ah,40h; jnz -> the skip; NaN commands do NOT create)
        {
            new_data = (BMovementData*)operator_new_internal(sizeof(BMovementData));   // asm 0x4A3B14 (push 18h)
            if (new_data != 0)
                new_data = BMovementData__BMovementData(new_data);   // asm 0x4A3B30
            this_->movementData = new_data;                  // asm 0x4A3B3B
            if (this_->movementData != 0)
                this_->movementData->acceleration.y = v;     // asm 0x4A3B43-0x4A3B4D ([eax+14h] - the forward acceleration component)
        }
        else if (this_->movementData != 0)                   // asm 0x4A3AF9-0x4A3B01 - already moving: update in place
        {
            this_->movementData->acceleration.y = v;         // the shared store
        }
    }
    __except (operator_delete_internal(new_data), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6428C0 - identical funclet
    {
    }
}

// Replacement for RGE_Moving_Object::calculateMaximumSpeed. Original:
// SEMANTIC: the speed CAP used by the movement planner - the master's speed, scaled by the sprite's base_speed when the sprite carries one; terrainType is accepted but unused (the planar planner's hook point). With the baked speed-modifier mechanic (SPEED_MOD) folded in.
// @ 0x004A6250 (idb_functions.h; Attributes: thunk per the .c). Range
// [0x4A6250, 0x4A627F), returns double, retn 4 (terrainType UNUSED by
// the original - kept for ABI). The original body is maximumSpeed's
// gate shape WITHOUT the modifier (asm 0x4A6250-0x4A627F: sprite
// @+0x1C test; base_speed @+0x74 fcomp const_d_zero; test ah,41h;
// fld [eax+D0h]; fmul [edx+74h]; retn 4 / pure load).
// BAKED (SPEED_MOD): casts.cpp:885 setHook(0x004A6250, speed4) - the
// ENTRY of this function (an ACTIVE patch; install order setCastHooks
// dllmain:451 < install_engine :765 previously overwrote the stub's
// E9, so this bake is the stub's first effective shipping form). The
// stub fetches getSpeedModifier(this) (raw float bits in eax;
// __stdcall) UNCONDITIONALLY before the gate and multiplies it into
// master->speed BEFORE the base_speed multiply (left-assoc: fld
// [esp]=modifier; fld [eax+D0h]=speed; fmul st(1),st; fstp st ->
// st0 = speed*modifier at 53-bit; the gate path then fmul
// [edx+74h]); the double-zero fcomp gate is result-identical to a
// float compare under all inputs. The double return reproduces the
// stub's unrounded 53-bit st0 (FLOAT PRECISION corollary (h)). Call
// site retired in casts.cpp (:885, commented in place). // SPEED_MOD
// markers at the affected lines.
double __fastcall RGE_Moving_Object__calculateMaximumSpeed_new(
    RGE_Moving_Object* this_, DWORD dummy, int terrainType)
{
    uint32_t modifier_bits = getSpeedModifier((RGE_Static_Object*)this_);   // baked SPEED_MOD (speed4): the terrain modifier from the unit's terrain standing (raw float bits; fetched unconditionally, before the gate - the stub's order)
    float modifier;
    memcpy(&modifier, &modifier_bits, sizeof(modifier));   // the helper returns raw float bits (casts.cpp:140) - 4-byte value copy, aliasing-clean
    if (this_->sprite != 0 && this_->sprite->base_speed > 0.0)   // asm 0x4A6253-0x4A6265 (double-zero fcomp) - the sprite's own cap only counts when it carries one
        return (double)this_->master_obj->speed * (double)modifier * (double)this_->sprite->base_speed;   // baked SPEED_MOD: master speed x terrain modifier x sprite cap, left-assoc fmul x2 - st0 unrounded at retn (0x4A626A-0x4A6273 form)
    return (double)this_->master_obj->speed * (double)modifier;   // baked SPEED_MOD: no sprite cap - just the modified master speed (loc_4A6276 form)
}

// Replacement for RGE_Moving_Object::setRunSprite. Original:
// SEMANTIC: swaps the unit's displayed sprite to its RUN set - the master supplies the sprite (polymorphic per tier); none -> no-op. The double getter call is real (gate + fetch).
// @ 0x004A69B0 (idb_functions.h). Range [0x4A69B0, 0x4A69E0), returns
// int. Decode (asm 0x4A69B0-0x4A69E0): master_obj @+0x14 ->
// master->vfptr->get_run_sprite(master, this) (asm 0x4A69B9 call
// [eax+7Ch] = master vtbl slot 31) == 0 -> return 0; ELSE the SAME
// getter is called a SECOND time (asm 0x4A69C9 call [edx+7Ch] - a
// REAL double call, not a render artifact) and its result is passed
// DIRECTLY to vfptr->new_sprite (asm 0x4A69CC-0x4A69CF push eax;
// call [edi+0xB0] = runtime vtbl slot 44) - the result is NEVER
// stored to this->sprite (the bootstrap-store lesson, DESYNC FIX #4);
// return 1.
// MASTER-VTBL CAST (cross-class rule (b) + cast-compatibility
// protocol): the runtime master_obj field is typed
// RGE_Master_Static_Object*, whose vtbl struct ends at slot 29
// (setStationaryYawRevolutionTime @+0x74) - slots 30/31 are declared
// only in the derived master vtbls, ALL at the same offsets (verified
// member-by-member: RGE_Master_Moving_ObjectVtbl def@14980 slots
// 30/31 = :15012/15013 get_move_sprite/get_run_sprite, and the same
// +0x78/+0x7C indices in the Missile (def@21459), Action, Combat,
// TRIBE_Combat and TRIBE_Building vtbls). The callers of setRunSprite
// are runtime Moving+ objects (the address sits in the
// Action/Combat/Moving runtime vftables - asm DATA XREFs), whose
// master_obj is a Moving+-tier master per the factory identity
// (DECOMP_HIERARCHY.md) - the cast to RGE_Master_Moving_Object* (the
// lowest tier declaring the slots) is layout-safe (vfptr @0) and
// slot-stable across every possible runtime tier.
int __fastcall RGE_Moving_Object__setRunSprite_new(RGE_Moving_Object* this_, DWORD dummy)
{
    RGE_Master_Moving_Object* master = (RGE_Master_Moving_Object*)this_->master_obj;   // asm 0x4A69B4 ([esi+14h]); see the cast note above
    if (master->vfptr->get_run_sprite(master, (RGE_Static_Object*)this_) == 0)   // asm 0x4A69B9 (slot +0x7C) - the master refuses (no run sprite for this tier/unit): stay as-is
        return 0;                                            // asm 0x4A69DD-0x4A69E0
    this_->vfptr->new_sprite(this_,
        master->vfptr->get_run_sprite(master, (RGE_Static_Object*)this_));   // asm 0x4A69C9-0x4A69CF - the SECOND getter call is real: its result goes straight into the sprite registration (never stored on the unit)
    return 1;                                                // asm 0x4A69D6
}

// Replacement for RGE_Moving_Object::setMoveSprite. Original:
// SEMANTIC: swaps the displayed sprite to the MOVE set - the setRunSprite shape against the master's move-sprite getter.
// @ 0x004A69F0 (idb_functions.h). Range [0x4A69F0, 0x4A6A20), returns
// int. Decode (asm 0x4A69F0-0x4A6A20): the setRunSprite shape with
// get_move_sprite (asm 0x4A69F9/0x4A6A09 call [eax+78h] = master vtbl
// slot 30); same double-call + pass-through-to-new_sprite form.
int __fastcall RGE_Moving_Object__setMoveSprite_new(RGE_Moving_Object* this_, DWORD dummy)
{
    // the unit's master, through the Moving-master vtbl (slots 30/31 live
    // only in the derived tiers - see the setRunSprite cast note)
    RGE_Master_Moving_Object* master = (RGE_Master_Moving_Object*)this_->master_obj;   // asm 0x4A69F4

    // the master refuses (no move sprite for this tier/unit): stay as-is
    if (master->vfptr->get_move_sprite(master, (RGE_Static_Object*)this_) == 0)   // asm 0x4A69F9 (slot +0x78)
        return 0;                                            // asm 0x4A6A1D-0x4A6A20

    // the SECOND getter call is real: its result goes straight into the
    // sprite registration (never stored on the unit)
    this_->vfptr->new_sprite(this_,
        master->vfptr->get_move_sprite(master, (RGE_Static_Object*)this_));   // asm 0x4A6A09-0x4A6A0F (slot +0x78 -> new_sprite +0xB0)
    return 1;                                                // asm 0x4A6A16
}

// Replacement for RGE_Moving_Object::setFidgetSprite. Original:
// SEMANTIC: swaps the displayed sprite to the FIDGET (idle-animation) set - the setRunSprite shape against the master's plain sprite getter.
// @ 0x004A6A30 (idb_functions.h). Range [0x4A6A30, 0x4A6A5D+), returns
// int. Decode (asm 0x4A6A30-0x4A6A5D): the setRunSprite shape with
// get_sprite (asm 0x4A6A39/0x4A6A49 call [eax+4] = master vtbl slot 1
// - IN the RGE_Master_Static_ObjectVtbl, no master cast needed);
// same double-call + pass-through-to-new_sprite form.
int __fastcall RGE_Moving_Object__setFidgetSprite_new(RGE_Moving_Object* this_, DWORD dummy)
{
    // the plain sprite getter is IN the Static master vtbl (slot 1) - no cast
    if (this_->master_obj->vfptr->get_sprite(this_->master_obj, (RGE_Static_Object*)this_) == 0)   // asm 0x4A6A39 (slot +0x04)
        return 0;                                            // asm 0x4A6A5D

    // the SECOND getter call is real: its result goes straight into the
    // sprite registration (never stored on the unit)
    this_->vfptr->new_sprite(this_,
        this_->master_obj->vfptr->get_sprite(this_->master_obj, (RGE_Static_Object*)this_));   // asm 0x4A6A49-0x4A6A4F (slot +0x04 -> new_sprite +0xB0)
    return 1;                                                // asm 0x4A6A4F-region epilogue
}

// ============ RUNTIME METHODS (batch M5a/M5b - path-distance pair + yaw family) ============

// Replacement for RGE_Moving_Object::distanceToHighestLevelPath2.
// SEMANTIC: how far the unit is from its highest-priority (linked) path - resolves the linked path (or re-fetches it by id through the world), then measures the unit's position against the path; 0.0 when there is nothing to measure against. The movement planner's arrival signal.
// Original: @ 0x004A2C90 (idb_functions.h). Range [0x4A2C90,
// 0x4A2CFF), returns double. Decode (asm 0x4A2C90-0x4A2CFF):
// pathData @+0x118 == 0 -> 0.0 (fld const_f_zero, asm 0x4A2C9E);
// path = BPathData__highestLinkedPathPointer(pathData) (:2509, asm
// 0x4A2CA7); null -> world = (*base_game)->world (asm 0x4A2CB0-0x4A2CBB,
// +0x420) -> path = RGE_Game_World__path(world,
// BPathData__highestLinkedPath(pathData)) (:2508/:6094, asm
// 0x4A2CC1-0x4A2CC9) -> null -> 0.0 (asm 0x4A2CD2); else return
// BPath__distanceFromPath(path, this->positionValue @+0xF4) (:2528,
// asm 0x4A2CDB-0x4A2CF8 - the 12-byte by-value BVector stack copy,
// esi+0F4h). The distanceFromPath return is a PURE st0 PASSTHROUGH
// (asm call 0x4A2CF8; retn 0x4A2CFF, no store) - the double-typed
// constant captures the 53-bit value (FLOAT PRECISION corollary (h)).
double __fastcall RGE_Moving_Object__distanceToHighestLevelPath2_new(
    RGE_Moving_Object* this_, DWORD dummy)
{
    if (this_->pathData == 0)                                // asm 0x4A2C9A-0x4A2C9C
        return 0.0f;                                         // asm 0x4A2C9E (fld const_f_zero)
    BPath* path = BPathData__highestLinkedPathPointer(this_->pathData);   // asm 0x4A2CA7 (:2509)
    if (path == 0)
    {
        RGE_Game_World* world = (RGE_Game_World*)(*base_game)->world;   // asm 0x4A2CB0-0x4A2CBB (@+0x420)
        path = RGE_Game_World__path(world, BPathData__highestLinkedPath(this_->pathData));   // asm 0x4A2CC1-0x4A2CC9 (:2508, :6094)
        if (path == 0)
            return 0.0f;                                     // asm 0x4A2CD2 (fld const_f_zero)
    }
    return BPath__distanceFromPath(path, this_->positionValue);   // asm 0x4A2CDB-0x4A2CF8 (:2528; by-value BVector; st0 passthrough)
}

// Replacement for RGE_Moving_Object::distanceToHighestLevelPath.
// SEMANTIC: the same measurement as distanceToHighestLevelPath2 but from an arbitrary point (the caller's BVector) instead of the unit's position.
// Original: @ 0x004A2D00 (idb_functions.h). Range [0x4A2D00,
// 0x4A2D71), returns double, retn 0Ch = the 12-byte by-value BVector
// point (the idb proto (this_, BVector point) is CORRECT - the caller
// stack copy is real, asm 0x4A2D4F-0x4A2D65 x->z->y dword copies).
// Decode (asm 0x4A2D00-0x4A2D71): the distanceToHighestLevelPath2
// shape with distanceFromPath(path, point) (asm 0x4A2D6A; st0
// passthrough - corollary (h)).
double __fastcall RGE_Moving_Object__distanceToHighestLevelPath_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector point)
{
    if (this_->pathData == 0)                                // asm 0x4A2D0A-0x4A2D0C
        return 0.0f;                                         // asm 0x4A2D0E (fld const_f_zero)
    BPath* path = BPathData__highestLinkedPathPointer(this_->pathData);   // asm 0x4A2D19 (:2509)
    if (path == 0)
    {
        RGE_Game_World* world = (RGE_Game_World*)(*base_game)->world;   // asm 0x4A2D22-0x4A2D2D (@+0x420)
        path = RGE_Game_World__path(world, BPathData__highestLinkedPath(this_->pathData));   // asm 0x4A2D33-0x4A2D3B (:2508, :6094)
        if (path == 0)
            return 0.0f;                                     // asm 0x4A2D44 (fld const_f_zero)
    }
    return BPath__distanceFromPath(path, point);             // asm 0x4A2D4F-0x4A2D6A (:2528; st0 passthrough)
}

// Replacement for RGE_Moving_Object::calculateTurnRadius. Original:
// SEMANTIC: the turn radius for the CURRENT speed - the master's base turn radius scaled by the ratio of the forward velocity to the master's turn-radius speed (faster = wider turns).
// @ 0x004A6510 (idb_functions.h:2201). Range [0x4A6510, 0x4A6553),
// returns double. Decode (asm 0x4A6510-0x4A6553): movementData
// @+0x120 == 0 -> 0.0 (fld const_f_zero, asm 0x4A651E); else master
// getTurnRadius (vtbl slot +0x5C = 23, asm 0x4A6535 - the slot is
// double-typed per the corrected RGE_Master_Static_ObjectVtbl :3608;
// in the Static vtbl, no cast) * velocity.y (fmul [eax+8], asm
// 0x4A6542) -> fstp var_4 STORE-ROUND (asm 0x4A6545 - the product is
// float in the original, kept float here); master getTurnRadiusSpeed
// (slot +0x60 = 24, asm 0x4A6549, :3609) -> fdivr (asm 0x4A654C:
// st0 = product / speed) - st0 UNROUNDED at retn (corollary (h)).
double __fastcall RGE_Moving_Object__calculateTurnRadius_new(RGE_Moving_Object* this_, DWORD dummy)
{
    // no movement data -> the unit has never been commanded: zero radius
    if (this_->movementData == 0)                            // asm 0x4A651A-0x4A651C
        return 0.0f;                                         // asm 0x4A651E (fld const_f_zero)

    // the master, through the Static vtbl (slots 23/24 - getTurnRadius/
    // getTurnRadiusSpeed - are in the base; now double-typed, no cast)
    RGE_Master_Static_Object* master = (RGE_Master_Static_Object*)this_->master_obj;    // asm 0x4A6527-0x4A652B (@+0x14; explicit cast per the cross-class rule - the flattened runtime structs are unrelated C++ types)

    // the radius scales with the CURRENT forward speed: base radius x the
    // forward velocity component - store-rounded to m32 exactly as the
    // original's fstp var_4 (the explicit cast keeps the rounding point)
    float product = (float)(master->vfptr->getTurnRadius(master)
        * (double)this_->movementData->velocity.y);          // asm 0x4A6535-0x4A6545 (fstp var_4 store-round @0x4A6545)

    // divided by the speed at which that radius was calibrated - the 53-bit
    // quotient stays unrounded on st0 at retn (corollary (h))
    return (double)product / master->vfptr->getTurnRadiusSpeed(master);   // asm 0x4A6549-0x4A654C (fdivr: product / speed)
}

// Replacement for RGE_Moving_Object::calculateYawAngle. Original:
// SEMANTIC: the yaw STEP toward the new orientation - the angle between the current and desired forward vectors, clamped to the master's maximum yaw rate for this frame interval; the cross product's vertical decides the turn direction; limitedAngle reports whether the step had to be clamped.
// @ 0x004A6350 (idb_functions.h:2199). Range [0x4A6350, 0x4A63F0)
// (retn 10h = 4 stack args), returns double. Decode (asm 0x4A6350-
// 0x4A63EF): v = angleBetweenVector(newOF, curOF) (:5791, asm
// 0x4A6363) -> fstp v STORE-ROUND (asm 0x4A636B - v is float); yaw =
// getMaximumYawPerSecondMoving (vtbl slot +0x6C = 27, asm 0x4A6371,
// :3612 double-typed; in the Static vtbl, no cast) * interval (fmul
// on raw st0, asm 0x4A6374 - 53-bit, NO store); IF v <= yaw (fld v
// m32; fcomp st(1) - rounded-angle vs the 53-bit product, test ah,41h,
// asm 0x4A6378-0x4A6383): limitedAngle = 0 (asm 0x4A63A8-region; the
// product popped, no store); ELSE: v < 0 -> fchs (asm 0x4A6396; the
// C0-only test = comiss/jb incl. NaN), v = (float)yaw (fstp v STORE-
// ROUND, asm 0x4A639C - v IS float in the original), limitedAngle = 1
// (asm 0x4A63A0); BVector__operator_mul(newOF, &result, curOF) (:5789,
// asm 0x4A63B4-0x4A63BC); result.z <= 0.0 (fld m32; fcomp
// const_f_zero; test ah,41h, asm 0x4A63C1-0x4A63D3 - direct float
// compare, rule (b)) -> return v : return -v (both exits load the m32
// v, exact widening - asm 0x4A63D5-0x4A63E1).
double __fastcall RGE_Moving_Object__calculateYawAngle_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* curOF, BVector* newOF,
    float interval, int* limitedAngle)
{
    // the angle between the desired and current forward vectors - the
    // original store-rounds it to m32 immediately (the explicit cast keeps
    // that rounding point under the double-typed constant)
    float v = (float)BVector__angleBetweenVector(newOF, curOF);   // asm 0x4A6363-0x4A636B (:5791; fstp v store-round @0x4A636B)

    // the maximum turn the master allows this frame: yaw rate x the frame
    // interval - the fmul lands directly on the slot call's st0 (53-bit,
    // no store)
    double yaw = this_->master_obj->vfptr->getMaximumYawPerSecondMoving(this_->master_obj)
        * (double)interval;                                  // asm 0x4A6371-0x4A6374 (slot +0x6C)

    // the step fits? (the rounded angle vs the 53-bit allowance). The x87
    // idiom is test ah,41h; jnz - C0|C3 = "< OR unordered" -> the branch is
    // UNORDERED-TRUE (NaN v takes the no-clamp path). MSVC's SSE codegen for
    // C's <= is unordered-FALSE, so the faithful form is the negated
    // opposite (compiles comisd + jna/jbe = unordered-taken) - precision
    // sub-rule (i); the observed desync was exactly this class (NaN newOF)
    if (X87_LE_NAN(v, yaw))                                  // asm 0x4A6378-0x4A6383 (fld v; fcomp st(1); test ah,41h - {<=, NaN}; the macro = the sub-rule (i) fix)
    {
        *limitedAngle = 0;                                   // no clamp needed; the allowance is popped unused (asm 0x4A63A8-region)
    }
    else
    {
        // the angle reads negative when the turn is clockwise - flip the
        // allowance's sign to match. The x87 test ah,1 is C0-ONLY = "< OR
        // unordered" -> the negate fires for NaN too; the faithful form is
        // the negated >= (precision sub-rule (i))
        if (X87_LT_NAN(v, 0.0f))                             // asm 0x4A6385-0x4A6394 (test ah,1 - {<, NaN}; the macro = the sub-rule (i) fix)
            yaw = -yaw;                                      // asm 0x4A6396 (fchs on the 53-bit product)

        // the clamped step, rounded to m32 exactly as the original stores it
        v = (float)yaw;                                      // asm 0x4A639C (fstp v store-round - v IS float)
        *limitedAngle = 1;                                   // asm 0x4A63A0
    }

    // the cross product of desired x current: its VERTICAL (result.z) picks
    // the turn direction
    BVector result;                                          // asm [esp+result] (-0Ch)
    BVector__operator_mul(newOF, &result, curOF);            // asm 0x4A63B4-0x4A63BC (:5789)

    // vertical up -> turn one way, down (or flat) -> the other; both exits
    // load the m32 v - exact widening to the double return. The x87 gate is
    // test ah,41h; jnz = UNORDERED-TRUE (NaN result.z -> +v; the observed
    // desync: NaN newOF made result.z NaN and the C <= took -v, flipping the
    // turn direction) - the negated > is the faithful form (sub-rule (i))
    if (X87_LE_NAN(result.z, 0.0f))                          // asm 0x4A63C1-0x4A63D3 (fld m32; fcomp const_f_zero; test ah,41h - {<=, NaN}; the macro = the sub-rule (i) fix - the observed desync: NaN newOF took -v, flipping the turn direction)
        return v;                                            // asm 0x4A63E1 path (fld v m32)
    return -v;                                               // asm 0x4A63D5-0x4A63DE (fld v; fchs)
}

// Replacement for RGE_Moving_Object::evacuate_area. Original:
// SEMANTIC: repositions the unit OUT of a placement rectangle - if the unit is standing still and the area scan finds a free tile, it is walked to that tile's centre (+0.5s) at its current height.
// @ 0x004A6BA0 (idb_functions.h:2209). Range [0x4A6BA0, 0x4A6C1B),
// void, retn 10h = 4 stack args (int x1, int y1, int x2, int y2 - the
// raw-dword consumption at asm 0x4A6BC2-0x4A6BDF confirms the int
// rendering for THIS function; the Building check_placement_for_units
// caller computes the bounds as floats and passes the bits). Decode
// (asm 0x4A6BA0-0x4A6C1B): ex = 0 (asm 0x4A6BA6), ey = 0 (the .c's
// "float v8" slot - an IDA artifact: find_evacuate_area writes the
// int tile coord through the int* out-param and evacuate_area reads
// it back with fild); IF !vfptr->is_moving(this) (slot +0x144, asm
// 0x4A6BB8, test al,al): IF find_evacuate_area (:2210, the four
// raw-dword forwards + &ex/&ey): move_to (slot +0x118, asm 0x4A6C11;
// RGE_Static_Object* = 0, x = ex+0.5, y = ey+0.5, z = world_z @+0x50)
// - the two fild/fadd const_f_half/fstp pairs (asm 0x4A6BEB-0x4A6C0C)
// are m32 STORE-ROUNDS, replicated by the explicit (float) casts.
// The two bare "push ecx" sites in the listing (0x4A6BF5/0x4A6C03)
// are the known IDA esp-frame derailment - the .c render + the x87
// block fix the argument values exactly.
void __fastcall RGE_Moving_Object__evacuate_area_new(
    RGE_Moving_Object* this_, DWORD dummy, int x1, int y1, int x2, int y2)
{
    // the scan's out-params: the found tile (ex/ey; the .c's "float v8" slot
    // for ey is an IDA artifact - find_evacuate_area writes the int tile
    // through the int* and evacuate_area reads it back with fild)
    int ex = 0;                                              // asm 0x4A6BA6
    int ey = 0;                                              // asm 0x4A6BAE (var_8; see the decode note)

    // only a unit that is NOT moving needs evacuation (slot +0x144)
    if (!this_->vfptr->is_moving(this_))                     // asm 0x4A6BB6-0x4A6BC0
    {
        // scan the rectangle for a free tile
        if (RGE_Moving_Object__find_evacuate_area(
                this_, x1, y1, x2, y2, &ex, &ey))            // asm 0x4A6BC2-0x4A6BE2 (:2210 - the four raw-dword forwards)
        {
            // walk to the found tile's CENTRE (int + 0.5, each an m32
            // store-round - rule (e)), keeping the current height
            this_->vfptr->move_to(this_, 0,
                (float)((double)ex + 0.5),                   // asm 0x4A6BFF-0x4A6C0C (fild/fadd/fstp - m32 store-round)
                (float)((double)ey + 0.5),                   // asm 0x4A6BEB-0x4A6BFC (fild/fadd/fstp - m32 store-round)
                this_->world_z);                             // asm 0x4A6BEF ([esi+50h]; call [edx+118h] @0x4A6C11 - slot +0x118)
        }
    }
}

// Replacement for RGE_Moving_Object::find_evacuate_area. Original:
// SEMANTIC: the evacuation scan - two walkers do a Bresenham perimeter walk around the placement rectangle (opposite halves), probing every tile for evacuability and keeping the WALKABLE candidate nearest to the unit (distance-scored, 999 seed); returns whether any was found, via the ex/ey out-params. The loop bound is the perimeter step budget; the walkers-meet early exit reads never-initialized mirror slots in the original (the OOS the baked EVAC_OOS_FIX addresses below).
// @ 0x004A6C20 (idb_functions.h:2210). Range [0x4A6C20, 0x4A6ECE),
// retn 18h = 6 stack args, u8 return. Decode (asm 0x4A6C20-0x4A6ECE):
// the Bresenham perimeter walk - worldX/worldY = ftol(world_x
// @+0x48/world_y @+0x4C) (asm 0x4A6C28-0x4A6C36); the step budget =
// 2*(y2-y1) - x1 + x2 + 3 (asm 0x4A6C4D-0x4A6C74); the nearest-edge
// pick = (worldX - x1 <= x2 - worldX) ? 0 : 2 (setle/dec/and 2, asm
// 0x4A6C7C-0x4A6C85); the initial probe tile (pick 0 -> (x1-1,
// worldY), pick 2 -> (x2+1, worldY) - the .c's switch cases 1/3 are
// unreachable from the pick domain, asm off_4A6ED4); bestDist seeded
// 999.0f (asm 0x4A6C66, the float bits as an immediate); the initial
// try_evac_tile probe + the ex/ey/bestDist/found stores; then the
// TWO-WALKER loop (dir[2] seeded {0,1} or {2,3} per the pick, the
// walker index flips per iteration, setnz asm 0x4A6E9B-0x4A6EA1):
// the 4-case direction state machine (asm 0x4A6D8F-0x4A6E2B, the
// corner turns storing 0/1/2/3), the try_evac_tile probe, the score
// = distance_to_position(x+0.5, y+0.5, 0) with the two fild/fadd/
// fstp m32 store-rounds (asm 0x4A6E4C-0x4A6E69, rule (e)), the
// compare vs the m32-rounded bestDist (fcom, asm 0x4A6E71) and the
// update store-round (fstp, asm 0x4A6E84).
// BAKED (EVAC_OOS_FIX): playeroptions.cpp:206 setHook(0x004A6E95,
// on_evac_area) - a MID-FUNCTION entry hook at the walker-flip/exit
// join (the call site retired in place). The stub gates on the
// owner's option byte ([owner+0x21], the playeroptions
// CURRENT_PLAYER_OPTION_LEVEL byte): == 0x0B -> the ORIGINAL
// behavior (the stub replays the overwritten instructions: ecx =
// var_C, the flip, resume 0x4A6EA3); != 0x0B -> the flip then
// budget-only continuation (the stub: budget > 0 -> jmp 0x4A6D60 the
// loop head; else the replicated epilogue: al = var_31, pops, retn
// 18h). THE REASON (owner hint, confirmed): the original's loop-exit
// compares read the NEVER-STORED mirror slots (the .c's v38[2]/
// v37[2] = asm var_4/var_10 - no stores in the range) = per-machine
// stack garbage -> per-client walk divergence -> the OOS the patch
// fixes. The mirrors are DETERMINISTICALLY INITIALIZED (0) per the
// owner direction 2026-11-17 - the constant is tunable (future work:
// identify which constants reproduce original outcomes most often);
// the 0x0B legacy path reads them in the original's shape.
// RESTRUCTURE (owner-directed 2026-11-17): the asm's while(1) with
// the top decrement + the bottom exits is written as a for loop -
// the budget decrement is the pure loop count (the original executes
// exactly `budget` iterations: decrement top, probe, exit when
// post-decrement value <= 0), the bottom budget>0 continue is the
// for continuation condition, the legacy walker-meet exits map to
// break; probe order and value flow identical per iteration. The
// loop re-reads x1/y1/x2/y2 from the stack each iteration (asm
// 0x4A6D60 reloads ebp = x1) - the written form uses the params
// directly (no clobber in C - identical). Single retn site
// 0x4A6ECE (the loop exit and the fall-through converge).
unsigned __int8 __fastcall RGE_Moving_Object__find_evacuate_area_new(
    RGE_Moving_Object* this_, DWORD dummy, int x1, int y1, int x2, int y2,
    int* ex, int* ey)
{
    // the unit's tile: the float world position truncated to the grid
    int worldX = (int)this_->world_x;                        // asm 0x4A6C28-0x4A6C31 (__ftol)
    int worldY = (int)this_->world_y;                        // asm 0x4A6C31-0x4A6C36 (__ftol)

    // the perimeter step budget: twice the rectangle height plus the width
    // plus the corner allowance - enough steps to circle the whole rim
    int budget = 2 * (y2 - y1) - x1 + x2 + 3;                // asm 0x4A6C4D-0x4A6C74

    // "no candidate yet" sentinel - any real tile centre scores below it
    float bestDist = 999.0f;                                 // asm 0x4A6C66 (the float bits as an immediate)
    unsigned __int8 found = 0;                               // asm 0x4A6C53 (var_31 = 0)

    // which vertical rim is closer to the unit - the walk starts on that side
    int pick = (worldX - x1 <= x2 - worldX) ? 0 : 2;         // asm 0x4A6C78-0x4A6C85 (setle/dec/and 2)

    // the two walkers' probe tiles
    int probeX[2];                                           // the .c's v38 (asm var_C/var_4)
    int probeY[2];                                           // the .c's v37 (asm var_18/var_10)

    // walker 0 starts one tile OUTSIDE the near rim, on the unit's row
    if (pick == 0)
    {
        probeY[0] = worldY;
        probeX[0] = x1 - 1;                                  // asm case 0 (off_4A6ED4)
    }
    else
    {
        probeY[0] = worldY;
        probeX[0] = x2 + 1;                                  // asm 0x4A6C8E-0x4A6C9B (loc_4A6C8E)
    }

    // if the rim tile beside the unit is already free, it's the answer
    if (RGE_Moving_Object__try_evac_tile(
            (RGE_Moving_Object*)this_, probeX[0], probeY[0]))   // asm 0x4A6CC6-region (:2211 - HOOKED this batch -> routes)
    {
        *ex = probeX[0];
        *ey = probeY[0];

        // scored by the planar distance from the tile CENTRE (grid + 0.5):
        // the centres are m32 store-rounded here (rule (e)); the distance
        // result stays on st0 at 53-bit (the st0-boundary capture)
        bestDist = (float)RGE_Static_Object__distance_to_position(
            (RGE_Static_Object*)this_,
            (float)((double)*ex + 0.5),
            (float)((double)*ey + 0.5),
            0.0f);                                           // the two fild/fadd/fstp store-rounds
        found = 1;
    }

    // walker 1 starts where walker 0 does - they then circle opposite halves
    probeX[1] = probeX[0];                                   // asm (v38[1] = v12)
    probeY[1] = probeY[0];                                   // asm (v37[1] = v37[0])

    // one walks the rim upward, the other downward (mirrored for the far rim)
    int dir[2];
    if (pick == 2 || pick == 0)
    {
        dir[0] = 0;
        dir[1] = 1;
    }
    else
    {
        dir[0] = 2;
        dir[1] = 3;
    }

    // which walker takes the current step
    int w = 0;                                               // asm 0x4A6D5C (xor esi,esi)

    // the .c's v38[2]/v37[2] (asm var_4/var_10): NEVER stored in the original,
    // so the walkers-met test below reads per-machine stack garbage - the OOS
    // this function's bake fixes. Deterministic zero init per owner direction
    // 2026-11-17; the constant is tunable.
    int mirrorX = 0;
    int mirrorY = 0;

    // exactly `budget` iterations: the original decrements at the top and
    // exits when the post-decrement value is spent (RESTRUCTURE, see header)
    for (int step = 0; step < budget; ++step)
    {
        // step along the current rim; the 4-direction machine turns at corners
        switch (dir[w])                                      // asm jmp ds:off_4A6EE4[eax*4] @0x4A6D88
        {
        case 0:                                              // up: stop at the top rim, then head horizontally
            if (probeY[w] <= y1)
                goto turn_x;
            --probeY[w];                                     // asm 0x4A6D9B-0x4A6D9C
            break;

        case 1:                                              // down: stop at the bottom rim, then horizontal
            if (probeY[w] < y2)
            {
                ++probeY[w];                                 // asm 0x4A6DCF-0x4A6DD0
            }
            else
            {
            turn_x:                                          // asm loc_4A6DB1 (the LABEL_17 join): the horizontal leg -
                                                             // away from the rectangle if outside it, toward it if inside -
                                                             // then onto the next rim
                if (probeX[w] > x1)
                {
                    --probeX[w];                             // asm 0x4A6DC4-0x4A6DC5
                    dir[w] = 2;                              // asm mov [var_20], edx (= 2)
                }
                else
                {
                    ++probeX[w];                             // asm 0x4A6DB9
                    dir[w] = 3;                              // asm mov [var_20], ecx (= 3)
                }
            }
            break;

        case 2:                                              // left along the rim: at the left edge, turn up or down onto the next rim
            if (probeX[w] > x1)
            {
                --probeX[w];                                 // asm 0x4A6E04
            }
            else if (probeY[w] > y1)
            {
                --probeY[w];                                 // asm 0x4A6DF5 (the LABEL_25 join)
                dir[w] = 0;                                  // asm mov [var_20], 0
            }
            else
            {
                ++probeY[w];                                 // asm 0x4A6DEA
                dir[w] = 1;                                  // asm mov [var_20], edi (= 1)
            }
            break;

        default:                                             // right: mirror of case 2 (asm loc_4A6E07)
            if (probeX[w] < x2)
            {
                ++probeX[w];                                 // asm 0x4A6E2A
            }
            else if (probeY[w] > y1)                         // asm 0x4A6E13-0x4A6E1D (jg -> the LABEL_25 join)
            {
                --probeY[w];
                dir[w] = 0;
            }
            else
            {
                ++probeY[w];
                dir[w] = 1;
            }
            break;
        }

        // the stepped tile
        int py = probeY[w];                                  // asm 0x4A6E2F (edi)
        int px = probeX[w];                                  // asm 0x4A6E33 (ebp)

        // free AND pathable? score the tile centre by planar distance to the
        // unit; the nearest candidate wins. The centres are m32 store-rounded
        // (rule (e)); the score stays on st0 at 53-bit for the compare
        // (asm 0x4A6E71) and the update store-rounds it back to m32.
        // The x87 update branch is test ah,1 FALL-THROUGH = "< OR unordered"
        // - a NaN score UPDATES in the original (bestDist becomes NaN, the
        // tile is taken); the literal C < is unordered-false and would skip -
        // the faithful form is the negated >= (precision sub-rule (i))
        if (RGE_Moving_Object__try_evac_tile(
                (RGE_Moving_Object*)this_, px, py))          // asm 0x4A6E43 (:2211 - HOOKED -> routes)
        {
            double score = RGE_Static_Object__distance_to_position(
                (RGE_Static_Object*)this_,
                (float)((double)px + 0.5),                   // asm 0x4A6E5C-0x4A6E69 (fild/fadd/fstp store-round)
                (float)((double)py + 0.5),                   // asm 0x4A6E4C-0x4A6E59 (fild/fadd/fstp store-round)
                0.0f);
            if (X87_LT_NAN(score, (double)bestDist))         // asm 0x4A6E71-0x4A6E7A (fcom var_30; test ah,1; jz -> the discard - the taken set is {>=, NaN}: a NaN score UPDATES, as the original)
            {
                bestDist = (float)score;                     // asm 0x4A6E84 (fstp var_30 store-round)
                *ex = px;                                    // asm 0x4A6E88
                *ey = py;                                    // asm 0x4A6E8A
                found = 1;                                   // asm 0x4A6E8C
            }
        }

        // hand the next step to the other walker
        w = !w;                                              // asm 0x4A6E9B-0x4A6EA1 (setnz)

        // baked EVAC_OOS_FIX (playeroptions.cpp:206 @0x004A6E95, retired in
        // place): unless this player runs the legacy option level, the walk is
        // bounded by the budget alone - the for test enforces it
        if (((unsigned __int8*)this_->owner)[0x21] != 0x0B)
            continue;

        // the 0x0B legacy path: the walkers-met test against the mirror slots
        // (the uninitialized reads the fix bypasses)
        if (probeX[0] == mirrorX || probeY[0] == mirrorY)    // asm 0x4A6EA3-0x4A6EB5
            break;
    }

    // found-or-not (single retn site 0x4A6ECE; the stub's budget-exhausted
    // path replicates this epilogue)
    return found;
}

// Replacement for RGE_Moving_Object::try_evac_tile. Original:
// SEMANTIC: is this tile a legal evacuation destination - in map bounds (the && gate is asm-exact), the master's placement rules pass for the unit standing at the tile centre, and the unit can actually PATH there from its current position (totalCanPath).
// @ 0x004A6F00 (idb_functions.h:2211). Range [0x4A6F00, 0x4A7000),
// retn 8, u8 return. Decode (asm 0x4A6F00-0x4A6FFF): the OOB gates
// are the asm-exact && shape - the width half only runs when x < 0
// (test eax/eax; jge skip; cmp map_width @+8; jl skip -> return 0,
// asm 0x4A6F07-0x4A6F25: dead-in-practice, FAITHFUL), height @+0xC
// the same (asm 0x4A6F28-0x4A6F47); owner @+0x18 -> world @+0x8C ->
// map @+0x34 (asm 0x4A6F0E-0x4A6F17); the probe floats are m32
// store-rounds (fild/fadd const_f_half/fstp, asm 0x4A6F4A-0x4A6F7D,
// rule (e)) - the .c's "float goal / int ya" slots are scratch reuse
// (the frame 'goal' BVector's .x slot holds y+0.5 for the
// check_placement arg; the 'y' local holds x+0.5 - no semantic swap:
// the totalCanPath goal block builds {x+0.5, 0, y+0.5} at
// [edx]/[edx+4]/[edx+8], asm 0x4A6FCC-0x4A6FDB); master
// check_placement slot +0x38 = 14 (asm 0x4A6F8A) with the raw
// float-bit args (owner, x+0.5, y+0.5, six 0s, 1, this, 1) - blocked
// -> return 0; start = positionValue @+0xF4 (asm 0x4A6FD5-0x4A6FED);
// totalCanPath (:2537, __fastcall) with targetID = -1 (or edx,-1,
// asm 0x4A6FF0), range = 0, pathDistance = 0, unobstructiblePlayerID/
// GroupID = -1, tryMidLevelPathing = 1, useRangeWithoutTarget = 0,
// useLowLevelHulledArea = 0.
unsigned __int8 __fastcall RGE_Moving_Object__try_evac_tile_new(
    RGE_Moving_Object* this_, DWORD dummy, int x, int y)
{
    // out-of-map guard: the && shape is asm-exact - the width half only runs
    // when x < 0, so the rejection is dead-in-practice (kept faithfully)
    if (x < 0 && x >= this_->owner->world->map->map_width)   // asm 0x4A6F07-0x4A6F25
        return 0;
    if (y < 0 && y >= this_->owner->world->map->map_height)  // asm 0x4A6F28-0x4A6F47 (height @+0xC)
        return 0;

    // the tile CENTRE floats, each an m32 store-round of int + 0.5 (rule (e));
    // the .c's "float goal / int ya" frame slots are scratch reuse - no swap
    float goal = (float)((double)y + 0.5);                   // asm 0x4A6F4A-0x4A6F68 (fild/fadd/fstp)
    float xa = (float)((double)x + 0.5);                     // asm 0x4A6F6C-0x4A6F7D (fild/fadd/fstp)

    // placement legality: the unit standing at the tile centre must pass the
    // master's placement rules (slot +0x38; the float args push as raw bits)
    if (this_->master_obj->vfptr->check_placement(           // asm 0x4A6F8A
            this_->master_obj,
            (RGE_Player*)this_->owner, xa, goal,             // dual-naming cast: the runtime field is TRIBE_Player*, the vtbl slot declares RGE_Player* (the M1/Combat(70) precedent)
            0, 0, 0, 0, 0, 0,
            1, (RGE_Static_Object*)this_, 1))
    {
        return 0;                                            // asm 0x4A6F91-0x4A6F97 - blocked: not an evacuation destination
    }

    // the pathability probe: goal = {x+0.5, 0, y+0.5} (the planar centre, zero
    // height), start = the unit's current position; totalCanPath decides
    BVector v5;                                              // asm [edx]/[edx+4]/[edx+8]
    v5.x = xa;                                               // asm 0x4A6FCC
    v5.z = 0.0f;                                             // asm 0x4A6FD2 (the QWORD render's zero dword)
    v5.y = goal;                                             // asm 0x4A6FDB
    return totalCanPath(this_, -1, this_->positionValue, v5, // asm 0x4A6FD5-0x4A6FF5 (:2537; start = positionValue @+0xF4)
        0.0f, 0, -1, -1, 1, 0, 0);
}

__declspec(naked) double __fastcall RGE_Moving_Object__calculateYawAngle_orig(
    RGE_Moving_Object* this_, DWORD dummy, BVector* curOF, BVector* newOF,
    float interval, int* limitedAngle)
{
    __asm
    {
        sub     esp, 0Ch
        push    ebx
        mov     ebx, [esp + 18h]
        mov     eax, 004A6358h
        jmp     eax
    }
}

FILE* f = NULL;

double __fastcall RGE_Moving_Object__calculateYawAngle_logger(
    RGE_Moving_Object* this_, DWORD dummy, BVector* curOF, BVector* newOF,
    float interval, int* limitedAngle)
{
    char b[0x100];
    sprintf(b, "getMaximumYawPerSecondMoving=%f curOF={%f, %f, %f}, newOF={%f, %f, %f}, interval=%f, ",
        this_->master_obj->vfptr->getMaximumYawPerSecondMoving(this_->master_obj),
        curOF->x, curOF->z, curOF->y, newOF->x, newOF->z, newOF->y,
        interval);
    double result = RGE_Moving_Object__calculateYawAngle_orig(this_, 0, curOF, newOF, interval, limitedAngle);
    sprintf(b + strlen(b), "limitedAngle=%d, result=%f\n", *limitedAngle, result);

    if (!f)
        f = fopen("val_log.txt", "at");

    fputs(b, f);
    //fclose(f);

    return result;
}

// Replacement for RGE_Moving_Object::handle_teleport_z_correction.
// Original: @ 0x004A1970 (idb_functions.h:2139). Range [0x4A1970,
// 0x4A19C6), returns double, retn 4. SEMANTIC: the teleport HEIGHT
// reconciler - runs the base correction against the ground and marks
// the unit on-ground when the correction agrees with the ground level
// (or when there is no tile to stand on).
// Decode (asm 0x4A1970-0x4A19C6): corrected = RGE_Static_Object__
// handle_teleport_z_correction (:3833, UNHOOKED -> routes via the
// double-typed constant, the 53-bit capture) with ground_z re-pushed
// (asm 0x4A1977); the ground-match gate is the x87 test ah,40h; jz ->
// ZERO idiom (asm 0x4A1985) - the on_ground=1 set is {=, NaN}
// (unordered-equal-TRUE, sub-rule (i) item 5) - X87_EQ_NAN is the
// faithful form (the literal C == is unordered-FALSE and would zero
// on_ground for NaN); tile @+0x28 == 0 -> on_ground = 0 (asm
// 0x4A1999-0x4A199E); return world_z @+0x50 (asm 0x4A19A4, fld m32 -
// the exact widening to the double return). Single retn site
// 0x4A19Ax (pop esi; retn 4).
double __fastcall RGE_Moving_Object__handle_teleport_z_correction_new(
    RGE_Moving_Object* this_, DWORD dummy, float ground_z)
{
    // the base correction (HOOKED -> routes; the st0 result captured at
    // 53-bit through the double-typed constant)
    double corrected = RGE_Static_Object__handle_teleport_z_correction(
        (RGE_Static_Object*)this_, ground_z);                // asm 0x4A1978 (:3833)

    // the ground-match: {=, NaN} -> on_ground (the x87 test ah,40h; jz ->
    // the zero path; the composed form is the {=, NaN}-true test)
    if (X87_EQ_NAN(corrected, ground_z))
        this_->on_ground = 1;                                // asm 0x4A198A (byte @+0xD0)
    else
        this_->on_ground = 0;                                // asm 0x4A1993 (cl = 0)

    // no tile beneath -> not on ground, whatever the correction said
    if (this_->tile == 0)                                    // asm 0x4A1999 (@+0x28)
        this_->on_ground = 0;                                // asm 0x4A199E

    return this_->world_z;                                   // asm 0x4A19A4 (fld [esi+50h] - exact widening)
}

// Replacement for RGE_Moving_Object::angle_to_object. Original:
// @ 0x004A1100 (idb_functions.h:2135). Range [0x4A1100, 0x4A119F),
// returns double, retn 0Ch = 3 stack args. SEMANTIC: the facing angle
// FROM this unit TO a target (or to a raw x/y when there is no
// target), expressed as a signed facet-multiple: the atan2 of the
// planar delta, wrapped into (-pi, pi], scaled by the sprite's facet
// density (facets x 1/2pi - flt_65C1A8 = 0.15915494); 0.0 when the
// sprite has no facets.
// Decode (asm 0x4A1100-0x4A119F): sprite @+0x1C; the facet guards are
// INT tests (facet_num < 8 -> the scale floor 8; !sprite ||
// facet_num <= 0 -> return 0.0 via fld const_f_zero, asm 0x4A119C);
// the planar delta: to_object ? (to->world_x/y @+0x48/+0x4C -
// this->world_x/y) : (x/y params - this->world_x/y) (asm
// 0x4A1130-0x4A1146); atan2 (the CRT double; asm 0x4A1148-region);
// the WRAP COMPARES are the x87 test ah,1; jz -> skip idioms - the
// taken set includes UNORDERED (sub-rule (i)): v7 < 0 -> +2pi
// (const_f_2pi, asm 0x4A1155-0x4A115A -> X87_LT_NAN), v8 < -pi
// (flt_65C1AC, asm 0x4A1181-0x4A1186 -> X87_LT_NAN); the middle
// v8 > pi compare is test ah,41h; jnz -> skip (asm 0x4A116E-0x4A1173)
// - the jnz targets the NEGATIVE path so the literal > MATCHES
// (polarity table); the scale: fild facet_num; fmul flt_65C1A8;
// fmulp st(1),st (asm 0x4A118C-0x4A1196) = v8 * (facet x 1/2pi) - the
// .c multiply order exact.
// DESYNC FIX (owner-approved 2026-11-18): the shipped form dropped the
// shared-tail dy subtraction - fsub [ecx+4Ch] @0x4A1146 is the CONVERGED
// tail of BOTH delta paths (jmp 0x4A1139 / fall-through) and the .c
// render's atan2(v6 - this->world_y, v5) has it - so atan2 received the
// ABSOLUTE y instead of the delta. Also all four constants are dd floats
// widened exactly by the x87 ops (const_f_2pi @0x65B388, flt_65B604
// @0x65B604, flt_65C1AC @0x65C1AC, flt_65C1A8 @0x65C1A8); the bare
// double literals differed by 2e-8..4e-8 (wrap boundaries + the facet
// scale) - now f-suffixed for the exact widening. Follow-up (same
// approval round): the deltas widen BEFORE the subtraction ((double)
// casts) - the x87 fsub computes at 53-bit and feeds fpatan unrounded;
// the bare float-float form compiled to subss+cvtps2pd (a float
// rounding the original never performed).
double __fastcall RGE_Moving_Object__angle_to_object_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Static_Object* to_object,
    float x, float y)
{
    RGE_Sprite* sprite = this_->sprite;                      // asm 0x4A1100-region
    int facets = sprite ? sprite->facet_num : 0;             // asm 0x4A1104-0x4A1115 (int tests)
    if (facets < 8)
        facets = 8;                                          // asm 0x4A1118-0x4A1128 (the scale floor)
    if (sprite == 0 || sprite->facet_num <= 0)
        return 0.0;                                          // asm 0x4A119C (fld const_f_zero)

    // the planar delta to the target (or to the raw fallback coords)
    double dx;
    double dy;
    if (to_object != 0)
    {
        dx = (double)to_object->world_x - (double)this_->world_x;   // asm 0x4A1130-0x4A1133 (fld m32; fsub m32 - 53-bit intermediates: widen-before-op)
        dy = (double)to_object->world_y - (double)this_->world_y;   // asm 0x4A1136 (fld) + 0x4A1146 (fsub [ecx+4Ch], shared tail - both paths converge via jmp 0x4A1139)
    }
    else
    {
        dx = (double)x - (double)this_->world_x;                    // asm 0x4A113B-0x4A113F
        dy = (double)y - (double)this_->world_y;                    // asm 0x4A1142 (fld) + 0x4A1146 (fsub [ecx+4Ch], shared tail)
    }

    // the wrapped angle: atan2 then the two x87 wrap compares - the taken
    // set includes unordered (test ah,1; jz -> skip): a NaN angle gets the
    // +2pi shifts in the original -> X87_LT_NAN
    double angle = atan2(dy, dx);                            // asm 0x4A1148
    if (X87_LT_NAN(angle, 0.0))
        angle += 6.2831855f;                                 // asm 0x4A1155-0x4A115A (fadd const_f_2pi @0x65B388, dd float)

    // relative to the unit's current facing
    double rel = angle - this_->angle;                       // asm 0x4A1160 (this->angle @+0xC8)

    // the middle wrap: test ah,41h; jnz -> SKIP the subtraction - the
    // jnz targets the skip path so the literal > MATCHES (polarity table)
    if (rel > 3.1415927f)
        rel -= 6.2831855f;                                   // asm 0x4A1173 (fsub const_f_2pi; flt_65B604 @0x65B604 = pi, dd float)

    // the second wrap: test ah,1; jz -> skip - unordered taken -> X87_LT_NAN
    if (X87_LT_NAN(rel, -3.1415927f))
        rel += 6.2831855f;                                   // asm 0x4A1181-0x4A1186 (fadd const_f_2pi; flt_65C1AC @0x65C1AC, dd float)

    // the facet multiple: rel x (facets / 2pi)
    return rel * ((double)facets * 0.15915494f);             // asm 0x4A118C-0x4A1196 (fild; fmul flt_65C1A8 @0x65C1A8, dd float; fmulp st(1),st)
}

// Replacement for RGE_Moving_Object::setVelocity. Original:
// @ 0x004A3870 (idb_functions.h:2175). Range [0x4A3870, 0x4A3920),
// void, retn 8. SEMANTIC: the full-vector velocity command - lazily
// creates the BMotionData when the unit is idle AND the commanded
// vector is a NONZERO LENGTH (the full planar+vertical norm, not the
// forward-only gate of setForwardVelocity), then copies the whole
// BVector. SEH allocation-unwind chunk 0x642860.
// Decode (asm 0x4A3870-0x4A3920): the norm gate sqrt(x^2 + z^2 + y^2)
// (asm 0x4A3897-0x4A38AF, fsqrt) rendered `!= 0.0` - the x87 idiom is
// the ORDERED-NONZERO test (unordered -> no create; a NaN vector does
// NOT allocate) -> X87_NE_ORD(v, 0.0) is the faithful form (the
// literal C != is unordered-TRUE and would allocate on NaN - probe:
// ne_f = 10, sub-rule (i)); operator_new(0x18) = sizeof(BMovementData)
// walked; the guarded ctor (0x004BBCF0); movementData = result; the
// 12-byte velocity copy (the .c's assignment).
// RESTRUCTURE (documented): the .c's `movementData || norm != 0 &&
// (create-embedded)` form is written as the nested if/else - path-exact
// (the store is shared between the pre-existing and the created paths;
// a NaN/zero norm skips both in the original).
void __fastcall RGE_Moving_Object__setVelocity_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* v)
{
    BMovementData* new_data = 0;   // asm var_4 - the SEH-freed slot (0-init)
    __try
    {
        // the norm gate: the original's `!= 0.0` render is the
        // ordered-nonzero test - NaN vectors do NOT create the block
        if (this_->movementData != 0
            || X87_NE_ORD(sqrt((double)v->x * (double)v->x
                + (double)v->z * (double)v->z
                + (double)v->y * (double)v->y), 0.0))        // asm 0x4A3897-0x4A38B0+ (fsqrt; the ordered-nonzero test)
        {
            if (this_->movementData == 0)
            {
                new_data = (BMovementData*)operator_new_internal(sizeof(BMovementData));   // asm push 18h
                if (new_data != 0)
                    new_data = BMovementData__BMovementData(new_data);   // asm (ctor 0x004BBCF0)
                this_->movementData = new_data;
            }
            if (this_->movementData != 0)
                this_->movementData->velocity = *v;          // the 12-byte copy
        }
    }
    __except (operator_delete_internal(new_data), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642860
    {
    }
}

// Replacement for RGE_Moving_Object::setAcceleration. Original:
// @ 0x004A3960 (idb_functions.h:2177). Range [0x4A3960, 0x4A3A10),
// void, retn 8. SEMANTIC: the full-vector acceleration command - the
// setVelocity shape storing into BMotionData::acceleration. SEH
// allocation-unwind chunk 0x642880.
void __fastcall RGE_Moving_Object__setAcceleration_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* v)
{
    BMovementData* new_data = 0;   // asm var_4 - the SEH-freed slot (0-init)
    __try
    {
        if (this_->movementData != 0
            || X87_NE_ORD(sqrt((double)v->x * (double)v->x
                + (double)v->z * (double)v->z
                + (double)v->y * (double)v->y), 0.0))        // asm (fsqrt; the ordered-nonzero test)
        {
            if (this_->movementData == 0)
            {
                new_data = (BMovementData*)operator_new_internal(sizeof(BMovementData));
                if (new_data != 0)
                    new_data = BMovementData__BMovementData(new_data);
                this_->movementData = new_data;
            }
            if (this_->movementData != 0)
                this_->movementData->acceleration = *v;      // the 12-byte copy
        }
    }
    __except (operator_delete_internal(new_data), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642880
    {
    }
}

// Replacement for RGE_Moving_Object::calculateForward. Original:
// @ 0x004A6290 (idb_functions.h:2198). Range [0x4A6290, 0x4A62E9+),
// retn 0Ch = 3 stack args, int (always 1). SEMANTIC: computes the
// movement forward vector from start to destination - when the two
// points coincide the unit keeps its current orientation; otherwise
// the delta is normalized to unit length (the PLANAR normalize: sqrt
// over .x/.y only, the vertical .z zeroed by the 0/v5 term).
// Decode (asm 0x4A6290-0x4A62E6): the three-component equality chain
// is the x87 test ah,40h; jz -> normalize idiom x3 (asm 0x4A62A0-62C1)
// - the taken set per component is {=, NaN} (sub-rule (i) item 5):
// ANY NaN component takes the equal path in the original -> the
// X87_EQ_NAN composed form per component; the equal path:
// *newOF = orientationForwardValue @+0x100 (asm 0x4A62C3-0x4A62DB, the
// 12-byte copy) + return 1 (asm 0x4A62DE, retn 0Ch 0x4A62E6); the
// normalize path: BVector__operator_minus (this = destination; the
// .c's `operator_minus(destination, &result, start)`, asm 0x4A62E9-62EF)
// -> *newOF = result -> the planar sqrt normalize (the .c:
// sqrt(x*x + y*y); x/v5; z = 0/v5; y/v5) -> return 1.
int __fastcall RGE_Moving_Object__calculateForward_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* start, BVector* destination,
    BVector* newOF)
{
    // the coincidence test: ANY NaN component reads as "equal" in the
    // original (the x87 test ah,40h idiom - {=, NaN} taken) - the unit
    // keeps its current orientation
    if (X87_EQ_NAN(start->x, destination->x)
        && X87_EQ_NAN(start->z, destination->z)
        && X87_EQ_NAN(start->y, destination->y))             // asm 0x4A62A0-0x4A62C1 (test ah,40h; jz x3)
    {
        *newOF = this_->orientationForwardValue;             // asm 0x4A62C3-0x4A62DB (@+0x100, the 12-byte copy)
        return 1;                                            // asm 0x4A62DE
    }

    // the delta + the planar normalize
    BVector result;
    BVector__operator_minus(destination, &result, start);    // asm 0x4A62E9-0x4A62EF (this = destination)
    *newOF = result;                                         // the .c's *newOF = result
    double len = sqrt((double)newOF->x * (double)newOF->x
        + (double)newOF->y * (double)newOF->y);              // the PLANAR norm (x + y per the .c)
    newOF->x = (float)((double)newOF->x / len);              // asm (fld/fdiv)
    newOF->z = (float)(0.0 / len);                           // asm - the vertical zeroed via the divide (the .c's 0.0 / v5)
    newOF->y = (float)((double)newOF->y / len);              // asm
    return 1;                                                // asm mov eax, 1
}

// Replacement for RGE_Moving_Object::straightProjectedPosition. Original:
// @ 0x004A6860 (idb_functions.h:2203). Range [0x4A6860, 0x4A697C),
// retn 0Ch, returns BVector*. SEMANTIC: where the unit would be if it
// kept accelerating along newOF for the given seconds, speed-capped.
// Decode (asm 0x4A6860-0x4A697C): no movementData -> *result =
// positionValue (asm 0x4A6874-0x4A688A, add esi,0xF4; the 3-dword copy;
// retn 0Ch 0x4A6890); else resulta = acceleration x seconds (the SCALAR
// operator_mul overload - un-exported free function, call operator_mul
// asm 0x4A68A2 with ECX = sret - the moveForward inline precedent
// applies: the 3-multiplies are the faithful form); v10 = velocity +
// resulta (BVector__operator_plus :5786, asm 0x4A68B3, this =
// movementData); the cap: the unit's tile via ftol(positionValue.y) =
// the row / ftol(positionValue.x) = the col (asm 0x4A68D6/0x4A68E9),
// map_row_offset[row] (asm 0x4A68DD/0x4A68EE, the real RGE_Map field
// @0xBF18) -> terrain_type @+5 (asm mov dl,[ecx+eax+5]; shl eax,5 =
// the 32-byte tile stride) -> the calculateMaximumSpeed dispatch
// call [eax+31Ch] (slot 199 = our hook, asm 0x4A6902); the clamp is
// the two-gate x87 form (asm 0x4A6908-0x4A692F): gate 1 test ah,41h;
// jz -> clamp (ordered && v10.y > cap), gate 2 test ah,41h; jz -> keep
// (ordered && v10.y > 0) - the fall-throughs (incl. NaN) CLAMP - the
// literal-composite form below is polarity-exact (the polarity-table
// MATCH row: both jz target the negative/keep paths; the .c's
// `v10.y > v8 || v10.y <= 0.0` clamp form DIVERGES on NaN - it would
// keep a NaN speed; the written composite follows the asm); a = the
// clamped speed x seconds (asm 0x4A6931-0x4A693D, the product
// store-rounded by fstp m32 @0x4A693D - replicated by the float
// local); the advance: newOF x a (the scalar overload inlined,
// asm 0x4A694A) -> operator_plus(positionValue, &v11, ...) (asm
// 0x4A6959, :5786) -> *result = v11 (the 3-dword copy, asm
// 0x4A695E-0x4A6976).
// AUDIT FIX (owner-approved 2026-11-18): the advance multiplies target
// the asm's sret LOCAL (-0xC), not the caller's *result - the direct
// form clobbers *result before operator_plus reads positionValue when
// a caller aliases result = &positionValue (the function has NO direct
// .c callers - vtable dispatch only - so aliasing callers are
// unprovable and the asm's safe shape is mandatory). Range corrected
// 0x4A6980 -> 0x4A697C (the endp; 0x4A6980 is the next proc's align).
// CLAMP FIX (same approval round): the composite
// `!(v > 0.0f && v <= cap)` nested the cap compare into the keep
// condition - on a NaN cap with ordered v > 0 the asm's gate 2 (which
// tests v against 0.0 only) KEEPS the speed while the nested form
// CLAMPED to NaN - rewritten as the cap-first chain
// `v > cap || X87_LE_NAN(v, 0.0f)` matching the gate order (the
// 2026-11-19 macro-mandate sweep converted the interim negated form
// to the macro; gate 1 stays the literal > MATCH row).
BVector* __fastcall RGE_Moving_Object__straightProjectedPosition_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* result, BVector* newOF,
    float secondsInTheFuture)
{
    if (this_->movementData == 0)
    {
        *result = this_->positionValue;
        return result;
    }

    // resulta = acceleration x seconds (the scalar overload inlined)
    BVector resulta;
    resulta.x = this_->movementData->acceleration.x * secondsInTheFuture;
    resulta.z = this_->movementData->acceleration.z * secondsInTheFuture;
    resulta.y = this_->movementData->acceleration.y * secondsInTheFuture;

    // v10 = velocity + resulta
    BVector v10;
    BVector__operator_plus(&this_->movementData->velocity, &v10, &resulta);

    // the speed cap from the tile under the unit
    int row = (int)this_->positionValue.y;                   // the row axis (asm __ftol)
    int col = (int)this_->positionValue.x;                   // asm __ftol
    RGE_Map* map = this_->owner->world->map;
    unsigned __int8 terrain = map->map_row_offset[row][col].terrain_type;
    double cap = this_->vfptr->calculateMaximumSpeed(this_, terrain);

    // keep only an ordered speed: the asm CLAMPS when (ordered && v > cap)
    // (gate 1, jz @0x4A6916) or when NOT(ordered && v > 0) (gate 2, test
    // ah,41h; jz @0x4A6927 -> keep) - the gates are a CHAIN and gate 2
    // tests v against 0.0 only, so a NaN cap falls through BOTH and the
    // original KEEPS the speed: the composite below preserves that (an
    // earlier form nested the cap compare into the keep condition and
    // clamped on a NaN cap). Gate 1's jz targets the keep path = the
    // literal > MATCH row; gate 2's clamp set is {<=, NaN} = the macro.
    if (v10.y > cap || X87_LE_NAN(v10.y, 0.0f))
        v10.y = (float)cap;                              // asm 0x4A6929 (fstp m32 - the cap store-round)

    // a = the clamped speed x seconds
    float a = v10.y * secondsInTheFuture;

    // the position advance: newOF x a (the scalar overload inlined) - the
    // asm targets its sret LOCAL (-0xC), NOT the caller's *result: a
    // caller may alias result with positionValue, and the direct form
    // would clobber positionValue before operator_plus reads it
    BVector displacement;                                    // the asm's -0xC sret local (operator_mul sret @0x4A6946)
    displacement.x = newOF->x * a;
    displacement.z = newOF->z * a;
    displacement.y = newOF->y * a;

    // plus the unit's current position
    BVector advanced;
    BVector__operator_plus(&this_->positionValue, &advanced, &displacement);   // asm 0x4A6959 (var_18 = positionValue + displacement)
    *result = advanced;                                      // asm 0x4A695E-0x4A6976 (var_18 -> [result])
    return result;
}

// Replacement for RGE_Moving_Object::calculateReachedPoint. Original:
// @ 0x004A63F0 (idb_functions.h:2200). Range [0x4A63F0, 0x4A64FE),
// retn 0Ch, int. SEMANTIC: the reach test against the pos1->pos2
// segment - 1 = reached (or the segment is degenerate/negative), 0 =
// the segment is shorter than the remaining distance, 2 = en route.
// NOTE: BVector is {x, z, y} (idb_types.h:1761 - z @+4, y @+8; the
// vertical stored last; calibrated against operator_minus @0x606A60's
// write order and the var_24.z store at var_24+4) - so var_14 = .z and
// var_10 = .y in this function's .lst frame labels.
// Decode (asm 0x4A63F0-0x4A64FE): the three operator_minus deltas; the
// isFlagSet(3)/(4) gates (asm 0x4A6430/0x4A643D) zero toPoint2.z and
// the first length's vertical (the join value); the three sqrt lengths
// in the asm's operand orders (len1 = y^2+x^2+vz^2; len2 = y^2+x^2+z^2;
// segLen = (z^2+y^2)+x^2); the epsilon gate (fcom flt_6565F4 = dd
// 1.0e-7 @0x6565F4; test ah,1; jz -> continue, asm 0x4A649C-0x4A64A9)
// is UNORDERED-TAKEN - a NaN len2 returns 1 -> X87_LT_NAN; the segment
// gate (fcomp st(1); test ah,41h; jnz -> return 0, asm 0x4A64D8-0x4A64DF)
// is {<=, NaN}-taken -> X87_LE_NAN (a NaN len1 also returns 0); the
// final ladder's operands are ordered by then - the literal >= matches.
// DESYNC FIX (owner-approved 2026-11-18): (1) rule (j) - the bare
// float-field norm products compiled mulss/addss (24-bit roundings per
// product and per sum; len2/segLen summed entirely at float before
// sqrtsd) where the x87 chain is 53-bit end to end - every operand now
// widens BEFORE the multiply; (2) rule (e) - len2 is fst'd to m32
// @0x4A6498 (into the caller's pos1 slot, scratch reuse) and the FINAL
// compare consumes the ROUNDED value (fcomp [esp+24h+v] @0x4A64E1)
// while the epsilon gate consumes the unrounded st0 - the (float)
// len2_reached intermediate restores the store-round the transcription
// dropped (the .c's `v = v7; if (v6 >= v)` shows it); the segLen
// expression is aligned to the asm's (z^2+y^2)+x^2 order (commutative,
// cosmetic). Exit inventory: 3 asm return sites (loc_4A64AB eax=1 - a
// 2-inbound join per amendment A: the 0x4A64A9 fall-through AND jz
// @0x4A64EA, both written as return 1; loc_4A64EC eax=2; loc_4A64F7
// eax=0).
int __fastcall RGE_Moving_Object__calculateReachedPoint_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* point, BVector* pos1, BVector* pos2)
{
    BVector toPoint1;                                        // the .c's result (point - pos1)
    BVector toPoint2;                                        // the .c's v9 (point - pos2)
    BVector segment;                                         // the .c's v11 (pos2 - pos1)
    BVector__operator_minus(point, &toPoint1, pos1);         // :5787
    BVector__operator_minus(point, &toPoint2, pos2);
    BVector__operator_minus(pos2, &segment, pos1);

    // the flight-phase flatten: flags 3/4 zero the second delta's vertical
    // and the first length's vertical term
    double vz;
    if (RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 3)
        || RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 4))   // asm 0x4A6442
    {
        vz = 0.0;                                            // the .c's v5
        toPoint2.z = 0.0f;                                   // the .c's v9.z = 0
    }
    else
    {
        vz = toPoint1.z;                                     // the .c's v5 = result.z
    }

    double len1 = sqrt((double)toPoint1.y * toPoint1.y + (double)toPoint1.x * toPoint1.x + vz * vz);       // asm 0x4A645A-0x4A6476 (y^2 + x^2 + vz^2, all 53-bit)
    double len2 = sqrt((double)toPoint2.y * toPoint2.y + (double)toPoint2.x * toPoint2.x + (double)toPoint2.z * toPoint2.z);   // asm 0x4A647A-0x4A6496

    // the store-round: len2 is fst'd to m32 @0x4A6498 (into the caller's
    // pos1 slot - scratch reuse) and the FINAL compare consumes the
    // ROUNDED value (rule (e)); the epsilon gate consumes the unrounded st0
    float len2_reached = (float)len2;                        // asm 0x4A6498 (fst [esp+24h+v])

    // the epsilon gate: unordered-taken (a NaN len2 = "reached")
    if (X87_LT_NAN(len2, 0.0000001f))                        // asm 0x4A649C-0x4A64A9 (fcomp flt_6565F4 = dd 1.0e-7 @0x6565F4)
        return 1;

    // the segment shorter than the remaining distance -> not reached;
    // {<=, NaN}-taken (a NaN len1 or a NaN segment = 0)
    double segLen = sqrt((double)segment.z * segment.z + (double)segment.y * segment.y + (double)segment.x * segment.x);   // asm 0x4A64B6-0x4A64D2 ((z^2 + y^2) + x^2)
    if (X87_LE_NAN(segLen, len1))                            // asm 0x4A64D8-0x4A64DF (fcomp st(1); test ah,41h; jnz -> return 0)
        return 0;

    // the operands are ordered here (every NaN source is gated upstream)
    // and the len2 operand is the FLOAT-ROUNDED store - the literal >= MATCH
    if (len1 >= len2_reached)                                // asm 0x4A64E1-0x4A64EA (fcomp [esp+24h+v] - the ROUNDED operand)
        return 1;
    return 2;                                                // asm 0x4A64EC (mov eax,2)
}

// Replacement for RGE_Moving_Object::initMovementData. Original:
// @ 0x4A6A70 (idb_functions.h:2208). Range [0x4A6A70, 0x4A6B96+), void.
// SEMANTIC: the movement-state RESET - the terrain exceptions, the
// motion scalars, the orientation (forward restored to +X, right to -Y,
// the position zeroed), the patrol-information refresh, and the three
// heap frees.
// Decode (asm 0x4A6A70-0x4A6B96) - THE STORE-SET DIFF FOUND TWO ITEMS
// THE .C RENDER DROPS: (1) the vtable call slot +0x264 = 153 =
// removeAllUserDefinedWaypoints (asm 0x4A6AC7, HOOKED -> routes; the
// REAL Moving vftable initializer verified - slot 153 = removeAll...
// and 155 = getPatrolInformation; the idb struct member names match the
// real data slot-for-slot - the earlier "slot 153 = getPatrolInformation"
// was MY enumeration script's defect: its member regex missed the two
// pointer-return declarations at 150/155 and shifted the printed indices
// by +2); (2) orientationForwardValue.x = 1.0 (asm 0x4A6ACD-0x4A6AEB,
// the 1.0 float local stored to [esi+0x100]) - the .c's zero-list omits
// both.
// The store set: currentTerrainException2/1 = -1 (@+0xE0/0xDC); trail_
// remainder = 0 (@+0xB8); velocity_x/y/z = 0 (@+0xBC/0xC0/0xC4); angle
// = 0 (@+0xC8); turn_towards_time = 0 (@+0xCC); turnTimer = 0 (@+0xD4);
// continueCounter = 0 (@+0xD8); waitingToMoveValue = 0 (@+0xE4, byte);
// numberWaitDelays = 0 (@+0xE5, byte); forward.x = 1.0 (@+0x100),
// forward.z = 0 (@+0x104); right.x = 0 (@+0x10C), right.z = 0
// (@+0x110), right.y = -1.0 (@+0x114); position.x/z/y = 0 (@+0xF4/
// 0xF8/0xFC); the three frees: pathData (BPathData::destructor :2506 +
// delete -> internal, asm 0x4A6B31-0x4A6B43), mFuturePathData (asm
// 0x4A6B49-0x4A6B63), movementData (bare delete -> internal, asm
// 0x4A6B69-0x4A6B7C); lastMoveTimeValue = 0 (@+0x124); mHasSubstitute
// Position = 0 (@+0x128); mConsecutiveSubstituteCount = 0 (@+0x138).
// The remove-all call sits BEFORE the orientation block (the asm
// position preserved - a re-order would be observable through the hook).
void __fastcall RGE_Moving_Object__initMovementData_new(
    RGE_Moving_Object* this_, DWORD dummy)
{
    this_->currentTerrainException2 = -1;                // asm 0x4A6A7D (@+0xE0)
    this_->currentTerrainException1 = -1;                // asm 0x4A6A83 (@+0xDC)
    this_->trail_remainder = 0.0f;                       // asm 0x4A6A8B (@+0xB8)
    this_->velocity_x = 0.0f;                            // asm 0x4A6A91 (@+0xBC)
    this_->velocity_y = 0.0f;                            // asm 0x4A6A97 (@+0xC0)
    this_->velocity_z = 0.0f;                            // asm 0x4A6A9D (@+0xC4)
    this_->angle = 0.0f;                                 // asm 0x4A6AA3 (@+0xC8)
    this_->turn_towards_time = 0.0f;                     // asm 0x4A6AA9 (@+0xCC)
    this_->turnTimer = 0.0f;                             // asm 0x4A6AAF (@+0xD4)
    this_->continueCounter = 0;                          // asm 0x4A6AB5 (@+0xD8, int - the C4244 catch)
    this_->waitingToMoveValue = 0;                       // asm 0x4A6ABB (@+0xE4, byte)
    this_->numberWaitDelays = 0;                         // asm 0x4A6AC1 (@+0xE5, byte)

    // the user-defined waypoint wipe (slot +0x264 = 153 in the REAL Moving
    // vftable = removeAllUserDefinedWaypoints - verified against the
    // collapsed vftable initializer; HOOKED -> routes to removeAllUser
    // DefinedWaypoints_new; zero stack args, the no-push call is exact; the
    // .c render drops this call - the store-set diff caught it)
    RGE_Moving_Object__removeAllUserDefinedWaypoints(this_);   // asm 0x4A6AC7

    // the orientation reset: forward restored to +X (the .c render drops
    // the forward.x store - the store-set diff caught it), right to -Y
    this_->orientationForwardValue.x = 1.0f;             // asm 0x4A6AEB (@+0x100)
    this_->orientationForwardValue.z = 0.0f;             // asm 0x4A6AFB (@+0x104)
    this_->orientationRightValue.x = 0.0f;               // asm 0x4A6B11 (@+0x10C)
    this_->orientationRightValue.z = 0.0f;               // asm 0x4A6B23 (@+0x110)
    this_->orientationRightValue.y = -1.0f;              // asm 0x4A6B2E (@+0x114)
    this_->positionValue.x = 0.0f;                       // asm 0x4A6B06 (@+0xF4)
    this_->positionValue.z = 0.0f;                       // asm 0x4A6B17 (@+0xF8)
    this_->positionValue.y = 0.0f;                       // asm 0x4A6B1D (@+0xFC)

    // the three frees (the .c's bare operator_delete -> internal per the
    // CRT rule)
    if (this_->pathData != 0)                            // asm 0x4A6B04/0x4A6B31
    {
        BPathData__destructor(this_->pathData);          // :2506 (asm 0x4A6B35)
        operator_delete_internal(this_->pathData);       // asm 0x4A6B3B
        this_->pathData = 0;                             // asm 0x4A6B43 (@+0x118)
    }
    if (this_->mFuturePathData != 0)                     // asm 0x4A6B49/0x4A6B51
    {
        BPathData__destructor(this_->mFuturePathData);   // asm 0x4A6B55 (:2506)
        operator_delete_internal(this_->mFuturePathData);   // asm 0x4A6B5B
        this_->mFuturePathData = 0;                      // asm 0x4A6B63 (@+0x11C)
    }
    if (this_->movementData != 0)                        // asm 0x4A6B69/0x4A6B71
    {
        operator_delete_internal(this_->movementData);   // asm 0x4A6B74
        this_->movementData = 0;                         // asm 0x4A6B7C (@+0x120)
    }

    this_->lastMoveTimeValue = 0;                        // asm 0x4A6B82 (@+0x124)
    this_->mHasSubstitutePosition = 0;                   // asm 0x4A6B88 (@+0x128)
    this_->mConsecutiveSubstituteCount = 0;              // asm 0x4A6B8E (@+0x138)
}

// Replacement for RGE_Moving_Object::passableTile2. Original:
// @ 0x004A1B30 (idb_functions.h:2144). Range [0x4A1B30, 0x4A1C29),
// retn 8, int. SEMANTIC: the single-tile walkability probe - 1 = the
// tile is passable for this unit, 0 = blocked. The probe truncates the
// query coords to tile indices, selects the obstruction domain by the
// master's movement_type (flyers probe airspace), rejects out-of-bounds
// tiles, folds the tile-terrain mask against the hard-obstruction grid,
// arms the unit's own id as a one-entry ignore list, caps the query by
// the master's radius, and lets the collision query's hit count answer.
// Decode (asm 0x4A1B30-0x4A1C29): the ftol pair (asm 0x4A1B3A/0x4A1B45)
// - plain (int) casts per the conversion-type rule; the domain select
// (byte ptr [master+78h] == 1 -> OBSystem_Air, asm 0x4A1B55);
// the bounds gates (jl x / jl y / jge Width @+0xC / jge Height @+0x10,
// asm 0x4A1B61-0x4A1B7D -> loc_4A1C23 = the 5-edge return-0 join with
// the mask gate); GetTerrainMask (vtbl idx 7 @0x1C, idb_types.h:9952,
// asm 0x4A1B9B) on world->terrains[master->terrain] (owner @this+0x18
// -> world @+0x8C -> terrains @+0x60 (the RGE_Game_World walk);
// master->terrain i16 @+0x76 movsx, asm 0x4A1B88 - the census-
// calibrated walk); mask ^ 0xFF vs HardObArray[y][x] (@+0x20,
// array-null-guarded, asm 0x4A1B9E-0x4A1BB0, jnz -> return 0);
// SetBufferRadius(1, master->radius_x @+0x3C) (vtbl idx 23 @0x5C, asm
// 0x4A1BBF); SetIgnoreList(1, &scratch, 1) (vtbl idx 21 @0x54, asm
// 0x4A1BC5-0x4A1BD8) with the scratch = this->id - the global is
// LOAD-BEARING: SetIgnoreList stores the ids POINTER (the manager's
// IgnoreIDList @+0x9C) and neither function ever disarms, so a stack
// local would dangle into later queries; CheckForCollisions(x, y, 0,
// 0, 32, &num, &obs, 25, -2, 0, 0) (vtbl idx 19 @0x4C,
// idb_types.h:9964, asm 0x4A1BDB-0x4A1C01 - the 11-push order matches
// the vtbl signature arg-for-arg; 32/25/-2 UNKNOWN query parameters ->
// the magic-constant registry); SetBufferRadius(0, 0) disarm (asm
// 0x4A1C04-0x4A1C0C); return *numberObSystemCollisions <= 0 (setle,
// asm 0x4A1C0F-0x4A1C1D).
int __fastcall RGE_Moving_Object__passableTile2_new(
    RGE_Moving_Object* this_, DWORD dummy, float x, float y)
{
    // the obstruction domain: flyers probe the airspace manager - the
    // twin struct/vtbl are slot-identical (idb_types.h:5868/10146), so
    // the sideways cast is layout-proven at every accessed offset
    RGE_Obstruction_Manager* ob_system = *OBSystem_Land;          // common.cpp:14 (asm 0x4A1B4A loads the slot dword)
    if (this_->master_obj->movement_type == 1)                    // asm 0x4A1B55
        ob_system = (RGE_Obstruction_Manager*)*OBSystem_Air;      // common.cpp:15 (asm 0x4A1B5B)

    int tile_x = (int)x;                                          // asm 0x4A1B3A (ftol truncation)
    int tile_y = (int)y;                                          // asm 0x4A1B45

    if (tile_x < 0)                                               // asm 0x4A1B61-0x4A1B63
        return 0;                                                 // asm 0x4A1C23 (loc_4A1C23)
    if (tile_y < 0)                                               // asm 0x4A1B69-0x4A1B6B
        return 0;
    if (tile_x >= ob_system->Width)                               // asm 0x4A1B71-0x4A1B74 (@+0xC)
        return 0;
    if (tile_y >= ob_system->Height)                              // asm 0x4A1B7A-0x4A1B7D (@+0x10)
        return 0;

    float* terrain = this_->owner->world->terrains[this_->master_obj->terrain];   // asm 0x4A1B83-0x4A1B95
    int terrain_mask = ob_system->vfptr->GetTerrainMask(ob_system, terrain);      // asm 0x4A1B98-0x4A1B9B (vtbl idx 7)
    int inverted_mask = terrain_mask ^ 0xFF;                      // asm 0x4A1BA1

    if (ob_system->HardObArray != 0)                              // asm 0x4A1BA6
    {
        if ((ob_system->HardObArray[tile_y][tile_x] & inverted_mask) != 0)   // asm 0x4A1BAA-0x4A1BAD
            return 0;                                             // asm 0x4A1BB0 (jnz)
    }

    ob_system->vfptr->SetBufferRadius(ob_system, true, this_->master_obj->radius_x);   // asm 0x4A1BB2-0x4A1BBF (vtbl idx 23)
    *OBSystem_Ignore_Scratch_Tile2 = this_->id;                   // asm 0x4A1BC7 (the persistent scratch - see the decode note)
    ob_system->vfptr->SetIgnoreList(ob_system, true, OBSystem_Ignore_Scratch_Tile2, 1);   // asm 0x4A1BCD-0x4A1BD8 (vtbl idx 21)
    ob_system->vfptr->CheckForCollisions(                         // asm 0x4A1BDB-0x4A1C01 (vtbl idx 19)
        ob_system,
        x, y,                                                     // asm 0x4A1BDB/0x4A1BE1
        0.0f, 0.0f,
        32,                                                       // UNKNOWN query parameter (registry)
        numberObSystemCollisions,
        obSystemCollisions,                                       // the slot the callee writes the results base into (the missile precedent; the owner's ObsRecord** listing correction - the cast dropped)
        25,                                                       // UNKNOWN query parameter (registry)
        -2,
        0,
        0);
    ob_system->vfptr->SetBufferRadius(ob_system, false, 0.0f);    // asm 0x4A1C04-0x4A1C0C (the disarm)

    return *numberObSystemCollisions <= 0;                        // asm 0x4A1C0F-0x4A1C1D (setle)
}

// Replacement for RGE_Moving_Object::passableTile. Original:
// @ 0x004A1C30 (idb_functions.h:2145). Range [0x4A1C30, 0x4A1D42),
// retn 10h, int. DECODE PRIMARY: the .c render is flagged "positive sp
// value has been detected" - IDA's frame tracking is broken in this
// function (its v18-v26/retaddr labels are garbage); the decode is
// asm-primary with the raw esp arithmetic: 0x4A1CC6 = ignoreUnits
// (encoded 1Ch, esp = entry-10h), 0x4A1CCE = numberIgnoreUnits (20h),
// 0x4A1CF4 = y (-1Ch+arg_30 = 18h from entry-10h), 0x4A1CFA = x
// (-18h+arg_2C = 18h from entry-14h) - each verified against the call
// shapes. SEMANTIC: passableTile2 with a CALLER-SUPPLIED ignore list:
// when ignoreUnits != 0 and numberIgnoreUnits > 0 the caller's array is
// armed; otherwise the unit's own id goes into the second persistent
// scratch global (0x6AD0AC). Everything else is passableTile2's ladder
// (the asm is a byte-level clone through the mask gate at 0x4A1C30-
// 0x4A1CB0, then diverges at the ignore-list block).
// Decode (asm 0x4A1C30-0x4A1D42): the ftol pair 0x4A1C3A/0x4A1C45; the
// domain select 0x4A1C55; the bounds gates 0x4A1C61-0x4A1C7D; the
// terrain mask 0x4A1C83-0x4A1CB0 (GetTerrainMask idx 7 @0x1C; the
// HardObArray gate jnz -> loc_4A1D3C); SetBufferRadius(1, radius_x)
// 0x4A1CB6-0x4A1CC3 (idx 23); the CONDITIONAL ignore list 0x4A1CC6-
// 0x4A1CF1: caller list when (ignoreUnits != 0 && numberIgnoreUnits >
// 0) - two tests jz/jle @0x4A1CCC/0x4A1CD4 - else the scratch fallback
// (dword_6AD0AC = this->id @0x4A1CE1); both paths join @0x4A1CED (the
// 2-inbound join per amendment A: the jmp @0x4A1CDA and the fallback
// fall-through) on push 1 + SetIgnoreList (idx 21 @0x54); the query
// 0x4A1CF4-0x4A1D1A (idx 19 - the y/x loads carry the frame-confusion
// annotations but resolve to (x, y) as in passableTile2); SetBuffer
// Radius(0, 0) 0x4A1D1D-0x4A1D25; return *numberObSystemCollisions <= 0
// (setle 0x4A1D28-0x4A1D36).
int __fastcall RGE_Moving_Object__passableTile_new(
    RGE_Moving_Object* this_, DWORD dummy, float x, float y,
    int* ignoreUnits, int numberIgnoreUnits)
{
    RGE_Obstruction_Manager* ob_system = *OBSystem_Land;          // common.cpp:14 (asm 0x4A1C4A)
    if (this_->master_obj->movement_type == 1)                    // asm 0x4A1C55
        ob_system = (RGE_Obstruction_Manager*)*OBSystem_Air;      // common.cpp:15 (asm 0x4A1C5B)

    int tile_x = (int)x;                                          // asm 0x4A1C3A
    int tile_y = (int)y;                                          // asm 0x4A1C45

    if (tile_x < 0)                                               // asm 0x4A1C61-0x4A1C63
        return 0;                                                 // asm 0x4A1D3C (loc_4A1D3C)
    if (tile_y < 0)                                               // asm 0x4A1C69-0x4A1C6B
        return 0;
    if (tile_x >= ob_system->Width)                               // asm 0x4A1C71-0x4A1C74
        return 0;
    if (tile_y >= ob_system->Height)                              // asm 0x4A1C7A-0x4A1C7D
        return 0;

    float* terrain = this_->owner->world->terrains[this_->master_obj->terrain];   // asm 0x4A1C83-0x4A1C95
    int terrain_mask = ob_system->vfptr->GetTerrainMask(ob_system, terrain);      // asm 0x4A1C98-0x4A1C9B
    int inverted_mask = terrain_mask ^ 0xFF;                      // asm 0x4A1CA1

    if (ob_system->HardObArray != 0)                              // asm 0x4A1CA6
    {
        if ((ob_system->HardObArray[tile_y][tile_x] & inverted_mask) != 0)   // asm 0x4A1CAA-0x4A1CAD
            return 0;                                             // asm 0x4A1CB0 (jnz)
    }

    ob_system->vfptr->SetBufferRadius(ob_system, true, this_->master_obj->radius_x);   // asm 0x4A1CB6-0x4A1CC3

    // the ignore list: the caller's array when populated, else the
    // unit's own id in the second persistent scratch (both paths join
    // on the same SetIgnoreList call - the asm's 0x4A1CED join)
    if (ignoreUnits != 0 && numberIgnoreUnits > 0)                // asm 0x4A1CC6-0x4A1CD4 (jz/jle)
    {
        ob_system->vfptr->SetIgnoreList(ob_system, true, ignoreUnits, numberIgnoreUnits);   // asm 0x4A1CD6-0x4A1CDA + join 0x4A1CED
    }
    else
    {
        *OBSystem_Ignore_Scratch_Tile = this_->id;                // asm 0x4A1CE1
        ob_system->vfptr->SetIgnoreList(ob_system, true, OBSystem_Ignore_Scratch_Tile, 1);  // asm 0x4A1CDC + join 0x4A1CED
    }

    ob_system->vfptr->CheckForCollisions(                         // asm 0x4A1CF4-0x4A1D1A
        ob_system,
        x, y,                                                     // asm 0x4A1CFA/0x4A1CF4 (the frame-confused loads - resolved x/y)
        0.0f, 0.0f,
        32,                                                       // UNKNOWN query parameter (registry)
        numberObSystemCollisions,
        obSystemCollisions,                                       // the owner's ObsRecord** listing correction - the cast dropped
        25,                                                       // UNKNOWN query parameter (registry)
        -2,
        0,
        0);
    ob_system->vfptr->SetBufferRadius(ob_system, false, 0.0f);    // asm 0x4A1D1D-0x4A1D25

    return *numberObSystemCollisions <= 0;                        // asm 0x4A1D28-0x4A1D36 (setle)
}

// Replacement for RGE_Moving_Object::canPathWithAdditionalPassability. Original:
// @ 0x004A21D0 (idb_functions.h:2150). Range [0x4A21D0, 0x4A22B2),
// retn 38h, int. SEMANTIC: a canPath probe with three additional
// passable terrains - the tile-path query from the unit's tile to the
// goal tile, with the global pathing system's terrain-exception slots
// armed from the caller's eTerrain1/2/3 and two caller-supplied
// AI/unobstructible parameters armed into mAI/gbg_unknown1; all five
// restored to -1 after the query. PROTOTYPE NOTE: the original idb
// prototype was missing the mAI_arg/gbg_arg parameters (retn 38h = 56
// bytes vs 48 declared; the [E+0x28]/[E+0x2C] slots armed pathSystem
// .mAI/gbg_unknown1) - owner-corrected + regenerated 2026-11-18.
// Decode (asm 0x4A21D0-0x4A22B2): the goal-int gates (test/jl p.x
// @0x4A21D5-0x4A21DA, p.y @0x4A21E0-0x4A21E6 - XYZPoint ints,
// idb_types.h:3923); the map bounds (owner @this+0x18 -> world @+0x8C
// -> map @+0x34; cmp [ecx+8] map_width / [ecx+0Ch] map_height, asm
// 0x4A21F8-0x4A2204 -> loc_4A22AE = the 5-edge return-0 join);
// exceptionPath null gate (asm 0x4A220A-0x4A2210); the five
// pathSystem arms (mTerrainException1/2/3 @0x4A2226/0x4A2235/0x4A2244,
// mAI @0x4A2253, gbg_unknown1 @0x4A225E); findTilePath
// (idb_functions.h:2543) with the ftol-pair starts ((int)world_x/
// (int)world_y, asm 0x4A226A-0x4A2278 on [esi+4Ch]/[esi+48h]) and
// goals (p.x/p.y raw ints, asm 0x4A2269/0x4A2268), the 12 args in the
// callee-prototype order (pushes 0x4A221A-0x4A2278); the bool return
// masked (and eax,0FFh @0x4A2283); the five -1 disarms (asm
// 0x4A228C-0x4A22A4).
int __fastcall RGE_Moving_Object__canPathWithAdditionalPassability_new(
    RGE_Moving_Object* this_, DWORD dummy, XYZPoint p, float aRange,
    int targetID, float* pathDistance, int eTerrain1, int eTerrain2,
    int eTerrain3, int mAI_arg, int gbg_arg, int unobstructiblePlayerID,
    int unobstructibleGroupID, BPath* exceptionPath)
{
    if (p.x < 0)                                              // asm 0x4A21D5-0x4A21DA
        return 0;                                             // asm 0x4A22AE (loc_4A22AE)
    if (p.y < 0)                                               // asm 0x4A21E0-0x4A21E6
        return 0;
    RGE_Map* map = this_->owner->world->map;                  // asm 0x4A21EC-0x4A21F5 (owner @+0x18 -> world @+0x8C -> map @+0x34)
    if (p.x >= map->map_width)                                // asm 0x4A21F8-0x4A21FB (cmp [ecx+8])
        return 0;
    if (p.y >= map->map_height)                               // asm 0x4A2201-0x4A2204 (cmp [ecx+0Ch])
        return 0;
    if (exceptionPath == 0)                                   // asm 0x4A220A-0x4A2210 (test rPath)
        return 0;

    pathSystem->mTerrainException1 = eTerrain1;               // asm 0x4A2226
    pathSystem->mTerrainException2 = eTerrain2;               // asm 0x4A2235
    pathSystem->mTerrainException3 = eTerrain3;               // asm 0x4A2244
    pathSystem->mAI = mAI_arg;                                // asm 0x4A2253
    pathSystem->gbg_unknown1 = gbg_arg;                       // asm 0x4A225E

    bool result = PathingSystem__findTilePath(                // asm 0x4A227E-0x4A2283 (idb_functions.h:2543)
        pathSystem,
        (int)this_->world_x, (int)this_->world_y,             // asm 0x4A226A-0x4A2278 (the ftol pair on [esi+4Ch]/[esi+48h])
        p.x, p.y,                                             // asm 0x4A2269/0x4A2268 (goalX/goalY)
        this_,                                                // asm 0x4A2267 (unit)
        aRange,                                               // asm 0x4A225D
        targetID,                                             // asm 0x4A224E
        pathDistance,                                         // asm 0x4A223F (rLength)
        unobstructiblePlayerID,                               // asm 0x4A2230
        unobstructibleGroupID,                                // asm 0x4A2221
        exceptionPath,                                        // asm 0x4A221C (rPath)
        false);                                               // asm 0x4A221A (useRangeWithoutTarget)

    pathSystem->mTerrainException1 = -1;                      // asm 0x4A228C
    pathSystem->mTerrainException2 = -1;                      // asm 0x4A2292
    pathSystem->mTerrainException3 = -1;                      // asm 0x4A2298
    pathSystem->mAI = -1;                                     // asm 0x4A229E
    pathSystem->gbg_unknown1 = -1;                            // asm 0x4A22A4

    return result;                                            // asm 0x4A2283 (and eax,0FFh)
}

// Replacement for RGE_Moving_Object::get_waypoint_checksum. Original:
// @ 0x004A3000 (idb_functions.h:2168). Range [0x4A3000, 0x4A30E3),
// retn (0 stack args), int. SEMANTIC: a movement-consistency checksum -
// the sum of every waypoint's three components across every path in
// the unit's pathData chain (resolving path ids through the world when
// a node has no cached path), plus the unit's last move time, truncated
// to int. 0 when owner or world is null.
// Decode (asm 0x4A3000-0x4A30E3): the null gates (owner @+0x18, world
// @+0x8C, asm 0x4A3009/0x4A3019 -> xor eax; return 0); the accumulator
// = 0.0f (asm 0x4A3028, a FLOAT local with per-step fstp m32
// store-rounds - rule (e) replicated); the linkedPath chain walk
// (pathData @+0x118, node->linkedPath @+4, asm 0x4A3020/0x4A30AF-
// 0x4A30B4); per node: the cached path @+0x10 when non-null else
// RGE_Game_World__path(world, node->pathID @+0x14) (asm 0x4A3038-
// 0x4A3055, null path -> the next node); the waypoint loop re-reads
// waypointsCount @+0xC per iteration (asm 0x4A30A3) and sums
// waypoint(i)->[0]/[4]/[8] (the three BVector slots) via three
// BPath__waypoint calls each with its own out buffer (asm 0x4A3060-
// 0x4A30A9); the tail filds the ZERO-EXTENDED lastMoveTimeValue
// (@+0x124 - the high dword forced 0, asm 0x4A30BE-0x4A30CA - the
// mechanical form is the (unsigned int) widening), adds the
// accumulator at 53-bit and truncates via __ftol (asm 0x4A30CA-0x4A30D2).
int __fastcall RGE_Moving_Object__get_waypoint_checksum_new(
    RGE_Moving_Object* this_, DWORD dummy)
{
    if (this_->owner == 0 || this_->owner->world == 0)       // asm 0x4A3009/0x4A3019
        return 0;                                             // asm 0x4A30DD

    float checksum = 0.0f;                                    // asm 0x4A3028 (float accumulator, per-step fstp m32 store-rounds)
    for (BPathData* node = this_->pathData; node != 0; node = node->linkedPath)   // asm 0x4A3020 (@+0x118) / 0x4A30AF-0x4A30B4 (+4)
    {
        BPath* path = node->path;                             // asm 0x4A3038 (@+0x10)
        if (path == 0)
        {
            path = RGE_Game_World__path((RGE_Game_World*)this_->owner->world, node->pathID);   // asm 0x4A303F-0x4A3051 (idb_functions.h:6094; TRIBE_World* -> RGE_Game_World* - the identical-prefix family cast, the base_game precedent)
            if (path == 0)                                    // asm 0x4A3053
                continue;                                     // asm 0x4A3055 (-> loc_4A30AF, the next node)
        }
        BVector wp_x;                                         // the asm's wpIndex out buffer
        BVector wp_z;                                         // the asm's outVector out buffer
        BVector wp_y;                                         // the asm's var_C out buffer
        for (int i = 0; i < path->waypointsCount; ++i)        // asm 0x4A3057-0x4A305E; the count re-read @0x4A30A3
        {
            checksum = checksum + BPath__waypoint(path, &wp_x, i)->x;   // asm 0x4A3060-0x4A3078 ([eax])
            checksum = checksum + BPath__waypoint(path, &wp_z, i)->z;   // asm 0x4A307B-0x4A308B ([eax+4])
            checksum = checksum + BPath__waypoint(path, &wp_y, i)->y;   // asm 0x4A3090-0x4A30A0 ([eax+8])
        }
    }

    return (int)((double)(unsigned int)this_->lastMoveTimeValue + (double)checksum);   // asm 0x4A30BE-0x4A30D2 (fild qword zero-extended; fadd; ftol)
}

// Replacement for RGE_Moving_Object::addCurrentPath. Original:
// @ 0x004A34B0 (idb_functions.h:2170). Range [0x4A34B0, 0x4A35B2),
// retn 10h, int, SEH frame (the allocation-unwind chunk 0x642820:
// operator_delete on the half-constructed pathData - [ebp+4] - when
// the ctor throws at EH state 0; the written ctor call regenerates the
// equivalent unwind, the setVelocity precedent). SEMANTIC: installs a
// new current path on the unit: resets the future-path node, creates
// the pathData node if absent (0x34 = sizeof(BPathData), walked:
// 12 ints + BBitVector(int)), stamps the path type, and routes to
// setReadyToFollowPath (pathType 0: the range and target stamped
// after) or setReadyToPath (otherwise).
// Decode (asm 0x4A34B0-0x4A35B2): p == 0 -> return 0 (unwind, asm
// 0x4A34CB-0x4A34E3); BPathData__reset on mFuturePathData @+0x11C when
// non-null (asm 0x4A34E6-0x4A34F0); the pathData @+0x118 branch: null
// -> operator_new(0x34) + BPathData__BPathData + this->pathData = new
// (null new -> return 0 via the unwind, asm 0x4A34F5-0x4A3546), else
// BPathData__reset (asm 0x4A3549-0x4A354F); node->waypointLevel @+0xC
// = pathType (asm 0x4A3554-0x4A355A); pathType == 0 ->
// setReadyToFollowPath(node, p) + node->range @+0x20 = r +
// node->targetID @+0x24 = tID (asm 0x4A355F-0x4A3585), else
// setReadyToPath(node, p, r, tID) (asm 0x4A3587-0x4A3598); return 1
// (asm 0x4A35A2-0x4A35B2).
int __fastcall RGE_Moving_Object__addCurrentPath_new(
    RGE_Moving_Object* this_, DWORD dummy, BPath* p, int tID, float r, int pathType)
{
    if (p == 0)                                               // asm 0x4A34CB-0x4A34CF
        return 0;                                             // asm 0x4A34D1-0x4A34E3 (the SEH-unwind exit)

    if (this_->mFuturePathData != 0)                          // asm 0x4A34E6-0x4A34EE (@+0x11C)
        BPathData__reset(this_->mFuturePathData);             // asm 0x4A34F0 (idb_functions.h:2507)

    if (this_->pathData == 0)                                 // asm 0x4A34F5-0x4A34FD (@+0x118)
    {
        BPathData* created = (BPathData*)operator_new_internal(sizeof(BPathData));   // asm 0x4A34FF-0x4A3506 (52 = sizeof(BPathData), walked: 12 ints + BBitVector)
        if (created != 0)                                     // asm 0x4A350D-0x4A3517
            BPathData__BPathData(created);                    // asm 0x4A351B (idb_functions.h:2505)
        this_->pathData = created;                            // asm 0x4A352E (@+0x118, after the ctor)
        if (created == 0)                                     // asm 0x4A3524-0x4A3534
            return 0;                                         // asm 0x4A3536-0x4A3546 (the SEH-unwind exit)
    }
    else
    {
        BPathData__reset(this_->pathData);                    // asm 0x4A3549-0x4A354F
    }

    BPathData* node = this_->pathData;                        // asm 0x4A354E-0x4A3554
    node->waypointLevel = pathType;                           // asm 0x4A3554-0x4A355A (@+0xC)
    if (pathType == 0)                                        // asm 0x4A3558-0x4A355D
    {
        BPathData__setReadyToFollowPath(node, p);             // asm 0x4A355F-0x4A3566 (idb_functions.h:2514)
        node->range = r;                                      // asm 0x4A3571-0x4A3575 (@+0x20)
        node->targetID = tID;                                 // asm 0x4A357E-0x4A3582 (@+0x24)
    }
    else
    {
        BPathData__setReadyToPath(node, p, r, tID);           // asm 0x4A3587-0x4A3598 (idb_functions.h:2513)
    }

    return 1;                                                 // asm 0x4A35A2-0x4A35B2
}

// Replacement for RGE_Moving_Object::findFirstTerrainAlongExceptionPath. Original:
// @ 0x004A22C0 (idb_functions.h). Range [0x4A22C0, 0x4A2412), retn 10h,
// int. SEMANTIC: walks the exception path's waypoints as tile-int line
// segments and returns the CENTER of the first tile whose terrain
// matches eTerrain (via the unit's firstTileAlongLine vtable slot, idx
// 147 @0x24C); *rX/*rY = tile + 0.5f on a hit (return 1), *rX/*rY =
// -1.0f on the count<=1 and loop-exhausted failures (return 0), and a
// plain return 0 (rX/rY UNTOUCHED) when the segment index runs past the
// waypoint count. Needs >= 2 waypoints.
// Decode (asm 0x4A22C0-0x4A2412): the gates (exceptionPath null +
// waypointsCount @+0xC < 2, asm 0x4A22CA-0x4A22D9 -> loc_4A23DA = the
// plain return-0 join); the seed pairs - each waypoint fetched TWICE
// (once per component) into the same out buffer: (int)wp0.x
// @0x4A22E8-0x4A22EF, (int)wp0.y @0x4A2301-0x4A2309 ([eax+8] - the
// {x,z,y} BVector lens), (int)wp1.x @0x4A231F-0x4A2326, (int)wp1.y
// @0x4A2337-0x4A233F (var_34/var_30); the count <= 1 gate (asm
// 0x4A2348-0x4A234D -> loc_4A23C6: rX/rY = -1.0f then return 0 - kept
// despite the earlier gate: waypoint() could mutate the count, the
// redundancy is the game's own); the segment loop (asm 0x4A2353):
// firstTileAlongLine (vtbl idx 147 @0x24C, asm 0x4A236B) with
// s = &{prev.x, prev.y} (var_2C/var_28), d = &{cur.x, cur.y}
// (var_34/var_30), the out point = the wpIndex buffer, (eTerrain,
// eTerrain, 1); the hit (cmp eax,1 jz @0x4A2374 -> loc_4A23E6): fild
// the out point's two ints + fadd const_f_half -> *rX/*rY, return 1;
// the miss: s = d (asm 0x4A2379-0x4A2384), ++i, i >= count -> the
// PLAIN return 0 (asm 0x4A238C -> loc_4A23DA - rX/rY UNTOUCHED, the
// .c's split preserved), else fetch waypoint(i) into d (the
// outVector/var_C buffers, asm 0x4A238E-0x4A23BB) and loop while
// i < count (asm 0x4A23C2-0x4A23C4); the loop-exhausted fall ->
// loc_4A23C6 (the -1.0f fail).
int __fastcall RGE_Moving_Object__findFirstTerrainAlongExceptionPath_new(
    RGE_Moving_Object* this_, DWORD dummy, int eTerrain, float* rX,
    float* rY, BPath* exceptionPath)
{
    if (exceptionPath == 0 || exceptionPath->waypointsCount < 2)   // asm 0x4A22CA-0x4A22D9
        return 0;                                                  // asm 0x4A23DA (loc_4A23DA)

    XYPoint prev;                                                  // the asm's var_2C/var_28 pair
    XYPoint cur;                                                   // the asm's var_34/var_30 pair
    {
        BVector out_a;                                             // the asm's wpIndex out buffer (reused per seed fetch)
        prev.x = (int)BPath__waypoint(exceptionPath, &out_a, 0)->x;   // asm 0x4A22E8-0x4A22EF
        prev.y = (int)BPath__waypoint(exceptionPath, &out_a, 0)->y;   // asm 0x4A2301-0x4A2309 ([eax+8])
        cur.x = (int)BPath__waypoint(exceptionPath, &out_a, 1)->x;    // asm 0x4A231F-0x4A2326
        cur.y = (int)BPath__waypoint(exceptionPath, &out_a, 1)->y;    // asm 0x4A2337-0x4A233F
    }

    if (exceptionPath->waypointsCount <= 1)                        // asm 0x4A2348-0x4A234D (kept: waypoint() could mutate the count)
    {
        *rX = -1.0f;                                               // asm 0x4A23CE (loc_4A23C6)
        *rY = -1.0f;                                               // asm 0x4A23D4
        return 0;                                                  // asm 0x4A23DA
    }

    BVector found;                                                 // the asm's wpIndex buffer (BVector-sized; the callee fills the first two ints as an XYPoint - the .c reads .x/.z of this buffer)
    for (int i = 1; i < exceptionPath->waypointsCount; ++i)        // asm 0x4A234B/0x4A2381-0x4A23C4 (both count re-reads preserved)
    {
        if (this_->vfptr->firstTileAlongLine(                      // asm 0x4A2353-0x4A236B (vtbl idx 147 @0x24C)
                this_,
                &prev, &cur, (XYPoint*)&found,
                eTerrain, eTerrain, 1) == 1)                       // asm 0x4A2371-0x4A2374 (cmp eax,1 jz)
        {
            *rX = found.x + 0.5f;                                  // asm 0x4A23E6-0x4A23FC (fild/fadd const_f_half/fstp)
            *rY = found.z + 0.5f;                                  // asm 0x4A23FE-0x4A2408
            return 1;                                              // asm 0x4A240A
        }
        prev = cur;                                                // asm 0x4A2379-0x4A2384 (s = d)
        if (i + 1 >= exceptionPath->waypointsCount)                // asm 0x4A2376-0x4A238C (the re-read; inc/cmp/jge)
            return 0;                                              // asm 0x4A23DA (the PLAIN return - rX/rY untouched)
        {
            BVector out_b;                                         // the asm's outVector buffer
            BVector out_c;                                         // the asm's var_C buffer
            cur.x = (int)BPath__waypoint(exceptionPath, &out_b, i + 1)->x;   // asm 0x4A238E-0x4A239D (the post-increment index)
            cur.y = (int)BPath__waypoint(exceptionPath, &out_c, i + 1)->y;   // asm 0x4A23AE-0x4A23BB
        }
    }
    *rX = -1.0f;                                                   // asm 0x4A23CE (loc_4A23C6 - the exhausted fall)
    *rY = -1.0f;                                                   // asm 0x4A23D4
    return 0;                                                      // asm 0x4A23DA
}

// Replacement for RGE_Moving_Object::canLinePath. Original:
// @ 0x004A25B0 (idb_functions.h:2153). Range [0x4A25B0, 0x4A270D),
// retn 14h, int. SEMANTIC: walks the straight line from (sX,sY) to
// (dX,dY) in 2*|(int)dominant-delta| float steps, probing each NEW
// tile (the truncated cursor changed) through passableTile2 (vtbl
// idx 104 @0x1A0 = passableTile2, our hook); an impassable tile
// returns 0, the destination coming within range returns 1 en route,
// and the exhausted walk returns 1.
// PRECISION: the step increments divide at 53-bit then store to float
// (asm fstp m32) - rule (c): written as (float)((double)/(double)),
// divss would be a new rounding; the cursor accumulators are float
// locals with per-step fstp m32 stores (rule (d)/(e), coordinate
// ranges); the distance chain (fild/fsub/fmul/fsqrt) runs 53-bit with
// no intermediate stores - rule (a): all-double. The degenerate and
// distance compares have no NaN sources (fild of ints, sqrt of a sum
// of squares) - the literal forms are vacuous polarity-MATCH rows.
// PASSABLETILE_2 DISPATCH: the asm calls vtbl idx 104 @0x1A0 = the
// passableTile2 member (idb_types.h:10917 = the member index walked,
// 10917 - first-member 10813 = 104 = 0x1A0; the slot's implementation
// in every runtime vtable initializer = passableTile2 @0x4A1B30, our
// hook) - written as the NAMED VIRTUAL DISPATCH (owner-directed
// 2026-11-18): faithful to the asm's virtual dispatch. The direct
// idb-constant call form it replaces is behaviorally identical today
// (no slot-104 override exists in any runtime initializer) but
// hard-coded the implementation. GROUNDING for the member/slot pair
// (the 2026-11-18 IDB correction, owner-applied): the Combat runtime
// initializer 0x657544 is fully named through the window - 103 =
// passableTile (4-param, retn 10h, the +0x19C 4-arg push sites),
// 104 = passableTile2 (2-param, retn 8), 105 = currentTargetID (the
// +0x1A4 no-arg sites) - and the line arithmetic on the regenerated
// header puts passableTile2 at idx 104 = 0x1A0.
// Decode (asm 0x4A25B0-0x4A270D): the deltas (int sub, asm 0x4A25C0/
// 0x4A25D2, then fild - the exact widening); the degenerate gate (two
// fcomp const_f_zero/test ah,40h, asm 0x4A25DE-0x4A25F9 -> return 0);
// the dominant-axis selection (fabs/fabs/fcompp, test ah,1: |dx| >=
// |dy| jz -> dx else dy, asm 0x4A260B-0x4A2622); the step count (ftol
// + the cdq/xor/sub abs + shl 1, asm 0x4A2622-0x4A2635); the steps<=0
// -> return 1 gate (asm 0x4A2643 test, the jle @0x4A2669); the step
// increments (fdiv at 53-bit, fstp m32, asm 0x4A263B-0x4A264B); the
// cursor seeds (fild sX/sY -> float stores, asm 0x4A2651-0x4A2665;
// the caller's sY slot zeroed as the loop counter); the loop (asm
// 0x4A266F): cursor += step (float stores), the new-tile test (ftol
// pair vs prev -999/-999, asm 0x4A2687-0x4A26A1), passableTile2
// (asm 0x4A26AF-0x4A26BD, jz -> return 0), the distance chain (asm
// 0x4A26C5-0x4A26E5) vs range (test ah,41h jnz -> return 1), ++i /
// i < steps (asm 0x4A26F0-0x4A26FB); exhausted -> return 1 (asm
// 0x4A2701-0x4A270D).
// PRECISION SCOPE (owner-directed 2026-11-18, option 1): the two step
// divisions are compiled under precise semantics via this bracket -
// under /fp:fast MSVC transformed `(double)a / (double)t` into
// (1.0/t)*a (the reciprocal-multiply: two roundings instead of the
// x87 fdiv's one - verified in the first /Fa pass), diverging from the
// original's fdiv+fstp in low bits. float_control(precise) pins the
// divisions as divisions (divsd); the function's other ops are exact
// either way. The pragma bracket is the project's first use of
// function-scoped precision control - the /Fa gate check for this
// function must verify divsd (no reciprocal form) on every re-gate.
#pragma float_control(precise, on, push)
int __fastcall RGE_Moving_Object__canLinePath_new(
    RGE_Moving_Object* this_, DWORD dummy, int sX, int sY, int dX,
    int dY, float range)
{
    double dx_f = (double)(dX - sX);                          // asm 0x4A25C0-0x4A25CE (the int sub; fild - the exact widening)
    double dy_f = (double)(dY - sY);                          // asm 0x4A25D2-0x4A25DA

    if (dx_f == 0.0 && dy_f == 0.0)                           // asm 0x4A25DE-0x4A25F9 (two fcomp/test ah,40h gates; int origins - no NaN source, the literal form matches)
        return 0;                                             // asm 0x4A25FF

    double selected = fabs(dx_f) >= fabs(dy_f) ? dx_f : dy_f;   // asm 0x4A260B-0x4A2622 (fabs/fabs/fcompp; test ah,1 jz -> dx)
    int steps = (int)selected;                                // asm 0x4A2622-0x4A2628 (ftol)
    int two_steps = abs(steps) * 2;                           // asm 0x4A2627-0x4A2635 (cdq/xor/sub abs; shl 1)
    if (two_steps == 0)                                       // asm 0x4A2643 + 0x4A2669 (test ebp, ebp AFTER the abs/shl - ebp = two_steps, always >= 0: the gate fires only when the dominant delta truncates to 0; a negative-dominant line WALKS - the M8c-era "SIGNED PRE-ABS / asm wins" reading was a register misattribution; the .c's `v8 <= 0` render was right)
        return 1;                                             // asm 0x4A2701

    float dx_step = (float)((double)dx_f / (double)two_steps);   // asm 0x4A263B-0x4A2645 (fdiv at 53-bit; fstp m32 - rule (c))
    float dy_step = (float)((double)dy_f / (double)two_steps);   // asm 0x4A2649-0x4A264B
    float cursor_x = (float)sX;                               // asm 0x4A2651-0x4A2655 (fild/fstp [var_10])
    float cursor_y = (float)sY;                               // asm 0x4A2659-0x4A2665 (fstp [the reused sY slot])
    int prev_x = -999;                                        // asm 0x4A262A (mov edi, 0FFFFFC19h)
    int prev_y = -999;                                        // asm 0x4A2631 (mov ebx, edi)

    for (int i = 0; i < two_steps; ++i)                       // asm 0x4A2669 (the jle) / 0x4A26F0-0x4A26FB (inc/cmp/jl - the shared ++i block)
    {
        cursor_x += dx_step;                                  // asm 0x4A266F-0x4A2677 (float per-step store)
        cursor_y += dy_step;                                  // asm 0x4A267B-0x4A2683
        int cur_x = (int)cursor_x;                            // asm 0x4A2687-0x4A2690 (ftol)
        int cur_y = (int)cursor_y;                            // asm 0x4A2690-0x4A2696 (ftol)
        if (cur_x == prev_x && cur_y == prev_y)               // asm 0x4A269B-0x4A26A1 (the same-tile test vs prev -999/-999)
            continue;                                         // asm 0x4A26F0 (the shared ++i/cmp/jl block)
        prev_y = cur_y;                                       // asm 0x4A26A7
        prev_x = cur_x;                                       // asm 0x4A26AD
        if (this_->vfptr->passableTile2(this_, cursor_x, cursor_y) == 0)   // asm 0x4A26AF-0x4A26BD (vtbl idx 104 @0x1A0 = passableTile2 - the raw float cursor bits)
            return 0;                                         // asm 0x4A26BF-0x4A2605 (jz -> loc_4A25FF)
        double dy_delta = (double)dY - (double)cursor_y;      // asm 0x4A26C5-0x4A26C9 (fild dY; fsub)
        double dx_delta = (double)dX - (double)cursor_x;      // asm 0x4A26CD-0x4A26D1 (fild dX; fsub)
        double dist = sqrt(dx_delta * dx_delta + dy_delta * dy_delta);   // asm 0x4A26D5-0x4A26DF (the 53-bit chain)
        if (dist <= range)                                    // asm 0x4A26E5-0x4A26EE (fcomp range; test ah,41h; jnz -> return 1 - {<=,NaN}; no NaN source - the literal MATCH row)
            return 1;                                         // asm 0x4A2701
    }
    return 1;                                                 // asm 0x4A2701-0x4A270D (the exhausted walk)
}
#pragma float_control(pop)

// Replacement for RGE_Moving_Object::findAvoidancePath. Original:
// @ 0x004A2D80 (idb_functions.h:2165). Range [0x4A2D80, 0x4A2EDF),
// retn 0Ch, returns BPath*, SEH frame (the allocation-unwind chunk
// 0x6427E0: operator_delete on the half-constructed BPath - the
// setVelocity/addCurrentPath precedent; the written ctor call
// regenerates the unwind). SEMANTIC: resolves (and caches) the unit's
// zone-map index from its terrain, then asks the zone map whether the
// goal is reachable without an avoidance route (the same zone, or
// within range across zones): if so it allocates a fresh BPath and
// runs the tile-path query into it; returns the BPath pointer, or 0
// when the goal is out of cross-zone range or the allocation fails.
// The findTilePath bool is DISCARDED - the caller receives the path
// regardless.
// Decode (asm 0x4A2D80-0x4A2EDF): the cached index gate (zoneMapIndex
// @+0x80 == -1, asm 0x4A2DA7-0x4A2DAA); the resolution
// RGE_Zone_Map_List__get_zone_map(map->map_zones @RGE_Map+0xBF7C,
// world->terrains[master->terrain @+0x76], world->terrain_size @+0x5E,
// &zoneMapIndex) - the return DISCARDED, only the out index kept (asm
// 0x4A2DAC-0x4A2DD2); get_zone_map2(zml, index) (asm 0x4A2DD7-
// 0x4A2DEC); get_zone_info(zm, (int)world_x, (int)world_y) & 0xFF
// (asm 0x4A2DF1-0x4A2E15); get_zone_info(zm, goal->x, goal->y) & 0xFF
// (asm 0x4A2E17-0x4A2E25); the SAME-zone skip past the range check
// (asm 0x4A2E2A-0x4A2E2C); the cross-zone withinRange(zm,
// {(int)world_x,(int)world_y}, {goal->x, goal->y}, range, 0) gate -
// false -> return 0 (asm 0x4A2E2E-0x4A2E59 -> loc_4A2ECD with eax=0);
// operator_new(sizeof(BPath)) into the reused goalPoint slot (asm
// 0x4A2E5B-0x4A2E65); BPath__BPath(new, -1) (asm 0x4A2E75-0x4A2E79);
// null -> return 0 via the unwind (asm 0x4A2E84-0x4A2E92);
// findTilePath(pathSystem, (int)world_x, (int)world_y, goal->x,
// goal->y, this, range, targetID, 0, -1, -1, the new BPath, 0) - the
// bool DISCARDED (asm 0x4A2E94-0x4A2EC6); return the BPath (asm
// 0x4A2ECB).
BPath* __fastcall RGE_Moving_Object__findAvoidancePath_new(
    RGE_Moving_Object* this_, DWORD dummy, XYZPoint* goalPoint, float aRange,
    int targetID)
{
    if (this_->zoneMapIndex == -1)                            // asm 0x4A2DA7-0x4A2DAA (@+0x80)
        RGE_Zone_Map_List__get_zone_map(                      // asm 0x4A2DD2 (idb_functions.h:6122 - the return discarded, the out index kept)
            this_->owner->world->map->map_zones,              // asm 0x4A2DCC (RGE_Map+0xBF7C, idb_types.h:2941)
            this_->owner->world->terrains[this_->master_obj->terrain],   // asm 0x4A2DC3 (terrains @+0x60; terrain @+0x76)
            this_->owner->world->terrain_size,                // asm 0x4A2DC7 (@+0x5E)
            &this_->zoneMapIndex);

    RGE_Zone_Map* zone_map = RGE_Zone_Map_List__get_zone_map2(this_->owner->world->map->map_zones, this_->zoneMapIndex);   // asm 0x4A2DEC (idb_functions.h:6123)

    unsigned int unit_zone = RGE_Zone_Map__get_zone_info(zone_map, (int)this_->world_x, (int)this_->world_y);   // asm 0x4A2E07-0x4A2E15 (idb_functions.h:6112; the ftol pair; and 0FFh)
    unsigned int goal_zone = RGE_Zone_Map__get_zone_info(zone_map, goalPoint->x, goalPoint->y);   // asm 0x4A2E20-0x4A2E25

    if (goal_zone != unit_zone
        && RGE_Zone_Map__withinRange(                         // asm 0x4A2E52 (idb_functions.h:6113)
               zone_map,
               { (int)this_->world_x, (int)this_->world_y },   // asm 0x4A2E4F/0x4A2E4E (the XYPoint by value)
               { goalPoint->x, goalPoint->y },                 // asm 0x4A2E4D/0x4A2E4C
               aRange,                                         // asm 0x4A2E48
               0) == 0)                                        // asm 0x4A2E46 (new_goal)
        return 0;                                              // asm 0x4A2ECD-0x4A2EDF (eax = 0)

    BPath* path = (BPath*)operator_new_internal(sizeof(BPath));   // asm 0x4A2E5D (52 = sizeof(BPath), walked: 13 dwords incl. BBitVector)
    if (path != 0)                                             // asm 0x4A2E69-0x4A2E73
        BPath__BPath(path, -1);                                // asm 0x4A2E79 (idb_functions.h:2520)

    if (path == 0)                                             // asm 0x4A2E84-0x4A2E8E
        return 0;                                              // asm 0x4A2E90-0x4A2E92 (the SEH-unwind exit)

    PathingSystem__findTilePath(                               // asm 0x4A2EC6 (idb_functions.h:2543 - the bool DISCARDED)
        pathSystem,
        (int)this_->world_x, (int)this_->world_y,              // asm 0x4A2EB2-0x4A2EC0 (the ftol pair)
        goalPoint->x, goalPoint->y,                            // asm 0x4A2EB1/0x4A2EB0
        this_,                                                 // asm 0x4A2EAF
        aRange,                                                // asm 0x4A2EAE
        targetID,                                              // asm 0x4A2EAB
        0,                                                     // asm 0x4A2EA9 (rLength)
        -1,                                                    // asm 0x4A2EA7 (unobstructiblePlayerID)
        -1,                                                    // asm 0x4A2EA5 (unobstructibleGroupID)
        path,                                                  // asm 0x4A2EA4 (rPath)
        false);                                                // asm 0x4A2E9F (useRangeWithoutTarget)

    return path;                                               // asm 0x4A2ECB (mov eax, ebx)
}

// Replacement for RGE_Moving_Object::canQuickPath. Original:
// @ 0x004A1E80 (idb_functions.h:2148). Range [0x4A1E80, 0x4A1FE1),
// retn 0Ch, int, SEH frame (the dtor-jmp chunk 0x642780: the unwind
// runs BPath__destructor on the local ReturnPath - documented; the
// written pointer-call form does not emit EH state - the
// setVelocity/addCurrentPath precedent). SEMANTIC: the quick
// reachability probe to a TARGET OBJECT - resolves the target, arms
// the MShortPather ignore list with the target's own id (+ the
// terrain collision mask from GetTerrainMask), runs the short-path
// query from the unit to the target, writes the path length to
// pathDistance when requested, and reports 1 on a query result of 0
// or 1, 0 on an impassable/error result, a null target, or a failed
// local BPath construction.
// Decode (asm 0x4A1E80-0x4A1FE1): the target resolve
// RGE_Game_World__object(owner->world, targetID) (asm 0x4A1EA1-
// 0x4A1EAB, idb_functions.h:6086; the TRIBE_World family cast) - null
// -> return 0; the LOCAL BPath (the ReturnPath buffer @-0x40,
// BPath__BPath(&ReturnPath, -1), asm 0x4A1ECB-0x4A1ED1); the OB
// domain select (master movement_type @+0x78 == 1 -> Air, asm
// 0x4A1EE7-0x4A1EEB); the ignore-list arming: ignoreList_3 = targetID
// (the PERSISTENT global 0x6ACE98 - the manager stores the ids
// pointer), GetTerrainMask (vtbl idx 7 @0x1C, asm 0x4A1F14) -> the
// terrain mask, MShortPather__SetIgnoreList(the OB's ShortPathFinder
// @+0x1BC, 1, &ignoreList_3, 2, mask) (asm 0x4A1F29,
// idb_functions.h:2236); FindShortPath (asm 0x4A1F6D,
// idb_functions.h:2249) with the 14 args (StartX/Y = this->world_x/
// world_y RAW FLOATS @+0x48/+0x4C - no truncation, the prototype
// takes floats; Radius = master->radius_x @+0x3C; TargetX/Y = the
// target's world_x/world_y @+0x48/+0x4C; TargetID; Range;
// TerrainCollisionMask; ReturnPath; useHulledArea=0; playerID =
// owner->id @+0xA0; collisionUnits=0; numberCollisionUnits=0;
// mode=1); the result gates: 0 or 1 -> the success path (2+ ->
// BPath__destructor + return 0, asm 0x4A1F7C-0x4A1FA0); the success
// path: pathDistance != 0 -> BPath__calculatePathLength +
// *pathDistance = ReturnPath.pathLengthValue (@BPath+0x18, the m32
// store, asm 0x4A1FAB-0x4A1FB8); BPath__destructor at the exit (asm
// 0x4A1FC6); return 1 (asm 0x4A1FD1-0x4A1FE1).
int __fastcall RGE_Moving_Object__canQuickPath_new(
    RGE_Moving_Object* this_, DWORD dummy, int targetID, float range,
    float* pathDistance)
{
    RGE_Static_Object* target = RGE_Game_World__object((RGE_Game_World*)this_->owner->world, targetID);   // asm 0x4A1EA1-0x4A1EAB (idb_functions.h:6086; the TRIBE_World family cast)
    if (target == 0)                                          // asm 0x4A1EB2-0x4A1EB4
        return 0;                                             // asm 0x4A1EB6-0x4A1EC7

    __declspec(align(4)) unsigned char ReturnPath_storage[sizeof(BPath)];   // the asm's stack BPath @-0x40 (raw storage: BPath's const member deletes the default ctor - the original constructs via the raw call)
    BPath* const ReturnPath = (BPath*)ReturnPath_storage;     // asm 0x4A1ECB-0x4A1ED1 (the placement-cast over the raw storage)
    BPath__BPath(ReturnPath, -1);                             // asm 0x4A1ECB-0x4A1ED1 (idb_functions.h:2520)

    RGE_Obstruction_Manager* ob_system = *OBSystem_Land;      // asm 0x4A1ED9
    if (this_->master_obj->movement_type == 1)                // asm 0x4A1EE7 (byte @+0x78)
        ob_system = (RGE_Obstruction_Manager*)*OBSystem_Air;  // asm 0x4A1EED (the layout-proven twin cast)

    *ignoreList_3 = targetID;                                 // asm 0x4A1EF3 (the persistent global 0x6ACE98 - the manager stores the ids pointer)
    int terrain_mask = ob_system->vfptr->GetTerrainMask(ob_system, this_->owner->world->terrains[this_->master_obj->terrain]);   // asm 0x4A1F14-0x4A1F17 (vtbl idx 7 @0x1C)
    MShortPather* pather = ob_system->ShortPathFinder;        // asm 0x4A1F17 (@+0x1BC)
    MShortPather__SetIgnoreList(pather, true, ignoreList_3, 2, terrain_mask);   // asm 0x4A1F29 (idb_functions.h:2236)

    int result = MShortPather__FindShortPath(                 // asm 0x4A1F6D (idb_functions.h:2249)
        pather,
        this_->world_x, this_->world_y,                       // asm 0x4A1F6C/0x4A1F6B (the RAW floats - the prototype takes floats, no truncation)
        this_->master_obj->radius_x,                          // asm 0x4A1F64 (Radius @+0x3C)
        target->world_x, target->world_y,                     // asm 0x4A1F52/0x4A1F59 (the target's world_x/world_y @+0x48/+0x4C)
        targetID,                                             // asm 0x4A1F55
        range,                                                // asm 0x4A1F51
        terrain_mask,                                         // asm 0x4A1F50 (TerrainCollisionMask)
        ReturnPath,                                           // asm 0x4A1F4C
        false,                                                // asm 0x4A1F4A (useHulledArea)
        this_->owner->id,                                     // asm 0x4A1F41 (owner->id @+0xA0)
        0,                                                    // asm 0x4A1F35 (collisionUnits)
        0,                                                    // asm 0x4A1F33 (numberCollisionUnits)
        1);                                                   // asm 0x4A1F31 (mode)

    if (result != 0 && result != 1)                           // asm 0x4A1F72-0x4A1F7A (test eax / cmp eax,1 - both jz the success join)
    {
        BPath__destructor(ReturnPath);                        // asm 0x4A1F88 (idb_functions.h:2521)
        return 0;                                             // asm 0x4A1F8D-0x4A1FA0
    }

    if (pathDistance != 0)                                    // asm 0x4A1FA7-0x4A1FA9
    {
        BPath__calculatePathLength(ReturnPath);               // asm 0x4A1FAF (idb_functions.h:2526)
        *pathDistance = ReturnPath->pathLengthValue;          // asm 0x4A1FB4-0x4A1FB8 (@BPath+0x18, the m32 store)
    }

    BPath__destructor(ReturnPath);                            // asm 0x4A1FC6-0x4A1FCB (the exit destructor)
    return 1;                                                 // asm 0x4A1FD1-0x4A1FE1
}

// PRECISION SCOPE: the same float_control bracket as canLinePath_new
// (rule (k)) - the two hoisted step divisions must compile as divsd.
#pragma float_control(precise, on, push)
int __fastcall RGE_Moving_Object__canLinePath2_new(
    RGE_Moving_Object* this_, DWORD dummy, XYPoint* s, XYPoint* d,
    float range, XYPoint* rPoint)
{
    double dx_f = (double)(d->x - s->x);                      // asm 0x4A243D-0x4A243F (the int sub; fild @0x4A2448 - the exact widening)
    double dy_f = (double)(d->y - s->y);                      // asm 0x4A2446-0x4A2454

    if (dx_f == 0.0 && dy_f == 0.0)                           // asm 0x4A2458-0x4A2473 (the two fcomp/test ah,40h gates; int origins - no NaN source, the literal form matches)
        return 0;                                             // asm 0x4A2475-0x4A2482

    double selected = fabs(dx_f) >= fabs(dy_f) ? dx_f : dy_f;   // asm 0x4A2485-0x4A2494 (fabs/fabs/fcompp; test ah,1 jz -> dx)
    int steps = (int)selected;                                // asm 0x4A2496-0x4A24AC (ftol; both selection paths)
    int two_steps = abs(steps) * 2;                           // asm 0x4A249D-0x4A24B8 (cdq/xor/sub abs; shl 1)
    if (two_steps == 0)                                       // asm 0x4A24C9 + 0x4A24F1 (test eax, eax AFTER the abs/shl - eax = two_steps, always >= 0: the gate fires only when the dominant delta truncates to 0; a negative-dominant line WALKS - the M8c-era "SIGNED PRE-ABS" reading was a register misattribution)
        return 1;                                             // asm 0x4A258C-0x4A2598

    float dx_step = (float)((double)dx_f / (double)two_steps);   // asm 0x4A24C0-0x4A24CD (fdiv at 53-bit; fstp m32 - rule (c); hoisted ONCE - the .c's per-iteration render is source order)
    float dy_step = (float)((double)dy_f / (double)two_steps);   // asm 0x4A24D1-0x4A24D3
    float cursor_x = (float)s->x;                             // asm 0x4A24D9-0x4A24E5 (fild/fstp var_1C)
    float cursor_y = (float)s->y;                             // asm 0x4A24E9-0x4A24ED (fstp [the reused s slot])
    int prev_x = -999;                                        // asm 0x4A24C4 (mov ebx, 0FFFFFC19h)
    int prev_y = -999;                                        // asm 0x4A24CB (mov ebp, ebx)

    for (int i = 0; i < two_steps; ++i)                       // asm 0x4A24F1 (the jle) / 0x4A2577-0x4A2586 (inc/cmp/jl - the shared ++i block)
    {
        cursor_x += dx_step;                                  // asm 0x4A24F7-0x4A24FF (float per-step store)
        cursor_y += dy_step;                                  // asm 0x4A2503-0x4A250B
        int cur_x = (int)cursor_x;                            // asm 0x4A250F-0x4A251C (ftol)
        int cur_y = (int)cursor_y;                            // asm 0x4A2518-0x4A251E (ftol)
        if (cur_x == prev_x && cur_y == prev_y)               // asm 0x4A2523-0x4A252B (the same-tile test vs prev -999/-999)
            continue;                                         // asm 0x4A2577 (the shared ++i/cmp/jl block)
        prev_x = cur_x;                                       // asm 0x4A2536
        prev_y = cur_y;                                       // asm 0x4A253F
        if (this_->vfptr->passableTile2(this_, cursor_x, cursor_y) == 0)   // asm 0x4A2541-0x4A2549 (vtbl idx 104 @0x1A0 = passableTile2 - the raw float cursor bits)
        {
            rPoint->y = cur_y;                                // asm 0x4A259F ([eax+4], the y-first order)
            rPoint->x = cur_x;                                // asm 0x4A25A2 ([eax])
            return 0;                                         // asm 0x4A25A4-0x4A25AD (the blocking tile reported - no ++i)
        }
        double dy_delta = (double)d->y - (double)cursor_y;    // asm 0x4A254B-0x4A2552 (fild d->y; fsub)
        double dx_delta = (double)d->x - (double)cursor_x;    // asm 0x4A2556-0x4A2558 (fild d->x; fsub)
        double dist = sqrt(dx_delta * dx_delta + dy_delta * dy_delta);   // asm 0x4A255C-0x4A2566 (dx^2 + dy^2, the 53-bit chain)
        if (dist <= range)                                    // asm 0x4A256C-0x4A2575 (fcomp range; test ah,41h; jnz -> return 1; no NaN source - the literal MATCH row)
            return 1;                                         // asm 0x4A258C-0x4A2598
    }
    return 1;                                                 // asm 0x4A258C-0x4A2598 (the exhausted walk)
}
#pragma float_control(pop)

// Replacement for RGE_Moving_Object::addFuturePath. Original:
// @ 0x004A35C0 (idb_functions.h:2171). Range [0x4A35C0, 0x4A375E),
// retn 10h, int, SEH frame (the TWO allocation-unwind funclets
// 0x642840/0x64284B: operator_delete on the half-constructed
// BPathData for EH states 0 and 1 - documented; the written
// pointer-call form does not emit EH state - the setVelocity/
// addCurrentPath precedent). SEMANTIC: installs the caller's path as
// the unit's FUTURE path: the null/<2-waypoints gates; the
// mFuturePathData node created (sizeof(BPathData)) or reset; when
// pathType != 2, a world-registered trimmed copy is built (the first
// + last waypoints of the caller's path via RGE_Game_World__createPath
// / addWaypointAtStart/End, stamped with world->world_time) into a
// SECOND BPathData node (waypointLevel 2, setReadyToFollowPath,
// range/targetID stamped), linked to the future node via
// setLinkedPath(lP, 1); the future node gets setReadyToPath(caller's
// path, range, targetID) and waypointLevel = pathType. On the second
// BPathData allocation failure the caller's path is destroyed via
// RGE_Game_World__destroyPath2. Returns 1 (the null/<2-waypoints and
// the first-allocation-failure paths return 0).
// Decode (asm 0x4A35C0-0x4A375E): the gates (pathToDestroy null, asm
// 0x4A35E3-0x4A35E5; waypointsCount @+0xC < 2 @0x4A35E7-0x4A35ED ->
// loc_4A362C = return 0); the mFuturePathData @+0x11C branch (reset
// @0x4A3642, or operator_new + BPathData__BPathData + the store
// @0x4A35F9-0x4A362A, null -> return 0 via the unwind); the pathType
// == 2 gate (asm 0x4A3647-0x4A364F -> loc_4A3712 = the tail);
// RGE_Game_World__createPath((*base_game)->world) (asm 0x4A3655-
// 0x4A3660, idb_functions.h:6095); the trimmed copy: waypoint(newPath,
// 0) -> addWaypointAtStart (asm 0x4A366F-0x4A3680), waypoint(newPath,
// waypointsCount-1) -> addWaypointAtEnd (asm 0x4A3685-0x4A3699);
// creationTimeValue @BPath+0x1C = world->world_time @+0x10 (asm
// 0x4A369E-0x4A36AE); the SECOND BPathData (operator_new +
// BPathData__BPathData, asm 0x4A36A5-0x4A36D2; null ->
// destroyPath2(world, newPath) @0x4A3701-0x4A370D then the tail);
// waypointLevel @+0xC = 2 + setReadyToFollowPath(new_bd, the created
// path) + range @+0x20 / targetID @+0x24 stamps (asm 0x4A36E2-
// 0x4A36FF); the tail (asm 0x4A3712-0x4A3743):
// setReadyToPath(mFuturePathData, newPath, r, tID), waypointLevel =
// pathType, setLinkedPath(mFuturePathData, new_bd, 1) when new_bd;
// return 1 (asm 0x4A3748-0x4A375E).
int __fastcall RGE_Moving_Object__addFuturePath_new(
    RGE_Moving_Object* this_, DWORD dummy, BPath* newPath, int tID,
    float r, int pathType)
{
    if (newPath == 0 || newPath->waypointsCount < 2)          // asm 0x4A35E3-0x4A35ED
        return 0;                                             // asm 0x4A362C-0x4A363F (loc_4A362C)

    if (this_->mFuturePathData != 0)                          // asm 0x4A35EF-0x4A35F7 (@+0x11C)
        BPathData__reset(this_->mFuturePathData);             // asm 0x4A3642 (idb_functions.h:2507)
    else
    {
        BPathData* created = (BPathData*)operator_new_internal(sizeof(BPathData));   // asm 0x4A35FB (52 = sizeof(BPathData), walked)
        BPathData* constructed = 0;                           // asm 0x4A3607-0x4A3613
        if (created != 0)                                     // asm 0x4A3607-0x4A360D
            constructed = BPathData__BPathData(created);      // asm 0x4A3611 (idb_functions.h:2505)
        this_->mFuturePathData = constructed;                 // asm 0x4A3624 (@+0x11C)
        if (constructed == 0)                                 // asm 0x4A361A-0x4A362A
            return 0;                                         // asm 0x4A362C-0x4A363F (the SEH-unwind exit)
    }

    BPathData* linked = 0;                                    // asm 0x4A35DF (xor edi,edi)
    if (pathType != 2)                                        // asm 0x4A3647-0x4A364F
    {
        BPath* world_path = RGE_Game_World__createPath((RGE_Game_World*)(*base_game)->world);   // asm 0x4A3655-0x4A3660 (idb_functions.h:6095; the canonical base_game deref; the TRIBE_World family cast - the .c's own render carries it)
        if (world_path != 0)                                  // asm 0x4A3665-0x4A3669
        {
            BVector first_wp;                                 // the asm's wpIndex buffer
            BVector last_wp;                                  // the asm's outVector buffer
            BPath__addWaypointAtStart(world_path, BPath__waypoint(newPath, &first_wp, 0));   // asm 0x4A366F-0x4A3680 (idb_functions.h:2523)
            BPath__addWaypointAtEnd(world_path, BPath__waypoint(newPath, &last_wp, newPath->waypointsCount - 1));   // asm 0x4A3685-0x4A3699 (idb_functions.h:2524)
            world_path->creationTimeValue = (*base_game)->world->world_time;   // asm 0x4A369E-0x4A36AE (@BPath+0x1C; world_time @+0x10)
            BPathData* second = (BPathData*)operator_new_internal(sizeof(BPathData));   // asm 0x4A36AB (52 = sizeof(BPathData))
            if (second != 0)                                  // asm 0x4A36BD-0x4A36C7
                linked = BPathData__BPathData(second);        // asm 0x4A36CB (idb_functions.h:2505)
            if (linked != 0)                                  // asm 0x4A36D6-0x4A36E0
            {
                linked->waypointLevel = 2;                    // asm 0x4A36E5 (@+0xC)
                BPathData__setReadyToFollowPath(linked, world_path);   // asm 0x4A36EC (idb_functions.h:2514)
                linked->range = r;                            // asm 0x4A36F9 (@+0x20)
                linked->targetID = tID;                       // asm 0x4A36FC (@+0x24)
            }
            else
            {
                RGE_Game_World__destroyPath2((RGE_Game_World*)(*base_game)->world, newPath);   // asm 0x4A3701-0x4A370D (idb_functions.h:6097; the TRIBE_World family cast)
            }
        }
    }

    BPathData* future = this_->mFuturePathData;               // asm 0x4A371B (@+0x11C)
    BPathData__setReadyToPath(future, newPath, r, tID);       // asm 0x4A3723 (idb_functions.h:2513)
    future->waypointLevel = pathType;                         // asm 0x4A3734 (@+0xC)
    if (linked != 0)                                          // asm 0x4A3732-0x4A3738
        BPathData__setLinkedPath(future, linked, 1);          // asm 0x4A3743 (idb_functions.h:2515)

    return 1;                                                 // asm 0x4A3748-0x4A375E
}

// PRECISION SCOPE: rule (k) bracket - the two hoisted step divisions.
#pragma float_control(precise, on, push)
int __fastcall RGE_Moving_Object__firstTileAlongLine_new(
    RGE_Moving_Object* this_, DWORD dummy, XYPoint* s, XYPoint* d,
    XYPoint* rPoint, int tType1, int tType2, int checkPassability)
{
    double dx_f = (double)(d->x - s->x);                      // asm 0x4A272C-0x4A2735 (the int sub; fild - the exact widening)
    double dy_f = (double)(d->y - s->y);                      // asm 0x4A2739-0x4A2747

    if (dx_f == 0.0 && dy_f == 0.0)                           // asm 0x4A274B-0x4A2765 (the two fcomp/test ah,40h gates; int origins - no NaN source, the literal form matches)
        return 0;                                             // asm 0x4A276B-0x4A2774

    double selected = fabs(dx_f) >= fabs(dy_f) ? dx_f : dy_f;   // asm 0x4A2777-0x4A2786 (fabs/fabs/fcompp; test ah,1 jz -> dx)
    int steps = (int)selected;                                // asm 0x4A278A-0x4A27A6 (ftol; both selection paths)
    int two_steps = abs(steps) * 2;                           // asm 0x4A278F-0x4A27A8 (cdq/xor/sub abs; shl 1)
    if (two_steps == 0)                                       // asm 0x4A27BE + 0x4A27FB (test eax, eax AFTER the abs/shl - eax = two_steps, always >= 0: the gate fires only when the dominant delta truncates to 0; a negative-dominant line WALKS - the M8c-era "SIGNED PRE-ABS / asm wins" reading was a register misattribution; the .c's `v10 <= 0` render was right)
        return 0;                                             // asm 0x4A276B-0x4A2774

    float dx_step = (float)((double)dx_f / (double)two_steps);   // asm 0x4A27AE-0x4A27CB (fdiv at 53-bit; fstp m32 - rule (c); hoisted once)
    float dy_step = (float)((double)dy_f / (double)two_steps);   // asm 0x4A27CF-0x4A27D1
    float cursor_x = (float)s->x - dx_step;                   // asm 0x4A27D7-0x4A27E7 (fild s->x; fsub dx_step; fstp - the cursor starts ONE STEP BACK so the first probe covers the start tile)
    float cursor_y = (float)s->y - dy_step;                   // asm 0x4A27EB-0x4A27F7
    RGE_Map* map = this_->owner->world->map;                  // asm 0x4A27B6-0x4A27C8 (owner @+0x18 -> world @+0x8C -> map @+0x34)
    int prev_x = -999;                                        // asm 0x4A27B9
    int prev_y = -999;                                        // asm 0x4A27C6

    for (int i = 0; i < two_steps; ++i)                       // asm 0x4A27FB (the jle) / 0x4A2883-0x4A2892 (inc/cmp/jl - the shared ++i block)
    {
        cursor_x += dx_step;                                  // asm 0x4A2801-0x4A2809 (float per-step store)
        cursor_y += dy_step;                                  // asm 0x4A280D-0x4A2815
        int cur_x = (int)cursor_x;                            // asm 0x4A2819-0x4A2826 (ftol)
        int cur_y = (int)cursor_y;                            // asm 0x4A2822-0x4A2828 (ftol)
        if (cur_x == prev_x && cur_y == prev_y)               // asm 0x4A282D-0x4A2835 (the same-tile test)
            continue;                                         // asm 0x4A2883 (the shared ++i/cmp/jl block)
        prev_x = cur_x;                                       // asm 0x4A283B
        prev_y = cur_y;                                       // asm 0x4A283F
        int terrain = RGE_Map__get_terrain(map, cur_x, cur_y);   // asm 0x4A2837-0x4A2841 (idb_functions.h:1994; mapcol=cur_x, maprow=cur_y; the u8 result - the int args truncate to the callee's __int16 reads)
        if (terrain == tType1 || terrain == tType2)           // asm 0x4A2846-0x4A285F (and 0FFh; vs both types)
        {
            if (checkPassability == 1)                        // asm 0x4A2861-0x4A2866 (the exact-1 gate)
            {
                if (this_->vfptr->passableTile2(this_, cursor_x, cursor_y) == 1)   // asm 0x4A2868-0x4A287E (vtbl idx 104 @0x1A0; the raw float cursor bits; the hit = EXACTLY 1)
                {
                    rPoint->y = cur_y;                        // asm 0x4A28A8 ([eax+4], y-first)
                    rPoint->x = cur_x;                        // asm 0x4A28AB ([eax])
                    return 1;                                 // asm 0x4A28B0-0x4A28B9
                }
            }
        }
    }
    return 0;                                                 // asm 0x4A2898-0x4A28A1 (the exhausted walk)
}
#pragma float_control(pop)

// Replacement for RGE_Moving_Object::canPathWithObstructions. Original:
// @ 0x004A1FF0 (idb_functions.h:2149). Range [0x4A1FF0, 0x4A21C1),
// retn 18h, BOOL, SEH frame (the dtor-jmp chunk 0x6427A0: the unwind
// runs BPath__destructor on the local rPath - documented; the written
// pointer-call form does not emit EH state - the setVelocity/
// addCurrentPath precedent). SEMANTIC: the obstruction-reporting
// canPath: resolves the target object, the zone map (the cached
// zoneMapIndex), and the unit/target zones; when the zones differ the
// goal must be within range; then arms a fresh local BPath, CLEARS
// the caller's obstructions array count (ManagedArray_int_::
// numberValue @+4), runs the tile-path query (the unobstructible
// params + the pathDistance out), copies the unobstructibles into the
// caller's array when unobstructibleGroupID != -1, destroys the local
// path, and returns the query result. Every failing gate returns 0.
// Decode (asm 0x4A1FF0-0x4A21C1): the target resolve
// RGE_Game_World__object(owner->world, oID) (asm 0x4A2012-0x4A201C,
// idb_functions.h:6086; the TRIBE_World family cast) - null -> the
// eax=0 exit; the zone-map chain (the cached zoneMapIndex @+0x80 gate
// + RGE_Zone_Map_List__get_zone_map (the return discarded) +
// get_zone_map2 -> ebx, null -> the eax=0 exit, asm 0x4A202B-0x4A2085);
// get_zone_info(zm, (int)world_x, (int)world_y) & 0xFF -> ebp (asm
// 0x4A209F-0x4A20AC); get_zone_info(zm, target->world_x,
// target->world_y) & 0xFF (asm 0x4A20BF-0x4A20C4); the same-zone skip
// to loc_4A211F (asm 0x4A20C9-0x4A20CF; the range reload into ebp
// between the cmp and the jz); the cross-zone withinRange(zm, start,
// goal, range, 0) gate - false -> the eax=0 exit (asm 0x4A20D1-
// 0x4A2119 -> loc_4A21AF); BPath__BPath(&rPath, -1) (asm 0x4A211F-
// 0x4A2125); obstructions->numberValue @+4 = 0 (ManagedArray_int_,
// idb_types.h:1521 - the .c's *((_DWORD*)obstructions + 1) render;
// asm 0x4A213C); findTilePath with the 12 args (the ftol-pair
// starts/goals, asm 0x4A2143-0x4A217C, idb_functions.h:2543); the
// bool -> esi (and eax,0FFh, asm 0x4A2181); copyUnobstructibles when
// unobstructibleGroupID != -1 (asm 0x4A2186-0x4A2197,
// idb_functions.h:2546); BPath__destructor at the exit (asm
// 0x4A21A8); return esi (asm 0x4A21AD).
BOOL __fastcall RGE_Moving_Object__canPathWithObstructions_new(
    RGE_Moving_Object* this_, DWORD dummy, int targetID, float aRange,
    float* pathDistance, int unobstructiblePlayerID,
    int unobstructibleGroupID, void* obstructions)
{
    RGE_Static_Object* target = RGE_Game_World__object((RGE_Game_World*)this_->owner->world, targetID);   // asm 0x4A2012-0x4A201C (idb_functions.h:6086; the TRIBE_World family cast)
    if (target == 0)                                          // asm 0x4A2023-0x4A2025
        return 0;                                             // asm 0x4A21AF (loc_4A21AF; eax = the object call's 0)

    if (this_->zoneMapIndex == -1)                            // asm 0x4A2037-0x4A203A (@+0x80)
        RGE_Zone_Map_List__get_zone_map(                      // asm 0x4A2062 (idb_functions.h:6122 - the return discarded, the out index kept)
            this_->owner->world->map->map_zones,              // asm 0x4A205C (RGE_Map+0xBF7C, idb_types.h:2941)
            this_->owner->world->terrains[this_->master_obj->terrain],   // asm 0x4A205B (terrains @+0x60; terrain @+0x76)
            this_->owner->world->terrain_size,                // asm 0x4A2053 (@+0x5E)
            &this_->zoneMapIndex);

    RGE_Zone_Map* zone_map = RGE_Zone_Map_List__get_zone_map2(this_->owner->world->map->map_zones, this_->zoneMapIndex);   // asm 0x4A207C (idb_functions.h:6123)
    if (zone_map == 0)                                        // asm 0x4A2083-0x4A2085
        return 0;                                             // asm 0x4A21AF (eax = the get_zone_map2 null)

    unsigned int unit_zone = RGE_Zone_Map__get_zone_info(zone_map, (int)this_->world_x, (int)this_->world_y);   // asm 0x4A209F-0x4A20AC (idb_functions.h:6112; the ftol pair; and 0FFh)
    unsigned int goal_zone = RGE_Zone_Map__get_zone_info(zone_map, (int)target->world_x, (int)target->world_y);   // asm 0x4A20BF-0x4A20C4
    int range = aRange;                                       // asm 0x4A20CB (the reload between the cmp and the jz)

    if (goal_zone != unit_zone)
    {
        XYPoint start;                                        // the asm's start @-0x50
        XYPoint goal;                                         // the asm's goal @-0x48
        start.x = (int)this_->world_x;                        // asm 0x4A20D1-0x4A20DC
        start.y = (int)this_->world_y;                        // asm 0x4A20D9-0x4A20E8
        goal.x = (int)target->world_x;                        // asm 0x4A20E5-0x4A20F4
        goal.y = (int)target->world_y;                        // asm 0x4A20EC-0x4A20FD
        if (RGE_Zone_Map__withinRange(                        // asm 0x4A2112 (idb_functions.h:6113)
                zone_map,
                start, goal,                                  // asm 0x4A210F/0x4A210D/0x4A2108 (the XYPoint by value)
                range,                                        // asm 0x4A2107 (ebp)
                0) == 0)                                      // asm 0x4A2105 (new_goal)
            return 0;                                         // asm 0x4A21AF (eax = the withinRange 0)
    }

    // RAW-STORAGE PATTERN (owner-approved application, 2026-11-18):
    // BPath's const member deletes the default ctor - the local is
    // aligned raw storage + the placement pointer, constructed by the
    // raw BPath__BPath call (the canQuickPath precedent)
    __declspec(align(4)) unsigned char rPath_storage[sizeof(BPath)];   // the asm's rPath @-0x40 (raw storage)
    BPath* const rPath = (BPath*)rPath_storage;               // asm 0x4A211F-0x4A2125 (the placement-cast)
    BPath__BPath(rPath, -1);                                  // asm 0x4A2125 (idb_functions.h:2520)

    ((ManagedArray_int_*)obstructions)->numberValue = 0;      // asm 0x4A213C (mov dword ptr [ecx+4], 0 - the caller's obstruction count pre-clear; ManagedArray_int_ idb_types.h:1521)

    bool found = PathingSystem__findTilePath(                 // asm 0x4A217C-0x4A2186 (idb_functions.h:2543)
        pathSystem,
        (int)this_->world_x, (int)this_->world_y,             // asm 0x4A2170-0x4A217C (the ftol pair)
        (int)target->world_x, (int)target->world_y,           // asm 0x4A215E-0x4A2167 (the ftol pair)
        this_,                                                // asm 0x4A2155
        aRange,                                               // asm 0x4A2154 (ebp)
        targetID,                                             // asm 0x4A2153
        pathDistance,                                         // asm 0x4A2152 (rLength)
        unobstructiblePlayerID,                               // asm 0x4A2151
        unobstructibleGroupID,                                // asm 0x4A214D
        rPath,                                                // asm 0x4A2148
        false);                                               // asm 0x4A2143 (useRangeWithoutTarget)

    if (unobstructibleGroupID != -1)                          // asm 0x4A2186-0x4A2189 (cmp ebx, -1)
        PathingSystem__copyUnobstructibles(pathSystem, (ManagedArray_int_*)obstructions);   // asm 0x4A2197 (idb_functions.h:2546)

    BPath__destructor(rPath);                                 // asm 0x4A21A8-0x4A21AD (idb_functions.h:2521)
    return found;                                             // asm 0x4A21AD (mov eax, esi; the bool promotion = the and 0FFh)
}

// Replacement for RGE_Moving_Object::turn_towards. Original:
// @ 0x004A11B0 (idb_functions.h:2136). Range [0x4A11B0, 0x4A13B2),
// retn 0Ch, returns unsigned __int8. SEMANTIC: the facing manager -
// computes the facet-space angle to the target (angle_to_object),
// checks the dead band [~-0.5, 0.5] radians (already facing -> 1);
// turn_speed == 0 masters re-store the angle unchanged (the rate = 0,
// no rotation) and only fire the set_angle notification;
// turn_speed != 0 masters rate-limit: (world_time * 0.001 -
// turn_towards_time) < turn_speed -> 0 (still turning), else step ONE
// facet toward the target (the direction from the facet angle's
// sign), wrap into [0, 2pi], stamp turn_towards_time, fire the
// notification, re-probe and return 1 when the dead band is entered.
// Decode (asm 0x4A11B0-0x4A13B2): the facet floor (sprite null /
// facet_num < 8 -> 8, asm 0x4A11B9-0x4A11D5); the first angle probe
// (angle_to_object, asm 0x4A11E6-0x4A11EB, idb_functions.h:2135); the
// dead band (gate 1: fcom const_f_half; test ah,41h; jz -> the turn -
// the literal > MATCH row; gate 2: fcom flt_6545C4 = -0.5 @0x6545C4;
// test ah,1; jnz -> the turn - the {<, NaN} set -> X87_LT_NAN: a NaN
// facet angle CONTINUES to the turn, the literal < would wrongly
// return 1; the band -> return 1 via fstp st, asm 0x4A11F8-0x4A1210);
// the turn_speed == 0 gate (fcom [master+0xDC] @+0xDC = turn_speed;
// test ah,40h; jz -> the gradual path - the C3-taken set {== 0, NaN}
// -> the composed literal form per probe rule 5, asm 0x4A1216-0x4A1227);
// the GRADUAL path: the delta = turn_speed * (dbl_65C1A0 / facets) =
// 0 for turn_speed == 0 - the angle re-stored UNCHANGED (the .c's
// `v7 * (...)` render misassigns the x87 slot - the asm's fmulp
// multiplies TURN_SPEED, st(1) after the fild/fdivr - asm wins; the
// pre-wrap store asm 0x4A1245; the lower wrap: fcom 0; test ah,1 ->
// fadd const_f_2pi + the store, asm 0x4A124B-0x4A125C - NaN
// converges: NaN + 2pi stores the same NaN; the upper wrap: fcom
// const_f_2pi; test ah,41h; jnz -> the no-wrap discard (asm
// 0x4A12A3: fstp st) else fsub + the store, asm 0x4A1281-0x4A128B);
// the set_angle dispatch (asm 0x4A1262) - THE DISPATCH OFFSET CARRIES
// THE OWNER-SIDE CORRECTION (do not modify); return 1 (asm
// 0x4A126B-0x4A1271); the RATE-LIMITED path: scaled_time =
// (double)(the ZERO-EXTENDED world_time qword @+0x10) * 0.001f
// (fild the qword; fmul flt_657068 @0x657068; fst m32 - the
// store-round, rule (e), asm 0x4A12D3-0x4A12DD); the elapsed =
// scaled_time - turn_towards_time @+0xCC (asm 0x4A12E1); elapsed <
// turn_speed -> return 0 (fcomp [ecx+0xDC]; test ah,1; jnz, asm
// 0x4A12E7-0x4A12F2 - no NaN source, the literal MATCH row); the
// facet-step direction from the FACET ANGLE's sign (fcom
// const_f_zero on the facet angle; the flags persist across the
// fild/fdivr; test ah,1: negative -> fsubr [+C8]: angle - step;
// else fadd [+C8]: step + angle, asm 0x4A12F4-0x4A1317); the store
// (asm 0x4A131D) + the wraps (fcom 0 / fcom 2pi / +- const_f_2pi,
// asm 0x4A1323-0x4A1357 - NaN convergent); the set_angle dispatch
// (asm 0x4A1367); turn_towards_time = scaled_time (asm 0x4A136D-
// 0x4A1372); the SECOND angle probe (asm 0x4A137C-0x4A1381): gate 1
// the literal > MATCH (jz -> return 0, asm 0x4A1381-0x4A138C); gate 2
// the {<, NaN} set -> X87_LT_NAN (a NaN facet angle = still turning;
// the literal < would wrongly return 1, asm 0x4A138E-0x4A1399); the
// band -> return 1 (asm 0x4A139B-0x4A13B2).
// THE SET_ANGLE DISPATCH OFFSET carries the owner-side correction
// (do not modify - see the session ledger).
unsigned __int8 __fastcall RGE_Moving_Object__turn_towards_new(
    RGE_Moving_Object* this_, DWORD dummy, RGE_Static_Object* obj, float x,
    float y)
{
    int facets;                                               // asm 0x4A11B9-0x4A11D5 (the floor; the .c's v25)
    if (this_->sprite != 0 && this_->sprite->facet_num >= 8)
        facets = this_->sprite->facet_num;                    // asm 0x4A11C0-0x4A11CB (movsx @+0x70)
    else
        facets = 8;                                           // asm 0x4A11CD

    double angle = RGE_Moving_Object__angle_to_object(this_, obj, x, y);   // asm 0x4A11E6-0x4A11EB (idb_functions.h:2135)

    // the dead band [-0.5, 0.5]: already facing
    bool turn_needed = (angle > 0.5f) || X87_LT_NAN(angle, -0.5f);
    // gate 1: the literal > MATCH row (jz -> the turn; NaN falls to gate 2 = the asm);
    // gate 2: the {<, NaN} set -> X87_LT_NAN (a NaN facet angle = still turning)
    if (!turn_needed)
        return 1;                                             // asm 0x4A1205-0x4A1210 (the dead band; fstp st discards the angle)

    bool gradual = X87_EQ_NAN(this_->master_obj->turn_speed, 0.0f);
    // asm 0x4A1216-0x4A1227 (fcom [master+0xDC] @+0xDC = turn_speed; test ah,40h; jz -> the gradual path):
    // the C3-taken set is {== 0, NaN} - X87_EQ_NAN is the macro form of
    // probe rule 5's composed equality

    if (gradual)
    {
        // turn_speed == 0 (or NaN): the INSTANT-FACE snap. The gate's
        // fcomp (asm 0x4A121C) POPS turn_speed - the facet angle SURVIVES
        // in st(1) (it entered in st0 from angle_to_object and both
        // dead-band compares were fcom, no pop), so the fmulp st(1), st
        // multiplies THE ANGLE: facet_angle * (2pi/facets) = the target
        // offset in radians (facet_angle is the target offset in facet
        // units) -> new = the target absolute angle. (The M8c-era "fmulp
        // multiplies TURN_SPEED / the angle re-stored UNCHANGED" reading
        // used a wrong x87 stack model; the .c's v7 render was right.)
        double new_angle = angle * (6.2831853 / (double)facets)
                         + (double)this_->angle;              // asm 0x4A122D-0x4A1239 (fild facets; fdivr dbl_65C1A0 @0x65C1A0 = the double literal 6.2831853; fmulp st(1), st = THE ANGLE x step; fadd [+C8])
        this_->angle = (float)new_angle;                      // asm 0x4A1245 (fst [+C8] - the PRE-wrap store)
        if (X87_LT_NAN(new_angle, 0.0f))                      // asm 0x4A124B-0x4A1250 (fcom const_f_zero; test ah,1 - the {<, NaN} set: the macro form; NaN takes the += wrap and stores the same NaN)
        {
            new_angle += 6.2831855f;                          // asm 0x4A1252 (fadd const_f_2pi @0x65B388)
            this_->angle = (float)new_angle;                  // asm 0x4A125C (fstp [+C8])
        }
        else if (new_angle > 6.2831855f)                      // asm 0x4A1274-0x4A127F (fcom const_f_2pi; test ah,41h; jnz -> the no-wrap discard - the jnz targets the SKIP path: the literal > MATCH row; NaN never arrives - the {<, NaN} gate above took it)
        {
            new_angle -= 6.2831855f;                          // asm 0x4A1281 (fsub const_f_2pi)
            this_->angle = (float)new_angle;                  // asm 0x4A128B (fstp [+C8])
        }
        // else: the no-wrap discard (asm 0x4A12A3: fstp st - the pre-wrap store stands)
        this_->vfptr->set_angle(this_);                       // asm 0x4A1262-0x4A1268 (THE DISPATCH OFFSET CARRIES THE OWNER-SIDE CORRECTION - do not modify)
        return 1;                                             // asm 0x4A126B-0x4A1271
    }

    // the rate-limited path: one facet-step per turn_speed window
    double scaled_time = (double)(unsigned int)this_->owner->world->world_time * (double)0.001f;   // asm 0x4A12D3-0x4A12D7 (fild the ZERO-EXTENDED world_time qword @+0x10; fmul flt_657068 = 0.001 @0x657068 - the UNROUNDED 53-bit st0)
    float scaled_time_stored = (float)scaled_time;            // asm 0x4A12DD (fst [esp+y] - the store-round COPY; st0 stays unrounded and the elapsed chain consumes IT, not the copy)
    double elapsed = scaled_time - (double)this_->turn_towards_time;   // asm 0x4A12E1 (fsub [+CC] on the UNROUNDED st0 - the pre-fix form double-rounded through the float local: a rule-(e) misapplication - the rounded copy is consumed ONLY by the re-stamp)
    if (elapsed < (double)this_->master_obj->turn_speed)      // asm 0x4A12E7-0x4A12F2 (fcomp [ecx+0xDC]; test ah,1; jnz -> return 0 - still turning; no NaN source - the literal MATCH row)
        return 0;                                             // asm 0x4A13A7-0x4A13B2 (fstp st discards the facet angle)

    double facet_step = 6.2831853 / (double)facets;           // asm 0x4A12FE-0x4A1302 (fild var_C; fdivr dbl_65C1A0 @0x65C1A0 = the double literal 6.2831853)
    if (X87_LT_NAN(angle, 0.0))                               // asm 0x4A12F8-0x4A130D (fcomp const_f_zero - POPS the facet angle; the flags read at 0x4A1308 persist across the fild/fdivr; test ah,1 - the {<, NaN} set: the macro form)
        angle = (double)this_->angle - facet_step;            // asm 0x4A130F (fsubr [+C8]: st0 = this->angle - facet_step - the STORED radians stepped by one facet; the facet-units operand of the pre-fix form was the unit-mixing bug)
    else
        angle = facet_step + (double)this_->angle;            // asm 0x4A1317 (fadd [+C8] - the positive-facet direction)
    this_->angle = (float)angle;                              // asm 0x4A131D (fstp [+C8])
    if (X87_LT_NAN(this_->angle, 0.0f))                       // asm 0x4A1323-0x4A133A (fcomp 0 on the RELOADED stored value; test ah,1 - the {<, NaN} set: the macro form)
        this_->angle = this_->angle + 6.2831855f;             // asm 0x4A133C (fadd const_f_2pi)
    else if (this_->angle > 6.2831855f)                       // asm 0x4A1344-0x4A134F (fcom const_f_2pi; test ah,41h; jnz -> no wrap - the jnz targets the SKIP path: the literal > MATCH row, taken = {> 2pi} strictly; NaN never arrives - the {<, NaN} gate above took it)
        this_->angle = this_->angle - 6.2831855f;             // asm 0x4A1357 (fsub const_f_2pi)
    // both wrap paths converge on the dispatch (asm 0x4A135D: fstp [+C8])
    this_->vfptr->set_angle(this_);                           // asm 0x4A1363-0x4A1367 (THE DISPATCH OFFSET CARRIES THE OWNER-SIDE CORRECTION - do not modify)
    this_->turn_towards_time = scaled_time_stored;            // asm 0x4A136D-0x4A1372 (mov ecx, [esp+y]; mov [+CC], ecx - the ROUNDED store-round bits re-stamped - the timestamp for the next elapsed gate)

    double angle2 = RGE_Moving_Object__angle_to_object(this_, obj, x, y);   // asm 0x4A137C-0x4A1381 (the re-probe)
    if (angle2 > 0.5f)                                        // asm 0x4A1381-0x4A138C (fcom const_f_half; test ah,41h; jz -> return 0 - the literal MATCH row)
        return 0;                                             // asm 0x4A13A7-0x4A13B2 (fstp st; xor al,al)
    if (X87_LT_NAN(angle2, -0.5f))                            // asm 0x4A138E-0x4A1399 (fcom flt_6545C4; test ah,1; jnz -> return 0 - the {<, NaN} set: a NaN facet angle = still turning)
        return 0;                                             // asm 0x4A13A9-0x4A13B2
    return 1;                                                 // asm 0x4A139B-0x4A13B2 (al = 1 - facing achieved)
}

#define LINE_LOGGING_ENABLED

#ifdef LINE_LOGGING_ENABLED
#define fprintf_logger fprintf
#else
#define fprintf_logger(...) void(0)
#endif

// ---------------------------------------------------------------------------
// PASS 1 - THE ALIGNED TRANSCRIPTION (owner-directed two-pass protocol
// 2026-11-20): the IDA rendering transcribed 1:1 - the v-names, the
// BPath__waypoint copy-out carve buffers (each fetch lives in its own
// out-buffer; the multi-fetch aliasing rule), the label/goto structure -
// with VALUE-LEVEL FIXES ONLY:
//  - the asm-verified back-edges: the .c's five synthetic while(1) nests are
//    the binary's single dispatch loop loc_4A48A2; the reloop joins carry
//    their asm citations per site (J1-J7),
//  - the X87 NaN-polarity macros at the divergent compares (rule (i)),
//  - the C1/C2 mask folds (the u8/bool returns read AL - the literal masks
//    fold as no-ops; the C4806 finding),
//  - the pointer-to-slot derefs (*numberObSystemCollisions - the sweep),
//  - the base-pointer corrections (&this[1].ei.y2 / v54[2].damaged_lately_
//    timer / v96[2].mMaximumPathingGroupSize -> linked_child @+0x1E0 /
//    linked_owner @+0x1DC; asm lea/cmp verified; ei @+0x90 with y2 @+0x14
//    completes +0x13C+0xA4 = +0x1E0 exactly),
//  - the timestamp sources (owner->world->world_time; RGE_Player.world
//    @+0x8C -> world_time @+0x10 - FINDING D at all five stamps),
//  - the __ftol (int) casts x4, the dd-constant f-suffixes.
//
// Original: RGE_Moving_Object::doIntelligentMovementUpdate @0x004A47B0
// [0x4A47B0,0x4A5E6B) = 5819 bytes (lst :280690/:282654), int __thiscall
// (this_), retn 0. Caller: RGE_Moving_Object__intelligentMovementUpdate.
//
// SEMANTIC (gameplay): the per-tick intelligent-movement orchestrator for
// one moving unit - resolves the unit's BPathData against its BPath, runs
// the waypoint-increment / arrival / state-2 repath / movement-integration
// state machine, probes the obstruction system for fresh blockers
// (accumulating their ids into collisionUnits - the accumulator feeds the
// next findLowLevelPath's collision-count argument), pauses or repaths
// around higher-priority blockers, then calls moveUnit (keep moving),
// resets+stops (fully arrived) or stops (range reached).
//
// Approved deviations (2026-11-20, the presentation round):
//  (A) ReturnPath raw storage (BPath's const member deletes the default
//      ctor; the canQuickPath precedent, per-application approval),
//  (B) v157/v150 typed int (the .c pointer mis-types; the asm inc/test
//      proves the counters); the v94/v103/v97/v128/v20/v21/v82/v84 IDA
//      intermediates collapse into the indexed walks, the named field
//      reads and the named vtbl dispatches - every collapse value-identical
//      and cited at its site,
//  (C) the var_248 slot's dead float copy as v158_normCopy (asm 0x4A546E
//      fst; never read - transcribed for 1:1 fidelity).
//
// Owned callees route through the hooks per the hook-routing ruling
// (precalcDistToTarget/precalcDistToGoal/incrementWaypoint/checkDistance/
// checkRange/moveUnit + the hooked vtbl slots); resolveCollisions,
// smoothMidLevelPath, find_path, findTilePath, findLowLevelPath and
// findObSystemCollisions execute the unowned originals.
//
// PRECISION SCOPE (the whole-function precise wrap, owner-directed
// 2026-11-20): the original ran this body ENTIRELY on the x87 at PC53
// (every chain unrounded-until-store); the /fp:fast float-final-store
// shrink licensed mulss/addss/sqrtss float chains at any site whose
// result feeds a 53-bit-consumed comparison. The unit-5404 capture
// proved the flip: v151's shrunk float chain vs the orig's
// FLOAT(53-bit chain) inverted the label_103 tail test at the exact
// coincidence boundary (the position == the ReturnPath's tail after
// the arrival teleport). The whole-function precise wrap reproduces
// the original's uniform PC53 model (the widened chains emit
// mulsd/addsd/sqrtsd, one cvtsd2ss per store-round); the residual
// 80->53-bit x87-vs-SSE anomaly is the recorded accepted class.
#pragma float_control(precise, on, push)
int __fastcall RGE_Moving_Object__doIntelligentMovementUpdate_new(
    RGE_Moving_Object* this_, DWORD dummy)
{
    (void)dummy;
    // ---- the pass-2 locals: the roles named (the v-names dissolved; the
    // draft's mapping, site-verified against the baseline carve slots);
    // every carve buffer remains a DISTINCT object (the copy-out aliasing
    // rule). v25 is dropped: both its assignments are `= activePath`
    // (0x4A5360/0x4A5164) - it aliases the loop-carried path exactly.
    // v35 dissolves into the two pathFound stores at its LABEL_50 sites.
    double rawDt;                     // st7 - the raw dt (the store-round lands in deltaTime)
    RGE_Master_Moving_Object* masterHead;   // v6 - the master object (the OB-system domain select)
    BPath* resolvedPath;              // v8 - the loop-resolved BPath (the pre-install active path)
    BPathData* linkWalk;              // i - the dead linkedPath walk cursor (the final value unread)
    double goalDistRaw;               // v10 - the precalcDistToGoal raw return
    int pathState;                    // v13 - the pathData->state read at the dispatch
    BVector* unitPos;                 // v14 - ebx, = &positionValue
    BPath* topPath;                   // v17/v112/v113 - the highestLinkedPath result (BPath*)
    RGE_Game_World* worldNow;         // v18
    int topPathId;                    // v19
    BPath* newPath;                   // v22 - edi, the repath candidate (createPath's product)
    int fp1Result;                    // v33 - the find_path #1 status
    int ignoreCountArm2;              // v37 - the arm-2 ignore-list build count
    RGE_Player* ownerNow;             // v43 - the .c's TRIBE_Player* owner (the dual-naming cast)
    int lowResult;                    // v47 - the findLowLevelPath status
    double targetDist;                // v59 - the distance_to_object return (UNROUNDED st0)
    int fp2Result;                    // v65 - the find_path #2 status
    double tailNorm;                  // v67 - the ReturnPath tail norm (UNROUNDED st0)
    double tileTailDist;              // v70 - the findTile tail norm (UNROUNDED st0)
    int v71;                          // the ReturnPath copy-loop counter
    int v72;                          // _BOOL1 - the copy-loop pre-test
    double v74;                       // st7 - the post-smooth length read
    BPathData* v75;
    BPath* v76;                       // ebp - DEAD WALK-ARM PATH store (never read; 1:1)
    BVector* v77;
    BVector* v78;
    BPathData* v79;
    BPathData* v80;
    BPathData* v81;
    BVector* v83;
    BVector* v85;
    int reachedResult;                // v86 - the calculateReachedPoint result
    BVector* v87;
    float v88;                        // the moveTarget .y component temp
    int v90;                          // the walk-arm waypoint index
    int v91;                          // the walk-arm OB build count
    RGE_Static_Object* v95;           // the OB-pass record's blocker
    RGE_Static_Object* v96;           // the blocker's world object (+ the linked_owner swap)
    BPath* v98;                       // the highestLinkedPathPointer in the OB pass
    int v99;                          // the pathData->targetID in the OB pass
    unsigned __int16 v100;            // ax - the blocker's master object_group
    unsigned int v101;                // ebx - the path creationTimeValue
    TRIBE_Building_Object* v106;      // the pause-loop blocker object
    RGE_Static_Object* v107;
    int pausePriority;                // v108 - ebp - the blocker's movementPriority
    BPathData* linkedNow;             // v109 - the linkedPath takeover pointer
    int v114;                         // the range-gate topPath waypoint count
    BPathData* v115;                  // the range-gate pathData read
    BVector* v116;
    float v117;            // the unitRadius arg slot
    float v118;            // the range arg slot
    int v119;              // the findTile goalX
    int v120;              // the targetID arg slot
    int v121;              // the findTile goalY
    int v122;              // the ignore-count arg slot
    float v123;            // the findTile range arg slot
    float dest_target_radius;   // the find_path #1 dy arg slot
    float dest_target_radiusa;  // the find_path #2 dy arg slot
    int dest_target_radiusb;    // the findTile targetID arg slot
    float v127;            // the radius sum #1
    float v129;            // the radius sum #2
    float v130;            // the find_path #1 range arg slot
    float v131;            // the find_path #2 range arg slot
    float* v132;           // the terrainTable arg slot
    int v133;              // the OB-probe ignore-count arg slot
    float v134;            // the failure-arm waypoint .y
    float v135;            // the range-gate range read
    BVector* v136;         // = &positionValue
    BVector* v137;
    int v138;              // the owner id arg slot
    BVector* v139;
    BVector* v140;
    BVector* v141;         // the findTile tail fetched buffer
    int pathFound;                    // v143 - var_268 - the path-found flag
    float goal;            // var_264 - the precalcDistToTarget store
    RGE_Map_Analysis* goala;    // the analysis-pointer live of the goal slot
    float goald;           // the fst tail-norm copy live of the goal slot
    int goalb;             // the record-loop counter live of the goal slot
    int goalc;             // the pause-loop counter live of the goal slot
    volatile int smoothUsed;          // v149 - var_260 - the smooth-used flag; VOLATILE: the /Ogtpy
                                      // compiler coalescing was reading a stale copy at the
                                      // install variant check (the STUBSET=1 -> PREINST=1 ->
                                      // INSTALL-OK=0 trace, 2026-11-20)
    int freshBlockers;                // v150 - DEVIATION (B): int - the fresh-blocker count
    float repathDist;                 // v151 - var_25C - the repath distance (store-rounded)
    float bestTailDist;               // v159 - var_244 - the best-tail norm (FLT_MAX-seeded,
                                      //  store-rounded at each write)
    int targetBlocked;                // v152 - the target-collision flag live of var_25C
    int collisionCount;               // v157 - DEVIATION (B): int - the collisionUnits accumulator
    RGE_Map_Analysis* mapAnalysis;    // v158 - var_248 - the analysis-pointer live
    float normCopy;                   // v158_normCopy - DEVIATION (C): the dead float live of var_248
    float deltaTime;                  // a3 - var_240 - the dt (STORE-ROUNDED at 0x4A47FA)
    float goalDist;                   // v161 - var_23C - the precalcDistToGoal store
    BUnitGroup* unitGroup;            // v161_group - the group-pointer live of var_23C (the .c's
                                      //  COERCE_FLOAT pun, typed at the reuse site)
    int passCounter;                  // v162 - var_238 - the pass counter
    BVector repathDelta;              // v163 - the repath delta carve
    float fp2TargetRadius;            // v164 - the target radius #2
    float fp1TargetRadius;            // v165 - the target radius #1
    BVector moveTarget;               // a4 - the current waypoint copy (the moveUnit arg)
    BVector fwdDir;                   // a2 - the calculateForward direction out
    BVector v172;                     // the walk-arm delta carve
    BVector arm2TailDelta;            // v167 - the arm-2 tail delta carve
    BVector tileTailDelta;            // v168 - the findTile tail-quality delta carve
    BVector headDelta;                // result - the state-2 head delta carve
    BVector maTailDelta;              // v169 - the label_103 tail delta carve
    BVector smoothTailDelta;          // v170 - the smooth-arm tail delta carve
    BVector boundsWp;                 // v173 - the LABEL_73 bounds-gate waypoint carve
    BVector projectedPos;             // p2 - the straightProjectedPosition result
    BVector maIntendedWp;             // v176 - the label_103 intended-waypoint fetch
    BVector maCopyWp;                 // v177 - the label_103 copy-loop fetch (REUSED by the
                                      //  range-gate .y fetch - the live reuse of the dead
                                      //  carve slot, cited there)
    alignas(BPath) unsigned char returnPathStorage[sizeof(BPath)];
    BPath* const ReturnPath = (BPath*)returnPathStorage;   // DEVIATION (A)
    BVector fp2TailWp;                // v184 - the find_path #2 tail waypoint fetch
    BVector fp2DyWp;                  // v180 - the find_path #2 dy waypoint fetch
    BVector fp2DxWp;                  // v182 - the find_path #2 dx waypoint fetch
    BVector headWp;                   // wpIndex - the state-2 head waypoint fetch
    BVector noFlagWp;                 // v179 - the no-flag rebuild's waypoint fetch
    BVector prepWp;                   // v188 - the flagged-arm head waypoint fetch
    BVector stubEndWp;                // v195 - the stub end waypoint fetch
    BVector smoothDyWp;               // v206 - the map-analysis arm's dy waypoint carve
    BVector fp1DxWp;                  // v181 - the find_path #1 dx waypoint fetch
    BVector smoothLastWp;             // v203 - the smooth-arm last waypoint fetch
    BVector fp1TailWp;                // v183 - the find_path #1 tail waypoint fetch
    BVector lowStartWp;               // v185 - the findLowLevelPath start waypoint fetch
    BVector arm2LastWp;               // v201 - the arm-2 last-waypoint fetch
    BVector arm2CurWp;                // v187 - the arm-2 current waypoint fetch
    BVector obProbeWp;                // v197 - the OB-probe waypoint fetch
    BVector rebuildEndWp;             // v189 - the rebuild end waypoint fetch
    BVector tileGoalYWp;              // v205 - the findTile goal-y waypoint fetch
    BVector tileGoalXWp;              // v191 - the findTile goal-x waypoint fetch
    BVector tileLastWp;               // v199 - the findTile last-waypoint fetch
    BVector findTileCurWp;            // outVector - the findTile current waypoint reload
    BVector failWpDy;                 // v190 - the failure-arm dy waypoint fetch
    BVector failReloadWp;             // v192 - the failure-arm waypoint reload
    BVector walkWp;                   // v194 - the walk-arm waypoint fetch
    BVector fwdWp;                    // v196 - the calculateForward waypoint fetch
    BVector reachedWp;                // v198 - the calculateReachedPoint waypoint fetch
    BVector moveTargetSrcWp;          // v200 - the moveTarget source fetch
    BVector nextWpNext;               // v204 - the next-waypoint fetch (the index+1 case)
    BVector nextWpCur;                // v202 - the next-waypoint fetch (the index case)
    RGE_Obstruction_Manager* obSystemToUse;
    RGE_Static_Object* targetObj;      // the out-param of precalcDistToTarget
    BPathData* successor;              // obj - var_258 - the successor BPathData
    ObsRecord* obRecord;               // record - the OB-pass record (the .c's *obja)
    int pauseIdx;                      // the pause-loop index (the .c's v103)
    int pauseTargetId;                // v105 - the pause-loop targetID read
    TRIBE_Building_Object* pauseBldg;  // v106
    RGE_Static_Object* pauseTarget;    // v107
    size_t bldgChildA;     // v38 - the arm-2 linked_child segment counter (the loop-bound
    size_t tgtChildA;      // v41 - the arm-2 target linked_child counter (rule: _countof
    size_t bldgChildB;     // v52 - the OB-probe own linked_child counter (with size_t
    size_t tgtChildB;      // v55 - the OB-probe target linked_child counter (counters))
    size_t bldgChildW;     // v92 - the walk-arm linked_child segment counter
    bool proceedToWalk;               // the pass-2 handoff flag (per-iteration; the install
                                      // arms' walk transfers set it - see the walk arm)
    bool runArm2;                     // the arm-2 stage flag (the LABEL_73 skip-gate dissolution)
    bool toLabel97;                   // the LABEL_97 stage flag (the LABEL_73 body's goto dissolution)
    bool arm2GateFailed;              // the arm-2 failure-gate flag (the LABEL_73-entry dissolution)
    bool atLabel99Fall;               // the LABEL_99 fall flag (the wpc/len-gate taken-path)
    bool toLabel103;                  // the LABEL_103 entry flag (the findTile len-fail + the LABEL_100 gate)

    // ---- the head (asm 0x4A47B0-0x4A48A0) ----
    if (RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 2))   // asm 0x4A47D1-0x4A47DA
        return 1;                                                     // 0x4A47DC-0x4A47E1
    rawDt = RGE_Game_World__getWorldTimeDeltaSeconds((RGE_Game_World*)((RGE_Player*)this_->owner)->world);
                                                                      // 0x4A47E6-0x4A47EF (owner @+0x18 ->
                                                                      //  world @+0x8C; the dt fetch via the OWNER's
                                                                      //  world - NOT base_game)
    deltaTime = (float)rawDt;                                         // 0x4A47FA (fstp a3 - the store-round)
    passCounter = 0;                                                  // 0x4A47FA-0x4A4802 (var_238 = edx)
    if (this_->pathData && this_->pathData->pauseTime > 0.0)          // 0x4A4800-0x4A4816 (fcomp const_f_zero;
    {                                                                 //  test ah,41h; jnz - the literal > MATCH row)
        this_->pathData->pauseTime = this_->pathData->pauseTime - deltaTime;
                                                                      // 0x4A4818-0x4A481F (float-float fsub; ONE
                                                                      //  fstp m32 - rule (d))
        if (this_->pathData->pauseTime > 0.0)                         // 0x4A4822-0x4A4836 (literal > MATCH row)
        {
            this_->vfptr->stopMoving(this_);                          // 0x4A4838-0x4A483C (slot +0x304 = 193)
            return 0;                                                 // 0x4A4842
        }
        this_->pathData->pauseTime = 0.0;                             // 0x4A4847 ([ecx+28h] = edx = 0)
    }
    masterHead = this_->master_obj;                                   // 0x4A484F (+0x14)
    obSystemToUse = (RGE_Obstruction_Manager*)*OBSystem_Land;         // 0x4A484A-0x4A4852 (the slot deref)
    if (masterHead->movement_type == 1)                               // 0x4A4856 (byte @+0x78)
        obSystemToUse = (RGE_Obstruction_Manager*)*OBSystem_Air;      // 0x4A485C-0x4A4861
    fwdDir.x = 0.0f;                                                  // 0x4A4865-0x4A4880
    fwdDir.z = 0.0f;
    fwdDir.y = 0.0f;
    moveTarget.x = 0.0f;                                              // 0x4A4886-0x4A4896
    moveTarget.z = 0.0f;
    moveTarget.y = 0.0f;
    collisionCount = 0;                                               // 0x4A489E (ONCE per call - the accumulator
                                                                      //  feeds the next findLowLevelPath's count)

    // =====================================================================
    // THE DISPATCH LOOP (asm loc_4A48A2; every back-edge lands here; the
    // reloop joins J1-J7 + the arrival + the state-4/5 + the config are the
    // `continue` sites)
    // =====================================================================
    for (;;)
    {
        // ---- S1: the resolve + the dead linked walk (loc_4A48A2-loc_4A48F3) ----
        if (!this_->pathData)
            return 1;                                                 // 0x4A48AA (jz loc_4A5D97)
        resolvedPath = this_->pathData->path;                         // 0x4A48B0 (+0x10)
        BPath* activePath = resolvedPath;                             // 0x4A48B5 (v8 = var_26C)
        if (!activePath)
        {
            resolvedPath = RGE_Game_World__path((RGE_Game_World*)(*base_game)->world, this_->pathData->pathID);
                                                                      // 0x4A48BB-0x4A48C8 (pathID @+0x14;
                                                                      //  (*base_game)->world @0x420)
            activePath = resolvedPath;
            if (!activePath)
                return 1;                                             // 0x4A48D5
            this_->pathData->path = resolvedPath;                     // 0x4A48DB-0x4A48E1 (the cache)
        }
        // asm 0x4A48E4-0x4A48F3: the linkedPath walk - the final eax is NEVER
        // consumed (loc_4A48EE self-loop; the next live read is targetObj);
        // genuinely dead in the original - transcribed 1:1.
        for (linkWalk = this_->pathData; linkWalk; linkWalk = linkWalk->linkedPath)
            ;
        // ---- S2: the precalc pair (0x4A48F5-0x4A492A) ----
        targetObj = 0;                                                // 0x4A48F9
        goal = (float)RGE_Moving_Object__precalcDistToTarget(this_, &targetObj);
                                                                      // 0x4A48F5-0x4A490D (owned; fstp goal - the
                                                                      //  double return's store-round)
        goalDistRaw = RGE_Moving_Object__precalcDistToGoal(this_, targetObj, goal);
                                                                      // 0x4A4911-0x4A4919 (owned)
        goalDist = (float)goalDistRaw;                                // 0x4A492A (fstp var_23C - the store-round)
        // ---- S3: the increment arm (0x4A492E-0x4A4942) ----
        if ((this_->pathData->mFlags.valueValue & 1) != 0)            // 0x4A492E (test [eax+30h], ecx; the mask
        {                                                             //  BBitVectorAND_2 = 1 (.rdata:0065BD94,
            g_dimuMark = 14;                                          // DIAG
                                                                      //  dd 1) = the mFlags bit-0 test)
            if (!RGE_Moving_Object__incrementWaypoint(this_, activePath, targetObj))
                                                                      // 0x4A4933-0x4A4940 (owned; test al,al)
                return 1;                                             // 0x4A4942
        }
        // ---- S4: the state dispatch (0x4A494E-0x4A4963) ----
        pathState = this_->pathData->state;                           // 0x4A494E (+0x1C)
        if (pathState == 4 || pathState == 5)                         // 0x4A4951-0x4A495D
        {
            g_dimuMark = 13;                                          // DIAG
            if (RGE_Moving_Object__checkDistance(this_, goal) == 1)   // 0x4A5CD0-0x4A5CDA (owned; push edi = goal)
                                                                      //  TRANSCRIPTION FIX (the paired-capture
                                                                      //  divergence): the argument is `goal`
                                                                      //  (var_264, the precalcDistToTarget store)
                                                                      //  - NOT goalDist (v161, the
                                                                      //  precalcDistToGoal store); the rename slip
                                                                      //  made checkDistance#1 return 1 one cycle
                                                                      //  early (the linked-chain one swap short)
                return 2;                                             // 0x4A5E31
            continue;                                                 // 0x4A5CE0 (reloop - the ASM RELOOP join)
        }
        proceedToWalk = false;                                        // the pass-2 handoff flag (fresh per pass)
        if (pathState != 2)
            proceedToWalk = true;                                     // 0x4A4963 (jnz loc_4A569F -> the walk arm)
        else
        {
            // ================= THE STATE-2 REPATH ARM (asm 0x4A496C-0x4A569D) ======
            g_dimuMark = 5;                                               // DIAG
            unitPos = &this_->positionValue;                              // 0x4A497A (lea ebx,[esi+0F4h]; +0xF4)
            BVector* v15 = BPath__waypoint(resolvedPath, &headWp, this_->pathData->waypoint);
                                                                          // 0x4A496F-0x4A4980 (waypoint @+0x18)
            BVector__operator_minus(unitPos, &headDelta, v15);            // 0x4A4985-0x4A4990 (this = positionValue)
            headDelta.z = 0.0f;                                           // 0x4A4999
            if (RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 1))   // 0x4A4995-0x4A49AB
            {
                // PATHING_CAP (owner-ruled never-baked, 2026-11-20): pathing.cpp:109
                // setHook(0x004A49B9, pathing_moving_object) hooks THIS call site
                // mid-function, re-routing the availablePathingAttempts call to
                // RGE_Player__availablePathingAttempts_new2 (the cap-extension logic;
                // the stub reuses the on-stack numWaitDelays push as its arg3). The
                // family is INACTIVE (setPathingHooks commented, dllmain.cpp:345); if
                // re-enabled it writes into dead original bytes under this hook, and
                // the new2 cap semantics are intentionally not carried here. Its
                // sibling pathing.cpp:105 (0x49D15C) sits inside the shipped
                // BMotion::updateMovement range with the same disposition.
                if (!RGE_Player__availablePathingAttempts((RGE_Player*)this_->owner,
                        this_->numberWaitDelays) || passCounter >= 3)     // 0x4A49AD-0x4A49CB (numWaitDelays: the
                {                                                         //  zero-extended byte @+0xE5)
                    this_->vfptr->stopMoving(this_);                      // 0x4A5CE5-0x4A5CE9 (slot +0x304)
                    ++this_->numberWaitDelays;                            // 0x4A5CEF (inc byte @+0xE5)
                    return 0;                                             // 0x4A5CF5
                }
                RGE_Player__incrementPathingAttempts((RGE_Player*)this_->owner);  // 0x4A49D1-0x4A49D4
                int v16 = passCounter;
                this_->numberWaitDelays = 0;                              // 0x4A49DD
                passCounter = v16 + 1;                                    // 0x4A49E4-0x4A49E5
            }
            topPath = BPathData__highestLinkedPathPointer(this_->pathData);   // 0x4A49E9-0x4A49EF
            if (!topPath
                && (worldNow = (RGE_Game_World*)(*base_game)->world,      // 0x4A49F8-0x4A49FE
                    topPathId = BPathData__highestLinkedPath(this_->pathData),  // 0x4A4A04-0x4A4A0A
                    (topPath = RGE_Game_World__path(worldNow, topPathId)) == 0)  // 0x4A4A0F-0x4A4A19
                || (topPath->flags.valueValue & 1) == 0)                  // 0x4A4A1B-0x4A4A26 (ja = nonzero skips;
            {                                                             //  BBitVectorAND_2 = 1; BPath flags @+0x20)
                if (BPathData__continuesLeft(this_->pathData, 1) <= 0)    // 0x4A4A28-0x4A4A37
                {
                    BPathData__setFailure(this_->pathData);               // 0x4A5CFA-0x4A5D00
                    this_->vfptr->stopMoving(this_);                      // 0x4A5D05-0x4A5D09 (slot +0x304)
                    return 1;                                             // 0x4A5D0F-0x4A5D14
                }
            }
            this_->vfptr->setWaitingToMove(this_, 0);                     // 0x4A4A3D-0x4A4A43 (slot +0x1B8 = 110)
            // asm 0x4A4A49-0x4A4A71: operator_new(0x34) = sizeof(BPathData) (walked);
            // the SEH scope (var_4 = 0 @0x4A4A59, -1 @0x4A4A77) is regenerated by the
            // guarded-ctor form (the addCurrentPath/setVelocity precedent). The CRT
            // wrapper rule: operator_new_internal. The .c's v20/v21 intermediates
            // collapse: successor = the ctor's return = the same pointer (asm 0x4A4A71
            // re-stores eax).
            successor = (BPathData*)operator_new_internal(sizeof(BPathData));   // 0x4A4A49-0x4A4A53
            if (successor != 0)                                           // 0x4A4A57
                successor = BPathData__BPathData(successor);              // 0x4A4A66-0x4A4A68 (returns this)
            if (successor == 0)
            {
                // RESTRUCTURE: LABEL_232 (asm 0x4A4A57 region) inlined at its
                // first site; the second site (the install-failure tail gate,
                // 0x4A584B-0x4A585F) carries its own identical copy below -
                // asm-distinct sites, byte-identical statements.
                BPathData__setFailure(this_->pathData);                   // 0x4A5D82-0x4A5D88
                this_->vfptr->stopMoving(this_);                          // 0x4A5D8D-0x4A5D91 (slot +0x304)
                return 1;                                                 // 0x4A5D97
            }
            g_dimuMark = 8;                                               // DIAG
            newPath = RGE_Game_World__createPath((RGE_Game_World*)(*base_game)->world);  // 0x4A4A88-0x4A4A94
            if (!newPath)
            {
                if (successor)
                {
                    BPathData__destructor(successor);                     // 0x4A5D19-0x4A5D23
                    operator_delete_internal(successor);                  // 0x4A5D28-0x4A5D29
                }
                BPathData__setFailure(this_->pathData);                   // 0x4A5D31-0x4A5D37
                // RESTRUCTURE: LABEL_229 (asm 0x4A5D3C) inlined at both its sites
                // (here and in the walk arm's linkedNow-null path below) -
                // region-distinct sites, identical statements.
                this_->vfptr->stopMoving(this_);                          // 0x4A5D3C-0x4A5D40 (slot +0x304)
                return 1;                                                 // 0x4A5D46-0x4A5D4B
            }
            pathFound = 0;                                                // 0x4A4AA9 (var_268)
            smoothUsed = 0;                                               // 0x4A4AAD (var_260)
            if (!RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 1))  // 0x4A4AA5-0x4A4AB9 (the compiler pushes
            {                                                             //  ebx = unitPos BEFORE the jnz - the arg is
                                                                          //  shared by both arms)
                BPath__addWaypointAtStart(newPath, unitPos);              // 0x4A4ABB-0x4A4ABD
                BVector* v24 = BPath__waypoint(resolvedPath, &noFlagWp, this_->pathData->waypoint);
                                                                          // 0x4A4AC2-0x4A4AD6 (var_15C)
                BPath__addWaypointAtEnd(newPath, v24);                    // 0x4A4ADB-0x4A4ADE
                BPath__calculatePathLength(newPath);                      // 0x4A4AE3-0x4A4AE5
                pathFound = 1;                                            // 0x4A4AEE
                                                                          // 0x4A4AF6 - RESTRUCTURE: the LABEL_129
                                                                          //  jump dissolved - the stub arm is the
                                                                          //  if-arm of the flagged-pipeline if/else,
                                                                          //  and both arms converge at the install
                                                                          //  gate below (every pipeline path provably
                                                                          //  reaches LABEL_129; see the stage map)
            }
                else
                {
                // ---- the flagged arm (asm loc_4A4AFB) - THE FLAGGED BUILDER
                // PIPELINE: every path here provably reaches the install gate
                // (LABEL_129) below - the pipeline if/else convergence is the
                // dissolution of the stub's goto LABEL_129 (0x4A4AF6) ----
                BVector* v26 = BPath__waypoint(resolvedPath, &prepWp, this_->pathData->waypoint);
                                                                          // 0x4A4B06-0x4A4B14 (var_F0)
                BVector__operator_minus(v26, &repathDelta, unitPos);      // 0x4A4B19-0x4A4B1B (this = the fetched
                                                                          //  buffer; v = ebx)
                repathDelta.z = 0.0f;                                     // 0x4A4B36
                repathDist = (float)sqrt((double)repathDelta.y * repathDelta.y + (double)repathDelta.x * repathDelta.x);
                                                                          // 0x4A4B20-0x4A4B46 (the x87 chain at 53-bit;
                                                                          //  ONE fstp m32 - rule (d)/(j))
                if (this_->pathData->waypointLevel == 2                   // 0x4A4B40 ([eax+0Ch] == 2)
                    && *dword_689568 == 1                                 // 0x4A4B50
                    && repathDist > *flt_68F494)                          // 0x4A4B5D-0x4A4B6C (fcomp ds:flt_68F494;
                {                                                         //  test ah,41h; jnz - the literal > MATCH row)
                    fprintf_logger(g_dimuLog, "SMOOTHGATE taken wl=%d v151=%08x thr=%08x\n",   // DIAG
                        this_->pathData->waypointLevel, (unsigned int)repathDist, (unsigned int)*flt_68F494);  // DIAG
                    mapAnalysis = RGE_Map_Analysis_System__get_map_analysis(  // 0x4A4B72-0x4A4B85
                        (*base_game)->world->map_analysis_system,         // world @0x420 -> +0x74
                        (RGE_Master_Static_Object*)this_->master_obj);
                    fprintf_logger(g_dimuLog, "MAPGET ma_null=%d mk=%d\n",           // DIAG
                        (mapAnalysis != 0) ? 1 : 0, g_dimuMark);                     // DIAG
                    if (mapAnalysis == 0)
                    {
                        BPath__addWaypointAtStart(newPath, &this_->positionValue);   // 0x4A4CE0-0x4A4CE3
                        BVector* v36 = BPath__waypoint(resolvedPath, &stubEndWp, this_->pathData->waypoint);
                                                                          // 0x4A4CE8-0x4A4CFC (var_9C)
                        BPath__addWaypointAtEnd(newPath, v36);            // 0x4A4D01-0x4A4D04
                        smoothUsed = 1;                                   // 0x4A4D0E
                        fprintf_logger(g_dimuLog, "STUBSET v149=%d\n", smoothUsed);  // DIAG
                        pathFound = 1;                                    // 0x4A4D12 via LABEL_50 (var_268 = v35 = 1)
                                                                          //  RESTRUCTURE: the LABEL_50 fall inlined
                                                                          //  (v35 = 1 here)
                    }
                    else
                    {
                        fp1TargetRadius = 0.0f;                           // 0x4A4B9C (var_224 - the unconditional zero)
                        int v29 = this_->pathData->targetID;              // 0x4A4BA4 (+0x24)
                        if (v29 != -1)
                        {
                            RGE_Static_Object* v30 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, v29);
                                                                          // 0x4A4BAC-0x4A4BB9
                            if (v30)
                                fp1TargetRadius = v30->master_obj->radius_x;   // 0x4A4BC2-0x4A4BC8 ([+0x14] -> [+0x3C])
                        }
                        v130 = this_->pathData->range;                    // 0x4A4BD9 ([eax+20h])
                        v127 = fp1TargetRadius + this_->master_obj->radius_x;  // 0x4A4BD5-0x4A4BE2 (fld var_224 +
                                                                          //  fadd [edx+3Ch]; fstp var_290 = the arg7
                                                                          //  slot; the 53-bit add, one store - rule (d))
                        dest_target_radius = BPath__waypoint(resolvedPath, &smoothDyWp, this_->pathData->waypoint)->y;
                                                                          // 0x4A4BE4-0x4A4BF8 (var_138; .y @+8)
                        BVector* v32 = BPath__waypoint(resolvedPath, &fp1DxWp, this_->pathData->waypoint);
                                                                          // 0x4A4BFD-0x4A4C15 (var_144)
                        fp1Result = RGE_Map_Analysis__find_path(          // 0x4A4C16-0x4A4C2C
                                    mapAnalysis, newPath,
                                    unitPos->x,                           // 0x4A4C22 ([ebx])
                                    this_->positionValue.y,               // 0x4A4C1C ([esi+0FCh] = the .y member)
                                    v32->x,                               // 0x4A4C1A ([eax])
                                    dest_target_radius,
                                    v127, v130, 0, 0, 0);
                                                                          // asm 0x4A4C31 (and eax,0FFh) - FINDING C1:
                                                                          // the u8 return reads AL; the assignment IS
                                                                          // the mask's result (the literal mask folds
                                                                          // as the AL narrowing itself)
                        fprintf_logger(g_dimuLog, "FP sx=%08x sy=%08x dx=%08x dy=%08x dtr=%08x dr=%08x r=%d\n",  // DIAG
                            (unsigned int)unitPos->x, (unsigned int)this_->positionValue.y,          // DIAG
                            (unsigned int)v32->x, (unsigned int)dest_target_radius,              // DIAG
                            (unsigned int)v127, (unsigned int)v130, fp1Result);                  // DIAG
                        fprintf_logger(g_dimuLog, "FP sx=%08x sy=%08x dx=%08x dy=%08x dtr=%08x dr=%08x r=%d\n",  // DIAG
                            (unsigned int)unitPos->x, (unsigned int)this_->positionValue.y,          // DIAG
                            (unsigned int)v32->x, (unsigned int)dest_target_radius,              // DIAG
                            (unsigned int)v127, (unsigned int)v130, fp1Result);                  // DIAG
                        if (fp1Result != 3 && fp1Result != 4)             // 0x4A4C36-0x4A4C42
                        {
                            if (fp1Result)
                            {
                                smoothUsed = 1;                           // 0x4A4C55-0x4A4C59
                                pathFound = 1;                            // 0x4A4C5D
                                smoothMidLevelPath(this_, newPath);       // 0x4A4C61 (__fastcall ecx=esi, edx=edi)
                                v137 = BPath__waypoint(newPath, &smoothLastWp, newPath->waypointsCount - 1);
                                                                          // 0x4A4C66-0x4A4C75 (var_3C; count @+0xC)
                                BVector* v34 = BPath__waypoint(resolvedPath, &fp1TailWp, this_->pathData->waypoint);
                                                                          // 0x4A4C7A-0x4A4C97 (var_12C)
                                BVector__operator_minus(v34, &smoothTailDelta, v137);   // 0x4A4C9C-0x4A4C9E
                                smoothTailDelta.z = 0.0f;                 // 0x4A4CBF
                                if (sqrt((double)smoothTailDelta.y * smoothTailDelta.y + (double)smoothTailDelta.x * smoothTailDelta.x) > (double)repathDist)
                                                                          // 0x4A4CA3-0x4A4CD7 (fcomp var_25C on the
                                                                          //  UNROUNDED st0; test ah,41h; jnz - the
                                                                          //  literal > MATCH row)
                                {
                                    newPath->waypointsCount = 0;          // 0x4A4CDB
                                    pathFound = 0;                        // 0x4A4CDE via LABEL_50 (var_268 = v35 = 0)
                                                                          //  RESTRUCTURE: the LABEL_50 jump inlined
                                                                          //  (the discard's v35 = 0)
                                }
                            }
                        }
                    }
                    // THE STUB-PATH FALL-THROUGH FIX (the capstone pass-1 desync root
                    // cause, 2026-11-20): these two statements were transcribed inside
                    // the ma!=NULL arm; the asm joins BOTH arms at loc_4A4D16 and calls
                    // unconditionally (0x4A4D12 the stub's v143=v35 falls through;
                    // 0x4A4D16 joins the find_path exits; lst :281179-281185; the .c
                    // renders them after the ma if/else, inside the smooth-gate block).
                    // Without it the stub path's pathLengthValue stayed 0.0 (the ctor
                    // zero), the $LN297 len gate failed, and pass-1 wrongly ran the
                    // findTile arm (v149=0, wpc=3 -> FOLLOW) instead of READY + the
                    // config reloop.
                    BPath__calculatePathLength(newPath);                  // 0x4A4D16-0x4A4D18 (loc_4A4D12 fall +
                                                                          //  loc_4A4D16 join - BOTH arms)
                    newPath->creationTimeValue = ((RGE_Player*)this_->owner)->world->world_time;
                                                                          // 0x4A4D1D-0x4A4D29 (FINDING D: owner->world
                                                                          //  @+0x8C -> world_time @+0x10)
                }
                else
                {
                    fprintf_logger(g_dimuLog, "SMOOTHGATE skipped wl=%d v151=%08x thr=%08x\n",  // DIAG
                        this_->pathData->waypointLevel, (unsigned int)repathDist, (unsigned int)*flt_68F494);  // DIAG
                }
                bestTailDist = FLT_MAX;                                   // 0x4A4D34 (var_244 = SINGLE_MAXIMUM_VALUE;
                                                                          //  FLT_MAX per the C2177 rule)
                fprintf_logger(g_dimuLog, "A2GATE v143=%d wpc=%d cfg=%d v151=%08x\n",    // DIAG
                    pathFound, newPath->waypointsCount, *dword_689564, (unsigned int)repathDist);  // DIAG
                fprintf_logger(g_dimuLog, "PREINST v149=%d v143=%d\n", smoothUsed, pathFound);     // DIAG
                // the arm-2 skip gate (0x4A4D2C-0x4A4D64) - RESTRUCTURE: the goto
                // LABEL_73 becomes the runArm2 stage flag (taken = arm 2 skipped,
                // the LABEL_73 body runs directly below).
                runArm2 = !(pathFound && newPath->waypointsCount > 2      // 0x4A4D2C-0x4A4D42 (cmp var_268,0 - jz skips
                    || *dword_689564 != 1                                 //  the count test; cmp [edi+0Ch],2; jg)
                    || repathDist > 10.0f);                               // 0x4A4D55-0x4A4D64 (flt_6565F0; test ah,41h;
                                                                          //  jz - the literal > MATCH row)
                toLabel103 = false;                                   // per-iteration flag init (the findTile
                arm2GateFailed = false;                               //  len-fail + the LABEL_100 gate set it)
                atLabel99Fall = false;                                // per-iteration flag init (the wpc/len gate)
                if (runArm2)
                {
                    // ---- arm 2: findLowLevelPath (asm 0x4A4D6A-0x4A4F1D) ----
                    g_dimuMark = 6;                                       // DIAG
                    fprintf_logger(g_dimuLog, "ARM2 entered v149=%d\n", smoothUsed);  // DIAG
                    int v37 = 0;
                    newPath->waypointsCount = 0;                          // 0x4A4D6C ([edi+0Ch] = ebp = 0)
                    smoothUsed = 0;                                       // 0x4A4D71
                    ignoreListSize = 0;                                   // 0x4A4D75 (the owned global)
                    if (this_)                                            // 0x4A4D6F-0x4A4D7B (the dead self-test;
                    {                                                     //  cmp esi,ebp - kept faithful)
                        ignoreUnits[0] = this_->id;                       // 0x4A4D7D-0x4A4D80 (id @+0x10)
                        v37 = 1;
                        ignoreListSize = 1;
                        if (this_->type == 80)                            // 0x4A4D91-0x4A4D96 (byte @+0x5E == 0x50)
                        {
                            // BASE-POINTER FIX: the .c's &this[1].ei.y2 = +0x13C (sizeof
                            // RGE_Moving_Object) + offsetof(ei.y2) (= +0xA4: ei @+0x90,
                            // y2 @+0x14) = +0x1E0 = TRIBE_Building_Object::linked_child
                            // (walker-confirmed; asm 0x4A4D9A lea edx,[esi+1E0h]). The
                            // type-80 guard is the runtime-type evidence.
                            TRIBE_Building_Object* buildingObj = (TRIBE_Building_Object*)this_;
                            bldgChildA = 0;
                            do
                            {
                                RGE_Static_Object* child =
                                    (RGE_Static_Object*)buildingObj->linked_child[bldgChildA];
                                if (child)                                // 0x4A4DA0-0x4A4DA4
                                {
                                    ignoreUnits[v37++] = child->id;       // 0x4A4DA6-0x4A4DA9 (id @+0x10)
                                    ignoreListSize = v37;
                                    if (v37 >= _countof(ignoreUnits))     // 0x4A4DB1-0x4A4DB7 (cmp 80h; jge - the
                                        break;                            //  cap AFTER the store + count update)
                                }
                                ++bldgChildA;
                            } while (bldgChildA < _countof(buildingObj->linked_child));   // 0x4A4DC3-0x4A4DC6 (cmp ebx,4)
                        }
                    }
                    TRIBE_Building_Object* v40 = (TRIBE_Building_Object*)targetObj;   // 0x4A4DC8-0x4A4DCC
                    if (targetObj)
                    {
                        if (v37 < _countof(ignoreUnits))                  // 0x4A4DD0-0x4A4DD6 (the gate BEFORE the append)
                        {
                            ignoreUnits[v37++] = targetObj->id;           // 0x4A4DD8-0x4A4DDB
                            ignoreListSize = v37;
                            if (v40->type == 80)                          // 0x4A4DE9-0x4A4DEF
                            {
                                tgtChildA = 0;
                                do
                                {
                                    RGE_Static_Object* child =
                                        (RGE_Static_Object*)v40->linked_child[tgtChildA];   // 0x4A4DF3 (lea edx,[eax+1E0h])
                                    if (child)                            // 0x4A4DF9-0x4A4DFD
                                    {
                                        ignoreUnits[v37++] = child->id;   // 0x4A4DFF-0x4A4E02
                                        ignoreListSize = v37;
                                        if (v37 >= _countof(ignoreUnits))
                                            break;
                                    }
                                    ++tgtChildA;
                                } while (tgtChildA < _countof(v40->linked_child));
                            }
                        }
                    }
                    ownerNow = (RGE_Player*)this_->owner;                 // 0x4A4E21 (the dual-naming cast)
                    RGE_Master_Moving_Object* v44 = this_->master_obj;    // 0x4A4E24
                    BPathData* v45 = this_->pathData;                     // 0x4A4E27
                    v138 = ownerNow->id;                                  // 0x4A4E2D ([edx+0A0h])
                    v132 = ownerNow->world->terrains[v44->terrain];       // 0x4A4E33-0x4A4E43 (world @+0x8C ->
                                                                          //  terrains @+0x60; the asm movsx reads the
                                                                          //  i16 terrain @+0x76)
                    v122 = v37;
                    v120 = v45->targetID;                                 // 0x4A4E56 ([eax+24h])
                    v118 = v45->range;                                    // 0x4A4E4D ([eax+20h])
                    v117 = v44->radius_x;                                 // 0x4A4E5F ([ebp+3Ch])
                    BVector* v46 = BPath__waypoint(activePath, &lowStartWp, v45->waypoint);
                                                                          // 0x4A4E63-0x4A4E74 (var_114)
                    unitPos = &this_->positionValue;                      // 0x4A4E7D (the ebx reload)
                    lowResult = findLowLevelPath(                         // 0x4A4E7C-0x4A4E86 (__fastcall; ecx = the
                            obSystemToUse,                                //  OB system, edx = start)
                            unitPos,
                            v46,
                            v117, v118, v120,
                            ignoreUnits, v122,
                            collisionUnits,                               // 0x4A4E50 (push offset collisionUnits - arg9;
                                                                          //  THE LEAD RE-VERIFIED: the .lst's param
                                                                          //  label agrees)
                            collisionCount,                               // 0x4A4E47 (arg10 = the accumulated count;
                                                                          //  the .c's v128 = (BPath*)v157 round-trip)
                            newPath, v132,
                            0,                                            // 0x4A4E3A (useHulledArea)
                            v138);
                    BPath__calculatePathLength(newPath);                  // 0x4A4E8B-0x4A4E8D
                    newPath->creationTimeValue = ((RGE_Player*)this_->owner)->world->world_time;
                                                                          // 0x4A4E94-0x4A4EA3 (FINDING D; stored
                                                                          //  UNCONDITIONALLY before the checks)
                    if (lowResult == 3 || lowResult == 4                  // 0x4A4E97-0x4A4EAB
                        || X87_LE_NAN(newPath->pathLengthValue, 1.0e-7f)) // 0x4A4EAD-0x4A4EBB (fld [edi+18h]; fcomp
                        arm2GateFailed = true;                            //  flt_6565F4; test ah,41h; jnz - the
                    else                                                  //  {<=, NaN} set fails; literal <= would
                    {                                                     //  skip NaN: rule (i) DIVERGENT)
                        // the arm-2 success (asm 0x4A4EBD-0x4A4F21): the flag pair and the
                        // shared tail norm; the tail jmp 0x4A4F21 lands at the gate below.
                        pathFound = 1;                                    // 0x4A4EC3
                        v139 = BPath__waypoint(newPath, &arm2LastWp, newPath->waypointsCount - 1);
                                                                          // 0x4A4EBD-0x4A4ED4 (var_54)
                        BVector* v48 = BPath__waypoint(activePath, &arm2CurWp, this_->pathData->waypoint);
                                                                          // 0x4A4ED9-0x4A4EF5 (var_FC)
                        BVector__operator_minus(v48, &arm2TailDelta, v139);   // 0x4A4EFA-0x4A4EFC
                        arm2TailDelta.z = 0.0f;                           // 0x4A4F11
                        bestTailDist = (float)sqrt((double)arm2TailDelta.y * arm2TailDelta.y + (double)arm2TailDelta.x * arm2TailDelta.x);
                                                                          // 0x4A4F01-0x4A4F1D (fsqrt; fstp var_244)
                    }
                }
                toLabel97 = false;
                if (!runArm2 || arm2GateFailed)
                {
                    // ---- the LABEL_73 body (asm loc_4A4F26-0x4A5117; entered both
                    // from the skip gate above - arm 2 skipped - and from the arm-2
                    // failure gate) ----
                    if (!pathFound)                                       // 0x4A4F26-0x4A4F2C (test var_268; jnz
                    {                                                     //  -> the gate below; two test ah,1 gates -
                        if (repathDist >= 1.0 && goal >= 1.0)             //  the literal >= MATCH rows; the operand is
                                                                          //  `goal` (var_264, the precalcDistToTarget
                                                                          //  store) - TRANSCRIPTION FIX (the second
                                                                          //  paired-capture divergence): NOT goalDist
                                                                          //  (v161); the slip skipped the gate, ran
                                                                          //  the OB probe the original never ran, and
                                                                          //  forked the repath flow (unit 13871)
                            toLabel97 = true;                             // 0x4A4F58 -> LABEL_97
                        else
                        {
                            smoothUsed = 0;                               // 0x4A4F58-0x4A4F65
                            BPath__waypoint(activePath, &boundsWp, this_->pathData->waypoint);   // 0x4A4F6D-0x4A4F76 (var_1CC)
                            RGE_Master_Moving_Object* v49 = this_->master_obj;   // 0x4A4F7B
                            if (X87_LT_NAN(boundsWp.x, v49->radius_x)     // 0x4A4F7E-0x4A4F8D (fld var; fcomp [ecx+3Ch];
                                || X87_LT_NAN(boundsWp.y, v49->radius_y)) //  test ah,1; jnz - the {<, NaN} sets ->
                                toLabel97 = true;                         //  DIVERGENT; radius_y @+0x40) -> LABEL_97
                            else
                            {
                                RGE_Map* v50 = ((RGE_Player*)this_->owner)->world->map;   // 0x4A4FA8-0x4A4FB4 (world @+0x8C -> map @+0x34)
                                if (X87_LT_NAN((double)v50->map_width - (double)v49->radius_x, boundsWp.x)
                                                                          // 0x4A4FB4-0x4A4FC6 (fild [edx+8]; fsub
                                                                          //  [ecx+3Ch]; fcomp var - the 53-bit
                                                                          //  arithmetic feeds fcomp (rule (a)); test
                                                                          //  ah,1; jnz - the {<, NaN} set -> DIVERGENT)
                                    || X87_LT_NAN((double)v50->map_height - (double)v49->radius_y, boundsWp.y))
                                                                          // 0x4A4FCC-0x4A4FDE (fild [edx+0Ch] -
                                                                          //  map_height @+0xC)
                                    toLabel97 = true;                     // 0x4A5135 -> LABEL_97
                                else
                                {
                                    g_dimuMark = 7;                       // DIAG
                                    int v51 = 0;
                                    ignoreListSize = 0;                   // 0x4A4FE4-0x4A4FE8
                                    if (this_)                            // 0x4A4FE6-0x4A4FED (the dead self-test)
                                    {
                                        v51 = 1;
                                        ignoreUnits[0] = this_->id;       // 0x4A4FEF-0x4A4FF7
                                        ignoreListSize = 1;
                                        if (this_->type == 80)            // 0x4A5002-0x4A5006
                                        {
                                            TRIBE_Building_Object* buildingObj2 = (TRIBE_Building_Object*)this_;
                                            bldgChildB = 0;
                                            do
                                            {
                                                RGE_Static_Object* child2 =
                                                    (RGE_Static_Object*)buildingObj2->linked_child[bldgChildB];
                                                if (child2)               // 0x4A5010-0x4A5014
                                                {
                                                    ignoreUnits[v51++] = child2->id;   // 0x4A5016-0x4A5019
                                                    ignoreListSize = v51;
                                                    if (v51 >= _countof(ignoreUnits))  // 0x4A5021-0x4A502B
                                                        break;
                                                }
                                                ++bldgChildB;
                                            } while (bldgChildB < _countof(buildingObj2->linked_child));
                                        }
                                    }
                                    RGE_Static_Object* v54 = targetObj;   // 0x4A5036-0x4A503C
                                    if (targetObj)
                                    {
                                        if (v51 < _countof(ignoreUnits))  // 0x4A503E-0x4A5043
                                        {
                                            ignoreUnits[v51++] = targetObj->id;   // 0x4A5045-0x4A5048
                                            ignoreListSize = v51;
                                            if (v54->type == 80)          // 0x4A5055-0x4A505B
                                            {
                                                // BASE-POINTER FIX: the .c's &v54[2].damaged_lately_
                                                // timer = +0x168 (2*sizeof RGE_Static_Object) + 0x78
                                                // = +0x1E0 = linked_child (asm 0x4A505F lea edx,[ecx+1E0h]).
                                                TRIBE_Building_Object* targetBuilding = (TRIBE_Building_Object*)v54;
                                                tgtChildB = 0;
                                                do
                                                {
                                                    RGE_Static_Object* child3 =
                                                        (RGE_Static_Object*)targetBuilding->linked_child[tgtChildB];
                                                    if (child3)           // 0x4A5065-0x4A5069
                                                    {
                                                        ignoreUnits[v51++] = child3->id;   // 0x4A506B-0x4A506E
                                                        ignoreListSize = v51;
                                                        if (v51 >= _countof(ignoreUnits))
                                                            break;
                                                    }
                                                    ++tgtChildB;
                                                } while (tgtChildB < _countof(targetBuilding->linked_child));
                                            }
                                        }
                                    }
                                    v133 = v51;
                                    BVector* v57 = BPath__waypoint(activePath, &obProbeWp, this_->pathData->waypoint);
                                                                          // 0x4A5099-0x4A50AE (var_84)
                                    unitPos = &this_->positionValue;      // 0x4A50B7
                                    findObSystemCollisions(obSystemToUse, // 0x4A50B3-0x4A50C1 (__fastcall; p1 =
                                            this_,                        //  positionValue, p2 = the fetched
                                            unitPos, v57,                 //  waypoint; checkTerrain = 1;
                                            ignoreUnits, v133, 1, 1.0f);  //  radiusModifier = 1.0f)
                                    if (*numberObSystemCollisions)        // 0x4A50C6-0x4A50CD (mov eax,[global]; test -
                                        toLabel97 = true;                 //  the VALUE test: the deref sweep) -> LABEL_97
                                    else
                                    {
                                        newPath->waypointsCount = 0;      // 0x4A50D2
                                        BPath__addWaypointAtStart(newPath, unitPos);   // 0x4A50CF-0x4A50D9
                                        BVector* v58 = BPath__waypoint(activePath, &rebuildEndWp, this_->pathData->waypoint);
                                                                          // 0x4A50DE-0x4A50F4 (var_E4)
                                        BPath__addWaypointAtEnd(newPath, v58);   // 0x4A50F9-0x4A50FC
                                        newPath->creationTimeValue = ((RGE_Player*)this_->owner)->world->world_time;
                                                                          // 0x4A5101-0x4A510F (FINDING D)
                                        BPath__calculatePathLength(newPath);   // 0x4A510C-0x4A5112
                                        pathFound = 1;                    // 0x4A5117
                                    }
                                }
                            }
                        }
                    }
                }
                if (!toLabel97)
                {
                    if (newPath->waypointsCount >= 2                      // 0x4A511F-0x4A5123 (cmp [edi+0Ch],2; jl)
                        && newPath->pathLengthValue >= 0.01f)             // 0x4A5125-0x4A5133 (fld [edi+18h]; fcomp
                    {                                                     //  flt_654978 = 0.01f; test ah,1; jz - the
                        // LABEL_99 (0x4A5164): v25 = var_26C (activePath alias - the
                        // assignment is identity; dropped) - then jumps past LABEL_97
                        // straight to LABEL_100's gate below.
                        atLabel99Fall = true;
                    }                                                     //  literal >= MATCH row)
                }
                if (!atLabel99Fall)
                {
                        // ---- LABEL_97 (asm loc_4A5135-0x4A5351) ----
                        targetDist = RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_, targetObj);
                                                                          // 0x4A5135-0x4A513C (double return -
                                                                          //  consumed UNROUNDED in st0 at loc_4A5336)
                        bool toLabel99;
                        if (targetObj)
                            toLabel99 = X87_LT_NAN(targetDist, 1.0)       // 0x4A5336-0x4A5341 (fcomp const_f_one on the
                                && targetObj->master_obj->id != 45;       //  UNROUNDED st0; test ah,1; jz skip - the
                                                                          //  {<, NaN} set falls to the id test ->
                        else                                              //  DIVERGENT; the master id word @+0x18 ==
                            toLabel99 = X87_LT_NAN(repathDist, 1.0f);     //  45 = Cargo Trader; 0x4A514D-0x4A515E
                                                                          //  (fstp st discards v59; fld var_25C; test
                                                                          //  ah,1; jz skip - DIVERGENT)
                        if (!toLabel99 && *dword_68956C == 1)             // 0x4A5351-0x4A5358 (the LABEL_99 fall when
                        {                                                 //  the gate passes; the findTile arm when
                                                                          //  cfg56C is on)
                            // ---- the findTilePath arm (asm 0x4A535E-0x4A54AC) ----
                            newPath->waypointsCount = 0;                  // 0x4A5365 (v25 = v142 at 0x4A5360 - the
                                                                          //  activePath alias, dropped)
                            BPathData* v68 = this_->pathData;
                            smoothUsed = 0;                               // 0x4A5376
                            dest_target_radiusb = v68->targetID;          // 0x4A537B
                            v123 = v68->range;                            // 0x4A5373
                            v121 = (int)BPath__waypoint(activePath, &tileGoalYWp, v68->waypoint)->y;
                                                                          // 0x4A5384-0x4A539C (var_24; .y @+8;
                                                                          //  __ftol @0x4A5397 -> the (int) form)
                            v119 = (int)BPath__waypoint(activePath, &tileGoalXWp, this_->pathData->waypoint)->x;
                                                                          // 0x4A53A3-0x4A53C3 (var_CC; .x @+0; __ftol
                                                                          //  @0x4A53B8)
                            pathFound = PathingSystem__findTilePath(      // 0x4A53C4-0x4A53D7 (the direct pointer per
                                        pathSystem,                       //  common.h:120; the four tile coords are the
                                        (int)unitPos->x,                  //  __ftol conversions: 0x4A53CC ([ebx]) and
                                        (int)this_->positionValue.y,      //  0x4A53C4 ([esi+0FCh]))
                                        v119, v121,
                                        this_, v123, dest_target_radiusb,
                                        0, -1, -1,                        // rLength = 0; the two -1 unobstructible ids
                                        newPath, 0);
                                                                          // asm 0x4A53DF (and eax,0FFh) - FINDING C2:
                                                                          // the bool return's AL IS the mask's result;
                                                                          // the direct bool read is the same narrowing
                                                                          // (the literal & was rejected: a C4806 no-op)
                            newPath->creationTimeValue = ((RGE_Player*)this_->owner)->world->world_time;
                                                                          // 0x4A53DC-0x4A53F3 (FINDING D)
                            BPath__calculatePathLength(newPath);          // 0x4A53F6
                            if (X87_LT_NAN(newPath->pathLengthValue, 1.0e-7f))   // 0x4A53FB-0x4A5409 (test ah,1; jz keeps - the
                            {                                             //  {<, NaN} set fails -> DIVERGENT)
                                pathFound = 0;                            // 0x4A540B
                                toLabel103 = true;                        // 0x4A518A-region - RESTRUCTURE: the
                                                                          //  LABEL_103 entry (the body runs below,
                                                                          //  after the LABEL_100 gate stage)
                            }
                            else
                            {
                                // ---- the findTile tail-quality check (asm 0x4A5418-0x4A54AC) ----
                                v141 = BPath__waypoint(newPath, &tileLastWp, newPath->waypointsCount - 1);
                                                                          // 0x4A5418-0x4A5427 (var_6C)
                                BVector* v69 = BPath__waypoint(activePath, &findTileCurWp, this_->pathData->waypoint);
                                                                          // 0x4A542D-0x4A5446
                                BVector__operator_minus(v69, &tileTailDelta, v141);   // 0x4A544B-0x4A544D
                                tileTailDelta.z = 0.0f;                   // 0x4A5462
                                tileTailDist = sqrt((double)tileTailDelta.y * tileTailDelta.y + (double)tileTailDelta.x * tileTailDelta.x);
                                                                          // 0x4A5452-0x4A546C (fsqrt; st0 KEPT unrounded)
                                normCopy = (float)tileTailDist;           // 0x4A546E (fst var_248 - the float copy;
                                                                          //  DEAD: never read - the discard path
                                                                          //  re-stores v159 from st0 @0x4A5499)
                                if (X87_LT_NAN(                           // 0x4A5472-0x4A5494 (the 3-comp chain y^2+
                                        sqrt((double)repathDelta.y * repathDelta.y   //  z^2 + x^2 vs the UNROUNDED st(1);
                                                + (double)repathDelta.z * repathDelta.z   //  fcomp st(1); test ah,1; jz keeps - the
                                                + (double)repathDelta.x * repathDelta.x),   //  {<, NaN} set discards -> DIVERGENT;
                                        tileTailDist))                    //  repathDelta.z was zeroed at 0x4A4B36)
                                {
                                    bestTailDist = (float)tileTailDist;   // 0x4A5499 (fstp var_244)
                                    newPath->waypointsCount = 0;          // 0x4A549F
                                    newPath->pathLengthValue = 0.0f;      // 0x4A54A2
                                }                                         // 0x4A54A5/0x4A54AA - RESTRUCTURE: both
                                                                          //  ends goto LABEL_100 = the common fall
                                                                          //  into the gate below (the discard's
                                                                          //  goto inlined at its site)
                            }
                        }
                        // the cfg56C-off fall + the LABEL_99-pass fall + the findTile
                        // tail-quality fall ALL converge at LABEL_100's gate (0x4A5168).
                    }
                if (!toLabel103)
                {
                    // ---- LABEL_100's gate (asm loc_4A5168-0x4A5184) ----
                    if (!pathFound || newPath->waypointsCount < 2         // 0x4A5168-0x4A5184 (test var_268; cmp
                        || newPath->pathLengthValue < 0.01f)              //  [edi+0Ch],2; fcomp flt_654978; test ah,1;
                        toLabel103 = true;                                //  the literal forms - the MATCH rows)
                }
                if (toLabel103)
                {
                    // ---- LABEL_103 (asm loc_4A518A-0x4A557B) ----
                    if (this_->pathData->waypointLevel == 2               // 0x4A518A-0x4A5194
                        && *dword_689568 == 1                             // 0x4A519A-0x4A51A1
                        && X87_LT_NAN(repathDist, *flt_68F494))           // 0x4A51A7-0x4A51B6 (test ah,1; jz -> the
                    {                                                     //  LABEL_129 gate; the {<, NaN} set continues
                                                                          //  into the pass -> DIVERGENT)
                        goala = RGE_Map_Analysis_System__get_map_analysis(    // 0x4A51BC-0x4A51CF
                            (*base_game)->world->map_analysis_system,
                            (RGE_Master_Static_Object*)this_->master_obj);
                        BPath__BPath(ReturnPath, -1);                     // 0x4A51D4-0x4A51E1 (the stack BPath; the SEH
                                                                          //  cookie var_4 = 1 @0x4A51EA regenerates
                                                                          //  with the ctor call - the raw-storage form;
                                                                          //  UNCONDITIONAL in the wl-gate-taken path)
                        if (goala)
                        {
                            fp2TargetRadius = 0.0f;                       // 0x4A5203 (var_228 - the unconditional zero;
                                                                          //  precedes the id gate)
                            int v61 = this_->pathData->targetID;          // 0x4A5207-0x4A520D
                            if (v61 != -1)
                            {
                                RGE_Static_Object* v62 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, v61);
                                                                          // 0x4A5213-0x4A5220
                                if (v62)
                                    fp2TargetRadius = v62->master_obj->radius_x;   // 0x4A5229-0x4A522F
                            }
                            v131 = this_->pathData->range;                // 0x4A5240
                            v129 = fp2TargetRadius + this_->master_obj->radius_x;   // 0x4A523C-0x4A5258 (fld var_228 + fadd
                                                                          //  [edx+3Ch]; fstp var_290 = arg7; rule (d))
                            dest_target_radiusa = BPath__waypoint(activePath, &fp2DyWp, this_->pathData->waypoint)->y;
                                                                          // 0x4A5251-0x4A5264 (var_150; .y @+8)
                            BVector* v64 = BPath__waypoint(activePath, &fp2DxWp, this_->pathData->waypoint);
                                                                          // 0x4A526E-0x4A527C (var_138)
                            fp2Result = RGE_Map_Analysis__find_path(      // 0x4A5281-0x4A529A
                                        goala, ReturnPath,
                                        unitPos->x,                       // 0x4A528D ([ebx])
                                        this_->positionValue.y,           // 0x4A5283
                                        v64->x,
                                        dest_target_radiusa,
                                        v129, v131, 0, 0, 0);
                                                                          // asm 0x4A52A6 (and eax,0FFh) - FINDING C1 #2
                            fprintf_logger(g_dimuLog, "FP2 sx=%08x sy=%08x dx=%08x dy=%08x dtr=%08x dr=%08x r=%d\n",  // DIAG
                        (unsigned int)unitPos->x, (unsigned int)this_->positionValue.y,          // DIAG
                        (unsigned int)v64->x, (unsigned int)dest_target_radiusa,             // DIAG
                        (unsigned int)v129, (unsigned int)v131, fp2Result);                  // DIAG
                            v140 = BPath__waypoint(ReturnPath, &fp2TailWp, ReturnPath->waypointsCount - 1);
                                                                          // 0x4A529F-0x4A52BE (var_120; count @+0xC)
                            BVector* v66 = BPath__waypoint(activePath, &maIntendedWp, this_->pathData->waypoint);
                                                                          // 0x4A52C3-0x4A52E0 (var_1A8)
                            BVector__operator_minus(v66, &maTailDelta, v140);   // 0x4A52E5-0x4A52E7
                            maTailDelta.z = 0.0f;                         // 0x4A5308
                            tailNorm = sqrt((double)maTailDelta.y * maTailDelta.y + (double)maTailDelta.x * maTailDelta.x);
                                                                          // 0x4A52EC-0x4A5315 (fsqrt; st0 KEPT unrounded)
                            goald = (float)tailNorm;                      // 0x4A5317 (fst goal - the store-round COPY;
                                                                          //  st0 stays unrounded for the fcomp below)
                            if (X87_LE_NAN(tailNorm, (double)repathDist)) // 0x4A5317-0x4A5324 (fcomp var_25C on the
                            {                                             //  UNROUNDED st0; test ah,41h; jnz = the
                                                                          //  {<=, NaN} KEEP set - the literal <= would
                                                                          //  send NaN to the discard: DIVERGENT)
                                if (fp2Result != 3 && fp2Result != 4      // 0x4A54B1-0x4A54C5 (cmp ebx,3/4; test
                                    && fp2Result != 0)                    //  ebx,ebx - all three -> the tail)
                                {
                                    if (bestTailDist > (double)goald)     // 0x4A54C9-0x4A54DC (fld goal; fld var_244;
                                                                          //  fcomp st(1); test ah,41h; fstp st; jnz -
                                                                          //  the literal > MATCH row; both float m32)
                                    {
                                        v71 = 0;
                                        v72 = ReturnPath->waypointsCount <= 0;   // 0x4A54E2-0x4A54ED
                                        newPath->waypointsCount = 0;
                                        if (!v72)
                                        {
                                            do
                                            {
                                                BVector* v73 = BPath__waypoint(ReturnPath, &maCopyWp, v71);
                                                                          // 0x4A54F6-0x4A5506 (var_19C)
                                                BPath__addWaypointAtEnd(newPath, v73);   // 0x4A550B-0x4A550E
                                                ++v71;
                                            } while (v71 < ReturnPath->waypointsCount);
                                                                          // 0x4A5513-0x4A551D (the count RE-READ per
                                                                          //  iteration - the live reload, faithful)
                                        }
                                        smoothUsed = 1;                   // 0x4A5528
                                        pathFound = 1;                    // 0x4A552C
                                        smoothMidLevelPath(this_, newPath);   // 0x4A5530
                                        BPath__calculatePathLength(newPath);   // 0x4A5537
                                        v74 = newPath->pathLengthValue;   // 0x4A553F (fld BEFORE the stamp store -
                                                                          //  the compiler's schedule; semantically
                                                                          //  identical)
                                        newPath->creationTimeValue = ((RGE_Player*)this_->owner)->world->world_time;
                                                                          // 0x4A553C-0x4A554B (FINDING D)
                                        if (X87_LT_NAN(v74, *flt_68F494)) // 0x4A554E-0x4A5559 (test ah,1; jz tail; the
                                        {                                 //  {<, NaN} set zeroes -> DIVERGENT)
                                            newPath->waypointsCount = 0;  // 0x4A555D
                                            pathFound = 0;                // 0x4A5560
                                        }
                                    }
                                }
                            }
                            else
                            {
                                newPath->waypointsCount = 0;              // 0x4A532A (the ordered tail > v151 discard)
                                                                          //  0x4A5331 - RESTRUCTURE: the discard's
                                                                          //  goto the_returnpath_tail = the common
                                                                          //  fall into the destructor below
                            }
                        }
                        // the_returnpath_tail (0x4A5331 target): the destructor runs on
                        // EVERY wl-gate-taken path (goala-taken or not, found or discard) -
                        // the label sits between the if(goala) close and the wl-gate close.
                        BPath__destructor(ReturnPath);                    // 0x4A5564-0x4A5576 (the cookie var_4 = -1
                                                                          //  @0x4A556B regenerates)
                    }
                    // the wl-gate-fail fall + the LABEL_103 body's common exit: straight
                    // to the install gate (0x4A5564 -> 0x4A557B the fall). RESTRUCTURE:
                    // the LABEL_129 goto dissolved into the fall.
                    }
                } // the flagged-pipeline else closes - every pipeline path reached
                  // the install gate below (the stage-map proof); the stub arm
                  // above joins it here too (the goto LABEL_129 dissolution)
                // ---- LABEL_129: the install gate (asm loc_4A557B-0x4A569D) ----
                if (newPath->waypointsCount <= 1                      // 0x4A557B-0x4A5585 (cmp eax,1; jle)
                    || X87_LT_NAN(newPath->pathLengthValue, 1.0e-7f)) // 0x4A558B-0x4A5598 (test cl,ah with cl = 1 =
                {                                                     //  test ah,1; jnz - the {<, NaN} set fails;
                    fprintf_logger(g_dimuLog, "INSTALL-FAIL wpc=%d len=%08x\n",    // DIAG
                        newPath->waypointsCount, (unsigned int)newPath->pathLengthValue);   // DIAG
                    // ---- the install-FAILURE arm (asm loc_4A572E-0x4A5866) ---- //  literal < would skip NaN: DIVERGENT)
                    g_dimuMark = 10;                                  // DIAG
                    BPathData__destructor(successor);                 // 0x4A572E-0x4A5734
                    operator_delete_internal(successor);              // 0x4A5739-0x4A573A
                    RGE_Game_World__destroyPath2((RGE_Game_World*)(*base_game)->world, newPath);
                                                                      // 0x4A573F-0x4A574E
                    v134 = BPath__waypoint(activePath, &failWpDy, this_->pathData->waypoint - 1)->y;
                                                                      // 0x4A5753-0x4A5775 (var_D8; .y @+8)
                    BVector* v78 = BPath__waypoint(activePath, &failReloadWp, this_->pathData->waypoint - 1);
                                                                      // 0x4A5779-0x4A5788 (var_C0; the waypoint
                                                                      //  RELOADED - the .c's second waypoint fetch)
                    RGE_Static_Object__distance_to_position((RGE_Static_Object*)this_,
                            v78->x, v134, 0.0);                       // 0x4A578D-0x4A5792 (the result DISCARDED:
                                                                      //  asm 0x4A579D fstp st)
                    if (targetObj)
                    {
                        RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_, targetObj);
                                                                      // 0x4A57A1-0x4A57A4 (the result DISCARDED:
                                                                      //  asm 0x4A57B4 fstp st)
                        BPathData* v79 = this_->pathData;
                        if ((v79->mFlags.valueValue & 4) != 0)        // 0x4A57AF-0x4A57B9 (test [ecx+30h], eax;
                            BPathData__setFlag(v79, 2, 0);            //  dword_65BD9C = 4, .rdata:0065BD9C dd 4)
                    }
                    if (pathFound && (v80 = this_->pathData, X87_LE_NAN(goalDist, (double)v80->range)))
                                                                      // 0x4A57C4-0x4A57DD (fld var_23C; fcomp
                                                                      //  [ecx+20h]; test ah,41h; jz - the fall-
                                                                      //  through set is {<=, NaN}: DIVERGENT)
                    {
                        if ((activePath->flags.valueValue & 1) == 0)  // 0x4A57DF-0x4A57EA (test edx,eax; jbe;
                                                                      //  BBitVectorAND_2 = 1)
                        {
                            // RESTRUCTURE: LABEL_239 (0x4A5E27 shared tail) inlined -
                            // the second copy sits at the range-gate tail below
                            // (region-distinct sites, identical statements).
                            this_->vfptr->stopMoving(this_);          // 0x4A5E27-0x4A5E2B (slot +0x304)
                            return 2;                                 // 0x4A5E31
                        }
                        // LABEL_151 (0x4A57F0): the setFlag + the ASM RELOOP - inlined
                        // at both its sites (the second: the waypoint gate below).
                        BPathData__setFlag(this_->pathData, 0, 1);    // 0x4A57F0-0x4A57F4 (the v161-arm site)
                        continue;                                     // 0x4A57F9 - ASM RELOOP (join J4: the .c's
                                                                      //  fall-to-the-walk-arm is superseded)
                    }
                    else
                    {
                        v80 = this_->pathData;                        // 0x4A57FE (the else-arm's own read)
                        if (v80->waypoint < activePath->waypointsCount - 1)   // 0x4A57FE-0x4A580B ([ebp+0Ch] dec;
                                                                      //  cmp [ecx+18h])
                        {
                            // LABEL_151's second site (0x4A580D-0x4A5816 - the setFlag
                            // + the ASM RELOOP, join J5)
                            BPathData__setFlag(v80, 0, 1);
                            continue;                                 // 0x4A580D-0x4A5816 - ASM RELOOP (join J5)
                        }
                        if (BPathData__continuesLeft(this_->pathData, 0) > 0)   // 0x4A581B-0x4A5824 (push 0; test; jg)
                        {
                            this_->vfptr->pauseMovement(this_, 0.25f);   // 0x4A5D50-0x4A5D59 (slot +0x310 = 196;
                                                                      //  push 0.25 = the .c's 1048576000 bits)
                            this_->vfptr->stopMoving(this_);          // 0x4A5D5F-0x4A5D63 (slot +0x304)
                            return 0;                                 // 0x4A5D69
                        }
                        if ((activePath->flags.valueValue & 1) != 0)  // 0x4A582A-0x4A5835 (test ecx,eax; ja;
                                                                      //  BBitVectorAND_2 = 1)
                        {
                            this_->vfptr->pauseMovement(this_, 0.5f); // 0x4A5D6E-0x4A5D77 (push 0.5 = the .c's
                            return 0;                                 //  1056964608 bits)
                        }
                        BPathData* v81 = this_->pathData;
                        if (v81->waypointLevel >= 2                   // 0x4A583B-0x4A5845 (cmp [ecx+0Ch],2; jge)
                            || BPathData__highestLinkedPathHasMoreWaypoints(v81) != 1)
                                                                      // 0x4A584B-0x4A5853 (cmp eax,1; jnz)
                        {
                            // RESTRUCTURE: LABEL_232's second site (0x4A584B region) -
                            // identical statements to the allocation-failure site above
                            // (asm-distinct sites, byte-identical statements).
                            BPathData__setFailure(this_->pathData);   // 0x4A5D82-0x4A5D88
                            this_->vfptr->stopMoving(this_);          // 0x4A5D8D-0x4A5D91 (slot +0x304)
                            return 1;                                 // 0x4A5D97
                        }
                        this_->pathData->state = 5;                   // 0x4A585F ([edx+1Ch] = 5)
                        continue;                                     // 0x4A5866 - ASM RELOOP (join J6)
                    }
                }
                else
                {
                    // ---- the install-SUCCESS arm (asm 0x4A559E-0x4A569D) ----
                    fprintf_logger(g_dimuLog, "INSTALL-OK wpc=%d len=%08x v149=%d variant=%s\n",  // DIAG
                        newPath->waypointsCount, (unsigned int)newPath->pathLengthValue, smoothUsed,     // DIAG
                        (this_->pathData->waypointLevel != 2 || smoothUsed != 1) ? "FOLLOW" : "READY");  // DIAG
                    g_dimuMark = 9;                                   // DIAG
                    this_->pathData->state = 0;                       // 0x4A55A4 ([edx+1Ch] = 0)
                    this_->pathData->continueCounter = 10;            // 0x4A55B1 ([eax+2Ch] = 0Ah)
                    BPathData* v75 = this_->pathData;
                    if (v75->waypointLevel != 2 || smoothUsed != 1)   // 0x4A55BE-0x4A55CC (cmp [eax+0Ch],2; jnz;
                    {                                                 //  cmp var_260,1; jnz)
                        BPathData__setReadyToFollowPath(successor, newPath);   // 0x4A5648-0x4A564F
                        if (pathFound)                                // 0x4A5654-0x4A5659
                            BPathData__setLinkedPath(successor, this_->pathData, 1);   // 0x4A5661-0x4A5671
                        else
                            BPathData__setLinkedPath(successor, this_->pathData, 2);
                        successor->waypointLevel = 0;                 // 0x4A5676 ([ebx+0Ch] = 0)
                        activePath = newPath;                         // 0x4A5683 (var_26C = edi)
                        successor->range = this_->pathData->range;    // 0x4A5687-0x4A568A (+0x20)
                        successor->targetID = this_->pathData->targetID;   // 0x4A568D-0x4A5696 (+0x24)
                        this_->pathData = successor;                  // 0x4A5699 ([esi+118h] = ebx)
                        proceedToWalk = true;   /* the .c's break; the asm falls to loc_4A569F */
                                              // 0x4A569F - RESTRUCTURE: the goto walk_arm handoff
                    }
                    else
                    {
                        BPathData__setReadyToPath(successor, newPath, v75->range, v75->targetID);
                                                                      // 0x4A55CE-0x4A55DD (push tID, r, p)
                        if (pathFound)                                // 0x4A55E2-0x4A55ED
                            BPathData__setLinkedPath(successor, this_->pathData, 1);   // 0x4A55EF-0x4A5604
                        else
                            BPathData__setLinkedPath(successor, this_->pathData, 2);
                        successor->waypointLevel = 1;                 // 0x4A5609 ([ebx+0Ch] = ebp = 1)
                        activePath = newPath;                         // 0x4A5612
                        successor->range = this_->pathData->range;
                        successor->targetID = this_->pathData->targetID;
                        this_->pathData = successor;                  // 0x4A5628
                        if (*dword_689564 != 1 && *dword_68956C != 1) // 0x4A562E-0x4A5641 (cmp/jz -> reloop;
                            proceedToWalk = true;   /* the .c's break */   //  cmp/jnz -> the walk arm)
                                                                      //  0x4A569F - the goto walk_arm handoff
                        else
                            continue;                                 // 0x4A5634/0x4A5643 (the config-on reloop)
                    }
                }
                } // the state-2 arm's else closes - every arm path returned, continued,
                  // or set proceedToWalk (the two install-success handoffs)
                // ================= THE WALK ARM (asm loc_4A569F-0x4A5917) =============
                // Dispatch-loop level: reached from the state!=2 dispatch (0x4A4963
                // jnz) and from the two install-success handoffs above (proceedToWalk).
                v76 = activePath;                                             // 0x4A56A5 (ebp = var_26C; DEAD store -
                                                                              //  v76 is never read; transcribed 1:1)
                BVector* v77 = BPath__waypoint(activePath, &walkWp, this_->pathData->waypoint);
                                                                              // 0x4A56A9-0x4A56BD (var_A8)
                BVector__operator_minus(&this_->positionValue, &v172, v77);   // 0x4A56C2-0x4A56CD (this = ebx = positionValue)
                v172.z = 0.0f;                                                // 0x4A56EB
                if (sqrt((double)v172.y * v172.y + (double)v172.x * v172.x) >= 1.0e-7f)
                                                                              // 0x4A56D2-0x4A5705 (test ah,1; jz loc_4A570B
                {                                                             //  - the {<, NaN} set goes to the flag pair;
                                                                              //  the literal >= MATCH row)
                    // ---- the movement integration (the three named virtual dispatches;
                    // slots +0x320 = 200 / +0x334 = 205 / +0x328 = 202) ----
                    BVector* v83 = BPath__waypoint(activePath, &fwdWp, this_->pathData->waypoint);   // 0x4A5873-0x4A5889 (var_90)
                    this_->vfptr->calculateForward(this_, &this_->positionValue, v83, &fwdDir);
                                                                              // 0x4A588E-0x4A5892 (slot +0x320)
                    this_->vfptr->straightProjectedPosition(this_, &projectedPos, &fwdDir, deltaTime);
                                                                              // 0x4A5898-0x4A58B1 (slot +0x334; the dt
                                                                              //  arg = deltaTime's float bits, LODWORD(a3))
                    BVector* v85 = BPath__waypoint(activePath, &reachedWp, this_->pathData->waypoint);   // 0x4A58CB-0x4A58D6 (var_78)
                    reachedResult = this_->vfptr->calculateReachedPoint(this_, v85, &this_->positionValue, &projectedPos);
                                                                              // 0x4A58DB-0x4A58DE (slot +0x328)
                    if (reachedResult == 2)                                   // 0x4A58E4-0x4A58E7 (cmp eax,2; jz loc_4A570B)
                    {
                        // asm join J7 (0x4A570B-0x4A5729): the flag pair + RELOOP - the
                        // .c's fall-to-the-a4-fetch rendering is superseded by the asm.
                        g_dimuMark = 2;                                       // DIAG
                        BPathData__setFlag(this_->pathData, 1, 1);            // 0x4A570B-0x4A5715
                        BPathData__setFlag(this_->pathData, 0, 1);            // 0x4A571A-0x4A5724
                        continue;                                             // 0x4A5729
                    }
                    // asm 0x4A58ED-0x4A58F9: reachedResult == 1 or mConsecutiveSubstituteCount
                    // > 2 -> the flag pair then FALL to the a4 fetch (0x4A5919);
                    // otherwise no pair, fall directly.
                    if (reachedResult == 1 || this_->mConsecutiveSubstituteCount > 2)   // (+0x138; cmp ...,2; jle skips)
                    {
                        BPathData__setFlag(this_->pathData, 1, 1);            // 0x4A58FB-0x4A5905
                        BPathData__setFlag(this_->pathData, 0, 1);            // 0x4A590A-0x4A5914
                    }
                }
                else
                {
                    g_dimuMark = 1;                                           // DIAG (the arrival pair + reloop)
                    BPathData__setFlag(this_->pathData, 1, 1);                // 0x4A570B (the arrival path - the shared tail)
                    BPathData__setFlag(this_->pathData, 0, 1);
                    continue;                                                 // 0x4A5729 (reloop)
                }

                // ---- the a4 waypoint + the next-waypoint fetch (asm 0x4A5919-0x4A5973) ----
                BVector* v87 = BPath__waypoint(activePath, &moveTargetSrcWp, this_->pathData->waypoint);   // 0x4A5919-0x4A592D (var_60)
                moveTarget.x = v87->x;                                        // 0x4A5932-0x4A5948 (the component copies
                moveTarget.z = v87->z;                                        //  in the asm's order: [+eax] / [+4] / [+8])
                v88 = v87->y;
                BPathData* v89 = this_->pathData;
                moveTarget.y = v88;
                v90 = v89->waypoint;
                if (v90 < activePath->waypointsCount - 1)                     // 0x4A594C-0x4A5955 (cmp edx,ecx; jl)
                    BPath__waypoint(activePath, &nextWpNext, v90 + 1);        // 0x4A5962-0x4A596D (var_30; the fetch is
                else                                                          //  DISCARDED (eax dead at 0x4A5976) -
                    BPath__waypoint(activePath, &nextWpCur, v90);             //  0x4A5957-0x4A5960 (var_48; dead) -
                                                                              //  transcribed 1:1
                // the .c's v161 = COERCE_FLOAT(getUnitGroup(...)) - the pointer typed at
                // the reuse site (the null test at 0x4A5ADE is the pointer test).
                unitGroup = RGE_Game_World__getUnitGroup(                     // 0x4A5976-0x4A5986 (mGroupID @+0x88)
                        (RGE_Game_World*)(*base_game)->world, this_->mGroupID);
                if (!RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_, 1))   // 0x4A598B-0x4A599A (jz loc_4A5CAF)
                {
                    // 0x4A5CAF - RESTRUCTURE: the goto target_gate = the common fall
                    // into the target gate below (the .c's break of the synthetic nest)
                }
                else
                {
                // ---- the OB pass (asm 0x4A59A0-0x4A5CAB) ----
                    g_dimuMark = 3;                                           // DIAG
                    v91 = 0;
                    ignoreListSize = 0;                                       // 0x4A59A0-0x4A59A4
                    if (this_)
                    {
                        v91 = 1;
                        ignoreUnits[0] = this_->id;
                        ignoreListSize = 1;
                        if (this_->type == 80)                                // 0x4A59BE-0x4A59C2
                        {
                            TRIBE_Building_Object* buildingObj3 = (TRIBE_Building_Object*)this_;
                            bldgChildW = 0;
                            do
                            {
                                RGE_Static_Object* child4 =
                                    (RGE_Static_Object*)buildingObj3->linked_child[bldgChildW];
                                if (child4)                                   // 0x4A59CC-0x4A59D0
                                {
                                    ignoreUnits[v91++] = child4->id;          // 0x4A59D2-0x4A59D5
                                    ignoreListSize = v91;
                                    if (v91 >= _countof(ignoreUnits))         // 0x4A59DD-0x4A59E7
                                        break;
                                }
                                ++bldgChildW;
                            } while (bldgChildW < _countof(buildingObj3->linked_child));
                        }
                    }
                    findObSystemCollisions(obSystemToUse,                     // 0x4A59F2-0x4A5A0F (__fastcall; THE LEAD
                            this_,                                            //  CONFIRMED: push offset ignoreUnits
                            &this_->positionValue, &projectedPos,             //  @0x4A5A06; p1 = positionValue, p2 = the
                            ignoreUnits, v91, 0, 1.0f);                       //  projected position; checkTerrain = 0)
                    if (*numberObSystemCollisions <= 0)                       // 0x4A5A14-0x4A5A1B (cmp eax,edi(0); jle -
                    {                                                         //  the VALUE test: the deref sweep)
                        // 0x4A5CAF - the goto target_gate = the fall below (the OB-pass
                        // zero-count exit; the if-block's end converges there)
                    }
                    else
                    {
                        targetBlocked = 0;                                    // 0x4A5A25 (var_25C)
                        freshBlockers = 0;                                    // 0x4A5A29 (var_260)
                        goalb = 0;                                            // 0x4A5A2D (the goal slot)
                        do
                        {
                            obRecord = obSystemCollisions[goalb];             // 0x4A5A38-0x4A5A44 (the .c's obja walk ->
                                                                              //  the indexed form)
                            v95 = obRecord->Object;                           // 0x4A5A46 ([ecx+8])
                            if (v95)
                            {
                                v96 = RGE_Game_World__object((RGE_Game_World*)this_->owner->world, v95->id);
                                                                              // 0x4A5A51-0x4A5A5E
                                if (v96)
                                {
                                    if (v96->type == 80 && ((TRIBE_Building_Object*)v96)->linked_owner)
                                                                              // 0x4A5A6B-0x4A5A79 (byte @+0x5E; BASE-
                                                                              //  POINTER FIX: the .c's v96[2].mMaximum-
                                                                              //  PathingGroupSize = +0x168+0x74 = +0x1DC =
                                                                              //  linked_owner (asm [eax+1DCh]))
                                        v96 = (RGE_Static_Object*)((TRIBE_Building_Object*)v96)->linked_owner;
                                    if (v96)                                  // 0x4A5A7D-0x4A5A81
                                    {
                                        if (BPath__ignoreUnitsContain(activePath, v96->id) != 1)   // 0x4A5A87-0x4A5A97
                                        {
                                            v98 = BPathData__highestLinkedPathPointer(this_->pathData);
                                                                              // 0x4A5A9D-0x4A5AA3
                                            if (!v98 || BPath__ignoreUnitsContain(v98, v96->id) != 1)
                                                                              // 0x4A5AAC-0x4A5ABA (the .c's v97[4] = the
                                                                              //  id @+0x10; the named field form)
                                            {
                                                v99 = this_->pathData->targetID;   // 0x4A5AC0-0x4A5AC6
                                                if (v99 >= 0 && v96->id == v99)   // 0x4A5ACD-0x4A5AD0
                                                    targetBlocked = 1;    // 0x4A5AD2
                                                if (unitGroup == 0        // 0x4A5ADA-0x4A5AE0 (test ecx,ecx - the
                                                                              //  NULL test; the .c's v161 == 0.0 pun)
                                                    || BUnitGroup__getSpecificMoveReason(unitGroup, this_->id)
                                                                              // 0x4A5AE2-0x4A5AED
                                                    || (v100 = v95->master_obj->object_group, v100 == 18)
                                                                              // 0x4A5AEF-0x4A5AFF (word @+0x1E;
                                                                              //  18 = Building)
                                                    || v100 == 6          // 6 = Wall
                                                    || v100 == 8          // 8 = Gate
                                                    || v100 == 10         // 10 = Turret
                                                    || v100 == 9)         // 9 = Fortress/AA Turret
                                                {
                                                    v101 = activePath->creationTimeValue;   // 0x4A5B1D-0x4A5B21 ([eax+1Ch])
                                                    if (v101 < v96->vfptr->lastMoveTime(v96))
                                                                              // 0x4A5B1D-0x4A5B2C (slot +0x30C = 195;
                                                                              //  cmp ebx,eax; jnb skips)
                                                    {
                                                        ++freshBlockers;  // 0x4A5B2E-0x4A5B32
                                                        if (collisionCount < _countof(collisionUnits))
                                                                              // 0x4A5B33-0x4A5B3D (cmp ebp, offset
                                                                              //  numberObSystemCollisions = the array
                                                                              //  end = _countof(collisionUnits) = 256)
                                                        {
                                                            collisionUnits[collisionCount] = v96->id;
                                                                              // 0x4A5B43-0x4A5B47 ([ebp+0] = [edi+10h];
                                                                              //  the walker and v157 advance in lockstep
                                                                              //  - the indexed form is exact)
                                                            ++collisionCount;   // 0x4A5B46-0x4A5B4A (inc ecx; var_24C)
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            ++goalb;                                          // 0x4A5B51-0x4A5B6D (the count RE-READ per
                        } while (goalb < *numberObSystemCollisions);          //  iteration - faithful)
                        RGE_Moving_Object__resolveCollisions(this_, &projectedPos);   // 0x4A5B73-0x4A5B7D (unowned - the idb
                                                                              //  constant executes the original)
                        if (*numberObSystemCollisions == 1 && targetBlocked == *numberObSystemCollisions)
                                                                              // 0x4A5B82-0x4A5B90
                        {
                            BPathData__setFlag(this_->pathData, 0, *numberObSystemCollisions);
                                                                              // 0x4A5B92-0x4A5B9B (push eax = the loaded
                                                                              //  count value, guarded == 1)
                            BPathData__setFlag(this_->pathData, 2, 1);        // 0x4A5BA0-0x4A5BAA
                            continue;                                         // 0x4A5BAF - ASM RELOOP (join J1: the .c's
                                                                              //  fall-to-the-target-gate is superseded)
                        }
                        if (freshBlockers <= 0)                               // 0x4A5BB4-0x4A5BBA (test ecx; jle)
                        {
                            v76 = activePath;                                 // 0x4A5CAB (ebp = var_26C; DEAD store - 1:1)
                                                                              // 0x4A5CAF - the goto target_gate = the fall
                        }
                        else
                        {
                            goalc = 0;
                            if (*numberObSystemCollisions > 0)                // 0x4A5BC0-0x4A5BCA
                            {
                                pauseIdx = 0;
                                do
                                {
                                    RGE_Static_Object* v104 = obSystemCollisions[pauseIdx]->Object;   // 0x4A5BD5-0x4A5BDA ([ebx] -> [+8])
                                    if (v104)
                                    {
                                        pauseTargetId = this_->pathData->targetID;   // 0x4A5BDE-0x4A5BE4
                                        if (pauseTargetId < 0 || v104->id != pauseTargetId)   // 0x4A5BE7-0x4A5BEE (the target skip)
                                        {
                                            pauseBldg = (TRIBE_Building_Object*)RGE_Game_World__object(
                                                    (RGE_Game_World*)this_->owner->world, v104->id);
                                                                          // 0x4A5BF0-0x4A5BFD
                                            if (pauseBldg)
                                            {
                                                if (pauseBldg->type == 80 && pauseBldg->linked_owner)   // 0x4A5C06-0x4A5C16 (+0x1DC - as above)
                                                    pauseBldg = pauseBldg->linked_owner;
                                                pauseTarget = (RGE_Static_Object*)pauseBldg;
                                                if (pauseBldg)
                                                {
                                                    pausePriority = pauseTarget->vfptr->movementPriority(pauseTarget);
                                                                          // 0x4A5C1E-0x4A5C28 (slot +0x314 = 197)
                                                    if (pausePriority > this_->vfptr->movementPriority(this_))
                                                                          // 0x4A5C2A-0x4A5C36 (cmp ebp,eax; jle skips)
                                                        pauseTarget->vfptr->pauseMovement(pauseTarget, 0.5f);
                                                                          // 0x4A5C38-0x4A5C41 (slot +0x310 = 196;
                                                                          //  push 0.5 = the .c's 1056964608 bits)
                                                }
                                            }
                                        }
                                    }
                                    ++pauseIdx;
                                } while (pauseIdx < *numberObSystemCollisions);
                            }
                            linkedNow = this_->pathData->linkedPath;          // 0x4A5C61-0x4A5C67 (+0x4)
                            if (!linkedNow)
                            {
                                // RESTRUCTURE: LABEL_229's second site (0x4A5D3C) - identical
                                // statements to the createPath-failure site in the state-2 arm
                                // (region-distinct sites, identical statements).
                                this_->vfptr->stopMoving(this_);              // 0x4A5D3C-0x4A5D40 (LABEL_229 inlined;
                                                                              //  slot +0x304)
                                return 1;                                     // 0x4A5D46-0x4A5D4B
                            }
                            linkedNow->state = 2;                             // 0x4A5C72 ([edi+1Ch] = 2)
                            this_->pathData->linkedPath = 0;                  // 0x4A5C7F ([ecx+4] = 0)
                            if (this_->pathData)
                            {
                                BPathData__destructor(this_->pathData);       // 0x4A5C90-0x4A5C92
                                operator_delete_internal(this_->pathData);    // 0x4A5C97-0x4A5C98
                            }
                            this_->pathData = linkedNow;                      // 0x4A5CA0
                            continue;                                         // 0x4A5CA6 - ASM RELOOP (join J2: the .c's
                                                                              //  fall-to-the-target-gate is superseded)
                        }
                    }
                }
                // ---- the target gate (asm loc_4A5CAF-0x4A5CCB) ----
                // Reached: the walk arm's !isFlagSet fall, the OB-pass zero-count
                // exit, and the freshBlockers<=0 exit (all fall through from above).
                g_dimuMark = 15;                                                  // DIAG
                if (!targetObj)                                                   // 0x4A5CAF-0x4A5CB5 (test eax; jz)
                {
                    // -> the range gate (0x4A5DA1): RESTRUCTURE - the goto range_gate =
                    // the else-branch below (the gate is the target gate's only other
                    // exit besides the reloop and the moveUnit tail)
                }
                else if (RGE_Moving_Object__checkRange(this_, targetObj) != 1)    // 0x4A5CBB-0x4A5CC5 (owned; cmp al,1; jnz)
                {
                    // RESTRUCTURE: LABEL_241 (0x4A5E38 shared moveUnit tail) inlined -
                    // the second copy sits in the range gate below (both sites carry
                    // mark=11; the baseline's single shared label emits the identical
                    // mark/call/return sequence on each path).
                    g_dimuMark = 11;                                              // DIAG (the moveUnit tail; LABEL_241)
                    RGE_Moving_Object__moveUnit(this_, &fwdDir, deltaTime, &moveTarget);
                                                                                  // 0x4A5E38-0x4A5E4C (owned - hook-routed)
                    return 0;                                                     // 0x4A5E51
                }
                else
                {
                    continue;                                                     // 0x4A5CCB - ASM RELOOP (join J3: checkRange
                                                                                  //  == 1 re-dispatches; the .c's fall to the
                                                                                  //  range gate is superseded - the range gate
                                                                                  //  runs only when targetObj == 0)
                }
                // ---- the range gate (asm loc_4A5DA1-0x4A5E4C) ----
                if ((activePath->flags.valueValue & 1) != 0                       // 0x4A5DA1-0x4A5DAC (test ecx,eax; ja;
                                                                                  //  BBitVectorAND_2 = 1)
                    || (v115 = this_->pathData, X87_LE_NAN(v115->range, 0.1f))    // 0x4A5DB2-0x4A5DC6 (mov ecx,[esi+118h] =
                                                                                  //  v115; fld [ecx+20h]; fcomp flt_6545BC =
                                                                                  //  0.1f; test ah,41h; jnz - the {<=, NaN}
                                                                                  //  set -> moveUnit: DIVERGENT)
                    || (topPath = BPathData__highestLinkedPathPointer(v115),      // 0x4A5DC8-0x4A5DD1
                        topPath == 0)
                    || (v114 = topPath->waypointsCount,
                        v115 = this_->pathData,
                        v135 = BPath__waypoint(topPath, &maCopyWp, v114 - 1)->y,  // 0x4A5DDE-0x4A5DF8 (var_19C; .y @+8)
                        v116 = BPath__waypoint(topPath, &maIntendedWp, v114 - 1),   // 0x4A5DF6-0x4A5E01 (var_1A8;
                                                                          //  the REUSE of the maIntendedWp carve
                                                                          //  slot - the live reuse of the dead
                                                                          //  carve slots, faithful; TRANSCRIPTION
                                                                          //  FIX: this second fetch reuses v176/
                                                                          //  maIntendedWp, NOT boundsWp)
                        RGE_Static_Object__distance_to_position((RGE_Static_Object*)this_,
                                v116->x, v135, 0.0) - (double)v115->range       // 0x4A5E0B-0x4A5E10 (fsub [ebp+20h] on the
                            >= 1.0e-7f))                                          //  53-bit st0; fcomp flt_6565F4; test
                {                                                                 //  ah,1; jz -> moveUnit: the literal >= MATCH
                    // RESTRUCTURE: LABEL_241's second site (the range-gate moveUnit
                    // tail; the shared-label mark/call/return sequence per path)
                    g_dimuMark = 11;                                              // DIAG (the moveUnit tail; LABEL_241)
                    RGE_Moving_Object__moveUnit(this_, &fwdDir, deltaTime, &moveTarget);
                                                                                  // 0x4A5E38-0x4A5E4C (owned - hook-routed)
                    return 0;                                                     // 0x4A5E51
                }
                BPathData__reset(v115);                                           // 0x4A5E20-0x4A5E22 (owned)
                g_dimuMark = 12;                                                  // DIAG (fully arrived)
                // RESTRUCTURE: LABEL_239 (0x4A5E27 shared tail) inlined - the first
                // copy sits in the install-FAILURE arm above (region-distinct sites,
                // identical statements).
                this_->vfptr->stopMoving(this_);                                  // 0x4A5E27-0x4A5E2B (slot +0x304)
                return 2;                                                         // 0x4A5E31
    } // the dispatch loop
} // the function
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the capstone A/B harness - the _orig trampoline.
// The E9 at 0x4A47B0 (setHook) covers bytes 0x4A47B0-0x4A47B4 = the FIRST
// FIVE of the SIX bytes of `mov eax, large fs:0` (64 A1 00 00 00 00,
// lst :280765). The replay executes the FULL six-byte instruction (resuming
// mid-instruction at 0x4A47B5 would execute the stray 00 00 = add [eax],al),
// then jumps to 0x004A47B6 = `push 0FFFFFFFFh` (lst :280766) - the first
// instruction boundary past the E9 (the byte-boundary rule: the jump target
// = the entry + the replayed BYTE length from the .lst boundaries). The
// __thiscall ABI passes only ecx (= this_); edx is dead at entry, so the
// mov eax/jmp form is safe (the fastcall liveness rule) - and the replay's
// only clobber is eax, which is the replayed instruction's own destination.
// The SEH chain push at 0x4A47B8 uses the ORIGINAL handler address - reached
// by the jmp into the original body itself, so nothing here needs it.
static __declspec(naked) int __fastcall RGE_Moving_Object__doIntelligentMovementUpdate_orig(
    RGE_Moving_Object* this_, DWORD dummy)                            // DIAG
{
    __asm {
        mov eax, fs:[0]                // the replay of 0x4A47B0-0x4A47B5
        mov eax, 004A47B6h             // the resume: push 0FFFFFFFFh (lst :280766)
        jmp eax
    }
}

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the capstone logger wrapper. Lazy fopen, one handle
// per session. NO pointer values in the log (they shift between the paired
// runs); the float/double fields are logged as RAW BITS (the tick-signature
// discipline). wt/dt per the BMotion exemplar (the getter is pure - the
// extra call is precedented by the updateMovement harness). The bare
// sprintf/fopen/fprintf = the approved diagnostic exception.
// ---------------------------------------------------------------------------
// DIAG (remove at ship): the capstone A/B harness v2 - the shared log + the
// breadcrumb marks + the PATH-LIFECYCLE TRACE. FINDING from the first
// capture: the ignSz field is POLLUTED (the owned ignoreListSize fork - the
// vanilla body writes the game's 0x6AD578 slot, my body writes the owned
// definition) - not comparable, kept only as a marker. THE REAL SIGNAL:
// the world's path-id counter offset by one from wt=020081A - the lifecycle
// trace (createPath/destroyPath2/path, all events merged into this log)
// names the exact allocation. The g_dimuMark breadcrumbs tag my body's
// regions; the lifecycle rows carry the mark, so a createPath logged under
// mark=8 = my body's repath create. (The g_dimuLog/g_dimuMark definitions
// live at the top of the file - the breadcrumb sites precede this block.)

static FILE* DIMU_logOpen()                                               // DIAG
{
    if (!g_dimuLog)
    {
        g_dimuLog = fopen("val_log_dimu.txt", "at");
        if (!g_dimuLog)
            return (FILE*)0;
    }
    return g_dimuLog;
}

static void DIMU_logRow(const char* tag, RGE_Moving_Object* u)            // DIAG
{
    BPathData* pd = u->pathData;
    BPath* p = pd ? pd->path : (BPath*)0;
    unsigned int posBits[3];
    memcpy(posBits, &u->positionValue, 12);
    unsigned int rngBits = 0, pauseBits = 0, lenBits = 0;
    int st = -1, wp = -1, wl = -1, tid = -1, cc = -1, flg = -1, linked = -1;
    int pid = -1, wpc = -1, ctm = -1, pflg = -1;
    if (pd)
    {
        st = pd->state;
        wp = pd->waypoint;
        wl = pd->waypointLevel;
        flg = (int)pd->mFlags.valueValue;
        tid = pd->targetID;
        cc = pd->continueCounter;
        linked = (pd->linkedPath != 0);
        memcpy(&rngBits, &pd->range, 4);
        memcpy(&pauseBits, &pd->pauseTime, 4);
    }
    if (p)
    {
        pid = p->idValue;
        wpc = p->waypointsCount;
        memcpy(&lenBits, &p->pathLengthValue, 4);
        ctm = (int)p->creationTimeValue;
        pflg = (int)p->flags.valueValue;
    }
    fprintf(g_dimuLog, "%s id=%d st=%d wp=%d wl=%d flg=%x rng=%08x tid=%d pause=%08x cc=%d lnk=%d mk=%d"
        " pid=%d wpc=%d len=%08x ctm=%08x pflg=%x pos=%08x,%08x,%08x ignSz=%d obN=%d\n",
        tag, u->id, st, wp, wl, flg, rngBits, tid, pauseBits, cc, linked, g_dimuMark,
        pid, wpc, lenBits, ctm, pflg, posBits[0], posBits[1], posBits[2],
        ignoreListSize, *numberObSystemCollisions);
}

static int g_dimuTraceOrig = 0;                                           // DIAG: 1 = the vanilla body (_orig), 0 = the replacement (_new)

int __fastcall RGE_Moving_Object__doIntelligentMovementUpdate_logger(     // DIAG
    RGE_Moving_Object* this_, DWORD dummy)
{
    if (!DIMU_logOpen())
        return 1;
    unsigned int wt = ((RGE_Player*)this_->owner)->world->world_time;
    double dtl = RGE_Game_World__getWorldTimeDeltaSeconds(
        (RGE_Game_World*)((RGE_Player*)this_->owner)->world);
    unsigned int dtLo = 0, dtHi = 0;
    memcpy(&dtLo, &dtl, 4);
    memcpy(&dtHi, ((char*)&dtl) + 4, 4);
    fprintf(g_dimuLog, "TICK wt=%08x dt=%08x%08x\n", wt, dtHi, dtLo);
    g_dimuMark = 0;
    DIMU_logRow("IN ", this_);
    int r;
    if (g_dimuTraceOrig)
        r = RGE_Moving_Object__doIntelligentMovementUpdate_orig(this_, 0);
    else
        r = RGE_Moving_Object__doIntelligentMovementUpdate_new(this_, 0);
    DIMU_logRow("OUT", this_);
    fprintf(g_dimuLog, "RET=%d\n", r);
    return r;
}

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the PATH-LIFECYCLE TRACE. The trampolines replay
// the prologues past the E9s (the byte-boundary rule; the addresses from
// the .lst):
//  - createPath @0x624640: the E9 covers push 0FFFFFFFFh (2B, lst :921123)
//    + the first 3 bytes of push offset createPath_SEH (lst :921134, the
//    handler = 0x00647CEB, DATA XREF createPath+2); the replay reconstructs
//    BOTH, resume 0x00624647 (`mov eax, large fs:0`, lst :921125).
//  - destroyPath2 @0x624760: the E9 covers push esi (1B) + mov esi,[esp+4]
//    (4B); the replay = push esi + mov esi,[esp+8] (the +4 push shift),
//    resume 0x00624765 (`test esi,esi`).
//  - path @0x624610: the E9 covers mov edx,[esp+4] exactly (4B); the replay
//    = the same mov, resume 0x00624614 (`cmp edx,-1`). mov is
//    flag-preserving for the resume.
static __declspec(naked) BPath* __fastcall RGE_Game_World__createPath_orig(  // DIAG
    RGE_Game_World* this_, DWORD dummy)
{
    __asm {
        push    0FFFFFFFFh
        push    00647CEBh
        mov     eax, 00624647h
        jmp     eax
    }
}

static BPath* __fastcall RGE_Game_World__createPath_logger(                  // DIAG
    RGE_Game_World* this_, DWORD dummy)
{
    if (!DIMU_logOpen())
        return 0;
    BPath* r = RGE_Game_World__createPath_orig(this_, 0);
    if (r)
    {
        unsigned int lenBits = 0;
        memcpy(&lenBits, &r->pathLengthValue, 4);
        fprintf(g_dimuLog, "CREATE id=%d wpc=%d len=%08x ctm=%08x mk=%d\n",
            r->idValue, r->waypointsCount, lenBits, (int)r->creationTimeValue,
            g_dimuMark);
    }
    else
        fprintf(g_dimuLog, "CREATE id=-1 mk=%d\n", g_dimuMark);
    return r;
}

static __declspec(naked) int __fastcall RGE_Game_World__destroyPath2_orig(   // DIAG
    RGE_Game_World* this_, DWORD dummy, BPath* pathToDestroy)
{
    __asm {
        push esi
        mov esi, [esp+8]
        mov eax, 00624765h
        jmp eax
    }
}

static int __fastcall RGE_Game_World__destroyPath2_logger(                   // DIAG
    RGE_Game_World* this_, DWORD dummy, BPath* pathToDestroy)
{
    if (!DIMU_logOpen())
        return 0;
    int did = -1, dwpc = -1;
    unsigned int dlenBits = 0;
    if (pathToDestroy)
    {
        did = pathToDestroy->idValue;
        dwpc = pathToDestroy->waypointsCount;
        memcpy(&dlenBits, &pathToDestroy->pathLengthValue, 4);
    }
    int r = RGE_Game_World__destroyPath2_orig(this_, 0, pathToDestroy);
    fprintf(g_dimuLog, "DESTROY id=%d wpc=%d len=%08x r=%d mk=%d\n",
        did, dwpc, dlenBits, r, g_dimuMark);
    return r;
}

static __declspec(naked) BPath* __fastcall RGE_Game_World__path_orig(        // DIAG
    RGE_Game_World* this_, DWORD dummy, int pathID)
{
    __asm {
        mov edx, [esp+4]       // the entry layout = the original's ([esp]=ret, [esp+4]=pathID)
        cmp edx, 0FFFFFFFFh
        mov eax, 00624617h
        jmp eax
    }
}

static BPath* __fastcall RGE_Game_World__path_logger(                        // DIAG
    RGE_Game_World* this_, DWORD dummy, int pathID)
{
    if (!DIMU_logOpen())
        return 0;
    BPath* r = RGE_Game_World__path_orig(this_, 0, pathID);
    fprintf(g_dimuLog, "RESOLVE req=%d got=%d mk=%d\n", pathID,
        r ? r->idValue : -1, g_dimuMark);
    return r;
}

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the findObSystemCollisions ARG TRACE. The collision
// query's inputs (p1/p2 raw bits, the ignore count, checkTerrain, the radius)
// + the result count - logged from BOTH runs (the hook is global), so the
// first arg divergence between the paired captures names the exact flipped
// input. Prologue @0x4A0110 (lst :272780): push ebx/esi/edi (3B) + mov
// edi,edx (2B) = the E9's exact 5-byte span; the replay reconstructs all
// four instructions, resume 0x004A0115.
static __declspec(naked) void __fastcall findObSystemCollisions_orig(        // DIAG
    void* OB_System_To_Use, RGE_Moving_Object* obj, BVector* p1, BVector* p2,
    int* ignoreList, int ignoreListSize, bool checkTerrain, float radiusModifier)
{
    __asm {
        push ebx
        push esi
        push edi
        mov edi, edx
        mov eax, 004A0115h
        jmp eax
    }
}

static void __fastcall findObSystemCollisions_logger(                        // DIAG
    void* OB_System_To_Use, RGE_Moving_Object* obj, BVector* p1, BVector* p2,
    int* ignoreList, int ignoreListSize, bool checkTerrain, float radiusModifier)
{
    if (!DIMU_logOpen())
    {
        findObSystemCollisions_orig(OB_System_To_Use, obj, p1, p2,
            ignoreList, ignoreListSize, checkTerrain, radiusModifier);
        return;
    }
    findObSystemCollisions_orig(OB_System_To_Use, obj, p1, p2,
        ignoreList, ignoreListSize, checkTerrain, radiusModifier);
    unsigned int p1b[3] = { 0, 0, 0 }, p2b[3] = { 0, 0, 0 }, rb = 0;
    if (p1) memcpy(p1b, p1, 12);
    if (p2) memcpy(p2b, p2, 12);
    memcpy(&rb, &radiusModifier, 4);
    fprintf(g_dimuLog, "OBQ ob=%d uid=%d p1=%08x,%08x,%08x p2=%08x,%08x,%08x ign=%d n=%d ct=%d rad=%08x mk=%d\n",
        (OB_System_To_Use != 0) ? 1 : 0,
        obj ? obj->id : -1,
        p1b[0], p1b[1], p1b[2], p2b[0], p2b[1], p2b[2],
        ignoreListSize, *numberObSystemCollisions,
        checkTerrain ? 1 : 0, rb, g_dimuMark);
}

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the find_path ARG TRACE. The map-analysis query's
// inputs + the u8 result - logged from BOTH runs. Prologue @0x4982F0 (lst
// :259114+14): sub esp,20h (3B) + fld dword ptr [esp+28h] (7B, the sx load,
// D9 84 24 28 00 00 00) - the E9 covers the sub's 3B + the fld's first 2B
// (D9 84); the replay reconstructs BOTH, resume 0x004982FA (the fld's end,
// the first boundary past the E9 span).
static __declspec(naked) unsigned __int8 __fastcall RGE_Map_Analysis__find_path_orig(  // DIAG
    RGE_Map_Analysis* this_, DWORD dummy, BPath* ReturnPath, float sx, float sy,
    float dx, float dy, float dest_target_radius, float dest_radius,
    RGE_Player* player, int exclusion_num, Exclude_Rect* exclusion)
{
    __asm {
        sub esp, 20h
        fld dword ptr [esp+28h]
        mov eax, 004982F7h
        jmp eax
    }
}

static unsigned __int8 __fastcall RGE_Map_Analysis__find_path_logger(        // DIAG
    RGE_Map_Analysis* this_, DWORD dummy, BPath* ReturnPath, float sx, float sy,
    float dx, float dy, float dest_target_radius, float dest_radius,
    RGE_Player* player, int exclusion_num, Exclude_Rect* exclusion)
{
    if (!DIMU_logOpen())
    {
        return RGE_Map_Analysis__find_path_orig(this_, 0, ReturnPath, sx, sy,
            dx, dy, dest_target_radius, dest_radius, player, exclusion_num,
            exclusion);
    }
    unsigned int r = RGE_Map_Analysis__find_path_orig(this_, 0, ReturnPath,
        sx, sy, dx, dy, dest_target_radius, dest_radius, player,
        exclusion_num, exclusion);
    unsigned int sxb = 0, syb = 0, dxb = 0, dyb = 0, dtrb = 0, drb = 0;
    memcpy(&sxb, &sx, 4);
    memcpy(&syb, &sy, 4);
    memcpy(&dxb, &dx, 4);
    memcpy(&dyb, &dy, 4);
    memcpy(&dtrb, &dest_target_radius, 4);
    memcpy(&drb, &dest_radius, 4);
    // DIAG (the tail-content fields): the ReturnPath's count + the LAST
    // waypoint's raw bits - the label_103 tail test consumes exactly this
    // waypoint (index count-1); the paired-capture divergence at the tail
    // test (unit 5404) needs the output side of find_path, not just r.
    int rpc = -1;                                                     // DIAG
    unsigned int lwb0 = 0, lwb1 = 0, lwb2 = 0;                        // DIAG
    if (ReturnPath)                                                   // DIAG
    {                                                                 // DIAG
        rpc = ReturnPath->waypointsCount;                             // DIAG
        if (rpc > 0)                                                  // DIAG
        {                                                             // DIAG
            BVector lw;                                               // DIAG
            BPath__waypoint(ReturnPath, &lw, rpc - 1);                // DIAG (the idb constant 0x4BC440; unhooked)
            memcpy(&lwb0, &lw, 4);                                    // DIAG (BVector = {x, z, y} - offset order)
            memcpy(&lwb1, ((char*)&lw) + 4, 4);                       // DIAG
            memcpy(&lwb2, ((char*)&lw) + 8, 4);                       // DIAG
        }                                                             // DIAG
    }                                                                 // DIAG
    fprintf(g_dimuLog, "FPHOOK ma=%d rp=%d sx=%08x sy=%08x dx=%08x dy=%08x dtr=%08x dr=%08x r=%d rpc=%d lw=%08x,%08x,%08x mk=%d\n",
        (this_ != 0) ? 1 : 0, ReturnPath ? ReturnPath->idValue : -1,
        sxb, syb, dxb, dyb, dtrb, drb, (int)(r & 0xFF), rpc,
        lwb0, lwb1, lwb2, g_dimuMark);
    return (unsigned __int8)r;
}

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the get_map_analysis RETURN TRACE. Prologue
// @0x49B470 (lst :264917+14): mov edx,[esp+4] (4B) + sub esp,0Ch (3B) +
// test edx,edx (2B) = the E9's 5-byte span covers the mov + the sub's
// first byte (83); the replay reconstructs mov + sub + test, resume
// 0x0049B479 (`jnz short`, the flags from my test).
static __declspec(naked) RGE_Map_Analysis* __fastcall
    RGE_Map_Analysis_System__get_map_analysis_orig(                      // DIAG
    RGE_Map_Analysis_System* this_, DWORD dummy, RGE_Master_Static_Object* master_obj)
{
    __asm {
        mov edx, [esp+4]
        sub esp, 0Ch
        test edx, edx
        mov eax, 0049B479h
        jmp eax
    }
}

static RGE_Map_Analysis* __fastcall
    RGE_Map_Analysis_System__get_map_analysis_logger(                    // DIAG
    RGE_Map_Analysis_System* this_, DWORD dummy, RGE_Master_Static_Object* master_obj)
{
    if (!DIMU_logOpen())
    {
        return RGE_Map_Analysis_System__get_map_analysis_orig(this_, 0, master_obj);
    }
    RGE_Map_Analysis* r = RGE_Map_Analysis_System__get_map_analysis_orig(
        this_, 0, master_obj);
    fprintf(g_dimuLog, "MAGET ma_obj=%d ret_null=%d mk=%d\n",
        master_obj ? master_obj->id : -1, (r != 0) ? 1 : 0, g_dimuMark);
    return r;
}

// ---------------------------------------------------------------------------
// DIAG (remove at ship): the WAYPOINT-FETCH TRACE - BPath::waypoint
// (0x4BC440) hooked with a CALLER-FILTERED logger. The filter: only calls
// returning into the doIntelligentMovementUpdate body [0x4A47B0, 0x4A5E6C)
// are logged (the game's original body occupies the SAME addresses in the
// _orig dispatch, so the filter catches both paired runs; all other
// callers - BUnitGroup, UnitAIModule - take the unlogged fast path).
// Purpose: the label_103 tail test (v67 <= v151, unit 5404) consumed
// waypoint(v25=v142, pathData->waypoint) from the LINKED path's BPath -
// per-pathData state the lifecycle/arg traces cannot observe. The first
// divergent WPFETCH row names the exact read (path id, index, coords).
// Trampoline replay: the E9 at 0x4BC440 covers mov eax,[esp+8] (4B) +
// xor edx,edx's first byte; the replay reconstructs BOTH, resume
// 0x004BC446 (cmp eax,edx - intact; re-sets the flags for the jl).
// Call-entry esp = the original's entry esp (the naked fn is only
// entered via the logger's call), so the [esp+8] operand needs no shift.
static __declspec(naked) BVector* __fastcall BPath__waypoint_orig(       // DIAG
    BPath* this_, DWORD dummy, BVector* outVector, int wpIndex)
{
    __asm {
        mov eax, [esp+8]       ; wpIndex (call-entry layout: [esp]=ret, [esp+4]=out)
        xor edx, edx
        push 004BC446h
        ret
    }
}

// The filter ranges: (a) the game's DIMU body [0x4A47B5, 0x4A5E6C) - the
// _orig dispatch's callers; (b) the compiled _new body's window - runtime-
// derived at install (my body lives in the DLL's own .text, not the game's
// range; without (b) the _new run logs no WPFETCH rows at all).
static unsigned int g_wpGameLo = 0x4A47B5u;                              // DIAG
static unsigned int g_wpGameHi = 0x4A5E6Cu;                              // DIAG
static unsigned int g_wpNewLo = 0;                                       // DIAG
static unsigned int g_wpNewHi = 0;                                       // DIAG

static BVector* __fastcall BPath__waypoint_logger(                       // DIAG
    BPath* this_, DWORD dummy, BVector* outVector, int wpIndex)
{
    void* ra = _ReturnAddress();
    unsigned int c = (unsigned int)ra;
    int inDimu = ((c >= g_wpGameLo && c < g_wpGameHi)
        || (g_wpNewLo != 0 && c >= g_wpNewLo && c < g_wpNewHi));
    if (!inDimu)
        return BPath__waypoint_orig(this_, 0, outVector, wpIndex);
    if (!DIMU_logOpen())
        return BPath__waypoint_orig(this_, 0, outVector, wpIndex);
    BVector* r = BPath__waypoint_orig(this_, 0, outVector, wpIndex);
    int pid = this_ ? this_->idValue : -1;
    unsigned int b0 = 0, b1 = 0, b2 = 0;
    memcpy(&b0, r, 4);                                                   // BVector = {x, z, y} - offset order
    memcpy(&b1, ((char*)r) + 4, 4);
    memcpy(&b2, ((char*)r) + 8, 4);
    fprintf(g_dimuLog, "WPFETCH pid=%d idx=%d out=%08x,%08x,%08x mk=%d\n",
        pid, wpIndex, b0, b1, b2, g_dimuMark);
    return r;
}

void install_RGE_Moving_Object()
{
    setHook((void*)0x004A0540, RGE_Moving_Object__RGE_Moving_Object_new);
    setHook((void*)0x004A06A0, RGE_Moving_Object__vector_deleting_destructor_new);
    setHook((void*)0x004A06C0, RGE_Moving_Object__RGE_Moving_Object_read_new);
    setHook((void*)0x004A0800, RGE_Moving_Object__destructor_new);
    setHook((void*)0x00408B80, RGE_Moving_Object__getAngle_new);
    setHook((void*)0x00408B90, RGE_Moving_Object__zeroUserDefinedWaypoints_new);
    setHook((void*)0x00408BA0, RGE_Moving_Object__numberUserDefinedWaypoints_new);
    setHook((void*)0x00408BB0, RGE_Moving_Object__position_new);
    setHook((void*)0x00408BE0, RGE_Moving_Object__getPathData_new);
    setHook((void*)0x00408BF0, RGE_Moving_Object__unknown_new);
    setHook((void*)0x00408C00, RGE_Moving_Object__orientationForward_new);
    setHook((void*)0x004A1010, RGE_Moving_Object__new_sprite_new);
    setHook((void*)0x004A1050, RGE_Moving_Object__spawn_death_obj_new);
    setHook((void*)0x004A1090, RGE_Moving_Object__transform_new);
    setHook((void*)0x004A10D0, RGE_Moving_Object__is_moving_new);
    setHook((void*)0x004A10F0, RGE_Moving_Object__stop_moving_new);
    setHook((void*)0x004A1B30, RGE_Moving_Object__passableTile2_new);
    setHook((void*)0x004A1C30, RGE_Moving_Object__passableTile_new);
    setHook((void*)0x004A21D0, RGE_Moving_Object__canPathWithAdditionalPassability_new);
    setHook((void*)0x004A3000, RGE_Moving_Object__get_waypoint_checksum_new);
    setHook((void*)0x004A34B0, RGE_Moving_Object__addCurrentPath_new);
    setHook((void*)0x004A22C0, RGE_Moving_Object__findFirstTerrainAlongExceptionPath_new);
    setHook((void*)0x004A2420, RGE_Moving_Object__canLinePath2_new);
    setHook((void*)0x004A25B0, RGE_Moving_Object__canLinePath_new);
    setHook((void*)0x004A2D80, RGE_Moving_Object__findAvoidancePath_new);
    setHook((void*)0x004A11B0, RGE_Moving_Object__turn_towards_new);
    setHook((void*)0x004A1E80, RGE_Moving_Object__canQuickPath_new);
    setHook((void*)0x004A1FF0, RGE_Moving_Object__canPathWithObstructions_new);
    setHook((void*)0x004A35C0, RGE_Moving_Object__addFuturePath_new);
    setHook((void*)0x004A2710, RGE_Moving_Object__firstTileAlongLine_new);
    setHook((void*)0x004A19B0, RGE_Moving_Object__set_velocity_new);
    setHook((void*)0x004A19E0, RGE_Moving_Object__new_angle_new);
    setHook((void*)0x004A28C0, RGE_Moving_Object__maximumSpeed_new);
    setHook((void*)0x004A28F0, RGE_Moving_Object__waitingToMove_new);
    setHook((void*)0x004A2920, RGE_Moving_Object__setWaitingToMove_new);
    setHook((void*)0x004A2930, RGE_Moving_Object__userDefinedWaypoint_new);
    setHook((void*)0x004A2960, RGE_Moving_Object__addUserDefinedWaypoint_new);
    setHook((void*)0x004A2AB0, RGE_Moving_Object__removeAllUserDefinedWaypoints_new);
    setHook((void*)0x004A2AE0, RGE_Moving_Object__resizeUserDefinedWaypoints_new);
    setHook((void*)0x004A3770, RGE_Moving_Object__stopAllMovement_new);
    setHook((void*)0x004A3830, RGE_Moving_Object__velocity_new);
    setHook((void*)0x004A3920, RGE_Moving_Object__acceleration_new);
    setHook((void*)0x004A3B70, RGE_Moving_Object__moveForward_new);
    setHook((void*)0x004A3C00, RGE_Moving_Object__setPosition_new);
    setHook((void*)0x004A3F10, RGE_Moving_Object__pauseMovement_new);
    setHook((void*)0x004A2BB0, RGE_Moving_Object__getPatrolInformation_new);
    setHook((void*)0x004A3790, RGE_Moving_Object__setOrientation_new);
    setHook((void*)0x004A3C40, RGE_Moving_Object__yaw_new);
    setHook((void*)0x004A3DD0, RGE_Moving_Object__stopMoving_new);
    setHook((void*)0x004A3E00, RGE_Moving_Object__calculateFutureVelocity_new);
    setHook((void*)0x004A3F40, RGE_Moving_Object__precalcDistToTarget_new);
    setHook((void*)0x004A4000, RGE_Moving_Object__precalcDistToGoal_new);
    setHook((void*)0x004A40A0, RGE_Moving_Object__incrementWaypoint_new);
    setHook((void*)0x004A4460, RGE_Moving_Object__checkDistance_new);
    setHook((void*)0x004A45A0, RGE_Moving_Object__checkRange_new);
    setHook((void*)0x004A4650, RGE_Moving_Object__moveUnit_new);
    setHook((void*)0x004A3A10, RGE_Moving_Object__forwardVelocity_new);
    setHook((void*)0x004A3A30, RGE_Moving_Object__setForwardVelocity_new);
    setHook((void*)0x004A3AC0, RGE_Moving_Object__forwardAcceleration_new);
    setHook((void*)0x004A3AE0, RGE_Moving_Object__setForwardAcceleration_new);
    setHook((void*)0x004A6250, RGE_Moving_Object__calculateMaximumSpeed_new);
    setHook((void*)0x004A69B0, RGE_Moving_Object__setRunSprite_new);
    setHook((void*)0x004A69F0, RGE_Moving_Object__setMoveSprite_new);
    setHook((void*)0x004A6A30, RGE_Moving_Object__setFidgetSprite_new);
    setHook((void*)0x004A2C90, RGE_Moving_Object__distanceToHighestLevelPath2_new);
    setHook((void*)0x004A2D00, RGE_Moving_Object__distanceToHighestLevelPath_new);
    setHook((void*)0x004A6510, RGE_Moving_Object__calculateTurnRadius_new);
    setHook((void*)0x004A6350, RGE_Moving_Object__calculateYawAngle_new);
    setHook((void*)0x004A6BA0, RGE_Moving_Object__evacuate_area_new);
    setHook((void*)0x004A6C20, RGE_Moving_Object__find_evacuate_area_new);
    setHook((void*)0x004A6F00, RGE_Moving_Object__try_evac_tile_new);
    setHook((void*)0x004A1970, RGE_Moving_Object__handle_teleport_z_correction_new);
    setHook((void*)0x004A1100, RGE_Moving_Object__angle_to_object_new);
    setHook((void*)0x004A3870, RGE_Moving_Object__setVelocity_new);
    setHook((void*)0x004A3960, RGE_Moving_Object__setAcceleration_new);
    setHook((void*)0x004A6290, RGE_Moving_Object__calculateForward_new);
    setHook((void*)0x004A6860, RGE_Moving_Object__straightProjectedPosition_new);
    setHook((void*)0x004A63F0, RGE_Moving_Object__calculateReachedPoint_new);
    setHook((void*)0x004A6A70, RGE_Moving_Object__initMovementData_new);
    setHook((void*)0x004A47B0, RGE_Moving_Object__doIntelligentMovementUpdate_logger);   // DIAG: the A/B harness (flip back at ship)
    //setHook((void*)0x004A47B0, RGE_Moving_Object__doIntelligentMovementUpdate_new);
    setHook((void*)0x00624640, RGE_Game_World__createPath_logger);    // DIAG: the path-lifecycle trace
    setHook((void*)0x00624760, RGE_Game_World__destroyPath2_logger);  // DIAG
    setHook((void*)0x00624610, RGE_Game_World__path_logger);          // DIAG
    setHook((void*)0x004A0110, findObSystemCollisions_logger);        // DIAG: the OB-query arg trace
    setHook((void*)0x004982F0, RGE_Map_Analysis__find_path_logger);   // DIAG: the find_path arg trace
    setHook((void*)0x0049B470, RGE_Map_Analysis_System__get_map_analysis_logger);  // DIAG: the map-analysis return trace
    g_wpNewLo = (unsigned int)RGE_Moving_Object__doIntelligentMovementUpdate_new;  // DIAG: the _new body's window
    g_wpNewHi = g_wpNewLo + 0x4000;                                 // DIAG (generous bound; extra rows are harmless)
    setHook((void*)0x004BC440, BPath__waypoint_logger);               // DIAG: the waypoint-fetch trace (the DIMU-caller-filtered)
}
