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
// `v > cap || !(v > 0.0f)` matching the gate order.
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
    // (gate 1, jz @0x4A6916) or when NOT(ordered && v > 0) (gate 2, jz
    // @0x4A6927) - the gates are a CHAIN and gate 2 tests v against 0.0
    // only, so a NaN cap falls through BOTH and the original KEEPS the
    // speed: the composite below preserves that (the previous form nested
    // the cap compare into the keep condition and clamped on a NaN cap)
    if (v10.y > cap || !(v10.y > 0.0f))
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
}
