#include "stdafx.h"

// BAKED-PATCH helpers (the casts.cpp / los.cpp storage schemes this
// cluster must reproduce):
// casts.cpp:763 (__fastcall, naked) � frees the patch's UNIT_EXTRA
// (allocated with the patch's own malloc, freed with the patch's own
// free inside this helper) and clears the pointer storage.
extern void __fastcall removeUnitExtra(RGE_Static_Object* unit);
// casts.cpp:18 (__stdcall) - the patch's UNIT_EXTRA per-frame processor
// (the effect-timer decay, the hp drain, the construction solidify
// counter, the keepUnitExtra cleanup); baked into update_new (the
// processUnitHook entry-fight disposition).
extern void __stdcall processUnitExtra(RGE_Static_Object* unit);
// los.cpp:6 (__stdcall) � frees the patch's LOS data chain. (The
// pointer lives in ei.theObj � an idb-typed RGE_Static_Object* slot
// the patch repurposes for its UNIT_LOS_DATA*.)
struct UNIT_LOS_DATA;
extern void __stdcall delete_unit_los_data(UNIT_LOS_DATA* los_data);

// jedimaster.cpp globals (the BAKED gbg_isPadawan conditional - the
// EXPL_DROID precedent; the externs cpp-local per the SPEED2 form)
extern bool padawans_installed;
extern __int16* padawans_list;
extern int padawans_n;
extern bool masters_installed;
extern __int16* masters_list;
extern int masters_n;

// .rdata vftable of RGE_Static_Object (.rdata:00662C8C, lst :994791)
static RGE_Static_ObjectVtbl* const rge_static_object_vftable =
    (RGE_Static_ObjectVtbl*)0x00662C8C;

// Replacement for RGE_Static_Object::gbg_get_civ_override
// Original: RGE_Static_Object__gbg_get_civ_override @ 0x0054B3E0 (idb_functions.h:3776)
// ABI: __thiscall � this_ in ECX, no stack arguments, int in EAX.
// No EDX dummy needed (zero-arg method; precedent: land.cpp:4).
// BAKE-IN (langdll.cpp:306 @ 0x0054B3E9): original compares against 9;
// the patched immediate value is CIV_COUNT + 1 (consts.h:3 = 15 > 16).
int __fastcall RGE_Static_Object__gbg_get_civ_override_new(RGE_Static_Object* this_)
{
    int result = this_->gbg_civ_override;
    if (result <= 0 || result >= CIV_COUNT + 1)
        result = this_->owner->culture;
    return result;
}

// Replacement for RGE_Static_Object::RGE_Static_Object (runtime ctor)
// Original: @ 0x0054B2E0 (idb_functions.h:3774). Range [0x54B2E0,
// 0x54B366) (lst :551419/:551474, retn 18h = 6 stack args). ABI:
// __thiscall - this_ in ECX, returns this. Callers: the master
// factories (owned, hook-routed), RGE_Animated_Object__RGE_Animated_
// Object (lst :551420).
// Decode (asm 0x54B2E0-0x54B365): zoneMapIndex = -1, mGroupID = -1,
// then the zero block (shield_hit_timer/sprite/old_sprite/CurrentObstruction
// Data/ObstructionDataList/mPathingGroup/mPathingGroupSize/mMaximum
// PathingGroupSize/unitAIValue/inObstructionMapValue/lastInObstruction
// MapValue/underAttackValue), vfptr, gbg_civ_override = owner->culture
// (byte @0xA5 zero-extended; edx was zeroed), player_object_node = 0,
// if (do_setup) setup(tobj, obj_owner, x, y, z).
// BAKED (casts.cpp:897 writeData @0x0054B315, 18 bytes over the three
// byte-stores at +0x84/+0x85/+0x86): the patched binary additionally
// zeroes damaged_lately_timer and unit_extra_padding_1/2/3 (dword store
// at +0x78) and unit_extra_padding_4 (dword store at +0x84 covering
// +0x84..+0x87) - the UNIT_EXTRA pointer storage the patch assembles
// via getUnitExtra/addUnitExtra must read NULL on a fresh object.
// BAKED (los.cpp:1023 hook @0x0054B33F): ei.theObj = 0 (the los patch's
// UNIT_LOS_DATA pointer slot; the stub replays the do_setup load and
// compare it displaces).
RGE_Static_Object* __fastcall RGE_Static_Object__RGE_Static_Object_new(
    RGE_Static_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* tobj, TRIBE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    this_->zoneMapIndex = -1;
    this_->mGroupID = -1;
    this_->shield_hit_timer = 0.0f;
    this_->sprite = 0;
    this_->old_sprite = 0;
    this_->CurrentObstructionData = 0;
    this_->ObstructionDataList = 0;
    this_->mPathingGroup = 0;
    this_->mPathingGroupSize = 0;
    this_->mMaximumPathingGroupSize = 0;
    this_->unitAIValue = 0;
    this_->inObstructionMapValue = 0;                             // asm 0x54B315-0x54B326 (original byte-stores)
    this_->lastInObstructionMapValue = 0;
    this_->underAttackValue = 0;
    this_->damaged_lately_timer = 0;                              // BAKED: casts.cpp:897 @ 0x0054B315
    this_->unit_extra_padding_1 = 0;                              // BAKED: UNIT_EXTRA pointer storage
    this_->unit_extra_padding_2 = 0;                              //   (the patch's dword clears at
    this_->unit_extra_padding_3 = 0;                              //    +0x78-0x7B and +0x84-0x87)
    this_->unit_extra_padding_4 = 0;
    this_->mROOAlreadyCalled = 0;                                 // asm 0x54B327 (outside the writeData region)
    this_->vfptr = rge_static_object_vftable;
    this_->gbg_civ_override = obj_owner->culture;
    this_->player_object_node = 0;
    this_->ei.theObj = 0;                                         // BAKED: los.cpp:1023 @ 0x0054B33F
    if (do_setup != 0)
        RGE_Static_Object__setup(this_, tobj, (RGE_Player*)obj_owner, x, y, z);
    return this_;
}

// Replacement for RGE_Static_Object::RGE_Static_Object_read (ctor from
// stream) � Original: @ 0x0054B600 (idb_functions.h:3787). Range
// [0x54B600, 0x54B66F) (lst :551882/:551926, retn 0Ch = 3 stack args).
// Same init set as the ctor EXCEPT gbg_civ_override = 0 (the ctor
// stores owner->culture; the read path zeroes it and setup_read fills
// it). BAKED (casts.cpp:898 writeData @0x0054B638): the same dword-
// clear pair as the ctor. BAKED (los.cpp:1024 hook @0x0054B61D):
// ei.theObj = 0 (the stub replays the +0xC/+0x1C stores it displaces).
RGE_Static_Object* __fastcall RGE_Static_Object__RGE_Static_Object_read_new(
    RGE_Static_Object* this_, DWORD dummy,
    int infile, RGE_Game_World* gworld, int do_setup)
{
    this_->zoneMapIndex = -1;
    this_->mGroupID = -1;
    this_->gbg_civ_override = 0;
    this_->ei.theObj = 0;                                         // BAKED: los.cpp:1024 @ 0x0054B61D
    this_->shield_hit_timer = 0.0f;
    this_->sprite = 0;
    this_->old_sprite = 0;
    this_->CurrentObstructionData = 0;
    this_->ObstructionDataList = 0;
    this_->mPathingGroup = 0;
    this_->mPathingGroupSize = 0;
    this_->mMaximumPathingGroupSize = 0;
    this_->unitAIValue = 0;
    this_->inObstructionMapValue = 0;
    this_->lastInObstructionMapValue = 0;
    this_->underAttackValue = 0;
    this_->damaged_lately_timer = 0;                              // BAKED: casts.cpp:898 @ 0x0054B638
    this_->unit_extra_padding_1 = 0;
    this_->unit_extra_padding_2 = 0;
    this_->unit_extra_padding_3 = 0;
    this_->unit_extra_padding_4 = 0;
    this_->mROOAlreadyCalled = 0;                                 // asm 0x54B64A (outside the writeData region)
    this_->vfptr = rge_static_object_vftable;
    this_->player_object_node = 0;
    if (do_setup != 0)
        RGE_Static_Object__setup_read(this_, infile, gworld);
    return this_;
}

// Replacement for RGE_Static_Object::destructor (runtime)
// Original: @ 0x0054B680 (idb_functions.h:3788). Range [0x54B680,
// 0x54B86A) (lst :551935/:552160), void result. ABI: __thiscall.
// BAKED (casts.cpp:895 entry hook @0x0054B680): removeUnitExtra runs
// before the body (the stub replays the prologue verbatim; the patch's
// malloc/free pairing stays inside casts.cpp).
// BAKED (los.cpp:1025 hook @0x0054B691): free ei.theObj via
// delete_unit_los_data and null it.
// Body decode (asm 0x54B686-0x54B869): vfptr store;
// remove_visible_resource; if owner: if object_state < 3 - the attribute
// refund loop (master->attribute_flag[i] == 2 && attribute_type_held[i]
// >= 0 -> owner->vfptr->add_attribute_num(owner, type, -amount, 0),
// 3 iterations over attribute_amount_held[3]); if object_state < 7 ->
// owner->victory_conditions->update_for_object(this); sprite_list ->
// vfptr[0](1) (scalar-delete) + null; if owner: selected & 1 ->
// unselect_one_object, then owner->vfptr->removeObject(owner, this,
// sleep_flag, dopple_flag, player_object_node), owner = 0; tile &&
// !ObjectDeleteLock -> OL_DeleteObjectListEntry((*base_game)->world->
// map, tile, this), tile = 0; obstruction: master radius_z > 0 &&
// mROOAlreadyCalled == 0 -> (Air if master->movement_type == 1 else
// Land)->RemoveObjectObstruction(this); ObstructionDataList ->
// (Air/Land)->DeleteObList(this); inside_obj -> Remove_node
// (inside_obj->objects, this), null; objects -> exit_obj loop over
// Number_of_objects (descending), New_Object_List__destructor +
// operator_delete, null; mGroupID != -1 -> getUnitGroup + removeUnit;
// unitAIValue -> vfptr[0](1) + null; mPathingGroup -> operator_delete
// + null; mPathingGroupSize/mMaximumPathingGroupSize = 0.
void __fastcall RGE_Static_Object__destructor_new(
    RGE_Static_Object* this_)
{
    removeUnitExtra(this_);                                       // BAKED: casts.cpp:895 @ 0x0054B680
    this_->vfptr = rge_static_object_vftable;
    RGE_Static_Object__remove_visible_resource(this_);
    if (this_->ei.theObj)                                         // BAKED: los.cpp:1025 @ 0x0054B691 (the hook sits
    {                                                             //   AFTER remove_visible_resource in the patched flow)
        delete_unit_los_data((UNIT_LOS_DATA*)this_->ei.theObj);   // the patch stores UNIT_LOS_DATA* in this slot
        this_->ei.theObj = 0;
    }
    if (this_->owner != 0)
    {
        if (this_->object_state < 3)
        {
            RGE_Master_Static_Object* master = this_->master_obj;
            for (size_t i = 0; i < _countof(master->attribute_flag); ++i)
            {
                if (master->attribute_flag[i] == 2
                    && master->attribute_type_held[i] >= 0)
                    ((RGE_Player*)this_->owner)->vfptr->add_attribute_num(
                        (RGE_Player*)this_->owner,
                        master->attribute_type_held[i],
                        -master->attribute_amount_held[i],
                        0);
            }
        }
        if (this_->object_state < 7)
            RGE_Victory_Conditions__update_for_object(this_->owner->victory_conditions, this_);
    }
    if (this_->sprite_list != 0)
    {
        this_->sprite_list->vfptr->__vecDelDtor(this_->sprite_list, 1);
        this_->sprite_list = 0;
    }
    if (this_->owner != 0)
    {
        if (this_->selected & 1)
            RGE_Player__unselect_one_object((RGE_Player*)this_->owner, this_);
        ((RGE_Player*)this_->owner)->vfptr->removeObject(
            (RGE_Player*)this_->owner, this_,
            this_->sleep_flag, this_->dopple_flag,
            this_->player_object_node);
        this_->owner = 0;
    }
    if (this_->tile != 0 && this_->tile->ObjectDeleteLock == 0)
    {
        RGE_Map__OL_DeleteObjectListEntry((*base_game)->world->map, this_->tile, this_);
        this_->tile = 0;
    }
    if (this_->master_obj != 0 && this_->master_obj->radius_z > 0.0f
        && this_->mROOAlreadyCalled == 0)
    {
        if (this_->master_obj->movement_type == 1)
            (*OBSystem_Air)->vfptr->RemoveObjectObstruction(*OBSystem_Air, this_);
        else
            (*OBSystem_Land)->vfptr->RemoveObjectObstruction(*OBSystem_Land, this_);
    }
    if (this_->ObstructionDataList != 0)
    {
        if (this_->master_obj != 0 && this_->master_obj->movement_type == 1)
            (*OBSystem_Air)->vfptr->DeleteObList(*OBSystem_Air, this_);
        else
            (*OBSystem_Land)->vfptr->DeleteObList(*OBSystem_Land, this_);
    }
    if (this_->inside_obj != 0)
    {
        RGE_New_Object_List__Remove_node(this_->inside_obj->objects, this_);
        this_->inside_obj = 0;
    }
    if (this_->objects != 0)
    {
        for (int i = this_->objects->Number_of_objects - 1; i >= 0; --i)
            this_->objects->List[i]->vfptr->exit_obj(this_->objects->List[i]);
        RGE_New_Object_List__destructor(this_->objects);
        operator_delete_internal(this_->objects);
        this_->objects = 0;
    }
    if (this_->mGroupID != -1)
    {
        BUnitGroup* group = RGE_Game_World__getUnitGroup(
            (RGE_Game_World*)(*base_game)->world, this_->mGroupID);
        if (group != 0)
            BUnitGroup__removeUnit(group, this_->id);
    }
    if (this_->unitAIValue != 0)
    {
        this_->unitAIValue->vfptr->__vecDelDtor(this_->unitAIValue, 1);
        this_->unitAIValue = 0;
    }
    if (this_->mPathingGroup != 0)
    {
        operator_delete_internal(this_->mPathingGroup);
        this_->mPathingGroup = 0;
    }
    this_->mPathingGroupSize = 0;
    this_->mMaximumPathingGroupSize = 0;
}

// Replacement for RGE_Static_Object::recycle_out_of_game (runtime)
// Original: @ 0x0054C0A0 (idb_functions.h:3802). Range [0x54C0A0,
// 0x54C293) (lst :553200/:553440), void result. ABI: __thiscall.
// BAKED (casts.cpp:896 entry hook @0x0054C0A0) + BAKED (los.cpp:1026
// hook @0x0054C0A7) - same prepends as the destructor.
// Body decode (asm 0x54C0A7-0x54C292): if !master->recyclable (@+0xBC)
// -> vfptr[0](1) self-destruct + return; owner->world->
// recycle_object_out_of_game(master_type, this) == 0 -> self-destruct +
// return; if object_state < 3 - the same attribute refund loop; if
// object_state < 7 - victory update; sprite_list ->
// RGE_Active_Sprite_List__delete_list (DIRECT call - the destructor
// uses vfptr[0](1) instead); unselect + removeObject + owner = 0;
// tile -> OL_DeleteObjectListEntry + null; inside_obj -> Remove_node +
// null; objects -> exit_obj loop + list destructor + delete + null;
// unitAIValue -> release + null; mPathingGroupSize = 0 (NO
// mPathingGroup free, NO mMaximumPathingGroupSize clear, NO
// mGroupID/BUnitGroup removal - faithful recycling differences);
// master radius_z > 0 -> (Air/Land by movement_type)->RemoveObjectObstruction
// (this); ObstructionDataList -> (Air/Land)->DeleteObList +
// master_obj = 0 + return.
void __fastcall RGE_Static_Object__recycle_out_of_game_new(
    RGE_Static_Object* this_)
{
    removeUnitExtra(this_);                                       // BAKED: casts.cpp:896 @ 0x0054C0A0
    if (this_->ei.theObj)                                         // BAKED: los.cpp:1026 @ 0x0054C0A7
    {
        delete_unit_los_data((UNIT_LOS_DATA*)this_->ei.theObj);   // the patch stores UNIT_LOS_DATA* in this slot
        this_->ei.theObj = 0;
    }
    if (this_->master_obj == 0 || this_->master_obj->recyclable == 0)
    {
        if (this_ != 0)
            this_->vfptr->__vecDelDtor(this_, 1);                 // non-recyclable: self-destruct
        return;
    }
    if (RGE_Game_World__recycle_object_out_of_game(
            (RGE_Game_World*)this_->owner->world, this_->master_obj->master_type, this_) == 0)
    {
        if (this_ != 0)
            this_->vfptr->__vecDelDtor(this_, 1);                 // recycle failed: self-destruct
        return;
    }
    if (this_->object_state < 3)
    {
        RGE_Master_Static_Object* master = this_->master_obj;
        for (size_t i = 0; i < _countof(master->attribute_flag); ++i)
        {
            if (master->attribute_flag[i] == 2
                && master->attribute_type_held[i] >= 0)
                ((RGE_Player*)this_->owner)->vfptr->add_attribute_num(
                    (RGE_Player*)this_->owner,
                    master->attribute_type_held[i],
                    -master->attribute_amount_held[i],
                    0);
        }
    }
    if (this_->object_state < 7)
        RGE_Victory_Conditions__update_for_object(this_->owner->victory_conditions, this_);
    if (this_->sprite_list != 0)
        RGE_Active_Sprite_List__delete_list(this_->sprite_list);
    if (this_->owner != 0)
    {
        if (this_->selected & 1)
            RGE_Player__unselect_one_object((RGE_Player*)this_->owner, this_);
        ((RGE_Player*)this_->owner)->vfptr->removeObject(
            (RGE_Player*)this_->owner, this_,
            this_->sleep_flag, this_->dopple_flag,
            this_->player_object_node);
        this_->owner = 0;
    }
    if (this_->tile != 0 && this_->tile->ObjectDeleteLock == 0)
    {
        RGE_Map__OL_DeleteObjectListEntry((*base_game)->world->map, this_->tile, this_);
        this_->tile = 0;
    }
    if (this_->inside_obj != 0)
    {
        RGE_New_Object_List__Remove_node(this_->inside_obj->objects, this_);
        this_->inside_obj = 0;
    }
    if (this_->objects != 0)
    {
        for (int i = this_->objects->Number_of_objects - 1; i >= 0; --i)
            this_->objects->List[i]->vfptr->exit_obj(this_->objects->List[i]);
        RGE_New_Object_List__destructor(this_->objects);
        operator_delete_internal(this_->objects);
        this_->objects = 0;
    }
    if (this_->unitAIValue != 0)
    {
        this_->unitAIValue->vfptr->__vecDelDtor(this_->unitAIValue, 1);
        this_->unitAIValue = 0;
    }
    this_->mPathingGroupSize = 0;
    if (this_->master_obj != 0 && this_->master_obj->radius_z > 0.0f)
    {
        if (this_->master_obj->movement_type == 1)
            (*OBSystem_Air)->vfptr->RemoveObjectObstruction(*OBSystem_Air, this_);
        else
            (*OBSystem_Land)->vfptr->RemoveObjectObstruction(*OBSystem_Land, this_);
    }
    if (this_->ObstructionDataList != 0)
    {
        if (this_->master_obj != 0 && this_->master_obj->movement_type == 1)
        {
            (*OBSystem_Air)->vfptr->DeleteObList(*OBSystem_Air, this_);
            this_->master_obj = 0;
            return;
        }
        (*OBSystem_Land)->vfptr->DeleteObList(*OBSystem_Land, this_);
    }
    this_->master_obj = 0;
}

// ============ THE SHIELD UPDATE SIDE ============

// Replacement for RGE_Static_Object::gbg_doShieldRecharge. Original:
// @ 0x0054ED30 (idb_functions.h:3822). Range [0x54ED30, 0x54EE68)
// (lst :557918/:558048, retn = no stack args), void. ABI: __thiscall.
// Virtual: vftable DATA XREF (the runtime vftables slot it; the entry
// hook covers every virtual dispatch). Caller: RGE_Static_Object::
// update @0x54EF00 dispatches it via vfptr->gbg_doShieldRecharge when
// is_shielded && master hp >= sp. Semantics: grow the shield
// charge-tick accumulator by the world time delta; each crossing of
// attributes[25] (the owner-supplied Shield recharge interval) fires
// one tick - the gain is tier-scaled by the sp bands (2/4/8/12/16/20
// at 100/1000/2000/3000/4000) x attributes[10] (the recharge
// multiplier), added to sp; when sp reaches master->hp (the i16
// @+0x32 - the shield max doubles as the master's hp field) sp is
// pinned to it and the ticks stop.
// Decode (asm 0x54ED30-0x54EE68): the entry gate fld attributes[25]/
// fcomp/test ah,41h/jnz -> return - MATCH row (the literal >). The
// accumulator add fld/fadd/fst m32 - the store-round lands BEFORE the
// proceed compare, which consumes the UNROUNDED x87 value - the
// double form (rule (a)); the proceed gate test ah,1/jnz - MATCH row
// (the literal >=; NaN returns in both). The tick fld/fsub/fstp is a
// single subtraction feeding only the store (rule (d), float-safe).
// The band cascade: five fld sp/fcomp const/test ah,1/jz gates (MATCH
// rows, rule (b) bit-safe) - the fallthroughs pick 2.0/4.0/8.0
// (flt_657DC0, the game-facts constant)/12.0/16.0, the final jz path
// 20.0; the .c's nested rendering kept. The gain gate fld
// attributes[10]/fcomp/test ah,41h/jnz skips the multiply - MATCH
// row. The chain fmul/fadd/fst m32 = ONE store-round; the cap compare
// consumes the UNROUNDED sum vs master->hp (movsx word @+0x32/fild/
// fxch/fcompp - the exact integer widened); test ah,1/jz -> cap -
// MATCH row (NaN continues, matching).
// THE MACRO SITE: the tick-continue gate @0x54EE38-0x54EE4C is the
// FALLTHROUGH-FIRES-ON-UNORDERED class - test ah,1/jz -> the next
// tick; {<, NaN} falls through to the RETURN - the .c's literal `<`
// would CONTINUE on NaN (the calculateYawAngle lesson) ->
// X87_LT_NAN.
// Restructure: the .c's while(1)/break + post-loop cap kept as the
// guard + return form - the same path set (the break exits to the
// post-loop cap; the cap inside the guard returns directly).
void __fastcall RGE_Static_Object__gbg_doShieldRecharge_new(
    RGE_Static_Object* this_, DWORD dummy)
{
    TRIBE_Player* owner = this_->owner;                              // asm 0x54ED32 (@+0x18, the read-once form)
    if (owner->attributes[25] > 0.0f)                                // asm 0x54ED36-0x54ED4A (fld [attributes+64h]/fcomp const_f_zero/test ah,41h/jnz -> return; MATCH row)
    {
        double accumulated = (double)owner->world->world_time_delta_seconds   // asm 0x54ED50-0x54ED5C (world @+0x8C; the field @+0xA8; fld/fadd - the 53-bit sum)
            + (double)this_->shield_recharge_accumulator;            // (the accumulator @+0x8)
        this_->shield_recharge_accumulator = (float)accumulated;     // asm 0x54ED5F (fst m32 - the store-round)
        if (accumulated >= (double)owner->attributes[25])            // asm 0x54ED68-0x54ED73 (fcomp m32; test ah,1/jnz -> return; MATCH row: NaN returns in both)
        {
            for (;;)
            {
                this_->shield_recharge_accumulator -= owner->attributes[25];   // asm 0x54ED7C-0x54ED81 (fld/fsub/fstp - rule (d) float-safe; the asm holds &attributes[25] in edx - the indexed re-read is identical, no in-loop writes)
                double gain;
                if (this_->sp >= 100.0f)                             // asm 0x54ED84-0x54ED92 (fld sp/fcomp flt_6565EC/test ah,1/jz next; MATCH row)
                {
                    if (this_->sp >= 1000.0f)                        // asm 0x54ED9C-0x54EDAA (flt_662B80)
                    {
                        if (this_->sp >= 2000.0f)                    // asm 0x54EDB4-0x54EDC2 (flt_662B84)
                        {
                            if (this_->sp >= 3000.0f)                // asm 0x54EDCC-0x54EDDA (flt_663000)
                                gain = (this_->sp >= 4000.0f) ? 20.0 : 16.0;   // asm 0x54EDE4-0x54EDFA (flt_662FF8; 20 = flt_65ABEC, 16 = flt_659EB8; MATCH row)
                            else
                                gain = 12.0;                         // asm 0x54EDDC-0x54EDE2 (flt_662FFC)
                        }
                        else
                            gain = 8.0;                              // asm 0x54EDC4-0x54EDCA (flt_657DC0 = the game-facts 8.0f constant)
                    }
                    else
                        gain = 4.0;                                  // asm 0x54EDAC-0x54EDB2 (flt_65AB38)
                }
                else
                    gain = 2.0;                                      // asm 0x54ED94-0x54ED9A (flt_6552F4)
                if (owner->attributes[10] > 0.0f)                    // asm 0x54EE02-0x54EE16 (fld [attributes+28h]/fcomp/test ah,41h/jnz skips the multiply; MATCH row)
                    gain = gain * (double)owner->attributes[10];     // asm 0x54EE18 (fmul - the 53-bit product)
                double new_sp = gain + (double)this_->sp;            // asm 0x54EE1B (fadd)
                this_->sp = (float)new_sp;                           // asm 0x54EE1E (fst m32 - the store-round)
                if (new_sp >= (double)this_->master_obj->hp)         // asm 0x54EE21-0x54EE36 (movsx word @+0x32; fild; fxch; fcompp; test ah,1/jz -> cap; MATCH row: NaN continues, matching)
                {
                    this_->sp = (float)this_->master_obj->hp;        // asm 0x54EE56-0x54EE62 (fild/fstp m32 - the pin)
                    return;                                          // asm -> loc_54EE65 (the epilogue)
                }
                if (X87_LT_NAN(this_->shield_recharge_accumulator, owner->attributes[25]))   // asm 0x54EE38-0x54EE4C: fld the FIELD (the rounded store)/fcomp [interval]/test ah,1/jz -> the next tick; {<, NaN} FALLS THROUGH to the return - the fallthrough-fires-on-unordered class (the .c's literal < would continue on NaN)
                    return;                                          // asm 0x54EE52-0x54EE55
            }
        }
    }
}

// Replacement for RGE_Static_Object::gbg_doShieldDropoff. Original:
// @ 0x0054EE70 (idb_functions.h:3823). Range [0x54EE70, 0x54EEF8)
// (lst :558057/:558107, retn = no stack args), void. ABI: __thiscall.
// Virtual: vftable DATA XREF. Caller: RGE_Static_Object::update
// dispatches it via vfptr when NOT (is_shielded && hp >= sp) and
// sp > 0.0. Semantics: the same accumulator with a hardcoded 1.0 s
// interval - each tick sp -= attributes[26] (the owner-supplied
// drop-off rate), floored at 0.
// Decode (asm 0x54EE70-0x54EEF8): the entry gates fld attributes[26]/
// fcomp/test ah,41h/jnz -> return (MATCH row, the literal >) and
// fld sp/fcomp/test ah,1/jnz -> return (taken {<, NaN} - the literal
// sp >= 0.0f is the MATCH row: NaN returns in both). The accumulator
// add fld/fadd/fst m32 (the store-round) + fcomp const_f_one on the
// UNROUNDED sum/test ah,41h/jnz -> return - MATCH row (the literal
// > 1.0; the double form per rule (a)). The tick fld/fsub const_f_one/
// fstp - rule (d). The sp chain fld/fsub/fst m32 (the store-round)
// then fcomp const_f_zero consumes the UNROUNDED difference.
// THE MACRO SITES: (1) the cap gate @0x54EED3-0x54EEDE - test ah,1/
// jnz -> the cap on {<, NaN} - the .c's literal `v3 < 0.0` break would
// CONTINUE on NaN (diverges) -> X87_LT_NAN; the cap itself is the
// INTEGER zero store (mov dword [sp], 0 - bit-identical to 0.0f).
// (2) the accumulator gate @0x54EEE0-0x54EEEE - test ah,41h/jz -> the
// next tick; {<=, NaN} falls through to the RETURN - the .c's literal
// `<= 1.0` return would CONTINUE on NaN (unordered-false; diverges)
// -> X87_LE_NAN.
void __fastcall RGE_Static_Object__gbg_doShieldDropoff_new(
    RGE_Static_Object* this_, DWORD dummy)
{
    TRIBE_Player* owner = this_->owner;                              // asm 0x54EE70 (@+0x18)
    if (owner->attributes[26] > 0.0f && this_->sp >= 0.0f)           // asm 0x54EE73-0x54EE97 (fld [attributes+68h]/test ah,41h/jnz; fld sp/test ah,1/jnz - both MATCH rows)
    {
        double accumulated = (double)owner->world->world_time_delta_seconds   // asm 0x54EE99-0x54EEA5
            + (double)this_->shield_recharge_accumulator;
        this_->shield_recharge_accumulator = (float)accumulated;     // asm 0x54EEA8 (fst m32 - the store-round)
        if (accumulated > 1.0)                                       // asm 0x54EEAB-0x54EEB6 (fcomp const_f_one on the UNROUNDED sum; test ah,41h/jnz -> return; MATCH row)
        {
            for (;;)
            {
                this_->shield_recharge_accumulator -= 1.0f;          // asm 0x54EEB8-0x54EEC1 (fld/fsub const_f_one/fstp - rule (d))
                double new_sp = (double)this_->sp
                    - (double)owner->attributes[26];                 // asm 0x54EECA-0x54EECD (fld sp/fsub [attributes+68h])
                this_->sp = (float)new_sp;                           // asm 0x54EED0 (fst m32 - the store-round)
                if (X87_LT_NAN(new_sp, 0.0))                         // asm 0x54EED3-0x54EEDE: fcomp const_f_zero on the UNROUNDED difference; test ah,1/jnz -> the cap - taken {<, NaN} (the .c's literal < would continue on NaN)
                {
                    this_->sp = 0.0f;                                // asm 0x54EEF1 (mov dword [sp], 0 - bit-identical)
                    return;                                          // asm -> locret_54EEF8
                }
                if (X87_LE_NAN(this_->shield_recharge_accumulator, 1.0f))   // asm 0x54EEE0-0x54EEEE: fld the FIELD/fcomp const_f_one/test ah,41h/jz -> the next tick; {<=, NaN} falls through to the return (the .c's literal <= would continue on NaN)
                    return;                                          // asm 0x54EEF0
            }
        }
    }
}

// Replacement for RGE_Static_Object::update. Original: @ 0x0054EF00
// (idb_functions.h:3824). Range [0x54EF00, 0x54F1BC) (lst :558116/:558429,
// retn = no stack args), unsigned __int8 return. ABI: __thiscall. Virtual:
// vftable DATA XREF (the Static vftable slot 38; the entry hook covers the
// derived dispatch). Caller: RGE_Animated_Object__update+5 (the ladder base
// call, hook-routed). Semantics: decay the shield-hit flash timer, run the
// shield recharge/dropoff pair when active (state 2), apply the sleep
// transition, and drive the per-state object machine (the death sequence
// with the facet randomization + the death sound, the attribute-held decay
// with the NEGDECAY regeneration mod baked, the undead/spawn transitions,
// the recycle-ready report).
// Decode (asm 0x54EF00-0x54F1BB): the timer decay entry gate fld/fcomp/
// test ah,41h/jnz - MATCH row (the literal >); the subtraction fst m32
// stores the round BEFORE the compare, which consumes the UNROUNDED x87
// value - the double form (rule (a)); the zeroing gate test ah,1/jz -
// {<, NaN} falls to the zero-store - THE MACRO SITE (the .c's literal <
// would leave NaN unzeroed) -> X87_LT_NAN. The state-2 dispatch: is_shielded
// (slot 15) al-test; the hp>=sp gate (movsx word master @+0x32/fild/fcomp
// sp; test ah,1/jnz -> the dropoff - MATCH row: NaN drops off in both);
// the recharge slot 16 / the sp>0 dropoff gate (MATCH row) / the slot 17.
// The sleep transition: removeObject (slot 73) / addObject (slot 72) with
// the dopple_flag RE-READ after removeObject (the double-read kept). The
// inside_obj state propagation (> 2 -> set_object_state slot 53). The
// object_state switch (the off_54F1C0 jump table, restored): case 0/2 ->
// check_damage_sprites (slot 38) -> LABEL_15 (the .c's goto dissolved into
// the shared case body - see the RESTRUCTURE note); the hp >= 1.0 gate
// (MATCH row: NaN takes the death path); the death sequence:
// set_object_state(3), get_death_sprite (master slot 3) -> new_sprite
// (slot 44), the facet randomization (flag & 4 @RGE_Sprite+0x80; facet_num
// @+0x70; facet_num * rand_internal() / RAND_MAX_INTERNAL - the
// 80010003h/sar-14 magic division = /0x7FFF, verified numerically; the
// >= facet_num clamp = facet_num - 1), the death sound (rand_internal
// pre-draw + srand_internal(seed) - the pre-play reseed kept; the
// civ-override player_id via the slot 2 dispatch), case 3/5 -> the
// attribute-held death gate (MATCH row) -> undead (master slot 4) or
// spawn_death_obj (slot 77), the attribute_rot decay with the NEGDECAY
// test-mask rewrite baked (X87_NE_ORD - ordered-nonzero rot only; negative
// rot regenerates) and the X87_LT_NAN zeroing, then the NEGDECAY clamp to
// attribute_amount_held[0] baked from the negDecay stub; case 7 ->
// set_object_state(8); case 8 -> (recyclable != 0) + 1; the default tail
// returns 0.
unsigned __int8 __fastcall RGE_Static_Object__update_new(
    RGE_Static_Object* this_, DWORD dummy)
{
    processUnitExtra(this_);                                         // UNIT_EXTRA (the casts.cpp:892 processUnitHook bake): the cheat-layer per-frame processor - the effect-timer decay, the hp drain, the construction solidify counter, the keepUnitExtra cleanup. The patched entry flow ran it after the 7-byte prologue replay, BEFORE the update body (the stub entered the original at 0x54EF07)
    if (this_->shield_hit_timer > 0.0f)                              // asm 0x54EF07-0x54EF15 (fld @+0xC/fcomp const_f_zero/test ah,41h/jnz skip; MATCH row)
    {
        double timer = (double)this_->shield_hit_timer               // asm 0x54EF1A-0x54EF23 (fld/fsub - the 53-bit difference)
            - (double)this_->owner->world->world_time_delta_seconds;   // (owner @+0x18 -> world @+0x8C; the field @+0xA8)
        this_->shield_hit_timer = (float)timer;                      // asm 0x54EF29 (fst m32 - the store-round)
        if (X87_LT_NAN(timer, 0.0))                                  // asm 0x54EF2C-0x54EF37: fcomp const_f_zero consumes the UNROUNDED value; test ah,1/jz skips on {>=}; {<, NaN} falls to the zero-store (the .c's literal < would leave NaN unzeroed)
            this_->shield_hit_timer = 0.0f;                          // asm 0x54EF39
    }
    if (this_->object_state == 2)                                    // asm 0x54EF3C-0x54EF40 (@+0x58)
    {
        if (this_->vfptr->is_shielded(this_)                         // asm 0x54EF46 (slot 15 = +0x3C)
            && (double)this_->master_obj->hp >= (double)this_->sp)   // asm 0x54EF4D-0x54EF64 (movsx word @+0x32/fild; fcomp sp; test ah,1/jnz -> the dropoff; MATCH row: NaN drops off in both)
            this_->vfptr->gbg_doShieldRecharge(this_);               // asm 0x54EF6A (slot 16 = +0x40)
        else if (this_->sp > 0.0f)                                   // asm 0x54EF6F-0x54EF7D (test ah,41h/jnz skip; MATCH row)
            this_->vfptr->gbg_doShieldDropoff(this_);                // asm 0x54EF83 (slot 17 = +0x44)
    }
    if (this_->goto_sleep_flag)                                      // asm 0x54EF86-0x54EF89 (@+0x5B)
    {
        TRIBE_Player* owner = this_->owner;                          // asm 0x54EF8E (@+0x18)
        unsigned __int8 dopple = this_->dopple_flag;                 // asm 0x54EF94 (@+0x5A)
        this_->goto_sleep_flag = 0;                                  // asm 0x54EF97
        this_->sleep_flag = 1;                                       // asm 0x54EF9A (@+0x59)
        owner->vfptr->removeObject(owner, this_, 0, dopple, this_->player_object_node);   // asm 0x54EFA3 (slot 73 = +0x124)
        this_->player_object_node = owner->vfptr->addObject(owner, this_, 1, this_->dopple_flag);   // asm 0x54EFAE-0x54EFB7 (slot 72 = +0x120; the dopple RE-READ after removeObject - the double-read kept faithful)
        return 0;                                                    // asm 0x54EFC1
    }
    if (this_->inside_obj)                                           // asm 0x54EFC9-0x54EFCC (@+0x2C)
    {
        unsigned __int8 inside_state = this_->inside_obj->object_state;   // asm 0x54F1A4 (mov al, [eax+58h])
        if (inside_state > 2)                                        // asm 0x54F1A7-0x54F1A9 (jbe -> the default tail)
            this_->vfptr->set_object_state(this_, inside_state);     // asm 0x54F1B0 (slot 53 = +0xD4)
        return 0;                                                    // asm -> loc_54F1B6
    }
    switch (this_->object_state)                                     // asm 0x54EFD2-0x54EFE0 (the off_54F1C0 jump table, 9 cases - restored)
    {
    case 0u:
    case 2u:
        // RESTRUCTURE: the .c's `case 0u: goto LABEL_15; case 2u:
        // check_damage_sprites(); LABEL_15:` - the goto dissolved into the
        // shared case body guarded by the switch discriminant (case 0 ->
        // state 0 -> the check skipped; case 2 -> state 2 -> the check runs;
        // the same path set - a pure function of the switch value, asm
        // 0x54EFE7 the case-2 entry -> 0x54EFF1 LABEL_15)
        if (this_->object_state == 2u)                               // asm 0x54EFE7-0x54EFEB (slot 38 = +0x98)
            this_->vfptr->check_damage_sprites(this_);
        if (this_->hp >= 1.0f)                                       // asm 0x54EFF1-0x54EFFF (LABEL_15: fld @+0x3C/fcomp const_f_one/test ah,1/jz -> the default tail; MATCH row: NaN takes the death path in both)
            break;
        this_->vfptr->set_object_state(this_, 3);                    // asm 0x54F00B (slot 53)
        this_->vfptr->new_sprite(this_, this_->master_obj->vfptr->get_death_sprite(this_->master_obj, this_));   // asm 0x54F019 (master slot 3 = +0xC) / 0x54F01F (slot 44 = +0xB0)
        if (!this_->sprite_list)                                     // asm 0x54F025 (@+0x24)
        {
            if (this_->sprite)                                       // asm 0x54F02D (@+0x1C)
            {
                if ((this_->sprite->flag & 4) != 0                   // asm 0x54F031 (test byte [eax+80h], 4 - RGE_Sprite::flag @0x80)
                    && this_->sprite->facet_num > 0)                 // asm 0x54F03A-0x54F03E (@+0x70 i16, jle)
                {
                    int facet_value = this_->sprite->facet_num * rand_internal() / RAND_MAX_INTERNAL;   // asm 0x54F042-0x54F063 (call _rand; the 80010003h/sar-14 magic division = /0x7FFF - the CRT rule: rand_internal + the consts.h bound)
                    this_->facet = (unsigned __int8)facet_value;     // asm 0x54F063 (mov [esi+45h], dl)
                    if ((unsigned __int8)facet_value >= this_->sprite->facet_num)   // asm 0x54F066-0x54F06E (movzx dx, dl/cmp facet_num/jl)
                        this_->facet = (unsigned __int8)(this_->sprite->facet_num - 1);   // asm 0x54F070-0x54F072 (mov al/dec al)
                }
            }
        }
        if (this_->master_obj->death_sound)                          // asm 0x54F07B-0x54F07E (@+0x50)
        {
            int seed = rand_internal();                              // asm 0x54F084 - the pre-play draw
            if (this_->owner->id == this_->owner->world->curr_player)   // asm 0x54F08E-0x54F0A1 (@+0xA0 vs movsx word @+0x94)
                RGE_Sound__play(this_->master_obj->death_sound, 1, 0, 0,   // asm 0x54F0A3-0x54F0B7 (:3662; playType 1/pan 0/volume 0)
                    this_->vfptr->gbg_get_civ_override(this_),       // asm 0x54F0A9 (slot 2 = +0x8) - the civ-override player_id
                    -1);                                             // asm 0x54F0A5 (culture -1)
            srand_internal((unsigned int)seed);                      // asm 0x54F0BD-0x54F0C2 (call srand @0x632BD3; add esp,4) - THE RESEED: the pre-play value restores the game's RNG stream after RGE_Sound::play's internal draws
            return 0;                                                // asm 0x54F0C5-0x54F0CB
        }
        break;                                                       // asm -> loc_54F1B6
    case 3u:
    case 5u:
        if (this_->attribute_type_held == -1                         // asm 0x54F0CC-0x54F0D1 (cmp word @+0x5C, 0FFFFh)
            || X87_LE_NAN(this_->attribute_amount_held, 0.0f))       // asm 0x54F0D3-0x54F0E1 (fld @+0x54/fcomp const_f_zero/test ah,41h/jnz -> the death seq; the taken set {<=, NaN} targets the POSITIVE branch - the literal <= is unordered-false on NaN and would SKIP the death seq, diverging (the first /Fa run proved it: comiss+jae let NaN fall into the rot block) -> the macro)
        {
            if (this_->master_obj->undead_flag)                      // asm 0x54F138-0x54F13B (@+0x30)
            {
                this_->vfptr->set_object_state(this_, 6);            // asm 0x54F141 (slot 53)
                this_->vfptr->new_sprite(this_, this_->master_obj->vfptr->get_undead_sprite(this_->master_obj, this_));   // asm 0x54F14F (master slot 4 = +0x10) / 0x54F155 (slot 44)
            }
            else
            {
                this_->vfptr->set_object_state(this_, 7);            // asm 0x54F166
                this_->vfptr->spawn_death_obj(this_);                // asm 0x54F170 (slot 77 = +0x134)
            }
            return 0;                                                // both arms
        }
        // NEGDECAY (writeByte 0x54F0F6 = the 41h -> 40h test-mask rewrite at
        // this gate, BAKED per the step-3b default): the patched gate skips
        // the decay only on rot == 0 or NaN - NEGATIVE rot proceeds (the
        // regeneration mod); the proceed partition is ordered != - the
        // X87_NE_ORD macro (x87_nan.h:22; a manual disjunction is the
        // macro's own body - the mandate requires the macro form; a literal
        // != would proceed on NaN - diverging)
        if (X87_NE_ORD(this_->master_obj->attribute_rot, 0.0f))      // asm 0x54F0E3-0x54F0F7 (fld @+0x90/fcomp const_f_zero/test - PATCHED to ah,40h; jnz -> the default tail; the macro expansion: comiss+jb || comiss+ja, unfolded)
        {
            double v15 = (double)this_->attribute_amount_held        // asm 0x54F106-0x54F112 (fld delta/fmul rot/fsubr amount - the 53-bit chain)
                - (double)this_->owner->world->world_time_delta_seconds * (double)this_->master_obj->attribute_rot;
            this_->attribute_amount_held = (float)v15;               // asm 0x54F115 (fst m32 - the store-round)
            if (X87_LT_NAN(v15, 0.0))                                // asm 0x54F118-0x54F123: fcomp const_f_zero consumes the UNROUNDED value; test ah,1 - the vanilla gate (the negDecay hook replaced the jz); the {<, NaN} fallthrough zeroes (the .c's literal < would continue on NaN)
            {
                this_->attribute_amount_held = 0.0f;                 // asm 0x54F129 (mov ebx)
                return 0;                                            // asm 0x54F12C-0x54F132
            }
            // NEGDECAY (the negDecay naked stub @0x54F123, BAKED): on the
            // >= branch the ROUNDED amount is clamped to the master's
            // configured held amount (attribute_amount_held[0] @+0x80 - the
            // float-bits copy); the literal > is the MATCH row (NaN -> no
            // clamp, matching negDecay's test ah,41h/jnz)
            if (this_->attribute_amount_held > this_->master_obj->attribute_amount_held[0])   // negDecay: fld [esi+54h]/fcomp dword [ecx+80h]/test ah,41h/jnz
                this_->attribute_amount_held = this_->master_obj->attribute_amount_held[0];   // negDecay: mov eax,[ecx+80h]/mov [esi+54h],eax (the float-bits copy)
        }
        break;                                                       // asm -> loc_54F1B6
    case 7u:
        this_->vfptr->set_object_state(this_, 8);                    // asm 0x54F183 (slot 53)
        return 0;                                                    // asm 0x54F189-0x54F18F
    case 8u:
        return (this_->master_obj->recyclable != 0) + 1;             // asm 0x54F190-0x54F1A3 (@+0xBC; setnz/inc = 1 or 2)
    default:
        break;                                                       // asm loc_54F1B6
    }
    return 0;                                                        // asm loc_54F1B6 (mov al, bl)
}




// ===========================================================================
// BATCH S1 (owner-approved 2026-11-26): the Static micro-method tier - the
// 45 hooks of the <=39-byte class (the asm walk 0x422A40-0x552EA0 + the
// 0x408A50/0x40C840 trio). Every body decoded instruction-level; the
// shared-block tails (the 505/600 notifies, the looped calls) merged by
// the compiler where the vanilla merged them.
// ---------------------------------------------------------------------------

// asm 0x422A40 (retn 10h, empty): the Static-tier command stub - the virtual
// slot exists so the vftable dispatch is uniform; the body does nothing.
void __fastcall RGE_Static_Object__do_command_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* formal1,
    float formal2, float formal3, float formal4)
{
}

// asm 0x422A60 (xor al,al; retn 10h): the move command stub - always fails.
unsigned __int8 __fastcall RGE_Static_Object__move_to_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* formal1,
    float formal2, float formal3, float formal4)
{
    return 0;                                                          // asm xor al, al
}

// asm 0x40C840 (mov eax, [ecx+7Ch]): the AI-module getter (unitAIValue @+0x7C).
UnitAIModule* __fastcall RGE_Static_Object__unitAI_new(RGE_Static_Object* this_)
{
    return this_->unitAIValue;                                         // asm mov eax, [ecx+7Ch]
}

// asm 0x422A80 (mov eax, [ecx+14h]; retn 10h): the master_obj getter.
RGE_Master_Static_Object* __fastcall RGE_Static_Object__get_command_master_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* formal1,
    float formal2, float formal3, float formal4)
{
    return this_->master_obj;                                          // asm mov eax, [ecx+14h]
}

// asm 0x54BE40 (mov ecx, [ecx+14h]; mov eax, [ecx]; jmp [eax]): the tail
// dispatch into the MASTER's do_i_care_about_power (the master vtbl slot 0,
// idb_types.h:3602) - the named virtual dispatch through master_obj
// (the TRIBE_Building runtime do_i_care tail-jumps into this static one,
// so the static hook covers that dispatch too).
unsigned __int8 __fastcall RGE_Static_Object__do_i_care_about_power_new(RGE_Static_Object* this_)
{
    return this_->master_obj->vfptr->do_i_care_about_power(this_->master_obj);   // asm 0x54BE40-0x54BE45
}

// asm 0x408A90 (fld const_f_one; retn 4): the flat 1.0 modifier.
double __fastcall RGE_Static_Object__calc_attack_modifier_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* formal)
{
    return 1.0;                                                        // asm fld ds:const_f_one
}

// asm 0x422AD0 (fld const_f_zero; retn 10h): the static tier never yaws.
double __fastcall RGE_Static_Object__calculateYawAngle_new(
    RGE_Static_Object* this_, DWORD dummy, BVector* curOf, BVector* newOF,
    float interval, int* limitedAngle)
{
    return 0.0;                                                        // asm fld ds:const_f_zero
}

// asm 0x5524F0 (mov al, [ecx+86h]): the underAttackValue getter (@+0x86).
unsigned __int8 __fastcall RGE_Static_Object__underAttack_new(RGE_Static_Object* this_)
{
    return this_->underAttackValue;                                    // asm mov al, [ecx+86h]
}

// asm 0x552E10 (mov eax, [ecx+14h]; fld [eax+34h]): the master's LOS float
// (the idb function name searchRadius is a misnomer - the field is los @+0x34).
double __fastcall RGE_Static_Object__searchRadius_new(RGE_Static_Object* this_)
{
    return this_->master_obj->los;                                     // asm mov eax, [ecx+14h]; fld dword ptr [eax+34h]
}

// asm 0x54B8A0 (cmp byte [ecx+58h], 2; setnbe): the AI-update skip flag -
// object_state (@+0x58) above 2 (dying/corpse/undead per the state registry).
unsigned __int8 __fastcall RGE_Static_Object__gbg_skip_unit_ai_update_new(RGE_Static_Object* this_)
{
    return this_->object_state > 2u;                                   // asm cmp byte ptr [ecx+58h], 2; setnbe al
}

// asm 0x552930 (mov dword [ecx+70h], 0): clears mPathingGroupSize (@+0x70).
void __fastcall RGE_Static_Object__removeAllFromPathingGroup_new(RGE_Static_Object* this_)
{
    this_->mPathingGroupSize = 0;                                      // asm mov dword ptr [ecx+70h], 0
}

// asm 0x552500 (mov al, [esp+v]; mov [ecx+86h], al): the underAttack setter.
void __fastcall RGE_Static_Object__setUnderAttack_new(
    RGE_Static_Object* this_, DWORD dummy, unsigned __int8 v)
{
    this_->underAttackValue = v;                                       // asm mov al, [esp+v]; mov [ecx+86h], al
}

// asm 0x552EA0 (mov eax, [ecx+30h]; test; jnz; mov eax, [eax+4]): the
// contained-object count - objects (@+0x30, RGE_New_Object_List*) and its
// Number_of_objects (@+4); null list -> 0.
int __fastcall RGE_Static_Object__garrisoned_count_new(RGE_Static_Object* this_)
{
    if (this_->objects)                                                // asm test eax, eax
        return this_->objects->Number_of_objects;                      // asm mov eax, [eax+4]
    return 0;                                                          // asm retn (eax = 0 from the test)
}

// asm 0x54B450 (cmp word [master+1Eh], 3Eh; setz): the group == 62 predicate
// (62 = Air Cruiser, the owner-supplied registry).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isAirCruiser_new(RGE_Static_Object* this_)
{
    return this_->master_obj->object_group == 62;                      // asm cmp word ptr [eax+1Eh], 3Eh; setz al
}

// asm 0x422A50 (mov eax, [esp+range]; mov dword [eax], 0; or eax, -1):
// zeroes the *range out-param and returns -1 - the static tier has no
// action type for any order.
int __fastcall RGE_Static_Object__actionTypeIfOrdered_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z, float* range)
{
    *range = 0.0f;                                                     // asm mov dword ptr [eax], 0
    return -1;                                                         // asm or eax, 0FFFFFFFFh
}

// asm 0x422B30 (mov eax, [esp+result]; xor ecx, ecx; mov [eax], ecx x3):
// zeroes the out BVector (x/z/y dwords) and returns it.
BVector* __fastcall RGE_Static_Object__orientationForward_new(
    RGE_Static_Object* this_, DWORD dummy, BVector* result)
{
    result->x = 0.0f;                                                  // asm mov [eax], ecx
    result->z = 0.0f;                                                  // asm mov [eax+4], ecx
    result->y = 0.0f;                                                  // asm mov [eax+8], ecx
    return result;                                                     // asm (eax = the result)
}

// asm 0x422B10 (the same zero-store triple; retn 0Ch): the static tier has
// no velocity - the projection is the zero vector.
BVector* __fastcall RGE_Static_Object__straightProjectedPosition_new(
    RGE_Static_Object* this_, DWORD dummy, BVector* result, BVector* newOF,
    float secondsInTheFuture)
{
    result->x = 0.0f;                                                  // asm mov [eax], ecx
    result->z = 0.0f;                                                  // asm mov [eax+4], ecx
    result->y = 0.0f;                                                  // asm mov [eax+8], ecx
    return result;                                                     // asm (eax = the result)
}

// asm 0x54B5B0 (cmp word [master+1Eh], 5; setz): the group == 5 predicate
// (5 = Monster/Trouble, the owner-supplied registry).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isPredatorUnit_new(RGE_Static_Object* this_)
{
    return this_->master_obj->object_group == 5;                       // asm cmp word ptr [eax+1Eh], 5; setz cl
}

// asm 0x550AD0 (mov al, [ecx+58h]; cmp 3 jb; cmp 6 jz; else 1): the dying
// predicate - object_state >= 3 except the undead 6 (the state registry).
unsigned __int8 __fastcall RGE_Static_Object__is_dying_new(RGE_Static_Object* this_)
{
    unsigned __int8 os = this_->object_state;                          // asm mov al, [ecx+58h]
    if (os < 3u)                                                       // asm cmp al, 3; jb loc_550ADE
        return 0;                                                      // asm xor al, al
    if (os == 6u)                                                      // asm cmp al, 6; jz loc_550ADE
        return 0;                                                      // asm xor al, al
    return 1;                                                          // asm mov al, 1
}

// asm 0x551990 (mov [ecx+5Ch], ax; mov [ecx+54h], edx): the carried
// attribute type (@+0x5C) and amount (@+0x54) setter.
void __fastcall RGE_Static_Object__set_attribute_new(
    RGE_Static_Object* this_, DWORD dummy, __int16 attr_type, float attr_amount)
{
    this_->attribute_type_held = attr_type;                            // asm mov [ecx+5Ch], ax
    this_->attribute_amount_held = attr_amount;                        // asm mov [ecx+54h], edx
}

// asm 0x54FF00 (mov ecx, [ecx+24h]; test; jz; call remove_sprite): the
// overlay removal on the sprite_list (@+0x24), null-guarded.
void __fastcall RGE_Static_Object__remove_overlay_sprite_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Sprite* nsprite)
{
    if (this_->sprite_list)                                            // asm test ecx, ecx
        RGE_Active_Sprite_List__remove_sprite(this_->sprite_list, nsprite);   // asm 0x54FF0C (the direct idb constant)
}

// asm 0x551500 (mov eax, [ecx+2Ch]; test; jz; call [edx+15Ch]): the
// container's remove_obj virtual (the RGE_Static_ObjectVtbl slot 87 =
// remove_obj(this, obj) - the named virtual dispatch; the leaving object
// passed as the argument).
void __fastcall RGE_Static_Object__exit_obj_new(RGE_Static_Object* this_)
{
    if (this_->inside_obj)                                             // asm test eax, eax
        this_->inside_obj->vfptr->remove_obj(this_->inside_obj, this_);   // asm 0x403?? 0x55150C (call dword ptr [edx+15Ch])
}

// asm 0x551B80 (push y/x/obj; call distance_to_object2): the 2-D distance
// forwarder - the callee (this, obj, x, y) receives this_ via ecx
// (unclobbered) and the flat world coords.
double __fastcall RGE_Static_Object__distance_to_object_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    return RGE_Static_Object__distance_to_object2(this_, obj, this_->world_x, this_->world_y);   // asm 0x4015?? 0x551B86-0x551B8D
}

// asm 0x551A60 (mov al, [ecx+5Fh]; cmp 0FAh jnb; inc; push 0; call
// set_sleep_flag): the worker-num increment (capped below 250) + the wake.
// The declared args (work_obj/action_type/attack_flag) are unconsumed.
void __fastcall RGE_Static_Object__set_being_worked_on_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Action_Object* work_obj,
    __int16 action_type, unsigned __int8 attack_flag)
{
    if (this_->worker_num < 0xFAu)                                     // asm cmp al, 0FAh; jnb loc_551A6C
        this_->worker_num = this_->worker_num + 1;                     // asm inc al; mov [ecx+5Fh], al
    RGE_Static_Object__set_sleep_flag(this_, 0);                       // asm push 0; call RGE_Static_Object__set_sleep_flag (the direct idb constant)
}

// asm 0x552EB0 (test ecx; jl; cmp 9 jg; mov eax,1; shl eax,cl): the group
// 0-9 bit mask - __fastcall int(int) per the idb prototype (the number IS
// the ecx input; no this).
int __fastcall RGE_Static_Object__get_group_number_mask_new(int group_num)
{
    if (group_num < 0 || group_num > 9)                                // asm test ecx, ecx; jl; cmp ecx, 9; jg
        return 0;                                                      // asm xor eax, eax
    return 1 << group_num;                                             // asm mov eax, 1; shl eax, cl
}

// asm 0x54F500 (mov ecx, [ecx+18h]; push oID; mov ecx, [ecx+8Ch]; call
// RGE_Game_World__object): the id -> live-object resolver through the
// owner's world (@+0x8C).
RGE_Static_Object* __fastcall RGE_Static_Object__get_object_pointer_new(
    RGE_Static_Object* this_, DWORD dummy, int obj_id)
{
    return RGE_Game_World__object((RGE_Game_World*)this_->owner->world, obj_id);   // asm 0x54F50E-0x54F513
}

// asm 0x551FF0 (mov [ecx+48h/4Ch/50h]): the world position setter.
void __fastcall RGE_Static_Object__set_location_new(
    RGE_Static_Object* this_, DWORD dummy, float x, float y, float z)
{
    this_->world_x = x;                                                // asm mov [ecx+48h], eax
    this_->world_y = y;                                                // asm mov [ecx+4Ch], edx
    this_->world_z = z;                                                // asm mov [ecx+50h], eax
}

// asm 0x552C40 (fld y/z/x; fstp result/x/z/y): the position getter - the
// x87 stack order lands world_x in result[0], world_z in [4], world_y in
// [8] (the BVector {x, z, y} layout); returns the result pointer (eax).
BVector* __fastcall RGE_Static_Object__position_new(
    RGE_Static_Object* this_, DWORD dummy, BVector* result)
{
    result->x = this_->world_x;                                        // asm fstp dword ptr [eax] (the x87 y/z/x push order)
    result->z = this_->world_z;                                        // asm fstp dword ptr [eax+4]
    result->y = this_->world_y;                                        // asm fstp dword ptr [eax+8]
    return result;                                                     // asm (eax = the result)
}

// asm 0x551560 (mov ecx, [ecx+30h]; push obj; call Remove_node; mov
// [esi+2Ch], 0): the removal from the owner's contained-objects list
// (objects @+0x30, RGE_New_Object_List::Remove_node) + the inside_obj
// backref (@+0x2C) cleared.
void __fastcall RGE_Static_Object__remove_obj_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    RGE_New_Object_List__Remove_node(this_->objects, obj);             // asm 0x551569 (ecx = the list; the stack = obj)
    obj->inside_obj = 0;                                               // asm mov dword ptr [esi+2Ch], 0
}

// asm 0x54B5C0 (cmp word [master+18h], 5B9h/2B8h): the master-id predicate
// (1465 / 696 - the registry candidates, owner designations owed).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isStaticPredatorUnit_new(RGE_Static_Object* this_)
{
    return this_->master_obj->id == 1465 || this_->master_obj->id == 696;   // asm cmp ax, 5B9h; jz; cmp ax, 2B8h; jz
}

// asm 0x54B480 (cmp 54h/212h): the master-id predicate (84 / 530 - the
// registry candidates).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isSpaceport_new(RGE_Static_Object* this_)
{
    return this_->master_obj->id == 84 || this_->master_obj->id == 530;     // asm cmp ax, 54h; jz; cmp ax, 212h; jz
}

// asm 0x54B460 (cmp 1F4h/25Bh): the master-id predicate (500 / 603 - the
// registry candidates).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isGEAssaultMech_new(RGE_Static_Object* this_)
{
    return this_->master_obj->id == 500 || this_->master_obj->id == 603;    // asm cmp ax, 1F4h; jz; cmp ax, 25Bh; jz
}

// asm 0x5513A0 (sprite/sprite_list null guards; call [edx+4]): the sprite
// list's frame virtual (the RGE_Active_Sprite_ListVtbl slot 1 = frame);
// the guards return AX = 0.
__int16 __fastcall RGE_Static_Object__get_animation_frame_new(RGE_Static_Object* this_)
{
    if (!this_->sprite)                                                // asm test eax, eax
        return 0;                                                      // asm xor ax, ax
    if (!this_->sprite_list)                                           // asm test ecx, ecx
        return 0;                                                      // asm xor ax, ax
    return this_->sprite_list->vfptr->frame(this_->sprite_list, this_->sprite);        // asm 0x5513B1 (call dword ptr [edx+4] - slot 1)
}

// asm 0x54B5E0 (call destructor; test flags&1; operator_delete; return this):
// the vector-deleting destructor - the tier destructor (hooked, routes)
// then the flags-gated heap release (the M1 Animated vdel precedent).
void* __fastcall RGE_Static_Object__vector_deleting_destructor_new(
    RGE_Static_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Static_Object__destructor(this_);                              // asm 0x54B5E3 (call RGE_Static_Object__destructor - HOOKED, routes)
    if (flags & 1)                                                     // asm test [esp+arg_0], 1
        operator_delete_internal(this_);                               // asm 0x54B5F0 (push esi; call operator_delete - the CRT wrapper)
    return this_;                                                      // asm mov eax, esi
}

// asm 0x551380 (sprite/sprite_list null guards; call [edx+1Ch]): the sprite
// list's set_frame virtual (the RGE_Active_Sprite_ListVtbl slot 7).
void __fastcall RGE_Static_Object__set_animation_frame_new(
    RGE_Static_Object* this_, DWORD dummy, __int16 frame)
{
    if (!this_->sprite)                                                // asm test eax, eax
        return;                                                        // asm jz locret_55139B
    if (!this_->sprite_list)                                           // asm test ecx, ecx
        return;                                                        // asm jz locret_55139B
    this_->sprite_list->vfptr->set_frame(this_->sprite_list, this_->sprite, frame);        // asm 0x551397 (call dword ptr [edx+1Ch] - slot 7)
}

// asm 0x54B870 (movsx master hp i16 @+0x32; fild; fcomp this hp @+0x3C;
// test ah,41h; jnz -> 0): the damaged predicate - NOT damaged when the
// master's max hp (as int) is <= the current hp (the {<=, NaN} row =
// X87_LE_NAN; a higher-than-max hp reads undamaged in the vanilla).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isDamaged_new(RGE_Static_Object* this_)
{
    if (X87_LE_NAN((double)this_->master_obj->hp, (double)this_->hp))  // asm 0x54B87C-0x54B888 (fild; fcomp; test ah,41h; jnz loc_54B88E)
        return 0;                                                      // asm xor al, al
    return 1;                                                          // asm mov al, 1
}

// asm 0x54FED0 (sprite_list null guard; facet = byte @+0x45; push facet/0/0/
// order/nsprite; call add_sprite): the overlay sprite add on the sprite list.
void __fastcall RGE_Static_Object__add_overlay_sprite_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Sprite* nsprite, unsigned __int8 order)
{
    if (this_->sprite_list)                                            // asm test ecx, ecx
        RGE_Active_Sprite_List__add_sprite(this_->sprite_list, nsprite, order, 0, 0, this_->facet);   // asm 0x54FEED (facet = movzx byte [eax+45h])
}

// asm 0x54B4E0 (movsx group; add -11; the byte_54B50C/jpt_54B4F7 switch):
// the military-unit group predicate - groups 11, 13, 15-17, 32-40, 43, 44,
// 46-57, 59, 61-64 -> 1 (the asm case enumeration; 41/42 -> 1 is the
// vanilla's own answer). THE SWITCH-TABLE BYTE COMPARISON at the gate
// (the emitted DB vs byte_54B50C).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isMilitaryUnit_new(RGE_Static_Object* this_)
{
    switch (this_->master_obj->object_group) {                         // asm movsx eax, word ptr [eax+1Eh]; add eax, 0FFFFFFF5h
    case 11: case 13: case 15: case 16: case 17:                       // jpt_54B4F7 -> loc_54B4FE
    case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40:
    case 43: case 44: case 46: case 47: case 48: case 49: case 50: case 51: case 52:
    case 53: case 54: case 55: case 56: case 57: case 59: case 61: case 62: case 63: case 64:
        return 1;                                                      // asm mov al, 1
    default:
        return 0;                                                      // asm xor al, al
    }
}

// asm 0x54B400 (add -43; byte_54B42C/jpt_54B417): the aircraft group
// predicate - groups 43, 48, 59, 62-64 -> 1. THE SWITCH-TABLE BYTE
// COMPARISON at the gate (vs byte_54B42C).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isAircraft_new(RGE_Static_Object* this_)
{
    switch (this_->master_obj->object_group) {                         // asm movsx eax, word ptr [eax+1Eh]; add eax, 0FFFFFFD5h
    case 43: case 48: case 59: case 62: case 63: case 64:              // jpt_54B417 -> loc_54B41E
        return 1;                                                      // asm mov al, 1
    default:
        return 0;                                                      // asm xor al, al
    }
}

// asm 0x54B280 (add -11; byte_54B2AC/jpt_54B297): the attack-ground button
// predicate - groups 11, 32, 36, 43, 49, 62 -> 1. THE SWITCH-TABLE BYTE
// COMPARISON at the gate (vs byte_54B2AC).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isAttackGroundButtonAvailable_new(RGE_Static_Object* this_)
{
    switch (this_->master_obj->object_group) {                         // asm movsx eax, word ptr [eax+1Eh]; add eax, 0FFFFFFF5h
    case 11: case 32: case 36: case 43: case 49: case 62:              // jpt_54B297 -> loc_54B29E
        return 1;                                                      // asm mov al, 1
    default:
        return 0;                                                      // asm xor al, al
    }
}

// asm 0x550700 (test [esi+46h]&1 -> unselect; set_sleep_flag(0); [esi+3Ch]=0):
// the death preparation - the selected-bit unselect through the owner,
// the sleep flag cleared, the hp (@+0x3C) zeroed.
void __fastcall RGE_Static_Object__die_die_die_new(RGE_Static_Object* this_)
{
    if (this_->selected & 1)                                           // asm test byte ptr [esi+46h], 1
        RGE_Player__unselect_one_object((RGE_Player*)this_->owner, this_);          // asm 0x55070D (ecx = the owner; push esi - the TRIBE_Player* -> RGE_Player* dual-naming cast)
    RGE_Static_Object__set_sleep_flag(this_, 0);                       // asm 0x550716 (push 0; the direct idb constant)
    this_->hp = 0.0f;                                                  // asm mov dword ptr [esi+3Ch], 0
}

// asm 0x54B550 (add -12; byte_54B57C/jpt_54B567): the civilian group
// predicate - groups 12, 14, 45, 58 -> 1 (the Cargo Trader / Utility
// Trawler / Worker civilian-economy groups; 12 per the vanilla). THE
// SWITCH-TABLE BYTE COMPARISON at the gate (vs byte_54B57C).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isCivilianUnit_new(RGE_Static_Object* this_)
{
    switch (this_->master_obj->object_group) {                         // asm movsx eax, word ptr [eax+1Eh]; add eax, 0FFFFFFF4h
    case 12: case 14: case 45: case 58:                                // jpt_54B567 -> loc_54B56E
        return 1;                                                      // asm mov al, 1
    default:
        return 0;                                                      // asm xor al, al
    }
}

// asm 0x408A50 (ftol world_y -> push; ftol world_x -> push; ecx =
// player->visible @+0x88; call get_visible): the visibility probe - the
// (map_col, map_row) convention (the state-file MAP entry); the owned
// get_visible_new routes.
unsigned __int8 __fastcall RGE_Static_Object__get_visibility_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Player* player)
{
    return RGE_Visible_Map__get_visible(player->visible, (int)this_->world_x, (int)this_->world_y);   // asm 0x408A53-0x408A6F (map_col = ftol(world_x), map_row = ftol(world_y))
}

// asm 0x551580 (owner null -> 0; fld master los @+0x34; ftol; push 0
// (Square); push radius; ecx = player->visible; call get_los_table): the
// LOS table resolver at the master's los radius, the non-square variant.
LOSTBL* __fastcall RGE_Static_Object__get_los_table_new(RGE_Static_Object* this_)
{
    if (!this_->owner)                                                 // asm test esi, esi
        return 0;                                                      // asm xor eax, eax
    return RGE_Visible_Map__get_los_table(this_->owner->visible,       // asm 0x403?? 0x55159D-0x5515A0 (ecx = [esi+88h])
        (int)this_->master_obj->los, 0);                               // asm fld dword ptr [eax+34h]; __ftol; push 0 (Square)
}

// asm 0x54F520 (test sid; jl -> 0; owner->world @+0x8C; cmp word
// sprite_num @+0x40; jge -> 0; sprites[sid] @+0x44): the id -> sprite
// resolver with the signed/count bounds.
RGE_Sprite* __fastcall RGE_Static_Object__get_sprite_pointer_new(
    RGE_Static_Object* this_, DWORD dummy, __int16 sprite_id)
{
    if (sprite_id < 0)                                                 // asm test dx, dx; jl loc_54F545
        return 0;                                                      // asm xor eax, eax
    RGE_Game_World* world = (RGE_Game_World*)this_->owner->world;      // asm mov eax, [ecx+18h]; mov eax, [eax+8Ch] (the TRIBE_World family cast)
    if (sprite_id >= world->sprite_num)                                // asm cmp dx, [eax+40h]; jge loc_54F545
        return 0;                                                      // asm xor eax, eax
    return world->sprites[sprite_id];                                  // asm movsx ecx, dx; mov edx, [eax+44h]; mov eax, [edx+ecx*4]
}

// ===================== BATCH S2a (2026-11-26): the mid-tier methods (48-77B) =====================
// Grounding set: the runtime struct walk (idb_types.h:1447-1501 - master_obj
// @0x14, owner @0x18, sprite_list @0x24, inside_obj @0x2C, objects @0x30, hp
// @0x3C, facet @0x45, selected @0x46, world_x/y/z @0x48/4C/50,
// attribute_amount_held @0x54, object_state @0x58, worker_num @0x5F, mGroupID
// @0x88); the vtbl slots (idb_types.h:3958 line-arith - gbg_isBiological
// 24/+0x60, gbg_isDamaged 25/+0x64, teleport 42/+0xA8, set_object_state
// 53/+0xD4, exit_obj 85/+0x154, garrisoned_count 162/+0x288,
// create_object_list 169/+0x2A4; RGE_Active_Sprite_ListVtbl::set_facet
// 8/+0x20; TRIBE_PlayerVtbl::isAlly 9/+0x24); (*base_game)->outline_type
// @+0x70 / game_mode @+0x1E0 (the asm offsets, the header reconciled);
// SDI_Capture_Info = the existing common.h binding @0x0077FA70.

// ---------------------------------------------------------------------------
// gbg_isResourceBuilding @0x0054B4A0 [0x54B4A0,0x54B4D1): the resource-building
// master-id membership test (signed i16 id; the asm cmp/sub chain 0x54B4A7-
// 0x54B4CA decoded as the set). Registry candidates (game-facts UNKNOWN
// per-value; the set overlaps do_i_care_about_power's power set).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isResourceBuilding_new(
    RGE_Static_Object* this_)
{
    __int16 id = this_->master_obj->id;                                       // asm mov eax,[ecx+14h]; movsx eax,word ptr [eax+18h]
    return id == 68 || id == 319 || id == 323 || id == 562 || id == 584;      // asm cmp 44h/13Fh/143h; sub 232h; sub 16h (RESTRUCTURE: the membership switch -> disjunction, pure function of id)
}

// ---------------------------------------------------------------------------
// calculateDamage @0x005518F0 [0x5518F0,0x551921): the Static base damage sum -
// weapon[i].value (i16) * attack_modifier accumulated on st0 at 53-bit (fild /
// fmul m32 / faddp, NO store round; rules (a)/(j): the chain is double). The
// return is the unrounded st0 (retn 14h passthrough, corollary (h)).
// attacking_player/attackingObject are unreferenced by the original (ABI kept).
// PRECISION SCOPE: the per-term multiply order is load-bearing - the original
// computes fild/fmul/faddp per element (result += value * modifier); under
// /fp:fast the unrolled loop reassociated to (sum * modifier) once - the
// distributivity does NOT hold exactly in floating point. The precise bracket
// pins the per-term association (the DIMU v151 precedent).
#pragma float_control(precise, on, push)
double __fastcall RGE_Static_Object__calculateDamage_new(
    RGE_Static_Object* this_, DWORD dummy, int weapon_num,
    RGE_Armor_Weapon_Info* damage, float attack_modifier,
    RGE_Player* attacking_player, RGE_Static_Object* attackingObject)
{
    double result = 0.0;                                                      // asm fld ds:const_f_zero
    if (damage != 0 && weapon_num > 0)                                        // asm test eax,eax; jz; mov ecx,[esp+weapon_num]; test; jle
    {
        for (int i = 0; i < weapon_num; i++)                                  // asm loop loc_551909: dec ecx; jnz
            result += (double)damage[i].value * (double)attack_modifier;      // asm movsx edx,word ptr [eax]; add eax,4; fild; fmul [esp+attack_modifier]; faddp st(1),st (the +=4 walk -> indexed over the verified stride-4 element)
    }
    return result;
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// inLockedFormation @0x00552850 [0x552850,0x552884): the formation-lock query.
// mFormationLocked @+0xF4 = 244 (the calibrated BUnitGroup walk: mCurrentCommand
// @+828 = the A4 /Fa anchor; the asm [eax+0F4h] agrees).
int __fastcall RGE_Static_Object__inLockedFormation_new(RGE_Static_Object* this_)
{
    if (this_->mGroupID == -1)                                                // asm cmp eax,-1; jz loc_552880
        return 0;
    BUnitGroup* group = RGE_Game_World__getUnitGroup(                         // asm push groupID; mov ecx,[owner->world]; call :6099 (the TRIBE_World family cast)
        (RGE_Game_World*)this_->owner->world, this_->mGroupID);
    if (group == 0 || group->mFormationLocked == 0)                           // asm test eax,eax; jz; mov ecx,[eax+0F4h]; test ecx,ecx; jnz ret1
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// add_obj @0x00551520 [0x551520,0x551555): container placement - lazy object
// list create (slot 169), Add_node, the back-pointer, the x/y position sync.
void __fastcall RGE_Static_Object__add_obj_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    if (this_->objects == 0)                                                  // asm mov eax,[esi+30h]; test; jnz loc_551536
        this_->objects = this_->vfptr->create_object_list(this_);             // asm call [eax+2A4h] slot 169
    RGE_New_Object_List__Add_node(this_->objects, obj);                       // asm push obj; call (idb constant)
    obj->inside_obj = this_;                                                  // asm mov [edi+2Ch],esi
    obj->world_x = this_->world_x;                                            // asm mov ecx,[esi+48h]; mov [edi+48h],ecx
    obj->world_y = this_->world_y;                                            // asm mov edx,[esi+4Ch]; mov [edi+4Ch],edx
}

// ---------------------------------------------------------------------------
// release_being_worked_on @0x00551A80 [0x551A80,0x551AB8): the worker-release
// counter; when the last worker leaves an ACTIVE unit (state 2, hp >= 0 - the
// literal MATCH row: test ah,1; jz targets the sleep arm on {>= ordered}) or an
// UNDEAD one (state 6), it goes back to sleep.
void __fastcall RGE_Static_Object__release_being_worked_on_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* caller)
{
    if (this_->worker_num != 0)                                               // asm mov al,[ecx+5Fh]; test; jbe loc_551A8C; dec al; store
        this_->worker_num--;
    if (this_->worker_num != 0)                                               // asm mov al,[ecx+5Fh]; test; jnz locret_551AB7
        return;
    if (this_->object_state == 2 && this_->hp >= 0.0f)                        // asm cmp dl,2; jnz loc_551AAB; fld [ecx+3Ch]; fcomp ds:const_f_zero; fnstsw ax; test ah,1; jz loc_551AB0
    {
        RGE_Static_Object__set_sleep_flag(this_, 1);                          // asm push 1; call (idb constant, UNHOOKED -> routes when owned)
        return;
    }
    if (this_->object_state == 6)                                             // asm cmp dl,6; jnz locret
        RGE_Static_Object__set_sleep_flag(this_, 1);                          // the asm's shared loc_551AB0 (the compiler merges the identical tails)
}

// ---------------------------------------------------------------------------
// set_facet @0x00552A10 [0x552A10,0x552A4D): the facing store - the BYTE
// compare early-out (the char->u8 cast: an int-promoted compare diverges for
// facets >= 0x80), the facet stored BEFORE the sprite-list dispatch, then the
// list's set_facet (slot 8) with the ftol tile-space x/y.
void __fastcall RGE_Static_Object__set_facet_new(
    RGE_Static_Object* this_, DWORD dummy, char new_facet)
{
    unsigned __int8 facet_byte = (unsigned __int8)new_facet;                  // asm mov bl,[esp+4+new_facet]
    if (this_->facet == facet_byte)                                           // asm cmp [esi+45h],bl; jz loc_552A49 (byte-width)
        return;
    RGE_Active_Sprite_List* sprite_list = this_->sprite_list;                 // asm mov edi,[esi+24h]
    this_->facet = facet_byte;                                                // asm mov [esi+45h],bl (the store precedes the dispatch)
    if (sprite_list == 0)                                                     // asm test edi,edi; jz loc_552A49
        return;
    sprite_list->vfptr->set_facet(                                            // asm mov ebp,[edi]; xor ax,ax; mov al,bl; push eax; call [ebp+20h] slot 8 (the explicit-this form)
        sprite_list,
        facet_byte,
        (int)this_->world_x,                                                  // asm fld [esi+48h]; call __ftol; push eax
        (int)this_->world_y);                                                 // asm fld [esi+4Ch]; call __ftol; push eax (push order y,x,facet -> params facet,x,y)
}

// ---------------------------------------------------------------------------
// can_be_healed @0x0054B8B0 [0x54B8B0,0x54B8EF): biological + damaged + allied
// healer + ACTIVE (state 2) + not garrisoned. gbg_isDamaged = the S1-owned
// hook (routes); gbg_isBiological stays original until owned.
unsigned __int8 __fastcall RGE_Static_Object__can_be_healed_new(
    RGE_Static_Object* this_, DWORD dummy, int player_id)
{
    if (!this_->vfptr->gbg_isBiological(this_))                               // asm call [eax+60h] slot 24; test al,al; jz loc_54B8EB
        return 0;
    if (!this_->vfptr->gbg_isDamaged(this_))                                  // asm call [edx+64h] slot 25; test al,al; jz loc_54B8EB
        return 0;
    if (this_->owner->vfptr->isAlly(this_->owner, player_id) == 0)            // asm push player_id; call [eax+24h] slot 9; test eax,eax; jz loc_54B8EB
        return 0;
    if (this_->object_state != 2)                                             // asm cmp byte ptr [esi+58h],2; jnz loc_54B8EB
        return 0;
    if (this_->inside_obj != 0)                                               // asm mov eax,[esi+2Ch]; test; jnz loc_54B8EB
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// can_be_repaired @0x0054B900 [0x54B900,0x54B949): the repair twin - mechanical
// (NOT biological) + damaged + allied + ACTIVE + not garrisoned + not a
// Bridge/Eye Candy master (object_group 42, the registry).
unsigned __int8 __fastcall RGE_Static_Object__can_be_repaired_new(
    RGE_Static_Object* this_, DWORD dummy, int player_id)
{
    if (this_->vfptr->gbg_isBiological(this_))                                // asm call [eax+60h]; test al,al; jnz loc_54B945 (the negated arm)
        return 0;
    if (!this_->vfptr->gbg_isDamaged(this_))                                  // asm call [edx+64h]; test al,al; jz loc_54B945
        return 0;
    if (this_->owner->vfptr->isAlly(this_->owner, player_id) == 0)            // asm push player_id; call [eax+24h]; test eax,eax; jz
        return 0;
    if (this_->object_state != 2)                                             // asm cmp byte ptr [esi+58h],2; jnz
        return 0;
    if (this_->inside_obj != 0)                                               // asm mov eax,[esi+2Ch]; test; jnz
        return 0;
    if (this_->master_obj->object_group == 42)                                // asm mov eax,[esi+14h]; cmp word ptr [eax+1Eh],2Ah; jz
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// capture_frame @0x0054CE20 [0x54CE20,0x54CE5F): the selection-capture
// dispatcher - the capture flag + non-menu mode + outline_type 2/3 (the
// off_54CE64 4-entry table: cases 0/1 = the return; the compare form
// preserves the partition, verified state-by-state at the gate).
void __fastcall RGE_Static_Object__capture_frame_new(
    RGE_Static_Object* this_, DWORD dummy, TDrawArea* render_area, __int16 x, __int16 y)
{
    if (*SDI_Capture_Info == 0)                                               // asm mov eax,SDI_Capture_Info; test; jz locret (common.h binding 0x0077FA70)
        return;
    if ((*base_game)->game_mode == 1)                                         // asm mov eax,base_game; cmp dword ptr [eax+1E0h],1; jz locret
        return;
    unsigned __int8 outline = (*base_game)->outline_type;                     // asm mov dl,[eax+70h]
    if (outline == 2 || outline == 3)                                         // asm cmp eax,3; ja locret; jmp ds:off_54CE64[eax*4] (cases 2,3)
        RGE_Static_Object__capture_frame_3d_square(this_, render_area, x, y); // asm push y/x/render_area; call (idb constant, UNHOOKED)
}

// ---------------------------------------------------------------------------
// draw_back_frame @0x0054CDD0 [0x54CDD0,0x54CE17): the back-frame dispatcher -
// menu mode or outline_type 1 = the 3d cube back; outline_type 2..3 (u8
// unsigned jbe/ja) = the 3d square back.
void __fastcall RGE_Static_Object__draw_back_frame_new(
    RGE_Static_Object* this_, DWORD dummy, TDrawArea* render_area, __int16 x, __int16 y)
{
    unsigned __int8 outline = (*base_game)->outline_type;                     // asm mov al,[eax+70h] (read once)
    if ((*base_game)->game_mode == 1 || outline == 1)                         // asm cmp dword ptr [eax+1E0h],1; jz loc_54CE02; cmp al,1; jz loc_54CE02
        RGE_Static_Object__draw_frame_3d_cube_back(this_, render_area, x, y); // asm push y/x/render_area; call (idb constant, UNHOOKED)
    else if (outline > 1 && outline <= 3)                                     // asm jbe locret_54CE16; cmp al,3; ja locret (u8 unsigned)
        RGE_Static_Object__draw_frame_3d_square_back(this_, render_area, x, y);
}

// ---------------------------------------------------------------------------
// destroy_obj @0x005506B0 [0x5506B0,0x5506F5): the self-destruct - skips the
// terminal states (>= 7 unsigned), exits the container (slot 85 - the S1-owned
// exit_obj routes), unselects, zeroes hp, wakes, parks the state at 7 (slot 53).
void __fastcall RGE_Static_Object__destroy_obj_new(RGE_Static_Object* this_)
{
    if (this_->object_state >= 7)                                             // asm cmp byte ptr [esi+58h],7; jnb loc_5506F3
        return;
    if (this_->inside_obj != 0)                                               // asm mov eax,[esi+2Ch]; test; jz loc_5506C8
        this_->vfptr->exit_obj(this_);                                        // asm call [eax+154h] slot 85
    if ((this_->selected & 1) != 0)                                           // asm test byte ptr [esi+46h],1; jz loc_5506D7
        RGE_Player__unselect_one_object((RGE_Player*)this_->owner, this_);    // asm mov ecx,[esi+18h]; push esi; call (idb constant; the TRIBE_Player* -> RGE_Player* dual-naming cast)
    this_->hp = 0.0f;                                                         // asm mov dword ptr [esi+3Ch],0
    RGE_Static_Object__set_sleep_flag(this_, 0);                              // asm push 0; mov ecx,esi; call (idb constant)
    this_->vfptr->set_object_state(this_, 7);                                 // asm mov edx,[esi]; push 7; call [edx+0D4h] slot 53
}

// ---------------------------------------------------------------------------
// set_attribute_amount @0x005519B0 [0x5519B0,0x5519F8): the carried-amount
// setter - delta adds at 53-bit with the m32 store-round (rule (e), widened
// operands per rule (j)); check_max clamps to the master's i16 max, the gate =
// the x87 C0-only {max<held, NaN} set (test ah,1) -> the macro.
// PRECISION SCOPE: the delta add must compute at 53-bit then store-round once
// (the asm fld/fadd/fstp m32); /fp:fast shrinks the explicit (double)-cast
// form to addss (the incrementWaypoint lesson - the casts do not resist) ->
// the precise bracket.
#pragma float_control(precise, on, push)
void __fastcall RGE_Static_Object__set_attribute_amount_new(
    RGE_Static_Object* this_, DWORD dummy, float attr_amount,
    unsigned __int8 delta, unsigned __int8 check_max)
{
    if (delta != 0)                                                           // asm mov al,[esp+delta]; test; jz loc_5519C4
        this_->attribute_amount_held =
            (float)((double)attr_amount + (double)this_->attribute_amount_held);  // asm fld [esp+attr_amount]; fadd dword ptr [ecx+54h]; fstp [ecx+54h]
    else
        this_->attribute_amount_held = attr_amount;                           // asm mov eax,[esp+attr_amount]; mov [ecx+54h],eax (bit copy)
    if (check_max == 0)                                                       // asm mov al,[esp+check_max]; test; jz locret_5519F7
        return;
    __int16 max_amount = this_->master_obj->attribute_max_amount;             // asm mov edx,[ecx+14h]; movsx eax,word ptr [edx+8Ch]
    if (X87_GT_NAN(this_->attribute_amount_held, (double)max_amount))         // asm fild dword ptr [esp+delta]; fcom dword ptr [ecx+54h]; fnstsw ax; test ah,1; jz loc_5519F5 (the store fires on {max<held, NaN})
        this_->attribute_amount_held = (float)max_amount;                     // asm fstp dword ptr [ecx+54h]
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// more_room @0x005513C0 [0x5513C0,0x55140A): the garrison capacity query -
// needed = the arriving group's garrisoned_count + 1 (or 1), plus the current
// list size, <= the master's capacity byte (the signed setle).
unsigned __int8 __fastcall RGE_Static_Object__more_room_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    int needed = 1;                                                           // asm mov eax,1
    if (obj != 0)                                                             // asm mov ecx,[esp+8+obj]; test; jz loc_5513DA
        needed = obj->vfptr->garrisoned_count(obj) + 1;                       // asm call [eax+288h] slot 162 (S1-owned -> routes); inc eax
    int current = 0;
    if (this_->objects != 0)                                                  // asm mov ecx,[esi+30h]; test; jnz loc_5513F5
        current = this_->objects->Number_of_objects;                          // asm mov ecx,[ecx+4]
    return needed + current <= this_->master_obj->obj_max;                    // asm add ecx,eax; mov bl,[edx+38h]; cmp ecx,ebx; setle al
}

// ---------------------------------------------------------------------------
// rehook @0x0054F4B0 [0x54F4B0,0x54F4FC): the post-load re-registration -
// re-teleports at the current position (slot 42; the double return DISCARDED
// by the original's fstp st @0x54F4CE), then re-enters the container. The
// inside_obj slot is DUAL-LIFETIME: the live pointer during play (add_obj
// stores it), the ENCODED container id (id + 1) between setup_read (asm
// 0x54C5AD inc eax, byte-verified 40h) and this call; save writes the plain
// id (asm 0x54F5B2/BF). The decode is the -1 (asm 0x54F4D5 dec eax,
// byte-verified 48h) + the world's id lookup (the index-lookup callee
// @0x623DB0, the negative pool bounded @+0xC0 / the positive table @+0xAC
// bounded @+0xB4). Faithful in both lifetimes: with a live pointer in the
// slot the original's own ptr-1 index misses the bounds checks and skips
// the re-enter - the identical form. REVIEW ITEM 1: the pointer-typed
// field's value consumed as the encoded id - owner-approved 2026-11-26
// (the IDB itself puns the store side: setup_read renders
// inside_obj = (RGE_Static_Object*)(v17 + 1)).
void __fastcall RGE_Static_Object__rehook_new(RGE_Static_Object* this_)
{
    this_->vfptr->teleport(                                                   // asm push world_z/y/x; mov ecx,esi; call [eax+0A8h] slot 42; fstp st (the return DISCARDED)
        this_, this_->world_x, this_->world_y, this_->world_z);
    if (this_->inside_obj == 0)                                               // asm mov eax,[esi+2Ch]; test eax,eax; jz loc_54F4FA
        return;
    int encoded_container = (int)this_->inside_obj;                           // the encoded-id value read (see the header note)
    this_->inside_obj = 0;                                                    // asm mov dword ptr [esi+2Ch],0
    RGE_Static_Object* container = RGE_Game_World__object(                    // asm dec eax; push eax; mov ecx,[edx+8Ch]; call :6086 (the TRIBE_World family cast)
        (RGE_Game_World*)this_->owner->world, encoded_container - 1);
    if (container == 0)                                                       // asm test eax,eax; jz loc_54F4FA
        return;
    this_->vfptr->enter_obj(this_, container, 1);                             // asm push 1; push eax; call [edx+150h] slot 84
}

// ===================== BATCH S2b (2026-11-26): the mid-tier methods (88-105B) =====================

// ---------------------------------------------------------------------------
// heal @0x00551A00 [0x551A00,0x551A59): the hp adjuster - the sum stores
// rounded (fst m32) while st0 stays UNROUNDED for the drain-floor gate; both
// gates are the x87 C0-only {<, NaN} fall-through sets (test ah,1 x2 - a NaN
// healing FLOORS in the original; the literal && form would skip -> the
// macros); the cap gate is the literal > MATCH row (test ah,41h; jz -> the
// {> ordered} skip).
// PRECISION SCOPE: the sum must compute at 53-bit with ONE m32 store-round
// (the asm fld/fadd/fst); /fp:fast shrinks the cast form -> the precise
// bracket (the S2a pair).
#pragma float_control(precise, on, push)
unsigned __int8 __fastcall RGE_Static_Object__heal_new(
    RGE_Static_Object* this_, DWORD dummy, float healing)
{
    double sum = (double)healing + (double)this_->hp;                         // asm fld [esp+healing]; fadd dword ptr [ecx+3Ch]
    this_->hp = (float)sum;                                                   // asm fst dword ptr [ecx+3Ch] (the store-round; st0 kept unrounded)
    if (X87_LT_NAN(healing, 0.0f) && X87_LT_NAN(sum, 1.0))                    // asm fcomp ds:const_f_zero / ds:const_f_one; fnstsw ax; test ah,1; jz loc_551A31/loc_551A33
        this_->hp = 1.0f;                                                     // asm mov dword ptr [ecx+3Ch],1.0
    __int16 master_hp = this_->master_obj->hp;                                // asm mov eax,[ecx+14h]; movsx edx,word ptr [eax+32h] (i16 @50)
    if (master_hp > this_->hp)                                                // asm fild; fcom [ecx+3Ch]; test ah,41h; jz loc_551A54 -> return 0
        return 0;
    this_->hp = (float)master_hp;                                             // asm fstp dword ptr [ecx+3Ch] (the exact i16->float clamp)
    return 1;
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// create_object_list @0x0054C850 [0x54C850,0x54C8AB): the container list
// factory - operator_new + the (8,8) ctor; the allocation-unwind SEH chunk
// @0x6448B0 folded (the M11 scoped form).
RGE_New_Object_List* __fastcall RGE_Static_Object__create_object_list_new(
    RGE_Static_Object* this_)
{
    RGE_New_Object_List* list = 0;
    __try
    {
        list = (RGE_New_Object_List*)operator_new_internal(sizeof(RGE_New_Object_List));   // asm push 14h; call operator_new (sizeof: {List,Number_of_objects,ListSize,GrowSize,DeletingAll} = 20, the 1b-1 walk)
        if (list != 0)                                                        // asm test eax,eax; jz loc_54C89A
            RGE_New_Object_List__RGE_New_Object_List(list, 8, 8);             // asm push 8/8; mov ecx,eax; call (the ctor constant)
    }
    __except (operator_delete_internal(list), EXCEPTION_CONTINUE_SEARCH)      // RGE_Static_Object__create_object_list_SEH @0x6448B0
    {
    }
    return list;
}

// ---------------------------------------------------------------------------
// create_sprite_list @0x0054C8B0 [0x54C8B0,0x54C90D): the sprite-list factory
// - the create_object_list twin; the chunk @0x6448D0 folded.
RGE_Active_Sprite_List* __fastcall RGE_Static_Object__create_sprite_list_new(
    RGE_Static_Object* this_)
{
    RGE_Active_Sprite_List* list = 0;
    __try
    {
        list = (RGE_Active_Sprite_List*)operator_new_internal(sizeof(RGE_Active_Sprite_List));   // asm push 0Ch; call operator_new (sizeof: {vfptr,list,owner} = 12)
        if (list != 0)                                                        // asm test eax,eax; jz loc_54C8FB
            RGE_Active_Sprite_List__RGE_Active_Sprite_List(list, this_);      // asm push esi (owner_in); mov ecx,eax; call (the ctor constant)
    }
    __except (operator_delete_internal(list), EXCEPTION_CONTINUE_SEARCH)      // RGE_Static_Object__create_sprite_list_SEH @0x6448D0
    {
    }
    return list;
}

// ---------------------------------------------------------------------------
// remove_shadows @0x00552940 [0x552940,0x55299D): the shadow-rect redraw
// request - the four bounds each compute at 53-bit then ftol once
// (rule (a)/(f)); y1 subtracts the ftol'd radius_z before the -1.
// PRECISION SCOPE: the same shrink class -> the precise bracket.
#pragma float_control(precise, on, push)
void __fastcall RGE_Static_Object__remove_shadows_new(RGE_Static_Object* this_)
{
    RGE_Master_Static_Object* master = this_->master_obj;                     // asm mov edi,[esi+14h]
    RGE_Map__request_redraw(                                                  // asm push 0/y2/x2/y1/x1; mov ecx,owner->world->map; call (the constant)
        this_->owner->world->map,                                             // asm mov eax,[esi+18h]; mov ecx,[eax+8Ch]; mov ecx,[ecx+34h]
        (int)((double)this_->world_x - (double)master->radius_x) - 1,         // asm fld [esi+48h]; fsub [edi+3Ch]; call __ftol; dec eax
        (int)((double)this_->world_y - (double)master->radius_y) - (int)master->radius_z - 1,   // asm fld [esi+4Ch]; fsub [edi+40h]; __ftol; fld [edi+44h]; __ftol; sub ebx,eax; dec ebx
        (int)((double)master->radius_x + (double)this_->world_x),             // asm fld [edi+3Ch]; fadd [esi+48h]; __ftol
        (int)((double)master->radius_y + (double)this_->world_y),             // asm fld [edi+40h]; fadd [esi+4Ch]; __ftol
        0);                                                                   // asm push 0 (attribute)
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// gbg_isPadawan @0x0054B170 [0x54B170,0x54B1CF): the padawan master test.
// BAKED (the jedimaster.cpp:47 entry collision, the EXPL_DROID class - the
// txt-driven full replacement installed ONLY when the padawan list loads):
// the patch body (group 50 Jedi + the is_id_in_list txt set) when
// padawans_installed, the vanilla hardcoded membership otherwise. The patch
// call site retired in place.
// The vanilla id sets: registry candidates (the Jedi masters 50/51 family).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isPadawan_new(
    RGE_Static_Object* this_)
{
    if (padawans_installed)                                                   // jedimaster.cpp:22-25 (the conditional bake)
    {
        if (this_->master_obj->object_group == 50)                            // 50 = Jedi (the registry); jedimaster.cpp:22
            return is_id_in_list(this_->master_obj->copy_id, padawans_list, padawans_n);
        return 0;
    }
    __int16 copy_id = this_->master_obj->copy_id;                             // asm mov ax,[ecx+1Ah] (@26)
    if (copy_id == 180 || copy_id == 183 || copy_id == 232 || copy_id == 204 || copy_id == 52 || copy_id == 239 || copy_id == 647 || copy_id == 652)   // asm cmp ax, imm16 x8 (RESTRUCTURE: the membership chain -> disjunction)
        return 1;
    __int16 id = this_->master_obj->id;                                       // asm mov ax,[ecx+18h] (@24)
    return id == 1075 || id == 1076 || id == 800 || id == 814 || id == 824;   // asm cmp ax, imm16 x5
}

// ---------------------------------------------------------------------------
// get_frame @0x00551310 [0x551310,0x551375): the frame resolver - sprite null
// -> 0; the sprite-list form (slot dispatch -> the constant; the u8 return
// zero-extends = the asm's and eax,0FFh) vs the direct sprite form (ecx =
// the sprite; the facet arg = this->facet both ways).
int __fastcall RGE_Static_Object__get_frame_new(
    RGE_Static_Object* this_, DWORD dummy, __int16* min_x, __int16* min_y,
    __int16* max_x, __int16* max_y)
{
    if (this_->sprite == 0)                                                   // asm mov ecx,[eax+1Ch]; test ecx,ecx; jnz loc_551320; xor eax,eax
        return 0;
    if (this_->sprite_list != 0)                                              // asm mov esi,[eax+24h]; test esi,esi; jz loc_551352
        return RGE_Active_Sprite_List__get_frame(                             // asm push facet/sprite/max_y/max_x/min_y/min_x; mov ecx,esi; call (u8 return)
            this_->sprite_list, min_x, min_y, max_x, max_y,
            this_->sprite, this_->facet);
    return RGE_Sprite__get_frame(this_->sprite, min_x, min_y, max_x, max_y,   // asm push 0/facet/max_y/max_x/min_y/min_x; call (ecx = the sprite from 0x551313)
        this_->facet, 0);
}

// ---------------------------------------------------------------------------
// give_attribute_to_owner @0x0054C910 [0x54C910,0x54C976): the death/ownership
// transfer - for each master attribute slot with flag in {1,2} and a
// non-negative type id, the owner gains the held amount (the raw float bits).
void __fastcall RGE_Static_Object__give_attribute_to_owner_new(
    RGE_Static_Object* this_)
{
    for (size_t i = 0; i < _countof(this_->master_obj->attribute_flag); i++)   // asm esi 0x80->0x8Ch step 4; edi 0..2; the type cursor 0x7A->0x80 step 2 (the parallel cursors -> the indexed form)
    {
        RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov eax,[ebx+14h] (the per-iteration reload)
        unsigned __int8 flag = master->attribute_flag[i];                     // asm mov cl,[eax+edi+98h]
        if (flag != 1 && flag != 2)                                           // asm cmp cl,1; jb loc_54C959; cmp cl,2; ja loc_54C959
            continue;
        __int16 attribute_type = master->attribute_type_held[i];              // asm mov dx,[ecx+eax]
        if (attribute_type < 0)                                               // asm test dx,dx; jl loc_54C959
            continue;
        this_->owner->vfptr->add_attribute_num(this_->owner, attribute_type,  // asm push 0/amount/type; call [ebp+80h] slot 32 (the amount = the raw bits - no arithmetic)
            master->attribute_amount_held[i], 0);
    }
}

// ---------------------------------------------------------------------------
// take_attribute_from_owner @0x0054C980 [0x54C980,0x54C9DB): the give twin -
// the flag == 2 gate ONLY, and the amount NEGATED (fld/fchs - the exact sign
// flip) before the dispatch.
void __fastcall RGE_Static_Object__take_attribute_from_owner_new(
    RGE_Static_Object* this_)
{
    for (size_t i = 0; i < _countof(this_->master_obj->attribute_flag); i++)   // asm esi 0x80->0x8Ch step 4; edi 0..2; the type cursor 0x7A->0x80 step 2
    {
        RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov eax,[ecx+14h] (the per-iteration reload)
        if (master->attribute_flag[i] != 2)                                   // asm cmp byte ptr [eax+edi+98h],2; jnz loc_54C9C6
            continue;
        __int16 attribute_type = master->attribute_type_held[i];              // asm mov dx,[eax+ebp]
        if (attribute_type < 0)                                               // asm test dx,dx; jl loc_54C9C6
            continue;
        this_->owner->vfptr->add_attribute_num(this_->owner, attribute_type,  // asm push 0; fld [esi+eax]; fchs; fstp (the negated amount); push type; call [ebx+80h] slot 32
            -master->attribute_amount_held[i], 0);
    }
}

// ---------------------------------------------------------------------------
// draw_front_frame @0x0054CD50 [0x54CD50,0x54CDB7): the front-frame dispatcher
// - menu mode or outline_type 1 = the 3d cube front; outline_type 0 = the vtbl
// draw_frame; outline_type 2..3 = the 3d square front (the off_54CDBC table;
// the compare form preserves the partition, verified state-by-state at the
// gate - the S2a capture_frame note).
void __fastcall RGE_Static_Object__draw_front_frame_new(
    RGE_Static_Object* this_, DWORD dummy, TDrawArea* render_area, __int16 x, __int16 y)
{
    if ((*base_game)->game_mode == 1)                                         // asm cmp dword ptr [eax+1E0h],1; jnz loc_54CD75 (the cube arm = loc_54CD5E, also the jump-table case 1)
        RGE_Static_Object__draw_frame_3d_cube_front(this_, render_area, x, y);   // asm push y/x/render_area; call (idb constant, UNHOOKED)
    else
    {
        unsigned __int8 outline = (*base_game)->outline_type;                 // asm mov dl,[eax+70h]
        if (outline == 1)                                                     // the off_54CDBC case 1 -> loc_54CD5E
            RGE_Static_Object__draw_frame_3d_cube_front(this_, render_area, x, y);
        else if (outline == 0)                                                // the case 0
            this_->vfptr->draw_frame(this_, render_area, x, y);               // asm mov eax,[ecx]; call [eax+90h] slot 36
        else if (outline == 2 || outline == 3)                                // the cases 2,3
            RGE_Static_Object__draw_frame_3d_square_front(this_, render_area, x, y);   // asm push y/x/render_area; call (idb constant, UNHOOKED)
    }
}

// ---------------------------------------------------------------------------
// play_unload_sound @0x0054B370 [0x54B370,0x54B3D9): the unload sound player -
// the 3d sound info gate, the world's gbg_unknown_4 sound index (the
// gbg_unknown_3 sibling @+0x19C; the semantics UNKNOWN -> the registry
// candidate), the sounds-table entry gate, then the play with the lingering
// -1 culture push (the enter_target precedent) and the S1-owned civ hook.
// THE STALE-VALUE RETURN FLOW: the early exits return the intermediate
// pointers as the int (the .c's result tracking - faithful).
int __fastcall RGE_Static_Object__play_unload_sound_new(RGE_Static_Object* this_)
{
    int pan = 0;
    int volume = 0;
    int result = RGE_Static_Object__get_3d_sound_info(this_, &pan, &volume);  // asm lea volume/pan; push; mov ecx,esi; call (char return)
    if ((unsigned __int8)result != 0)                                         // asm test al,al; jz loc_54B3D3
    {
        TRIBE_World* world = this_->owner->world;                             // asm mov edx,[esi+18h]; mov eax,[edx+8Ch]
        result = (int)world;                                                  // the stale-value flow
        int sound_index = world->gbg_unknown_4;                               // asm mov edi,[eax+1A0h]
        if (sound_index != -1)                                                // asm cmp edi,0FFFFFFFFh; jz loc_54B3D3
        {
            RGE_Sound** sounds = world->sounds;                               // asm mov eax,[eax+3Ch]
            result = (int)sounds;                                             // the stale-value flow
            if (sounds[sound_index] != 0)                                     // asm cmp dword ptr [eax+edi*4],0; jz loc_54B3D3
            {
                int civ = this_->vfptr->gbg_get_civ_override(this_);          // asm push -1 (the play culture arg LINGERING - the enter_target precedent); mov ecx,esi; call [edx+8] slot 2 (the S1-owned hook routes)
                result = RGE_Sound__play(sounds[sound_index], 1, pan, volume, // asm push eax(civ)/volume/pan/1; mov ecx,[eax+3Ch]; mov ecx,[ecx+edi*4]; call :3662
                    (short)civ, -1);
            }
        }
    }
    return result;                                                            // asm loc_54B3D3 (the shared exit; eax per the flow above)
}

// ---------------------------------------------------------------------------
// change_unique_id @0x0054C7E0 [0x54C7E0,0x54C84A): the id re-roller - remove
// from the player list (slot 73), the recyclable-gated id generator, the id
// store, re-add (slot 72) keeping the returned list node.
void __fastcall RGE_Static_Object__change_unique_id_new(RGE_Static_Object* this_)
{
    this_->owner->vfptr->removeObject(                                        // asm mov edx,[esi+68h]; mov ecx,[esi+18h]; push node/dopple/sleep/this; call [eax+124h] slot 73
        this_->owner, this_, this_->sleep_flag, this_->dopple_flag,
        this_->player_object_node);
    int new_id;
    if (this_->master_obj->recyclable != 0)                                   // asm mov eax,[esi+14h]; mov cl,[eax+0BCh]; test; jz loc_54C81C
        new_id = RGE_Game_World__get_next_reusable_object_id(                 // asm mov ecx,[ecx+8Ch]; call :623E60 (the TRIBE_World family cast)
            (RGE_Game_World*)this_->owner->world);
    else
        new_id = RGE_Game_World__get_next_object_id(                          // asm mov ecx,[edx+8Ch]; call :623E00
            (RGE_Game_World*)this_->owner->world);
    this_->id = new_id;                                                       // asm mov [esi+10h],eax
    this_->player_object_node = this_->owner->vfptr->addObject(               // asm push dopple/sleep/this; call [eax+120h] slot 72; mov [esi+68h],eax
        this_->owner, this_, this_->sleep_flag, this_->dopple_flag);
}

// ---------------------------------------------------------------------------
// rotate @0x00551930 [0x551930,0x55198B): the facet rotator - the int-domain
// wrap (facet + amount, the do-while up-wrap by the sprite's facet_num while
// negative, the while down-wrap); the tail IS set_facet's body inlined (cmp /
// store / the null-guarded slot-8 dispatch @0x551957-0x551983) -> the owned
// replacement call per the inlined-function rule.
void __fastcall RGE_Static_Object__rotate_new(
    RGE_Static_Object* this_, DWORD dummy, int amount)
{
    int facet_num = this_->sprite->facet_num;                                 // asm mov eax,[esi+1Ch]; movsx eax,word ptr [eax+70h] (NO null guard - faithful)
    int wrapped = amount + this_->facet;                                      // asm mov bl,[esi+45h] (zero-extended); add ebx,ecx
    if (wrapped < 0)                                                          // asm jns loc_55194D
    {
        do                                                                    // asm loc_551949: add ebx,eax; js loc_551949
            wrapped += facet_num;
        while (wrapped < 0);
    }
    while (wrapped >= facet_num)                                              // asm cmp ebx,eax; jl loc_551957; sub ebx,eax; cmp; jge
        wrapped -= facet_num;
    RGE_Static_Object__set_facet_new(this_, 0, (char)(unsigned __int8)wrapped);   // the inlined set_facet tail -> the owned call (the routing ruling)
}

// ===================== BATCH S2c (2026-11-26): the mid tier (108-134B, part 1) =====================

// ---------------------------------------------------------------------------
// set_sleep_flag @0x005529A0 [0x5529A0,0x552A0D): the flag machine - the
// byte-domain compare early-out; sleep-set = goto_sleep+sleep; the wake with
// goto_sleep armed = the paired clears; the wake WITHOUT it = the full
// player-list re-registration (remove slot 73 / add slot 72).
void __fastcall RGE_Static_Object__set_sleep_flag_new(
    RGE_Static_Object* this_, DWORD dummy, char flag_in)
{
    unsigned __int8 flag_byte = (unsigned __int8)flag_in;                 // asm mov cl,[esp+4+flag_in]
    if (this_->sleep_flag == flag_byte)                                   // asm mov al,[esi+59h]; cmp al,cl; jz loc_552A0B (byte-width)
        return;
    if (flag_byte != 0)                                                   // asm test cl,cl; jnz loc_552A04
    {
        this_->goto_sleep_flag = 1;                                       // asm mov byte ptr [esi+5Bh],1
        this_->sleep_flag = flag_byte;                                    // asm mov [esi+59h],cl
        return;
    }
    if (this_->goto_sleep_flag != 0)                                      // asm mov cl,[esi+5Bh]; test cl,cl; jz loc_5529C5
    {
        this_->sleep_flag = 0;                                            // asm mov byte ptr [esi+59h],0
        this_->goto_sleep_flag = 0;                                       // asm mov byte ptr [esi+5Bh],0
        return;
    }
    this_->owner->vfptr->removeObject(this_->owner, this_,                // asm push node/dopple/sleep(the entry sleep_flag)/this; call [ebp+124h] slot 73
        this_->sleep_flag, this_->dopple_flag, this_->player_object_node);
    this_->sleep_flag = 0;                                                // asm mov byte ptr [esi+59h],0 @0x5529EB - BETWEEN the two calls (the DESYNC FIX: the dropped interleaved store - the object re-registered awake while its own flag stayed set -> the later crash)
    this_->player_object_node = this_->owner->vfptr->addObject(           // asm push dopple/0/this; call [eax+120h] slot 72; mov [esi+68h],eax
        this_->owner, this_, 0, this_->dopple_flag);
}

// ---------------------------------------------------------------------------
// spawn_death_obj @0x0054F440 [0x54F440,0x54F4B0): the death-spawn factory -
// the signed death_spawn_obj_id gate, the civ-dependent master (the world's
// master_players[civ] = the RGE_Master_Player* whose master_objects @+0x30)
// with the owner's own list fallback, then the master make_new_obj (slot 12).
RGE_Static_Object* __fastcall RGE_Static_Object__spawn_death_obj_new(
    RGE_Static_Object* this_)
{
    __int16 spawn_id = this_->master_obj->death_spawn_obj_id;             // asm mov eax,[esi+14h]; mov di,[eax+58h] (@88 word)
    if (spawn_id < 0)                                                     // asm test di,di; jl loc_54F4AA (signed)
        return 0;
    RGE_Master_Static_Object* master = 0;
    TRIBE_World* world = (*base_game)->world;                             // asm mov ecx,base_game; mov ebx,[ecx+420h]
    if (world != 0)                                                       // asm test ebx,ebx; jz loc_54F481
    {
        int civ = this_->vfptr->gbg_get_civ_override(this_);              // asm mov di,[edx+58h] (the live reload); mov ecx,esi; call [eax+8] slot 2 (the S1-owned hook routes)
        master = world->master_players[civ]->master_objects[spawn_id];    // asm mov ecx,[ebx+54h]; mov edx,[ecx+eax*4]; mov ecx,[edx+30h]; mov ecx,[ecx+eax*4] (RGE_Master_Player::master_objects @+0x30)
    }
    if (master == 0)                                                      // asm test ecx,ecx; jnz loc_54F491
        master = this_->owner->master_objects[spawn_id];                  // asm mov eax,[esi+18h]; mov ecx,[eax+74h]; mov ecx,[ecx+edx*4]
    if (master == 0)                                                      // asm test ecx,ecx; jz loc_54F4AA
        return 0;
    return master->vfptr->make_new_obj(                                   // asm mov edx,[ecx]; push world_z/y/x/owner; call [edx+30h] slot 12
        master, (RGE_Player*)this_->owner, this_->world_x, this_->world_y,
        this_->world_z);
}

// ---------------------------------------------------------------------------
// handle_teleport_z_correction @0x0054FD20 [0x54FD20,0x54FD91): the base z
// corrector - movement_type 0 = the raw-bit set; 1 = the flyer rules (the AIR
// group set {43,48,59,62,63,64} - the registry - get ground_z + the DOUBLE
// 1.0 per the dd rule; else the world_z < ground_z floor - the literal <
// MATCH row, NaN skips); 2+ = untouched. ALL paths return the STORED m32
// reloaded (the corollary-(h) float return form).
// PRECISION SCOPE: ground_z + 1.0 must compute at 53-bit then store-round
// once -> the precise bracket.
#pragma float_control(precise, on, push)
double __fastcall RGE_Static_Object__handle_teleport_z_correction_new(
    RGE_Static_Object* this_, DWORD dummy, float ground_z)
{
    unsigned __int8 movement = this_->master_obj->movement_type;          // asm mov al,[edx+78h] (@120)
    if (movement == 0)                                                    // asm sub eax,0; jz loc_54FD86
    {
        this_->world_z = ground_z;                                        // asm mov edx,[esp+ground_z]; mov [ecx+50h],edx (the bit copy)
    }
    else if (movement == 1)                                               // asm dec eax; jnz loc_54FD8D
    {
        __int16 group = this_->master_obj->object_group;                  // asm mov ax,[edx+1Eh]
        if (group == 48 || group == 63 || group == 64 || group == 43 || group == 62 || group == 59)   // asm cmp ax,30h/3Fh/40h/2Bh/3Eh/3Bh; jz loc_54FD73
            this_->world_z = (float)(ground_z + 1.0);                     // asm fld [esp+ground_z]; fadd ds:const_d_one; fstp dword ptr [ecx+50h] (the store-round)
        else if (this_->world_z < ground_z)                               // asm fld [ecx+50h]; fcomp [esp+ground_z]; test ah,1; jz loc_54FD8D
            this_->world_z = ground_z;                                    // asm mov eax,[esp+ground_z]; mov [ecx+50h],eax
    }
    return this_->world_z;                                                // asm fld dword ptr [ecx+50h] (the m32 reload)
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// gbg_unknown23 @0x0054B950 [0x54B950,0x54B9C4): the worker armament test -
// group 58 (Worker) with a sprite: the sprite NAME's last digit in
// {'1','3','4','5','7'} (the get_string_id civ-suffix family); else the
// object_flags bit 4 (mechanical - the registry). The original calls the pure
// master get_sprite 3x (0x54B961/B976/B98F) - HOISTED to one call (the
// conditionally-recomputed pure helper rule).
unsigned __int8 __fastcall RGE_Static_Object__gbg_unknown23_new(
    RGE_Static_Object* this_)
{
    RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov ecx,[esi+14h]
    if (master->object_group != 58)                                       // asm cmp word ptr [ecx+1Eh],3Ah; jnz loc_54B9B2 (58 = Worker - the registry)
        return (master->object_flags >> 4) & 1;                           // asm mov eax,[ecx+0ACh]; shr eax,4; and eax,1 (the registry bit 4 = mechanical)
    RGE_Sprite* sprite = master->vfptr->get_sprite(master, this_);        // asm push esi; call [eax+4] slot 1 (x3 in the original - the hoist)
    if (sprite == 0)                                                      // asm test eax,eax; jz loc_54B9B2
        return (master->object_flags >> 4) & 1;
    size_t len = strlen(sprite->name);                                    // asm add edi,54h; xor eax,eax; repne scasb; not ecx; dec ecx (the name @+0x54)
    char digit = sprite->name[len - 1];                                   // asm movsx eax,byte ptr [edi+eax+53h]
    return digit == '1' || digit == '3' || digit == '4' || digit == '5' || digit == '7';   // asm add eax,-31h; the off_54B9C4 cases {0,2,3,4,6} (RESTRUCTURE: the switch -> disjunction, pure function of the digit)
}

// ---------------------------------------------------------------------------
// gbg_isBiological @0x0054B9E0 [0x54B9E0,0x54BA60): the gbg_unknown23 twin -
// the groups {58 Worker, 14 Utility Trawler}, the name digit set
// {'2','6','8'}, the object_flags bit 5 (biological - the registry).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isBiological_new(
    RGE_Static_Object* this_)
{
    RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov ecx,[esi+14h]
    __int16 group = master->object_group;                                 // asm mov ax,[ecx+1Eh]
    if (group != 58 && group != 14)                                       // asm cmp ax,3Ah; jz loc_54B9F7; cmp ax,0Eh; jnz loc_54BA4E (the registry)
        return (master->object_flags >> 5) & 1;                           // asm mov eax,[ecx+0ACh]; shr eax,5; and eax,1 (the registry bit 5 = biological)
    RGE_Sprite* sprite = master->vfptr->get_sprite(master, this_);        // asm call [eax+4] slot 1 (x3 in the original - the hoist)
    if (sprite == 0)                                                      // asm test eax,eax; jz loc_54BA4E
        return (master->object_flags >> 5) & 1;
    size_t len = strlen(sprite->name);                                    // the name @+0x54 (the strlen chain)
    // BAKED (the medic_worker stub, langdll.cpp:178/321 - the hook site
    // 0x54BA2B retired 2026-11-26): the two-digit-civ suffix support - the
    // tens digit '1' re-bases the ones-digit decode (the stub's cmp 31h /
    // mov edx,26h form); the not-'1' base 30h = the vanilla set. The stub
    // was UNCONDITIONALLY installed ("KEEP - live"), so the bake is too.
    char civ = sprite->name[len - 1] - '0';  
    if (sprite->name[len - 2] == '1') civ += 10;

    switch (civ)
    {
    case 2:
    case 6:
    case 10:
        return true;
    default:
        return false;
    }
}

// ---------------------------------------------------------------------------
// gbg_isDetector @0x0054BBC0 [0x54BBC0,0x54BC35): the detector test -
// object_flags bit 3 (detector - the registry); group {50 Jedi, 51 Jedi
// w-Holocron} with attributes[58] (Masters Can See Hidden Units) > 0; group
// {11 Cruiser, 13 Destroyer, 15 Frigate 1, 16 AA Destroyer 1} with
// attributes[23] (Submarine Detection) > 0. The attribute gates are the
// literal > MATCH rows (the 41h jnz targets the skip).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isDetector_new(
    RGE_Static_Object* this_)
{
    RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov eax,[ecx+14h]
    if ((master->object_flags & 8) != 0)                                  // asm test byte ptr [eax+0ACh],8; jz loc_54BBCF
        return 1;
    __int16 group = master->object_group;                                 // asm mov dx,[eax+1Eh]
    if ((group == 50 || group == 51)                                      // asm cmp dx,32h/33h (the registry)
        && this_->owner->attributes[58] > 0.0f)                           // asm mov eax,[eax+0ACh]; fld dword ptr [eax+0E8h]; fcomp ds:const_f_zero; test ah,41h; jnz loc_54BBFE
        return 1;
    if ((group == 11 || group == 13 || group == 15 || group == 16)        // asm cmp dx,0Bh/0Dh/0Fh/10h (the registry)
        && this_->owner->attributes[23] > 0.0f)                           // asm fld dword ptr [edx+5Ch]; test ah,41h; jnz loc_54BC32
        return 1;
    return 0;
}

// ---------------------------------------------------------------------------
// isFlagSet @0x00552E20 [0x00552E20,0x00552E95): the two-flag query - flag 2
// = NOT moving (the maximumSpeed slot-98 double dispatch; the {== 0, NaN}
// set -> X87_EQ_NAN - the literal == would take NaN to return 0); flag 1 =
// all three master radii > 0 (the literal > MATCH rows - the 41h jnz targets
// the fail, NaN fails); flags 3/4 = always 1.
int __fastcall RGE_Static_Object__isFlagSet_new(
    RGE_Static_Object* this_, DWORD dummy, int n)
{
    if (n == 2)                                                           // asm cmp eax,2; jnz loc_552E43
    {
        if (!X87_EQ_NAN(this_->vfptr->maximumSpeed(this_), 0.0))          // asm mov eax,[ecx]; call [eax+188h] slot 98 (the double return); fcomp ds:const_f_zero; test ah,40h; jnz loc_552E8F
            return 0;
        return 1;
    }
    if (n == 1)                                                           // asm cmp eax,1; jnz loc_552E80
    {
        RGE_Master_Static_Object* master = this_->master_obj;             // asm mov ecx,[ecx+14h]
        if (master->radius_z > 0.0f && master->radius_x > 0.0f &&         // asm fld [ecx+44h]/[ecx+3Ch]/[ecx+40h]; fcomp ds:const_f_zero; test ah,41h; jnz loc_552E7B x2; jz loc_552E8F
            master->radius_y > 0.0f)
            return 1;
        return 0;
    }
    if (n == 3 || n == 4)                                                 // asm cmp eax,3/4; jz loc_552E8F
        return 1;
    return 0;
}

// ---------------------------------------------------------------------------
// modify_percent @0x00550320 [0x00550320,0x0055039F): the modify family,
// percent form - flag 0 = the hp percent (the 53-bit chain, +0.5f, ftol, THE
// i16 NARROWING, fild back - the master-side trio family form); flag 1 = the
// LOS re-explore at ftol(amount * master los) (the unexplore slot 89 /
// explore slot 88 dispatches).
// PRECISION SCOPE: both chains compute at 53-bit then truncate once -> the
// precise bracket.
#pragma float_control(precise, on, push)
void __fastcall RGE_Static_Object__modify_percent_new(
    RGE_Static_Object* this_, DWORD dummy, float amount, unsigned __int8 flag)
{
    if (flag != 0)                                                        // asm and eax,0FFh; sub eax,0; jz loc_55037D
    {
        if (flag == 1 && this_->tile != 0 && this_->object_state == 2)    // asm dec eax; jnz loc_55039D; mov eax,[esi+28h]; test; jz; cmp byte ptr [esi+58h],2; jnz
        {
            this_->vfptr->unexplore_terrain(this_, (RGE_Player*)this_->owner, 0, -1, 0);   // asm push 0/0FFFFFFFFh/0/owner; call [eax+164h] slot 89 (the dual-naming cast)
            this_->vfptr->explore_terrain(this_, (RGE_Player*)this_->owner, 0,         // asm fld [esp+amount]; fmul [edx+34h] (the master los @52); push 0; call __ftol; push eax/0/owner; call [edi+160h] slot 88
                (int)((double)amount * (double)this_->master_obj->los), 0);
        }
        return;
    }
    this_->hp = (float)(short)(int)((double)this_->hp * (double)amount    // asm fld dword ptr [esi+3Ch]; fmul [esp+amount]; fadd ds:const_f_half; call __ftol; movsx ecx,ax; fild; fstp [esi+3Ch]
        + (double)0.5f);
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// THE ZONE FAMILY - the shared resolve chain (the zoneMapIndex @+0x80 gate:
// -1 -> get_zone_map(map_zones, terrains[master terrain @+0x76],
// world terrain_size @+0x5E, &the index); else get_zone_map2(index)) + the
// per-function zone-map query. The callers: the Gather set_state pair (the
// AI-water check), the Transport drop-off, the AI staging.
// lookupZone @0x00552610 [0x00552610,0x00552691): the tile lookup.
unsigned __int8 __fastcall RGE_Static_Object__lookupZone_new(
    RGE_Static_Object* this_, DWORD dummy, int x, int y)
{
    if (x < 0 || y < 0)                                                   // asm test ebx,ebx; jl loc_55268C; test edi,edi; jl
        return 0;
    RGE_Zone_Map* zone_map;
    if (this_->zoneMapIndex == -1)                                        // asm cmp eax,0FFFFFFFFh; jnz loc_552661
        zone_map = RGE_Zone_Map_List__get_zone_map(                       // asm push esi/num/rules; mov ecx,[edx+0BF7Ch]; call :625840
            this_->owner->world->map->map_zones,
            this_->owner->world->terrains[this_->master_obj->terrain],    // asm movsx edx,word ptr [ecx+76h]; mov ecx,[eax+60h]; mov ecx,[ecx+edx*4]
            this_->owner->world->terrain_size,                            // asm movsx edx,word ptr [eax+5Eh]
            &this_->zoneMapIndex);                                        // asm lea esi,[ecx+80h]; push esi
    else
        zone_map = RGE_Zone_Map_List__get_zone_map2(                      // asm push eax; call :625FD0
            this_->owner->world->map->map_zones, this_->zoneMapIndex);
    if (zone_map == 0)                                                    // asm test eax,eax; jz loc_55268C
        return 0;
    return RGE_Zone_Map__get_zone_info(zone_map, x, y);                   // asm push y/x; mov ecx,eax; call :625840-region
}

// ---------------------------------------------------------------------------
// currentZone @0x005526A0 [0x005526A0,0x00552721): the unit's own tile zone.
unsigned __int8 __fastcall RGE_Static_Object__currentZone_new(
    RGE_Static_Object* this_)
{
    RGE_Zone_Map* zone_map;
    if (this_->zoneMapIndex == -1)                                        // asm cmp eax,0FFFFFFFFh; jnz loc_5526E2
        zone_map = RGE_Zone_Map_List__get_zone_map(
            this_->owner->world->map->map_zones,
            this_->owner->world->terrains[this_->master_obj->terrain],
            this_->owner->world->terrain_size,
            &this_->zoneMapIndex);
    else
        zone_map = RGE_Zone_Map_List__get_zone_map2(
            this_->owner->world->map->map_zones, this_->zoneMapIndex);
    if (zone_map == 0)                                                    // asm test edi,edi; jz loc_55271C
        return 0;
    return RGE_Zone_Map__get_zone_info(zone_map, (int)this_->world_x,     // asm fld [esi+48h]/[esi+4Ch]; call __ftol x2; push y/x; mov ecx,edi; call (the direct-load ftols)
        (int)this_->world_y);
}

// ---------------------------------------------------------------------------
// withinRangeOfZone @0x00552730 [0x00552730,0x005527B0): the cross-zone range
// query (the transport drop-off gate).
int __fastcall RGE_Static_Object__withinRangeOfZone_new(
    RGE_Static_Object* this_, DWORD dummy, unsigned __int8 zone, float range,
    XYPoint* goal)
{
    RGE_Zone_Map* zone_map;
    if (this_->zoneMapIndex == -1)                                        // asm cmp eax,0FFFFFFFFh; jnz loc_55276F
        zone_map = RGE_Zone_Map_List__get_zone_map(
            this_->owner->world->map->map_zones,
            this_->owner->world->terrains[this_->master_obj->terrain],
            this_->owner->world->terrain_size,
            &this_->zoneMapIndex);
    else
        zone_map = RGE_Zone_Map_List__get_zone_map2(
            this_->owner->world->map->map_zones, this_->zoneMapIndex);
    if (zone_map == 0)                                                    // asm test eax,eax; jz loc_5527AD
        return 0;
    return RGE_Zone_Map__withinRange2(zone_map, zone, *goal, range, 0);   // asm push 0/range/goal.y/goal.x/zone; mov ecx,eax; call (the by-value XYPoint = the 2-dword copies)
}

// ---------------------------------------------------------------------------
// findClosestPointInTerrainType @0x005527C0 [0x005527C0,0x00552847): the AI
// staging query (the TribeInformationAIModule callers).
int __fastcall RGE_Static_Object__findClosestPointInTerrainType_new(
    RGE_Static_Object* this_, DWORD dummy, XYPoint p, XYPoint* rVal,
    int type1, int type2, int range)
{
    RGE_Zone_Map* zone_map;
    if (this_->zoneMapIndex == -1)                                        // asm cmp eax,0FFFFFFFFh; jnz loc_5527FF
        zone_map = RGE_Zone_Map_List__get_zone_map(
            this_->owner->world->map->map_zones,
            this_->owner->world->terrains[this_->master_obj->terrain],
            this_->owner->world->terrain_size,
            &this_->zoneMapIndex);
    else
        zone_map = RGE_Zone_Map_List__get_zone_map2(
            this_->owner->world->map->map_zones, this_->zoneMapIndex);
    if (zone_map == 0)                                                    // asm test eax,eax; jz loc_552844
        return 0;
    return RGE_Zone_Map__findClosestPointInTerrainType(zone_map, p, rVal, // asm push range/type2/type1/rVal/p.y/p.x; mov ecx,eax; call
        type1, type2, range);
}

// ===================== BATCH S2c-2 (2026-11-26): the mid tier (141-182B) =====================

// ---------------------------------------------------------------------------
// modify_delta @0x00550290 [0x00550290,0x0055031E): the modify family, delta
// form - flag 0 = the hp delta scaled to the master max ((max + amount) * hp
// / max at 53-bit, ftol, THE i16 NARROWING, fild back - the DIVISION never
// collapses: the precise bracket); flag 1 = the LOS re-explore at
// ftol(amount + master los) (the modify_percent twin's ADD form).
// PRECISION SCOPE: the flag-0 chain (the fadd/fmul/fdiv/ftol) and the flag-1
// sum -> the precise bracket.
#pragma float_control(precise, on, push)
void __fastcall RGE_Static_Object__modify_delta_new(
    RGE_Static_Object* this_, DWORD dummy, float amount, unsigned __int8 flag)
{
    if (flag != 0)                                                        // asm and eax,0FFh; sub eax,0; jz loc_5502ED
    {
        if (flag == 1 && this_->tile != 0 && this_->object_state == 2)    // asm dec eax; jnz loc_55031C; mov eax,[esi+28h]; test; jz; cmp byte ptr [esi+58h],2; jnz
        {
            this_->vfptr->unexplore_terrain(this_, (RGE_Player*)this_->owner, 0, -1, 0);   // asm push 0/0FFFFFFFFh/0/owner; call [eax+164h] slot 89
            this_->vfptr->explore_terrain(this_, (RGE_Player*)this_->owner, 0,         // asm fld [esp+amount]; fadd [edx+34h] (the master los @52 - the ADD form); push 0; call __ftol; push eax/0/owner; call [edi+160h] slot 88
                (int)((double)amount + (double)this_->master_obj->los), 0);
        }
        return;
    }
    __int16 master_hp = this_->master_obj->hp;                            // asm mov ecx,[esi+14h]; movsx edx,word ptr [ecx+32h]
    this_->hp = (float)(short)(int)(((double)master_hp + (double)amount)  // asm fild (the exact i16); fld st; fadd [esp+amount]; fmul [esi+3Ch]; fdiv st,st(1); call __ftol; movsx eax,ax (THE i16 NARROWING); fstp st; fild; fstp [esi+3Ch]
        * (double)this_->hp / (double)master_hp);
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// shadow_draw @0x0054CBD0 [0x0054CBD0,0x0054CC64): the shadow renderer - the
// tile + state < 7 gates; the sprite-list form (the ASL shadow_draw slot 5 =
// +0x14, the WORD offset adds, the facet movzx) vs the direct sprite form
// (RGE_Sprite::shadow_draw with the 0 frame).
void __fastcall RGE_Static_Object__shadow_draw_new(
    RGE_Static_Object* this_, DWORD dummy, TDrawArea* render_area, __int16 x,
    __int16 y, unsigned __int8 flag)
{
    if (this_->tile == 0 || this_->object_state >= 7)                     // asm mov ecx,[eax+28h]; test; jz loc_54CC62; cmp byte ptr [eax+58h],7; jnb loc_54CC62
        return;
    if (this_->sprite_list != 0)                                          // asm mov ecx,[eax+24h]; test; jz loc_54CC22
    {
        this_->sprite_list->vfptr->shadow_draw(                           // asm push flag/render_area/color_table/y+off/x+off/facet; call [edx+14h] slot 5
            this_->sprite_list, this_->facet,
            x + this_->shadow_x_offset, y + this_->shadow_y_offset,       // asm mov si,[eax+38h/3Ah]; add si,[esp+x/y] (the WORD adds)
            this_->owner->color_table, render_area, flag);
        return;
    }
    if (this_->sprite == 0)                                               // asm mov ecx,[eax+1Ch]; test; jz loc_54CC62
        return;
    RGE_Sprite__shadow_draw(this_->sprite, this_->facet, 0,               // asm push flag/draw_area/color_table/y/x/0/facet; call (the constant)
        x + this_->shadow_x_offset, y + this_->shadow_y_offset,
        this_->owner->color_table, render_area, flag);
}

// ---------------------------------------------------------------------------
// addToPathingGroup @0x00552890 [0x00552890,0x00552928): the pathing-group
// append - the size < max fast path; the grow (0 -> 24, else 2x;
// operator_new(4*max), the indexed copy, the -1 tail fill, the old free);
// the alloc failure = the silent return (faithful - no append).
void __fastcall RGE_Static_Object__addToPathingGroup_new(
    RGE_Static_Object* this_, DWORD dummy, int uID)
{
    if (this_->mPathingGroupSize >= this_->mMaximumPathingGroupSize)      // asm cmp ecx,eax; jl loc_552911
    {
        if (this_->mMaximumPathingGroupSize != 0)                         // asm test eax,eax; jnz loc_5528AB
            this_->mMaximumPathingGroupSize = 2 * this_->mPathingGroupSize;   // asm lea eax,[ecx+ecx]
        else
            this_->mMaximumPathingGroupSize = 24;                         // asm mov dword ptr [esi+74h],18h
        int* fresh = (int*)operator_new_internal(                          // asm mov ecx,[esi+74h]; shl ecx,2; push; call operator_new (the runtime-size expression form)
            this_->mMaximumPathingGroupSize * sizeof(int));
        if (fresh != 0)                                                    // asm test edi,edi; jz loc_552925
        {
            if (this_->mPathingGroup != 0)                                 // asm mov eax,[esi+6Ch]; test; jz loc_55290E
            {
                int i = 0;
                for (; i < this_->mPathingGroupSize; i++)                   // asm loc_5528D6: the copy loop (the .c's v5[i-1] walk -> indexed)
                    fresh[i] = this_->mPathingGroup[i];
                for (; i < this_->mMaximumPathingGroupSize; i++)            // asm loc_5528F1: the -1 tail fill
                    fresh[i] = -1;
                operator_delete_internal(this_->mPathingGroup);            // asm push edx; call operator_delete
            }
            this_->mPathingGroup = fresh;                                  // asm mov [esi+6Ch],edi
        }
        else
        {
            return;                                                        // asm jz loc_552925 (the silent no-append)
        }
    }
    this_->mPathingGroup[this_->mPathingGroupSize] = uID;                 // asm mov ecx,[esi+6Ch]; mov edx,[esp+uID]; mov [ecx+eax*4],edx
    this_->mPathingGroupSize++;                                           // asm inc eax; mov [esi+70h],eax
}

// ---------------------------------------------------------------------------
// is_powered @0x0054BDA0 [0x0054BDA0,0x0054BE3C): the power test - the tile
// coords from ftol(world_x/world_y) with the inside_obj override; the
// self-powered set (the master ids {109, 82, 50 Farm} - 109/82 the registry
// candidates - and the groups {34 Undeployed Cannon, 36 Cannon, 10 Turret,
// 9 Fortress} - the registry); else the RGE_Tile::is_powered query at
// &map_row_offset[row=ftol y][col=ftol x] (the 32-byte column stride).
unsigned __int8 __fastcall RGE_Static_Object__is_powered_new(
    RGE_Static_Object* this_, DWORD dummy, unsigned __int8 flag)
{
    int col = (int)this_->world_x;                                        // asm fld [esi+48h]; call __ftol; mov edi,eax
    int row = (int)this_->world_y;                                        // asm fld [esi+4Ch]; call __ftol
    if (this_->inside_obj != 0)                                           // asm mov ebx,[esi+2Ch]; test; jz loc_54BDD0
    {
        col = (int)this_->inside_obj->world_x;                            // asm fld [ebx+48h]; ftol; mov edi,eax
        row = (int)this_->inside_obj->world_y;                            // asm fld [ebx+4Ch]; ftol
    }
    RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov edx,[esi+14h]
    __int16 id = master->id;                                              // asm mov cx,[edx+18h] (@24)
    if (id == 109 || id == 82 || id == 50)                                // asm cmp cx,6Dh/52h/32h
        return 1;
    __int16 group = master->object_group;                                 // asm mov cx,[edx+1Eh]
    if (group == 34 || group == 36 || group == 10 || group == 9)          // asm cmp cx,22h/24h/0Ah/9 (the registry)
        return 1;
    return RGE_Tile__is_powered(                                          // asm shl edi,5; mov ecx,map_row_offset[+0BF18h]; mov ecx,[ecx+eax*4]; add ecx,edi; push flag/player_id; call (the constant)
        &this_->owner->world->map->map_row_offset[row][col],
        this_->owner->id, flag);                                          // asm mov al,[esi+0A0h] (the owner id @160, u8)
}

// ---------------------------------------------------------------------------
// teleport_off_map @0x0054F770 [0x0054F770,0x0054F819): the off-map
// teleport - the OB deregistration (the Remove slot 11 gated on radius_z >
// 0 - the literal > MATCH row; the DeleteObList slot 16), the map entry
// delete, the LOS un-explore, the tile/position clears (-1.0f bits), the
// unselect + the selected-group clear (slot 217).
void __fastcall RGE_Static_Object__teleport_off_map_new(RGE_Static_Object* this_)
{
    if (this_->tile == 0)                                                 // asm mov eax,[esi+28h]; test; jz loc_54F817
        return;
    RGE_Obstruction_Manager* ob = *OBSystem_Land;                         // asm mov edi,OBSystem_Land (the pointer-to-slot deref)
    if (this_->master_obj != 0 && this_->master_obj->movement_type == 1)  // asm test eax,eax; jz loc_54F798; cmp byte ptr [eax+78h],1; jnz (the sideways-cast proof: the twin structs/vtbls)
        ob = (RGE_Obstruction_Manager*)*OBSystem_Air;                     // asm mov edi,OBSystem_Air
    if (this_->master_obj->radius_z > 0.0f)                               // asm fld [eax+44h]; fcomp ds:const_f_zero; test ah,41h; jnz loc_54F7B0 (the literal > MATCH row)
        ob->vfptr->RemoveObjectObstruction(ob, this_);                    // asm mov eax,[edi]; push esi; mov ecx,edi; call [eax+2Ch] slot 11
    if (this_->ObstructionDataList != 0)                                  // asm mov eax,[esi+64h]; test; jz loc_54F7BF
        ob->vfptr->DeleteObList(ob, this_);                               // asm mov edx,[edi]; push esi; mov ecx,edi; call [edx+40h] slot 16
    RGE_Map__OL_DeleteObjectListEntry((*base_game)->world->map,           // asm mov ecx,base_game; push theObj/theTile; mov edx,[ecx+420h]; mov ecx,[edx+34h]; call (the constant)
        this_->tile, this_);
    this_->vfptr->unexplore_terrain(this_, (RGE_Player*)this_->owner, 0, -1, 0);   // asm push 0/0FFFFFFFFh/0/owner; call [eax+164h] slot 89 (the dual-naming cast)
    RGE_Player* owner = (RGE_Player*)this_->owner;                        // asm mov ecx,[esi+18h] (the dual-naming cast)
    this_->tile = 0;                                                      // asm mov dword ptr [esi+28h],0
    this_->world_x = -1.0f;                                               // asm mov eax,-1.0; mov [esi+48h],eax (the 0xBF800000 bits)
    this_->world_y = -1.0f;                                               // asm mov [esi+4Ch],eax
    this_->world_z = -1.0f;                                               // asm mov [esi+50h],eax
    RGE_Player__unselect_one_object(owner, this_);                        // asm mov ecx,[esi+18h]; push esi; call (the constant)
    this_->vfptr->set_Selected_Groups(this_, 0);                          // asm mov edx,[esi]; push 0; mov ecx,esi; call [edx+364h] slot 217
}

// ---------------------------------------------------------------------------
// gbg_isMaster @0x0054B1D0 [0x0054B1D0,0x0054B27D): the jedi-master test.
// BAKED (the jedimaster.cpp:40 entry collision, the EXPL_DROID class - the
// txt-driven full replacement installed ONLY when the masters list loads):
// the patch body (group 50 Jedi + the is_id_in_list txt set) when
// masters_installed, the vanilla hardcoded membership otherwise. The patch
// call site retired in place. The vanilla id sets: registry candidates (the
// Jedi masters 50/51 family).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isMaster_new(
    RGE_Static_Object* this_)
{
    if (masters_installed)                                                // jedimaster.cpp:14-17 (the conditional bake)
    {
        if (this_->master_obj->object_group == 50)                        // 50 = Jedi (the registry); jedimaster.cpp:14
            return is_id_in_list(this_->master_obj->copy_id, masters_list, masters_n);
        return 0;
    }
    __int16 copy_id = this_->master_obj->copy_id;                         // asm mov ax,[ecx+1Ah] (@26)
    if (copy_id == 115 || copy_id == 125 || copy_id == 136 || copy_id == 134 || copy_id == 89 || copy_id == 140 || copy_id == 643 || copy_id == 645)   // asm cmp ax, imm16 x8 (RESTRUCTURE)
        return 1;
    __int16 id = this_->master_obj->id;                                   // asm mov ax,[ecx+18h] (@24)
    return id == 1077 || id == 1085 || id == 1082 || id == 1084 || id == 1080   // asm cmp ax, imm16 x16
        || id == 725 || id == 728 || id == 731 || id == 747 || id == 753
        || id == 761 || id == 770 || id == 778 || id == 784 || id == 806
        || id == 792;
}

// ---------------------------------------------------------------------------
// inRange @0x00551AC0 [0x00551AC0,0x00551B70): the two-object range test -
// the abs deltas, the per-side radius chains, the 0-clamps, then
// range^2 > the sum at 53-bit (the literal > value form - the setcc class,
// the unordered -> 0 identical to the asm's 41h jnz -> 0).
// PRECISION SCOPE: the whole chain (the mixed store-round model - the x
// delta STORED rounded (fstp m32), the y delta kept on st0 unrounded (the
// .c's float/double locals), the squares feeding the compare) -> the
// precise bracket.
#pragma float_control(precise, on, push)
int __fastcall RGE_Static_Object__inRange_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj, float range)
{
    float dx = obj->world_x - this_->world_x;                             // asm fld [edx+48h]; fsub [ecx+48h]; fstp [esp+obj] (the m32 store-round)
    double dy = (double)obj->world_y - (double)this_->world_y;            // asm fld [edx+4Ch]; fsub [ecx+4Ch] (the st0 kept UNROUNDED)
    double abs_dx = dx;                                                   // asm fld [esp+obj] (the rounded reload)
    if (dx < 0.0f)                                                        // asm fcom ds:const_f_zero; test ah,1; jz loc_551AE7 (the negate value-identical on NaN)
        abs_dx = -abs_dx;
    if (dy < 0.0)                                                         // asm fcom st(1); fxch/fchs/fxch (the same class)
        dy = -dy;
    RGE_Master_Static_Object* this_master = this_->master_obj;            // asm mov eax,[ecx+14h]
    RGE_Master_Static_Object* obj_master = obj->master_obj;               // asm mov ecx,[edx+14h]
    float x_dist = (float)((double)abs_dx - (double)this_master->radius_x // asm fld [eax+3Ch]; fsubr st,st(1); fsub [ecx+3Ch]; fstp [esp+obj]
        - (double)obj_master->radius_x);
    double y_dist = (double)dy - (double)this_master->radius_y            // asm fsub [eax+40h]; fsub [ecx+40h]
        - (double)obj_master->radius_y;
    if (X87_LT_NAN(x_dist, 0.0f))                                         // asm fcom; test ah,1; jz loc_551B2F (the {< 0, NaN} set - the macro form)
        x_dist = 0.0f;                                                    // asm mov [esp+obj],0 (the {< 0, NaN} set)
    if (X87_LE_NAN(y_dist, 0.0))                                          // asm fcom; test ah,41h; jz loc_551B44 (the {<=, NaN} zero)
        y_dist = 0.0;
    return (double)range * (double)range > y_dist * y_dist + (double)x_dist * (double)x_dist;   // asm fld st/fmul st,st(1); fld/fmul [esp+obj]; faddp; fld/fmul [esp+range]; fcompp (the squares at 53-bit - the float squares = the mulss desync class)
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// distance_to_position @0x00551C60 [0x00551C60,0x00551CFF): the position
// range - the same family, the single-object radii, the z param unreferenced
// (ABI kept), the double return (the st0 passthrough).
// PRECISION SCOPE: the whole chain at 53-bit -> the precise bracket.
#pragma float_control(precise, on, push)
double __fastcall RGE_Static_Object__distance_to_position_new(
    RGE_Static_Object* this_, DWORD dummy, float x, float y, float z)
{
    float dx = this_->world_x - x;                                        // asm fld [ecx+48h]; fsub [esp+x]; fstp [esp+x] (the store-round)
    double dy = (double)this_->world_y - (double)y;                       // asm fld [ecx+4Ch]; fsub [esp+y] (the st0 kept)
    double abs_dx = dx;                                                   // asm fld [esp+x]
    if (dx < 0.0f)                                                        // asm fcom; test ah,1; jz
        abs_dx = -abs_dx;
    if (dy < 0.0)                                                         // asm fcom; test ah,1; jz (the fxch/fchs/fxch class)
        dy = -dy;
    RGE_Master_Static_Object* master = this_->master_obj;                 // asm mov eax,[ecx+14h]
    float x_dist = (float)(abs_dx - (double)master->radius_x);            // asm fld [eax+3Ch]; fsubr st,st(1); fstp [esp+x]
    double y_dist = dy - (double)master->radius_y;                        // asm fstp st; fsub [eax+40h]
    if (X87_LT_NAN(x_dist, 0.0f))                                         // asm fcom; test ah,1; jz loc_551CC4 (the {< 0, NaN} set - the literal < would propagate NaN where the original clamps)
        x_dist = 0.0f;                                                    // asm mov [esp+x],0
    if (X87_LE_NAN(y_dist, 0.0))                                          // asm fcom; test ah,41h; jz loc_551CD9 (the {<=, NaN} zero)
        y_dist = 0.0;
    double result = sqrt(y_dist * y_dist + (double)x_dist * (double)x_dist);              // asm fld st/fmulp; fld/fmul [esp+x]; faddp; fsqrt (the squares at 53-bit - the float mulss square = the ~1-ULP desync)
    if (X87_LE_NAN(result, 0.0))                                          // asm fcom; test ah,41h; jz locret_551CFE (the {<=, NaN} zero)
        result = 0.0;                                                     // asm fstp st; fld ds:const_f_zero
    return result;                                                        // asm retn 0Ch (the st0 passthrough)
}
#pragma float_control(pop)

// ---------------------------------------------------------------------------
// distance_to_object2 @0x00551BA0 [0x00551BA0,0x00551C57): the two-object
// range - the distance_to_position twin with the obj param; the null ->
// +FLT_MAX (the fld ds:flt_656A10 - the exact m32 widening; the FLT_MAX
// form per the C2177 rule).
// PRECISION SCOPE: the whole chain at 53-bit -> the precise bracket.
#pragma float_control(precise, on, push)
double __fastcall RGE_Static_Object__distance_to_object2_new(
    RGE_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj, float x,
    float y)
{
    if (obj == 0)                                                         // asm mov edx,[esp+obj]; test; jnz loc_551BB1
        return FLT_MAX;                                                   // asm fld ds:flt_656A10 (+FLT_MAX - the binary-verified 7F7FFFFF)
    float dx = obj->world_x - this_->world_x;                             // asm fld [edx+48h]; fsub [ecx+48h]; fstp [esp+obj]
    double dy = (double)obj->world_y - (double)this_->world_y;            // asm fld [edx+4Ch]; fsub [ecx+4Ch]
    double abs_dx = dx;                                                   // asm fld [esp+obj]
    if (dx < 0.0f)                                                        // asm fcom; test ah,1; jz
        abs_dx = -abs_dx;
    if (dy < 0.0)                                                         // asm fcom; test ah,1; jz
        dy = -dy;
    RGE_Master_Static_Object* this_master = this_->master_obj;            // asm mov eax,[ecx+14h]
    RGE_Master_Static_Object* obj_master = obj->master_obj;               // asm mov ecx,[edx+14h]
    float x_dist = (float)((double)abs_dx - (double)this_master->radius_x // asm fld [eax+3Ch]; fsubr st,st(1); fsub [ecx+3Ch]; fstp [esp+obj]
        - (double)obj_master->radius_x);
    double y_dist = (double)dy - (double)this_master->radius_y            // asm fsub [eax+40h]; fsub [ecx+40h]
        - (double)obj_master->radius_y;
    if (X87_LT_NAN(x_dist, 0.0f))                                         // asm fcom; test ah,1; jz loc_551C1C (the {< 0, NaN} set - the macro form)
        x_dist = 0.0f;
    if (X87_LE_NAN(y_dist, 0.0))                                          // asm fcom; test ah,41h; jz
        y_dist = 0.0;
    double result = sqrt((double)y_dist * (double)y_dist + (double)x_dist * (double)x_dist);   // asm fld st/fmulp; fld/fmul; faddp; fsqrt (the squares at 53-bit)
    if (X87_LE_NAN(result, 0.0))                                          // asm fcom; test ah,41h; jz
        result = 0.0;
    return result;                                                        // asm retn 0Ch
}
#pragma float_control(pop)

void install_RGE_Static_Object(){
    setHook((void*)0x0054B3E0, RGE_Static_Object__gbg_get_civ_override_new);
    setHook((void*)0x0054B2E0, RGE_Static_Object__RGE_Static_Object_new);
    setHook((void*)0x0054B600, RGE_Static_Object__RGE_Static_Object_read_new);
    setHook((void*)0x0054B680, RGE_Static_Object__destructor_new);
    setHook((void*)0x0054C0A0, RGE_Static_Object__recycle_out_of_game_new);
    setHook((void*)0x0054ED30, RGE_Static_Object__gbg_doShieldRecharge_new);
    setHook((void*)0x0054EE70, RGE_Static_Object__gbg_doShieldDropoff_new);
    setHook((void*)0x0054EF00, RGE_Static_Object__update_new);
    // BATCH S1 (2026-11-26): the micro-method tier
    setHook((void*)0x00422A40, RGE_Static_Object__do_command_new);
    setHook((void*)0x00422A60, RGE_Static_Object__move_to_new);
    setHook((void*)0x0040C840, RGE_Static_Object__unitAI_new);
    setHook((void*)0x00422A80, RGE_Static_Object__get_command_master_new);
    setHook((void*)0x0054BE40, RGE_Static_Object__do_i_care_about_power_new);
    setHook((void*)0x00408A90, RGE_Static_Object__calc_attack_modifier_new);
    setHook((void*)0x00422AD0, RGE_Static_Object__calculateYawAngle_new);
    setHook((void*)0x005524F0, RGE_Static_Object__underAttack_new);
    setHook((void*)0x00552E10, RGE_Static_Object__searchRadius_new);
    setHook((void*)0x0054B8A0, RGE_Static_Object__gbg_skip_unit_ai_update_new);
    setHook((void*)0x00552930, RGE_Static_Object__removeAllFromPathingGroup_new);
    setHook((void*)0x00552500, RGE_Static_Object__setUnderAttack_new);
    setHook((void*)0x00552EA0, RGE_Static_Object__garrisoned_count_new);
    setHook((void*)0x0054B450, RGE_Static_Object__gbg_isAirCruiser_new);
    setHook((void*)0x00422A50, RGE_Static_Object__actionTypeIfOrdered_new);
    setHook((void*)0x00422B30, RGE_Static_Object__orientationForward_new);
    setHook((void*)0x00422B10, RGE_Static_Object__straightProjectedPosition_new);
    setHook((void*)0x0054B5B0, RGE_Static_Object__gbg_isPredatorUnit_new);
    setHook((void*)0x00550AD0, RGE_Static_Object__is_dying_new);
    setHook((void*)0x00551990, RGE_Static_Object__set_attribute_new);
    setHook((void*)0x0054FF00, RGE_Static_Object__remove_overlay_sprite_new);
    setHook((void*)0x00551500, RGE_Static_Object__exit_obj_new);
    setHook((void*)0x00551B80, RGE_Static_Object__distance_to_object_new);
    setHook((void*)0x00551A60, RGE_Static_Object__set_being_worked_on_new);
    setHook((void*)0x00552EB0, RGE_Static_Object__get_group_number_mask_new);
    setHook((void*)0x0054F500, RGE_Static_Object__get_object_pointer_new);
    setHook((void*)0x00551FF0, RGE_Static_Object__set_location_new);
    setHook((void*)0x00552C40, RGE_Static_Object__position_new);
    setHook((void*)0x00551560, RGE_Static_Object__remove_obj_new);
    setHook((void*)0x0054B5C0, RGE_Static_Object__gbg_isStaticPredatorUnit_new);
    setHook((void*)0x0054B480, RGE_Static_Object__gbg_isSpaceport_new);
    setHook((void*)0x0054B460, RGE_Static_Object__gbg_isGEAssaultMech_new);
    setHook((void*)0x005513A0, RGE_Static_Object__get_animation_frame_new);
    setHook((void*)0x0054B5E0, RGE_Static_Object__vector_deleting_destructor_new);
    setHook((void*)0x00551380, RGE_Static_Object__set_animation_frame_new);
    setHook((void*)0x0054B870, RGE_Static_Object__gbg_isDamaged_new);
    setHook((void*)0x0054FED0, RGE_Static_Object__add_overlay_sprite_new);
    setHook((void*)0x0054B4E0, RGE_Static_Object__gbg_isMilitaryUnit_new);
    setHook((void*)0x0054B400, RGE_Static_Object__gbg_isAircraft_new);
    setHook((void*)0x0054B280, RGE_Static_Object__gbg_isAttackGroundButtonAvailable_new);
    setHook((void*)0x00550700, RGE_Static_Object__die_die_die_new);
    setHook((void*)0x0054B550, RGE_Static_Object__gbg_isCivilianUnit_new);
    setHook((void*)0x00408A50, RGE_Static_Object__get_visibility_new);
    setHook((void*)0x00551580, RGE_Static_Object__get_los_table_new);
    setHook((void*)0x0054F520, RGE_Static_Object__get_sprite_pointer_new);
    // BATCH S2a (2026-11-26): the mid-tier methods (48-77B; rehook added after
    // the encoding-review approval) - re-enabled after the set_sleep_flag
    // bisect closed (the dropped [esi+59h]=0 store restored 2026-11-26)
    setHook((void*)0x0054B4A0, RGE_Static_Object__gbg_isResourceBuilding_new);
    setHook((void*)0x005518F0, RGE_Static_Object__calculateDamage_new);
    setHook((void*)0x00552850, RGE_Static_Object__inLockedFormation_new);
    setHook((void*)0x00551520, RGE_Static_Object__add_obj_new);
    setHook((void*)0x00551A80, RGE_Static_Object__release_being_worked_on_new);
    setHook((void*)0x00552A10, RGE_Static_Object__set_facet_new);
    setHook((void*)0x0054B8B0, RGE_Static_Object__can_be_healed_new);
    setHook((void*)0x0054B900, RGE_Static_Object__can_be_repaired_new);
    setHook((void*)0x0054CE20, RGE_Static_Object__capture_frame_new);
    setHook((void*)0x0054CDD0, RGE_Static_Object__draw_back_frame_new);
    setHook((void*)0x005506B0, RGE_Static_Object__destroy_obj_new);
    setHook((void*)0x005519B0, RGE_Static_Object__set_attribute_amount_new);
    setHook((void*)0x005513C0, RGE_Static_Object__more_room_new);
    setHook((void*)0x0054F4B0, RGE_Static_Object__rehook_new);
    // BATCH S2b (2026-11-26): the mid-tier methods (88-105B)
    setHook((void*)0x00551A00, RGE_Static_Object__heal_new);
    setHook((void*)0x0054C850, RGE_Static_Object__create_object_list_new);
    setHook((void*)0x0054C980, RGE_Static_Object__take_attribute_from_owner_new);
    setHook((void*)0x00551930, RGE_Static_Object__rotate_new);
    setHook((void*)0x0054C8B0, RGE_Static_Object__create_sprite_list_new);
    setHook((void*)0x00552940, RGE_Static_Object__remove_shadows_new);
    setHook((void*)0x0054B170, RGE_Static_Object__gbg_isPadawan_new);
    setHook((void*)0x00551310, RGE_Static_Object__get_frame_new);
    setHook((void*)0x0054C910, RGE_Static_Object__give_attribute_to_owner_new);
    setHook((void*)0x0054CD50, RGE_Static_Object__draw_front_frame_new);
    setHook((void*)0x0054B370, RGE_Static_Object__play_unload_sound_new);
    setHook((void*)0x0054C7E0, RGE_Static_Object__change_unique_id_new);
    // BATCH S2c (2026-11-26): the mid tier (108-134B, part 1)
    setHook((void*)0x005529A0, RGE_Static_Object__set_sleep_flag_new);
    setHook((void*)0x0054F440, RGE_Static_Object__spawn_death_obj_new);
    setHook((void*)0x0054FD20, RGE_Static_Object__handle_teleport_z_correction_new);
    setHook((void*)0x0054B950, RGE_Static_Object__gbg_unknown23_new);
    setHook((void*)0x0054B9E0, RGE_Static_Object__gbg_isBiological_new);
    setHook((void*)0x0054BBC0, RGE_Static_Object__gbg_isDetector_new);
    setHook((void*)0x00552E20, RGE_Static_Object__isFlagSet_new);
    setHook((void*)0x00550320, RGE_Static_Object__modify_percent_new);
    setHook((void*)0x00552610, RGE_Static_Object__lookupZone_new);
    setHook((void*)0x005526A0, RGE_Static_Object__currentZone_new);
    setHook((void*)0x00552730, RGE_Static_Object__withinRangeOfZone_new);
    setHook((void*)0x005527C0, RGE_Static_Object__findClosestPointInTerrainType_new);
    // BATCH S2c-2 (2026-11-26): the mid tier (141-182B)
    setHook((void*)0x00550290, RGE_Static_Object__modify_delta_new);
    setHook((void*)0x0054CBD0, RGE_Static_Object__shadow_draw_new);
    setHook((void*)0x00552890, RGE_Static_Object__addToPathingGroup_new);
    setHook((void*)0x0054BDA0, RGE_Static_Object__is_powered_new);
    setHook((void*)0x00551C60, RGE_Static_Object__distance_to_position_new);
    setHook((void*)0x0054F770, RGE_Static_Object__teleport_off_map_new);
    setHook((void*)0x0054B1D0, RGE_Static_Object__gbg_isMaster_new);
    setHook((void*)0x00551AC0, RGE_Static_Object__inRange_new);
    setHook((void*)0x00551BA0, RGE_Static_Object__distance_to_object2_new);
}
