#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Make vftable (.rdata:00654B80, lst :986304; slot 0 =
// the SHARED family scalar deleting destructor; the tier owns no
// destructor of its own). THE EXTENSION FIELD: work_timer @+0x40 = 64
// (the Action mirror + work_timer - the same slot the Enter tier gives
// mFirstTimeEnter and the Move_To tier gives range).
static RGE_Action_MakeVtbl* const rge_action_make_vftable =
    (RGE_Action_MakeVtbl*)0x00654B80;

// Replacement for RGE_Action_Make::RGE_Action_Make2 (the read ctor).
// Original: @ 0x00406F60. Range [0x406F60, 0x406FC1) (lst :11194/:11237,
// retn 8 = 2 stack args), RGE_Action_Make* return. ABI: __thiscall.
// NOT virtual. Callers: RGE_Action_List__create_action2 +21B (the load
// path). The SEH: the trivial unwind (the chunk @0x640930). Decode (asm
// 0x406F60-0x406FC0): RGE_Action::RGE_Action2(this, infile, obj_in, 1)
// (:170 - HOOKED -> routes), rge_read(infile, &work_timer @+0x40, 4),
// the vfptr store (.rdata:00654B80), action_type = 21 (the MAKE
// registry value); return this.
RGE_Action_Make* __fastcall RGE_Action_Make__RGE_Action_Make2_new(
    RGE_Action_Make* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x406F8A (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    rge_read(infile, &this_->work_timer, sizeof(this_->work_timer));  // asm 0x406FAA (@+0x40, 4 - the staged BEFORE the vfptr/type stores, the asm order)
    this_->vfptr = rge_action_make_vftable;                           // asm 0x406F9E
    this_->action_type = RGE_ACTION_TYPE_MAKE;                        // asm 0x406FA4 (mov word @+4, 15h)
    return this_;                                                     // asm 0x406FB3 (mov eax, esi)
}

// Replacement for RGE_Action_Make::RGE_Action_Make (the ctor). Original:
// @ 0x00406FD0. Range [0x406FD0, 0x406FFC) (lst :11246/:11266, retn 8 =
// 2 stack args), RGE_Action_Make* return. ABI: __thiscall. NOT virtual.
// Caller: RGE_Action_List__create_task_action +27B (the OWNED anchor -
// routes). NO SEH (plain). Decode (asm 0x406FD0-0x406FFB): RGE_Action::
// RGE_Action(this, obj_in, 1) (:172 - HOOKED -> routes); the vfptr,
// action_type, task @+0x30, work_timer = 120.0f (the IMM BIT-STORE -
// mov dword [esi+40h], 42F00000h; the exact float); return this.
RGE_Action_Make* __fastcall RGE_Action_Make__RGE_Action_Make_new(
    RGE_Action_Make* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x406FDA (:172 - HOOKED -> routes)
    this_->vfptr = rge_action_make_vftable;                       // asm 0x406FE3
    this_->action_type = RGE_ACTION_TYPE_MAKE;                    // asm 0x406FE9 (mov word @+4, 15h)
    this_->task = task_in;                                        // asm 0x406FEF ([esi+30h])
    this_->work_timer = 120.0f;                                   // asm 0x406FF2 (mov dword [esi+40h], 42F00000h)
    return this_;                                                 // asm 0x406FF9 (mov eax, esi)
}

// Replacement for RGE_Action_Make::first_in_stack. Original:
// @ 0x00407000. Range [0x407000, 0x40702B) (lst :11275/:11301, retn 4 =
// 1 stack arg), void. ABI: __thiscall. Virtual: the Make vftable (the
// DATA XREF lst :11276). NO SEH. Decode (asm 0x407000-0x40702A):
// set_state(6 Work) via the vtbl slot 24 = +0x60 (the SELF VIRTUAL
// dispatch - the named form required); task->attribute_type @+0x14
// (i16, the word load) - the SIGNED negative gate (test ax,ax; jl =
// the 0x8000 test): non-negative -> attribute_type_held @+0x5C != it
// (the word cmp) -> set_attribute(obj, type, 0.0f) (slot 62 = +0xF8 =
// 248 decimal). THE ATTRIBUTE SEMANTICS (the owner-supplied registry):
// the type = the resource the task gathers (15 Meat / 16 Berry /
// 17 Fish / 0-3 the storages) - setting the unit's carried attribute.
void __fastcall RGE_Action_Make__first_in_stack_new(
    RGE_Action_Make* this_, DWORD dummy, unsigned __int8 first_time)
{
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);        // asm 0x407007 (push 6; call [eax+60h] - slot 24 = +0x60)
    __int16 attribute_type = this_->task->attribute_type;         // asm 0x40700D (mov ax, [ecx+14h])
    if (attribute_type >= 0)                                      // asm 0x407011-0x407014 (test ax, ax; jl - the signed negative gate)
    {
        if (this_->obj->attribute_type_held != attribute_type)    // asm 0x407019 (cmp [ecx+5Ch], ax)
            this_->obj->vfptr->set_attribute(this_->obj, attribute_type, 0.0f);   // asm 0x407024 (slot 62 = +0xF8 = 248 decimal; (type, 0.0))
    }
}

// Replacement for RGE_Action_Make::set_state. Original: @ 0x00407030.
// Range [0x407030, 0x407070) (lst :11310/:11339, retn 4 = 1 stack arg),
// void. ABI: __thiscall. Virtual: the Make vftable (the DATA XREF lst
// :11310). NO SEH. THE SINGLE-CASE FORM: the asm cmp al,6/jnz - no
// table (the table-byte check: nothing to diff). Decode (asm
// 0x407030-0x40706F): state = new_state; ONLY Work(6): task =
// task @+0x30; obj; task->attribute_type @+0x14 == 16 (BERRY STORAGE -
// the owner-supplied registry) -> attributes[36] = Farm Food Amount
// ([edx+90h] = +0x90 = 36*4; the RGE_PLAYER_ATTRIBUTE enum);
// else -> attributes[88] = Fish Trap Food Amount ([edx+160h] = 88*4);
// timer @+0x2C = work_timer @+0x40 = the value (the two m32 stores,
// the asm order timer first).
void __fastcall RGE_Action_Make__set_state_new(
    RGE_Action_Make* this_, DWORD dummy, unsigned __int8 new_state)
{
    this_->state = new_state;                                     // asm 0x407036 ([ecx+0Ch])
    if (new_state == RGE_ACTION_STATE_WORK)                       // asm 0x407015-region (cmp al, 6; jnz -> locret)
    {
        RGE_Task* task = this_->task;                             // asm 0x40703B ([ecx+30h])
        RGE_Action_Object* obj = this_->obj;                      // asm 0x40703E ([ecx+8])
        float* attributes = obj->owner->attributes;               // asm 0x407046-0x407049 (owner @+0x18 -> attributes @+0xAC = 172)
        float make_time;
        if (task->attribute_type == 16)                           // asm 0x407041 (cmp word [eax+14h], 10h - BERRY STORAGE)
            make_time = attributes[RGE_PLAYER_ATTRIBUTE_FARM_FOOD_AMOUNT];      // asm 0x407051 (mov eax, [edx+90h] = attributes[36] - Farm Food Amount)
        else
            make_time = attributes[RGE_PLAYER_ATTRIBUTE_FISH_TRAP_FOOD_AMOUNT]; // asm 0x407062 (mov eax, [edx+160h] = attributes[88] - Fish Trap Food Amount)
        this_->timer = make_time;                                 // asm 0x407059/0x40706A ([ecx+2Ch])
        this_->work_timer = make_time;                            // asm 0x40705C/0x40706D ([ecx+40h])
    }
}

// Replacement for RGE_Action_Make::update. Original: @ 0x00407080.
// Range [0x407080, 0x40715A) = 218 bytes (lst :11348/:11444, retn = no
// stack args), unsigned __int8 return. ABI: __thiscall. Virtual: the
// Make vftable slot (DATA XREF lst :11348). NO SEH. THE ARM FORM: the
// asm dec/sub-5 chain (Done(1) -> return 1; NOT Work(6) -> return 0) -
// no jump table (the table-byte check: nothing to diff). Decode (asm
// 0x407080-0x407159): Done -> return 1; Work: THE WORK GATE -
// timer > 0.0 ordered (test ah,41h/jz = the literal > MATCH row) ->
// the work block; {timer <= 0, NaN} -> work_timer <= 0.0: X87_LE_NAN
// (test ah,41h/jnz - the {<=,NaN} set -> the work block - THE MACRO
// REQUIRED: the NaN work_timer FIRES in the original; the literal <=
// would take the else = DIVERGENT); the fall (both ordered > 0) -> THE
// EMPTY-CARRY CHECK: attribute_amount_held @+0x54 > 0 ordered (the
// literal > MATCH row - the jz targets the return-0) -> return 0;
// {<= 0, NaN} -> die_die_die (slot 57 = +0xE4 = 228 decimal) ->
// return 0. THE WORK BLOCK (loc_4070DD): pre_amount =
// attribute_amount_held (the snapshot); gain = (float)(delta *
// master->gbg_work_rate_2 @+0x114 = 276 * task->work_val1 @+0x1C = 28)
// (the 53-bit fmul chain, ONE fstp store-round - the rule (d)/(e)
// form); the work_flag @+0x30 (word) gate (the flags persisting across
// the x87 block): non-zero -> add_attribute_num (slot 32 = +0x80) with
// (task->attribute_type, gain, 0); zero -> set_attribute_amount (slot
// 63 = +0xFC = 252 decimal) with (gain, 1, 1); then timer = timer -
// (attribute_amount_held - pre_amount) (the fld/fsub/fsubr/fstp chain
// - the .c's exact render); return 0.
unsigned __int8 __fastcall RGE_Action_Make__update_new(
    RGE_Action_Make* this_, DWORD dummy)
{
    if (this_->state == RGE_ACTION_STATE_DONE)                    // asm 0x40708B-0x40808C (dec eax; jz)
        return 1;                                                 // asm loc_407154 (mov al, 1)
    if (this_->state != RGE_ACTION_STATE_WORK)                    // asm 0x407092-0x407095 (sub eax, 5; jnz -> loc_40714D)
        return 0;                                                 // asm loc_40714D (xor al, al)
    if (this_->timer > 0.0 || X87_LE_NAN(this_->work_timer, 0.0)) // asm 0x40709B-0x4070B9 (the timer gate: fcomp const_d_zero; test ah,41h/jz - the ordered > -> the work = the literal MATCH row; the work_timer gate: test ah,41h/jnz - the {<=,NaN} set -> the work = the MACRO REQUIRED)
    {
        // THE WORK BLOCK (loc_4070DD)
        float pre_amount = this_->obj->attribute_amount_held;     // asm 0x407102-0x407105 (mov edi, [eax+54h]; the snapshot -> var_4)
        float gain = (float)((double)this_->obj->owner->world->world_time_delta_seconds   // asm 0x4070F6 (fld [edi+0A8h] = 168)
            * (double)this_->obj->master_obj->gbg_work_rate_2     // asm 0x4070FC (fmul [ebx+114h] = 276 - the master Action extension)
            * (double)this_->task->work_val1);                    // asm 0x407109 (fmul [edx+1Ch] = 28 - work_val1; fstp var_8 = the store-round)
        if (this_->task->work_flag)                               // asm 0x4070EB/0x407110 (cmp word [edx+30h], 0; jz - the flags persisting across the x87 block)
            this_->obj->owner->vfptr->add_attribute_num(this_->obj->owner, this_->task->attribute_type, gain, 0);   // asm 0x407120 (slot 32 = +0x80 = 128 decimal; (type, gain, 0))
        else
            this_->obj->vfptr->set_attribute_amount(this_->obj, gain, 1, 1);   // asm 0x407135 (slot 63 = +0xFC = 252 decimal; (gain, 1, 1))
        this_->timer = (float)((double)this_->timer - ((double)this_->obj->attribute_amount_held - (double)pre_amount));   // asm 0x40713B-0x40714A (fld the new amount; fsub pre; fsubr timer; fstp timer)
        return 0;                                                 // asm loc_40714D
    }
    // THE EMPTY-CARRY CHECK (the both-ordered-> else, loc_4070BB)
    if (this_->obj->attribute_amount_held > 0.0)                  // asm 0x4070BB-0x4070CC (fld @+0x54; fcomp const_d_zero; test ah,41h/jz - the ordered > -> return 0 = the literal MATCH row)
        return 0;                                                 // asm loc_40714D
    this_->obj->vfptr->die_die_die(this_->obj);                   // asm 0x4070D0 (slot 57 = +0xE4 = 228 decimal - the die_die_die dispatch, the missile_move slot precedent)
    return 0;                                                     // asm 0x4070D6
}

void install_RGE_Action_Make()
{
    setHook((void*)0x00406F60, RGE_Action_Make__RGE_Action_Make2_new);
    setHook((void*)0x00406FD0, RGE_Action_Make__RGE_Action_Make_new);
    setHook((void*)0x00407000, RGE_Action_Make__first_in_stack_new);
    setHook((void*)0x00407030, RGE_Action_Make__set_state_new);
    setHook((void*)0x00407080, RGE_Action_Make__update_new);
}
