#include "stdafx.h"

// BAKED-PATCH helpers (the casts.cpp / los.cpp storage schemes this
// cluster must reproduce):
// casts.cpp:763 (__fastcall, naked) — frees the patch's UNIT_EXTRA
// (allocated with the patch's own malloc, freed with the patch's own
// free inside this helper) and clears the pointer storage.
extern void __fastcall removeUnitExtra(RGE_Static_Object* unit);
// los.cpp:6 (__stdcall) — frees the patch's LOS data chain. (The
// pointer lives in ei.theObj — an idb-typed RGE_Static_Object* slot
// the patch repurposes for its UNIT_LOS_DATA*.)
struct UNIT_LOS_DATA;
extern void __stdcall delete_unit_los_data(UNIT_LOS_DATA* los_data);

// .rdata vftable of RGE_Static_Object (.rdata:00662C8C, lst :994791)
static RGE_Static_ObjectVtbl* const rge_static_object_vftable =
    (RGE_Static_ObjectVtbl*)0x00662C8C;

// Replacement for RGE_Static_Object::gbg_get_civ_override
// Original: RGE_Static_Object__gbg_get_civ_override @ 0x0054B3E0 (idb_functions.h:3776)
// ABI: __thiscall — this_ in ECX, no stack arguments, int in EAX.
// No EDX dummy needed (zero-arg method; precedent: land.cpp:4).
// BAKE-IN (langdll.cpp:306 @ 0x0054B3E9): original compares against 9;
// the patched immediate value is CIV_COUNT + 1 (consts.h:3 = 15 → 16).
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
// then the zero block (gbg_unknown_3/sprite/old_sprite/CurrentObstruction
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
    this_->gbg_unknown_3 = 0;
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
// stream) — Original: @ 0x0054B600 (idb_functions.h:3787). Range
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
    this_->gbg_unknown_3 = 0;
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

void install_RGE_Static_Object()
{
    setHook((void*)0x0054B3E0, RGE_Static_Object__gbg_get_civ_override_new);
    setHook((void*)0x0054B2E0, RGE_Static_Object__RGE_Static_Object_new);
    setHook((void*)0x0054B600, RGE_Static_Object__RGE_Static_Object_read_new);
    setHook((void*)0x0054B680, RGE_Static_Object__destructor_new);
    setHook((void*)0x0054C0A0, RGE_Static_Object__recycle_out_of_game_new);
}
