#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Attack vftable (.rdata:00654540, lst :986095). THE STRUCT:
// the base Action mirror (64 = 0x40) + the extension: move_sprite @+0x40
// (64), fight_sprite @+0x44 (68), wait_sprite @+0x48 (72), range @+0x4C
// (76), min_range @+0x50 (80), missile_id @+0x54 (84), fire_missile_at_frame
// @+0x56 (86), need_to_attack @+0x58 (88), was_same_owner @+0x59 (89),
// indirect_fire_flag @+0x5A (90), lastTargetPositionX/Y/Z @+0x5C/60/64
// (92/96/100); sizeof = 104 = 0x68 (idb_types.h:20925-20958; the offsets
// re-pin per asm at each decode). THIS FILE = the COMPLETE Attack write
// through batch AT2 (11 of 14): AT1 (destructor/vdel/stop/set_target_obj/
// first_in_stack/copy_obj_sprites) + AT2 (Attack3/ctor/Attack2/save/work).
// AT3 (set_state/ready_to_attack/update) follows.
static RGE_Action_AttackVtbl* const rge_action_attack_vftable =
    (RGE_Action_AttackVtbl*)0x00654540;

// ===========================================================================
// Replacement for RGE_Action_Attack::destructor. Original: @ 0x00401410
// (idb_functions.h:9). Range [0x401410,0x401416) (lst :507/:515), void.
// ABI: __thiscall (this-only - the dummy-less __fastcall is ABI-identical
// per the 2026-11-21 ruling). VIRTUAL (the Attack vftable; the vdel's
// callee). SEMANTIC: the tier destructor - the Attack vfptr restore, then
// the base Action destructor (the tier owns no heap of its own - no
// extension frees).
void __fastcall RGE_Action_Attack__destructor_new(RGE_Action_Attack* this_)
{
    this_->vfptr = rge_action_attack_vftable;                       // asm 0x401410 (mov dword ptr [ecx], offset vftable)
    RGE_Action__destructor((RGE_Action*)this_);                     // asm 0x401416 (jmp RGE_Action__destructor - the idb constant :173 @0x40A8E0, HOOKED -> routes; the call-in-C vs the tail-jmp = the behaviorally-identical wrapper class, the A1 precedent; the derived->base cast)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::RGE_Action_Attack3 (the read ctor).
// Original: @ 0x00401000 (idb_functions.h:5). Range [0x401000,0x401124)
// (lst :39/:146, retn 8 = 2 stack args), RGE_Action_Attack* return. ABI:
// __thiscall. NOT virtual. Caller: RGE_Action_List__create_action2+5F (the
// load path). The SEH: the trivial unwind (the chunk @0x640630 - the
// action-ctor house form, documented not written).
// SEMANTIC: the stream loader - the base ctor_read, then the Attack
// extension in the stream order: range, min_range, missile_id,
// fire_missile_at_frame, need_to_attack, was_same_owner,
// indirect_fire_flag, the three sprite IDs resolved through
// get_sprite_pointer on the unit, the lastTarget position trio.
RGE_Action_Attack* __fastcall RGE_Action_Attack__RGE_Action_Attack3_new(
    RGE_Action_Attack* this_, DWORD dummy, int infile, RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);    // asm 0x40102A (the idb constant - HOOKED -> routes; do_setup = 1; the derived->base cast)
    rge_read(infile, &this_->range, sizeof(this_->range));             // asm 0x40102F (@+0x4C, 4)
    this_->vfptr = rge_action_attack_vftable;                          // asm 0x40103E
    this_->action_type = RGE_ACTION_TYPE_ATTACK2;                       // asm 0x401044 (mov word @+4, 9)
    rge_read(infile, &this_->min_range, sizeof(this_->min_range));     // asm 0x40104F (@+0x50, 4)
    rge_read(infile, &this_->missile_id, sizeof(this_->missile_id));   // asm 0x40105B (@+0x54, 2)
    rge_read(infile, &this_->fire_missile_at_frame, sizeof(this_->fire_missile_at_frame));  // asm 0x401067 (@+0x56, 2)
    rge_read(infile, &this_->need_to_attack, sizeof(this_->need_to_attack));  // asm 0x401073 (@+0x58, 1)
    rge_read(infile, &this_->was_same_owner, sizeof(this_->was_same_owner));  // asm 0x40107F (@+0x59, 1)
    rge_read(infile, &this_->indirect_fire_flag, sizeof(this_->indirect_fire_flag));  // asm 0x40108B (@+0x5A, 1)
    __int16 move_id;                                                   // asm 0x401097-0x40109F (the 2-byte scratch - the arg-home reuse in the asm)
    rge_read(infile, &move_id, sizeof(move_id));                       // asm (2)
    this_->move_sprite = RGE_Static_Object__get_sprite_pointer((RGE_Static_Object*)this_->obj, move_id);   // asm 0x4010A4-0x4010B1 (:3829 @0x54F520; the ancestor cast)
    __int16 fight_id;
    rge_read(infile, &fight_id, sizeof(fight_id));                     // asm 0x4010B1-0x4010B9 (2)
    this_->fight_sprite = RGE_Static_Object__get_sprite_pointer((RGE_Static_Object*)this_->obj, fight_id);  // asm 0x4010C1-0x4010CE
    __int16 wait_id;
    rge_read(infile, &wait_id, sizeof(wait_id));                       // asm 0x4010CE-0x4010D6 (2)
    this_->wait_sprite = RGE_Static_Object__get_sprite_pointer((RGE_Static_Object*)this_->obj, wait_id);   // asm 0x4010DE-0x4010EB
    rge_read(infile, &this_->lastTargetPositionX, sizeof(this_->lastTargetPositionX));  // asm 0x4010EB (@+0x5C, 4)
    rge_read(infile, &this_->lastTargetPositionY, sizeof(this_->lastTargetPositionY));  // asm 0x4010FA (@+0x60, 4)
    rge_read(infile, &this_->lastTargetPositionZ, sizeof(this_->lastTargetPositionZ));  // asm 0x401106 (@+0x64, 4)
    return this_;                                                      // asm 0x401116-0x401124 (mov eax, esi)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::RGE_Action_Attack (the target ctor).
// Original: @ 0x00401150 (idb_functions.h:7). Range [0x401150,0x4012DA)
// (lst :178/:365, retn 24h = 9 stack args), RGE_Action_Attack* return.
// ABI: __thiscall. NOT virtual. Callers: RGE_Action_List__create_task_
// action+118 (the OWNED anchor - routes) + RGE_Combat_Object__attack2+190
// (the OWNED attack2_new - routes). The SEH: the trivial unwind (the
// chunk @0x640650).
// SEMANTIC: the target-form attack construction - the base ctor, the
// vfptr/type, the Building/linked_owner redirect when the master's
// area_effect_range is positive, the changed-target swing-counter clear,
// the base set_target_obj, the lastTarget trio from the target's world
// position (or the -1.0 sentinels), the field block, the owner-pair
// was_same_owner, the sprite fallbacks (the move_sprite fallback =
// get_run_sprite TWICE when non-null - the original's own redundant
// fetch, the call multiset load-bearing), the range zero fallback to the
// master weapon_range.
RGE_Action_Attack* __fastcall RGE_Action_Attack__RGE_Action_Attack_new(
    RGE_Action_Attack* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Static_Object* target_obj_in, RGE_Sprite* move_sprite_in,
    RGE_Sprite* fight_sprite_in, RGE_Sprite* wait_sprite_in,
    float range_in, float min_range_in, __int16 missile_id_in,
    __int16 fire_missile_at_frame_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);             // asm 0x401177 (the idb constant :172 - HOOKED -> routes)
    this_->vfptr = rge_action_attack_vftable;                          // asm 0x40117F
    this_->action_type = RGE_ACTION_TYPE_ATTACK2;                      // asm 0x401185 (mov word @+4, 9 - 9 = the ATTACK2 registry slot, NOT the task-side 7)
    RGE_Static_Object* workTarget = target_obj_in;                     // asm 0x40118B (mov edi)
    double areaEffect = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->area_effect_range;  // asm 0x401198-0x4011A6 (fld [eax+148h] = 328; the master downcast - the Combat-master factory identity, the unguarded asm)
    if (areaEffect > 0.0f                                              // asm 0x4011A6-0x4011A9 (fcomp const_f_zero; test ah,41h; jnz skip - the ordered > 0 falls = the literal > MATCH row)
        && target_obj_in->master_obj->master_type == RGE_MASTER_TYPE_BUILDING   // asm 0x4011AB-0x4011B2 (cmp byte ptr [edx+4], 50h)
        && ((TRIBE_Building_Object*)target_obj_in)->linked_owner) {    // asm 0x4011B4-0x4011BC (mov eax, [edi+1DCh]; test - the work_new/first_in_stack redirect precedent)
        workTarget = (RGE_Static_Object*)((TRIBE_Building_Object*)target_obj_in)->linked_owner;  // asm 0x4011BE (mov edi, eax)
    }
    if (workTarget != this_->target_obj)                               // asm 0x4011C0-0x4011C3 (cmp edi, [esi+10h])
        ((RGE_Combat_Object*)this_->obj)->attack_count = 0;            // asm 0x4011C5 (mov dword ptr [ecx+170h], ebx(0) - the ecx = obj staged at 0x40117C; the AT1 factory-identity downcast)
    RGE_Action__set_target_obj((RGE_Action*)this_, workTarget);        // asm 0x4011CB-0x4011CE (push edi; the idb constant :182 - HOOKED -> routes)
    if (workTarget) {                                                  // asm 0x4011D3-0x4011DA (cmp edi, ebx; the -1.0 staged in eax)
        this_->lastTargetPositionX = workTarget->world_x;              // asm 0x4011DC-0x4011DF (mov ecx, [edi+48h] -> [esi+5Ch])
        this_->lastTargetPositionY = workTarget->world_y;              // asm 0x4011E2-0x4011E5 ([edi+4Ch] -> [esi+60h])
        this_->lastTargetPositionZ = workTarget->world_z;              // asm 0x4011E8-0x4011EB ([edi+50h] -> [esi+64h])
    } else {
        this_->lastTargetPositionX = -1.0f;                            // asm 0x4011F0-0x4011F6 (the sentinel trio)
        this_->lastTargetPositionY = -1.0f;
        this_->lastTargetPositionZ = -1.0f;
    }
    this_->fight_sprite = fight_sprite_in;                             // asm 0x4011F9-0x401201 ([esi+44h])
    this_->wait_sprite = wait_sprite_in;                               // asm 0x401208 ([esi+48h])
    this_->target_x = -1.0f;                                           // asm 0x40120F (mov [esi+20h], eax(-1.0) - the sentinel trio)
    this_->target_y = -1.0f;                                           // asm 0x401212 ([esi+24h])
    this_->target_z = -1.0f;                                           // asm 0x401215 ([esi+28h])
    this_->range = range_in;                                           // asm 0x40121C ([esi+4Ch])
    this_->min_range = min_range_in;                                   // asm 0x401224 ([esi+50h])
    this_->move_sprite = move_sprite_in;                               // asm 0x40122E ([esi+40h])
    this_->missile_id = missile_id_in;                                 // asm 0x401231 ([esi+54h])
    this_->fire_missile_at_frame = fire_missile_at_frame_in;           // asm 0x401235 ([esi+56h])
    this_->need_to_attack = 0;                                         // asm 0x401239 ([esi+58h], bl = 0)
    this_->indirect_fire_flag = 0;                                     // asm 0x40123C ([esi+5Ah], bl = 0) - RESTORED: the dropped store left ind as heap garbage (the WORK_2 !indirect branch class)
    if (workTarget && workTarget->owner == obj_in->owner)              // asm 0x40123F-0x401249 (jz loc_401251; mov edx, [edi+18h]; cmp edx, [ebp+18h])
        this_->was_same_owner = 1;                                     // asm 0x40124B (mov byte ptr [esi+59h], 1)
    else
        this_->was_same_owner = 0;                                     // asm 0x401251 ([esi+59h], bl)
    if (!move_sprite_in) {                                             // asm 0x401254-0x401256 (cmp eax, ebx - eax = move_sprite_in held from 0x401218)
        RGE_Sprite* run = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_run_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x401258-0x401261 (call [edx+7Ch] - slot 31 = +0x7C = get_run_sprite, the M9 grounding)
        if (run)                                                       // asm 0x401264 (test eax, eax)
            run = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_run_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x401269-0x401271 (call [edx+7Ch] AGAIN - THE DOUBLE CALL, the original's own redundant fetch)
        else
            run = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm loc_401276 (call [edx+78h] - slot 30)
        this_->move_sprite = run;                                      // asm loc_401279-0x40127C (mov [esi+40h], eax)
    }
    if (!this_->fight_sprite)                                          // asm 0x40127C-0x40127F (cmp [esi+44h], ebx)
        this_->fight_sprite = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x401281-0x401290 (call [edx+84h] - slot 33)
    if (!this_->wait_sprite)                                           // asm 0x401293-0x401296 (cmp [esi+48h], ebx)
        this_->wait_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x401298-0x4012A1 (call [edx+4] - slot 1)
    if (X87_EQ_NAN(this_->range, 0.0f))                                // asm 0x4012A7-0x4012B5 (fld [esi+4Ch]; fcomp const_f_zero; test ah, 40h; jz skip - the C3-only {== 0, NaN} set -> the replacement; the .c's literal == would skip NaN)
        this_->range = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->weapon_range;  // asm 0x4012B7-0x4012C3 (mov edx, [ecx+144h] = 324 - the Combat master's weapon_range)
    return this_;                                                      // asm 0x4012C6-0x4012DA (mov eax, esi)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::RGE_Action_Attack2 (the coordinate
// ctor). Original: @ 0x004012E0 (idb_functions.h:8). Range
// [0x4012E0,0x401408) (lst :370/:503, retn 2Ch = 11 stack args),
// RGE_Action_Attack* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Action_List__create_task_action+195 (the OWNED anchor - routes) +
// RGE_Combat_Object__attack+117 (the OWNED attack_new - routes). The SEH:
// the trivial unwind (the chunk @0x640670).
// SEMANTIC: the coordinate-form attack construction - no target object:
// target_x/y = x/y, target_z = z, the lastTarget trio = the -1.0
// sentinels, indirect_fire_flag = 2 (vs the target ctor's 0 - the
// registry candidate), the same sprite fallbacks and the range zero
// fallback.
RGE_Action_Attack* __fastcall RGE_Action_Attack__RGE_Action_Attack2_new(
    RGE_Action_Attack* this_, DWORD dummy, RGE_Action_Object* obj_in,
    float x, float y, float z, RGE_Sprite* move_sprite_in,
    RGE_Sprite* fight_sprite_in, RGE_Sprite* wait_sprite_in,
    float range_in, float min_range_in, __int16 missile_id_in,
    __int16 fire_missile_at_frame_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);             // asm 0x401304 (the idb constant :172 - HOOKED -> routes)
    this_->target_x = x;                                               // asm 0x401309-0x401316 ([esi+20h])
    this_->target_y = y;                                               // asm 0x40130D-0x40131D ([esi+24h])
    RGE_Sprite* fight = fight_sprite_in;                               // asm 0x401319 (mov ecx)
    RGE_Sprite* wait = wait_sprite_in;                                 // asm 0x401320 (mov edx)
    this_->lastTargetPositionX = -1.0f;                                // asm 0x401324 (the sentinel trio)
    this_->lastTargetPositionY = -1.0f;                                // asm 0x401327
    this_->lastTargetPositionZ = -1.0f;                                // asm 0x40132A
    this_->target_z = z;                                               // asm 0x40133F ([esi+28h])
    this_->fight_sprite = fight;                                       // asm 0x401331 ([esi+44h])
    this_->wait_sprite = wait;                                         // asm 0x401338 ([esi+48h])
    this_->range = range_in;                                           // asm 0x401346 ([esi+4Ch])
    this_->min_range = min_range_in;                                   // asm 0x40134E ([esi+50h])
    this_->vfptr = rge_action_attack_vftable;                          // asm 0x401360
    this_->action_type = RGE_ACTION_TYPE_ATTACK2;                       // asm 0x401366 (mov word @+4, 9)
    this_->move_sprite = move_sprite_in;                               // asm 0x40136C ([esi+40h])
    this_->missile_id = missile_id_in;                                 // asm 0x40136F ([esi+54h])
    this_->fire_missile_at_frame = fire_missile_at_frame_in;           // asm 0x401373 ([esi+56h])
    this_->need_to_attack = 0;                                         // asm 0x401377
    this_->was_same_owner = 0;                                         // asm 0x40137B
    this_->indirect_fire_flag = 2;                                     // asm 0x40137F (mov byte ptr [esi+5Ah], 2 - the coordinate form = indirect; the registry candidate)
    if (!move_sprite_in) {                                             // asm 0x40135E-0x401383 (test eax, eax; jnz skip - the flags persisting across the store block, the asm order)
        RGE_Sprite* run = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_run_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x401385-0x40138E (call [edx+7Ch] - slot 31)
        if (run)                                                       // asm 0x401391 (test eax, eax)
            run = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_run_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x40139E (call [edx+7Ch] AGAIN - the double call)
        else
            run = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm loc_4013A3 (call [edx+78h])
        this_->move_sprite = run;                                      // asm loc_4013A6 (mov [esi+40h], eax)
    }
    if (!this_->fight_sprite)                                          // asm 0x4013A9-0x4013AE
        this_->fight_sprite = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x4013B0-0x4013BF
    if (!this_->wait_sprite)                                           // asm 0x4013C2-0x4013C7
        this_->wait_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x4013C9-0x4013D5
    if (X87_EQ_NAN(this_->range, 0.0f))                                // asm 0x4013D8-0x4013E6 (the C3-only {== 0, NaN} set)
        this_->range = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->weapon_range;  // asm 0x4013E8-0x4013F4
    return this_;                                                      // asm 0x4013F7-0x401408
}

// ===========================================================================
// Replacement for RGE_Action_Attack::save. Original: @ 0x00401420
// (idb_functions.h:10). Range [0x401420,0x401523) (lst :520/:625, retn 4
// = 1 stack arg), void. ABI: __thiscall. VIRTUAL (the Attack vftable).
// NO SEH. Decode (asm 0x401420-0x401522): the ctor_read's exact mirror -
// the base save, then the extension in the stream order: range (4),
// min_range (4), missile_id (2), fire_missile_at_frame (2),
// need_to_attack (1), was_same_owner (1), indirect_fire_flag (1), the
// three sprite IDs (RGE_Sprite::id @+0x82, or -1 when null, written as
// 2-byte words - the Static-save low-word precedent), the lastTarget
// position trio (4 x3).
void __fastcall RGE_Action_Attack__save_new(
    RGE_Action_Attack* this_, DWORD dummy, int outfile)
{
    RGE_Action__save((RGE_Action*)this_, outfile);                     // asm 0x401429 (the idb constant - HOOKED -> routes)
    rge_write(outfile, &this_->range, sizeof(this_->range));           // asm 0x40142E (@+0x4C, 4)
    rge_write(outfile, &this_->min_range, sizeof(this_->min_range));   // asm 0x40143A (@+0x50, 4)
    rge_write(outfile, &this_->missile_id, sizeof(this_->missile_id)); // asm 0x401446 (@+0x54, 2)
    rge_write(outfile, &this_->fire_missile_at_frame, sizeof(this_->fire_missile_at_frame));  // asm 0x401452 (@+0x56, 2)
    rge_write(outfile, &this_->need_to_attack, sizeof(this_->need_to_attack));  // asm 0x40145E (@+0x58, 1)
    rge_write(outfile, &this_->was_same_owner, sizeof(this_->was_same_owner));  // asm 0x40146A (@+0x59, 1)
    rge_write(outfile, &this_->indirect_fire_flag, sizeof(this_->indirect_fire_flag));  // asm 0x401476 (@+0x5A, 1)
    __int16 move_id = this_->move_sprite ? this_->move_sprite->id : -1;  // asm 0x401482-0x401496 (mov ax, [eax+82h] = RGE_Sprite::id; the null -> -1)
    rge_write(outfile, &move_id, sizeof(move_id));                     // asm 0x40149E-0x4014A6 (2)
    __int16 fight_id = this_->fight_sprite ? this_->fight_sprite->id : -1;  // asm 0x4014AB-0x4014BF
    rge_write(outfile, &fight_id, sizeof(fight_id));                   // asm 0x4014C7-0x4014CF (2)
    __int16 wait_id = this_->wait_sprite ? this_->wait_sprite->id : -1;  // asm 0x4014D4-0x4014E8
    rge_write(outfile, &wait_id, sizeof(wait_id));                     // asm 0x4014F0-0x4014F8 (2)
    rge_write(outfile, &this_->lastTargetPositionX, sizeof(this_->lastTargetPositionX));  // asm 0x4014FD (@+0x5C, 4)
    rge_write(outfile, &this_->lastTargetPositionY, sizeof(this_->lastTargetPositionY));  // asm 0x401509 (@+0x60, 4)
    rge_write(outfile, &this_->lastTargetPositionZ, sizeof(this_->lastTargetPositionZ));  // asm 0x401515 (@+0x64, 4)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::work. Original: @ 0x00402290
// (idb_functions.h:15). Range [0x402290,0x4023A1) (lst :2257/:2389, retn
// 10h = 4 stack args), int return. ABI: __thiscall. VIRTUAL (the Attack
// vftable slot 13 = +0x34). NO SEH.
// SEMANTIC: the re-target intake - the same master area_effect_range /
// Building / linked_owner redirect as the target ctor; the same-target
// early accept in {Goto, Goto2, Move, Work, Work2}; the speed-and-move
// gate (the maximumSpeed zero/NaN skip, canMoveToAttack on the
// WORKTARGET's id) commits directly; otherwise the range probe: the
// master weapon_range <= 0.1f widens the range by 0.5, and an ordered
// dist > range refuses (return 0), <= or NaN commits. THE x/y/z ARGS ARE
// NEVER CONSUMED (the original leaves them unread - noted, kept for the
// ABI).
int __fastcall RGE_Action_Attack__work_new(RGE_Action_Attack* this_, DWORD dummy,
    RGE_Static_Object* obj_in, float x, float y, float z)
{
    if (!obj_in)                                                       // asm 0x402299-0x40229B (test edi, edi; jz loc_40239C)
        return 0;                                                      // asm loc_40239C (xor eax, eax)
    RGE_Static_Object* workTarget = obj_in;                            // asm 0x402293 (mov edi)
    double areaEffect = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->area_effect_range;  // asm 0x4022A1-0x4022B3 (fld [eax+148h] = 328; the ctor's twin gate)
    if (areaEffect > 0.0f                                              // asm 0x4022B5-0x4022B8 (test ah, 41h; jnz skip - the ordered > 0 falls, the literal > MATCH row)
        && obj_in->master_obj->master_type == RGE_MASTER_TYPE_BUILDING // asm 0x4022BA-0x4022C1 (cmp byte ptr [edx+4], 50h)
        && ((TRIBE_Building_Object*)obj_in)->linked_owner) {           // asm 0x4022C3-0x4022CB (mov eax, [edi+1DCh]; test)
        workTarget = (RGE_Static_Object*)((TRIBE_Building_Object*)obj_in)->linked_owner;  // asm 0x4022CD (mov edi, eax)
    }
    if (workTarget == this_->target_obj) {                             // asm 0x4022CF-0x4022D2 (cmp edi, [esi+10h]; jnz skip)
        unsigned __int8 st = this_->state;                             // asm 0x4022D4 (mov al, [esi+0Ch])
        if (st == RGE_ACTION_STATE_GOTO || st == RGE_ACTION_STATE_GOTO_2    // asm 0x4022D7-0x4022DD (cmp 4 / cmp 5 - Goto2 joins THIS tier's set)
            || st == RGE_ACTION_STATE_MOVE || st == RGE_ACTION_STATE_WORK   // asm 0x4022DF-0x4022E5 (cmp 0Bh / cmp 6)
            || st == RGE_ACTION_STATE_WORK_2) {                        // asm 0x4022E7-0x4022E9 (cmp 7; the five jz share loc_4022EB - the OR form)
            return 1;                                                  // asm loc_4022EB-0x4022F3 (mov eax, 1)
        }
    }
    double speed = this_->obj->vfptr->maximumSpeed(this_->obj);        // asm 0x4022F6-0x4022FE (call [eax+188h] - slot 98, the M12a double-typed grounding; the local - the X87_EQ_NAN double-evaluation hazard on a vtbl call)
    if (!X87_EQ_NAN(speed, 0.0f)) {                                    // asm 0x4022FE-0x402309 (fcomp const_f_zero; test ah, 40h; jnz loc_402325 - the C3-only {== 0, NaN} skip)
        UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);  // asm 0x40230B-0x40230E (:261)
        if (ai->vfptr->canMoveToAttack(ai, workTarget->id) != 0) {     // asm 0x402313-0x402323 (mov ecx, [edi+10h] - THE WORKTARGET's id, not this->targetID; push; call [edx+0CCh] = slot 51; test)
            // the commit (asm loc_40236A - 2 inbound: canMove nonzero + the in-range fall)
            this_->indirect_fire_flag = 0;                             // asm 0x40236F (mov byte ptr [esi+5Ah], 0)
            this_->vfptr->set_target_obj(this_, workTarget);           // asm 0x40236A-0x402373 (push edi; call [edx+58h] - slot 22)
            this_->lastTargetPositionX = workTarget->world_x;          // asm 0x402376-0x40237B ([edi+48h] -> [esi+5Ch])
            this_->lastTargetPositionY = workTarget->world_y;          // asm 0x40237E-0x402383 ([edi+4Ch] -> [esi+60h])
            this_->lastTargetPositionZ = workTarget->world_z;          // asm 0x402386-0x40238B ([edi+50h] -> [esi+64h])
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);     // asm 0x402379+0x40238E (push 4 staged at 0x402379; call [eax+60h])
            return 1;                                                  // asm 0x402392-0x402399 (mov eax, 1)
        }
    }
    float dist = (float)RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, workTarget);  // asm 0x402325-0x402334 (:3871 the DOUBLE return; fstp [esp+0Ch+var_4] - the m32 store-round, reloaded rounded at 0x40235B - the rule-(e))
    double range = this_->range;                                       // asm 0x40233A-0x40234A (mov edx, [esi+4Ch]; fld [esp+0Ch+obj] - the m32 load, the arg-home reuse)
    double weaponRange = this_->obj->vfptr->weaponRange(this_->obj);   // asm 0x40233E-0x402344 (call [eax+194h] - slot 101 = +0x194, the C1 grounding; the st0 consumed UNROUNDED at the fcomp)
    if (X87_LE_NAN(weaponRange, 0.1f))                                 // asm 0x402344-0x402353 (fcomp ds:flt_6545BC = 0.1f binary-verified 3DCCCCCD; test ah, 41h; jz skip - the {<=, NaN} set takes the fadd)
        range = range + 0.5f;                                          // asm 0x402355 (fadd ds:const_f_half - the st1 stays live across the branch, the x87 stack model)
    if (dist > range)                                                  // asm 0x40235B-0x402368 (fld var_4; fcomp st(1); test ah, 41h; fstp st; jz loc_40239C - the {> ordered} -> return 0, the {<=, NaN} -> the commit - THE LITERAL > MATCH ROW, the .c's v17 > v12 form confirmed against the asm flags)
        return 0;                                                      // asm loc_40239C (xor eax, eax)
    this_->indirect_fire_flag = 0;                                     // asm 0x40236F
    this_->vfptr->set_target_obj(this_, workTarget);                   // asm 0x40236A-0x402373
    this_->lastTargetPositionX = workTarget->world_x;                  // asm 0x402376-0x40237B
    this_->lastTargetPositionY = workTarget->world_y;                  // asm 0x40237E-0x402383
    this_->lastTargetPositionZ = workTarget->world_z;                  // asm 0x402386-0x40238B
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);             // asm 0x40238E (push 4)
    return 1;                                                          // asm 0x402392-0x402399
}

// ===========================================================================
// Replacement for RGE_Action_Attack::set_state. Original: @ 0x00401620
// (idb_functions.h:13). Range [0x401620,0x40193B) (lst :771/:1121, retn 4
// = 1 stack arg), void. ABI: __thiscall. VIRTUAL (the Attack vftable slot
// 24 = +0x60). THE SEH: the per-creation allocation folds (the chunk
// @0x640690 - the var_4 state machine over the five creation sites;
// written as the scoped __try/__except folds, the setForwardVelocity
// precedent). SINGLE-PASS (8 gotos, 0 back-edges).
// SEMANTIC: the attack state machine. delete_list FIRST (unconditional).
// Done/Failed/Invalidated -> the wait sprite; Wait -> indirect_fire_flag
// = 0; Goto/Goto2/Move -> the indirect decrement + the Move_To ladder
// (Goto2 = lastTrio+range, Move = lastTrio+0.0, Goto = target -> Move_To2
// / coords / lastTrio / no-move); the created chain = setSubAction(1) +
// add_action + the mTargetMovedState copy + timer = 0; the fail join =
// the 505/600 notify (gated !subActionValue) + the RECURSIVE
// set_state(1); Work = the both-null 506/600 notify -> Done, the guarded
// indirect clear, ready_to_attack (the -1 init) -> 1: reset_attack_timer+
// Work2 / 2: Failed / else missile_id == -1 (di persists) -> fight : wait;
// Work2 -> the fight sprite + need_to_attack = 1.
void __fastcall RGE_Action_Attack__set_state_new(
    RGE_Action_Attack* this_, DWORD dummy, unsigned __int8 new_state)
{
    RGE_Action_List__delete_list(this_->sub_actions);                  // asm 0x401639-0x40163C (:62 - FIRST, unconditional)
    this_->state = new_state;                                          // asm 0x401647 ([esi+0Ch])
    switch (new_state) {                                               // asm 0x40164A-0x401661 (byte_401958; jpt_401661 - the emitted tables diffed byte-for-byte at the gate)
    case RGE_ACTION_STATE_DONE:                                        // asm loc_40191C (cases 1,13,14)
    case RGE_ACTION_STATE_FAILED:
    case RGE_ACTION_STATE_INVALIDATED:
        this_->obj->vfptr->new_sprite(this_->obj, this_->wait_sprite); // asm loc_401921-0x401925 (call [edx+0B0h] - slot 44)
        return;
    case RGE_ACTION_STATE_WAIT:                                        // asm loc_401668 (case 2)
        this_->indirect_fire_flag = 0;                                 // asm 0x401668 ([esi+5Ah] = 0)
        return;
    case RGE_ACTION_STATE_GOTO:                                        // asm loc_40167F (cases 4,5,11)
    case RGE_ACTION_STATE_GOTO_2:
    case RGE_ACTION_STATE_MOVE: {
        if (this_->indirect_fire_flag)                                 // asm 0x40167F-0x401684 (mov al, [esi+5Ah]; test; jbe skip)
            this_->indirect_fire_flag -= 1;                            // asm 0x401686-0x401688 (dec al; store - the u8 decrement)
        RGE_Action_Move_To* created = NULL;                            // asm loc_4017C8 (xor eax, eax)
        if (new_state == RGE_ACTION_STATE_GOTO_2) {                    // asm 0x40168B-0x40168E (cmp cl, 5 - cl = new_state held from 0x401641)
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));  // asm 0x401690-0x401692 (push 44h; call operator_new - the CRT wrapper)
            __try {
                if (created)                                           // asm 0x40169E-0x4016A0
                    RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj,   // asm 0x4016AE-0x4016C8 (the idb constant - HOOKED -> routes)
                        this_->lastTargetPositionX, this_->lastTargetPositionY, this_->lastTargetPositionZ,
                        this_->range, this_->move_sprite);
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH) {}  // the chunk @0x640690 (the var_4 state 0)
        } else if (new_state == RGE_ACTION_STATE_MOVE) {               // asm 0x4016D2-0x4016D5 (cmp cl, 0Bh)
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));  // asm 0x4016D7-0x4016D9
            __try {
                if (created)
                    RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj,   // asm 0x4016F5-0x40170D (push 0 = range_in)
                        this_->lastTargetPositionX, this_->lastTargetPositionY, this_->lastTargetPositionZ,
                        0.0f, this_->move_sprite);
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH) {}  // the var_4 state 1
        } else if (this_->target_obj) {                                // asm 0x401717-0x40171C (mov eax, [esi+10h]; test)
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));  // asm 0x40171E-0x401720
            __try {
                if (created)
                    RGE_Action_Move_To__RGE_Action_Move_To2(created, this_->obj,  // asm 0x40173C-0x40174E (the idb constant - HOOKED -> routes)
                        this_->target_obj, this_->range, this_->move_sprite);
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH) {}  // the var_4 state 2
        } else if (this_->target_x != -1.0f) {                         // asm 0x401755-0x40175F (mov ecx, [esi+20h]; cmp ecx, eax(-1.0))
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));  // asm 0x401761-0x401763
            __try {
                if (created)
                    RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj,   // asm 0x40177B-0x401795
                        this_->target_x, this_->target_y, this_->target_z,
                        this_->range, this_->move_sprite);
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH) {}  // the var_4 state 3
        } else if (this_->lastTargetPositionX != -1.0f) {              // asm 0x40179C-0x40179F (cmp [esi+5Ch], eax; jz loc_4017D8 = the no-move fail)
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));  // asm 0x4017A1-0x4017A3
            __try {
                if (created)
                    RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj,   // asm 0x4017BB-0x4017C3 -> jmp loc_4016FB (the SHARED lastTrio ctor)
                        this_->lastTargetPositionX, this_->lastTargetPositionY, this_->lastTargetPositionZ,
                        this_->range, this_->move_sprite);
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH) {}  // the var_4 state 4
        }
        if (created) {                                                 // asm loc_4017CA-0x4017D6 (mov edi, eax; test edi, edi)
            RGE_Action__setSubAction((RGE_Action*)created, 1u);        // asm 0x4017F7-0x4017FB (:190 - HOOKED -> routes)
            RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);  // asm 0x401800-0x401804 (:67)
            created->mTargetMovedState = this_->mTargetMovedState;     // asm 0x401809-0x40180C (mov cl, [esi+3Dh]; mov [edi+3Dh], cl)
            this_->timer = 0.0f;                                       // asm 0x40180F (mov dword ptr [esi+2Ch], 0)
            return;                                                    // asm 0x401816-0x401826
        }
        if (!this_->subActionValue)                                    // asm loc_4017D8-0x4017DD (mov al, [esi+3Ch]; test; jnz loc_40185E = the skip)
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x4017DF-0x4017F5 (push 0 x2; 258h = 600; 1F9h = 505; the .c's obj-dropped render = the artifact - BOTH id pushes are obj->id)
                505, RGE_UNITAI_ACTION_ATTACK, 0, 0);
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);         // asm loc_40185E-0x401864 (push 1 - the RECURSIVE vftable dispatch -> routes)
        return;
    }
    case RGE_ACTION_STATE_WORK:                                        // asm loc_401829 (case 6)
    {
        if (!this_->target_obj && this_->target_x == -1.0f) {          // asm 0x401829-0x401837 (mov eax, [esi+10h]; test; cmp dword ptr [esi+20h], -1.0)
            if (!this_->subActionValue)                                // asm 0x401839-0x40183E
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x401840-0x401851 (1FAh = 506, 258h = 600)
                    506, RGE_UNITAI_ACTION_ATTACK, 0, 0);
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);     // asm loc_40185E (the SHARED tail)
            return;
        }
        if (this_->indirect_fire_flag && this_->target_obj)            // asm loc_40187A-0x401883 (mov cl, [esi+5Ah]; test; jbe; test eax; jz - the .c's && form, both guards asm-real here)
            this_->indirect_fire_flag = 0;                             // asm 0x401885
        int rangeState = -1;                                           // asm 0x40188D-0x401893 (or edi, -1; mov [esp+rangeState], edi - the init BEFORE the call)
        int rta = RGE_Action_Attack__ready_to_attack(this_, &rangeState);  // asm 0x401897 (the tier's own function - routes when owned)
        if (rta == 1) {                                                // asm 0x40189C-0x40189F
            RGE_Combat_Object__reset_attack_timer((RGE_Combat_Object*)this_->obj);    // asm 0x4018A1-0x4018A4 (the idb-constant direct call; the Combat downcast)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK_2);   // asm 0x4018A9-0x4018AF (push 7)
            return;
        }
        if (rta == 2) {                                                // asm 0x4018C5-0x4018C8
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x4018CA-0x4018D0 (push 0Dh)
            return;
        }
        this_->obj->vfptr->new_sprite(this_->obj,                      // asm loc_4018E6-0x401925 (call [edx+0B0h] - slot 44)
            (this_->missile_id != -1) ? this_->wait_sprite : this_->fight_sprite);  // asm 0x4018E9-0x4018F4 (cmp [esi+54h], di(-1) - di PERSISTS from the or edi, -1 at 0x40188D; jnz -> wait, else fight)
        return;
    }
    case RGE_ACTION_STATE_WORK_2:                                      // asm loc_4018F6 (case 7)
        this_->obj->vfptr->new_sprite(this_->obj, this_->fight_sprite);  // asm 0x4018F6-0x4018FF (push eax; call [edx+0B0h])
        this_->need_to_attack = 1;                                     // asm 0x401905 (mov byte ptr [esi+58h], 1)
        return;
    default:                                                           // asm def_401661 (cases 3,8,9,10,12 + > 14)
        return;                                                        // asm 0x40192B-0x40193B
    }
}

// THE EXPL_DROID BAKE externs (expldroid.cpp - the suicide-attack-unit list
// and its membership test; the SPEED2 getSpeedModifier extern precedent).
extern bool expl_installed;
extern bool __fastcall is_not_expl(__int16 id);

// ===========================================================================
// Replacement for RGE_Action_Attack::update - PASS 1, THE ALIGNED
// TRANSCRIPTION (the two-pass decode: 15 gotos, 0 back-edges - the
// mechanical conjunction fails; the pass-1 form first, the suite run,
// then the restructure per the Gather precedent). Original: @ 0x00401970
// (idb_functions.h:14). Range [0x401970,0x402241) (lst :1142/:2233 =
// 2257 bytes, retn = no stack args), unsigned __int8 return. ABI:
// __thiscall. VIRTUAL (the Attack vftable slot 10 = +0x28). NO SEH.
// The switch tables: the state dispatch (13 entries) + the sub-update
// dispatch (5 entries) - the emitted tables diffed byte-for-byte at the
// gate (the SWITCH-TABLE BYTE COMPARISON; the DD-partition check).
// SEMANTIC: the per-tick Attack driver - the full state machine (the
// .c at :45892-46289; the walks in this conversation pin every gate).
unsigned __int8 __fastcall RGE_Action_Attack__update_new(RGE_Action_Attack* this_)
{
    RGE_Action_Object* obj = this_->obj;                               // asm 0x401977 (v2)
    this_->timer = (float)((double)obj->owner->world->world_time_delta_seconds + (double)this_->timer);  // asm 0x40198D-0x401996 (fld [edx+0A8h] = world_time_delta_seconds; fadd [esi+2Ch]; fstp - ONE m32 rounding; the flags persist across the x87 block)
    if (this_->target_obj && this_->targetID == -1)                    // asm 0x401988-0x40199E (the cmp flags persisting)
        this_->target_obj = 0;                                         // asm 0x4019A0
    if (this_->target_obj2 && this_->target2ID == -1)                  // asm 0x4019A3-0x4019AB
        this_->target_obj2 = 0;                                        // asm 0x4019AD
    BUnitGroup* group = RGE_Game_World__getUnitGroup((RGE_Game_World*)obj->owner->world, obj->mGroupID);  // asm 0x4019B0-0x4019C5 (:6099; mGroupID @+0x88)

    // STAGE: the target validity (loc_4019C7-0x401A41) - targetDead covers the
    // null/sentinel/invisible/dead-state arms; every arm of the dead block
    // below returns, the valid fall continues to the sync stage.
    bool targetDead = false;
    if (this_->targetID != -1) {                                       // asm 0x4019C7-0x4019CC (cmp eax, ebx; jz loc_401C93)
        RGE_Static_Object* target = this_->target_obj;
        if (!(target
            && RGE_Game_World__object((RGE_Game_World*)obj->owner->world, this_->targetID)  // asm 0x4019DB-0x4019ED (:6086)
            && !target->vfptr->is_invisible(target, obj->owner->id)    // asm 0x4019F5-0x401A0C (ecx = the TARGET; push owner->id; call [eax+48h] - slot 18)
            && !(target->object_state >= 3u                            // asm 0x401A12-0x401A1E (cmp al, 3; jb valid)
                && (target->master_obj->object_group != 31u            // asm 0x401A20-0x401A28 (cmp word ptr [edx+1Eh], 1Fh - the TREE)
                    || obj->master_obj->object_group != 36u            // asm 0x401A2E-0x401A39 (cmp word ptr [edx+1Eh], 24h - the CANNON)
                    || target->object_state > 5u)))) {                 // asm 0x401A3F-0x401A41 (cmp al, 5; ja INVALID)
            targetDead = true;
        }
    }
    if (targetDead) {
        // STAGE: the dead/invalid target (loc_401B3E-0x401C8F) - all arms return
        if (this_->target_obj
            && !RGE_Game_World__object((RGE_Game_World*)obj->owner->world, this_->targetID))  // asm loc_401B3E-0x401B5A (:6086)
            this_->vfptr->set_target_obj(this_, 0);                    // asm 0x401B5C-0x401B61 (slot 22)
        if (RGE_Static_Object__unitAI((RGE_Static_Object*)obj)) {      // asm 0x401B64-0x401B6E (:261)
            RGE_Static_Object* target = this_->target_obj;             // asm 0x401B75-0x401B78
            if (!target || target->owner->id || obj->master_obj->object_group != 58u)  // asm 0x401B7A-0x401B93 (the owner->id test; cmp group, di(3Ah) - the WORKER carve)
                UnitAIModule__removeCurrentTarget(RGE_Static_Object__unitAI((RGE_Static_Object*)obj));  // asm loc_401B95-0x401B9F (the Worker-with-neutral-target jz skip at 0x401B93)
        }
        this_->vfptr->set_target_obj(this_, 0);                        // asm loc_401BA4-0x401BAA
        this_->target_x = -1.0f;                                       // asm 0x401BB5-0x401BBB (the sentinel trio)
        this_->target_y = -1.0f;
        this_->target_z = -1.0f;
        bool skipNotify;
        if (obj->owner->id == 0                                        // asm 0x401BBE-0x401BC9
            || obj->owner->vfptr->computerPlayer(obj->owner)           // asm 0x401BCF-0x401BD6 (slot 7)
            || (RGE_Static_Object__unitAI((RGE_Static_Object*)obj)
                && UnitAIModule__currentOrder(RGE_Static_Object__unitAI((RGE_Static_Object*)obj)) == -1)) {  // asm 0x401BDC-0x401BF7
            skipNotify = this_->subActionValue != 0;                   // asm loc_401C8A-0x401C8F (jnz loc_401C79 = the skip)
        } else {
            UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)obj);
            if (ai->stateValue == 0u                                   // asm loc_401BFF-0x401C0F (mov cl, [eax+169h] - the unguarded read, the original's own shape)
                && obj->vfptr->maximumSpeed(obj) > 0.0                 // asm 0x401C11-0x401C27 (slot 98; test ah,41h; jnz skip - the ordered > 0 = the literal > MATCH row)
                && obj->master_obj->object_group != 58u                // asm 0x401C29-0x401C33 (cmp [edx+1Eh], di)
                && (!group || !group->mFormationLocked)) {             // asm 0x401C35-0x401C41 (test ebp; jz drive; mov eax, [ebp+0F4h])
                this_->vfptr->set_state(this_, 5u);                    // asm loc_401C43-0x401C49 (push 5)
                return 5;                                              // asm 0x401C4C-0x401C53 (mov al, 5)
            }
            skipNotify = this_->subActionValue != 0;                   // asm loc_401C54-0x401C59 (jnz loc_401C79)
        }
        if (!skipNotify)
            obj->vfptr->notify(obj, obj->id, obj->id,                  // asm loc_401C5B-0x401C73 (1FAh = 506, 258h = 600)
                506, RGE_UNITAI_ACTION_ATTACK, 0, 0);
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);         // asm loc_401C79-0x401C7F (push 1)
        return 5;                                                      // asm 0x401C85-0x401C89 (mov al, 5)
    }
    // STAGE: the position sync (loc_401A47-0x401B3D) - the chase arms return;
    // the equal-skip and the non-chase falls continue to the main stage.
    if (this_->targetID != -1 && !targetDead) {
        RGE_Static_Object* target = this_->target_obj;
        if (!(X87_EQ_NAN(target->world_x, this_->lastTargetPositionX)             // asm 0x401A47-0x401A52 (fld [ecx+48h]; fcomp [esi+5Ch]; test ah,40h; jz the update - the C3-only {==,NaN} pair)
            && X87_EQ_NAN(target->world_y, this_->lastTargetPositionY))) {        // asm 0x401A54-0x401A5F (fld [ecx+4Ch]; fcomp [esi+60h]; test ah,40h; jnz loc_401C93 = the skip)
            this_->lastTargetPositionX = target->world_x;                         // asm 0x401A65-0x401A68
            this_->lastTargetPositionY = target->world_y;                         // asm 0x401A6B-0x401A6E
            this_->lastTargetPositionZ = target->world_z;                         // asm 0x401A71-0x401A74
            unsigned __int8 vis = target->vfptr->get_visibility(target, (RGE_Player*)obj->owner);  // asm 0x401A7A-0x401A80 (push owner->id; call [edx+16Ch] = slot 91 - the OWNED get_visibility_new routes)
            if (vis == 15u || vis == 128u && target->master_obj->fog_flag) {      // asm 0x401A86-0x401A9D (cmp al, 0Fh; cmp al, 80h; mov al, [edx+75h] = fog_flag @+0x75)
                unsigned __int8 st = this_->state;                                // asm 0x401B1E-0x401B21
                if (st == RGE_ACTION_STATE_GOTO_2 || st == RGE_ACTION_STATE_MOVE) {  // asm 0x401B21-0x401B27
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);        // asm loc_401B2D-0x401B33 (push 4)
                    return 0;                                                     // asm 0x401B36-0x401B3D
                }
                // the fall: state not 5/11 -> the main stage (asm jnz loc_401C93)
            } else if (this_->state == RGE_ACTION_STATE_GOTO || this_->state == RGE_ACTION_STATE_GOTO_2) {  // asm 0x401A9F-0x401AA8
                if (obj->owner->id == 0                                           // asm 0x401AAE-0x401ABA
                    || (RGE_Static_Object__unitAI((RGE_Static_Object*)obj)
                        && UnitAIModule__currentOrder(RGE_Static_Object__unitAI((RGE_Static_Object*)obj)) == -1)) {  // asm 0x401ABC-0x401AD6
                    this_->vfptr->set_target_obj(this_, 0);                       // asm loc_401AF3-0x401AF8 (push edi(0))
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);      // asm 0x401AFB-0x401B01 (push 0Dh)
                    if (!this_->subActionValue)                                   // asm 0x401B04-0x401B09
                        obj->vfptr->notify(obj, obj->id, obj->id,                 // asm loc_401F29-0x401F35 (258h = 600, 1F9h = 505 - the SHARED notify tail)
                            505, RGE_UNITAI_ACTION_ATTACK, 0, 0);
                    return 3;                                                     // asm loc_401F3B-0x401F42 (mov al, 3)
                }
                if (this_->state == RGE_ACTION_STATE_GOTO_2) {                    // asm loc_401AD8-0x401ADC (cmp byte ptr [esi+0Ch], 5; jz loc_401C93 - the state-5 RE-DISPATCH the .c dropped)
                    // the re-dispatch fall -> the main stage
                } else {
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO_2);      // asm 0x401AE2-0x401AE8 (the set_state(5) + return 0)
                    return 0;
                }
            }
            // the vis-routed non-5/11 fall -> the main stage
        }
        // the position-equal skip -> the main stage
    }
    // STAGE: the main stage (loc_401C93) - the target2 cleanup, the herd/ally
    // gate, the inside_obj gate, the state switch.
    if (this_->target2ID != -1
        && !RGE_Game_World__object((RGE_Game_World*)obj->owner->world, this_->target2ID))  // asm loc_401C93-0x401CAE (:6086)
        this_->vfptr->set_target_obj2(this_, 0);                       // asm 0x401CB0-0x401CB5 (slot 23)
    RGE_Static_Object* target = this_->target_obj;
    if (target && obj->owner->id != 0) {                               // asm 0x401CB8-0x401CD2
        if ((!this_->was_same_owner && target->master_obj->object_group == 1u)   // asm 0x401CD4-0x401CE2 (mov dl, [esi+59h]; the HERD cmp bx(1))
            || target->master_obj->object_group != 1u) {               // asm loc_401CE4-0x401CEB (the .c's (!was_same_owner && group == 1) || group != 1)
            if (obj->owner->vfptr->isAlly(obj->owner, target->owner->id)) {  // asm loc_401CED-0x401CFE (slot 9 = +0x24)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);  // asm 0x401D00-0x401D05 (push ebx(1))
                if (!this_->subActionValue)                        // asm 0x401D08-0x401D0D
                    obj->vfptr->notify(obj, obj->id, obj->id,      // asm 0x401D0F-0x401D29 (1FBh = 507, 258h = 600; p2 = targetID)
                        507, RGE_UNITAI_ACTION_ATTACK, this_->targetID, 0);
                return 1;                                          // asm 0x401D2F-0x401D36 (mov al, bl)
            }
        }
    }
    RGE_Static_Object* targetNow = this_->target_obj;              // asm loc_401D37 (mov eax, [esi+10h]) - the FRESH re-read after the ally block
    if (!(targetNow && targetNow->inside_obj)) {                   // asm 0x401D3A-0x401D47 (the jz to the main switch)
    switch (this_->state) {                                        // asm loc_401DCE (the main state switch: jpt_401DDD)
    case RGE_ACTION_STATE_DONE:                                               // asm loc_402147 (case 1)
        return 1;                                                             // asm (mov al, bl)
    case RGE_ACTION_STATE_WAIT: {                                             // asm loc_4021E2 (case 2)
        if (this_->obj->sprite_list->vfptr->looped(this_->obj->sprite_list, this_->obj->sprite) == 1u) {  // asm 0x4021E2-0x4021F3 (slot 10 = +0x28; cmp al, bl)
            this_->vfptr->set_target_obj(this_, 0);                            // asm 0x4021F9-0x4021FF (slot 22)
            if (!this_->subActionValue)                                       // asm 0x402202-0x402207
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x402209-0x40221E (1FAh = 506, 258h = 600)
                    506, RGE_UNITAI_ACTION_ATTACK, 0, 0);
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);            // asm 0x402227-0x40222C (push ebx(1))
            return (this_->subActionValue != 1) ? 1u : 5u;                    // asm 0x40222F-0x402241 (dec/neg/sbb/and/add al, 5 - decoded exactly: 5 iff subActionValue == 1) - INSIDE the looped==1 path
        }
        return 0;                                                             // asm 0x4021F3 (jnz def_401DDD) - looped != 1 -> return 0
    }
    case RGE_ACTION_STATE_GOTO:                                               // asm loc_401DE4 (cases 4,5,11)
    case RGE_ACTION_STATE_GOTO_2:
    case RGE_ACTION_STATE_MOVE:
        switch (this_->sub_actions->vfptr->update(this_->sub_actions)) {      // asm 0x401DE4-0x401DF4 (slot 4; and eax, 0FFh; cmp 4; jpt_401DFA)
        case 0u: {                                                            // asm loc_401F43 (case 0)
            if (!this_->target_obj
                || fabs((double)this_->range) <= 0.0000001000000011686097     // asm 0x401F4A-0x401F5A (fld [esi+4Ch]; fabs; fcomp dbl_6545B0 = the 1e-7f widening, binary-verified; test ah,41h; jnz default - the {<=, NaN} -> return 0)
                || RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj) > this_->range) {  // asm 0x401F5C-0x401F6D (fcomp [esi+4Ch]; test ah,41h; jz default - the {> ordered} -> return 0, the literal > MATCH row)
                return 0;                                                     // asm def_401DDD
            }
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);             // asm loc_401F6F-0x401F75 (push 6 - the Work hand-off, the .c's LABEL_107)
            return 0;                                                     // asm 0x401F78-0x401F7F
        }
        case 1u: {                                                            // asm loc_401E39 (case 1)
            unsigned __int8 st = this_->state;                               // asm 0x401E39-0x401E3C
            if (st != RGE_ACTION_STATE_GOTO_2 && st != RGE_ACTION_STATE_MOVE) {  // asm 0x401E3C-0x401E42 (cmp 5; jz; cmp 0Bh)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);         // asm loc_401F6F-0x401F75 (push 6 - the Work hand-off)
                return 0;                                                      // asm 0x401F78-0x401F7F
            }
            if (!this_->target_obj) {                                         // asm loc_401E48-0x401E4D
                if (!this_->subActionValue)                                   // asm 0x401E4F-0x401E54
                    this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x401E56-0x401E6E (1FAh = 506, 258h = 600)
                        506, RGE_UNITAI_ACTION_ATTACK, 0, 0);
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);        // asm 0x401E74-0x401E79 (push ebx(1))
                return 1;                                                     // asm 0x401E7C-0x401E83 (mov al, bl)
            }
            unsigned __int8 vis = this_->target_obj->vfptr->get_visibility(this_->target_obj, (RGE_Player*)this_->obj->owner);  // asm 0x401E84-0x401E93 (slot 91)
            if (vis && (vis != 128u || this_->target_obj->master_obj->fog_flag)) {  // asm 0x401E93-0x401E9D
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);         // asm loc_401F6F-0x401F75 (push 6 - the shared Work hand-off, the .c's LABEL_107)
                return 0;                                                      // asm 0x401F78-0x401F7F
            }
            if (this_->state == RGE_ACTION_STATE_GOTO_2) {                    // asm loc_401EB0-0x401EB4 (cmp 5)
                if (this_->obj->owner->id != 0                                // asm 0x401EB6-0x401EC4
                    && !X87_LT_NAN(this_->range, 1.0f)                        // asm 0x401EC6-0x401ED4 (fld [esi+4Ch]; fcomp const_f_one; test ah,1; jnz fail - the C0-only {< 1.0, NaN} = X87_LT_NAN -> the fail path)
                    && !this_->obj->owner->vfptr->computerPlayer(this_->obj->owner)) {  // asm 0x401ED6-0x401EDD (slot 7)
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);    // asm 0x401EDF-0x401EE5 (push 0Bh)
                    return 0;                                                 // asm 0x401EE8-0x401EEF
                }
                this_->vfptr->set_target_obj(this_, 0);                        // asm loc_401EF0-0x401EF6 (slot 22)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);      // asm 0x401EF9-0x401EFF (push 0Dh)
            } else {
                this_->vfptr->set_target_obj(this_, 0);                        // asm loc_401F04-0x401F0A
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);      // asm loc_401F0D-0x401F13 (push 0Dh - LABEL_100)
            }
            if (!this_->subActionValue)                                       // asm loc_401F16-0x401F1B
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 505, RGE_UNITAI_ACTION_ATTACK, 0, 0);   // asm loc_401F29-0x401F35 (258h = 600, 1F9h = 505 - the SHARED notify tail, INLINED)
            return 3;                                                          // asm loc_401F3B-0x401F42 (mov al, 3)
        }
        case 3u:                                                              // asm loc_401F0D (case 3 = LABEL_100; the asm's table entry skips the set_target_obj)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);          // asm 0x401F0D-0x401F13 (push 0Dh)
            if (!this_->subActionValue)                                       // asm loc_401F16-0x401F1B
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 505, RGE_UNITAI_ACTION_ATTACK, 0, 0);   // asm loc_401F29-0x401F35 (258h = 600, 1F9h = 505 - the SHARED notify tail, INLINED)
            return 3;                                                          // asm loc_401F3B-0x401F42
        case 4u:                                                              // asm loc_401E01 (case 4)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_INVALIDATED);     // asm 0x401E01-0x401E07 (push 0Eh = 14)
            if (!this_->subActionValue)                                       // asm 0x401E0A-0x401E0F
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 507, RGE_UNITAI_ACTION_ATTACK, this_->targetID, 0);   // asm 0x401E11-0x401E2B (1FBh = 507, 258h = 600; p2 = targetID - the LABEL_81 tail, INLINED)
            return 4;                                                         // asm 0x401E31-0x401E38 (mov al, 4)
        default:                                                              // asm def_401DDD / jpt_401DFA case 2
            return 0;                                                          // asm 0x401F78-0x401F7F
        }
    case RGE_ACTION_STATE_WORK: {                                             // asm loc_401F80 (case 6)
        int rangeState = -1;                                                  // asm 0x401F84
        int rtaResult = RGE_Action_Attack__ready_to_attack(this_, &rangeState);     // asm 0x401F8C-0x401F8F (the tier's own function - routes)
        if (rtaResult == 2) {                                                       // asm 0x401F94-0x401F97
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);          // asm 0x401F9B-0x401FA1 (push 0Dh)
            return 0;                                                          // asm 0x401FA5-0x401FA9
        }
        if (rtaResult != 1u)                                                        // asm 0x401FAA-0x401FAC (cmp eax, ebx(1))
            return 0;                                                          // asm def_401DDD
        if (this_->target_obj
            && this_->obj->master_obj->master_type == 70u                     // asm 0x401FB5-0x401FBF (cmp byte ptr [eax+4], 46h - COMBAT)
            && RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)->autoAttackOrderFlag  // asm 0x401FC1-0x401FCE ([eax+191h] = 401)
            && !TRIBE_Combat_Object__canAttackUnit((TRIBE_Combat_Object*)this_->obj, this_->target_obj)) {  // asm 0x401FD0-0x401FDE (the OWNED canAttackUnit_new routes; the downcast call form)
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x401FE0-0x401FF8 (1FAh = 506, 258h = 600)
                506, RGE_UNITAI_ACTION_ATTACK, 0, 0);
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);            // asm 0x401FFE-0x402003 (push ebx(1))
            return 0;                                                          // asm 0x402009-0x40200D
        }
        RGE_Combat_Object__reset_attack_timer((RGE_Combat_Object*)this_->obj);  // asm 0x40200E-0x402013 (the idb-constant direct call); the Combat downcast (the OWNED reset_attack_timer_new routes)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK_2);              // asm 0x402016-0x40201C (push 7)
        return 0;                                                              // asm 0x402022-0x402026
    }
    case RGE_ACTION_STATE_WORK_2: {                                           // asm loc_402027 (case 7)
        // RESTRUCTURE (the pass-2 goto dissolution): the asm's 134/127 blocks
        // (entered via the 0x40202C/0x40203F/0x40209A jumps + the 0x402092 fall)
        // become the fire/sprite phase flags - the fire block ONCE at the phase
        // end, the sprite block per non-fire exit; every re-check kept.
        bool firePhase = this_->need_to_attack != 0;                      // asm 0x402027-0x40202A (jnz LABEL_134)
        if (!firePhase) {
            bool pass = this_->obj->vfptr->can_attack(this_->obj) == 1u;  // asm 0x402032-0x40203F (slot 61 = +0xF4; cmp al, bl(1))
            if (pass && !this_->target_obj && !this_->indirect_fire_flag) {   // asm 0x402041-0x40204D
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);    // asm 0x40204F-0x402055 (push 2 - LABEL_122)
                return 0;                                                 // asm 0x40205B-0x40205F
            }
            if (pass) {
                RGE_Combat_Object__reset_attack_timer((RGE_Combat_Object*)this_->obj);  // asm 0x402060-0x402063 (the idb-constant direct call); the Combat downcast
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);    // asm 0x402068-0x40206E (push 6)
                unsigned __int8 st = this_->state;                        // asm 0x402071-0x402074 (the RE-READ after the recursive set_state)
                if (st == RGE_ACTION_STATE_FAILED || st == RGE_ACTION_STATE_GOTO    // asm 0x402074-0x402086 (cmp 0Dh/4/5 - the jz def_401DDD x3)
                    || st == RGE_ACTION_STATE_GOTO_2)
                    return 0;
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK_2);  // asm 0x40208C-0x402092 (push 7 - the cast set_state sets need = 1)
            }
            firePhase = this_->need_to_attack != 0;                       // asm 0x402095-0x402098 (the RE-READ - BOTH the !pass and the post-set paths reach it - jnz LABEL_134)
            if (!firePhase) {
                // LABEL_127 (need == 0): the sprite state checks
                if (!this_->obj->sprite
                    || this_->obj->sprite_list->vfptr->looped(this_->obj->sprite_list, this_->obj->sprite) == 1u  // asm 0x4020A6-0x4020AC (slot 10 = +0x28)
                    || (this_->obj->sprite->flag & 1) == 0) {             // asm 0x4020B3-0x4020BF (test [edx+80h], bl - the RGE_Sprite::flag @+0x80)
                    if (this_->target_obj || this_->indirect_fire_flag) {  // asm 0x4020C5-0x4020D1
                        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);    // asm 0x4020E4-0x4020EA (push 6)
                        return 0;                                         // asm 0x4020F0-0x4020F4
                    }
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);    // asm 0x4020D3-0x4020D9 (push 2 - LABEL_122)
                    return 0;                                         // asm 0x4020DF-0x4020E3
                }
                return 0;                                             // asm (the looped sprite's early exit at 0x4020BF)
            }
        }
        // LABEL_134: the fire phase (firePhase == true)
        if (this_->obj->master_obj->object_group == 34u                       // asm loc_4020F5-0x402100 (cmp word ptr [ecx+1Eh], 22h - the UNDEPLOYED CANNON)
            && this_->obj->owner->vfptr->computerPlayer(this_->obj->owner)) { // asm 0x402102-0x40210C (slot 7)
            if (this_->target_obj) {                                           // asm 0x40210E-0x402116
                ((RGE_Combat_Object*)this_->obj)->vfptr->attack4((RGE_Combat_Object*)this_->obj, this_->target_obj, (RGE_Combat_Object*)this_->obj);  // asm 0x402118-0x402121 (call [edx+3BCh] - slot 239 = +0x3BC = attack4(this, target, attacker), the Combat-vtbl members 238/239 = attack3/attack4 line-arith verified; the .c's vfptr[1].gbg_get_civ_override render = the artifact)
                this_->need_to_attack = 0;                                     // asm 0x402122
                return 1;                                                      // asm 0x402126-0x40212D (mov al, bl)
            }
            ((RGE_Combat_Object*)this_->obj)->vfptr->attack3((RGE_Combat_Object*)this_->obj, this_->target_x, this_->target_y, this_->target_z, (RGE_Combat_Object*)this_->obj);  // asm loc_40212E-0x402140 (call [eax+3B8h] - slot 238 = +0x3B8 = attack3(this, x, y, z, attacker); the .c's play_unload_sound render = the artifact)
            this_->need_to_attack = 0;                                         // asm 0x402143
            return 1;                                                          // asm loc_402147-0x40214E (mov al, bl)
        }
        bool fireNow = false;
        unsigned __int16 frame;
        if (((RGE_Master_Combat_Object*)this_->obj->master_obj)->missile_id == -1) {  // asm loc_40214F-0x40215D (cmp word ptr [eax+154h], 0FFFFh - the master's missile_id @+340; the Combat-master downcast)
            // THE EXPL_DROID BAKE (expl1 @0x40215F-0x402165 // <EXPL_DROID>):
            // the live binary gates the immediate fire on expl_list
            // membership (the stub replays the id read and the original jz
            // at 0x402165 consumes the stub's flags); expl_installed ==
            // false -> the vanilla id == 1239 gate runs.
            if (expl_installed
                ? !is_not_expl(this_->obj->master_obj->id)          // the id in the suicide list -> fire now
                : this_->obj->master_obj->id == 1239u)              // asm 0x40215F-0x402165 (cmp word ptr [eax+18h], 4D7h - the REGISTRY CANDIDATE, patch-replaced)
                fireNow = true;
            else
                frame = (unsigned __int16)((int)((double)this_->obj->sprite->frame_num * 0.5));  // asm 0x402167-0x402181 (movsx word ptr [eax+6Eh] = frame_num @+0x6E; fild; fmul const_f_half; __ftol)
        } else {
            frame = (unsigned __int16)this_->fire_missile_at_frame;            // asm loc_402183 (mov ax, [esi+56h])
        }
        if (!fireNow && frame && !this_->obj->sprite_list->vfptr->frame_passed(this_->obj->sprite_list, this_->obj->sprite, frame))  // asm 0x402187-0x40219B (slot 9 = +0x24)
            return 0;                                                          // asm def_401DDD
        // LABEL_146: the fire
        if (this_->target_obj)                                                 // asm loc_4021A1-0x4021B5
            ((RGE_Combat_Object*)this_->obj)->vfptr->attack4((RGE_Combat_Object*)this_->obj, this_->target_obj, (RGE_Combat_Object*)this_->obj);  // asm 0x4021AD-0x4021B5 (call [edx+3BCh])
        else
            ((RGE_Combat_Object*)this_->obj)->vfptr->attack3((RGE_Combat_Object*)this_->obj, this_->target_x, this_->target_y, this_->target_z, (RGE_Combat_Object*)this_->obj);  // asm loc_4021C1-0x4021D0 (call [eax+3B8h])
        this_->need_to_attack = 0;                                             // asm 0x4021B5
        return 0;                                                              // asm 0x4021B9-0x4021C0
    }
    case RGE_ACTION_STATE_FAILED:                                             // asm loc_401F1D (case 13)
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm loc_401F29-0x401F35 (258h = 600, 1F9h = 505 - the SHARED notify tail, INLINED)
            505, RGE_UNITAI_ACTION_ATTACK, 0, 0);
        return 3;                                                              // asm loc_401F3B-0x401F42 (mov al, 3)
    default:                                                                   // asm def_401DDD (cases 3,8,9,10,12 + > 13)
        return 0;                                                              // asm 0x401F78-0x401F7F
    }
    } else {
        if (this_->obj->owner->id != 0
            && !this_->obj->owner->vfptr->computerPlayer(this_->obj->owner)
            && (!RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)
                || UnitAIModule__currentOrder(RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)) != -1)) {  // asm 0x401D4D-0x401D84
            this_->vfptr->set_target_obj(this_, 0);                            // asm 0x401D86-0x401D8C
            this_->target_x = -1.0f;                                          // asm 0x401D92-0x401D9D
            this_->target_y = -1.0f;
            this_->target_z = -1.0f;
            if (RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj))    // asm 0x401DA0-0x401DA7
                UnitAIModule__removeCurrentTarget(RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj));  // asm 0x401DA9-0x401DB3
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO_2);          // asm 0x401DB8-0x401DBE (push 5 - the .c's LABEL_27)
            return 0;
        }
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);                // asm loc_401DC9-0x401DCC (the gate failure) - falls to the shared tail
    }
 // the LABEL_81 tail (its goto was inlined at the case-4 site):
    if (!this_->subActionValue)                                               // asm loc_401E0A-0x401E0F
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x401E11-0x401E2B (1FBh = 507, 258h = 600; p2 = targetID)
            507, RGE_UNITAI_ACTION_ATTACK, this_->targetID, 0);
    return 4;                                                                  // asm 0x401E31-0x401E38 (mov al, 4)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::vector_deleting_destructor. Original:
// @ 0x00401130 (idb_functions.h:6). Range [0x401130,0x40114B) (lst
// :150/:173, retn 4 = 1 stack arg), void* return. ABI: __thiscall.
// VIRTUAL (the Attack vftable slot 0). SEMANTIC: the family deleting
// destructor - the tier destructor, then the flags&1 heap release, this_
// returned (the Static scalar-deleting family precedent).
void* __fastcall RGE_Action_Attack__vector_deleting_destructor_new(
    RGE_Action_Attack* this_, DWORD dummy, unsigned int flags)
{
    RGE_Action_Attack__destructor_new(this_);                       // asm 0x401133 (call RGE_Action_Attack__destructor - the OWNED replacement routes; the same-class call, no cast)
    if (flags & 1)                                                  // asm 0x401138-0x40113D (test byte ptr [esp+4+flags], 1)
        operator_delete_internal(this_);                            // asm 0x40113F-0x401145 (push esi; call operator_delete; add esp, 4 - the CRT-wrapper rule)
    return this_;                                                   // asm 0x401148-0x40114B (mov eax, esi)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::stop. Original: @ 0x00401610
// (idb_functions.h:12). Range [0x401610,0x40161C) (lst :750/:762), int
// return. ABI: __thiscall (this-only). VIRTUAL (the Attack vftable slot
// 11 = +0x2C; the DATA XREF also lists the TRIBE_Action_Pack vftable - the
// entry hook covers both dispatches). SEMANTIC: the attack stop = the
// Done state hand-off + 1.
int __fastcall RGE_Action_Attack__stop_new(RGE_Action_Attack* this_)
{
    this_->vfptr->set_state(this_, 1u);                             // asm 0x401610-0x401617 (mov eax, [ecx]; push 1; call [eax+60h] - slot 24 = +0x60 = set_state, the Attack Vtbl's last member - the NAMED dispatch)
    return 1;                                                       // asm 0x401617-0x40161C (mov eax, 1)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::set_target_obj. Original:
// @ 0x00402AD0 (idb_functions.h:18). Range [0x402AD0,0x402AEE) (lst
// :3158/:3179, retn 4 = 1 stack arg), void. ABI: __thiscall. VIRTUAL (the
// Attack vftable slot 22 = +0x58). SEMANTIC: the attack re-target - a
// genuinely NEW target resets the unit's attack_count (the swing
// counter), then the base set_target_obj.
void __fastcall RGE_Action_Attack__set_target_obj_new(
    RGE_Action_Attack* this_, DWORD dummy, RGE_Static_Object* new_obj)
{
    if (new_obj != this_->target_obj)                               // asm 0x402AD4-0x402AD9 (mov edx, [ecx+10h]; cmp eax, edx; jz skip)
        ((RGE_Combat_Object*)this_->obj)->attack_count = 0;         // asm 0x402ADB-0x402ADE (mov edx, [ecx+8]; mov dword ptr [edx+170h], 0 - the Combat extension's attack_count @+0x170 = 368, the batch 1b-4 map; the .c's obj[1].inside_obj render = the base-pointer artifact, the asm's dword width wins; the downcast = the Combat-tier factory identity - the asm is unguarded and every in-vivo Attack-action carrier is Combat+, the bare 0x144 Action tier would put the store outside its allocation)
    RGE_Action__set_target_obj((RGE_Action*)this_, new_obj);        // asm 0x402AE8-0x402AEE (push eax; call RGE_Action__set_target_obj - the idb constant :182, HOOKED -> routes; the derived->base cast)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::first_in_stack. Original:
// @ 0x00401530 (idb_functions.h:11). Range [0x401530,0x401606) (lst
// :629/:746, retn 4 = 1 stack arg), void. ABI: __thiscall. VIRTUAL (the
// Attack vftable slot 7 = +0x1C). SEMANTIC: the attack order intake - a
// unit without the AI module or inside a container cannot attack
// (Failed); a fresh order clears the swing counter; the ready-to-attack
// probe decides: 2 = Failed outright, 0/out-of-range = Failed unless the
// AI says the move to attack is possible (Goto), otherwise Work.
void __fastcall RGE_Action_Attack__first_in_stack_new(
    RGE_Action_Attack* this_, DWORD dummy, unsigned __int8 first_time)
{
    if (!RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)  // asm 0x401535-0x40153F (mov ecx, [esi+8]; call RGE_Static_Object__unitAI :261; test eax, eax)
        || this_->obj->inside_obj) {                                // asm 0x401550-0x401558 (mov ecx, [eax+2Ch]; test ecx, ecx)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);    // asm 0x401541-0x40154D / 0x40155A-0x401566 (push 0Dh; call [eax+60h])
        return;
    }
    if (first_time)                                                 // asm 0x401569-0x40156F (mov cl, [esp+0Ch+first_time]; test cl, cl)
        ((RGE_Combat_Object*)this_->obj)->attack_count = 0;         // asm 0x401571 (mov dword ptr [eax+170h], 0 - the same Combat-extension store as set_target_obj, the same factory-identity downcast)
    int rangeState = -1;                                            // asm 0x401582 (mov dword ptr [esp+10h+rangeState], 0FFFFFFFFh - the init BEFORE the call)
    int rta = RGE_Action_Attack__ready_to_attack(this_, &rangeState);  // asm 0x40157B-0x40158F (lea eax, [esp+0Ch+rangeState]; push eax; call RGE_Action_Attack__ready_to_attack - the tier's own function, the DIRECT call; the idb constant :16 @0x4023B0 routes when owned per the hook-routing ruling)
    if (rta == 2) {                                                 // asm 0x401591-0x401594 (cmp edi, 2)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);    // asm 0x401596-0x40159C (push 0Dh)
        return;
    }
    double speed = this_->obj->vfptr->maximumSpeed(this_->obj);     // asm 0x40159F-0x4015AA (mov ecx, [esi+8]; mov eax, [ecx]; call [eax+188h] - slot 98 = +0x188, the M12a DOUBLE-typed grounding; the st0 captured UNROUNDED - the corollary-(h) capture; the local because X87_EQ_NAN evaluates its operand twice and this operand is a vtbl CALL)
    if (!X87_EQ_NAN(speed, 0.0f)) {                                 // asm 0x4015AA-0x4015B5 (fcomp ds:const_f_zero; fnstsw ax; test ah, 40h; jnz loc_4015E0 - the C3-only {== 0, NaN} taken set = the composed form, the probe-table row 5)
        UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);  // asm 0x4015B7-0x4015BF (the re-call - faithful)
        if (ai->vfptr->canMoveToAttack(ai, this_->targetID) != 0) { // asm 0x4015BF-0x4015CF (mov ecx, [esi+18h]; push ecx; call [edx+0CCh] - slot 51 = +0xCC = canMoveToAttack(UnitAIModule*, int), idb_types.h:7652, the line-arithmetic double-checked on the member-47/21 anchors; test eax, eax)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);  // asm 0x4015D1-0x4015DD (push 4)
            return;
        }
    }
    if (rta != 0 || rangeState == 1)                                // asm 0x4015E0-0x4015E9 (test edi, edi; jnz keep; cmp [esp+0Ch+rangeState], 1; jz keep - the loc_4015E0 join, 2 inbound)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);      // asm 0x4015FA-0x401603 (push 6)
    else
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);    // asm 0x4015EB-0x4015F7 (push 0Dh)
}

// ===========================================================================
// Replacement for RGE_Action_Attack::copy_obj_sprites. Original:
// @ 0x4029D0 (idb_functions.h:17). Range [0x4029D0,0x402AC6) (lst
// :3003/:3154, retn 0Ch = 3 stack args), void. ABI: __thiscall. VIRTUAL
// (the Attack vftable slot 18 = +0x48). SEMANTIC: the attack-sprite
// re-binding for the task/transform copies - each of the three action
// sprites (move/fight/wait) is re-mapped through the task sprite trio
// when both tasks exist, else re-fetched from the unit's master when the
// current sprite still matches what that master's getter produces, with
// the source unit's master as the donor.
// THE REMAP SHAPE: task-first (the Move_To tier's order inverted - the
// asm checks the task pair before the getters); the fight/wait arms
// check BOTH work_sprite (@+0x38) and work_sprite2 (@+0x3C); IDA's
// footnotes (402A39/402A8E: conditional optimized away 'edi.4!=0') prove
// old_task/new_task are both guaranteed at the second checks - the
// combined task && new_task conjunct form is exact. The getter chains
// appear in both the paired-task and the task-null branches (the asm's
// loc_402A45/loc_402AA2 joins - the same sequence from both arrivals,
// the compiler-merge class).
void __fastcall RGE_Action_Attack__copy_obj_sprites_new(
    RGE_Action_Attack* this_, DWORD dummy, RGE_Master_Action_Object* source,
    RGE_Task* task, RGE_Task* new_task)
{
    if (this_->move_sprite) {                                       // asm 0x4029E2-0x4029E7 (mov eax, [esi+40h]; test)
        if (task && new_task && this_->move_sprite == task->move_sprite)  // asm 0x4029E9-0x4029F4 (test ebx; test edi; cmp eax, [ebx+34h] = 52)
            this_->move_sprite = new_task->move_sprite;             // asm 0x4029F6-0x4029F9 (mov eax, [edi+34h] -> loc_402A18)
        else {
            RGE_Sprite* fetched = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x4029FB-0x402A04 (push obj; mov ecx, [eax+14h]; call [edx+78h] - slot 30 = +0x78, the M4 lowest-declaring-tier cast)
            if (this_->move_sprite == fetched)                      // asm 0x402A07-0x402A0A (cmp [esi+40h], eax)
                fetched = ((RGE_Master_Moving_Object*)source)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)source, (RGE_Static_Object*)this_->obj);  // asm 0x402A0C-0x402A15 (mov eax, [ebp+0]; push ecx; call [eax+78h] - the SOURCE's own vtbl dispatch)
            this_->move_sprite = fetched;                           // asm loc_402A18-0x402A1B (mov [esi+40h], eax)
        }
    }
    if (this_->fight_sprite) {                                      // asm 0x402A1B-0x402A20 (mov eax, [esi+44h]; test)
        if (task && new_task) {
            if (this_->fight_sprite == task->work_sprite) {         // asm 0x402A2A-0x402A2D (cmp eax, [ebx+38h] = 56)
                this_->fight_sprite = new_task->work_sprite;        // asm 0x402A2F-0x402A32 (mov edx, [edi+38h] -> the store -> jmp loc_402A6B)
            } else if (this_->fight_sprite == task->work_sprite2) { // asm 0x402A3B-0x402A3E (cmp eax, [ebx+3Ch] = 60)
                this_->fight_sprite = new_task->work_sprite2;       // asm 0x402A40-0x402A43 (mov eax, [edi+3Ch] -> loc_402A68)
            } else {
                RGE_Sprite* fetched = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x402A45-0x402A4E (call [edx+84h] - slot 33 = +0x84, the C2 lowest-declaring-tier cast)
                if (this_->fight_sprite == fetched)                 // asm 0x402A54-0x402A57
                    fetched = ((RGE_Master_Combat_Object*)source)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)source, (RGE_Static_Object*)this_->obj);  // asm 0x402A59-0x402A62 (the .c's vfptr[1].do_i_care_about_power render = the cast-form artifact, the documented C2 class)
                this_->fight_sprite = fetched;                      // asm loc_402A68-0x402A6B
            }
        } else {
            RGE_Sprite* fetched = ((RGE_Master_Combat_Object*)this_->obj->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm loc_402A45 (the task-pair-null join)
            if (this_->fight_sprite == fetched)
                fetched = ((RGE_Master_Combat_Object*)source)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)source, (RGE_Static_Object*)this_->obj);
            this_->fight_sprite = fetched;
        }
    }
    if (this_->wait_sprite) {                                       // asm 0x402A6B-0x402A70 (mov eax, [esi+48h]; test)
        if (task && new_task) {
            if (this_->wait_sprite == task->work_sprite) {          // asm 0x402A7A-0x402A7D (cmp eax, [ebx+38h])
                this_->wait_sprite = new_task->work_sprite;         // asm 0x402A7F-0x402A83 (mov edx, [edi+38h] -> the store -> the pops + retn)
            } else if (this_->wait_sprite == task->work_sprite2) {  // asm 0x402A90-0x402A93 (cmp eax, [ebx+3Ch])
                this_->wait_sprite = new_task->work_sprite2;        // asm 0x402A95-0x402A99 (mov eax, [edi+3Ch] -> the store -> the pops + retn)
            } else {
                RGE_Sprite* fetched = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm 0x402AA2-0x402AAB (call [edx+4] - slot 1 = +4)
                if (this_->wait_sprite == fetched)                  // asm 0x402AAE-0x402AB1
                    fetched = source->vfptr->get_sprite(source, (RGE_Static_Object*)this_->obj);  // asm 0x402AB3-0x402ABC
                this_->wait_sprite = fetched;                       // asm 0x402ABF (mov [esi+48h], eax)
            }
        } else {
            RGE_Sprite* fetched = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);  // asm loc_402AA2 (the task-pair-null join)
            if (this_->wait_sprite == fetched)
                fetched = source->vfptr->get_sprite(source, (RGE_Static_Object*)this_->obj);
            this_->wait_sprite = fetched;
        }
    }
}

// ===========================================================================
// Replacement for RGE_Action_Attack::ready_to_attack. Original:
// @ 0x004023B0 (idb_functions.h:16). Range [0x4023B0,0x4029C5) (lst
// :2398/:2999 = 1557 bytes, retn 4 = 1 stack arg), int return. ABI:
// __thiscall. NOT virtual (a code-level helper; the callers = first_in_
// stack + set_state + update, the direct calls). NO SEH.
// SEMANTIC: the attack-readiness probe (the rangeState out: 0 = too close,
// 1 = in range, 2 = too far). The dist = the FLT_MAX seed + the target
// distance; the head weaponRange <= 0.1f widens by -0.5; the formation
// gate (mFormationLocked == 1 exact + BUnitGroup::canMoveToAttack) ->
// return 2; the target branch: the visibility -> the state-5 drive /
// Failed, the position +-0.5 ladder -> the re-probe, the min_range ->
// rangeState 0 + the 510/(-1,-1,-1) notify, the range+0.5 probe ->
// rangeState 2 + the 509/(0,0,0) notify + the stateValue==1 sqrt chase,
// the group-8 Gate linked-child min-walk, the sync; the no-target branch:
// the sentinel ladder -> distance_to_position, the same min/probe blocks;
// the tail: rangeState = 1 -> turn_towards -> can_attack -> 1/0.
// RESTRUCTURE (owner-directed 2026-11-26, the goto elimination - the pass-2
// form on the suite-verified baseline): the three labels dissolved - FAILED
// (3 sites) inlined per site, IN_RANGE = the post-if/else convergence (the
// equality-true / band-within / dist2-sync / no-target falls), LADDER_REGION
// = the sequential fall (the AI-probe arms return; the ladder cases and
// owner <= 0 fall through). Path-exact: every gate, operand, store, and
// exit unchanged; the /Fa call multiset + the jcc forms re-verified at the
// gate; the brace-depth walk per stage.
int __fastcall RGE_Action_Attack__ready_to_attack_new(
    RGE_Action_Attack* this_, DWORD dummy, int* rangeState)
{
    float dist = FLT_MAX;                                              // asm 0x4023B6-0x4023B9 (mov [var_8], SINGLE_MAXIMUM_VALUE)
    if (this_->target_obj) {                                           // asm 0x4023BE-0x4023C3
        dist = (float)RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj);  // asm 0x4023C5-0x4023CE (fstp [var_8] - the m32 store)
    }
    double weaponRange = this_->obj->vfptr->weaponRange(this_->obj);   // asm 0x4023D2-0x4023DD (slot 101 = +0x194)
    if (X87_LE_NAN(weaponRange, 0.1f))                                 // asm 0x4023DD-0x4023E8 (fcomp flt_6545BC = 0.1f; test ah,41h; jz skip - the {<=, NaN} set)
        dist = dist - 0.5f;                                            // asm 0x4023EA-0x4023F4 (fld var_8; fsub const_f_half; fstp - THE HEAD SUBTRACTS 0.5)
    BUnitGroup* group = RGE_Game_World__getUnitGroup((RGE_Game_World*)this_->obj->owner->world, this_->obj->mGroupID);  // asm 0x4023F8-0x40240B (:6099)
    if (group && group->mFormationLocked == 1u && this_->target_obj) { // asm 0x402410-0x402422 (cmp dword ptr [eax+0F4h], 1 - the EXACT == 1)
        if (!BUnitGroup__canMoveToAttack(group, this_->obj->id, this_->target_obj->id)  // asm 0x402424-0x402436 (:?; the args (unitID, targetID))
            && (dist > (double)this_->range                            // asm 0x40243A-0x402443 (fld var_8; fcomp [esi+4Ch]; test ah,41h; jz skip - the {> ordered} -> the min check)
                || X87_LT_NAN(dist, (double)this_->min_range)          // asm 0x402448-0x402454 (fcomp [esi+50h]; test ah,1 - the C0-only {<, NaN})
                && this_->min_range > 0.1f)) {                         // asm 0x402456-0x402464 (fld [esi+50h]; fcomp 0.1f; test ah,41h; jnz skip)
            return 2;                                                  // asm loc_402466-0x40246F (mov eax, 2)
        }
    }
    if (this_->target_obj) {                                           // asm 0x402472-0x402477 (mov ecx,[esi+10h]; test ecx,ecx; jnz loc_402601)
        if (this_->target_obj->owner->id > 0) {                        // asm 0x402601-0x40260C (mov eax,[ecx+18h]; mov edx,[eax+0A0h]; test edx,edx; jle loc_40269F - the SIGNED > 0)
            unsigned __int8 vis = this_->target_obj->vfptr->get_visibility(this_->target_obj, (RGE_Player*)this_->obj->owner);  // asm 0x402612-0x402621 (slot 91 = +0x16C)
            if (vis == 0u || (vis == 128u && this_->target_obj->master_obj->fog_flag == 0u)) {
                // loc_402636-0x40269C: the AI probe - vis == 0 via the asm jz at
                // 0x402623; the 128 && !fog fall-through joins loc_402636. Both
                // arms return; the ladder-region cases fall through (the asm
                // jnz loc_40269F pair at 0x402627 / 0x402634).
                UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);
                if (ai->stateValue == 0u                               // asm loc_402636-0x402644 ([eax+169h])
                    && this_->obj->vfptr->maximumSpeed(this_->obj) > 0.0f  // asm 0x40264D-0x40265E (slot 98; fcomp const_f_zero; test ah,41h; jnz Failed - the {<= 0, NaN} -> Failed, the ordered > 0 continues = THE LITERAL > MATCH ROW; the 41h mask here vs the 40h masks at the three move-check sites - the per-site TEST MASK IMMEDIATE)
                    && ai->vfptr->canMoveToAttack(ai, this_->targetID) == 1) {  // asm 0x402660-0x402676 (slot 51; cmp eax, 1)
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO_2);  // asm 0x40267B-0x402681 (push 5)
                    return 0;                                          // asm 0x402684-0x40268A
                }
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm loc_40268D-0x402693 (push 0Dh = LABEL_72)
                return 0;                                              // asm 0x402696-0x40269C
            }
        }
        // the ladder region (loc_40269F): owner->id <= 0 falls here (asm jle
        // loc_40269F); the two visibility ladder cases land via the jnz pair.
        if (!(X87_EQ_NAN(this_->target_obj->world_x, this_->target_x)  // asm 0x40269F-0x4026AD (fld [ecx+48h]; fcomp [esi+20h]; test ah,40h; jz loc_4026CD - the {=, NaN} equal-continue)
            && X87_EQ_NAN(this_->target_obj->world_y, this_->target_y) // asm 0x4026AF-0x4026BA (the y pair)
            && X87_EQ_NAN(this_->target_obj->world_z, this_->target_z))) {  // asm 0x4026BC-0x4026C7 (the z pair; jnz loc_402982) - all-equal -> the tail
            double dx = (double)this_->target_obj->world_x - (double)this_->target_x;  // asm 0x4026CD-0x4026D2 (fld [ecx+48h]; fsub [esi+20h] - dx stays UNROUNDED on st0 for both compares, unlike dy/dz which are fst-stored)
            float dy = this_->target_obj->world_y - this_->target_y;       // asm 0x4026D3-0x4026D6 (fld [ecx+4Ch]; fsub [esi+24h]; fstp [esp+x])
            float dz = this_->target_obj->world_z - this_->target_z;       // asm 0x4026DD-0x4026E3 (fstp [esp+var_8])
            if (dx > 0.5f || X87_LT_NAN(dx, -0.5f)                         // asm 0x4026E7-0x4026FA (fcom const_f_half(0.5); test ah,41h; jz the dx>=-0.5 check; fcomp flt_6545C4(-0.5); test ah,1; jnz the re-probe - the C0-only {< -0.5, NaN})
                || dy > 0.5f || X87_LT_NAN(dy, -0.5f)                      // asm 0x402723-0x402743 (the dy pair)
                || dz > 0.5f || X87_LT_NAN(dz, -0.5f)) {                   // asm (the dz pair; the jz loc_402982 = the all-within TAIL - straight there, NO sync)
                float dist2 = (float)RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj);  // asm 0x40274D-0x402756 (fst [esp+var_8])
                if (X87_LT_NAN(dist2, (double)this_->min_range)) {         // asm 0x40275A-0x402762 (fcomp [esi+50h]; test ah,1; jz the range probe - the C0-only {<, NaN})
                    *rangeState = 0;                                       // asm 0x402768 ([ecx] = 0)
                    if (!this_->subActionValue)                            // asm 0x40276E-0x402773
                        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x402775-0x40278A (1FEh = 510, the (-1,-1,-1) trio)
                            510, -1, -1, -1);
                    if (!X87_EQ_NAN(this_->obj->vfptr->maximumSpeed(this_->obj), 0.0f)) {  // asm 0x402790-0x4027A6 (slot 98; test ah,40h; jnz Failed)
                        UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);
                        if (ai->vfptr->canMoveToAttack(ai, this_->targetID))  // asm 0x4027AC-0x4027C4 (slot 51; test)
                            return 0;                                      // asm 0x4027CA-0x4027D0
                    }
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);  // asm loc_402905-0x40290B (push 0Dh)
                    return 0;                                          // asm 0x40290E-0x402914
                }
                if (this_->target_obj->master_obj->object_group == 8u) {   // asm loc_402800-0x402855 (cmp word ptr [ecx+1Eh], 8 - THE GATE; the linked-child min walk)
                    for (int i = 0; i < 2; i++) {                          // asm 0x40280F-0x402855 (xor ebp; the 2-iteration loop)
                        int childId = this_->target_obj->vfptr->getLinkedChild(this_->target_obj, i);  // asm 0x402811-0x402823 (push i; call [eax+358h] - slot 214 = +0x358)
                        RGE_Static_Object* child = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, childId);  // asm 0x402829-0x40282C (mov edi, [edx+420h] = (*base_game)->world; :6086; the TRIBE_World family cast)
                        if (child) {                                       // asm 0x402831-0x402833
                            float d2 = (float)RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, child);  // asm 0x402835-0x402839
                            if (X87_LT_NAN(d2, (double)dist2))             // asm 0x40283E-0x402847 (fcom var_8 - NO-POP; test ah,1; jz discard)
                                dist2 = d2;                                // asm 0x402849 (fstp [esp+var_8] - the min update)
                        }
                    }
                }
                float probeRange = this_->range;                       // asm 0x4027D3-0x4027DF (mov [esp+x], [esi+4Ch])
                double wr2 = this_->obj->vfptr->weaponRange(this_->obj);  // asm 0x4027DF-0x4027E5 (slot 101 - the st0 PARKED while the flags are taken)
                if (X87_LE_NAN(wr2, 0.1f))                             // asm 0x4027EB-0x4027F0 (fcomp flt_6545BC; test ah,41h; jz skip)
                    probeRange = probeRange + 0.5f;                    // asm 0x4027F2-0x4027FC (fld [esp+x]; fadd const_f_half; fstp)
                if (dist2 > (double)probeRange) {                      // asm loc_402859-0x402866 (fld var_8; fcomp [esp+x]; test ah,41h; jnz the sync - the {> ordered} fall = the too-far)
                    *rangeState = 2;                                   // asm 0x402870 ([ecx] = 2)
                    if (!this_->subActionValue)                        // asm 0x402876-0x40287B
                        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x40287D-0x402892 (1FDh = 509, the (0,0,0) trio)
                            509, 0, 0, 0);
                    bool chaseable = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)->stateValue != 1u;  // asm 0x402898-0x4028A7 (cmp byte ptr [eax+169h], 1; jnz the move check)
                    if (!chaseable) {                                  // asm (the stateValue == 1 sqrt chase)
                        float dxs = this_->target_obj->world_x - RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)->statePositionXValue;  // asm 0x4028B1-0x40228BD (fld [eax+16Ch] = statePositionXValue; fsubr [ecx+48h])
                        float dys = this_->target_obj->world_y - RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)->statePositionYValue;  // asm 0x4028C9-0x40228D5 (fld [eax+170h] = statePositionYValue; fsubr [edx+4Ch])
                        float chaseDist = (float)sqrt((double)dxs * (double)dxs + (double)dys * (double)dys);  // asm 0x4028DA-0x40228EE (fld st; fmulp; fld; fmul; faddp; fsqrt; fstp)
                        double wr3 = this_->obj->vfptr->weaponRange(this_->obj);  // asm 0x40228EE-0x40228F4 (slot 101)
                        chaseable = wr3 + 5.0f > (double)chaseDist;    // asm 0x40228F4-0x4022903 (fadd ds:flt_6545C0 = 5.0f; fcomp; test ah,41h; jz the move - the {> ordered} fall = Failed - the literal > MATCH row)
                    }
                    if (chaseable) {                                   // asm loc_402917-0x40294F
                        if (!X87_EQ_NAN(this_->obj->vfptr->maximumSpeed(this_->obj), 0.0f)) {  // asm 0x402917-0x40292D (slot 98; test ah,40h; jnz Failed)
                            UnitAIModule* ai2 = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);
                            if (ai2->vfptr->canMoveToAttack(ai2, this_->targetID))  // asm 0x40292F-0x402947 (slot 51; test)
                            {
                                this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);  // asm 0x402949-0x40294F (push 4)
                                return 0;                              // asm 0x402952-0x402958
                            }
                        }
                        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);  // asm loc_40295B-0x402961 (push 0Dh)
                        return 0;                                      // asm 0x402964-0x40296A
                    }
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);          // asm loc_40295B-0x402961 (push 0Dh - the stateValue == 1 && sqrt >= weaponRange+5 fall: FAILED, never the sync)
                    return 0;                                                          // asm 0x402964-0x40296A
                } else {
                    this_->target_x = this_->target_obj->world_x;          // asm loc_40296D (the {<=, NaN} jnz -> the sync)
                    this_->target_y = this_->target_obj->world_y;
                    this_->target_z = this_->target_obj->world_z;
                }
            }
            // all within +-0.5: asm 0x402743 (jz loc_402982) -> straight to the tail, NO sync
            // (the ladder-else sync DELETED: loc_40296D's only inbound xref is 0x402866, the dist2 arm)
        }
        // the all-equal path: the stored position is still exact -> the tail, no update
    } else {
        float x = this_->target_x;                                     // asm 0x40247D-0x402480
        if (X87_EQ_NAN(x, -1.0f)) {                                    // asm 0x402484-0x40248E (fld x; fcomp flt_6545C8 = -1.0f; test ah,40h; jnz the lastX block)
            if (X87_EQ_NAN(this_->lastTargetPositionX, -1.0f)) {       // asm loc_4024A3-0x4024B2 (fld [esi+5Ch]; fst x; fcomp -1.0; test ah,40h; jnz Failed)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm loc_402905-0x40290B (push 0Dh)
                return 0;                                              // asm 0x40290E-0x402914
            }
            dist = (float)RGE_Static_Object__distance_to_position((RGE_Static_Object*)this_->obj, this_->lastTargetPositionX, this_->lastTargetPositionY, this_->lastTargetPositionZ);  // asm 0x4024BB-0x4024CB
        } else {
            dist = (float)RGE_Static_Object__distance_to_position((RGE_Static_Object*)this_->obj, this_->target_x, this_->target_y, this_->target_z);  // asm 0x40249B-0x4024CB (push z/y/x; fst [var_8])
        }
        if (X87_LT_NAN(dist, (double)this_->min_range)) {              // asm 0x4024D4-0x4024DC (fcomp [esi+50h]; test ah,1; jz the range probe)
            *rangeState = 0;                                           // asm 0x4024E2
            if (!this_->subActionValue)                                // asm 0x4024E8-0x4024ED
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x4024EF-0x402504 (1FEh = 510, the (-1,-1,-1) trio)
                    510, -1, -1, -1);
            if (!X87_EQ_NAN(this_->obj->vfptr->maximumSpeed(this_->obj), 0.0f)) {  // asm 0x40250A-0x40251D (slot 98; test ah,40h; jnz Failed)
                UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);
                if (ai->vfptr->canMoveToAttack(ai, this_->targetID))   // asm 0x402526-0x40253E (slot 51; test)
                    return 0;                                          // asm 0x402544-0x40254A
            }
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm loc_402905-0x402914 (the Failed tail, INLINED)
            return 0;
        }
        float probeRange = this_->range;                               // asm 0x40254D-0x402553 (mov [esp+x], [esi+4Ch])
        double wr2 = this_->obj->vfptr->weaponRange(this_->obj);       // asm 0x402559-0x40255F (slot 101 - the st0 parked)
        if (X87_LE_NAN(wr2, 0.1f))                                     // asm 0x402565-0x402570 (fcomp flt_6545BC; the fnstsw AFTER the fld [esp+x] - the flags persist across the fld)
            probeRange = probeRange + 0.5f;                            // asm 0x402570-0x402576 (fadd const_f_half)
        if (dist > (double)probeRange) {                               // asm loc_402576-0x402583 (fld var_8; fcomp st(1); test ah,41h; fstp st; jnz loc_402982 = the in-range)
            *rangeState = 2;                                           // asm loc_402589-0x40258D ([eax] = 2)
            if (!this_->subActionValue)                                // asm 0x402593-0x402598
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id,  // asm 0x40259A-0x4025AF (1FDh = 509, the (0,0,0) trio)
                    509, 0, 0, 0);
            if (!X87_EQ_NAN(this_->obj->vfptr->maximumSpeed(this_->obj), 0.0f)) {  // asm loc_4025B5-0x4025CB (slot 98; test ah,40h; jnz Failed)
                UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);
                if (ai->vfptr->canMoveToAttack(ai, this_->targetID)) {  // asm 0x4025D1-0x40253E (slot 51)
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);  // asm 0x4025EF-0x4025F5 (push 4)
                    return 0;                                          // asm 0x4025F8-0x4025FE
                }
            }
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm loc_402905-0x402914 (the Failed tail, INLINED)
            return 0;
        }
    }
    *rangeState = 1;                                                   // asm loc_402982-0x402986 ([ecx] = 1)
    if (!this_->obj->vfptr->turn_towards(this_->obj, this_->target_obj, // asm 0x40298C-0x40299D (slot 221 = +0x374; the args (target_obj, target_x, target_y))
        this_->target_x, this_->target_y))
        return 0;                                                      // asm 0x4029A3-0x4029AD
    return this_->obj->vfptr->can_attack(this_->obj) != 0u;            // asm loc_4029B0-0x4029C2 (slot 61 = +0xF4; neg/sbb = the 1/0 return)
}

void install_RGE_Action_Attack()
{
    setHook((void*)0x00401410, RGE_Action_Attack__destructor_new);
    setHook((void*)0x00401130, RGE_Action_Attack__vector_deleting_destructor_new);
    setHook((void*)0x00401610, RGE_Action_Attack__stop_new);
    setHook((void*)0x00402AD0, RGE_Action_Attack__set_target_obj_new);
    setHook((void*)0x00401530, RGE_Action_Attack__first_in_stack_new);
    setHook((void*)0x004029D0, RGE_Action_Attack__copy_obj_sprites_new);
    setHook((void*)0x00401000, RGE_Action_Attack__RGE_Action_Attack3_new);
    setHook((void*)0x00401150, RGE_Action_Attack__RGE_Action_Attack_new);
    setHook((void*)0x004012E0, RGE_Action_Attack__RGE_Action_Attack2_new);
    setHook((void*)0x00401420, RGE_Action_Attack__save_new);
    setHook((void*)0x00402290, RGE_Action_Attack__work_new);
    setHook((void*)0x00401620, RGE_Action_Attack__set_state_new);
    setHook((void*)0x004023B0, RGE_Action_Attack__ready_to_attack_new);
    setHook((void*)0x00401970, RGE_Action_Attack__update_new);
}
