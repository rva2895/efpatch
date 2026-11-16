#include "stdafx.h"

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
// @ 0x00408B80 (idb_functions.h). Decode (.c:51402-51407): return
// this->angle (float @+0xC8 = 200; the .c's "v1+200" with v1 = the
// __thiscall ecx this - the "possibly undefined" render is the
// classic ecx artifact; the Moving ctor's extension init (batch
// 1b-2 store map) confirms angle @+0xC8). The master vtbl slot is
// NAMED getTurnRadius but the runtime body returns the angle field.
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): the original is
// fld dword ptr [ecx+0C8h]; retn (asm 0x408B80-0x408B86) - the m32
// load is exact; the double return is bit-identical for this body.
double __fastcall RGE_Moving_Object__getTurnRadius_new(RGE_Moving_Object* this_, DWORD dummy)
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
// @+0xE8? — userDefinedWaypoints @232 = 0xE8 per the Moving walk).
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
// (BVector) ✓) + the zero-fill loop (12 bytes per element, asm
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
            double d = (double)RGE_Static_Object__distance_to_object(
                (RGE_Static_Object*)this_, child) - 0.1;             // asm 0x4A3FB7-0x4A3FBF (flt_6545BC)
            if (d < (double)result)
                result = (float)d;                                   // asm 0x4A3FC5-0x4A3FD0
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
        minDist = (float)((double)wd - (double)pd->range - 0.1);     // asm 0x4A4323-0x4A4336 (the obj == 0 seed)
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
    float v14 = this_->vfptr->calculateYawAngle(this_, &fwd, a2, a3, &yawArg);   // asm 0x4A46D7-0x4A46E5 (slot +0x324)
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

void install_RGE_Moving_Object()
{
    setHook((void*)0x004A0540, RGE_Moving_Object__RGE_Moving_Object_new);
    setHook((void*)0x004A06A0, RGE_Moving_Object__vector_deleting_destructor_new);
    setHook((void*)0x004A06C0, RGE_Moving_Object__RGE_Moving_Object_read_new);
    setHook((void*)0x004A0800, RGE_Moving_Object__destructor_new);
    setHook((void*)0x00408B80, RGE_Moving_Object__getTurnRadius_new);
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
}
