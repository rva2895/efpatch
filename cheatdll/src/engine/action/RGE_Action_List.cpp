#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"

// Replacement for RGE_Action_List::create_task_action. Original:
// @ 0x004067E0 (idb_functions.h:64). Range [0x4067E0, 0x406AA3) (lst
// :10075/:10368, retn 18h = 6 stack args), RGE_Action* return. ABI:
// __thiscall. Virtual: the Action_List vftable slot 2 = +0x8 (line-arith
// 15065-15063 = 2; DATA XREF lst :10077). Caller:
// TRIBE_Action_List__create_task_action+7E5 (the ladder, hook-routed).
// Semantics: the FAMILY ANCHOR - the task->action_type switch (the
// GAME_FACTS registry field @+6) that constructs every derived action:
// 4 Explore, 5 Gather (dual-arm: the target-object Gather ctor vs the
// coordinate Gather2 ctor), 7 Attack (dual-arm: the target-object
// Attack ctor vs the coordinate Attack2 ctor, with the UnitAI seam),
// 0xA Bird, 0xD Guard, 0x15 Make; default -> 0. The gather_flag tail
// marks a fresh Gather's gbg_unknown_1 (@+0x5C, walked) = 1.
// Decode (asm 0x4067E0-0x406AA2): the switch via the 18-entry range
// table (byte_406AC4/jpt_406817, the -4 bias; default = 6,8,9,11,12,
// 14-20 + out-of-range). The allocations (operator_new, sizeof walked
// per the derived structs): Explore 0x40 (64 - the bare base), Gather
// 0x60 (96 - the extension through gbg_unknown_1 @92), Attack 0x68
// (104 - through lastTargetPositionZ @100), Bird 0x40 (64), Guard
// 0x40 (64), Make 0x44 (68 - work_timer @64). The per-arm SEH state
// var (var_4 = 0..7) + the unwind chunk @0x6408C0 = the allocation
// folds (__except + operator_delete_internal, the attack_new/enter_new
// precedent). THE BASE-POINTER RESOLUTIONS (the .c's
// `master_obj[1].X` renders - master_obj typed RGE_Master_Action_
// Object*, sizeof 300): [1].radius_x = 300+0x3C = 360 =
// minimum_weapon_range; (__int16)[1].death_sprite = 300+40 = 340 =
// missile_id; HIWORD([1].undead_sprite) = 300+44+2 = 346 =
// fire_missile_at_frame - all three the warm Combat-master groundings
// (asm [edx+168h]/[edx+154h]/[edx+15Ah]); the Action->Combat master
// cast is the verified master ladder. The UnitAI seam (case 7):
// setCurrentAction(258h = 600, the RGE_UNITAI_ACTION registry ATTACK
// value) - the setCurrentTarget calls: the target arm passes
// target->id @+0x10 / world_x/y/z @+0x48/4C/50 (walked), the
// coordinate arm passes -1 + the raw x/y/z and runs ONLY when the
// module exists (the !module path jumps past it), with the module
// RE-FETCHED (asm 0x406912). The gather tail (asm 0x406A25-0x406A38):
// gated on the action non-null AND gather_flag. Join inventory:
// loc_406A90 (every arm) + loc_406A23 (the failed allocations) - all
// covered. Collision scan [0x4067E0, 0x406AA3): extracted-and-tested,
// 0 in range (CLEAN). Two-pass bar: 707 bytes, 3 gotos (the IDA
// arm-merge renders), 0 asm back-edges - single-pass.
RGE_Action* __fastcall RGE_Action_List__create_task_action_new(
    RGE_Action_List* this_, DWORD dummy, RGE_Task* task,
    RGE_Static_Object* target_obj, float target_x, float target_y,
    float target_z, int gather_flag)
{
    RGE_Action* result = 0;
    switch (task->action_type)                                       // asm 0x4067FF (movsx word @+6 - the GAME_FACTS registry field)
    {
    case RGE_ACTION_TYPE_EXPLORE:                                    // EXPLORE (4)
    {
        RGE_Action_Explore* allocated = (RGE_Action_Explore*)operator_new_internal(sizeof(RGE_Action_Explore));   // asm 0x40697F (push 40h; the walked sizeof = 64)
        __try
        {
            if (allocated)
                result = (RGE_Action*)RGE_Action_Explore__RGE_Action_Explore(allocated, this_->obj, task, target_x, target_y, target_z);   // asm 0x4069B3 (:35)
        }
        __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // the SEH fold: the unwind chunk @0x6408C0
        {
        }
        return result;                                               // asm -> loc_406A90
    }
    case RGE_ACTION_TYPE_GATHER:                                     // GATHER (5) - the dual arm
    {
        if (target_obj)                                              // asm 0x4069C5
        {
            RGE_Action_Gather* allocated = (RGE_Action_Gather*)operator_new_internal(sizeof(RGE_Action_Gather));   // asm 0x4069C1 (push 60h; the walked sizeof = 96)
            __try
            {
                if (allocated)
                    result = (RGE_Action*)RGE_Action_Gather__RGE_Action_Gather(allocated, this_->obj, task, target_obj);   // asm 0x4069E7
            }
            __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)
            {
            }
        }
        else
        {
            RGE_Action_Gather* allocated = (RGE_Action_Gather*)operator_new_internal(sizeof(RGE_Action_Gather));   // asm 0x4069EE (the shared push 60h)
            __try
            {
                if (allocated)
                    result = (RGE_Action*)RGE_Action_Gather__RGE_Action_Gather2(allocated, this_->obj, task, target_x, target_y, target_z);   // asm 0x406A1C (:42)
            }
            __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)
            {
            }
        }
        if (result && gather_flag)                                   // asm 0x406A25-0x406A2F (the Gather-only tail)
            ((RGE_Action_Gather*)result)->gbg_unknown_1 = 1;         // asm 0x406A31 (mov dword [eax+5Ch], 1 - the .c's result[1].target2ID render = the base-pointer match: the Gather extension field @92, walked)
        return result;                                               // asm -> loc_406A90
    }
    case RGE_ACTION_TYPE_ATTACK:                                     // ATTACK (7) - the dual arm
    {
        UnitAIModule* module = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);   // asm 0x40685F (:261; the up-cast - the verified Action_Object subset Static_Object ladder)
        if (module)                                                  // asm 0x40686E
        {
            UnitAIModule__setCurrentAction(module, RGE_UNITAI_ACTION_ATTACK);   // asm 0x40687F (258h = 600 - the RGE_UNITAI_ACTION registry ATTACK value)
            if (target_obj)                                          // asm 0x406884
                UnitAIModule__setCurrentTarget(module, target_obj->id, target_obj->world_x, target_obj->world_y, target_obj->world_z);   // asm 0x4068A2 (id @+0x10, world_x/y/z @+0x48/4C/50)
        }
        if (target_obj)                                              // asm 0x4068A7 (LABEL_7)
        {
            RGE_Action_Attack* allocated = (RGE_Action_Attack*)operator_new_internal(sizeof(RGE_Action_Attack));   // asm 0x4068AB (push 68h; the walked sizeof = 104)
            __try
            {
                if (allocated)
                    result = (RGE_Action*)RGE_Action_Attack__RGE_Action_Attack(allocated, this_->obj, target_obj, task->move_sprite, task->work_sprite, 0, task->work_range, ((RGE_Master_Combat_Object*)this_->obj->master_obj)->minimum_weapon_range, ((RGE_Master_Combat_Object*)this_->obj->master_obj)->missile_id, ((RGE_Master_Combat_Object*)this_->obj->master_obj)->fire_missile_at_frame);   // asm 0x4068F8 (the three master fields @360/340/346 - the base-pointer renders resolved; the Action->Combat master cast, the verified ladder)
            }
            __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)
            {
            }
            return result;                                           // asm 0x4068FD -> loc_406A90
        }
        if (module)                                                  // asm loc_406902 - the coordinate arm runs only when the module exists (the !module path jumps past it)
            UnitAIModule__setCurrentTarget(RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj), -1, target_x, target_y, target_z);   // asm 0x406912-0x406919 (the RE-FETCHED unitAI, faithful)
        RGE_Action_Attack* allocated2 = (RGE_Action_Attack*)operator_new_internal(sizeof(RGE_Action_Attack));   // asm 0x40691E (push 68h)
        __try
        {
            if (allocated2)
                result = (RGE_Action*)RGE_Action_Attack__RGE_Action_Attack2(allocated2, this_->obj, target_x, target_y, target_z, task->move_sprite, task->work_sprite, 0, task->work_range, ((RGE_Master_Combat_Object*)this_->obj->master_obj)->minimum_weapon_range, ((RGE_Master_Combat_Object*)this_->obj->master_obj)->missile_id, ((RGE_Master_Combat_Object*)this_->obj->master_obj)->fire_missile_at_frame);   // asm 0x406975 (:8)
        }
        __except (operator_delete_internal(allocated2), EXCEPTION_CONTINUE_SEARCH)
        {
        }
        return result;                                               // asm 0x40697A -> loc_406A90
    }
    case RGE_ACTION_TYPE_BIRD:                                       // BIRD (10)
    {
        RGE_Action_Bird* allocated = (RGE_Action_Bird*)operator_new_internal(sizeof(RGE_Action_Bird));   // asm 0x40681E (push 40h; the walked sizeof = 64)
        __try
        {
            if (allocated)
                result = (RGE_Action*)RGE_Action_Bird__RGE_Action_Bird(allocated, this_->obj, task, target_x, target_y, target_z);   // asm 0x406852
        }
        __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)
        {
        }
        return result;                                               // asm 0x406857 -> loc_406A90
    }
    case RGE_ACTION_TYPE_GUARD:                                      // GUARD (13)
    {
        RGE_Action_Guard* allocated = (RGE_Action_Guard*)operator_new_internal(sizeof(RGE_Action_Guard));   // asm 0x406A62 (push 40h; the walked sizeof = 64)
        __try
        {
            if (allocated)
                result = (RGE_Action*)RGE_Action_Guard__RGE_Action_Guard(allocated, this_->obj, target_obj);   // asm 0x406A87
        }
        __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)
        {
        }
        return result;                                               // asm 0x406A8C -> loc_406A90
    }
    case RGE_ACTION_TYPE_MAKE:                                       // MAKE (21)
    {
        RGE_Action_Make* allocated = (RGE_Action_Make*)operator_new_internal(sizeof(RGE_Action_Make));   // asm 0x406A3A (push 44h; the walked sizeof = 68)
        __try
        {
            if (allocated)
                result = (RGE_Action*)RGE_Action_Make__RGE_Action_Make(allocated, this_->obj, task);   // asm 0x406A5B
        }
        __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)
        {
        }
        return result;                                               // asm 0x406A60 -> loc_406A90
    }
    default:
        return 0;                                                    // asm def_406817 (xor eax, eax)
    }
}

void install_RGE_Action_List()
{
    setHook((void*)0x004067E0, RGE_Action_List__create_task_action_new);
}
