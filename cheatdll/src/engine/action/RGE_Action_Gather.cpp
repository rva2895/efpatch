#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"
#include "farm.h"

extern int current_loaded_version;   // the patch-era DAT version gate (farm.cpp:5 / harbor.cpp)

// the farm.cpp helpers the adopted auto_rebuild_farm_new consumes (the
// VERBATIM ADOPTION - the bodies stay in farm.cpp):
void TRIBE_Player__use_farm_queue_new(TRIBE_Player* player, __int16 farm_obj_id);   // farm.cpp:302
int try_auto_queue_farm(TRIBE_Player* player, __int16 farm_obj_id);                 // farm.cpp:330
void pay_obj_cost(TRIBE_Player* player, __int16 obj_id);                            // farm.cpp:349
void __stdcall verify_food_resource(RGE_Static_Object* obj, __int16 attribute, bool keep_attribute);   // attribute_held.cpp:64
void* __stdcall new_farm_walk(RGE_Action_Gather* action_gather, RGE_Action_Move_To* action_move, int x, int y);   // farm.cpp:937
// the DIAG A-leg stub handlers (the retired set_state interior patches -
// re-hooked for the vanilla-trace build so _orig runs the PATCHED
// original, the pre-M6 baseline the bakes must reproduce):
void ctr2TransportFix4();   // aiunitcount.cpp:1108
void farm_walk();           // farm.cpp:957
void on_action_gather_set_state();   // attribute_held.cpp:125
void ctr2TransportFix5();   // aiunitcount.cpp:1127

// the RGE_Action_Gather vftable (.rdata:00654904, lst :986227; slot 0 =
// the SHARED family scalar deleting destructor; the tier owns no
// destructor of its own). THE STRUCT: the base Action mirror + the
// extension @+0x40 = 64: targetType @+0x40 (i32), numberStops @+0x44
// (u8), currentStop @+0x45 (u8), timePerStop @+0x48 (f32),
// timeAtCurrentStop @+0x4C (f32), lastStopX @+0x50 (u8), lastStopY
// @+0x51 (u8), have_worked @+0x52 (u8), farm_x @+0x54 (f32), farm_y
// @+0x58 (f32), gbg_unknown_1 @+0x5C (i32 - THE WORK2 GATHER-FLAG
// slot, the IR#3 grounding). sizeof = 96 = 0x60.
// THIS FILE = the COMPLETE Gather write (11 of 11 functions). Batches:
// M6 (2026-11-25): Gather3/Gather/Gather2/save/first_in_stack/set_state;
// M7 (this batch): update/get_move_sprite/get_wait_sprite/work +
// auto_rebuild_farm (the VERBATIM ADOPTION of the farm.cpp:362 handler -
// the canonical _new name transfers, the farm.cpp definition tombstoned).
// update is the two-pass bar's first action-family applicant (27 gotos /
// 13 back-edges): the body below is PASS 1, the aligned transcription
// (the .c structure 1:1, v-names preserved, value-level fixes only -
// the X87 macro gates, the timer mixed-precision forms, the bakes) -
// the suite-verified test artifact; PASS 2 (the restructure) follows on
// the verified baseline. BAKED this batch (owner-approved): farm.cpp:1009
// @ 0x404F54 (the user_released gate + Aqua), attribute_held.cpp:148
// @ 0x40560D (the gather early-full exit), farm.cpp:1007 @ 0x405FDE
// (the Aqua capture, no stop math), farm.cpp:1010 @ 0x405F38 (the work
// deposit-arm gate + Aqua).
static RGE_Action_GatherVtbl* const rge_action_gather_vftable =
    (RGE_Action_GatherVtbl*)0x00654904;

// THE FILE-WIDE PRECISE BRACKET (owner-directed desync fix, 2026-11-25):
// the original computes every float chain on the x87 at 53-bit; /fp:fast
// shrinks the float-typed chains to 24-bit (subss/addss/mulss) even
// through the explicit (double) casts (the incrementWaypoint lesson).
// THE DESYNC CLASS: set_state's multi-stop spread block (the min_x/min_y
// subs + the *2 spreads feeding ftol) and the ctor's farm-timer
// rate_x_val1 product - the 24-bit rounding flips the truncation/timer
// by ULPs and the gather walk diverges. Everything from Gather3 through
// update_new compiles precise (the work/update pair below nests inside;
// the get_wait/get_move/auto_rebuild tail has no float chains).
#pragma float_control(precise, on, push)

// Replacement for RGE_Action_Gather::RGE_Action_Gather3 (the read
// ctor). Original: @ 0x00404080 (idb_functions.h:40). Range
// [0x404080, 0x404160) (lst :5714/:5800, retn 8 = 2 stack args),
// RGE_Action_Gather* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Action_List__create_action2 +15F (the load path). The SEH: the
// trivial unwind (the chunk @0x6407B0). Decode (asm 0x404080-0x404153):
// RGE_Action::RGE_Action2 (the idb constant HOOKED -> routes), the NINE
// rge_reads (the Gather extension in the stream order): targetType
// @+0x40 (4), numberStops @+0x44 (1), currentStop @+0x45 (1),
// timePerStop @+0x48 (4), timeAtCurrentStop @+0x4C (4), lastStopX
// @+0x50 (1), lastStopY @+0x51 (1), farm_x @+0x54 (4), farm_y @+0x58
// (4) - ALL sizeof-exact; the vfptr (.rdata:00654904); action_type = 5
// (the GATHER registry value) staged between the reads; THEN
// have_worked @+0x52 = 0; if (state @+0xC == 7 || == 8) have_worked = 1
// (the mid-gather save/load state fixup); return this.
RGE_Action_Gather* __fastcall RGE_Action_Gather__RGE_Action_Gather3_new(
    RGE_Action_Gather* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x4040AA (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    rge_read(infile, &this_->targetType, sizeof(this_->targetType));          // asm 0x4040CA (@+0x40, 4)
    rge_read(infile, &this_->numberStops, sizeof(this_->numberStops));        // asm 0x4040D6 (@+0x44, 1)
    rge_read(infile, &this_->currentStop, sizeof(this_->currentStop));        // asm 0x4040E2 (@+0x45, 1)
    rge_read(infile, &this_->timePerStop, sizeof(this_->timePerStop));        // asm 0x4040EE (@+0x48, 4)
    rge_read(infile, &this_->timeAtCurrentStop, sizeof(this_->timeAtCurrentStop));   // asm 0x4040FA (@+0x4C, 4)
    rge_read(infile, &this_->lastStopX, sizeof(this_->lastStopX));            // asm 0x404106 (@+0x50, 1)
    rge_read(infile, &this_->lastStopY, sizeof(this_->lastStopY));            // asm 0x404112 (@+0x51, 1)
    rge_read(infile, &this_->farm_x, sizeof(this_->farm_x));                  // asm 0x40411E (@+0x54, 4)
    rge_read(infile, &this_->farm_y, sizeof(this_->farm_y));                  // asm 0x40412A (@+0x58, 4)
    this_->vfptr = rge_action_gather_vftable;                         // asm 0x4040BE
    this_->action_type = RGE_ACTION_TYPE_GATHER;                      // asm 0x4040C4 (mov word @+4, 5)
    this_->have_worked = 0;                                           // asm 0x404132 (mov byte [esi+52h], 0)
    if (this_->state == 7 || this_->state == 8)                       // asm 0x404136-0x40413C (cmp al, 7 / cmp al, 8 - the mid-gather state fixup)
        this_->have_worked = 1;                                       // asm 0x40413E (mov byte [esi+52h], 1)
    return this_;                                                     // asm 0x404146 (mov eax, esi)
}

// Replacement for RGE_Action_Gather::RGE_Action_Gather (the target
// ctor). Original: @ 0x00404160 (idb_functions.h:41). Range
// [0x404160, 0x404290) (lst :5809/:5922, retn 0Ch = 3 stack args),
// RGE_Action_Gather* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Action_List__create_task_action +207 (the OWNED anchor - routes)
// + RGE_Combat_Object__gather +12D (the OWNED gather_new - routes).
// The SEH: the trivial unwind (the chunk @0x6407D0). Decode (asm
// 0x404160-0x404280): RGE_Action::RGE_Action (:172 - HOOKED -> routes);
// task @+0x30; targetType @+0x40 = -1; gbg_unknown_1 @+0x5C = 0; the
// vfptr; action_type = 5; set_target_obj (target_obj_in) (:182 -
// HOOKED -> routes); IF the target: target_x/y/z = the target's
// world trio; targetType = the target's master object_group (movsx
// word @+0x1E); IF the target's master id @+0x18 == 50 (FARM - the
// owner-supplied registry): farm_x/farm_y = the target snapshot.
// THEN: numberStops = 1, currentStop = 0, timePerStop = 0.0f,
// timeAtCurrentStop = 0.0f, lastStopX/Y = 0, have_worked = 0;
// IF task && targetType == 7 (FARM - the object_group registry):
// numberStops = 4, currentStop = 0, THE FARM TIMER MATH: mult =
// attributes[task->attribute_type2 @+0x16] if != -1 else 1.0f
// (the RGE_PLAYER_ATTRIBUTE enum applies); timePerStop = (master->
// attribute_max_amount @+0x8C (i16 movsx) * mult) / (master->
// gbg_work_rate_2 @+0x114 = 276 * task->work_val1 @+0x1C = 28)
// * 0.25f (flt_654968 - the asm's fild/fmul/fmul/fdivp/fmul/fstp
// chain - the .c's exact render); return this.
RGE_Action_Gather* __fastcall RGE_Action_Gather__RGE_Action_Gather_new(
    RGE_Action_Gather* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in, RGE_Static_Object* target_obj_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x404186 (:172 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->task = task_in;                                        // asm 0x404195 ([esi+30h])
    this_->targetType = -1;                                       // asm 0x40419F (mov dword [esi+40h], 0FFFFFFFFh)
    this_->gbg_unknown_1 = 0;                                     // asm 0x4041A6 (mov [esi+5Ch], ebx = 0)
    this_->vfptr = rge_action_gather_vftable;                     // asm 0x4041A9
    this_->action_type = RGE_ACTION_TYPE_GATHER;                  // asm 0x4041AF (mov word @+4, 5)
    RGE_Action__set_target_obj((RGE_Action*)this_, target_obj_in);   // asm 0x4041B5 (:182 - HOOKED -> routes; the derived->base cast)
    if (target_obj_in)                                            // asm 0x4041BC
    {
        this_->target_x = target_obj_in->world_x;                 // asm 0x4041C1 ([edi+48h] -> [esi+20h])
        this_->target_y = target_obj_in->world_y;                 // asm 0x4041C7 ([edi+4Ch] -> [esi+24h])
        this_->target_z = target_obj_in->world_z;                 // asm 0x4041CD ([edi+50h] -> [esi+28h])
        this_->targetType = target_obj_in->master_obj->object_group;   // asm 0x4041D3-0x4041D7 (movsx word @+0x1E)
        // baked: farm.cpp:1008 @ 0x4041DD - the capture gate extends to the
        // Aqua Harvester master (199), version-gated
        if (target_obj_in->master_obj->id == 50 /* Farm */                 // asm 0x4041DD (cmp word @+0x18, 32h - FARM)
            || (target_obj_in->master_obj->id == AQUA_HARVESTER_ID && current_loaded_version >= 8)) // asm (the stub's second cmp + the version gate)
        {
            this_->farm_x = this_->target_x;                      // asm 0x4041EA ([esi+54h])
            this_->farm_y = this_->target_y;                      // asm 0x4041ED ([esi+58h])
        }
    }
    this_->numberStops = 1;                                       // asm 0x4041F3 (mov byte [esi+44h], 1)
    this_->currentStop = 0;                                       // asm 0x4041F9 (mov [esi+45h], bl)
    this_->timePerStop = 0.0f;                                    // asm 0x4041FC (mov [esi+48h], ebx)
    this_->timeAtCurrentStop = 0.0f;                              // asm 0x4041FF (mov [esi+4Ch], ebx)
    this_->lastStopX = 0;                                         // asm 0x404202 (mov [esi+50h], bl)
    this_->lastStopY = 0;                                         // asm 0x404205 (mov [esi+51h], bl)
    this_->have_worked = 0;                                       // asm 0x404208 (mov [esi+52h], bl)
    if (this_->task && this_->targetType == 7)                    // asm 0x40420B-0x404211 (the FARM object_group)
    {
        float mult = 1.0f;                                        // asm 0x404213 (fld const_f_one)
        this_->numberStops = 4;                                   // asm 0x404219 (mov byte [esi+44h], 4)
        this_->currentStop = 0;                                   // asm 0x40421D
        if (this_->task->attribute_type2 != -1)                   // asm 0x404220-0x404228 (cmp cx, 0FFFFh)
            mult = this_->obj->owner->attributes[this_->task->attribute_type2];   // asm 0x40422D-0x40423B (the attributes read @+0xAC; the fstp st discards the 1.0)
        // THE FARM TIMER: timePerStop = attr_max * mult / (rate * val1) * 0.25f
        int attr_max = this_->obj->master_obj->attribute_max_amount;   // asm 0x40423E-0x40424B (movsx word @+0x8C = 140; fild)
        const double rate_x_val1 = (double)this_->obj->master_obj->gbg_work_rate_2 * (double)this_->task->work_val1;   // asm 0x404255-0x40425B (fld @+0x114 = 276; fmul work_val1 @+0x1C = 28 - the product EXACT at 53-bit; the float-typed local rounded it to 24-bit - THE DESYNC CLASS; /Fa: cvtps2pd x2 + mulsd)
        this_->timeAtCurrentStop = 0.0f;                          // asm 0x40425E (mov [esi+4Ch], ebx)
        this_->timePerStop = (float)((double)attr_max * mult / rate_x_val1 * 0.25);   // asm 0x404261-0x404269 (fdivp; fmul flt_654968 = 0.25f; fstp timePerStop @+0x48)
    }
    return this_;                                                 // asm 0x404272 (mov eax, esi)
}

// Replacement for RGE_Action_Gather::RGE_Action_Gather2 (the
// coordinate ctor). Original: @ 0x00404290 (idb_functions.h:42).
// Range [0x404290, 0x404300) (lst :5931/:5972, retn 14h = 5 stack
// args), RGE_Action_Gather* return. ABI: __thiscall. NOT virtual.
// Caller: RGE_Action_List__create_task_action +23C (the OWNED anchor -
// routes). NO SEH (plain). Decode (asm 0x404290-0x4042F6): RGE_Action::
// RGE_Action (:172 - HOOKED -> routes); task @+0x30, target_x @+0x20,
// target_y @+0x24; gbg_unknown_1 = 0, currentStop = 0, timePerStop = 0,
// timeAtCurrentStop = 0, lastStopX/Y = 0, have_worked = 0 (the asm
// order: the clears BEFORE the vfptr/type stores); the vfptr;
// action_type = 5; target_z @+0x28; targetType = -1; farm_x/farm_y =
// -1.0f (mov ecx, -1.0 staged); numberStops = 1; return this.
RGE_Action_Gather* __fastcall RGE_Action_Gather__RGE_Action_Gather2_new(
    RGE_Action_Gather* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in, float x, float y, float z)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x40429A (:172 - HOOKED -> routes)
    this_->task = task_in;                                        // asm 0x402A7-region: 0x4042A7 ([esi+30h])
    this_->target_x = x;                                          // asm 0x4042B0 ([esi+20h])
    this_->target_y = y;                                          // asm 0x4042B3 ([esi+24h])
    this_->gbg_unknown_1 = 0;                                     // asm 0x4042BF ([esi+5Ch])
    this_->currentStop = 0;                                       // asm 0x4042C2 ([esi+45h])
    this_->timePerStop = 0.0f;                                    // asm 0x4042C5 ([esi+48h])
    this_->timeAtCurrentStop = 0.0f;                              // asm 0x4042C8 ([esi+4Ch])
    this_->lastStopX = 0;                                         // asm 0x4042CB ([esi+50h])
    this_->lastStopY = 0;                                         // asm 0x4042CE ([esi+51h])
    this_->have_worked = 0;                                       // asm 0x4042D1 ([esi+52h])
    this_->vfptr = rge_action_gather_vftable;                     // asm 0x4042D4
    this_->action_type = RGE_ACTION_TYPE_GATHER;                  // asm 0x4042DA (mov word @+4, 5)
    this_->target_z = z;                                          // asm 0x4042E0 ([esi+28h])
    this_->targetType = -1;                                       // asm 0x4042E3 (mov dword [esi+40h], 0FFFFFFFFh)
    this_->farm_x = -1.0f;                                        // asm 0x4042EA (mov ecx, -1.0 staged)
    this_->farm_y = -1.0f;                                        // asm 0x4042ED
    this_->numberStops = 1;                                       // asm 0x4042F0 (mov byte [esi+44h], 1)
    return this_;                                                 // asm 0x4042F4 (mov eax, esi)
}

// Replacement for RGE_Action_Gather::save. Original: @ 0x00404300
// (idb_functions.h:43). Range [0x404300, 0x404380) (lst :5981/:6030,
// retn 4 = 1 stack arg), void. ABI: __thiscall. Virtual: the Gather
// vftable (the DATA XREF lst :5981). NO SEH. Decode (asm 0x404300-
// 0x40437C): RGE_Action::save (:178 - the idb constant) THEN the NINE
// rge_writes (the Gather extension in the ctor3 stream order - the
// mirror): targetType @+0x40 (4), numberStops @+0x44 (1), currentStop
// @+0x45 (1), timePerStop @+0x48 (4), timeAtCurrentStop @+0x4C (4),
// lastStopX @+0x50 (1), lastStopY @+0x51 (1), farm_x @+0x54 (4),
// farm_y @+0x58 (4) - ALL sizeof-exact.
void __fastcall RGE_Action_Gather__save_new(
    RGE_Action_Gather* this_, DWORD dummy, int outfile)
{
    RGE_Action__save((RGE_Action*)this_, outfile);                        // asm 0x404309 (:178 @0x40AB50 - the idb constant)
    rge_write(outfile, &this_->targetType, sizeof(this_->targetType));            // asm 0x404315 (@+0x40, 4)
    rge_write(outfile, &this_->numberStops, sizeof(this_->numberStops));          // asm 0x404321 (@+0x44, 1)
    rge_write(outfile, &this_->currentStop, sizeof(this_->currentStop));          // asm 0x40432D (@+0x45, 1)
    rge_write(outfile, &this_->timePerStop, sizeof(this_->timePerStop));          // asm 0x404339 (@+0x48, 4)
    rge_write(outfile, &this_->timeAtCurrentStop, sizeof(this_->timeAtCurrentStop));   // asm 0x404345 (@+0x4C, 4)
    rge_write(outfile, &this_->lastStopX, sizeof(this_->lastStopX));              // asm 0x404351 (@+0x50, 1)
    rge_write(outfile, &this_->lastStopY, sizeof(this_->lastStopY));              // asm 0x40435D (@+0x51, 1)
    rge_write(outfile, &this_->farm_x, sizeof(this_->farm_x));                    // asm 0x404369 (@+0x54, 4)
    rge_write(outfile, &this_->farm_y, sizeof(this_->farm_y));                    // asm 0x404375 (@+0x58, 4)
}

// Replacement for RGE_Action_Gather::first_in_stack. Original:
// @ 0x00404380. Range [0x404380, 0x404490) (lst :6039/:6168, retn 4 =
// 1 stack arg), void. ABI: __thiscall. Virtual: the Gather vftable
// (the DATA XREF lst :6040). NO SEH. Decode (asm 0x404380-0x404488):
// the carried-attribute set (the Make precedent): task->attribute_type
// @+0x14 (i16) vs obj->attribute_type_held @+0x5C != -> set_attribute
// (type, 0.0f) (slot 62 = +0xF8 = 248 decimal); THE FARM-CROWDED CHECK:
// targetType @+0x40 == 7 (FARM - the object_group registry) &&
// target_obj && worker_num @+0x5F (u8) > 1 -> set_state(11 Move) +
// set_target_obj(0) -> return (the farm has enough workers - the
// gatherer walks away); ELSE IF first_time && target_obj: THE
// LINKED-OWNER REDIRECT (the Enter ctor precedent): target->master_obj
// ->master_type @+4 == 80 && target->linked_owner @+0x1DC != 0 -> the
// effective building = linked_owner; THE OWNER + DROP-SITE MATCH:
// the effective building's owner @+0x18 == obj->owner && (the
// effective building's master id @+0x18 == obj->master_obj->drop_site
// @+0x118 = 280 || == backup_drop_site @+0x11A = 282) && obj->
// attribute_amount_held @+0x54 > 0.0 (the literal > MATCH row: the
// {<= 0, NaN} -> Goto(4)) -> set_target_obj2(the building) (slot 23 =
// +0x5C) + set_target_obj(0) (slot 22 = +0x58) + the sentinel trio +
// set_state(8 Return); the mismatch -> set_state(4 Goto); no target ->
// set_state(3 Search); !first_time && no target -> set_state(3 Search).
void __fastcall RGE_Action_Gather__first_in_stack_new(
    RGE_Action_Gather* this_, DWORD dummy, unsigned __int8 first_time)
{
    __int16 attribute_type = this_->task->attribute_type;         // asm 0x4043A (mov ax, [eax+14h])
    // baked: attribute_held.cpp:143 @ 0x40438E - the original's held!=attr ->
    // set_attribute(attr, 0) block replaced by the version-gated
    // food-attribute verify (keep = true; the stub's push 1/push eax/push
    // ecx + call, resuming at 0x40439F)
    verify_food_resource((RGE_Static_Object*)this_->obj, attribute_type, true);       // asm 0x40438E-0x40439E
    if (this_->targetType == 7 && this_->target_obj               // asm 0x4039F-0x403AA (cmp dword @+0x40, 7; the FARM object_group; the target test)
        && this_->target_obj->worker_num > 1)                     // asm 0x403AC-0x403B0 (cmp byte [eax+5Fh], 1; jbe - the u8 > 1)
    {
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);    // asm 0x403B8 (11 = Move - the farm has enough workers)
        this_->vfptr->set_target_obj(this_, 0);                   // asm 0x403C1 (slot 22)
        return;                                                   // asm 0x403C6
    }
    if (first_time && this_->target_obj)                          // asm 0x403CD-0x403DA (the arg consumed; the target test)
    {
        RGE_Static_Object* effective = this_->target_obj;         // asm 0x4043E4 (mov edx, ecx)
        if (effective->master_obj->master_type == RGE_MASTER_TYPE_BUILDING   // asm 0x403E6 (cmp byte [eax+4], 50h)
            && ((TRIBE_Building_Object*)effective)->linked_owner) // asm 0x403EC-0x403F4 (mov eax, [ecx+1DCh] = 476; the Enter ctor precedent)
            effective = (RGE_Static_Object*)((TRIBE_Building_Object*)effective)->linked_owner;   // asm 0x403F6 (the linked-building redirect)
        if (effective->owner == this_->obj->owner                 // asm 0x40401-0x40404 (cmp [ecx+18h], [edi+18h])
            && (effective->master_obj->id == this_->obj->master_obj->drop_site   // asm 0x40406-0x40410 (mov dx, [edx+18h]; cmp dx, [eax+118h] = 280)
                || effective->master_obj->id == this_->obj->master_obj->backup_drop_site)   // asm 0x40409 (cmp dx, [eax+11Ah] = 282)
            && this_->obj->attribute_amount_held > 0.0f)          // asm 0x40422-0x40430 (fld @+0x54; fcomp const_f_zero; test ah,41h/jnz - the {<= 0,NaN} -> Goto(4) = the literal MATCH row)
        {
            this_->vfptr->set_target_obj2(this_, effective);      // asm 0x40437 (slot 23 = +0x5C; push ecx = the effective building)
            this_->vfptr->set_target_obj(this_, 0);               // asm 0x4043E (slot 22)
            this_->target_x = -1.0f;                              // asm 0x40443 (mov eax, -1.0)
            this_->target_y = -1.0f;                              // asm 0x40444D
            this_->target_z = -1.0f;                              // asm 0x404450
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_RETURN);   // asm 0x404457 (push 8 - 8 = Return)
            return;                                               // asm 0x40445A
        }
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);    // asm 0x404465 (4 = Goto - the drop-site mismatch)
        return;                                                   // asm 0x404468
    }
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);      // asm 0x404473/0x404481 (3 = Search - the no-target paths)
}

// Replacement for RGE_Action_Gather::set_state. Original: @ 0x00404490.
// Range [0x404490, 0x404EE0) = 2640 bytes (lst :6177/:7132, retn 4 = 1
// stack arg), void. ABI: __thiscall. Virtual: the Gather vftable (the
// DATA XREF lst :6177). THE SEH: the Move_To allocation-unwind fold
// (the chunk @0x6407F0). THE SWITCH-TABLE BYTE CHECK: byte_404DE8 =
// db 0,1,2,3,10,4,5,6,10,10,7,10,0,0,7 + the 0Ah filler + the tail
// ...,7,8,9,10,10,9 (the 247-entry table, state-1..247). THE ARMS:
// Done(1)/Failed(13)/Invalidated(14) -> timer = 0 + the wait-sprite
// restore; Wait(2) -> the wait sprite + the target clears + the
// sentinel trio; Search(3) -> the drop-off find + the 514/609 notify +
// timer = -10.0 (the target2 first, then the drop_site pair via the
// find, then the bare -1/0); Goto(4) -> the target sync + the AI-water
// check (the owner's object_group_count[59]/[17] + the lookupZone/
// currentZone pair -> the internal state F4 = 244) + numberStops == 1
// || quick_build -> the Move_To2 target form / else the 10-attempt
// lastStop dedup + rand spread -> the coordinate Move_To; Work(6) ->
// THE FARM CAPTURE (the group 7 Farm: the owner mismatch + gameType ==
// 6 && worker_num + the target field @+0x230 <= 0 -> Done(1); the vtbl
// +188 dispatch with the obj owner) + the multi-stop sprite; Work2(7)
// -> the carried-attribute set + the work sprite (the group 58/14
// civ-sprite indexing: world->sprites[gbg_get_civ_override(obj)]
// [work_sprite->id]) + RGE_Sound::play(work_sound); Return(8) ->
// currentStop = 0 + the drop-site resolve (the useSameZoneDropsite
// vtbl +0x1CC dispatch + the find pair with obj as the zone_obj) + the
// AI-water -> F7 = 247 + the Move_To2 to target_obj2 / the 515/609
// notify -> Failed(13); Move(11)/15/242 -> the SHARED coordinate
// Move_To arm (the timer = 0); state 243 -> the drop-off find (the
// timer = -1.0 variant); states 244/247 -> the wait-sprite-only;
// states 245/246/248/249 -> the DEFAULT (the bare state stores - the
// update reads them). THE F5/F8 GATE: delete_list SKIPPED (the
// sub-actions preserved across the internal transitions).
void __fastcall RGE_Action_Gather__set_state_new(
    RGE_Action_Gather* this_, DWORD dummy, unsigned __int8 new_state)
{
    if (new_state != 0xF5 && new_state != 0xF8)                   // asm 0x4044AE-0x4044B9 (cmp bl, 0F5h/jz; cmp bl, 0F8h/jz - the delete_list SKIP for the internal transitions)
        RGE_Action_List__delete_list(this_->sub_actions);         // asm 0x4044BE (:62 - FIRST)
    this_->state = new_state;                                     // asm 0x4044C3 ([esi+0Ch])
    switch (new_state)                                            // asm 0x4044C6-0x4044E2 (byte_404DE8/jpt_4044E2 - the partition verified state-by-state: 1/13/14->Done, 2->Wait, 3->Search, 4->Goto, 6->Work, 7->Work2, 8->Return, 11/15/242->Move, 243, 244/247)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_404D89
    case RGE_ACTION_STATE_FAILED:                                 // 13
    case RGE_ACTION_STATE_INVALIDATED:                            // 14
        this_->timer = 0.0f;                                      // asm 0x404D8C ([esi+2Ch] = 0)
        this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
    case RGE_ACTION_STATE_WAIT:                                   // 2 -> loc_4046A3
    {
        this_->timer = 0.0f;                                      // asm 0x4046A6
        RGE_Sprite* wait_sprite = RGE_Action_Gather__get_wait_sprite(this_);   // asm 0x4046B1
        this_->obj->vfptr->new_sprite(this_->obj, wait_sprite);   // asm 0x4046BA (slot 44)
        this_->vfptr->set_target_obj(this_, 0);                   // asm 0x4046C6 (slot 22)
        this_->vfptr->set_target_obj2(this_, 0);                  // asm 0x4046CF (slot 23)
        this_->target_x = -1.0f;                                  // asm 0x4046D2-0x4046DE (the sentinel trio)
        this_->target_y = -1.0f;
        this_->target_z = -1.0f;
        return; // asm 0x4046F1
    }
    case RGE_ACTION_STATE_SEARCH:                                 // 3 -> loc_4044E9 - the drop-off find
    {
        if (this_->target_obj2)                                   // asm 0x4044EF
        {
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, this_->target_obj2->id);   // asm 0x40450C-0x40450C (0x202 = 514, 0x261 = 609; p2 = targetType, p3 = the target2 id)
            this_->timer = -10.0f;                                // asm 0x404515 (mov dword [esi+2Ch], -10.0)
            this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
        }
        if (this_->obj->master_obj->drop_site == -1)              // asm 0x404526-0x404531 (cmp word [eax+118h], 0FFFFh)
        {
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, -1);   // asm 0x404533-0x40453A (p3 = -1)
            this_->timer = -10.0f;                                // asm 0x4045B8
            this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
        }
        RGE_Static_Object* found = RGE_Player_Object_List__find_by_master_ids(this_->obj->owner->objects, this_->obj->master_obj->drop_site, this_->obj->master_obj->backup_drop_site, this_->obj->world_x, this_->obj->world_y, 1, 2, 0);   // asm 0x40455C (the drop_site/backup pair; the zone_obj = 0)
        if (found)                                                // asm 0x404564
        {
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, found->id);   // asm 0x404581
            this_->timer = -10.0f;                                // asm 0x40458A
            this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
        }
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, 0);   // asm 0x4045A3-0x4045AF (p3 = 0 - the no-drop-site-found)
        this_->timer = -10.0f;                                    // asm 0x4045B8
        this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
    }
    case 243:                                                     // 0xF3 -> loc_4045C6 - the drop-off find (the timer -1.0 variant)
    {
        if (this_->target_obj2)                                   // asm 0x4045CE
        {
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, this_->target_obj2->id);   // asm 0x4045E9
            this_->timer = -1.0f;                                 // asm 0x4045F2
            this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
        }
        if (this_->obj->master_obj->drop_site == -1)              // asm 0x404603-0x40460E
        {
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, -1);   // asm 0x404615-0x404617
            this_->timer = -1.0f;                                 // asm 0x404692
            this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
        }
        RGE_Static_Object* found = RGE_Player_Object_List__find_by_master_ids(this_->obj->owner->objects, this_->obj->master_obj->drop_site, this_->obj->master_obj->backup_drop_site, this_->obj->world_x, this_->obj->world_y, 1, 2, 0);   // asm 0x404639
        if (found)                                                // asm 0x404641
        {
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, found->id);   // asm 0x40465E
            this_->timer = -1.0f;                                 // asm 0x404667
            this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
        }
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, 0);   // asm 0x40467C-0x40468C
        this_->timer = -1.0f;                                     // asm 0x404692
        this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
    }
    case 244:                                                     // 0xF4 -> loc_404699
    case 247:                                                     // 0xF7
        this_->obj->vfptr->new_sprite(this_->obj, RGE_Action_Gather__get_wait_sprite(this_));   // the wait-sprite restore (the get_wait_sprite -> new_sprite inline)
        return;
    case RGE_ACTION_STATE_MOVE:                                   // 11 -> loc_4046F4 (the SHARED coordinate Move_To arm)
    case 15:                                                      // 0x0F
    case 242:                                                     // 0xF2
    {
        this_->timer = 0.0f;                                      // asm 0x4046F6 (mov [esi+2Ch], ebx = 0)
        RGE_Action_Move_To* created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x4046FB (push 44h = 68)
        __try
        {
            if (created)                                          // asm 0x404709
            {
                RGE_Sprite* sprite = RGE_Action_Gather__get_move_sprite(this_, 0);   // asm 0x404714 (unk = 0)
                RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj, this_->target_x, this_->target_y, this_->target_z, this_->task->work_range, sprite);   // asm 0x404733 (:100 - HOOKED -> routes)
            }
        }
        __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH)   // the SEH chunk @0x6407F0
        {
        }
        if (!created)                                             // asm 0x40473E
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x40474C (13 = Failed)
            return; // asm 0x404756
        }
        RGE_Action__setSubAction((RGE_Action*)created, 1);        // asm 0x40476B (:190 - BEFORE the add_action, the asm order)
        RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm 0x404774 (:67)
        return; // asm 0x40478A
    }
    case RGE_ACTION_STATE_GOTO:                                   // 4 -> loc_40478D
    {
        this_->timer = 0.0f;                                      // asm 0x404790 ([esi+2Ch] = 0)
        if (!this_->target_obj)                                   // asm 0x404797
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x4047A1 (13 = Failed)
            return; // asm 0x4047B5
        }
        this_->target_x = this_->target_obj->world_x;             // asm 0x4047C1 ([eax+48h] -> [esi+20h])
        this_->target_y = this_->target_obj->world_y;             // asm 0x4047C4-0x4047C5 ([eax+4Ch] -> [esi+24h])
        this_->target_z = this_->target_obj->world_z;             // asm 0x4047C7-0x4047CA ([eax+50h] -> [esi+28h])
        if (this_->obj->owner->vfptr->computerPlayer(this_->obj->owner) == 1)   // asm 0x4047D2 (slot 7 = +0x1C = 28 decimal)
        {
            /* baked: aiunitcount.cpp:1359 @ 0x4047E0 - the ctr2 count-table
               re-base: the gate reads the ctr2 extended per-master count
               table through owner->object_category_count_new (the pointer
               member of the object_category_count union at TRIBE_Player
               +0x2F0 = 752, walker-verified; the owner's IDB union
               declaration) at master ids 1659/1617 - address-identical to
               the vanilla object_group_count[59]/[17] reads under the
               vanilla layout (3952+118 = 4070 = 752+3318) */
            __int16* ctrCounts = this_->obj->owner->object_category_count_new;   // asm 0x4047E0 (mov eax,[eax+2F0h])
            if (ctrCounts[1659] > 0                               // asm 0x4047E0-0x4047E8 (cmp word [eax+1659*2], 0; jg)
                || ctrCounts[1617] > 0)                           // asm 0x4047EA-0x4047F2 (cmp word [eax+1617*2], 0; jle skip)
            {
                // THE AI-WATER CHECK (the Goto arm's version)
                unsigned __int8 target_zone = RGE_Static_Object__lookupZone(this_->target_obj, (int)this_->target_obj->world_x, (int)this_->target_obj->world_y);   // asm 0x404810-0x404824 (the ftol pair y-then-x pushed = the (x, y) order)
                if (RGE_Static_Object__currentZone((RGE_Static_Object*)this_->obj) != target_zone)   // asm 0x40482E (the SELF slot call :? - the currentZone)
                {
                    this_->vfptr->set_state(this_, 244);          // asm 0x40483E (push 0F4h - the internal water-crossing state)
                    return; // asm 0x408-region: 0x404846
                }
            }
        }
        // THE MULTI-STOP/GATHER-MOVE (the shared continuation, loc_404857)
        if (this_->numberStops == 1                               // asm 0x404857 (cmp byte [esi+44h], 1; jz -> the Move_To2 target form)
            || (*base_game)->quick_build)                          // asm 0x404861-0x40486E (quick_build @+0x9E0 = 2528, the pointer-to-slot deref)
        {
            // THE MOVE_TO2 TARGET FORM
            RGE_Action_Move_To* created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm loc_40495A (push 44h)
            __try
            {
                if (created)                                      // asm 0x40496A
                {
                    RGE_Sprite* sprite = RGE_Action_Gather__get_move_sprite(this_, 0);   // asm 0x40497A (unk = 0)
                    RGE_Action_Move_To__RGE_Action_Move_To2(created, this_->obj, this_->target_obj, this_->task->work_range, sprite);   // asm 0x404991 (:101 - HOOKED -> routes)
                }
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH)
            {
            }
            if (!created)                                         // asm 0x40499C
            {
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x4049B2 (13 = Failed)
                return; // asm 0x4049B8
            }
            RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm loc_404CD7 (:67)
            RGE_Action__setSubAction((RGE_Action*)created, 1);    // asm 0x404CE4 (:190)
            return; // asm 0x404CFA
        }
        // THE MULTI-STOP RANDOM SPREAD (loc_404874): the subs and the *2
        // spreads compute at 53-bit then ftol (the operand casts + the
        // file-wide precise bracket; the 24-bit shrink flipped the tile
        // truncation - THE DESYNC CLASS)
        RGE_Master_Static_Object* master = this_->target_obj->master_obj;   // asm 0x40487B (mov edi, [ecx+14h])
        int min_x = (int)((double)this_->target_x - (double)master->radius_x);    // asm 0x404878-0x404881 (fld target_x; fsub radius_x @+0x3C; ftol; /Fa: cvtps2pd x2 + subsd + cvttsd2si)
        int min_y = (int)((double)this_->target_y - (double)master->radius_y);    // asm 0x404886-0x40488E (fld target_y; fsub radius_y @+0x40; ftol)
        int spread_x = (int)((double)master->radius_x * 2.0);               // asm 0x404893-0x40489C (fld radius_x; fadd st, st = *2; ftol - the *2 is exact at any precision)
        int spread_y = (int)((double)master->radius_y * 2.0);               // asm 0x4048A1-0x4048AA
        int new_x = this_->lastStopX;                             // asm 0x4048B3 (mov bl, [esi+50h])
        int new_y = this_->lastStopY;                             // asm 0x4048B6 (mov dl, [esi+51h])
        for (int attempt = 0; attempt < 10; attempt++)            // asm 0x4048C7-0x4048FB (the 10-attempt loop; the inc edi; cmp 0Ah)
        {
            if (new_x != this_->lastStopX                         // asm 0x4048CC-0x4048CE (cmp ebx, lastStopX; jnz loc_4048FD - EACH jnz is an independent exit term)
                || new_y != this_->lastStopY)                     // asm 0x4048D5-0x4048D7 (cmp edx, lastStopY; jnz loc_4048FD - exit when EITHER differs; the loop continues only while BOTH match)
                break;                                            // asm 0x4048FD (the found-a-different-stop break)
            new_x = min_x + rand_internal() % spread_x;           // asm 0x4048D9-0x4048E5 (call _rand; cdq; idiv spread_x; add)
            new_y = min_y + rand_internal() % spread_y;           // asm 0x4048E7-0x4048F5 (the same)
        }
        this_->lastStopX = (unsigned __int8)new_x;                // asm 0x40490A (mov [esi+50h], bl)
        this_->lastStopY = (unsigned __int8)new_y;                // asm 0x40490D (mov [esi+51h], dl)
        RGE_Action_Move_To* created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x40490D-region: 0x40490D
        __try
        {
            if (created)                                          // asm 0x40491B
            {
                /* baked: farm.cpp:1035 @ 0x404928 - the ctr2 farm-walk
                   offset: new_farm_walk builds the coordinate Move_To
                   itself (the v>=8 + radius>0.5 +0.5f farm-center offset,
                   task->work_range, get_move_sprite(1)) and the stub
                   resumes at the !created check (0x40499A) */
                new_farm_walk(this_, created, new_x, new_y);      // asm 0x404928-0x404999 (the stub's call replaces the get_move_sprite + Move_To ctor region)
            }
        }
        __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH)
        {
        }
        if (!created)                                             // asm loc_40499A
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x4049B2
            return; // asm 0x4049B8
        }
        RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm loc_404CD7
        RGE_Action__setSubAction((RGE_Action*)created, 1);        // asm 0x404CE4
        return; // asm 0x404CFA
    }
    case RGE_ACTION_STATE_WORK:                                   // 6 -> loc_4049C9 - THE FARM CAPTURE
    {
        if (this_->target_obj                                     // asm 0x4049CC-0x4049CE
            && this_->target_obj->master_obj->object_group == 7   // asm 0x4049D3-0x4049D8 (cmp word @+0x1E, 7 - the FARM)
            && this_->target_obj->owner != this_->obj->owner      // asm 0x4049DD-0x4049E3 (the owner MISMATCH)
            && !(TRIBE_Game__gameType((TRIBE_Game*)*base_game) == 6   // asm 0x4049EB-0x4049F2 (:5513 @0x5A6A10; the base_game deref + the TRIBE_Game dual-naming cast)
                && this_->target_obj->worker_num))                // asm 0x4049F7-0x4049FA (test al; jnz -> the skip)
        {
            if (((TRIBE_Building_Object*)this_->target_obj)->build_pts <= 0.0f)   // asm 0x4049FE-0x404A12 (fld [ecx+230h] = 560; fcomp const_f_zero; test ah,41h/jz - the {<= 0, NaN} -> Done(1); the ordered > 0 falls = the literal > MATCH row)
            {
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);   // asm 0x404A1A (1 = Done)
                return; // asm 0x404A2E
            }
            this_->target_obj->vfptr->change_ownership(this_->target_obj, (RGE_Player*)this_->obj->owner);   // asm 0x40A3A-region: 0x404A36-0x40A3A (the target vtbl slot 47 = +0xBC = 188 decimal; the TRIBE_Player dual-naming cast)
        }
        // THE MULTI-STOP SPRITE (loc_404A40): the asm's get_move_sprite
        // RESULT flows in eax into the shared tail loc_404D9C
        // (new_sprite(obj, THE MOVE SPRITE)) - the IDA merged this entry
        // with the Done-arm's loc_404D95 wait-sprite entry; the first
        // transcription took the wait-sprite form for both (THE M6
        // TRANSCRIPTION BUG - the desync root cause)
        this_->timer = 0.0f;                                      // asm 0x404A43
        RGE_Sprite* sprite;
        if (this_->numberStops                                    // asm 0x404A4A (test al, al; jbe = the 0-variant)
            && this_->numberStops > this_->currentStop)           // asm 0x404A4E (cmp al, [esi+45h]; jbe -> the 0-variant)
            sprite = RGE_Action_Gather__get_move_sprite(this_, 1);// asm 0x404A60 (unk = 1)
        else
            sprite = RGE_Action_Gather__get_move_sprite(this_, 0);// asm 0x404A74 (unk = 0)
        this_->obj->vfptr->new_sprite(this_->obj, sprite);        // asm 0x404A65/0x404A79 -> loc_404D9C (slot 44; new_sprite(obj, THE MOVE SPRITE))
        return;
    }
    case RGE_ACTION_STATE_WORK_2:                                  // 7 -> loc_404A7E - THE GATHER SPRITE + the work sound
    {
        this_->have_worked = 1;                                   // asm 0x404A84 (mov byte [esi+52h], 1)
        this_->timer = 0.0f;                                      // asm 0x404A88
        __int16 attribute_type = this_->task->attribute_type;     // asm 0x404A8F (mov ax, [edx+14h])
        // baked: attribute_held.cpp:146 @ 0x404A93 - the same verify with
        // keep = false (the stub's push 0/push eax/push ecx + call,
        // resuming at 0x404AA4)
        verify_food_resource((RGE_Static_Object*)this_->obj, attribute_type, false);  // asm 0x404A93-0x404AA3
        RGE_Sprite* sprite = this_->task->work_sprite;            // asm 0x404AB1 ([eax+38h])
        if (sprite)                                               // asm 0x404AAC
        {
            __int16 group = this_->obj->master_obj->object_group; // asm 0x404AB7 (mov dx, [edx+1Eh])
            if (group == 58 || group == 14)                       // asm 0x404AB8-0x404AC2 (Worker / Utility Trawler - the registry)
                sprite = (RGE_Sprite*)(*base_game)->world->sprites[RGE_Static_Object__gbg_get_civ_override((RGE_Static_Object*)this_->obj) + sprite->id];   // asm 0x404AC4-0x404AE7 (the civ-sprite indexing: slot 2 = +0x8; world->sprites @+0x44 = 68; sprites[civ + sprite_id @+0x82 = 130])
            this_->obj->vfptr->new_sprite(this_->obj, sprite);    // asm 0x404AF0 (slot 44)
        }
        RGE_Sound* work_sound = this_->task->work_sound;          // asm 0x404AF9 ([eax+44h] = 68)
        if (work_sound)                                           // asm 0x404AFC
            RGE_Sound__play(work_sound, 1, 0, 0, -1, -1);         // asm 0x404B0E (:3662 @0x544400)
        return; // asm 0x404B24
    }
    case RGE_ACTION_STATE_RETURN:                                 // 8 -> loc_404B27 - THE DROP-SITE RESOLVE
    {
        this_->currentStop = 0;                                   // asm 0x404B2A (mov byte [esi+45h], 0)
        if (this_->obj->attribute_amount_held <= 0.0f)            // asm 0x404B2E-0x404B3C (fld @+0x54; fcomp const_f_zero; test ah,41h/jnz - the {<= 0,NaN} -> loc_404D38)
        {
            // THE NO-CARRIED BLOCK (loc_404D38): the target-clear check
            if (this_->target_obj                                 // asm 0x404D3F-0x404D3D (test edx)
                && this_->target_obj->attribute_amount_held <= 0.0f   // asm 0x404D42-0x404D4D (fld @+0x54; fcomp; test ah,41h/jz = the ordered > 0 -> the keep)
                && !this_->obj->vfptr->keepGatheringWhenObjectIsOut(this_->obj, this_->target_obj->master_obj->id))   // asm 0x404D4A-0x404D61 (slot 113 = +0x1C4 = 452 decimal)
                this_->vfptr->set_target_obj(this_, 0);           // asm 0x404D69 (slot 22 = +0x58)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm 0x404D72 (3 = Search)
            return; // asm 0x404D86
        }
        if (!this_->target_obj2)                                  // asm 0x404B42 (test eax)
        {
            if (this_->obj->master_obj->drop_site == -1)          // asm 0x404B50-0x404B58 (cmp word @+0x118, 0FFFFh)
            {
                this_->vfptr->set_target_obj2(this_, 0);          // asm loc_404BE0 (slot 23)
            }
            else
            {
                if (this_->obj->vfptr->useSameZoneDropsite(this_->obj) == 1)   // asm 0x404B60-0x404B68 (slot 115 = +0x1CC = 460 decimal)
                {
                    RGE_Static_Object* found = RGE_Player_Object_List__find_by_master_ids(this_->obj->owner->objects, this_->obj->master_obj->drop_site, this_->obj->master_obj->backup_drop_site, this_->obj->world_x, this_->obj->world_y, 1, 2, (RGE_Static_Object*)this_->obj);   // asm 0x404B95 (the zone_obj = the obj - NOT 0)
                    this_->vfptr->set_target_obj2(this_, found);  // asm 0x404B9D (slot 23)
                }
                if (!this_->target_obj2)                          // asm 0x404BA3-0x404BA5
                {
                    RGE_Static_Object* found = RGE_Player_Object_List__find_by_master_ids(this_->obj->owner->objects, this_->obj->master_obj->drop_site, this_->obj->master_obj->backup_drop_site, this_->obj->world_x, this_->obj->world_y, 1, 2, 0);   // asm 0x404BD3 (the zone_obj = 0)
                    this_->vfptr->set_target_obj2(this_, found);  // asm 0x404BDB (slot 23)
                }
            }
        }
        // THE AI-WATER CHECK (the Return arm's version)
        __int16* ctrCounts = this_->obj->owner->object_category_count_new;   // asm 0x404C06/0x404C10 (mov eax,[eax+2F0h]) - baked: aiunitcount.cpp:1360 @ 0x404C06 (the ctr2 re-base, the mirror of the 0x4047E0 bake)
        if (this_->obj->owner->vfptr->computerPlayer(this_->obj->owner) == 1   // asm 0x404BF1 (slot 7)
            && this_->target_obj2                                 // asm 0x404BFC
            && (ctrCounts[1659] > 0                               // asm 0x404C06 ([eax+0FE6h] -> the re-based table[1659])
                || ctrCounts[1617] > 0)                           // asm 0x404C10 ([eax+0F92h] -> the re-based table[1617])
            && (RGE_Static_Object__lookupZone(this_->target_obj2, (int)this_->target_obj2->world_x, (int)this_->target_obj2->world_y) != RGE_Static_Object__currentZone((RGE_Static_Object*)this_->obj)))   // asm 0x404C2E (the lookupZone/currentZone pair)
        {
            this_->vfptr->set_state(this_, 247);                  // asm 0x404C43 (push 0F7h - the internal water-crossing state)
            return; // asm 0x404C4F
        }
        if (this_->target_obj2)                                   // asm 0x404C66
        {
            RGE_Action_Move_To* created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x404C6E (push 44h)
            __try
            {
                if (created)                                      // asm 0x404C7C
                {
                    RGE_Sprite* sprite = RGE_Action_Gather__get_move_sprite(this_, 0);   // asm 0x404C8C (unk = 0)
                    RGE_Action_Move_To__RGE_Action_Move_To2(created, this_->obj, this_->target_obj2, this_->task->work_range, sprite);   // asm 0x404CA3 (:101 - HOOKED -> routes)
                }
            }
            __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH)
            {
            }
            if (!created)                                         // asm 0x404CB0
            {
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x404CBC (13 = Failed)
                return; // asm 0x404CD4
            }
            RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm 0x404CDB (:67)
            RGE_Action__setSubAction((RGE_Action*)created, 1);    // asm 0x404CE4 (:190)
            return; // asm 0x404CFA
        }
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 515, RGE_UNITAI_ACTION_GATHER, 0, 0);   // asm loc_404CFD (push 203h = 515, 261h = 609 - THE no-drop-site notify, the 5xx candidate)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);  // asm 0x404D21 (13 = Failed)
        return; // asm 0x404D35
    }
    default:                                                      // asm def_4044E2
        return; // asm 0x404DA6
    }
}

// ===========================================================================
// Replacement for RGE_Action_Gather::get_move_sprite. Original:
// @ 0x00405D40 (idb_functions.h:47). Range [0x405D40,0x405DE5) (lst
// :8765/:8843, retn 4 = 1 stack arg), RGE_Sprite* return. ABI: __thiscall.
// NOT virtual (a code-level helper - only update/work/save/... sit in the
// vftable). Callers: set_state +284/+49C/+1FB/+54E-region (the in-class
// sprite pickers) and the adopted new_farm_walk.
// Decode (asm 0x405D40-0x405DE4): when unk == 0 and the task's carry_sprite
// matches the task attribute_type against the unit's held type with a held
// amount > 0: return the carry sprite - CIV-INDEXED (world->sprites
// [civ + sprite->id @+0x82]) for Worker(58)/Utility Trawler(14) masters.
// Otherwise the same shape for the task's move_sprite.
RGE_Sprite* __fastcall RGE_Action_Gather__get_move_sprite_new(
    RGE_Action_Gather* this_, DWORD dummy, char unk)
{
    RGE_Task* task = this_->task;                                        // asm 0x405D4B ([esi+30h])
    if (!unk) {                                                          // asm 0x405D45-0x405D49 (test al,al; jnz move)
        RGE_Sprite* carry = task->carry_sprite;                          // asm 0x405D4E ([eax+40h], RGE_Task @idb_types.h:6634)
        if (carry) {                                                     // asm 0x405D51-0x405D53
            __int16 attrType = task->attribute_type;                     // asm 0x405D55 ([eax+14h], :6620)
            if (attrType != -1) {                                        // asm 0x405D59-0x405D5D
                RGE_Action_Object* obj = this_->obj;                     // asm 0x405D5F ([esi+8])
                if (obj->attribute_type_held == attrType                 // asm 0x405D62-0x405D66 (cmp [ecx+5Ch], ax - the WORD @+0x5C)
                    && obj->attribute_amount_held > 0.0f) {              // asm 0x405D68-0x405D76 (fcomp const_f_zero; test ah,41h; jnz = the {<=0,NaN} skip - the literal > is the polarity MATCH row: NaN fails the conjunct and skips in both)
                    __int16 group = obj->master_obj->object_group;       // asm 0x405D78-0x405D7F (mov eax,[ecx+14h]; mov ax,[eax+1Eh])
                    if (group != 58 /* Worker */ && group != 14 /* Utility Trawler - the object_group registry */)
                        return carry;                                    // asm 0x405D8B-0x405D8E
                    int civ = obj->vfptr->gbg_get_civ_override(obj);     // asm 0x405D91-0x405D9C (slot 2 = +0x8, the A3c grounding)
                    return (*base_game)->world->sprites[civ + carry->id]; // asm 0x405DC9-0x405DE1 (movsx word [edx+82h] = RGE_Sprite::id; the world slot deref -> world @0x420; world->sprites @+0x44 (RGE_Sprite**, idb_types.h:963); [edx+ecx*4])
                }
            }
        }
    }
    RGE_Sprite* move = task->move_sprite;                                // asm 0x405D9E-0x405DA4 ([eax+34h], :6631)
    if (move) {                                                          // asm 0x405DA4-0x405DA6
        RGE_Action_Object* obj = this_->obj;                             // asm 0x405DA8 ([esi+8])
        __int16 group = obj->master_obj->object_group;                   // asm 0x405DAB-0x405DB2
        if (group == 58 || group == 14) {                                // asm 0x405DB6-0x405DC3 (cmp 3Ah / cmp 0Eh)
            int civ = obj->vfptr->gbg_get_civ_override(obj);             // asm 0x405DBE-0x405DC0 (slot 2)
            return (*base_game)->world->sprites[civ + move->id];         // asm 0x405DC9-0x405DE1
        }
    }
    return move;                                                         // asm 0x405DE4-0x405DE5
}

// ===========================================================================
// Replacement for RGE_Action_Gather::get_wait_sprite. Original:
// @ 0x00405DF0 (idb_functions.h:48). Range [0x405DF0,0x405E6C) (lst
// :8852/:8908, retn), RGE_Sprite* return. ABI: __thiscall. NOT virtual.
// Decode (asm 0x405DF0-0x405E6B): the carry-sprite gate (type match +
// amount > 0) civ-indexed for Worker/Trawler; EVERY gate failure falls
// back to master->vfptr->get_sprite(master, this unit) (slot 1 = +0x4,
// the Building-draw precedent).
RGE_Sprite* __fastcall RGE_Action_Gather__get_wait_sprite_new(RGE_Action_Gather* this_)
{
    RGE_Task* task = this_->task;                                        // asm 0x405DF3-0x405DF6 ([esi+30h])
    RGE_Sprite* carry = task->carry_sprite;                              // asm 0x405DF6 ([eax+40h])
    RGE_Action_Object* obj = this_->obj;                                 // asm 0x405E07 ([esi+8])
    if (!carry)                                                          // asm 0x405DF9-0x405DFB
        return obj->master_obj->vfptr->get_sprite(obj->master_obj, (RGE_Static_Object*)obj); // asm 0x405E5F-0x405E68 (the master vtbl slot 1; the ancestor cast per the cross-class rule)
    __int16 attrType = task->attribute_type;                             // asm 0x405DFD ([eax+14h])
    if (attrType == -1)                                                  // asm 0x405E01-0x405E05
        return obj->master_obj->vfptr->get_sprite(obj->master_obj, (RGE_Static_Object*)obj);
    if (obj->attribute_type_held != attrType                             // asm 0x405E0A-0x405E0E (cmp [ecx+5Ch], ax)
        || X87_LE_NAN(obj->attribute_amount_held, 0.0f))                 // asm 0x405E10-0x405E1E (fcomp const_f_zero; test ah,41h; jnz fallback - the skip set is {<=0, NaN}; the .c's literal <= would keep NaN in the civ arm - the negated-opposite form, rule (i))
        return obj->master_obj->vfptr->get_sprite(obj->master_obj, (RGE_Static_Object*)obj);
    __int16 group = obj->master_obj->object_group;                       // asm 0x405E20-0x405E31
    if (group == 58 || group == 14) {                                    // asm 0x405E27-0x405E37
        int civ = obj->vfptr->gbg_get_civ_override(obj);                 // asm 0x405E37-0x405E43 (slot 2)
        return (*base_game)->world->sprites[civ + task->carry_sprite->id]; // asm 0x405E43-0x405E5B
    }
    return carry;                                                        // asm 0x405E33-0x405E36
}

#pragma float_control(precise, on, push)

// ===========================================================================
// Replacement for RGE_Action_Gather::work. Original: @ 0x00405E80
// (idb_functions.h:50). Range [0x405E80,0x4060A6) (lst :8933/:9165,
// retn 10h = 4 stack args), int return. ABI: __thiscall. VIRTUAL (the
// Gather vftable slot 13 = +0x34, idb_types.h:28782; the entry hook covers
// the dispatch).
// Decode (asm 0x405E80-0x4060A5): have_worked = 0; the null-obj coordinate
// arm (targets + state 15); the same-target early accept in states
// {4,6,7,11,15}; the Building(80) linked-owner redirect (the target's
// linked_owner @+0x1DC when object_state < 2 and same owner ->
// set_only_action(0) + work2(1,0,-1) -> return 1); the Building-path
// Farm/Aqua gate (id != 50 && !(id == 199 && v >= 8) -> return 0 - the
// baked farm.cpp:973 @ 0x405F38 extension); the drop-site deposit arm
// (WORKTARGET's master id, the asm's eax = lea [ebp+14h] @0x405F35, ==
// drop_site @+0x118 / backup @+0x11A + amount > 0 -> the state-8 deposit
// prep); else the gather arm with the FARM CAPTURE (id 50: farm_x/y
// snapshot + the timePerStop math) and the baked Aqua capture arm
// (farm.cpp:967 @ 0x405FDE: farm_x/y ONLY, no stop math, v >= 8 inside
// the helper).
int __fastcall RGE_Action_Gather__work_new(RGE_Action_Gather* this_, DWORD dummy,
    RGE_Static_Object* obj_in, float x, float y, float z)
{
    this_->have_worked = 0;                                              // asm 0x405E8C (mov byte [esi+52h], 0)
    if (!obj_in) {                                                       // asm 0x405E8A-0x405E90
        this_->target_x = x;                                             // asm 0x406076-0x406082
        this_->target_y = y;                                             // asm 0x40607A-0x406087
        this_->target_z = z;                                             // asm 0x40607E-0x40608E
        this_->vfptr->set_target_obj(this_, 0);                          // asm 0x40608A-0x406091 (slot 22 = +0x58)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE_NO_SEARCH); // asm 0x406094-0x40609A (slot 24 = +0x60; 15)
        return 1;                                                        // asm 0x40609D-0x4060A2
    }
    if (obj_in == this_->target_obj) {                                   // asm 0x405E96-0x405E99 (cmp edi,[esi+10h])
        unsigned __int8 st = this_->state;                               // asm 0x405E9B ([esi+0Ch])
        if (st == RGE_ACTION_STATE_GOTO || st == RGE_ACTION_STATE_WORK   // asm 0x405E9E-0x405EB0 (cmp 4 / 6 / 7)
            || st == RGE_ACTION_STATE_WORK_2 || st == RGE_ACTION_STATE_MOVE
            || st == RGE_ACTION_STATE_MOVE_NO_SEARCH) {                  // asm 0x405EB6-0x405EC0 (cmp 0Bh / 0Fh)
            return 1;                                                    // asm 0x40609D (the shared return-1 tail)
        }
    }
    RGE_Master_Static_Object* objMaster = obj_in->master_obj;            // asm 0x405EC4-0x405EC9 ([edi+14h]; ebx = &master_obj)
    RGE_Static_Object* workTarget = obj_in;                              // asm 0x405ECC (ebp = edi)
    if (objMaster->master_type == 80 /* RGE_MASTER_TYPE_BUILDING */) {   // asm 0x405ED2-0x405EDA (mov dl,[ecx+4]; cmp 50h)
        if (((TRIBE_Building_Object*)obj_in)->linked_owner)              // asm 0x405EDC-0x405EE6 ([edi+1DCh] - the M12 linked_owner grounding; the guarded downcast: master_type==80 = the Building-tier factory identity, the Combat(70) precedent)
            workTarget = (RGE_Static_Object*)((TRIBE_Building_Object*)obj_in)->linked_owner; // asm 0x405EE6 (mov ebp, eax - the redirect; the result-assignment ancestor cast per the cross-class rule (a))
        if (workTarget->object_state < 2u) {                             // asm 0x405EE8-0x405EEC (cmp byte [ebp+58h], 2; jnb skip)
            RGE_Action_Object* obj = this_->obj;                         // asm 0x405EEE ([esi+8])
            if (workTarget->owner == obj->owner) {                       // asm 0x405EF1-0x405EF7 ([ebp+18h] vs [ebx+18h])
                obj->vfptr->set_only_action(obj, 0);                     // asm 0x405EF9-0x405EFF (slot 231 = +0x39C)
                obj->vfptr->work2(obj, workTarget, workTarget->world_x,  // asm 0x405F05-0x405F1C (slot 227 = +0x38C; the IR#2-corrected
                    workTarget->world_y, workTarget->world_z,            //   form: obj, x, y, z, work_flag 1, gather_flag 0, certain_action -1)
                    1, 0, -1);
                return 1;                                                // asm 0x405F22-0x405F2B
            }
            objMaster = obj_in->master_obj;                              // asm 0x405F2E-0x405F30 (the slot reload)
        }
        // baked: farm.cpp:973 @ 0x405F38 (action_gather_work) - the Aqua
        // Harvester (199) joins the Building-path Farm gate, version-gated
        if (workTarget->master_obj->id != 50 /* Farm */                  // asm 0x405F38-0x405F3D (cmp word [ecx+18h], 32h; jz loc_405F48)
            && !(workTarget->master_obj->id == AQUA_HARVESTER_ID         // the stub's second cmp
                && current_loaded_version >= 8))                         // the stub's cmp 8/jge
            return 0;                                                    // asm 0x405F3F-0x405F45 (the stub's not_farm resume: pop/xor eax/retn 10h)
    }
    RGE_Action_Object* obj = this_->obj;                                 // asm 0x405F48 ([esi+8])
    if (obj_in->owner == obj->owner                                      // asm 0x405F4B-0x405F51 ([edi+18h] vs [edx+18h])
        && (workTarget->master_obj->id == obj->master_obj->drop_site     // asm 0x405F53-0x405F63 (ax = [eax+18h], eax = lea [ebp+14h] @0x405F35 = WORKTARGET's master, the .c's v10 = &v9->master_obj; NOT objMaster/v8 - the ebx reload feeds only the gather arm; cmp [ecx+118h])
            || workTarget->master_obj->id == obj->master_obj->backup_drop_site)   // asm 0x405F65-0x405F6C (cmp [ecx+11Ah])
        && obj->attribute_amount_held > 0.0f) {                          // asm 0x405F6E-0x405F7C (fcomp const_f_zero; test ah,41h; jnz = the {<=0,NaN} skip - the literal > is the MATCH row)
        this_->timer = 0.0f;                                             // asm 0x405F84 ([esi+2Ch] = 0)
        this_->vfptr->set_target_obj(this_, 0);                          // asm 0x405F7E-0x405F8B (slot 22)
        this_->target_x = -1.0f;                                         // asm 0x405F8E-0x405F94
        this_->target_y = -1.0f;                                         // asm 0x405F97
        this_->target_z = -1.0f;                                         // asm 0x405F9A
        this_->vfptr->set_target_obj2(this_, obj_in);                    // asm 0x405F93-0x405FA1 (slot 23 = +0x5C)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_RETURN);         // asm 0x405FA4-0x405FAA (8)
        return 1;                                                        // asm 0x405FAD-0x405FB6
    }
    // the gather arm
    this_->vfptr->set_target_obj(this_, obj_in);                         // asm 0x405FB9-0x405FBE (slot 22)
    this_->target_x = obj_in->world_x;                                   // asm 0x405FC1-0x405FC7
    this_->target_y = obj_in->world_y;                                   // asm 0x405FC7-0x405FCD
    this_->target_z = obj_in->world_z;                                   // asm 0x405FCD-0x405FD3
    this_->targetType = objMaster->object_group;                         // asm 0x405FD3-0x405FD9 (movsx word @+1Eh; [esi+40h])
    if (objMaster->id == 50 /* Farm */) {                                // asm 0x405FDC-0x405FE3 (cmp word [eax+18h], 32h; jz loc_405FE5 = the capture, jnz loc_406058 = skip)
        RGE_Task* task = this_->task;                                    // asm 0x405FE5-0x405FE8 ([esi+30h])
        this_->farm_x = this_->target_x;                                 // asm 0x405FE8-0x405FEE ([esi+54h])
        this_->farm_y = this_->target_y;                                 // asm 0x405FEB-0x405FF3 ([esi+58h])
        if (task) {                                                      // asm 0x405FF1-0x405FF6 (test eax,eax)
            double mult = 1.0;                                           // asm 0x405FF8 (fld const_f_one - the st0 seed)
            this_->numberStops = 4;                                      // asm 0x405FFE ([esi+44h] = 4)
            this_->currentStop = 0;                                      // asm 0x406002 ([esi+45h] = 0)
            if (task->attribute_type2 != -1) {                           // asm 0x406006-0x40600E (cmp cx, 0FFFFh)
                mult = obj->owner->attributes[task->attribute_type2];    // asm 0x406010-0x406021 (fstp st discards the seed; owner->attributes @+0xAC; fld [edx+ecx*4])
            }
            const double attrMax = (double)obj->master_obj->attribute_max_amount; // asm 0x406024-0x406039 (movsx word [ecx+8Ch] = 140; fild - NO intermediate float round)
            const double rateTimesVal = (double)task->work_val1 * (double)obj->master_obj->gbg_work_rate_2; // asm 0x40603B-0x40603E (fld [eax+1Ch] = 28; fmul [ecx+114h] = 276 - the x87 float-float product is EXACT at 53-bit; rule (j) widen-before-op; /Fa: cvtps2pd x2 + mulsd)
            this_->timeAtCurrentStop = 0.0f;                             // asm 0x406044 ([esi+4Ch] = 0)
            this_->timePerStop = (float)(attrMax * mult / rateTimesVal * 0.25f); // asm 0x406039-0x406053 (fmul st,st(1); fdivp st(1),st; fmul flt_654968 (0.25f - binary-verified 3E800000); fstp - ONE store-round; division never collapses - all-double operands, rule (k); /Fa: mulsd x2 + divsd + mulsd + cvtsd2ss x1)
        }
    }
    // baked: farm.cpp:967 @ 0x405FDE (action_gather_farm_xy_1) - the Aqua
    // Harvester capture arm: the farm snapshot ONLY (action_gather_set_farm_xy,
    // farm.cpp:71 - NO stop math; the version gate lives inside the helper)
    else if (objMaster->id == AQUA_HARVESTER_ID && current_loaded_version >= 8) {
        this_->farm_x = this_->target_x;                                 // the helper's only stores (farm.cpp:75-76)
        this_->farm_y = this_->target_y;
    }
    this_->vfptr->set_target_obj2(this_, 0);                             // asm 0x406058-0x40605E (slot 23)
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);               // asm 0x406061-0x406067 (4)
    return 1;                                                            // asm 0x40606A-0x406073
}

#pragma float_control(precise, on, push)

// ===========================================================================
// Replacement for RGE_Action_Gather::update - PASS 1, THE ALIGNED
// TRANSCRIPTION (the two-pass decode; the .c structure 1:1, the IDA
// v-names and gotos preserved, value-level fixes only). Original:
// @ 0x00404EE0 (idb_functions.h:46). Range [0x404EE0,0x405BC9) (lst
// :7176/:8702 = 3305 bytes, retn = no stack args), unsigned __int8 return.
// ABI: __thiscall. VIRTUAL (the Gather vftable slot 10 = +0x28,
// idb_types.h:28779; the entry hook covers the dispatch).
// Two-pass bar: 27 gotos + 13 back-edges (the mechanical conjunction (b)).
// The switch: byte_405C04 (249 bytes) + off_405BCC (14 dwords), both
// binary-verified from battlegrounds_x2.exe; the state-4 and the
// 11/15/242 sub-switches = off_405D14/off_405D00 (5 dwords each).
// THE PRECISION MODEL: the whole-function precise wrap (the capstone
// precedent) - every float chain emits the x87-equivalent form; the /Fa
// census gates the shrink signature.
// SEMANTIC HEADER: the per-tick Gather driver. The head purges dead
// targets (id lookups; state >= 7 targets release the binding, a released
// Farm clears the farm snapshot). The state machine: Done -> 1; Search ->
// the -10.0 drop-off timer with the 514/609 re-target notifies (the
// drop_site/backup pair lookup); Goto -> the sub-action dispatch (Work on
// arrival, the timer tick for the multi-stop patrol); Work -> the
// turn-towards + the Work2 hand-off (the Farm/Aqua crowd checks);
// Work2 -> THE GATHER TICK (the quick_build/full-target gain, the
// attribute cap with the overflow split, the +/-0.01 mirror writes, the
// early-full exit - the ATTRIBUTE_HELD bake); Return -> the deposit (the
// work sound, the attribute_type3 bank + the gather track, the AI
// re-seek via the UnitAI target types -> the recursive work() or the
// sentinel/no-AI fallouts); Move/MoveNoSearch/F2 -> the sub dispatch with
// the auto-rebuild-farm + the 127 notification; Failed -> the 505 notify
// + the Done/Wait split (return 3); F3 -> the -1.0 drop-off timer + the
// have_worked re-drive; F4/F7 -> the water-crossing state swaps; F5/F8 ->
// the inside-container wait swaps; F6/F9 -> the inside-container gates.
unsigned __int8 __fastcall RGE_Action_Gather__update_new(RGE_Action_Gather* this_)
{
    TRIBE_Combat_Object *v2;
    RGE_Static_Object *v3;
    RGE_Action_Object *v4;
    double v5;
    RGE_Static_Object *v6;
    unsigned __int8 result;
    RGE_Master_Action_Object *v8;
    __int16 v9;
    RGE_Static_Object *v10;
    RGE_Action_Object *v11;
    double v12;
    unsigned __int8 v13;
    RGE_Action_Object *v14;
    RGE_Master_Action_Object *v15;
    __int16 v16;
    double v17;
    unsigned __int8 v18;
    RGE_Static_Object *v19;
    RGE_Action_Object *v20;
    __int16 v21;
    __int16 v22;
    RGE_Static_Object *v23;
    RGE_Static_Object *v24;
    RGE_Action_Object *v25;
    __int16 v26;
    unsigned __int8 v27;
    double v28;
    double v29;
    __int16 v30;
    double v31;
    RGE_Action_Object *v32;
    double v33;
    double v34;
    RGE_Static_Object *v35;
    RGE_Action_Object *v36;
    RGE_Static_Object *v37;
    RGE_Static_Object *v38;
    double v39;
    unsigned __int8 v40;
    RGE_Action_Object *v41;
    RGE_Sound *v42;
    RGE_Task *v43;
    UnitAIModule *v44;
    int v45;
    RGE_Static_Object *v46;
    UnitAIModule *v47;
    UnitAIModuleVtbl *v48;
    UnitAIModule *v49;
    int v50;
    int v51;
    UnitAIModule *v52;
    int v53;
    RGE_Static_Object *v54;
    UnitAIModule *v55;
    UnitAIModuleVtbl *v56;
    UnitAIModule *v57;
    int v58;
    UnitAIModule *v59;
    int v60;
    RGE_Static_Object *v61;
    UnitAIModule *v62;
    int v63;
    UnitAIModule *v64;
    RGE_Static_Object *v65;
    __int16 v66;
    unsigned __int8 v67;
    float v68;
    float v69;
    float v70;
    float v71;
    float v72;
    __int16 v73;

    // the head: dead-target purge (the IDA-carve point ebp = 0 doubles as
    // the null constant all arms read)
    v21 = this_->targetID;                                               // asm 0x404EE9 ([esi+18h])
    if (v21 != -1 && !RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, v21)) { // asm 0x404EEC-0x404F05 (:6086; the TRIBE_World family cast)
        this_->vfptr->set_target_obj(this_, 0);                          // asm 0x404F07-0x404F0E (slot 22 = +0x58)
    }
    v22 = this_->target2ID;                                              // asm 0x404F15 ([esi+1Ch])
    if (v22 != -1 && !RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, v22)) { // asm 0x404F18-0x404F31
        this_->vfptr->set_target_obj2(this_, 0);                         // asm 0x404F33-0x404F38 (slot 23 = +0x5C)
    }
    v2 = (TRIBE_Combat_Object*)this_->target_obj;                        // asm 0x404F3B ([esi+10h]; the Building-tier downcast - the id-50/199 gate proves the tier before the user_released read)
    if (v2 && v2->object_state >= 7u) {                                  // asm 0x404F43-0x404F4F (cmp [eax+58h], bl(7); jb skip)
        // baked: farm.cpp:1009 @ 0x404F54 - the farm_x-reset gate extends
        // to the Aqua Harvester master (199)
        if (v2->master_obj->id == 50 /* Farm */                          // asm 0x404F51-0x404F59 (cmp word [ecx+18h], 32h)
            || v2->master_obj->id == AQUA_HARVESTER_ID) {                // asm (the stub's second cmp)
            if (v2->user_released)                                       // asm 0x404F5B-0x404F63 (mov cl,[eax+1BCh]; test)
                this_->farm_x = -1.0f;                                   // asm 0x404F65 ([esi+54h] = edi(-1.0))
        }
        this_->vfptr->set_target_obj(this_, 0);                          // asm 0x404F68-0x404F6D
    }
    v3 = this_->target_obj2;                                             // asm 0x404F70 ([esi+14h])
    if (v3 && v3->object_state >= 7u) {                                  // asm 0x404F75-0x404F7A
        this_->vfptr->set_target_obj2(this_, 0);                         // asm 0x404F7C-0x404F81 (slot 23)
    }
    // the state dispatch: byte_405C04[state-1] -> off_405BCC (binary-verified)
    switch (this_->state) {                                              // asm 0x404F84-0x404F9D (mov al,[esi+0Ch]; dec; cmp 0F8h; ja default; cl = byte_405C04[eax]; jmp off_405BCC[ecx*4])
    case RGE_ACTION_STATE_DONE:                                          // asm jumptable case 0 -> loc_405B6E
        return 1;                                                        // asm 0x405B6E-0x405B77 (mov al, 1)
    case RGE_ACTION_STATE_SEARCH:                                        // asm case 2 -> loc_404FA4
        if (this_->timer >= 0.0)                                         // asm 0x404FA4-0x404FB2 (fld timer; fcomp const_d_zero(m64); test ah,1; jz default - the {>= ordered} skip; the literal >= is the MATCH row: NaN continues in both)
            return 0;
        v4 = this_->obj;                                                 // asm 0x404FB8 ([esi+8])
        v5 = (double)v4->owner->world->world_time_delta_seconds + (double)this_->timer;  // asm 0x404FBE-0x404FCA (fld [world+0A8h]; fadd [esi+2Ch] - 53-bit; /Fa: cvtps2pd x2 + addsd)
        this_->timer = (float)v5;                                        // asm 0x404FCD (fst [esi+2Ch] - the m32 store-round, st0 stays UNROUNDED)
        if (X87_LE_NAN(v5, 0.0))                                         // asm 0x404FD0-0x404FDB (fcomp const_d_zero; test ah,41h; jnz default - the {<=0, NaN} set; the .c's literal <= would keep NaN ticking - the negated-opposite form)
            return 0;
        v6 = this_->target_obj2;                                         // asm 0x404FE1 ([esi+14h])
        this_->timer = -10.0f;                                           // asm 0x404FE4 ([esi+2Ch] = -10.0)
        if (v6) {                                                        // asm 0x404FEB-0x404FED
            v4->vfptr->notify(v4, v4->id, v4->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, v6->id); // asm 0x404FEF-0x405008 (slot 120 = +0x1E0; push 261h/202h; 514 = the gather re-target notify - the 5xx candidate)
            return 0;                                                    // asm 0x40500E-0x405017
        }
        v8 = v4->master_obj;                                             // asm 0x405018 ([ecx+14h])
        v9 = v8->drop_site;                                              // asm 0x40501B ([eax+118h] = 280)
        if (v9 == -1) {                                                  // asm 0x405022-0x405026
            v4->vfptr->notify(v4, v4->id, v4->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, -1); // asm 0x405028-0x40503F
            return 0;
        }
        v10 = RGE_Player_Object_List__find_by_master_ids(v4->owner->objects, // asm 0x40504F-0x40506E (:2299; owner->objects @+0x78; the drop_site/backup pair @+0x118/+0x11A; x/y = world_x/y; certain_state 1, object_state 2, zone_obj 0)
            v9, v8->backup_drop_site, v4->world_x, v4->world_y, 1u, 2u, 0);
        v11 = this_->obj;                                                // asm 0x405073-0x405076
        if (!v10) {                                                      // asm 0x405076-0x405078
            v11->vfptr->notify(v11, v11->id, v11->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, 0); // asm 0x4050A3-0x40503F (the shared notify tail)
            return 0;
        }
        v11->vfptr->notify(v11, v11->id, v11->id, 514, RGE_UNITAI_ACTION_GATHER, this_->targetType, v10->id); // asm 0x40507A-0x405093
        return 0;
    case RGE_ACTION_STATE_GOTO:                                          // asm case 3 -> loc_405256
        if (!this_->target_obj) {                                        // asm 0x405256-0x405259 (cmp [esi+10h], ebp)
            v14 = this_->obj;                                            // asm 0x40525B ([esi+8])
            if (v14->attribute_amount_held > 0.0f)                       // asm 0x40525E-0x40526C (the literal > MATCH row - NaN skips to the master chain in both)
                goto LABEL_189;
            v15 = v14->master_obj;                                       // asm 0x405281 ([ecx+14h])
            goto LABEL_57;
        }
        if (this_->timer >= 0.0) {                                       // asm 0x4052C5-0x4052D3 (the literal >= MATCH row)
            switch (this_->sub_actions->vfptr->update(this_->sub_actions)) { // asm 0x40531F-0x405336 (slot 4 = +0x10; and eax,0FFh = the AL read IS the mask - the u8 return; dec; cmp 4; ja default; jmp off_405D14[eax*4])
            case 1u:                                                     // asm off_405D14[0] -> loc_40537F
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);   // asm 0x40537F-0x405385 (6)
                result = 0;
                break;
            case 3u:                                                     // asm off_405D14[2] -> loc_405358
                if (this_->obj->owner->vfptr->computerPlayer(this_->obj->owner) == 1) // asm 0x405358-0x405366 (slot 7 = +0x1C)
                    goto LABEL_43;
                goto LABEL_135;
            case 4u:                                                     // asm off_405D14[3]/[4] -> loc_40533D
            case 5u:
                this_->vfptr->set_target_obj(this_, 0);                  // asm 0x40533D-0x405342 (slot 22)
                goto LABEL_33;
            default:
                return 0;                                                // asm off_405D14[1] -> loc_4057A8
            }
            return result;
        }
        v17 = (double)this_->obj->owner->world->world_time_delta_seconds + (double)this_->timer; // asm 0x4052D5-0x4052EA (the 53-bit tick; /Fa: cvtps2pd x2 + addsd)
        this_->timer = (float)v17;                                       // asm 0x4052EA (fst)
        if (X87_LE_NAN(v17, 0.0f))                                       // asm 0x4052ED-0x4052F8 (fcomp const_f_zero(m32); test ah,41h; jnz default)
            return 0;
        v18 = this_->numberStops;                                        // asm 0x4052FE ([esi+44h])
        this_->timer = 0.0f;                                             // asm 0x405301 ([esi+2Ch] = ebp)
        if (v18 > 1u) {                                                  // asm 0x405304-0x405306 (cmp al, 1; ja)
LABEL_187:
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);       // asm 0x405B3C-0x405B42 (4)
            return 0;
        }
        goto LABEL_33;
    case RGE_ACTION_STATE_WORK:                                          // asm case 5 -> loc_405392
        v19 = this_->target_obj;                                         // asm 0x405392 ([esi+10h])
        if (v19) {
            v22 = v19->master_obj->id;                                   // asm 0x4053F4-0x4053FB ([eax+14h]; mov cx,[ecx+18h])
            if (v22 != 50 && v22 != 199 /* Aqua Harvester - the original's own 0xC7 test */) // asm 0x4053FF-0x405406 (cmp 32h / cmp 0C7h)
                goto LABEL_60;
            if (v19->object_state < 2u)                                  // asm 0x405408-0x40540C (cmp byte [eax+58h], 2; jnb LABEL_454-line)
                goto LABEL_53;
            if (v19->worker_num <= 1u)                                   // asm 0x405421-0x405425 (cmp byte [eax+5Fh], 1; jbe LABEL_60)
                goto LABEL_60;
            goto LABEL_96;                                               // asm 0x405427-0x405430 (push 0F3h - the >1 crowded path sets 243)
        } else {
            v20 = this_->obj;                                            // asm 0x40539C ([esi+8])
            if (v20->attribute_amount_held > 0.0f)                       // asm 0x40539F-0x4053AA (the literal > MATCH row)
                goto LABEL_127;
            v21 = v20->master_obj->id;                                   // asm 0x4053B0-0x4053B7
            if (v21 != 259 && v21 != 214 && (v21 != 13 || this_->targetType != 7)) // asm 0x4053BB-0x4053D8 (cmp 103h / 0D6h / 0Dh; cmp [esi+40h], ebx(7))
                goto LABEL_53;
        }
        goto LABEL_96;
LABEL_60:
        if (!((unsigned __int8)this_->obj->vfptr->turn_towards(this_->obj, v19, 0, 0))) // asm 0x40543D-0x40544D (slot 221 = +0x374; test al,al; jz default)
            return 0;
        v23 = this_->target_obj;                                         // asm 0x405453 ([esi+10h])
        if (v23->attribute_amount_held > 0.0f                            // asm 0x405456-0x405464 (the literal > MATCH row)
            || this_->obj->vfptr->keepGatheringWhenObjectIsOut(this_->obj, v23->master_obj->id)) { // asm 0x405466-0x40547B (slot 113 = +0x1C4; test al,al; jnz keep-gathering)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK_2);     // asm 0x4054AC-0x4054B1 (7; ebx = 7 held from the head)
            this_->timeAtCurrentStop = 0.0f;                             // asm 0x4054B4 ([esi+4Ch] = ebp)
            return 0;
        }
        this_->vfptr->set_target_obj(this_, 0);                          // asm 0x40547D-0x405482
        if (this_->obj->attribute_amount_held > 0.0f)                    // asm 0x405485-0x405496 (the literal > MATCH row)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_RETURN);     // asm 0x405498-0x40549E (8)
        v15 = this_->obj->master_obj;                                    // asm 0x4054A1-0x4054A4 ([esi+8]; [ecx+14h])
LABEL_57:
        v16 = v15->id;                                                   // asm 0x405284 (mov ax,[ecx+18h])
        if (v16 != 259 && v16 != 214 && (v16 != 13 || this_->targetType != 7)) // asm 0x405288-0x4052A9 (cmp 103h / 0D6h / 0Dh; cmp [esi+40h], ebx)
            goto LABEL_53;
        goto LABEL_96;
    case RGE_ACTION_STATE_WORK_2:                                        // asm case 6 -> loc_4054C1
        v24 = this_->target_obj;                                         // asm 0x4054C1 ([esi+10h])
        if (!v24) {
            v25 = this_->obj;                                            // asm 0x4054C8 ([esi+8])
            if (v25->attribute_amount_held > 0.0f)                       // asm 0x4054CB-0x4054D9 (the literal > MATCH row)
                goto LABEL_127;
            v26 = v25->master_obj->id;                                   // asm 0x4054DF-0x4054E6
            if (v26 != 259 && v26 != 214 && (v26 != 13 || this_->targetType != 7)) { // asm 0x4054EA-0x405503
LABEL_53:
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH); // asm loc_40551B-region (push 3)
                return 0;
            }
            goto LABEL_96;
        }
        if (this_->numberStops > 1u) {                                   // asm 0x40552E-0x405533 (mov al,[esi+44h]; cmp 1; jbe)
            v27 = this_->currentStop;                                    // asm 0x405535 ([esi+45h])
            if (this_->numberStops > (unsigned __int8)(v27 + 1) && !(*base_game)->quick_build) { // asm 0x405538-0x405555 (and/zero-extend; inc edi; cmp; jle; base_game+9E0h = 2528 - the deref rule)
                v28 = (double)this_->obj->owner->world->world_time_delta_seconds + (double)this_->timeAtCurrentStop; // asm 0x405557-0x405569 (the 53-bit tick on timeAtCurrentStop; /Fa: cvtps2pd x2 + addsd)
                this_->timeAtCurrentStop = (float)v28;                   // asm 0x40556C (fst [esi+4Ch])
                if (v28 >= this_->timePerStop) {                         // asm 0x40556F-0x405577 (fcom [esi+48h]; test ah,1; jnz skip - the literal >= MATCH row: NaN skips in both)
                    this_->currentStop = (unsigned __int8)(v27 + 1);     // asm 0x405579-0x40557D (inc dl; mov [esi+45h], dl)
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO); // asm 0x40557B-0x405584 (push 4)
                    return 0;
                }
            }
        }
        if ((*base_game)->quick_build)                                   // asm 0x405591-0x405596 (cmp [eax+9E0h], ebp; jz product)
            v29 = v24->attribute_amount_held;                            // asm 0x40559E-0x4055A1 (fld [ecx+54h])
        else
            v29 = (double)this_->obj->owner->world->world_time_delta_seconds // asm 0x4055A3-0x4055C1 (fld [world+0A8h]; fmul [master+114h]; fmul [task+1Ch] - the asm's exact (delta*rate)*val1 order, 53-bit unrounded; /Fa: cvtps2pd x3 + mulsd x2)
                * (double)this_->obj->master_obj->gbg_work_rate_2
                * (double)this_->task->work_val1;
        if (v29 > v24->attribute_amount_held)                            // asm 0x4055C4-0x4055CC (fcom [ecx+54h] NO-POP; test ah,41h; jnz keep - the literal > MATCH row: NaN keeps the computed gain in both)
            v29 = v24->attribute_amount_held;
        v30 = this_->task->attribute_type2;                              // asm 0x4055D3-0x4055DA ([esi+30h]; mov ax,[eax+16h])
        v31 = 1.0;                                                       // asm 0x405601 (fld const_f_one - the seed)
        if (v30 != -1) {                                                 // asm 0x4055DA-0x4055DE (cmp ax, 0FFFFh)
            v31 = this_->obj->owner->attributes[v30];                    // asm 0x4055E0-0x4055EF (fld [edx+eax*4])
            if (X87_LE_NAN(v31, 0.0))                                    // asm 0x4055F2-0x4055FD (fcom const_f_zero; test ah,41h; jz keep - the {<=0, NaN} set seeds 1.0; the .c's literal <= would keep a NaN multiplier)
                v31 = 1.0;
        }
        v32 = this_->obj;                                                // asm 0x405607 ([esi+8])
        v72 = (float)v32->master_obj->attribute_max_amount;              // asm 0x40560D-0x40561C (movsx word [edx+8Ch] = 140; fild; fstp var_4 - the FLOAT store-round of attr_max)
        v70 = (float)(v31 * v29);                                        // asm 0x405620-0x405624 (fld st; fmul st,st(2); fst var_8 - the rounded product copy; the UNROUNDED product continues below)
        v33 = v31 * v29 + v32->attribute_amount_held;                    // asm 0x405628 (fadd [eax+54h] - the 53-bit sum over the UNROUNDED product)
        if (v33 > v72) {                                                 // asm 0x40562B-0x405634 (fcom var_4; test ah,41h; jnz skip - the literal > MATCH row)
            v34 = v33 - v72;                                             // asm 0x405636 (fsub var_4)
            v70 = (float)(v70 - v34);                                    // asm 0x40563A-0x405640 (fld var_8 - the ROUNDED v70; fsub st,st(1); fstp var_8)
            v29 = v29 - v34 / v31;                                       // asm 0x405644-0x405646 (fdiv st,st(1); fsubp st(2) - the gain stays UNROUNDED on st)
        }
        // baked: attribute_held.cpp:148 @ 0x40560D - the early-full exit:
        // held amount >= the cap (ordered) -> the unit is full, straight to
        // the deposit state (the stub's fcomip jb gate; NaN takes the
        // gather path in both - the literal < is the MATCH row)
        if (!(v32->attribute_amount_held < v72)) {                       // asm (the stub's fcomip st,st(1); jb - jb-not-taken = {>= ordered})
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_RETURN);     // asm (the stub's jmp 0x405754 = loc_405754: push 8)
            return 0;
        }
        v68 = (float)-v29;                                               // asm 0x40564C-0x405656 (fchs; fstp [esp+var_24] - the arg store-round of the negated UNROUNDED gain)
        v24->vfptr->set_attribute_amount(v24, v68, 1, 0);                // asm 0x40564E-0x405659 (the TARGET's Static vtbl slot 63 = +0xFC, idb_types.h:4007; args (float, 1, 0))
        v35 = this_->target_obj;                                         // asm 0x40565F ([esi+10h])
        if (X87_LT_NAN(v35->attribute_amount_held, 0.1f)                 // asm 0x405662-0x405670 (fld; fcomp flt_6545BC (0.1f - binary-verified 3DCCCCCD); test ah,1; jz skip - the {<, NaN} continue set; the .c's literal < would skip NaN)
            && v35->attribute_amount_held > 0.0f) {                      // asm 0x405672-0x405680 (fcomp const_f_zero; test ah,41h; jnz skip - the literal > MATCH row)
            v70 = (float)((double)v70 + (double)v35->attribute_amount_held + (double)0.01f); // asm 0x405682-0x405694 (fld var_8; fadd [ecx+54h]; fadd flt_654978 (0.01f - binary-verified 3C23D70A); fstp var_8 - ONE final rounding)
            v35->vfptr->set_attribute_amount(v35, 0, 0, 0);              // asm 0x40568A-0x405698 (the target's slot 63; args (0, 0, 0))
        }
        this_->obj->vfptr->set_attribute_amount(this_->obj, v70, 1, 0);  // asm 0x40569E-0x4056AB (the unit's ACTION vtbl slot 63 = +0xFC, idb_types.h:11679; args (float, 1, 0))
        v36 = this_->obj;                                                // asm 0x4056B1
        v71 = (float)fabs((double)v36->attribute_amount_held - (double)v72); // asm 0x4056B4-0x4056BD (fld; fsub var_4; fabs; fstp var_8 - the 53-bit sub; /Fa: cvtps2pd x2 + subsd + the fabs andps + cvtsd2ss)
        if (v36->attribute_amount_held >= (double)v72)                    // asm 0x4056C1-0x4056CD (fcomp var_4; test ah,1; jz LABEL_120 - the literal >= MATCH row: NaN falls into the tolerance block in both)
            goto LABEL_120;
        if (X87_LT_NAN(v71, 0.1f)) {                                     // asm 0x4056CF-0x4056DE (fld var_8; fcomp flt_6545BC; test ah,1; jz past - the {<, NaN} continue set)
            v37 = this_->target_obj;                                     // asm 0x4056E0 ([esi+10h])
            if (v37->attribute_amount_held >= (double)v71) {             // asm 0x4056E3-0x4056EF (fcomp var_8; test ah,1; jnz LABEL_120 - the literal >= MATCH row: NaN -> LABEL_120 in both)
                v69 = -v71;                                              // asm 0x4056F1-0x4056FB (fld var_8 - the ROUNDED v71; fchs; fstp)
                v37->vfptr->set_attribute_amount(v37, v69, 1, 0);        // asm 0x4056F7-0x405700 (the target's slot 63)
                this_->obj->vfptr->set_attribute_amount(this_->obj, v71, 1, 0); // asm 0x405706-0x405713 (the unit's slot 63)
            }
LABEL_120:
            if (X87_LT_NAN(v71, 0.1f))                                   // asm 0x405719-0x405728 (fld var_8; fcomp flt_6545BC; test ah,1; jnz loc_405754 - the {<, NaN} set; the .c's literal < would skip NaN to the target check)
                goto LABEL_189;
        }
        v38 = this_->target_obj;                                         // asm 0x40572A ([esi+10h])
        if (v38->attribute_amount_held > 0.0f                            // asm 0x40572D-0x40573B (the literal > MATCH row)
            || this_->obj->vfptr->keepGatheringWhenObjectIsOut(this_->obj, v38->master_obj->id)) { // asm 0x40573D-0x405752 (slot 113; test al,al; jnz default)
            return 0;
        }
        goto LABEL_189;
    case RGE_ACTION_STATE_RETURN:                                        // asm case 7 -> loc_405767
        if (X87_LT_NAN(this_->timer, 0.0)) {                             // asm 0x405767-0x405775 (fld timer; fcomp const_d_zero; test ah,1; jz sub-path - the {<, NaN} tick set; the .c's literal < would take NaN to the sub path)
            v39 = (double)this_->obj->owner->world->world_time_delta_seconds + (double)this_->timer; // asm 0x405777-0x40578C (the 53-bit tick; /Fa: cvtps2pd x2 + addsd)
            this_->timer = (float)v39;                                   // asm 0x40578C (fst)
            if (v39 > 0.0) {                                             // asm 0x40578F-0x40579A (fcomp const_d_zero; test ah,41h; jnz skip - the literal > MATCH row)
                this_->timer = 0.0f;                                     // asm 0x40579C ([esi+2Ch] = ebp)
LABEL_127:
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_RETURN); // asm 0x40579F-0x4057A5 (8)
            }
            return 0;                                                    // asm 0x4057A8 (default tail)
        }
        v40 = this_->sub_actions->vfptr->update(this_->sub_actions);     // asm 0x4057B2-0x4057B7 (slot 4; the AL read IS the mask)
        if (v40 != 1u) {                                                 // asm 0x4057BA-0x4057BC (cmp al, 1)
            if (v40 > 2u && v40 <= 5u)                                   // asm 0x4057BE-0x4057C4 (cmp 2; jbe / cmp 5; ja)
                goto LABEL_135;
            return 0;                                                    // asm 0x4057A8
        }
        if (this_->target_obj2) {                                        // asm 0x4057E2-0x4057E7 ([esi+14h])
            if (RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj2) > 1.0) { // asm 0x405ED-0x405801 (:3871 double; fcomp const_d_one; test ah,41h; jnz deposit - the literal > MATCH row: NaN deposits in both)
LABEL_135:
                v41 = this_->obj;                                        // asm 0x4057C6 ([esi+8])
                this_->timer = -3.0f;                                    // asm 0x4057C9/0x405806 ([esi+2Ch] = -3.0)
                v41->vfptr->stop_moving(v41);                            // asm 0x4057D2/0x40580F (slot 220 = +0x370)
                return 0;
            }
            v42 = this_->task->work_sound2;                              // asm 0x40581F-0x405822 ([esi+30h]; [ecx+48h] = 72)
            if (v42)
                RGE_Sound__play(v42, 1, 0, 0, -1, -1);                   // asm 0x405829-0x405831 (:3662 - (playType 1, pan 0, volume 0, player_id -1, culture -1))
            v43 = this_->task;                                           // asm 0x405836 ([esi+30h])
            v73 = v43->attribute_type3;                                  // asm 0x405839 ([edx+18h] = 24)
            if (v73 == -1)                                               // asm 0x40583D-0x405841 (cmp dx, 0FFFFh)
                v73 = this_->obj->attribute_type_held;                   // asm 0x405843-0x405846 (mov dx,[eax+5Ch])
            this_->obj->owner->vfptr->add_attribute_num(this_->obj->owner, // asm 0x40584A-0x40585D (the Player vtbl slot 32 = +0x80, idb_types.h:7177; (i16 type, float amount, u8 1))
                v73, this_->obj->attribute_amount_held, 1);
            this_->obj->owner->vfptr->trackUnitGather(this_->obj->owner, // asm 0x405863-0x405881 (the Player vtbl slot 24 = +0x60, :7169; (int id, int type, int amount) - the __ftol at 0x40586F = (int), the .c's (__int64) render is the artifact)
                this_->obj->id, v73, (int)this_->obj->attribute_amount_held);
            this_->obj->vfptr->set_attribute(this_->obj, -1, 0);         // asm 0x405881-0x40588D (the Action vtbl slot 62 = +0xF8)
            if (this_->target_obj) {                                     // asm 0x405893-0x405898 ([esi+10h])
                this_->vfptr->set_target_obj2(this_, 0);                 // asm 0x40589A-0x4058A0 (slot 23)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);   // asm 0x4058A3-0x4058A9 (push 4)
                result = 0;
            } else {
                if (!RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)) // asm 0x4058B6-0x4058C0 (:261; test eax,eax; jz LABEL_156)
                    goto LABEL_156;
                v44 = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj); // asm 0x4058C6-0x4058CE (the original re-calls unitAI - faithful)
                v45 = UnitAIModule__currentTargetType(v44);              // asm 0x4058D0 (:452)
                v46 = (RGE_Static_Object*)this_->obj;
                if (v45 == -1) {                                         // asm 0x4058D8-0x4058DB
                    v52 = RGE_Static_Object__unitAI(v46);                // asm 0x4058DD
                    v53 = UnitAIModule__lastTargetType(v52);             // asm 0x405907 (:453)
                    v54 = (RGE_Static_Object*)this_->obj;
                    if (v53 == -1) {                                     // asm 0x40590F-0x405912
                        v59 = RGE_Static_Object__unitAI(v54);            // asm 0x405937
                        v51 = v59->vfptr->convertToLOSResourceType(v59, this_->targetType); // asm 0x40593C-0x405944 (the UnitAI vtbl slot 47 = +0xBC, idb_types.h:7632)
                    } else {
                        v55 = RGE_Static_Object__unitAI(v54);            // asm 0x405914
                        v56 = v55->vfptr;                                // asm 0x40591E
                        v57 = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj); // asm 0x405920
                        v58 = UnitAIModule__lastTargetType(v57);         // asm 0x405927
                        v51 = v56->convertToLOSResourceType(v55, v58);   // asm 0x40592F (slot 47)
                    }
                } else {
                    v47 = RGE_Static_Object__unitAI(v46);                // asm 0x4058DD-region
                    v48 = v47->vfptr;                                    // asm 0x4058E7
                    v49 = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj); // asm 0x4058E9
                    v50 = UnitAIModule__currentTargetType(v49);          // asm 0x4058F0
                    v51 = v48->convertToLOSResourceType(v47, v50);       // asm 0x4058F8 (slot 47)
                }
                v60 = v51;
                if (v51 == -1
                    || ((v61 = (RGE_Static_Object*)this_->obj, !this_->target_obj2)
                        ? (v64 = RGE_Static_Object__unitAI(v61),
                           v63 = v64->vfptr->closestResourceObject(v64, v60, -1, -1, 0)) // asm 0x40596F-0x40597C (the UnitAI vtbl slot 16 = +0x40, :7601 - args (type, -1, -1, 0))
                        : (v62 = RGE_Static_Object__unitAI(v61),
                           v63 = v62->vfptr->closestResourceObject(v62, v60, this_->target_obj2->id, -1, 0)), // asm 0x40595B-0x40597C (args (type, target2->id, -1, 0))
                        v63 == -1)) {                                    // asm 0x405982-0x405985 (cmp eax, -1; jz LABEL_156)
LABEL_156:
                    if (this_->target_x == -1.0f || this_->target_y == -1.0f) { // asm 0x4059BF-0x4059CE (cmp ecx, eax(-1.0); cmp [esi+24h] - the RAW DWORD sentinel compares, the Explore precedent: the exact -1.0f bits, value-identical universally)
LABEL_162:
                        if (this_->obj->owner->vfptr->computerPlayer(this_->obj->owner) == 1) { // asm 0x405A10-0x405A1E (slot 7; cmp 1)
                            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE); // asm 0x405A20-0x405A26 (push 1)
                            result = 0;
                        } else {
LABEL_164:
                            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT); // asm 0x405A33-0x405A39 (push 2)
                            result = 0;
                        }
                    } else {
                        v66 = this_->obj->master_obj->id;                // asm 0x4059D0-0x4059DA
                        if (v66 == 259 || v66 == 214 || v66 == 13 && this_->targetType == 7) { // asm 0x4059DA-0x4059F4 (cmp 103h/0D6h/0Dh; cmp dword [esi+40h], 7)
LABEL_96:
                            this_->vfptr->set_state(this_, 243u);        // asm 0x4059FA-0x405A03 (push 0F3h - the internal F3 drop-off-timer state, the Gather-internal namespace)
                            result = 0;
                        } else {
LABEL_33:
                            this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE); // asm 0x405345-0x40534B (push 0Bh)
                            result = 0;
                        }
                    }
                } else {
                    v65 = RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, v63); // asm 0x405987-0x405999 (:6086)
                    if (!v65)
                        return 0;                                        // asm 0x40599B-0x4057A8 (default)
                    this_->vfptr->work(this_, v65, v65->world_x, v65->world_y, v65->world_z); // asm 0x4059A1-0x4059B2 (the Gather vftable slot 13 = +0x34 - the recursive work dispatch)
                    result = 0;
                }
            }
        } else {
LABEL_189:
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_RETURN);     // asm 0x405754-0x40575A (push 8)
            result = 0;
        }
        return result;
    case RGE_ACTION_STATE_MOVE:                                          // asm cases 10 -> loc_405109
    case RGE_ACTION_STATE_MOVE_NO_SEARCH:                                // asm case 14
    case 242:                                                            // asm case 241 - the internal F2
        switch (this_->sub_actions->vfptr->update(this_->sub_actions)) { // asm 0x405109-0x405120 (slot 4; the AL mask; jmp off_405D00[eax*4])
        case 1u:                                                         // asm off_405D00[0] -> loc_4051FF
        case 5u:                                                         // asm off_405D00[4] -> loc_4051FF
            if (this_->state != 242)                                     // asm 0x4051FF-0x405203 (cmp byte [esi+0Ch], 0F2h; jnz LABEL_52)
                goto LABEL_52;
            if (RGE_Action_Gather__auto_rebuild_farm(this_))             // asm 0x405205-0x40520E (mov ecx, esi; call - the DIRECT call; the idb constant routes into the adopted replacement)
                return 0;
            if (this_->farm_x != -1.0f)                                  // asm 0x405214-0x405217 (cmp [esi+54h], edi(-1.0) - the raw sentinel compare)
                (*base_game)->vfptr->notification(*base_game, 127, this_->obj->owner->id, this_->obj->id, 0, 0); // asm 0x405219-0x405236 (the Base_Game vtbl slot 17 = +0x44; push 7Fh)
LABEL_52:
            if (this_->state == RGE_ACTION_STATE_MOVE)                   // asm 0x405239-0x40523D (cmp byte [esi+0Ch], 0Bh)
                goto LABEL_53;
            goto LABEL_162;
        case 3u:                                                         // asm off_405D00[2] -> loc_405176
            if (this_->state != 242)                                     // asm 0x405176-0x40517A
                goto LABEL_42;
            if (RGE_Action_Gather__auto_rebuild_farm(this_))             // asm 0x40517C-0x405185
                return 0;
            if (this_->farm_x != -1.0f)                                  // asm 0x40518B-0x40518E
                (*base_game)->vfptr->notification(*base_game, 127, this_->obj->owner->id, this_->obj->id, 0, 0); // asm 0x405190-0x4051AD
LABEL_42:
            if (this_->obj->owner->vfptr->computerPlayer(this_->obj->owner) == 1) { // asm 0x4051B0-0x4051BE (slot 7)
LABEL_43:
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED); // asm 0x4051C0-0x4051C6 (push 0Dh)
                result = 0;
            } else {
                if (this_->state == RGE_ACTION_STATE_MOVE)               // asm 0x4051D3-0x4051D7
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH); // asm 0x4051D9-0x4051DF (push 3)
                else
                    this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT); // asm 0x4051EC-0x4051F2 (push 2)
                result = 0;
            }
            return result;
        case 4u:                                                         // asm off_405D00[3] -> loc_405127
            this_->vfptr->set_target_obj(this_, 0);                      // asm 0x405127-0x40512C (slot 22)
            v13 = this_->state;                                          // asm 0x40512F ([esi+0Ch])
            if (v13 == RGE_ACTION_STATE_MOVE)                            // asm 0x405132-0x405134
                goto LABEL_33;
            if (v13 == RGE_ACTION_STATE_MOVE_NO_SEARCH)                  // asm 0x405149-0x40514B
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE_NO_SEARCH); // asm 0x40514D-0x405153
            else
                this_->vfptr->set_state(this_, 242u);                    // asm 0x405160-0x405169 (push 0F2h)
            return 0;
        default:
            return 0;                                                    // asm off_405D00[1] -> loc_4057A8
        }
    case RGE_ACTION_STATE_FAILED:                                        // asm case 12 -> loc_405B78
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 505, RGE_UNITAI_ACTION_GATHER, 0, 0); // asm 0x405B78-0x405B8E (push 1F9h/261h - 505 = the blocked-enter notify, the 5xx candidate)
        if (this_->obj->owner->vfptr->computerPlayer(this_->obj->owner) == 1) // asm 0x405B94-0x405BA2
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);       // asm 0x405BA4-0x405BAA (push 1)
        else
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);       // asm 0x405BB7-0x405BBD (push 2)
        return 3;                                                        // asm 0x405BB0/0x405BC3 (mov al, 3)
    case 243:                                                            // asm case 242 -> loc_4050AB - the internal F3 drop-off-timer state
        if (this_->timer >= 0.0)                                         // asm 0x4050AB-0x4050B9 (the literal >= MATCH row)
            return 0;
        v12 = (double)this_->obj->owner->world->world_time_delta_seconds + (double)this_->timer; // asm 0x4050BF-0x4050D1 (the 53-bit tick; /Fa: cvtps2pd x2 + addsd)
        this_->timer = (float)v12;                                       // asm 0x4050D4 (fst)
        if (X87_LE_NAN(v12, 0.0))                                        // asm 0x4050D7-0x4050E2 (fcomp const_d_zero; test ah,41h; jnz default)
            return 0;
        if (!this_->have_worked)                                         // asm 0x4050E8-0x4050ED (mov al,[esi+52h]; test)
            goto LABEL_164;
        this_->vfptr->set_state(this_, 242u);                            // asm 0x4050F3-0x4050FC (push 0F2h)
        return 0;
    case 244:                                                            // asm case 243 -> loc_405A46 - the water-crossing marker pair
    case 247:                                                            // asm case 246
        if (!RGE_Action_List__have_action(this_->sub_actions)            // asm 0x405A46-0x405A50 (:72 - the AL read IS the u8; test)
            || !RGE_Action_List__get_action2(this_->sub_actions, 3))     // asm 0x405A56-0x405A62 (:79; push 3; test)
            return 0;
        if (this_->state == 244)                                         // asm 0x405A68-0x405A6C (cmp 0F4h)
            this_->vfptr->set_state(this_, 245u);                        // asm 0x405A6E-0x405A77 (push 0F5h)
        else
            this_->vfptr->set_state(this_, 248u);                        // asm 0x405A84-0x405A8D (push 0F8h)
        return 0;
    case 245:                                                            // asm case 244 -> loc_405A9A - the inside-container wait pair
    case 248:                                                            // asm case 247
        v67 = this_->sub_actions->vfptr->update(this_->sub_actions);     // asm 0x405A9A-0x405A9F (slot 4; the AL mask)
        if (v67 == 1u) {                                                 // asm 0x405AA2-0x405AA6 (cmp al, 1; bl = 0F5h held)
            if (this_->obj->inside_obj) {                                // asm 0x405ACC-0x405AD2 (cmp [eax+2Ch], ebp)
                if (this_->state == 245)                                 // asm 0x405AD4-0x405AD7
                    this_->vfptr->set_state(this_, 246u);                // asm 0x405AD9-0x405AE2 (push 0F6h)
                else
                    this_->vfptr->set_state(this_, 249u);                // asm 0x405AE7-0x405AF0 (push 0F9h)
            }
        } else if (v67 > 2u && v67 <= 5u) {                              // asm 0x405AA8-0x405AAE (cmp 2; jbe / cmp 5; ja)
            if (this_->state == 245)                                     // asm 0x405AB0-0x405AB3
                this_->vfptr->set_state(this_, 244u);                    // asm 0x405AB5-0x405ABC (push 0F4h)
            else
                this_->vfptr->set_state(this_, 247u);                    // asm 0x405ABE-0x405AC7 (push 0F7h)
        }
        if (!this_->obj->inside_obj)                                     // asm 0x405AF3-0x405AF9 (the shared tail)
            return 0;                                                    // asm 0x4057A8
        if (this_->state == 245)                                         // asm 0x405AFF-0x405B02
            this_->vfptr->set_state(this_, 246u);                        // asm 0x405B04-0x405B0D
        else
            this_->vfptr->set_state(this_, 249u);                        // asm 0x405B1A-0x405B23
        return 0;
    case 246:                                                            // asm case 245 -> loc_405B30
        if (!this_->obj->inside_obj)                                     // asm 0x405B30-0x405B36 (cmp [ecx+2Ch], ebp; jnz default)
            goto LABEL_187;
        return 0;
    case 249:                                                            // asm case 248 -> loc_405B4F
        if (!this_->obj->inside_obj)                                     // asm 0x405B4F-0x405B55
            goto LABEL_189;
        return 0;
    default:                                                             // asm byte_405C04 = 13 -> loc_4057A8
        return 0;                                                        // asm 0x4057A8-0x4057B1 (xor al,al)
    }
}

#pragma float_control(pop)

// ===========================================================================
// Replacement for RGE_Action_Gather::auto_rebuild_farm - THE VERBATIM
// ADOPTION (owner-approved; the entry-hook option (iii), the get_weapon
// precedent). Original: @ 0x004060B0 (idb_functions.h:51). Range
// [0x4060B0,0x406173) (lst :9174/:9262, retn), int return. ABI: __thiscall
// (this-only - the handler's dummy-less __fastcall is ABI-identical per
// the 2026-11-21 ruling). NOT virtual (a code-level helper; the vftable
// slots the SHARED family scalar-dtor at slot 0).
// ADOPTION MAPPING (the handler was the LIVE behavior - the review proves
// the mapping, not a re-derivation): the farm_x/-1.0 sentinel gate -> the
// asm 0x4060C8-0x4060DA (test ah,40h = the {==, NaN} set; DELIBERATE
// DELTA: the handler adds the farm_y conjunct and uses the literal ==
// - NaN unreachable in the shipped game state, flagged not fixed); the
// get_farm_queue_count <= 0 gate -> asm 0x4060B4-0x4060C2; the
// check_placement 12-arg dispatch (slot 14 = +0x38, idb_types.h:15896) ->
// asm 0x4060E0-0x406117; make_new_obj_2 (slot 34 = +0x88, :15916) ->
// asm 0x40611F-0x406130; the queue consume -> asm 0x40613C-0x406142;
// removeAllFromPathingGroup (:3889) -> asm 0x406147-0x40614A; do_command
// (the Action vtbl slot 68 = +0x110) -> asm 0x40614F-0x40615F.
// DELIBERATE DELTAS (the patch's own extensions, live behavior - baked):
// (a) the Aqua Harvester tier (group 14 -> master 199, v>=8, the aqua
// queue); (b) the auto-queue/pay machinery (try_auto_queue_farm +
// pay_obj_cost when the queue is empty - the original returns 0);
// (c) use_farm_queue -> TRIBE_Player__use_farm_queue_new(owner, id);
// (d) check_placement's out-params NULL vs the original's &scratch (the
// callee writes the obstruction out-param - the live handler ships NULL,
// its tolerance is the callee's unchanged behavior).
int __fastcall RGE_Action_Gather__auto_rebuild_farm_new(RGE_Action_Gather* action)
{
    if (action->farm_x == -1.0f || action->farm_y == -1.0f)
        return 0;

    __int16 farm_obj_id;
    int count;
    if (action->obj->master_obj->object_group == 14)
    {
        if (current_loaded_version < 8)
            return 0;
        farm_obj_id = AQUA_HARVESTER_ID;
        count = action->obj->owner->aqua_harvester_queue_count_new;
    }
    else
    {
        farm_obj_id = 50;
        if (current_loaded_version >= 8)
            count = action->obj->owner->farm_queue_count_new;
        else
            count = action->obj->owner->farm_queue_count;
    }

    if (count <= 0)
    {
        if (!try_auto_queue_farm(action->obj->owner, farm_obj_id))
            return 0;
    }

    TRIBE_Master_Building_Object* farm_bldg = (TRIBE_Master_Building_Object*)action->obj->owner->master_objects[farm_obj_id];
    if (farm_bldg->vfptr->check_placement(farm_bldg, (RGE_Player*)action->obj->owner, action->farm_x, action->farm_y, NULL, 1, 1, 1, 1, 1, 1, NULL, 0))
        return 0;

    RGE_Static_Object* new_farm = farm_bldg->vfptr->make_new_obj_2(farm_bldg, (RGE_Player*)action->obj->owner, action->farm_x, action->farm_y, 0.0f, -1);
    if (!new_farm)
        return 0;

    if (count <= 0)
        pay_obj_cost(action->obj->owner, farm_obj_id);
    else
        TRIBE_Player__use_farm_queue_new(action->obj->owner, farm_obj_id);

    RGE_Static_Object__removeAllFromPathingGroup((RGE_Static_Object*)action->obj);
    action->obj->vfptr->do_command(action->obj, new_farm, action->farm_x, action->farm_y, 0.0f);

    return 1;
}

// ===========================================================================
// DIAG — THE SET_STATE A/B HARNESS (the diagnostic process; REMOVE AT
// SHIP). The logger-recovery-rule shape (the 2fc06f1 exemplar): the
// naked _orig trampoline with the byte-exact prologue replay, the
// one-incremental-line logger, the one-global flip.
// THE TRAMPOLINE: the E9 at 0x404490 covers 0x404490-0x404494 = the
// first 5 bytes of the 6-byte `mov eax, large fs:0` (0x404490-0x404495)
// - the replay is the ENTIRE mov (eax then carries the SEH chain head
// for the body's `push eax` at 0x40449D - the address setup must NOT
// use eax), the resume = 0x404496 (the .lst's next-instruction
// boundary). THE +4 COMPENSATION (`add esp, 4`): the harness chain is
// call-based (the game's call -> the E9 -> jmp -> the logger -> call ->
// _orig), so the body's entry esp would sit 4 bytes LOW and the body's
// mid-function arg read `mov ebx, [esp+20h+new_state]` (0x4044A9) would
// hit the logger's return slot; +4 restores the E9-flow frame. ecx (the
// __thiscall this) survives untouched; edx (the dead dummy) carries the
// resume address. THE VERIFICATION: row 1's new_state=... must match
// the live state transitions - a garbage new_state means the
// compensation is mis-sized.
__declspec(naked) void __fastcall RGE_Action_Gather__set_state_orig(
    RGE_Action_Gather* this_, DWORD dummy, unsigned __int8 new_state)
{
    __asm
    {
        mov     eax, fs:[0]
        push    00404496h
        ret
    }
}

static int g_setStateTraceOrig = 0;   // 1 = the vanilla trace (build A), 0 = set_state_new (build B)

static void __fastcall RGE_Action_Gather__set_state_logger(
    RGE_Action_Gather* this_, DWORD dummy, unsigned __int8 new_state)
{
    char b[0x200];
    // the extended capture: the unit position (the walk result), the
    // multi-stop state, the farm snapshot, and the FIRST sub-action's
    // identity + walk target (the Goto/Return arms' created Move_To -
    // the farm_walk +0.5 offset detector)
    int saT = -1; float saX = 0.0f, saY = 0.0f; int saSt = -1; float tileAmt = -1.0f;
    // DIAG: the rand GENERATOR STATE read non-invasively (0x0069EC98 =
    // the game CRT's rand seed dword; the MSVC LCG seed*214013+2531011,
    // the draw = (seed>>16)&0x7fff) - no draw is consumed, the stream is
    // untouched, and the logged state lets the arm's draws be recomputed
    // exactly from the row.
    unsigned int rnd = *(unsigned int*)0x0069EC98;
    if (this_->sub_actions && this_->sub_actions->list && this_->sub_actions->list->action)
    {
        saT = this_->sub_actions->list->action->action_type;
        saX = this_->sub_actions->list->action->target_x;
        saY = this_->sub_actions->list->action->target_y;
        saSt = this_->sub_actions->list->action->state;
    }
    if (this_->target_obj)
        tileAmt = this_->target_obj->attribute_amount_held;
    sprintf(b, "ss-in id=%d rnd=%08X ns=%u st=%u nstops=%u cstop=%u hw=%u amt=%f tps=%f tacs=%f tile=%f tgt=%d tgt2=%d tx=%f ty=%f tz=%f wx=%f wy=%f lsx=%d lsy=%d fx=%f fy=%f saT=%d saSt=%d saX=%f saY=%f\n",
        (int)this_->obj->id,
        rnd,
        (unsigned)new_state, (unsigned)this_->state,
        (unsigned)this_->numberStops, (unsigned)this_->currentStop,
        (unsigned)this_->have_worked, this_->obj->attribute_amount_held,
        this_->timePerStop, this_->timeAtCurrentStop,
        tileAmt,
        this_->targetID, this_->target2ID,
        this_->target_x, this_->target_y, this_->target_z,
        this_->obj->world_x, this_->obj->world_y,
        (int)this_->lastStopX, (int)this_->lastStopY,
        this_->farm_x, this_->farm_y,
        saT, saSt, saX, saY);
    if (g_setStateTraceOrig)
        RGE_Action_Gather__set_state_orig(this_, 0, new_state);
    else
        RGE_Action_Gather__set_state_new(this_, 0, new_state);
    float tileOut = -1.0f;
    if (this_->target_obj)
        tileOut = this_->target_obj->attribute_amount_held;
    sprintf(b + strlen(b), "ss-out st=%u timer=%f tps=%f tacs=%f tile=%f tgt=%d tgt2=%d cstop=%u wx=%f wy=%f lsx=%d lsy=%d saT=%d saSt=%d saX=%f saY=%f\n",
        (unsigned)this_->state, this_->timer,
        this_->timePerStop, this_->timeAtCurrentStop,
        tileOut,
        this_->targetID, this_->target2ID,
        (unsigned)this_->currentStop,
        this_->obj->world_x, this_->obj->world_y,
        (int)this_->lastStopX, (int)this_->lastStopY,
        saT, saSt, saX, saY);

    static FILE* ssLog = NULL;
    if (!ssLog)
        ssLog = fopen("val_log.txt", "at");

    fputs(b, ssLog);
}

void install_RGE_Action_Gather()
{
    setHook((void*)0x00404080, RGE_Action_Gather__RGE_Action_Gather3_new);
    setHook((void*)0x00404160, RGE_Action_Gather__RGE_Action_Gather_new);
    setHook((void*)0x00404290, RGE_Action_Gather__RGE_Action_Gather2_new);
    setHook((void*)0x00404300, RGE_Action_Gather__save_new);
    setHook((void*)0x00404380, RGE_Action_Gather__first_in_stack_new);
    // DIAG: the set_state hook flipped to the logger (the A/B harness;
    // flip g_setStateTraceOrig for the build A/B; REMOVE AT SHIP)
    //setHook((void*)0x00404490, RGE_Action_Gather__set_state_logger);
    setHook((void*)0x00404490, RGE_Action_Gather__set_state_new);
    setHook((void*)0x00404EE0, RGE_Action_Gather__update_new);
    setHook((void*)0x00405D40, RGE_Action_Gather__get_move_sprite_new);
    setHook((void*)0x00405DF0, RGE_Action_Gather__get_wait_sprite_new);
    setHook((void*)0x00405E80, RGE_Action_Gather__work_new);
    setHook((void*)0x004060B0, RGE_Action_Gather__auto_rebuild_farm_new);
}
