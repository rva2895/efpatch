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


void install_RGE_Static_Object()
{
    setHook((void*)0x0054B3E0, RGE_Static_Object__gbg_get_civ_override_new);
    setHook((void*)0x0054B2E0, RGE_Static_Object__RGE_Static_Object_new);
    setHook((void*)0x0054B600, RGE_Static_Object__RGE_Static_Object_read_new);
    setHook((void*)0x0054B680, RGE_Static_Object__destructor_new);
    setHook((void*)0x0054C0A0, RGE_Static_Object__recycle_out_of_game_new);
    setHook((void*)0x0054ED30, RGE_Static_Object__gbg_doShieldRecharge_new);
    setHook((void*)0x0054EE70, RGE_Static_Object__gbg_doShieldDropoff_new);
    setHook((void*)0x0054EF00, RGE_Static_Object__update_new);
}
