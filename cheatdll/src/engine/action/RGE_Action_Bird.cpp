#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Bird vftable (.rdata:0065464C, lst :986142; slot 0 =
// the SHARED family scalar deleting destructor - the same
// TRIBE_Action_Artifact__scalar_deleting_destructor the Guard/Make
// vftables slot; the tier owns no destructor of its own)
static RGE_Action_BirdVtbl* const rge_action_bird_vftable =
    (RGE_Action_BirdVtbl*)0x0065464C;

// Replacement for RGE_Action_Bird::RGE_Action_Bird2 (the read ctor).
// Original: @ 0x00402B00. Range [0x402B00, 0x402B25) (lst :3188/:3208,
// retn 8 = 2 stack args), RGE_Action_Bird* return. ABI: __thiscall.
// NOT virtual. Callers: RGE_Action_List__create_action2 +9F (the load
// path). NO SEH (plain). Decode (asm 0x402B00-0x402B24):
// RGE_Action::RGE_Action2(this, infile, obj_in, 1) (the idb constant
// HOOKED -> routes), the vfptr store (the Bird static vftable
// .rdata:0065464C), action_type = 10 (the BIRD registry value);
// return this.
RGE_Action_Bird* __fastcall RGE_Action_Bird__RGE_Action_Bird2_new(
    RGE_Action_Bird* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x402B11 (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_bird_vftable;                           // asm 0x402B16
    this_->action_type = RGE_ACTION_TYPE_BIRD;                        // asm 0x402B1C (mov word @+4, 0Ah)
    return this_;                                                     // asm 0x402B22 (mov eax, esi)
}

// Replacement for RGE_Action_Bird::RGE_Action_Bird (the ctor). Original:
// @ 0x00402B30. Range [0x402B30, 0x402B6A) (lst :3217/:3245, retn 14h =
// 5 stack args), RGE_Action_Bird* return. ABI: __thiscall. NOT virtual.
// Caller: RGE_Action_List__create_task_action +72 (the OWNED anchor -
// routes). NO SEH (plain). Decode (asm 0x402B30-0x402B69): RGE_Action::
// RGE_Action(this, obj_in, 1) (:172 - HOOKED -> routes); task @+0x30,
// target_y @+0x24, the vfptr, action_type, target_x @+0x20, target_z
// @+0x28 (the compiler's out-of-source-order schedule reconciled);
// return this.
RGE_Action_Bird* __fastcall RGE_Action_Bird__RGE_Action_Bird_new(
    RGE_Action_Bird* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in, float x, float y, float z)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x402B3A (:172 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->task = task_in;                                        // asm 0x402B4B ([esi+30h])
    this_->target_y = y;                                          // asm 0x402B52 ([esi+24h])
    this_->vfptr = rge_action_bird_vftable;                       // asm 0x402B55
    this_->action_type = RGE_ACTION_TYPE_BIRD;                    // asm 0x402B5B (mov word @+4, 0Ah)
    this_->target_x = x;                                          // asm 0x402B61 ([esi+20h])
    this_->target_z = z;                                          // asm 0x402B64 ([esi+28h])
    return this_;                                                 // asm 0x402B67 (mov eax, esi)
}

// Replacement for RGE_Action_Bird::first_in_stack. Original:
// @ 0x00402B70. Range [0x402B70, 0x402BAC) (lst :3254/:3288, retn 4 = 1
// stack arg), void. ABI: __thiscall. Virtual: the Bird vftable (the
// DATA XREF lst :3255). NO SEH. Decode (asm 0x402B70-0x402BAB): IF
// first_time: THE ALTITUDE TELEPORT - obj->vfptr->teleport (slot 42 =
// +0xA8 = 168 decimal; the missile_move precedent) with (obj->world_x
// @+0x48, obj->world_y @+0x4C, 9.0f - the imm push 41100000h = float
// 9.0, the bird altitude) - the DOUBLE return DISCARDED (the original's
// own fstp st @0x402B99 = the x87 hygiene; the written statement drops
// it). set_state(3 Search) UNCONDITIONALLY on every path (slot 24 =
// +0x60).
void __fastcall RGE_Action_Bird__first_in_stack_new(
    RGE_Action_Bird* this_, DWORD dummy, unsigned __int8 first_time)
{
    if (first_time)                                               // asm 0x402B79
    {
        this_->obj->vfptr->teleport(this_->obj, this_->obj->world_x, this_->obj->world_y, 9.0f);   // asm 0x402B7B-0x402B91 (slot 42 = +0xA8 = 168; push 9.0/y/x = the (x, y, z=9.0f) identity order; the double return discarded = the original's fstp st)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);  // asm 0x402B9B (3 = Search)
        return;                                                   // asm 0x402B9F
    }
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);      // asm 0x402BA8 (3 = Search)
}

// Replacement for RGE_Action_Bird::set_state. Original: @ 0x00402BB0.
// Range [0x402BB0, 0x402C7B) (lst :3297/:3386, retn 4 = 1 stack arg),
// void. ABI: __thiscall. Virtual: the Bird vftable (the DATA XREF lst
// :3297). NO SEH. THE ARM FORM: the asm uses the sub/jz CHAIN (sub eax,4
// -> Goto; sub eax,2 -> Work; else) - NO jump table exists in the asm
// range (the table-byte check: nothing to diff). Decode (asm
// 0x402BB0-0x402C7A): state = new_state; Goto(4): timer =
// (float)(10 * rand / RAND_MAX + 5) - THE MAGIC-IMUL DIVIDE (80010003h /
// sar 0Eh = /RAND_MAX_INTERNAL 32767, the do_attack precedent; the whole
// chain in INTS then the single fild/fstp float store);
// task->move_sprite @+0x34 non-null -> new_sprite (slot 44 = +0xB0) ->
// return; Work(6): the same with task->work_sprite @+0x38; ANY OTHER
// STATE: delete_list(sub_actions) (:62) -> return; the SPRITE-LESS
// FALLBACK (both sprite-null arms join): get_sprite(master, obj) (slot
// 1 = +0x4) -> new_sprite (slot 44). THE RAND RULE: rand_internal() +
// RAND_MAX_INTERNAL (consts.h) - never the bare rand/32767.
void __fastcall RGE_Action_Bird__set_state_new(
    RGE_Action_Bird* this_, DWORD dummy, unsigned __int8 new_state)
{
    this_->state = new_state;                                     // asm 0x402BB8 ([esi+0Ch])
    if (new_state == RGE_ACTION_STATE_GOTO)                       // asm 0x402BC0-0x402BC3 (sub eax, 4; jz -> loc_402C1C)
    {
        this_->timer = (float)(10 * rand_internal() / RAND_MAX_INTERNAL + 5);   // asm 0x402C1C-0x402C46 (call _rand; the magic-imul 80010003h/sar 0Eh; lea +5; fild/fstp timer @+0x2C - the int chain, the single float store)
        if (this_->task->move_sprite)                             // asm 0x402C49 ([edx+34h])
        {
            this_->obj->vfptr->new_sprite(this_->obj, this_->task->move_sprite);   // asm 0x402C56 (slot 44 = +0xB0 = 176 decimal)
            return;                                               // asm 0x402C19
        }
    }
    else if (new_state == RGE_ACTION_STATE_WORK)                  // asm 0x402BC5-0x402BC8 (sub eax, 2; jz -> loc_402BD7)
    {
        this_->timer = (float)(10 * rand_internal() / RAND_MAX_INTERNAL + 5);   // asm 0x402BD7-0x402BFD
        if (this_->task->work_sprite)                             // asm 0x402C04 ([edx+38h])
        {
            this_->obj->vfptr->new_sprite(this_->obj, this_->task->work_sprite);   // asm 0x402C11
            return;                                               // asm 0x402C5E
        }
    }
    else
    {
        RGE_Action_List__delete_list(this_->sub_actions);         // asm 0x402BCD (:62 - the idb constant)
        return;                                                   // asm 0x402BD4
    }
    // THE SPRITE-LESS FALLBACK (both sprite-null arms join here)
    RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);   // asm 0x402C6C (slot 1 = +0x4)
    this_->obj->vfptr->new_sprite(this_->obj, base_sprite);       // asm 0x402C73 (slot 44)
}

// Replacement for RGE_Action_Bird::update. Original: @ 0x00402C80.
// Range [0x402C80, 0x402EBA) = 563 bytes (lst :3395/:3646, retn = no
// stack args), unsigned __int8 return. ABI: __thiscall. Virtual: the
// Bird vftable slot (DATA XREF lst :3395). THE SEH: the allocation-
// unwind fold (the chunk @0x6406E0 = mov eax,[ebp-10h]/push/call
// operator_delete - the raw alloc freed on the unwind; written as the
// scoped __except per the set_state_move precedent). THE SWITCH-TABLE
// BYTE CHECK: jpt_402CD0 = the 6-case table (state-1..6): Done(1) ->
// return 1; Search(3) -> THE RANDOM WALK; Goto(4) -> the fly tick;
// Work(6) -> the work tick; the rest -> default. Decode (asm
// 0x402C80-0x402EB9): THE TIMER TICK (the conditional form - NOT the
// Enter's unconditional tick): timer > 0.0 ordered (fcomp
// const_d_zero; test ah,41h/jnz = the {<=,NaN} skip - the literal >
// MATCH row) -> timer = timer - world_time_delta_seconds (the fstp
// store-round, rule (d)/(e) - the stored-only form). Done -> return 1.
// Search (loc_402D64) - THE RANDOM WALK: map = obj->owner->world->map;
// target_x = (float)(map_width @+8 * rand_internal() /
// RAND_MAX_INTERNAL) (the int imul/magic-imul chain then the single
// fild/fstp store); target_y = the same with map_height @+0xC;
// target_z = obj->world_z; THE CLAMPS (the asm skip-chain): X87_LE_NAN
// (target_x, 0.0f) -> target_x = 1.0f (test ah,41h/jz+ = the {<=,NaN}
// clamp set - the macro kept mechanical; the inputs provably non-NaN -
// the fild'd int quotients); X87_LT_NAN((double)(map_width - 1),
// target_x) -> target_x = (float)(map_width - 1) (test ah,1 - the
// {<,NaN} set: the fild'd (width-1) vs the stored x; the fall-through
// clamps); the same pair for y with map_height. THE MOVE_TO CREATION:
// operator_new(0x44) = sizeof(RGE_Action_Move_To) walked (the Action
// mirror + range @64); the ctor (:100 - HOOKED -> routes) with (obj,
// target_x, target_y, target_z, task->work_range @+0x24, obj->sprite
// @+0x1C); the allocation-unwind fold (the chunk 0x6406E0); null ->
// set_state(3 Search) -> return 0 (loc_402D49); else add_action (:67)
// -> set_state(4 Goto) -> return 0. Goto (loc_402CD7): timer <= 0.0
// ordered (test ah,41h/jz - the literal <= MATCH row: the {<=,NaN} set
// fires) -> set_state(6 Work); the sub-action update (the list vtbl
// slot 4 = +0x10): result Done(1) or Search(3) -> set_state(3 Search);
// return 0. Work (loc_402D20): timer <= 0.0 -> set_state(4 Goto); the
// sub-action update == Done(1) ONLY -> set_state(3 Search); return 0.
// default -> return 0.
unsigned __int8 __fastcall RGE_Action_Bird__update_new(
    RGE_Action_Bird* this_, DWORD dummy)
{
    if (this_->timer > 0.0)                                       // asm 0x402C99-0x402CA7 (fld timer @+0x2C; fcomp const_d_zero; test ah,41h/jnz - the {<=,NaN} skip = the literal MATCH row)
        this_->timer = (float)((double)this_->timer - (double)this_->obj->owner->world->world_time_delta_seconds);   // asm 0x402CA9-0x402CBE (fld/fsub/fstp - the stored-only form)
    switch (this_->state)                                         // asm 0x402CC1-0x402CD0 (jpt_402CD0 - THE EMITTED TABLE MUST EQUAL THE 6-CASE PARTITION)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_402EA9
        return 1;                                                 // asm 0x402EAD (mov al, 1)
    case RGE_ACTION_STATE_SEARCH:                                 // 3 -> loc_402D64 - THE RANDOM WALK
    {
        RGE_Map* map = this_->obj->owner->world->map;             // asm 0x402D68-0x402D71 (obj->owner->world @+0x8C -> map @+0x34; TRIBE_World::map = RGE_Map* directly, idb_types.h:959)
        this_->target_x = (float)(map->map_width * rand_internal() / RAND_MAX_INTERNAL);   // asm 0x402D74-0x402D9A (call _rand; imul map_width @+8; the magic-imul 80010003h/sar 0Eh; fild/fstp target_x @+0x20)
        this_->target_y = (float)(map->map_height * rand_internal() / RAND_MAX_INTERNAL);  // asm 0x402D9D-0x402DC4 (map_height @+0xC; the same chain; fstp target_y @+0x24)
        this_->target_z = this_->obj->world_z;                    // asm 0x402DD7-0x402DDA ([edx+50h] -> [esi+28h])
        if (X87_LE_NAN(this_->target_x, 0.0f))                    // asm 0x402DCE-0x402DE2 (fld/fcomp const_f_zero; test ah,41h/jz+ - the {<=,NaN} clamp set)
            this_->target_x = 1.0f;                               // asm 0x402DE4 (mov ecx, 1.0 staged at 0x402DBB)
        if (X87_LT_NAN((double)(map->map_width - 1), this_->target_x))   // asm 0x402DE7-0x402DFB (fild (width-1); fcom x; test ah,1/jz = the discard - the {<,NaN} fall clamps)
            this_->target_x = (float)(map->map_width - 1);        // asm 0x402DFD (fstp [esi+20h])
        if (X87_LE_NAN(this_->target_y, 0.0f))                    // asm 0x402E04-0x402E12
            this_->target_y = 1.0f;                               // asm 0x402E14
        if (X87_LT_NAN((double)(map->map_height - 1), this_->target_y))  // asm 0x402E17-0x402E29
            this_->target_y = (float)(map->map_height - 1);       // asm 0x402E2E (fstp [esi+24h])
        RGE_Action_Move_To* created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x402E35-0x402E3C (push 44h = 68 walked)
        __try
        {
            if (created)                                          // asm 0x402E45
                RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj, this_->target_x, this_->target_y, this_->target_z, this_->task->work_range, this_->obj->sprite);   // asm 0x402E6C (:100 - the idb constant, HOOKED -> routes; the args x/y/z/range/sprite - sprite @+0x1C)
        }
        __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH)   // the allocation-unwind fold: the chunk @0x6406E0 (mov eax,[ebp-10h]/push/call operator_delete)
        {
        }
        if (!created)                                             // asm 0x402E75-0x402E7F
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm loc_402D49 (3 = Search)
            return 0;                                             // asm 0x402D52
        }
        RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm 0x402E89 (:67 - the idb constant)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);    // asm 0x402E94 (4 = Goto)
        return 0;                                                 // asm 0x402E97
    }
    case RGE_ACTION_STATE_GOTO:                                   // 4 -> loc_402CD7
    {
        if (X87_LE_NAN(this_->timer, 0.0))                        // asm 0x402CD7-0x402CE5 (fld/fcomp const_d_zero; test ah,41h/jz - the {<=,NaN} set FALLS THROUGH to the set_state: the NaN timer FIRES in the original - the macro required; the literal <= would skip = DIVERGENT)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);// asm 0x402CED (6 = Work)
        unsigned __int8 sub = this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x402CF5 (the list vtbl slot 4 = +0x10; and eax,0FFh - the declared u8 return, the mask folds)
        if (sub == RGE_ACTION_STATE_DONE || sub == RGE_ACTION_STATE_SEARCH)   // asm 0x402CF8-0x402D03 (dec eax; jz / sub eax,2; jnz)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm loc_402D05 (3 = Search)
        return 0;                                                 // asm 0x402D0E
    }
    case RGE_ACTION_STATE_WORK:                                   // 6 -> loc_402D20
    {
        if (X87_LE_NAN(this_->timer, 0.0))                        // asm 0x402D20-0x402D2E (the same form - the NaN timer FIRES)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);// asm 0x402D36 (4 = Goto)
        unsigned __int8 sub = this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x402D3E (slot 4)
        if (sub == RGE_ACTION_STATE_DONE)                         // asm 0x402D41-0x402D47 (dec eax; jnz default - ONLY Done(1))
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm loc_402D49 (3 = Search)
        return 0;                                                 // asm 0x402D52 / def_402CD0
    }
    default:
        return 0;                                                 // asm def_402CD0 (xor al, al)
    }
}

void install_RGE_Action_Bird()
{
    setHook((void*)0x00402B00, RGE_Action_Bird__RGE_Action_Bird2_new);
    setHook((void*)0x00402B30, RGE_Action_Bird__RGE_Action_Bird_new);
    setHook((void*)0x00402B70, RGE_Action_Bird__first_in_stack_new);
    setHook((void*)0x00402BB0, RGE_Action_Bird__set_state_new);
    setHook((void*)0x00402C80, RGE_Action_Bird__update_new);
}
