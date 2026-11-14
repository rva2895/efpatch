#include "stdafx.h"
#include "RGE_Master_Action_Object.h"

// .rdata vftable of RGE_Master_Action_Object
// (battlegrounds_x2.lst: .rdata:0065ADFC RGE_Master_Action_Object__vftable)
static RGE_Master_Action_ObjectVtbl* const rge_master_action_object_vftable =
    (RGE_Master_Action_ObjectVtbl*)0x0065ADFC;

// Replacement for RGE_Master_Action_Object::setup
// Original: RGE_Master_Action_Object__setup @ 0x00487B50 (idb_functions.h:1790).
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), int result (always 1).
// Derived override: base Moving setup call (through the idb constant —
// routes into the replacement per the HOOK-ROUTING RULING), master_type
// forced to RGE_MASTER_TYPE_ACTION (0x28), then the eleven Action
// extension fields copied from other_object (tasks @260 copied last,
// asm 0x487BE0-0x487BE6). Extension offsets 260-296 verified by the
// mechanical walker (calibrated on master_type@4 / selected_sound@72 /
// outline@196-204 / speed@208 / the Moving block 212-256).
int __fastcall RGE_Master_Action_Object__setup_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    RGE_Master_Action_Object* other_object)
{
    RGE_Master_Moving_Object__setup((RGE_Master_Moving_Object*)this_, (RGE_Master_Moving_Object*)other_object);
    this_->master_type = RGE_MASTER_TYPE_ACTION;
    this_->default_task = other_object->default_task;
    this_->search_radius = other_object->search_radius;
    this_->work_rate = other_object->work_rate;
    this_->gbg_work_rate_2 = other_object->work_rate;
    this_->drop_site = other_object->drop_site;
    this_->backup_drop_site = other_object->backup_drop_site;
    this_->task_by_group = other_object->task_by_group;
    this_->command_sound = other_object->command_sound;
    this_->move_sound = other_object->move_sound;
    this_->run_pattern = other_object->run_pattern;
    this_->tasks = other_object->tasks;
    return 1;
}

// Replacement for RGE_Master_Action_Object::setup2
// Original: RGE_Master_Action_Object__setup2 @ 0x00487C00 (idb_functions.h:1791).
// ABI: __thiscall — this_ in ECX, 3 stack args (retn 0Ch), int result (always 1).
// Base call, master_type forced, then the stream sequence: default_task
// (2), search_radius (4), work_rate (4, gbg_work_rate_2 mirrors it
// immediately), drop_site (2), backup_drop_site (2), task_by_group (1),
// two i16 sound indices (the original reuses the sounds/sprites PARAM
// SLOTS as the read buffers — asm 0x487C84/0x487C91; named locals here;
// both resolutions index the SOUNDS array — asm uses ebp=sounds for
// both), run_pattern (1), then the guarded sound resolutions and the
// task-list binding: tasks = (*base_game)->world->tasks[id]
// (world->tasks @0x18C — asm + shifted-walker adjacency; see ledger).
int __fastcall RGE_Master_Action_Object__setup2_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Moving_Object__setup2((RGE_Master_Moving_Object*)this_, infile, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_ACTION;
    rge_read(infile, &this_->default_task, sizeof(this_->default_task));
    rge_read(infile, &this_->search_radius, sizeof(this_->search_radius));
    rge_read(infile, &this_->work_rate, sizeof(this_->work_rate));
    this_->gbg_work_rate_2 = this_->work_rate;
    rge_read(infile, &this_->drop_site, sizeof(this_->drop_site));
    rge_read(infile, &this_->backup_drop_site, sizeof(this_->backup_drop_site));
    rge_read(infile, &this_->task_by_group, sizeof(this_->task_by_group));
    __int16 command_sound_ix;
    __int16 move_sound_ix;
    rge_read(infile, &command_sound_ix, sizeof(command_sound_ix));
    rge_read(infile, &move_sound_ix, sizeof(move_sound_ix));
    rge_read(infile, &this_->run_pattern, sizeof(this_->run_pattern));
    this_->command_sound = (command_sound_ix < 0) ? 0 : sounds[command_sound_ix];
    this_->move_sound = (move_sound_ix < 0) ? 0 : sounds[move_sound_ix];
    this_->tasks = (*base_game)->world->tasks[this_->id];
    return 1;
}

// Replacement for RGE_Master_Action_Object::setup3
// Original: RGE_Master_Action_Object__setup3 @ 0x00487D10 (idb_functions.h:1792).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call (the IDA "possibly undefined" v19/v20 args were a tracking
// artifact — the asm forwards the real sounds/in_id), master_type
// forced, then ONE fscanf "%f %f %hd %hd %hd %hd %hd %hd" (asm
// 0x487D31-0x487D6B): the %hd scratch targets are the in_id/sprites/
// infile/sounds PARAM SLOTS (original) — named locals here; search_radius
// / work_rate / drop_site / backup_drop_site read directly. gbg mirrors
// work_rate; the byte scratches narrow on store; the two guarded sound
// resolutions index the SOUNDS array; then the default-task search:
// tasks = world->tasks[id], default_task = -1, first is_default task in
// the list wins. RESTRUCTURE (documented below): the original's
// list_num <= 0 guard is provably subsumed by the loop condition. The
// exhausted-loop path returns 1 explicitly (asm 0x487E0F mov eax,1) —
// the pseudocode's bare `return;` was a rendering.
int __fastcall RGE_Master_Action_Object__setup3_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id)
{
    RGE_Master_Moving_Object__setup3((RGE_Master_Moving_Object*)this_, infile, sprites, sounds, in_id);
    this_->master_type = RGE_MASTER_TYPE_ACTION;
    __int16 task_by_group_s; __int16 run_pattern_s;
    __int16 command_sound_ix; __int16 move_sound_ix;
    // the original reuses the in_id/sprites/infile/sounds param slots as
    // the %hd scratch buffers (asm 0x487D31-0x487D6B); named locals here
    fscanf_internal(infile, "%f %f %hd %hd %hd %hd %hd %hd",
        &this_->search_radius, &this_->work_rate, &this_->drop_site,
        &this_->backup_drop_site, &task_by_group_s, &run_pattern_s,
        &command_sound_ix, &move_sound_ix);
    this_->gbg_work_rate_2 = this_->work_rate;
    this_->task_by_group = (unsigned __int8)task_by_group_s;
    this_->run_pattern = (unsigned __int8)run_pattern_s;
    this_->command_sound = (command_sound_ix < 0) ? 0 : sounds[command_sound_ix];
    this_->move_sound = (move_sound_ix < 0) ? 0 : sounds[move_sound_ix];
    this_->default_task = -1;
    RGE_Task_List* task_list = (*base_game)->world->tasks[this_->id];
    this_->tasks = task_list;
    // RESTRUCTURE: the original's list_num <= 0 guard (asm 0x487DF0-0x487DF6,
    // movsx list_num / test / jle to the epilogue) is provably subsumed by
    // the loop condition — index < list_num fails for every list_num <= 0,
    // so the collapsed loop has the exact same path set: default_task stays
    // -1 and tasks is still assigned.
    for (int index = 0; index < task_list->list_num; ++index)
    {
        if (task_list->list[index]->is_default)
        {
            this_->default_task = (__int16)index;
            break;
        }
    }
    return 1;
}

// Replacement for RGE_Master_Action_Object::setup4
// Original: RGE_Master_Action_Object__setup4 @ 0x00487E30 (idb_functions.h:1793).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, then list[copy_id] (copy_id was stream-read by the base
// setup4; UNCONDITIONAL sign-extended index, no negative guard —
// faithful). BASE-POINTER CORRECTION per the template rule: the IDA
// v6[1].field accesses at +0x108..+0x128 are the referenced master's
// Action extension fields (offsets 264-296, walker-confirmed); the list
// element is re-typed RGE_Master_Action_Object* accordingly. The ten
// extension fields copy when the reference is non-null (gbg_work_rate_2
// from work_rate again); then tasks = world->tasks[id] (OVERWRITING the
// base-copied tasks — later write wins) and the stream tail re-reads
// search_radius (4) / work_rate (4) — also overwriting the copied
// values — with gbg mirroring work_rate again (asm 0x487EDD-0x487F1F).
int __fastcall RGE_Master_Action_Object__setup4_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Moving_Object__setup4((RGE_Master_Moving_Object*)this_, infile, list, sprites, sounds);
    RGE_Master_Action_Object* other = (RGE_Master_Action_Object*)list[this_->copy_id];
    this_->master_type = RGE_MASTER_TYPE_ACTION;
    if (other)
    {
        this_->default_task = other->default_task;
        this_->search_radius = other->search_radius;
        this_->work_rate = other->work_rate;
        this_->gbg_work_rate_2 = other->work_rate;
        this_->drop_site = other->drop_site;
        this_->backup_drop_site = other->backup_drop_site;
        this_->task_by_group = other->task_by_group;
        this_->command_sound = other->command_sound;
        this_->move_sound = other->move_sound;
        this_->run_pattern = other->run_pattern;
    }
    this_->tasks = (*base_game)->world->tasks[this_->id];
    rge_read(infile, &this_->search_radius, sizeof(this_->search_radius));
    rge_read(infile, &this_->work_rate, sizeof(this_->work_rate));
    this_->gbg_work_rate_2 = this_->work_rate;
    return 1;
}

// Replacement for RGE_Master_Action_Object::RGE_Master_Action_Object2
// Original: RGE_Master_Action_Object__RGE_Master_Action_Object2 @ 0x004879E0
// (idb_functions.h:1787). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), returns this. SEH ctor-unwind frame folded per the
// make_new_obj pattern: on exception the MOVING destructor runs on this
// (the immediate base — its dtor chains through Animated to Static) and
// the exception propagates (asm unwind chunk 0x641F50: mov ecx,
// [ebp-10h] / jmp RGE_Master_Moving_Object__destructor; handler =
// __CxxFrameHandler dispatch, unwind-only scope). Base Moving ctor2
// called with do_setup = 0. The Action extension has no ctor defaults —
// everything is set by the setups; if (do_setup) the derived setup
// (Action::setup2 — which binds tasks to world->tasks[id] and sets
// master_type). The if (do_setup) is load-bearing (no guard collapse).
RGE_Master_Action_Object* __fastcall RGE_Master_Action_Object__RGE_Master_Action_Object2_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Action_Object* result = this_;
    __try
    {
        RGE_Master_Moving_Object__RGE_Master_Moving_Object2((RGE_Master_Moving_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = rge_master_action_object_vftable;
        if (do_setup)
            RGE_Master_Action_Object__setup2(this_, infile, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Moving_Object__destructor((RGE_Master_Moving_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Action_Object::RGE_Master_Action_Object3
// Original: RGE_Master_Action_Object__RGE_Master_Action_Object3 @ 0x00487A50
// (idb_functions.h:1788). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x641F70, same
// Moving-destructor action). Base Moving ctor3 with do_setup = 0;
// if (do_setup) Action::setup3.
RGE_Master_Action_Object* __fastcall RGE_Master_Action_Object__RGE_Master_Action_Object3_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    RGE_Master_Action_Object* result = this_;
    __try
    {
        RGE_Master_Moving_Object__RGE_Master_Moving_Object3((RGE_Master_Moving_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = rge_master_action_object_vftable;
        if (do_setup)
            RGE_Master_Action_Object__setup3(this_, infile, sprites, sounds, in_id);
        result = this_;
    }
    __except (RGE_Master_Moving_Object__destructor((RGE_Master_Moving_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Action_Object::RGE_Master_Action_Object4
// Original: RGE_Master_Action_Object__RGE_Master_Action_Object4 @ 0x00487AD0
// (idb_functions.h:1789). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x641F90, same
// Moving-destructor action — chunk confirmed). Base Moving ctor4 with
// do_setup = 0; if (do_setup) Action::setup4.
// Caller coverage: RGE_Master_Player__load_master_object/load_object
// call these ctors directly; the Combat ctor chain builds on them.
RGE_Master_Action_Object* __fastcall RGE_Master_Action_Object__RGE_Master_Action_Object4_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Action_Object* result = this_;
    __try
    {
        RGE_Master_Moving_Object__RGE_Master_Moving_Object4((RGE_Master_Moving_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = rge_master_action_object_vftable;
        if (do_setup)
            RGE_Master_Action_Object__setup4(this_, infile, list, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Moving_Object__destructor((RGE_Master_Moving_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Action_Object::destructor
// Original: RGE_Master_Action_Object__destructor @ 0x00487F30 (idb_functions.h:1794).
// ABI: __thiscall — this_ in ECX, no stack args, void return, no dummy.
// Action vftable store, then tail-jmp to the Moving destructor; the
// replacement calls it (call vs tail-jmp not observable). Base call
// routes through the idb constant -> hooked replacement per the
// HOOK-ROUTING RULING.
void __fastcall RGE_Master_Action_Object__destructor_new(RGE_Master_Action_Object* this_)
{
    this_->vfptr = rge_master_action_object_vftable;
    RGE_Master_Moving_Object__destructor((RGE_Master_Moving_Object*)this_);
}

// Replacement for RGE_Master_Action_Object::vector_deleting_destructor
// Original: RGE_Master_Action_Object__vector_deleting_destructor @ 0x004879C0.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), void* result (this).
// Destructor + operator_delete_internal on flags & 1, returning this
// (the asm calls the game's operator_delete; the wrapper is the
// game-side deleter per the Static scalar_deleting precedent).
void* __fastcall RGE_Master_Action_Object__vector_deleting_destructor_new(
    RGE_Master_Action_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Action_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Master_Action_Object::make_new_obj
// Original: RGE_Master_Action_Object__make_new_obj @ 0x00487FA0
// (idb_functions.h:1796). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), RGE_Static_Object* result.
// The runtime-object factory (same ladder as the Static/Animated/Moving
// make_new_obj replacements): recyclable @188 (asm 0x487FBE byte test)
// -> RGE_Game_World__recycle_object_in_to_game(owner->world, master_type)
// (owner->world @0x8C = 140, asm [ebx+8Ch]; idb_types.h:3696) -> guarded
// vtable dispatch (recycle_in_to_game slot @0x74 = member 29 of
// RGE_Static_ObjectVtbl, idb_types.h:3960; asm 0x487FF2 [edx+74h]) ->
// the recycled object; else operator_new(0x144) + the RGE_Action_Object
// runtime ctor (do_setup = 1; asm 0x48803D).
// ALLOCATION VERIFIED: sizeof(RGE_Action_Object) = 324 = 0x144 —
// sequential walk over idb_types.h:7423-7501 (ei @144 = 36 bytes;
// BVector = {float x,y,z} = 12, idb_types.h:1757-1762; tail
// Selected_Group_Info @322 -> 324).
// SEH ctor-unwind frame folded per the make_new_obj precedent (FUNCTION
// CHUNK 0x641FD0: mov eax,[ebp+4] / operator_delete — the allocation is
// freed on exception and the exception propagates).
RGE_Static_Object* __fastcall RGE_Master_Action_Object__make_new_obj_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    RGE_Action_Object* new_obj = 0;
    __try
    {
        if (this_->recyclable)
        {
            RGE_Static_Object* recycled =
                RGE_Game_World__recycle_object_in_to_game(owner->world, this_->master_type);
            if (recycled)
            {
                recycled->vfptr->recycle_in_to_game(recycled, (RGE_Master_Static_Object*)this_, owner, x, y, z);
                result = recycled;
            }
        }
        if (!result)
        {
            new_obj = (RGE_Action_Object*)operator_new_internal(sizeof(RGE_Action_Object));
            if (new_obj)
                result = (RGE_Static_Object*)RGE_Action_Object__RGE_Action_Object(new_obj, this_, owner, x, y, z, 1);
        }
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Action_Object::make_new_master
// Original: RGE_Master_Action_Object__make_new_master @ 0x00488070
// (idb_functions.h:1797). ABI: __thiscall — this_ in ECX, no stack args,
// RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0x12C) — sizeof(RGE_Master_
// Action_Object) = 300 = 0x12C walker-verified — null-checked, then the
// base Moving COPY-CTOR with do_setup = 0 (asm 0x4880AF), the Action
// vfptr store (asm 0x4880BC), the base MOVING setup — NOT Action::setup;
// the asm calls RGE_Master_Moving_Object__setup (0x4880C2; its internal
// setTurnRadiusOffTurnSpeed dispatch therefore fires with the ACTION
// vfptr already stored, exactly as the binary sequences it) — then
// master_type = 40 (asm 0x4880C7) and the ELEVEN Action extension copies
// in the asm's order (offsets 260-296; gbg_work_rate_2 takes the source's
// work_rate; tasks LAST, asm 0x488149).
// SEH fold per the Animated/Moving make_new_master precedent (FUNCTION
// CHUNK 0x641FF0: entry 1 operator_deletes the allocation; entry 2
// [0x641FFB] jmps RGE_Master_Moving_Object__destructor — the
// destructor-chained path folds into the same filter; the original only
// arms it after the base ctor returns, so exceptions during the ctor
// take the free-only path in both forms).
RGE_Master_Static_Object* __fastcall RGE_Master_Action_Object__make_new_master_new(
    RGE_Master_Action_Object* this_)
{
    RGE_Master_Action_Object* new_master =
        (RGE_Master_Action_Object*)operator_new_internal(sizeof(RGE_Master_Action_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Moving_Object__RGE_Master_Moving_Object(
                (RGE_Master_Moving_Object*)new_master, (RGE_Master_Moving_Object*)this_, 0);
            new_master->vfptr = rge_master_action_object_vftable;
            RGE_Master_Moving_Object__setup(
                (RGE_Master_Moving_Object*)new_master, (RGE_Master_Moving_Object*)this_);
            new_master->master_type = RGE_MASTER_TYPE_ACTION;
            new_master->default_task = this_->default_task;
            new_master->search_radius = this_->search_radius;
            new_master->work_rate = this_->work_rate;
            new_master->gbg_work_rate_2 = this_->work_rate;
            new_master->drop_site = this_->drop_site;
            new_master->backup_drop_site = this_->backup_drop_site;
            new_master->task_by_group = this_->task_by_group;
            new_master->command_sound = this_->command_sound;
            new_master->move_sound = this_->move_sound;
            new_master->run_pattern = this_->run_pattern;
            new_master->tasks = this_->tasks;
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Action_Object::copy_obj
// Original: RGE_Master_Action_Object__copy_obj @ 0x004881F0
// (idb_functions.h:1800). ABI: __thiscall — this_ in ECX, 1 stack arg
// (retn 4), void return. No SEH frame (plain function).
// Base Moving copy_obj call (idb constant -> our hooked replacement per
// the HOOK-ROUTING RULING), then the TEN Action extension copies in the
// asm's order (offsets 260-296; gbg_work_rate_2 from the SOURCE's
// work_rate; tasks LAST). BASE-POINTER CORRECTION per the template rule:
// the IDA renderings read source through RGE_Master_Static_Object*
// (source[1].obj_max/radius_x/radius_y/selected_sound/created_sound/
// death_sound/damage_sound/los = source + 208 + Static offsets 56/60/64/
// 72/76/80/84/52) — the byte offsets land on the Action extension fields
// @260-296 (walker-confirmed), so the source is re-typed
// RGE_Master_Action_Object*. WIDTH RESOLUTIONS from the asm: drop_site
// AND backup_drop_site are SEPARATE word moves (0x488230-0x488245; the
// pseudocode's single dword read was an IDA merge), and there is NO
// run_pattern copy (the original does not copy it here).
void __fastcall RGE_Master_Action_Object__copy_obj_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Moving_Object__copy_obj((RGE_Master_Moving_Object*)this_, source);
    RGE_Master_Action_Object* src = (RGE_Master_Action_Object*)source;
    this_->default_task = src->default_task;
    this_->search_radius = src->search_radius;
    this_->work_rate = src->work_rate;
    this_->gbg_work_rate_2 = src->work_rate;
    this_->drop_site = src->drop_site;
    this_->backup_drop_site = src->backup_drop_site;
    this_->task_by_group = src->task_by_group;
    this_->command_sound = src->command_sound;
    this_->move_sound = src->move_sound;
    this_->tasks = src->tasks;
}

// Replacement for RGE_Master_Action_Object::modify
// Original: RGE_Master_Action_Object__modify @ 0x00488290
// (idb_functions.h:1801). ABI: __thiscall — this_ in ECX, 2 stack args
// (retn 8), void. No SEH frame (plain function).
// flag == 13 -> work_rate (asm 0x4882BD, @272) AND gbg_work_rate_2
// (asm 0x4882C0, @276) both take the SAME raw amount dword; flag == 23
// (0x17) -> search_radius (asm 0x4882AE, @268); default -> the base
// Moving modify (idb constant -> hooked replacement per the
// HOOK-ROUTING RULING).
void __fastcall RGE_Master_Action_Object__modify_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 13)
    {
        this_->work_rate = amount;
        this_->gbg_work_rate_2 = amount;
    }
    else if (flag == 23)
    {
        this_->search_radius = amount;
    }
    else
    {
        RGE_Master_Moving_Object__modify((RGE_Master_Moving_Object*)this_, amount, flag);
    }
}

// Replacement for RGE_Master_Action_Object::modify_delta
// Original: @ 0x004882D0 (idb_functions.h:1802). Same ABI. flag 13 ->
// ONE x87 computation stored twice (fld/fadd/fst [ecx+110h]/fstp
// [ecx+114h], asm 0x4882FD-0x48830D); flag 23 -> search_radius += amount
// (fld/fadd/fstp 0x4882EA-0x4882F4); default -> Moving modify_delta.
void __fastcall RGE_Master_Action_Object__modify_delta_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 13)
    {
        float updated = amount + this_->work_rate;
        this_->work_rate = updated;
        this_->gbg_work_rate_2 = updated;
    }
    else if (flag == 23)
    {
        this_->search_radius = amount + this_->search_radius;
    }
    else
    {
        RGE_Master_Moving_Object__modify_delta((RGE_Master_Moving_Object*)this_, amount, flag);
    }
}

// Replacement for RGE_Master_Action_Object::modify_percent
// Original: @ 0x00488320 (idb_functions.h:1803). Same ABI. flag 13 ->
// one computation stored twice (fld/fmul/fst/fstp 0x48834D-0x48835D);
// flag 23 -> search_radius *= amount; default -> Moving modify_percent.
void __fastcall RGE_Master_Action_Object__modify_percent_new(
    RGE_Master_Action_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 13)
    {
        float scaled = amount * this_->work_rate;
        this_->work_rate = scaled;
        this_->gbg_work_rate_2 = scaled;
    }
    else if (flag == 23)
    {
        this_->search_radius = amount * this_->search_radius;
    }
    else
    {
        RGE_Master_Moving_Object__modify_percent((RGE_Master_Moving_Object*)this_, amount, flag);
    }
}

// Replacement for RGE_Master_Action_Object::small_save
// Original: @ 0x00488370 (idb_functions.h:1804). __thiscall, 1 stack arg
// (retn 4), void. No SEH. Base Moving small_save (hooked -> routes), then
// search_radius (asm 0x488386, w4) and work_rate (asm 0x488390, w4).
void __fastcall RGE_Master_Action_Object__small_save_new(
    RGE_Master_Action_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Moving_Object__small_save((RGE_Master_Moving_Object*)this_, outfile);
    rge_write(outfile, &this_->search_radius, sizeof(this_->search_radius));
    rge_write(outfile, &this_->work_rate, sizeof(this_->work_rate));
}

// Replacement for RGE_Master_Action_Object::save
// Original: @ 0x004883B0 (idb_functions.h:1805). Same ABI. Locals init -1
// (asm 0x4883B9 or eax,-1), base Moving save, then the sound ids are
// extracted from RGE_Sound::id @+0xC (asm 0x4883D6/0x4883E8; idb_types.h
// :2960-2967) into the id locals — the asm's 4-byte local stores land
// only in their LOW WORDS on the stream (w2 writes at 0x488452/0x48845F;
// the __int16 local form is byte-identical, per the Static save
// precedent). Nine writes in asm order: default_task w2, search_radius
// w4, work_rate w4, drop_site w2, backup_drop_site w2, task_by_group w1,
// command id w2, move id w2, run_pattern w1.
void __fastcall RGE_Master_Action_Object__save_new(
    RGE_Master_Action_Object* this_, DWORD dummy, int outfile)
{
    __int16 command_id = -1;
    __int16 move_id = -1;
    RGE_Master_Moving_Object__save((RGE_Master_Moving_Object*)this_, outfile);
    if (this_->command_sound)
        command_id = this_->command_sound->id;
    if (this_->move_sound)
        move_id = this_->move_sound->id;
    rge_write(outfile, &this_->default_task, sizeof(this_->default_task));
    rge_write(outfile, &this_->search_radius, sizeof(this_->search_radius));
    rge_write(outfile, &this_->work_rate, sizeof(this_->work_rate));
    rge_write(outfile, &this_->drop_site, sizeof(this_->drop_site));
    rge_write(outfile, &this_->backup_drop_site, sizeof(this_->backup_drop_site));
    rge_write(outfile, &this_->task_by_group, sizeof(this_->task_by_group));
    rge_write(outfile, &command_id, sizeof(command_id));
    rge_write(outfile, &move_id, sizeof(move_id));
    rge_write(outfile, &this_->run_pattern, sizeof(this_->run_pattern));
}

void install_RGE_Master_Action_Object()
{
    setHook((void*)0x00487B50, RGE_Master_Action_Object__setup_new);
    setHook((void*)0x00487C00, RGE_Master_Action_Object__setup2_new);
    setHook((void*)0x00487D10, RGE_Master_Action_Object__setup3_new);
    setHook((void*)0x00487E30, RGE_Master_Action_Object__setup4_new);
    setHook((void*)0x004879E0, RGE_Master_Action_Object__RGE_Master_Action_Object2_new);
    setHook((void*)0x00487A50, RGE_Master_Action_Object__RGE_Master_Action_Object3_new);
    setHook((void*)0x00487AD0, RGE_Master_Action_Object__RGE_Master_Action_Object4_new);
    setHook((void*)0x00487F30, RGE_Master_Action_Object__destructor_new);
    setHook((void*)0x004879C0, RGE_Master_Action_Object__vector_deleting_destructor_new);
    setHook((void*)0x00487FA0, RGE_Master_Action_Object__make_new_obj_new);
    setHook((void*)0x00488070, RGE_Master_Action_Object__make_new_master_new);
    setHook((void*)0x004881F0, RGE_Master_Action_Object__copy_obj_new);
    setHook((void*)0x00488290, RGE_Master_Action_Object__modify_new);
    setHook((void*)0x004882D0, RGE_Master_Action_Object__modify_delta_new);
    setHook((void*)0x00488320, RGE_Master_Action_Object__modify_percent_new);
    setHook((void*)0x00488370, RGE_Master_Action_Object__small_save_new);
    setHook((void*)0x004883B0, RGE_Master_Action_Object__save_new);
}
