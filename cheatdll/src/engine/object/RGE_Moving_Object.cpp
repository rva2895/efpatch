#include "stdafx.h"

// .rdata vftable of RGE_Moving_Object (lst :990431)
static RGE_Moving_ObjectVtbl* const rge_moving_object_vftable =
    (RGE_Moving_ObjectVtbl*)0x0065BE14;

// casts.cpp:140 - the speed-reduction patch helper (__stdcall, raw float bits
// in eax). BAKE SOURCE for maximumSpeed_new (speed2) and, in the M3c batch,
// calculateFutureVelocity_new (speed3).
extern uint32_t __stdcall getSpeedModifier(RGE_Static_Object* unit);

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
float __fastcall RGE_Moving_Object__getTurnRadius_new(RGE_Moving_Object* this_, DWORD dummy)
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
// Range [0x4A28C0, 0x4A28F0), returns float. ABI: __thiscall, no stack args.
// Base decode (.c:174638-174649): IF sprite @+0x1C && sprite->base_speed
// @+0x74 > 0: master_obj->speed @+0xB4 * base_speed; ELSE master_obj->speed.
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
float __fastcall RGE_Moving_Object__maximumSpeed_new(RGE_Moving_Object* this_, DWORD dummy)
{
    uint32_t modifier_bits = getSpeedModifier((RGE_Static_Object*)this_);   // baked SPEED_MOD: casts.cpp:882-region stub (unconditional fetch before the gate)
    float modifier;
    memcpy(&modifier, &modifier_bits, sizeof(modifier));   // the helper returns raw float bits (casts.cpp:140 "returns 32 bit float in eax") - 4-byte value copy, aliasing-clean
    if (this_->sprite != 0 && this_->sprite->base_speed > 0.0)
        return (float)((double)this_->master_obj->speed * (double)modifier * (double)this_->sprite->base_speed);   // asm stub: fmul x2 at 53-bit, left-assoc
    return (float)((double)this_->master_obj->speed * (double)modifier);       // asm stub loc_4A28E4
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
        wp->x = x;                                   // .c:174699
        wp->y = z;                                   // .c:174700 (the y/z SWAP - faithful)
        wp->z = y;                                   // .c:174701
        double fx = floor((double)wp->x);            // .c:174702-174704
        double fz = floor((double)wp->z);            // .c:174705-174706
        if (fabs((double)wp->x - fx) < 0.0000001000000011686097)   // .c:174708
            wp->x = (float)(fx + 0.5);               // .c:174709
        if (fabs((double)wp->z - fz) < 0.0000001000000011686097)   // .c:174711
            wp->z = (float)(fz + 0.5);               // .c:174712
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
// @ 0x004A3B70. Range [0x4A3B70, 0x49C00-region), void. Decode
// (.c:175500-175519): IF fabs(v) >= 1e-7: positionValue snapshot +
// BVector__operator_mul (:5789: result = v * orientationForward-
// Value — the scalar-vector scale) + positionValue += the scaled
// vector + vfptr->teleport(this, x, Z, Y) — the teleport ARGS ARE
// SWAPPED (z passed as the y-slot, y as the z-slot; asm-confirmed
// at write per the .c push order .z/.y).
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
        this_->vfptr->teleport(this_, pos.x, pos.z, pos.y);   // .c:175513-175517 (the arg order matches the original exactly)
    }
}

// Replacement for RGE_Moving_Object::setPosition. Original:
// @ 0x004A3C00. Range [0x4A3C00, 0x4A3C40). Decode (.c:175522-
// 175530): positionValue @+0xF4 = *pos + vfptr->teleport(this,
// pos->x, pos->Z, pos->Y) — the same z/y ARG SWAP.
void __fastcall RGE_Moving_Object__setPosition_new(
    RGE_Moving_Object* this_, DWORD dummy, BVector* pos)
{
    this_->positionValue = *pos;                     // .c:175524
    this_->vfptr->teleport(this_, pos->x, pos->z, pos->y);   // .c:175525-175529 (the arg order matches the original exactly)
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
}
