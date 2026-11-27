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

// casts.cpp:146 (__stdcall) - the UNIT_EXTRA stealth-off toggle (specialDamage
// types 43/44); BAKED into is_invisible_new (batch S4, the stealth1 fold)
extern bool __stdcall getStealthOff(RGE_Static_Object* unit);
// los.cpp:205 (__stdcall) - the LOS-depth mod's explore/unexplore position
// memory (the ei.theObj slot repurposed as the UNIT_LOS_DATA* chain head);
// BAKED into explore_terrain_new / unexplore_terrain_new (batch S4)
extern void __stdcall explore_log(bool explore, bool sq, int* tx1, int* ty1,
    int* tx2, int* ty2, int* radius, int* Square, UNIT_LOS_DATA** los_data);
// the DAT version (the aitimer/cargotrader extern pattern); consumed by the
// ADOPTED shield.cpp version-gated block inside is_shielded_new (batch S4)
extern int current_loaded_version;
// los.cpp:122 (__stdcall) - the LOS-depth mod's save extension (the ei.theObj
// chain flag + x/y/radius); BAKED into save_new (batch S6; the los.cpp:1034/
// 1035 pair retired)
extern void __stdcall los_write_save2(UNIT_LOS_DATA* los_data, int outfile);
// los.cpp:81 (__stdcall) - the option byte gate (owner+0x21 >= 0xD)
extern bool __stdcall is_los_save_option_enabled(RGE_Static_Object* unit);

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

// ===================== BATCH S3 (2026-11-27): the big-body tier opens - the smallest eight (77-249B, all single-pass) =====================

// SEMANTIC: the object-sprite display test. Returns 1 when this unit's sprite is drawable by player a2:
// the local player always sees its own units; the prog_mode-7 (in-game) path short-circuits everything
// except the invisible-master gate; otherwise the answer is NOT is_invisible(a2).
unsigned __int8 __fastcall RGE_Static_Object__gbg_isObjectSpriteDisplayed_new(RGE_Static_Object *this_, DWORD dummy, int a2)
{
    int prog_mode = (*base_game)->prog_mode;                              // asm 0x54BA60-0x54BA6C: mov eax,base_game / mov eax,[eax+1DCh]
    if (prog_mode != 7 && (this_->master_obj->object_flags & 0x80) != 0)
        return 0;                                                         // asm 0x54BA71-0x54BA7F: mov edx,[ecx+14h] master_obj @20; test byte [edx+0ACh],80h
                                                                          // (master object_flags @172, bit 7 = INVISIBLE UNIT - the object_flags registry); xor al,al
    if (a2 == this_->owner->id || prog_mode == 7)
        return 1;                                                         // asm 0x54BA83-0x54BA99: mov esi,[ecx+18h] owner @24; mov ebx,[esi+0A0h] id @160;
                                                                          // cmp edx,ebx / cmp eax,7; loc_54BAAA mov al,1
    return this_->vfptr->is_invisible(this_, a2) == 0;                    // asm 0x54BA9B-0x54BA9E: mov eax,[ecx] / push edx / call [eax+48h] -
                                                                          // the virtual dispatch slot 18 = +0x48 (idb_types.h:3978);
                                                                          // neg al / sbb eax,eax / inc eax @0x54BAA1-0x54BAA6 = the (x == 0) 1/0 form
}

// SEMANTIC: the standard unit renderer. Guards on a live tile and object_state < 7; draws the back
// (selection) frame when the unit is selected or the global outline mode is 3; then the sprite via the
// Active_Sprite_List when one exists (facet + screen-offset-adjusted position) or the bare RGE_Sprite
// otherwise; the front (selection) frame last when the back frame ran.
void __fastcall RGE_Static_Object__normal_draw_new(RGE_Static_Object *this_, DWORD dummy, TDrawArea *render_area, __int16 x, __int16 y)
{
    if (this_->tile == 0 || this_->object_state >= 7)
        return;                                                           // asm 0x54CC76-0x54CC85: mov eax,[esi+28h] tile @40; test; cmp byte [esi+58h],7 jnb
    int draw_back;                                                        // the original reuses its render_area arg slot as the flag (the frame label draw_area = 4)
                                                                          // - dedicated local per the param-slot-reuse precedent
    if (this_->selected != 0 || (*base_game)->outline_type == 3)          // asm 0x54CC8B-0x54CCA7: mov al,[esi+46h] selected @70; test al; cmp byte [eax+70h],3
                                                                          // ((*base_game)->outline_type @+0x70, u8)
    {
        draw_back = 1;                                                    // asm 0x54CCB7: mov byte [esp+1Ch+draw_area],1
        this_->vfptr->draw_back_frame(this_, render_area, x, y);          // asm 0x54CCB0-0x54CCBC: the virtual dispatch slot 35 = +0x8C (idb_types.h:3995)
    }
    else
    {
        draw_back = 0;                                                    // asm 0x54CCA9
    }
    if (this_->sprite_list != 0)                                          // asm 0x54CCC2-0x54CCC7: mov ecx,[esi+24h] sprite_list @36
    {
        this_->sprite_list->vfptr->normal_draw(this_->sprite_list, this_->facet,
            (__int16)(this_->screen_x_offset + x),                        // asm 0x54CCE0-0x54CCE4: mov dx,[esi+34h] @52 / add dx,bp - the 16-bit add,
                                                                          // wrap-identical to the i16 parameter conversion
            (__int16)(this_->screen_y_offset + y),                        // asm 0x54CCD6-0x54CCDA: mov dx,[esi+36h] @54 / add dx,[esp+18h+y]
            this_->owner->color_table, render_area);                      // asm 0x54CCCD/0x54CCCF: [edx+164h] owner->color_table @+0x164 (the TRIBE_Player walk);
                                                                          // the virtual dispatch slot 4 = +0x10 (idb_types.h:7573)
    }
    else if (this_->sprite != 0)                                          // asm 0x54CCF7-0x54CCFC: mov ecx,[esi+1Ch] sprite @28
    {
        RGE_Sprite__normal_draw(this_->sprite, this_->facet, 0,           // asm 0x54CCFE-0x54CD25: the DIRECT call (:3760 @0x54A740); facet via xor eax,eax + mov al,[esi+45h] @69
            this_->screen_x_offset + x,                                   // asm 0x54CD13-0x54CD1A: the movsx pair + the 32-bit add (the int params sign-extend)
            this_->screen_y_offset + y,
            this_->owner->color_table, render_area);
    }
    if (draw_back)
        this_->vfptr->draw_front_frame(this_, render_area, x, y);         // asm 0x54CD2A-0x54CD39: the virtual dispatch slot 34 = +0x88 (idb_types.h:3994)
}

#pragma float_control(precise, on, push)   // rule (k): the flag-0 division feeds __ftol (0x55026C) - pin the division as a division

// SEMANTIC: the runtime modify dispatcher. flag 0 = set this unit's hp to amount percent of the master's
// max hp (computed at x87 53-bit, truncated ftol, narrowed to i16, stored back as float); flag 1 = the LOS
// refresh around a live active unit (unexplore the old ring, explore with (int)amount as the new radius);
// flag 21 = the attribute resync against the owner when the master's first attribute slot is mode 2
// (bank the currently-held amount as a negative delta, then add the new amount).
void __fastcall RGE_Static_Object__modify_new(RGE_Static_Object *this_, DWORD dummy, float amount, unsigned __int8 flag)
{
    // the membership chain, decoded by the cumulative arithmetic at each site (the new template rule):
    // sub eax,0 / dec eax / sub eax,14h = the cumulative values {0}, {1}, {21} (asm 0x5501AB-0x5501BC)
    if (flag == 0)                                                        // asm 0x5501AB-0x5501B0: sub eax,0; jz loc_550256
    {
        this_->hp = (float)(short)((double)amount * (double)this_->hp / this_->master_obj->hp);
                                                                          // asm 0x550256-0x55027C: fld [esp+8+amount] / fmul [esi+3Ch] (hp @60, the product
                                                                          // at 53-bit) / movsx ecx, word [eax+32h] (master->hp i16 @50) / fidiv (the int
                                                                          // converts to double exactly) / call __ftol / movsx ax / fild / fstp [esi+3Ch];
                                                                          // the (short) cast = the ftol + i16 narrow, then the exact float store
    }
    else if (flag == 1)                                                   // asm 0x5501B6-0x5501B7: dec eax; jz loc_550214
    {
        if (this_->tile != 0 && this_->object_state == 2)                 // asm 0x550214-0x55021F: [esi+28h]; cmp byte [esi+58h],2 (ACTIVE)
        {
            this_->vfptr->unexplore_terrain(this_, (RGE_Player *)this_->owner, 0, -1, 0);
                                                                          // asm 0x550221-0x55022F: the virtual dispatch slot 89 = +0x164 (idb_types.h:4049);
                                                                          // pushes (owner, 0, -1, 0) matching the member (RGE_Player*, u8, int, int)
            this_->vfptr->explore_terrain(this_, (RGE_Player *)this_->owner, 0, (int)amount, 0);
                                                                          // asm 0x550235-0x55024B: the virtual dispatch slot 88 = +0x160 (idb_types.h:4048);
                                                                          // the 4th arg = __ftol(amount) @0x550239-0x55023D (a direct float load - the
                                                                          // pseudocode's (__int64)amount render was the ftol artifact)
        }
    }
    else if (flag == 21)                                                  // asm 0x5501B9-0x5501BC: sub eax,14h; jnz exit
    {
        if (this_->master_obj->attribute_flag[0] == 2)                    // asm 0x5501C2-0x5501CC: byte [eax+98h] @152
        {
            if (this_->master_obj->attribute_type_held[0] >= 0)           // asm 0x5501D2-0x5501D9: mov dx,[eax+7Ah] (i16 @122); test dx,dx; jl skips
            {
                float banked = -this_->master_obj->attribute_amount_held[0];
                                                                          // asm 0x5501E0-0x5501EB: fld [eax+80h] (float @128) / fchs / fstp var_10 - the exact negation
                this_->owner->vfptr->add_attribute_num(this_->owner, this_->master_obj->attribute_type_held[0], banked, 0);
                                                                          // asm 0x5501DB-0x5501EF: the virtual dispatch slot 32 = +0x80 (TRIBE_PlayerVtbl:1134);
                                                                          // the push 0 / push ecx (slot reserved, overwritten by the fstp) / push edx
                                                                          // pattern = args (type, banked, 0)
            }
            this_->owner->vfptr->add_attribute_num(this_->owner, this_->master_obj->attribute_type_held[0], amount, 0);
                                                                          // asm 0x5501F5-0x550209: the same slot; the amount parameter's raw dword passed
                                                                          // through (mov edx,[esp+8+amount] / push edx) = the bit-preserving float pass
        }
    }
}
#pragma float_control(pop)

// SEMANTIC: the death-doppleganger handshake. When the master is flagged create_doppleganger_on_death
// == 2, every non-gaia player whose visibility bit sees this tile (the tile's Unified-Map-Value dword:
// high word = the seen bits, low word = the explored bits; a viewer fires when its seen bit is set and
// its explored bit is clear) gets a doppleganger of this unit created from the fixed doppleganger master
// slot 243; then this unit registers itself with its owner's Doppleganger_Creator so the dopple tracks it.
void __fastcall RGE_Static_Object__create_doppleganger_when_dying_new(RGE_Static_Object *this_)
{
    if (this_->id < 0)                                                    // asm 0x550884-0x550889: mov eax,[esi+10h] id @16; test/jl signed
        return;
    if (this_->owner == 0 || this_->master_obj == 0)                      // asm 0x55088F-0x55089F: [esi+18h] / [esi+14h]
        return;
    if (this_->master_obj->create_doppleganger_on_death != 2)             // asm 0x5508A5-0x5508AC: cmp byte [eax+0BEh],2 (@190)
        return;
    unsigned int umv = (unsigned int)unified_map_offsets[(int)this_->world_y][(int)this_->world_x];
                                                                          // asm 0x5508B2-0x5508CC: fld [esi+4Ch] world_y / __ftol (the row), fld [esi+48h]
                                                                          // world_x / __ftol (the col), unified_map_offsets[edi*4] + [ecx+eax*4].
                                                                          // MAP_SIZE (mapsize.cpp:1765 writeDword 0x5508C8 = this instruction's row-table
                                                                          // disp32 - never baked per the MAP_SIZE rule)
    unsigned int lo = umv & 0xFFFF;                                       // asm 0x5508D1: and eax,0FFFFh (the saved copy var_4)
    unsigned int hi = umv >> 16;                                          // asm 0x5508D6: shr edi,10h
    if (hi != 0)                                                          // asm 0x5508D9-0x5508DF: test edi,edi; jbe skips the loop
    {
        int idx = 0;
        for (unsigned int bits = hi; bits != 0; idx++)                    // the asm shifts BOTH words each pass and continues while the SHIFTED hi != 0
                                                                          // (0x550926-0x550931: shr eax / shr edi; jnz - the intervening mov clears no flags)
        {
            if ((bits & 1) != 0 && (lo & 1) == 0 && idx > 0)              // asm 0x5508E5-0x5508F5: the hi bit set / the lo bit clear / ebp (the player byte offset) > 0
            {
                TRIBE_Player *viewer = this_->owner->world->players[idx];
                                                                          // asm 0x5508FA-0x550910: [ecx+8Ch] world @140; [edx+4Ch] players @+0x4C (the TRIBE_World walk);
                                                                          // [edx+ebp] (the dword-stride element)
                RGE_Master_Doppleganger_Object *dopple_master =
                    (RGE_Master_Doppleganger_Object *)viewer->master_objects[243];
                                                                          // asm 0x550914-0x550917: [eax+74h] master_objects; [ecx+3CCh] = element 243 (0x3CC/4).
                                                                          // The base->derived cast: slot 243 holds the doppleganger template master by
                                                                          // construction - its vtbl slot 30 IS make_new_obj_2 (the only tier declaring the
                                                                          // slot: RGE_Master_Doppleganger_ObjectVtbl:21953; the Animated master vtbl ends
                                                                          // at slot 29, idb_types.h:15988) - the M4 derived-master-cast precedent
                dopple_master->vfptr->make_new_obj_2(dopple_master, (RGE_Player *)viewer,
                    this_->world_x, this_->world_y, this_->world_z, this_);
                                                                          // asm 0x55091D-0x55091F: mov edx,[ecx] / call [edx+78h] - the virtual dispatch
                                                                          // slot 30 = +0x78; the pushes (player, x, y, z, this) match the member decl
                                                                          // (RGE_Player*, float, float, float, RGE_Static_Object*); the return discarded
                                                                          // (eax reloaded from var_4 @0x550922)
            }
            lo >>= 1;                                                     // asm 0x550929
            bits >>= 1;                                                   // asm 0x55092B (the loop-continue flag source)
        }
    }
    unsigned int *umv_addr = (unsigned int *)&unified_map_offsets[(int)this_->world_y][(int)this_->world_x];
                                                                          // asm 0x550935-0x55094E: the SAME ftol pair; lea edx,[ecx+eax*4]. // MAP_SIZE
                                                                          // (mapsize.cpp:1766 writeDword 0x55094A - never baked)
    RGE_Doppleganger_Creator__add_doppleganger_check(this_->owner->doppleganger_creator, this_, umv_addr);
                                                                          // asm 0x550951-0x55295C: [eax+90h] doppleganger_creator @144; the DIRECT call
                                                                          // (:1492 @0x470770), the int return discarded
}

// SEMANTIC: boarding a container. Unless forced, the target must report more_room; this unit exits any
// current container, unexplores its ring and unlinks from the map tile; deregisters from the land/air
// obstruction manager (and deletes its obstruction list); leaves its pathing group (with the stop);
// then the target takes it (add_obj) and the selection clears if it was selected.
void __fastcall RGE_Static_Object__enter_obj_new(RGE_Static_Object *this_, DWORD dummy, RGE_Static_Object *target, unsigned __int8 force_entry)
{
    if (force_entry == 0 && target->vfptr->more_room(target, 0) == 0)
        return;                                                           // asm 0x551410-0x55142F: test al (force); jnz skips; the virtual dispatch slot 83 =
                                                                          // +0x14C (idb_types.h:4043) with arg 0; test al,al; jz exits
    if (this_->inside_obj != 0)                                           // asm 0x551435-0x55143A: [esi+2Ch] inside_obj @44
        this_->inside_obj->vfptr->exit_obj(this_->inside_obj);            // asm 0x55143C-0x55143E: the virtual dispatch slot 85 = +0x154
    if (this_->tile != 0)                                                 // asm 0x551444-0x551449: [esi+28h] tile @40
    {
        this_->vfptr->unexplore_terrain(this_, (RGE_Player *)this_->owner, 0, -1, 0);
                                                                          // asm 0x55144B-0x551459: the virtual dispatch slot 89 = +0x164 (the same
                                                                          // (owner, 0, -1, 0) shape as modify's flag-1 arm)
        RGE_Map__OL_DeleteObjectListEntry((*base_game)->world->map, this_->tile, this_);
                                                                          // asm 0x55145F-0x551472: (*base_game)->world @+0x420 / world->map @+0x34; the DIRECT
                                                                          // call (:2006 @0x496A60), args (theTile, theObj)
        this_->tile = 0;                                                  // asm 0x551477: mov dword [esi+28h],0
    }
    RGE_Obstruction_Manager *ob = *OBSystem_Land;                         // asm 0x551482: mov edi,OBSystem_Land - the manager-pointer memory load (the slot deref)
    if (this_->master_obj->movement_type == 1)                            // asm 0x551488: cmp byte [edx+78h],1 (movement_type @120)
        ob = (RGE_Obstruction_Manager *)(*OBSystem_Air);                  // asm 0x55148E; the sideways cast Airspace->Obstruction - the layout-proven twin
                                                                          // (member-identical structs, slot-identical vtbls - the passableTile decode)
    ob->vfptr->RemoveObjectObstruction(ob, this_);                        // asm 0x551494-0x551499: the virtual dispatch slot 11 = +0x2C
    if (this_->ObstructionDataList != 0)                                  // asm 0x55149C-0x5514A1: [esi+64h] @100
        ob->vfptr->DeleteObList(ob, this_);                               // asm 0x5514A3-0x5514A8: the virtual dispatch slot 16 = +0x40
    if (this_->mGroupID != -1)                                            // asm 0x5514AB-0x5514B5: [esi+88h] mGroupID @136 (the A4 grounding)
    {
        BUnitGroup *group = RGE_Game_World__getUnitGroup((RGE_Game_World *)this_->owner->world, this_->mGroupID);
                                                                          // asm 0x5514B7-0x5514C1: the DIRECT call (:6099 @0x624920); the TRIBE_World family
                                                                          // cast (the A4 precedent)
        if (group != 0)                                                   // asm 0x5514C6-0x5514C8
            BUnitGroup__removeUnit(group, this_->id);                     // asm 0x5514CA-0x5514D0: the DIRECT call (:1699 @0x47F5C0); unit id @+0x10
        this_->vfptr->stopAllMovement(this_);                             // asm 0x5514D5-0x5514D9: the virtual dispatch slot 179 = +0x2CC (idb_types.h:4139)
    }
    target->vfptr->add_obj(target, this_);                                // asm 0x5514DF-0x5514E5: the virtual dispatch slot 86 = +0x158 (idb_types.h:4046)
    if ((this_->selected & 1) != 0)                                       // asm 0x5514EB: test byte [esi+46h],1
        RGE_Player__unselect_one_object((RGE_Player *)this_->owner, this_);
                                                                          // asm 0x5514F1-0x5514F5: the DIRECT call (:2619 @0x4C30A0); the dual-naming cast
}

// SEMANTIC: the damage application. calculateDamage (the virtual slot 59) computes the raw damage double
// on the x87 stack; shields absorb first (sp) with the shield-hit flash timer set on every shield-engaged
// hit and never on the plain-hp path; hp under 1.0 means death (hp = 0, wake the unit); a sounding master
// plays its damage sound with the 3D pan/volume the sound-info probe fills - with the RNG state saved and
// restored around the probe so the sound path never consumes a draw.
void __fastcall RGE_Static_Object__damage_new(RGE_Static_Object *this_, DWORD dummy, int weapon_num,
    RGE_Armor_Weapon_Info *damage, float attack_modifier, RGE_Player *attacking_player,
    RGE_Static_Object *attackingObject, int first_call)
{
    // first_call is unconsumed: the calculateDamage dispatch forwards 5 of the 6 stack args and this
    // function's retn 18h pops all 6 (asm 0x5517F0-0x551811; nothing reads the +18h slot in the range)
    double dmg = this_->vfptr->calculateDamage(this_, weapon_num, damage, attack_modifier,
        attacking_player, attackingObject);
                                                                          // asm 0x5517FC-0x551811: the virtual dispatch slot 59 = +0xEC (idb_types.h:4019,
                                                                          // the double-typed slot); the return rides st0 unrounded into every consumer below
    if (X87_LE_NAN(this_->sp, 0.0f) || X87_LE_NAN(dmg, 0.0f))
    {                                                                     // asm 0x551817-0x551834: fld [esi+40h] sp @64 / fcomp const_f_zero / test ah,41h; jnz x2 -
                                                                          // the two {<=0, NaN} gates short-circuit to the plain-hp arm
        this_->hp = (float)((double)this_->hp - dmg);                     // asm 0x55186F-0x551877: fld hp / fsub st,st(1) / fstp - the 53-bit difference, one m32 store
    }
    else if (dmg > this_->sp)
    {                                                                     // asm 0x551836-0x55183E: fcom sp / test ah,41h; jz -> the {>} branch - the jz targets the
                                                                          // positive branch of an ordered-only set: the literal > MATCH row (a NaN dmg falls to
                                                                          // the else, exactly the asm's fall-through); the operands widen = comisd
        double spill = dmg - (double)this_->sp;                           // asm 0x551855: fsub [esi+40h] - the 53-bit remainder
        this_->sp = 0.0f;                                                 // asm 0x55185B: mov [esi+40h],edi (edi = 0)
        this_->hp = (float)((double)this_->hp - spill);                   // asm 0x551858-0x551860: fld hp / fsub st,st(1) / fstp - the remainder consumed at 53-bit
        this_->shield_hit_timer = 1.5f;                                   // asm 0x551863-0x55186A: mov eax,ds:dword_662C88 / mov [esi+0Ch] - the CONFIRMED
                                                                          // 1.5f flash value (the game-facts shield row); never on the plain-hp arm
    }
    else
    {
        this_->sp = (float)((double)this_->sp - dmg);                     // asm 0x551840-0x551845: fld sp / fsub st,st(1) (dmg) / fstp - a NaN dmg lands here, asm-exact
        this_->shield_hit_timer = 1.5f;                                   // asm 0x551848-0x551850
    }
    if (X87_LT_NAN(this_->hp, 1.0f))
    {                                                                     // asm 0x551879-0x551887: fld hp / fcomp const_f_one / test ah,1 (the C0-ONLY mask) / jz skips -
                                                                          // the fall-through taken set is {< 1.0, NaN} = the LT_NAN macro (the
                                                                          // test-mask-immediate class: 01h, not 41h)
        this_->hp = 0.0f;                                                 // asm 0x55188C: mov [esi+3Ch],edi
        RGE_Static_Object__set_sleep_flag(this_, 0);                      // asm 0x551889-0x55188F: the DIRECT call (:3891) - routes to set_sleep_flag_new
    }
    if (this_->master_obj->damage_sound != 0)                             // asm 0x551894-0x55189A: [ecx+54h] @84 (the RGE_Sound* pointer, dword test vs edi = 0);
                                                                          // the null test skips rand, the sound AND the srand restore together
    {
        int seed = rand_internal();                                       // asm 0x55189C: call _rand - the RNG saver (the sound path must not consume a draw)
        int pan = 0;                                                      // the out-buffers were the attacking_player/attackingObject arg slots
        int volume = 0;                                                   // (the frame labels pan = 10h / volume = 14h) - dedicated locals per the precedent
        if (RGE_Static_Object__get_3d_sound_info(this_, &pan, &volume))
        {                                                                 // asm 0x5518A3-0x5518B6: lea edx,[esp+8+volume] / lea eax,[esp+8+pan] / the DIRECT call
                                                                          // (:3893 @0x552A50, UNOWNED - executes the original); test al,al
            RGE_Sound__play(this_->master_obj->damage_sound, 1, pan, volume, -1, -1);
                                                                          // asm 0x5518B8-0x5518CE: the DIRECT call (:3662 @0x544400); the pushes (1, pan, volume, -1, -1)
                                                                          // match (int playType, int pan, int volume, i16 player_id, i16 culture)
        }
        srand_internal((unsigned int)seed);                               // asm 0x5518D3-0x5518D9: push edi (the saved rand) / call srand / add esp,4 -
                                                                          // fires on BOTH get_3d_sound_info outcomes (the jz target loc_5518D3 is the srand)
    }
}

// SEMANTIC: the mouse-pick probe. The sprite must exist; the anchor = the unit's screen position minus the
// draw origin, adjusted by the sprite-list's current animation frame. The sprite is probed at the anchor
// first, then across the square offsets [-radius, radius) in x-outer/y-inner order - a hit anywhere
// returns 1.
unsigned __int8 __fastcall RGE_Static_Object__hit_test_new(RGE_Static_Object *this_, DWORD dummy,
    __int16 draw_x, __int16 draw_y, __int16 mouse_x, __int16 mouse_y, __int16 check_radius)
{
    if (this_->sprite == 0)                                               // asm 0x551D16-0x551D1C: mov eax,[ebx+1Ch] sprite @28
        return 0;
    int frame = 0;                                                        // asm 0x551D31: xor eax,eax (the no-list path)
    if (this_->sprite_list != 0)                                          // asm 0x551D22-0x551D27: mov ecx,[ebx+24h] sprite_list @36
        frame = this_->sprite_list->vfptr->frame(this_->sprite_list, this_->sprite);
                                                                          // asm 0x551D29-0x551D2C: push eax / call [edx+4] - the virtual dispatch slot 1 = +0x4
                                                                          // (RGE_Active_Sprite_ListVtbl:7570); the i16 return sign-extended (movsx eax,ax @0x551D4D)
    short hit_x = mouse_x - this_->screen_x_offset - draw_x;              // asm 0x551D38/D41/D5F: the 16-bit subs (bp = mouse_x; sub bp,[ebx+34h] @52; sub bp,edx)
    short hit_y = mouse_y - this_->screen_y_offset - draw_y;              // asm 0x551D33/D3D/D5D: (di = mouse_y; sub di,[ebx+36h] @54; sub di,esi)
    if (RGE_Sprite__hit_test(this_->sprite, hit_x, hit_y, this_->facet, frame))
        return 1;                                                         // asm 0x551D50-0x551D6C: the DIRECT call (:3766 @0x54AD60) at the anchor (the 0,0 offset);
                                                                          // facet via xor eax,eax + mov al,[ebx+45h] (the u8 zero-extend); loc_551D75 = return 1
    if (check_radius == 0)                                                // asm 0x551D7F-0x551D86: test cx,cx
        return 0;
    // The +/- radius square scan. The asm runs hit_x/hit_y as RUNNING cursors (both offset at the column
    // head 0x551DA0-0x551DA2, hit_y++ per row 0x551DBE, both restored from the saved copies
    // 0x551DC6-0x551DCA) - the enumeration and the x-outer/y-inner order are identical to the nested loop
    // written here. The asm's column-head guard (cmp si,cx; jge to the column increment, 0x551D9B-0x551D9E)
    // is the never-firing y-counter-reset invariant (-radius >= radius is false for every radius > 0, and
    // esi is reset to -radius before every arrival) - collapsed per the redundant-guard rule.
    for (short xoff = (short)-check_radius; xoff < check_radius; xoff++)
    {                                                                     // asm 0x551D88-0x551D99: neg esi (the counter); cmp ax,cx; jge exits (the i16 signed compare);
                                                                          // the increment + jl at 0x551DDA-0x551DE2
        for (short yoff = (short)-check_radius; yoff < check_radius; yoff++)
        {                                                                 // asm 0x551DA4-0x551DC4: the inner walk (inc esi/inc edi; cmp si,radius; jl)
            if (RGE_Sprite__hit_test(this_->sprite,
                (short)(hit_x + xoff), (short)(hit_y + yoff),
                this_->facet, frame))
                return 1;                                                 // asm 0x551DAD-0x551DBB: the DIRECT call per probe; test al,al; jnz -> return 1
        }
    }
    return 0;                                                             // asm loc_551DE4 (the shared exit, xor al,al)
}

// SEMANTIC: the unit debug trace (runtime vtbl slot 118, a cdecl-style pointer - the `this` arrives on the
// STACK as the first argument). Gated on the logDebugOn switch and the logDebugID filter (-1 = all units);
// lazily opens c:\genunit.txt and stamps it with the world time; formats the caller's varargs and prints
// "UNIT #id (U=time): text", or the bare "UNIT #id" when the text is empty or the owner/world chain is
// gone. (logDebug has no idb prototype - the asm-primary decode: this @+4, format @+8, varargs @+0Ch,
// caller-clean retn; the vtbl member decl idb_types.h:4078 corroborates the cdecl varargs shape.)
void __cdecl RGE_Static_Object__logDebug_new(RGE_Static_Object *this_, char *textIn, ...)
{
    char Dest[1024];                                                      // asm 0x552515: sub esp,400h
    va_list args;
    va_start(args, textIn);
    if (*logDebugOn)                                                      // asm 0x552510-0x55251E: mov eax,logDebugOn; test/jz
    {
        if (*logDebugID == -1 || *logDebugID == this_->id)                // asm 0x552524-0x552538: mov eax,logDebugID; cmp -1; cmp [esi+10h] id @16
        {
            if (*debugOut == 0)                                           // asm 0x55253E-0x552545: mov eax,debugOut; test
            {
                *debugOut = fopen_internal("c:\\genunit.txt", "w");       // asm 0x552547-0x552559: push aW / push aCGenunit_txt / call _fopen / mov debugOut,eax
                if (*debugOut == 0)
                    return;                                               // asm 0x55255E-0x552560: test eax,eax; jz
                fprintf_internal(*debugOut, "WORLD TIME: %ld\n", this_->owner->world->world_time);
                                                                          // asm 0x552566-0x552579: [esi+18h] owner / [ecx+8Ch] world / [edx+10h] world_time @16;
                                                                          // NO null checks on this path - the original's shape, kept faithful
            }
            vsprintf_internal(Dest, textIn, args);                        // asm 0x552581-0x552597: lea edx,[esp+404h+Args] (the stack varargs) / call _vsprintf
            if (strlen(Dest) != 0 && this_->owner != 0 && this_->owner->world != 0)
                                                                          // asm 0x5525A8-0x5525BF: the repne scasb/not/dec inline strlen (= strlen per the
                                                                          // inlined-function rule); the owner/world null checks AFTER the length test
                fprintf_internal(*debugOut, "UNIT #%5d (U=%ld): %s\n", this_->id, this_->owner->world->world_time, Dest);
                                                                          // asm 0x5525C1-0x5525DA
            else
                fprintf_internal(*debugOut, "UNIT #%5d\n", this_->id);    // asm 0x5525EA-0x5525FA (the strlen == 0 / owner == 0 / world == 0 join)
        }
    }
    va_end(args);
}

// ===================== BATCH S4 (2026-11-27): the mid tier (261-356B, all single-pass) =====================

// SEMANTIC: the stealth visibility test. A unit is invisible to player_id when: a detector of that player
// sees the tile (and the asker is not the owner), the owner's own enemy-detector sees it, or the unit is
// showing its death sprite. Otherwise it is visible unless stealth applies: the mechanical-stealth flag,
// an own-stealth master with the owner's Enable Stealth for Masters on, or a Jedi Starfighter with it on.
// The baked stealth1 patch gates the whole stealth question on the UNIT_EXTRA stealth-off toggle first.
unsigned __int8 __fastcall RGE_Static_Object__is_invisible_new(RGE_Static_Object *this_, DWORD dummy, int player_id)
{
    int tx = (int)this_->world_x;                                         // asm 0x54BAB5-0x54BABD: fld [esi+48h] @72 / __ftol
    int ty = (int)this_->world_y;                                         // asm 0x54BABD-0x54BAC5: fld [esi+4Ch] @76 / __ftol
    if (this_->inside_obj != 0)                                           // asm 0x54BAC7-0x54BACC: [esi+2Ch] @44
    {
        tx = (int)this_->inside_obj->world_x;                             // asm 0x54BACE-0x54BAD6
        ty = (int)this_->inside_obj->world_y;                             // asm 0x54BAD6-0x54BADE
    }
    RGE_Tile *tile = &this_->owner->world->map->map_row_offset[ty][tx];
                                                                          // asm 0x54BAE0-0x54BAFE: shl edi,5 (the x ftol * 32 = the column stride);
                                                                          // [ecx+8Ch] world / [edx+34h] map / [edx+0BF18h] map_row_offset /
                                                                          // [edx+eax*4] (the y ftol = the row)
    if (player_id != this_->owner->id
        && RGE_Tile__is_detector(tile, (unsigned __int8)player_id))       // asm 0x54BB00-0x54BB12: cmp edi,[eax+0A0h] (owner->id @160);
                                                                          // the DIRECT call (:1934 @0x48F8C0 - routes to the owned RGE_Tile::is_detector)
        return 0;                                                         // loc_54BBB3: xor al,al
    if (player_id == this_->owner->id
        && RGE_Tile__has_enemy_detector(tile, (unsigned __int8)player_id))
                                                                          // asm 0x54BB18-0x54BB2D: the DIRECT call (:1933 @0x48F810 - routes)
        return 0;
    if (this_->sprite == this_->master_obj->vfptr->get_death_sprite(this_->master_obj, 0))
                                                                          // asm 0x54BB33-0x54BB40: the master vtbl slot 3 = +0x0C;
                                                                          // cmp [esi+1Ch] sprite @28
        return 0;
    if (getStealthOff(this_))                                             // baked: casts.cpp:870 (stealth1) @0x54BB42 - the UNIT_EXTRA
        return 0;                                                         // stealth-off toggle (specialDamage types 43/44); the stub's
                                                                          // stealth_off exit replicates the original's return-0 tail
    if ((this_->master_obj->object_flags & 4) != 0)                       // asm 0x54BB42-0x54BB4C: the stub REPLAYS mov eax,[esi+14h] /
                                                                          // test byte [eax+0ACh],4 (bit 2 = STEALTH UNIT - the object_flags
                                                                          // registry) and resumes AT the original's jz - the flags drive it
        return 1;
    if (this_->vfptr->gbg_isMaster(this_)                                 // asm 0x54BB56-0x54BB5F: the virtual dispatch slot 13 = +0x34
        && this_->owner->attributes[56] > 0.0f)                           // asm 0x54BB61-0x54BB7B: [eax+0ACh] attributes @172; fld [ecx+0E0h]
                                                                          // = [56] Enable Stealth for Masters; fcomp const_f_zero / test ah,41h;
                                                                          // jnz -> the literal > MATCH row (a NaN skips to 0)
        return 1;
    if (this_->master_obj->object_group == 64                             // asm 0x54BB85-0x54BB8D: cmp word [edx+1Eh],40h (group @30 =
                                                                          // Jedi Starfighter)
        && this_->owner->attributes[56] > 0.0f)                           // asm 0x54BB8F-0x54BBA9: the same literal > MATCH row
        return 1;
    return 0;                                                             // asm loc_54BBB3
}

// SEMANTIC: the shield query (ADOPTED VERBATIM from the shield.cpp patch
// handler - the collision naming rule; the handler was a full reimplementation
// hooked at the entry 0x0054BC40). Self-shielding masters are always shielded;
// walls never; the air set is shielded while the owner's Shields On for
// Bombers/Fighters attribute is positive (except master 1273 and Air
// Transports); masters 4/977 via the Shielding attribute, 12 via Power Core
// Shielding; everything else consults the tile. The patch's version-gated
// extension: masters 4069/4778/5904 lose the tile shield at DAT v7+.
unsigned __int8 __fastcall RGE_Static_Object__is_shielded_new(RGE_Static_Object *this_)
{
    int x = (int)this_->world_x;                                          // the handler's truncation form (value-identical to the asm's
                                                                          // __ftol, 0x54BC46-0x54BC53)
    int y = (int)this_->world_y;
    if (this_->inside_obj != 0)                                           // asm 0x54BC58-0x54BC73: the coordinate override
    {
        x = (int)this_->inside_obj->world_x;
        y = (int)this_->inside_obj->world_y;
    }
    if ((this_->master_obj->object_flags & 0x40) != 0)                    // asm 0x54BC78-0x54BC7F: bit 6 = SELF-SHIELDING (the registry)
        return 1;
    switch (this_->master_obj->object_group)                              // asm 0x54BC88-0x54BCCC: the cmp chain on group @30
    {
    case 6:                                                               // Wall -> 0 (asm 0x54BC8C)
    case 59:                                                              // Air Transport -> 0 (the handler's partition-identical
                                                                          // restructure of the original's && group != 59 disjunct)
        return 0;
    case 43:                                                              // Bomber / Fighter / Air Cruiser / Geonosian Warrior /
    case 48:                                                              // Jedi Starfighter (the handle_teleport_z_correction air set)
    case 62:
    case 63:
    case 64:
        return this_->owner->attributes[38] > 0.0f                        // asm loc_54BD5A: fld [ecx+98h] = [38]; the literal > MATCH row
            && this_->master_obj->id != 1273;                             // asm 0x54BD76: cmp word [edx+18h],4F9h
    default:
        break;
    }
    switch (this_->master_obj->id)                                        // asm 0x54BCD2-0x54BCE1: cmp dx,4 / cmp dx,3D1h (977)
    {
    case 4:
    case 977:
        if (this_->owner->attributes[51] > 0.0f)                          // asm 0x54BCE3-0x54BCFD: fld [ecx+0CCh] = [51]; the literal > MATCH row
            return 1;
        break;
    case 12:
        if (this_->owner->attributes[33] > 0.0f)                          // asm 0x54BD06-0x54BD26: fld [eax+84h] = [33]
            return 1;
        break;
    case 4069:                                                            // baked: shield.cpp:35-51 - the patch's version-gated extension
    case 4778:
    case 5904:
        if (current_loaded_version >= 7)                                  // the shield.cpp:49 form
            return 0;
        break;
    default:
        break;
    }
    return RGE_Tile__is_shielded(&this_->owner->world->map->map_row_offset[y][x],
        (unsigned __int8)this_->owner->id);
                                                                          // asm 0x54BD2F-0x54BD59: shl ebx,5 (the column stride); the row =
                                                                          // the y ftol; mov dl,[esi+0A0h] - the owner-id LOW BYTE (the
                                                                          // constant's u8 param, :1936 @0x48FA40 - routes to the owned
                                                                          // RGE_Tile::is_shielded)
}

#pragma float_control(precise, on, push)   // rule (k): the four redraw-rectangle chains feed __ftol (the tile-truncation consumer)

// SEMANTIC: the sprite swap. A null request fetches the master's default sprite; when it differs from the
// current one, the old sprite is parked in old_sprite and unregistered (the sprite list path) or its
// shadows force a map redraw of the old sprite's rectangle; then the new sprite installs (the list add at
// draw order 60, or a shadow redraw for it) and the facet resets to 0 on both paths.
void __fastcall RGE_Static_Object__new_sprite_new(RGE_Static_Object *this_, DWORD dummy, RGE_Sprite *nsprite)
{
    RGE_Sprite *sprite = nsprite;                                         // asm 0x54FDA1: mov ebx,[esp+4+sprite]
    if (sprite == 0)                                                      // asm 0x54FDA8-0x54FDAA
        sprite = this_->master_obj->vfptr->get_sprite(this_->master_obj, this_);
                                                                          // asm 0x54FDAC-0x54FDB5: the master vtbl slot 1 = +0x4; the single
                                                                          // dispatch, the result USED
    if (this_->sprite == sprite)                                          // asm 0x54FDB7-0x54FDBC: cmp [esi+1Ch] @28
        return;
    RGE_Sprite *old_sprite = this_->sprite;
    this_->old_sprite = old_sprite;                                       // asm 0x54FDC8: mov [esi+20h] @32
    if (this_->sprite_list != 0)                                          // asm 0x54FDC2-0x54FDCB: [esi+24h] @36
        RGE_Active_Sprite_List__remove_sprite(this_->sprite_list, old_sprite);
                                                                          // asm 0x54FDCD-0x54FDCE: the DIRECT call (:564 @0x423290)
    else if (old_sprite != 0 && RGE_Sprite__check_for_shadows(old_sprite) != 0)
                                                                          // asm 0x54FDD5-0x54FDE2: the DIRECT call (:3769 @0x54B0B0)
    {
        RGE_Map__request_redraw(this_->owner->world->map,                 // asm 0x54FE26-0x54FE34: [ecx+8Ch] world / [edx+34h] map; the DIRECT
                                                                          // call (:1949 @0x490C40); the push order attribute/y2/x2/y1/x1
            (int)((double)this_->world_x - (double)this_->master_obj->radius_x) - 1,
                                                                          // asm 0x54FDF5-0x54FE00 + 0x54FE29: fld radius_x @60 / fsub world_x /
                                                                          // ftol / dec
            (int)((double)this_->world_y - (double)this_->master_obj->radius_y) - (int)this_->master_obj->radius_z - 1,
                                                                          // asm 0x54FE97-0x54FEAA: fld radius_z @68 / ftol; fld world_y @76 /
                                                                          // fsub radius_y @64; sub/dec - the (ftol(wy-ry) - ftol(rz) - 1) form
            (int)((double)this_->master_obj->radius_x + (double)this_->world_x),
                                                                          // asm 0x54FDF5-region pair: fld radius_x / fadd world_x / ftol
            (int)((double)this_->master_obj->radius_y + (double)this_->world_y),
            0);                                                           // asm 0x54FDE8: push 0 (attribute)
    }
    this_->sprite = sprite;                                               // asm 0x54FE3D: mov [esi+1Ch],ebx
    if (this_->sprite_list != 0)                                          // asm 0x54FE3A-0x54FE42
    {
        RGE_Active_Sprite_List__add_sprite(this_->sprite_list, sprite, 60, 0, 0, this_->facet);
                                                                          // asm 0x54FE44-0x54FE51: the DIRECT call (:563 @0x4231D0); order 0x3C;
                                                                          // the facet u8 zero-extended
        this_->facet = 0;                                                 // asm 0x54FE56: mov byte [esi+45h],0 @69
    }
    else
    {
        if (sprite != 0 && RGE_Sprite__check_for_shadows(sprite) != 0)    // asm 0x54FE60-0x54FE6D
        {
            RGE_Map__request_redraw(this_->owner->world->map,             // asm 0x54FE6F-0x54FEBE: the same chain shape
                (int)((double)this_->world_x - (double)this_->master_obj->radius_x) - 1,
                (int)((double)this_->world_y - (double)this_->master_obj->radius_y) - (int)this_->master_obj->radius_z - 1,
                (int)((double)this_->master_obj->radius_x + (double)this_->world_x),
                (int)((double)this_->master_obj->radius_y + (double)this_->world_y),
                0);
        }
        this_->facet = 0;                                                 // asm 0x54FEC3: mov byte [esi+45h],0 (BOTH paths store it)
    }
}
#pragma float_control(pop)

// SEMANTIC: the resource un-registration that runs when a trackable unit dies or departs. While the game
// is in states 0/3 and the master tracks-as-resource or spawns a seen-doppleganger: every player whose
// seen bit covers this tile loses it from their visible-resource list (the track_as_resource arm), and
// the cdd==1 masters spawn seen-dopplegangers for the players who see but have not explored the tile
// (the same slot-243 dopple master dispatch as the dying path); finally a cdd==2 master unregisters from
// its owner's Doppleganger_Creator.
void __fastcall RGE_Static_Object__remove_visible_resource_new(RGE_Static_Object *this_)
{
    int removed[9];                                                       // the asm's var_24 array - the stores are DEAD (never read in the
                                                                          // range 0x550730-0x550875); kept faithfully per the store-and-read
                                                                          // diff (the DIMU dead-store class)
    if (this_->id < 0)                                                    // asm 0x550736-0x55073B: [esi+10h] @16; test/jl
        return;
    if (this_->owner == 0 || this_->master_obj == 0)                      // asm 0x550741-0x550751: [esi+18h] / [esi+14h]
        return;
    unsigned __int8 game_state = this_->owner->world->game_state;         // asm 0x550757-0x55075D: [ecx+8Ch] world; mov cl,[ecx+29h] @41
    if (game_state != 0 && game_state != 3)                               // asm 0x550760-0x550767: test cl / cmp cl,3
        return;
    if (this_->master_obj->create_doppleganger_on_death == 0              // asm 0x55076D-0x55077F: byte [eax+0BEh] @190; byte [eax+0BDh] @189
        && this_->master_obj->track_as_resource == 0)                     // (track_as_resource - the census candidate @189)
        return;
    int tx = (int)this_->world_x;                                         // asm 0x550785-0x550795: the ftol pair (edi = the Y ftol saved
    int ty = (int)this_->world_y;                                         // BETWEEN the ftols; eax = the X ftol - the convention)
    unsigned int umv = (unsigned int)unified_map_offsets[ty][tx];
                                                                          // asm 0x55079A-0x5507A1: mov edx,unified_map_offsets[edi*4] /
                                                                          // mov ebx,[edx+eax*4]. MAP_SIZE (mapsize.cpp:1764 writeDword
                                                                          // 0x55079D = this instruction's row-table disp32 - never baked
                                                                          // per the MAP_SIZE rule)
    unsigned int lo = umv & 0xFFFF;                                       // asm 0x5507A6
    unsigned int hi = umv >> 16;                                          // asm 0x5507AC
    if (hi != 0)                                                          // asm 0x5507AF-0x5507B5: test ebp,ebp; jbe skips the loop
    {
        int idx = 0;
        for (unsigned int bits = hi; bits != 0; idx++)                    // asm 0x550842-0x55084D: the shift loop (the continue flag on the
                                                                          // shifted hi)
        {
            if ((bits & 1) != 0)                                          // asm 0x5507BD-0x5507C4
            {
                if (this_->master_obj->track_as_resource != 0)            // asm 0x5507C6-0x5507D1: byte [eax+0BDh]
                {
                    unsigned __int8 resource_group = this_->master_obj->resource_group;
                                                                          // asm 0x5507D8: mov cl,[eax+0BFh] (resource_group @191 - the
                                                                          // census candidate)
                    removed[idx] = Visible_Resource_Manager__Remove_Resource(
                        this_->owner->world->players[idx]->VR_List,
                        this_->id,
                        resource_group);
                                                                          // asm 0x5507D3-0x5507FA: the DIRECT call; [edx+edi] players[idx];
                                                                          // [eax+174h] VR_List @372 (the TRIBE_Player walk); the result
                                                                          // stored to the dead array
                }
                if (this_->master_obj->create_doppleganger_on_death == 1
                    && (lo & 1) == 0 && idx > 0)                          // asm 0x5507FE-0x550811: cmp byte [ecx+0BEh],1; test bl,1 (the
                                                                          // LO bit CLEAR required); test edi,edi (the byte offset > 0)
                {
                    TRIBE_Player *viewer = this_->owner->world->players[idx];
                                                                          // asm 0x550813-0x55082C: the same player walk
                    RGE_Master_Doppleganger_Object *dopple_master =
                        (RGE_Master_Doppleganger_Object *)viewer->master_objects[243];
                                                                          // asm 0x550830-0x550833: [eax+74h] / [ecx+3CCh] = element 243
                    dopple_master->vfptr->make_new_obj_2(dopple_master, (RGE_Player *)viewer,
                        this_->world_x, this_->world_y, this_->world_z, this_);
                                                                          // asm 0x55083B: call [edx+78h] - the virtual dispatch slot 30
                                                                          // (the S3-verified dopple-only declaration); the return discarded
                }
            }
            lo >>= 1;                                                 // asm 0x550845
            bits >>= 1;                                               // asm 0x550847 (the loop-continue flag source)
        }
    }
    if (this_->master_obj->create_doppleganger_on_death == 2)         // asm 0x550859-0x550860: cmp byte [eax+0BEh],2
        RGE_Doppleganger_Creator__remove_doppleganger_check(this_->owner->doppleganger_creator, this_);
                                                                          // asm 0x550862-0x55086C: [ecx+90h] @144; the DIRECT call
                                                                          // (:1493 @0x470820)
}

// SEMANTIC: the unit lifecycle driver. Forward-or-zero state changes only; the 2 -> 3+ crossing banks the
// held attributes back against the owner (the modify flag-21 twin) and fires the death doppleganger; the
// <=3 -> >=7 crossing removes the visible resource; the ACTIVE -> (dead-or-idle) crossing unexplores the
// old ring and re-explores it as the explored-but-not-visible footprint when the tile and LOS allow; the
// pre-death (<7 -> 7) crossing unexplores the visibility ring; then the state stores and a non-2/non-6
// wake clears the sleep flag.
void __fastcall RGE_Static_Object__set_object_state_new(RGE_Static_Object *this_, DWORD dummy, char new_object_state)
{
    unsigned __int8 new_state = (unsigned __int8)new_object_state;    // asm 0x550972: mov bl,[esp+8+new_object_state]
    unsigned __int8 old_state = this_->object_state;                  // asm 0x550979: mov al,[esi+58h] @88
    if (new_state == old_state)                                       // asm 0x55097C-0x55097E
        return;
    if (new_state < old_state && new_state != 0)                      // asm 0x550984-0x550988: jnb (the unsigned >=) skips; test bl,bl
        return;
    if (new_state >= 3 && old_state < 3)                              // asm 0x55098E-0x550995: cmp bl,3 jb / cmp al,3 jnb
    {
        for (size_t i = 0; i < _countof(this_->master_obj->attribute_flag); i++)
                                                                          // asm 0x5509A8-0x5509F2: the edi 128->140 step-4 walk =
                                                                          // _countof(attribute_flag) iterations (the arrays [3] @122/128/152)
        {
            if (this_->master_obj->attribute_flag[i] == 2)            // asm 0x5509AB: byte [eax+ebp+98h] @152+i
            {
                short type = this_->master_obj->attribute_type_held[i];
                                                                          // asm 0x5509B5-0x5509B9: mov dx,[ecx+eax] (@122+2i, i16)
                if (type >= 0)                                        // asm 0x5509BD-0x5509C0: test dx,dx; jl skips
                {
                    float banked = -this_->master_obj->attribute_amount_held[i];
                                                                          // asm 0x5509C7-0x5509CF: fld [edi+eax] (@128+4i) / fchs / fstp
                                                                          // var_1C - the exact negation
                    this_->owner->vfptr->add_attribute_num(this_->owner, type, banked, 0);
                                                                          // asm 0x5509C2-0x5509D3: the slot-reserved/overwritten push
                                                                          // idiom; the virtual dispatch slot 32 = +0x80; bl (the new state)
                                                                          // reloaded after the call @0x5509D9
                }
            }
        }
        if (this_->master_obj->create_doppleganger_on_death == 2)     // asm 0x5509F4-0x550A00: byte [edx+0BEh] @190
            this_->vfptr->create_doppleganger_when_dying(this_);      // asm 0x550A02-0x550A06: the virtual dispatch slot 55 = +0xDC
                                                                          // (idb_types.h:4015) - routes to the S3-owned
                                                                          // create_doppleganger_when_dying_new
    }
    if (old_state <= 3 && new_state >= 7)                             // asm 0x550A0C-0x550A15: cmp byte [esi+58h],3 ja / cmp bl,7 jb
        this_->vfptr->remove_visible_resource(this_);                 // asm 0x550A17-0x550A1B: the virtual dispatch slot 54 = +0xD8
                                                                          // (idb_types.h:4014) - routes to the S4-owned
                                                                          // remove_visible_resource_new
    if ((new_state >= 3 || new_state == 0) && old_state == 2)         // asm 0x550A21-0x550A2E: cmp bl,3 jnb / test bl,bl jnz /
                                                                          // cmp byte [esi+58h],2
    {
        if (this_->tile != 0)                                         // asm 0x550A30-0x550A35: [esi+28h] @40
        {
            this_->vfptr->unexplore_terrain(this_, (RGE_Player *)this_->owner, 0, -1, 0);
                                                                          // asm 0x550A37-0x550A45: the virtual dispatch slot 89 = +0x164
            if (this_->master_obj->los > 0.0f)                        // asm 0x550A4B-0x550A5C: fld [edx+34h] (master->los @52 - a
                                                                          // FLOAT field); the literal > MATCH row (a NaN skips)
                this_->vfptr->explore_terrain(this_, (RGE_Player *)this_->owner, 1, -1, 0);
                                                                          // asm 0x550A5E-0x550A6C: the virtual dispatch slot 88 = +0x160;
                                                                          // the pushes (owner, 1, -1, 0) - explore_type = 1 (the square
                                                                          // footprint)
        }
    }
    if (old_state < 7 && new_state == 7                               // asm 0x550A72-0x550A7B: cmp byte [esi+58h],7 jnb / cmp bl,7 jnz
        && this_->tile != 0                                           // asm 0x550A7D-0x550A82
        && this_->master_obj->los > 0.0f)                             // asm 0x550A84-0x550A95: the same literal > MATCH row
        this_->vfptr->unexplore_terrain(this_, (RGE_Player *)this_->owner, 1, -1, 0);
                                                                          // asm 0x550A97-0x550AA5: the pushes (owner, 1, -1, 0)
    unsigned __int8 sleep_flag = this_->sleep_flag;                   // asm 0x550AAB: mov al,[esi+59h] @89 - read BEFORE the store
    this_->object_state = new_state;                                  // asm 0x550AAE: mov [esi+58h],bl
    if (sleep_flag != 0 && new_state != 2 && new_state != 6)          // asm 0x550AB1-0x550ABD: test al / cmp bl,2 / cmp bl,6
        RGE_Static_Object__set_sleep_flag(this_, 0);                  // asm 0x550ABF-0x550AC3: the DIRECT call (:3891) - routes to
                                                                          // set_sleep_flag_new
}

#pragma float_control(precise, on, push)   // rule (k): the radius-square chains feed __ftol (the tile-truncation consumer)

// SEMANTIC: the LOS exploration entry. Master 112 (the registry candidate) never uses the square form; the
// radius comes from the caller or the master's LOS (as an ftol'd int); the circle form explores (x, y) at
// the radius, the square form requires both master radii positive and covers the sprite box; both arms log
// through the LOS-depth mod's position memory first (the baked on_explore/on_explore_sq side-calls).
int __fastcall RGE_Static_Object__explore_terrain_new(RGE_Static_Object *this_, DWORD dummy,
    RGE_Player *whos, unsigned __int8 explore_type, int override_los, int square)
{
    if (whos == 0)                                                    // asm 0x5515B7-0x5515BC: test ebp,ebp
        return 0;
    if (this_->master_obj->id == 112)                                 // asm 0x5515C2-0x5515CC: cmp word [edi+18h],70h
        explore_type = 0;                                             // the arg-slot store - the dedicated local per the precedent
    int radius = override_los;                                        // asm 0x5515D1-0x5515D8
    if (override_los == -1)
        radius = (int)this_->master_obj->los;                         // asm 0x5515DA-0x5515DD: fld [edi+34h] (the FLOAT los @52) / __ftol
    if (radius <= 0)                                                  // asm 0x5515E2-0x5515E4: test eax,eax; jle
        return 0;
    RGE_Visible_Map *visible = whos->visible;                         // asm 0x5515F0: [ebp+88h] @136
    int update_map = whos->id == whos->world->curr_player;            // asm 0x5515EA-0x55160D: [ecx+94h] curr_player @148 (i16, movsx);
                                                                      // cmp [ebp+0A0h] id @160; setz
    LOS_ExploreInfo *exp_data = &this_->ei;                           // asm 0x551618: lea edx,[esi+90h] @144
    if (explore_type == 0)                                            // asm 0x551609-0x551616
    {
        int tx = (int)this_->world_x;                                 // asm 0x551632-0x551636: fld [esi+48h] / __ftol (the ftol AFTER
        int ty = (int)this_->world_y;                                 // the arg pushes - st0 persists across them)
        int* p_tx = &tx; int* p_ty = &ty;                             // baked: los.cpp:1037 (on_explore) @0x55163E - the LOS-depth mod's
        int* p_radius = &radius; int* p_square = &square;             // position memory (the stub passes the original's stack-arg
        explore_log(1, 0, p_tx, p_ty, 0, 0, p_radius, p_square,       // addresses; los_data = the ExpData value = &this->ei, the
            (UNIT_LOS_DATA **)exp_data);                             // ei.theObj slot the mod repurposes)
        return RGE_Visible_Map__explore_terrain(visible, tx, ty, radius, square, update_map,
            this_->id, exp_data);
                                                                      // asm 0x55163C-0x55163E: ecx = visible; the DIRECT call
                                                                      // (:5947 @0x615620); the pushes (tx, ty, radius, Square,
                                                                      // UpdateMap, id, ExpData)
    }
    if (!(this_->master_obj->radius_x > 0.0f))                        // asm 0x55164A-0x551658: fld [edi+3Ch] @60 / test ah,41h; jnz ->
                                                                      // the literal > MATCH row (a NaN exits 0)
        return 0;
    if (!(this_->master_obj->radius_y > 0.0f))                        // asm 0x55165A-0x551668: [edi+40h] @64; the same MATCH row
        return 0;
    int tx1 = (int)((double)this_->world_x - (double)this_->master_obj->radius_x);
                                                                      // asm 0x55166A-0x551670: fld world_x / fsub radius_x / ftol
    int ty1 = (int)((double)this_->world_y - (double)this_->master_obj->radius_y);
                                                                      // asm 0x551675-0x55167D
    int tx2 = (int)((double)this_->master_obj->radius_x + (double)this_->world_x - 0.01f);
                                                                      // asm 0x551682-0x551690: fld radius_x / fadd world_x / fsub
                                                                      // flt_654978 (float32 0.01 - the standing grounding) / ftol
    int ty2 = (int)((double)this_->master_obj->radius_y + (double)this_->world_y - 0.01f);
                                                                      // asm 0x551695-0x5516A5
    int* p_tx1 = &tx1; int* p_ty1 = &ty1;                             // baked: los.cpp:1039 (on_explore_sq) @0x5516C6 - the sq arm's
    int* p_tx2 = &tx2; int* p_ty2 = &ty2;                             // explore_log side-call (sq=1 -> the active body no-ops, kept
    explore_log(1, 1, p_tx1, p_ty1, p_tx2, p_ty2, 0, 0, (UNIT_LOS_DATA **)exp_data);
                                                                      // faithful)
    return RGE_Visible_Map__explore_terrain_sq(visible, tx1, ty1, tx2, ty2, update_map,
        this_->id, exp_data);
                                                                      // asm 0x5516C4-0x5516C6: the DIRECT call (:5949 @0x6159F0)
}
#pragma float_control(pop)

#pragma float_control(precise, on, push)   // rule (k): the radius-square chains feed __ftol (the tile-truncation consumer)

// SEMANTIC: the unexplore mirror of explore_terrain: the same id-112 gate, the same ftol'd float-LOS
// radius, the same two arms - with the LOS-depth mod's RESTORE arm baked ahead of each Visible_Map call
// (explore_log's explore=0 side-call overwrites tx/ty/radius from the remembered explore origin, so the
// unexplore covers exactly the area its explore covered).
void __fastcall RGE_Static_Object__unexplore_terrain_new(RGE_Static_Object *this_, DWORD dummy,
    RGE_Player *whos, unsigned __int8 explore_type, int override_los, int square)
{
    if (whos == 0)                                                    // asm 0x5516E7-0x5516EC: test ebx,ebx
        return;
    if (this_->master_obj->id == 112)                                 // asm 0x5516F2-0x5516FC: cmp word [edi+18h],70h
        explore_type = 0;
    int radius = override_los;                                        // asm 0x551701-0x551708
    if (override_los == -1)
        radius = (int)this_->master_obj->los;                         // asm 0x55170A-0x55170D: fld [edi+34h] / __ftol
    if (radius <= 0)                                                  // asm 0x551712-0x551714: test eax,eax; jle
        return;
    RGE_Visible_Map *visible = whos->visible;                         // asm 0x551720: [ebx+88h]
    int update_map = whos->id == whos->world->curr_player;            // asm 0x551726-0x551739: [ecx+94h] / cmp [ebx+0A0h] / setz
    LOS_ExploreInfo *exp_data = &this_->ei;                           // asm 0x551740: lea edx,[esi+90h]
    if (explore_type == 0)                                            // asm 0x55173C-0x55173E
    {
        int tx = (int)this_->world_x;                                 // asm 0x55175A-0x55175E
        int ty = (int)this_->world_y;                                 // asm 0x551746-0x55174D (the fld order: y first, x second)
        int* p_tx = &tx; int* p_ty = &ty;                             // baked: los.cpp:1038 (on_unexplore) @0x551766 - the RESTORE arm
        int* p_radius = &radius; int* p_square = &square;             // (the matching-radius check inside explore_log overwrites
        explore_log(0, 0, p_tx, p_ty, 0, 0, p_radius, p_square,       // tx/ty/radius from the remembered explore origin)
            (UNIT_LOS_DATA **)exp_data);
        RGE_Visible_Map__unexplore_terrain(visible, tx, ty, radius, square, update_map,
            this_->id, exp_data);
                                                                      // asm 0x551764-0x551766: the DIRECT call (:5948 @0x6158D0)
    }
    else
    {
        if (!(this_->master_obj->radius_x > 0.0f))                    // asm 0x551772-0x551780: the literal > MATCH row
            return;
        if (!(this_->master_obj->radius_y > 0.0f))                    // asm 0x551782-0x551790
            return;
        int ty2 = (int)((double)this_->master_obj->radius_y + (double)this_->world_y - 0.01f);
                                                                      // asm 0x551792-0x5517AA: fld radius_y / fadd world_y / fsub
                                                                      // flt_654978 / ftol (the ty2 chain computed FIRST per the asm)
        int tx2 = (int)((double)this_->master_obj->radius_x + (double)this_->world_x - 0.01f);
                                                                      // asm 0x5517AF-0x5517BC
        int ty1 = (int)((double)this_->world_y - (double)this_->master_obj->radius_y);
                                                                      // asm 0x5517C1-0x5517C8
        int tx1 = (int)((double)this_->world_x - (double)this_->master_obj->radius_x);
                                                                      // asm 0x5517CD-0x5517D4
        int* p_tx1 = &tx1; int* p_ty1 = &ty1;                         // baked: los.cpp:1040 (on_unexplore_sq) @0x5517DC (sq=1 -> the
        int* p_tx2 = &tx2; int* p_ty2 = &ty2;                         // active body no-ops, kept faithful)
        explore_log(0, 1, p_tx1, p_ty1, p_tx2, p_ty2, 0, 0, (UNIT_LOS_DATA **)exp_data);
        RGE_Visible_Map__unexplore_terrain_sq(visible, tx1, ty1, tx2, ty2, update_map,
            this_->id, exp_data);
                                                                      // asm 0x5517DA-0x5517DC: the DIRECT call (:5950 @0x615BE0)
    }
}
#pragma float_control(pop)

// SEMANTIC: unloads the carried objects at the drop zone. The scratch is a RING OF DROP ANCHORS: anchor 1
// is the carrier itself (its position + its master's footprint); every successful drop appends the landed
// position + the DROPPED unit's footprint as the next anchor (capped at 29); each carried object is placed
// by find_drop_location near the current anchor, retried inward through the earlier anchors when the
// neighbourhood is full. A landed object repositions to the carrier, exits, teleports to the found tile
// (the virtual slot 42), and its AI re-states and clears its target. The scratch decomposes as four
// parallel 30-entry anchor tables + the find's x out-slot (1 + 4 x 30 = the 121-float total exactly).
unsigned __int8 __fastcall RGE_Static_Object__drop_held_objects_new(RGE_Static_Object *this_, DWORD dummy, int dropZone)
{
    float found_x = 0.0f;                                             // the asm's x[0] - find_drop_location's *x out
    float found_y = 0.0f;                                             // the asm's separate y out-slot
    float anchor_x[30];                                               // the asm's x[1..30]: the ring anchors' x (index = the ring,
                                                                      // 1..29; index 0 = the region slack)
    float anchor_rx[30];                                              // the asm's x[31..60]
    float anchor_y[30];                                               // the asm's x[61..90]
    float anchor_ry[30];                                              // the asm's x[91..120]
    unsigned __int8 dropped = 0;                                      // asm 0x55202C: mov byte [esp+...+var_1ED],0
    anchor_x[1] = this_->world_x;                                     // asm 0x55201F-0x552025: var_1DC - ANCHOR 1 = the carrier itself
    anchor_y[1] = this_->world_y;                                     // asm 0x552022-0x552031: var_EC = [ebp+4Ch]
    anchor_rx[1] = this_->master_obj->radius_x;                       // asm 0x552029-0x55204A: var_164 = [eax+3Ch]
    anchor_ry[1] = this_->master_obj->radius_y;                       // asm 0x55203B-0x55203E: var_74 = [eax+40h]
    if (this_->objects == 0)                                          // asm 0x552045-0x55204A: [ebp+30h] @48
        return 0;
    int idx = this_->objects->Number_of_objects - 1;                  // asm 0x552060-0x552064: [eax+4] / dec
    if (idx < 0)                                                      // asm 0x552068: js
        return dropped;
    int ring = 1;                                                     // asm 0x55201A: mov esi,1 - the ring depth (the anchor index)
    for (; idx >= 0; idx--)                                           // asm 0x552157-0x552160: the live list index, last-first (the
                                                                      // list shrinks as items land, the walk stays last-first)
    {
        RGE_Static_Object *obj = this_->objects->List[idx];           // asm 0x552077-0x55207E: [edx+eax*4]
        if (ring > 0)                                                 // asm 0x55207A-0x552081: test esi,esi; jle skips the drop
        {
            while (RGE_Static_Object__find_drop_location(this_, dropZone, &found_x, &found_y,
                    obj->master_obj, obj,
                    anchor_x[ring], anchor_y[ring], anchor_rx[ring], anchor_ry[ring]) == 0)
                                                                          // asm 0x552087-0x5520B6: the DIRECT call (:3878 @0x552180,
                                                                          // UNOWNED - executes the original); the args = the ring's
                                                                          // anchor (x, y, radius_x, radius_y)
            {
                ring--;                                               // asm 0x5520BF: dec esi - retry inward through the earlier anchors
                if (ring <= 0)                                        // asm 0x5520C0-0x5520C2
                    break;                                            // asm 0x5520C4: jmp loc_552157 (the item skipped - no room)
            }
            if (ring > 0)                                             // the asm's success-path join (loc_552157 receives both the
            {                                                         // loop-head skip and the ring-exhausted jmp)
                obj->world_x = this_->world_x;                        // asm 0x5520C9-0x5520D6: the x87 triple fld/fstp (the x, y, z
                obj->world_y = this_->world_y;                        // store order per the asm)
                obj->world_z = this_->world_z;
                obj->vfptr->exit_obj(obj);                            // asm 0x5520D2-0x5520DF: the virtual dispatch slot 85 = +0x154
                obj->vfptr->teleport(obj, found_x, found_y, this_->world_z);
                                                                          // asm 0x5520E5-0x5520F7: the virtual dispatch slot 42 = +0xA8
                                                                          // (the double return discarded by the fstp st @0x552102)
                UnitAIModule *unit_ai = obj->unitAIValue;             // asm 0x5520FD: [edi+7Ch] @124
                if (unit_ai != 0)
                {
                    unit_ai->vfptr->setState(unit_ai, unit_ai->stateValue);
                                                                          // asm 0x552106-0x55210F: mov al,[ecx+169h] stateValue @361;
                                                                          // the virtual dispatch slot 49 = +0xC4 (UnitAIModuleVtbl:7650)
                    UnitAIModule__removeCurrentTarget(unit_ai);       // asm 0x552115-0x552118: the DIRECT call (:501 @0x41E2D0)
                }
                dropped++;                                            // asm 0x55211D-0x552123: inc cl (the u8 wrap)
                if (ring < 29)                                        // asm 0x552123-0x552126: cmp esi,1Dh - the 29-anchor cap
                {
                    ring++;                                           // asm 0x552133: inc esi
                    anchor_x[ring] = found_x;                         // asm 0x55212C-0x552134: x[++v3+1] = x[0] - the landed
                    anchor_y[ring] = found_y;                         // asm 0x552138-0x552142: position + the DROPPED unit's
                    anchor_rx[ring] = obj->master_obj->radius_x;      // asm 0x552130-0x552149: footprint joins the ring
                    anchor_ry[ring] = obj->master_obj->radius_y;      // asm 0x55213C-0x552150
                }
            }
        }
    }
    return dropped;                                                   // asm 0x552168: mov al,[esp+1F8h+var_1ED]
}

// ===================== BATCH S5-A (2026-11-27): transform + box_hit_test =====================

// SEMANTIC: the unit transformation. Banks the held attributes back, snapshots the hp ratio, then
// re-registers everything under the new master: counts -1/+1, the obstruction Remove/Update pair, the
// sprite swap, hp = new_master->hp * ratio, and the positive attribute re-apply from the new master.
void __fastcall RGE_Static_Object__transform_new(RGE_Static_Object *this_, DWORD dummy, RGE_Master_Static_Object *obj)
{
    float hp_ratio = 0.0f;                                            // the asm's uninit register on the early-exit path - value-neutral
    if (this_->object_state > 2)                                      // asm 0x5503B6: cmp byte [esi+58h],2; ja exit
        return;
    for (size_t i = 0; i < _countof(this_->master_obj->attribute_flag); i++)
                                                                          // asm 0x5503D2-0x550418: var_4=122 step2 / edi=128 step4 / cmp edi,8Ch
        if (this_->master_obj->attribute_flag[i] == 2)                    // asm 0x5503D5: byte [eax+ebp+98h] @152+i
            if (this_->master_obj->attribute_type_held[i] >= 0)           // asm 0x5503E7: test dx,dx; jl
                this_->owner->vfptr->add_attribute_num(this_->owner,       // asm 0x5503FD: slot 32 = +0x80 (the negated amount, fchs+fstp)
                    this_->master_obj->attribute_type_held[i],
                    -this_->master_obj->attribute_amount_held[i], 0);
    unsigned __int8 state = this_->object_state;                      // asm 0x550425: mov al,[esi+58h]
    double ratio = (double)this_->hp / this_->master_obj->hp + 1e-7f; // asm 0x55041D-0x550439: movsx master hp i16 @50 / fild /
                                                                          // fld hp @60 / fdiv st,st(1) / fadd flt_6565F4 / fstp qword -
                                                                          // the 53-bit ratio; flt_6565F4 = 0x33D6BF95 = 1e-7f (the
                                                                          // standing binary verification - the dd rule)
    this_->hp = (float)this_->master_obj->hp;                         // asm 0x55043D: fstp [esi+3Ch] (the fild'd master hp)
    if (state == 2)                                                   // asm 0x55042F/0x550440: cmp al,2
        this_->vfptr->check_damage_sprites(this_);                    // asm 0x550446: slot 38 = +0x98
    if (this_->tile != 0)                                             // asm 0x55044C-0x550451: [esi+28h] @40
        this_->vfptr->unexplore_terrain(this_, (RGE_Player *)this_->owner, 0, -1, 0);
                                                                          // asm 0x550461: slot 89 = +0x164
    RGE_Player__update_object_count((RGE_Player *)this_->owner, this_->master_obj->id, -1);
                                                                          // asm 0x550474: the DIRECT call (:2660 @0x4C4240); id @50
    RGE_Player__update_built_object_count((RGE_Player *)this_->owner, this_->master_obj->id, -1);
                                                                          // asm 0x550486: the DIRECT call (:2661 @0x4C4350)
    if (this_->master_obj->movement_type == 1)                        // asm 0x550493: cmp [eax+78h],bl (bl = 1)
        (*OBSystem_Air)->vfptr->RemoveObjectObstruction(*OBSystem_Air, this_);
                                                                          // asm 0x550498-0x5504A1: push esi / call [edx+2Ch] slot 11 -
                                                                          // THE 2-ARG FORM (the .c's 5-arg render = the IDA spill artifact;
                                                                          // the raw block has no extra pushes - the member suspect RESOLVED)
    else
        (*OBSystem_Land)->vfptr->RemoveObjectObstruction(*OBSystem_Land, this_);
                                                                          // asm 0x5504A6-0x5504AF
    this_->master_obj = obj;                                          // asm 0x5504B2-region
    RGE_Player__update_object_count((RGE_Player *)this_->owner, obj->id, 1);
                                                                          // asm 0x5504C2
    RGE_Player__update_built_object_count((RGE_Player *)this_->owner, obj->id, 1);
                                                                          // asm 0x5504D3
    if (this_->tile != 0)                                             // asm 0x5504DB-0x5504DD
        this_->vfptr->explore_terrain(this_, (RGE_Player *)this_->owner, 0, -1, 0);
                                                                          // asm 0x5504ED: slot 88 = +0x160
    if (this_->master_obj->movement_type == 1)                        // asm 0x5504F6: cmp [ecx+78h],bl
        (*OBSystem_Air)->vfptr->UpdateObjectObstruction(*OBSystem_Air, this_);
                                                                          // asm 0x550504: slot 10 = +0x28 (NEW GROUNDING)
    else
        (*OBSystem_Land)->vfptr->UpdateObjectObstruction(*OBSystem_Land, this_);
                                                                          // asm 0x550512
    this_->vfptr->new_sprite(this_, this_->master_obj->vfptr->get_sprite(this_->master_obj, this_));
                                                                          // asm 0x55051C/0x550522: the master slot 1 = +0x4; new_sprite
                                                                          // slot 44 = +0xB0 (routes to new_sprite_new)
    state = this_->object_state;                                      // asm 0x550537-region
    this_->hp = (float)((double)obj->hp * ratio);                     // the ratio apply (the asm's fmul + fstp [esi+3Ch])
    if (state == 2)
        this_->vfptr->check_damage_sprites(this_);                    // asm 0x550546: slot 38
    for (size_t i = 0; i < _countof(obj->attribute_flag); i++)        // asm 0x55055B-0x55059C: the re-apply walk (the obj arg slot
                                                                          // reused as the counter scratch - the dedicated local here)
        if (obj->attribute_flag[i] == 2)                              // asm 0x55055E: byte [eax+ebp+98h]
            if (obj->attribute_type_held[i] >= 0)                     // asm 0x550570: test dx,dx; jl
                this_->owner->vfptr->add_attribute_num(this_->owner,   // asm 0x550581: slot 32; the push 0/push amount/push type
                                                                          // order = the POSITIVE amount pass-through
                    obj->attribute_type_held[i], obj->attribute_amount_held[i], 0);
}

// SEMANTIC: the marquee-box pick. Builds the sprite's screen-space bounding box (min/max from the current
// frame), then the 8-gate overlap ladder: either box corner inside the marquee, the marquee origin/end
// inside the box, the vertical and horizontal edge crossings, and the final cross test.
unsigned __int8 __fastcall RGE_Static_Object__box_hit_test_new(RGE_Static_Object *this_, DWORD dummy,
    __int16 draw_x, __int16 draw_y, __int16 start_x, __int16 start_y, __int16 end_x, __int16 end_y)
{
    __int16 min_x = 0, min_y = 0, max_x = 0, max_y = 0;               // asm 0x551DF8-region: the four dword zero stores
    if (this_->sprite == 0)                                           // asm: [esi+1Ch] @28
        return 0;
    if (this_->sprite_list != 0)                                      // asm 0x551E1B-region: [esi+24h] @36
        this_->sprite_list->vfptr->get_facetindex(this_->sprite_list, this_->sprite, this_->facet);
                                                                          // asm 0x551E28: the virtual dispatch slot 11 = +0x2C
                                                                          // (RGE_Active_Sprite_ListVtbl:7580, (Sprite*, i16)); the result unused
    else
    {
        int facetindex = 0;
        RGE_Sprite__get_facetindex(this_->sprite, this_->facet, 0, &facetindex);
                                                                          // asm 0x551E3D: the DIRECT call (:3757 @0x54A2E0); the .c's v21
                                                                          // arg-slot render = the artifact
    }
    if (this_->sprite == 0)                                           // asm 0x551E45: the SECOND sprite null gate
        return 0;
    int have_frame;
    if (this_->sprite_list != 0)                                      // asm 0x551E50
        have_frame = RGE_Active_Sprite_List__get_frame(this_->sprite_list, &min_x, &min_y,
            &max_x, &max_y, this_->sprite, this_->facet);
                                                                          // asm 0x551E6F: the DIRECT call (:574 @0x423600)
    else
        have_frame = RGE_Sprite__get_frame(this_->sprite, &min_x, &min_y, &max_x, &max_y,
            this_->facet, 0);                                         // asm 0x551E98: the DIRECT call (:3763 @0x54AA50)
    if (have_frame == 0)                                              // asm 0x551E9D: test eax,eax; jz
        return 0;
    __int16 bx0 = this_->screen_x_offset + draw_x + min_x;            // asm: the i16 adds (52 + draw_x + min_x)
    __int16 by0 = this_->screen_y_offset + draw_y;                    // asm: 54 + draw_y
    __int16 bx1 = this_->screen_x_offset + draw_x + max_x;
    __int16 by1 = by0 + min_y;                                        // asm: by0 + min_y (the y pair hangs off by0)
    __int16 by2 = by0 + max_y;
    if (bx0 >= start_x && bx0 <= end_x && by0 >= start_y && by0 <= end_y)
        return 1;                                                     // asm 0x551EE1-0x551EF7: the min-corner gate (the jl/jg pairs)
    if (bx1 >= start_x && bx1 <= end_x && by2 >= start_y && by2 <= end_y)
        return 1;                                                     // asm 0x551F05-0x551F1B: the max-corner gate
    if (start_x >= bx0 && start_x <= bx1 && start_y >= by0 && start_y <= by2)
        return 1;                                                     // asm 0x551F29-0x551F3B: the start-inside gate
    if (end_x >= bx0 && end_x <= bx1)                                 // asm 0x551F4E-0x551F53: the end_x vertical-edge gate
    {
        if (end_y >= by0 && end_y <= by2)
            return 1;
    }
    if (start_x < bx0 && end_x <= bx1)                                // asm: the left-edge pair
    {
        if (start_y > by0 && start_y < by2)                           // the STRICT interior tests
            return 1;
        if (end_y > by0 && end_y < by2)
            return 1;
    }
    if (start_y < by0 && end_y > by2 && ((start_x > bx0 && start_x < bx1) || (end_x > bx0 && end_x < bx1)))
        return 1;                                                     // asm: the final cross test - the AND-of-OR compound (the
                                                                      // multi-jcc truth table verified in-range at the gate)
    return 0;                                                         // asm loc_551FDD: xor al,al
}

#pragma float_control(precise, on, push)   // rule (k): the 1/sqrt2, 2pi/facets and fb/quotient divisions feed ftol/compare consumers

// SEMANTIC: the facing update. Normalizes the requested forward vector planarly, measures it against the
// (1,0,1)/sqrt(2) diagonal via the dot product, converts the angle to a facet index against the sprite's
// facet count (mirrored by the cross-product sign), and stores it - driving the sprite list's set_facet
// with the ftol'd world tile when the facet actually changed. The u parameter is never read.
void __fastcall RGE_Static_Object__setOrientation_new(RGE_Static_Object *this_, DWORD dummy, BVector *f, BVector *u)
{
    BVector f_local = *f;                                             // asm 0x552C6D-0x552C7D: the three dword copies (x, z, y)
    double n = sqrt((double)f_local.y * f_local.y + (double)f_local.x * f_local.x);
                                                                          // asm 0x552C81-0x552C93: fld f.y / fmul f.y / fld f.x / fmul f.x /
                                                                          // faddp (y^2+x^2) / fsqrt - the 53-bit chain
    f_local.x = (float)((double)f_local.x / n);                       // asm 0x552C95-0x552C9B: fld x / fdiv st,st(1) / fstp x - the m32
                                                                          // store-round (the dot consumes the rounded components)
    f_local.z = 0.0f;                                                 // asm 0x552C9F-0x552CA7: fld 0 / fdiv st,st(1) / fstp z
    f_local.y = (float)((double)f_local.y / n);                       // asm 0x552CAB-0x552CB1
    double sqrt2 = sqrt(2.0);                                         // asm 0x552CB7-0x552CBD: fld dbl_663008 / fsqrt - dbl_663008 =
                                                                          // 4000000000000000 = 2.0 (BINARY-VERIFIED raw 0x263008)
    BVector v;                                                        // the diagonal unit: v.x = 1/sqrt2, v.z = 0, v.y = v.x
    v.x = (float)(1.0 / sqrt2);                                       // asm 0x552CBF-0x552CC7: fld 1.0 / fdiv st,st(1) / fst (the st0
                                                                          // kept for the v.y store)
    v.z = 0.0f;                                                       // asm 0x552CCB-0x552CD3: fld 0 / fdiv st,st(2) / fstp v.z
    v.y = v.x;                                                        // asm 0x552CD7: fstp v.y (the same st0 value)
    float fa = (float)((double)f_local.y * v.y + (double)v.x * f_local.x + (double)v.z * f_local.z);
                                                                          // asm 0x552CDD-0x552CF9: the dot chain (fmul/faddp x2) / fst [esp+f]
                                                                          // - the m32 store-round (the compares + acos consume fa rounded)
    float fb;
    if (X87_EQ_NAN(fa, 1.0f))                                         // asm 0x552CFD-0x552D0A: fcomp const_f_one / test ah,40h / jz
                                                                          // skips - the C3-only test, the fall-through = {== 1.0, NaN} ->
                                                                          // fb = 0 (THE MACRO: the .c's literal == diverges on NaN)
        fb = 0.0f;
    else if (X87_EQ_NAN(fa, -1.0f))                                   // asm 0x552D14-0x552D25: fcomp flt_6545C8 (-1.0f,
                                                                          // BF800000 BINARY-VERIFIED raw 0x2545C8) / the same C3-only set
                                                                          // (the NaN row dead here - fa is ordered past gate 1)
        fb = 3.1415927f;
    else
        fb = acos(fa);                                                // asm 0x552D2F-0x552D38: fld fa / call __CIacos / fstp [esp+f]
    BVector result;
    BVector__operator_mul(&f_local, &result, &v);                     // asm 0x552D3C-0x552D4A: ecx = &f_local / push v / push result /
                                                                          // the DIRECT call (:5789 @0x606B00) - the cross product
    int v7 = result.z >= 0.0f;                                        // asm 0x552D4F-0x552D65: fld result.z / fcomp const_f_zero /
                                                                          // test ah,1; jz -> keep(1) - the fall-through (flip) fires on
                                                                          // {< 0, NaN}: the literal >= MATCH row kept literal
    int facets = 8;                                                   // asm 0x552D6C-0x552D7B: the sprite null or facet_num <= 8 -> 8
    if (this_->sprite != 0 && this_->sprite->facet_num > 8)           // asm 0x552D6E-0x552D79: mov ax,[eax+70h] facet_num @112 i16;
                                                                          // cmp ax,8; movsx; jg keeps the movsx'd value
        facets = this_->sprite->facet_num;
    double q = (double)fb / ((double)6.2831855f / facets);
                                                                          // asm 0x552D84-0x552D91: fild facets / fdivr const_f_2pi
                                                                          // (40C90FDB BINARY-VERIFIED raw 0x25B388) / fdivr [esp+f] - the
                                                                          // two single-rounding divisions
    float fc = (float)q;                                              // asm 0x552D95: fst [esp+f] - the m32 store-round (THE COMPARE
                                                                          // OPERAND per rule (e); the st0 stays unrounded for floor)
    int idx = (int)floor(q);                                          // asm 0x552D9C-0x552DA9: call _floor (the st0) / call __ftol
    if (X87_LE_NAN((double)idx + 0.5, fc))                            // asm 0x552DAF-0x552DC4: fild idx / fadd const_f_half / fcomp fc /
                                                                          // test ah,41h; jz skips - the inc taken-set = {<=, NaN} (THE
                                                                          // MACRO: the .c's literal <= diverges on NaN)
        idx++;
    if (!v7)                                                          // asm 0x552DC5-0x552DCB: test ebp,ebp; jnz keeps idx
        idx = facets - idx;                                           // asm: sub esi,ebx - the cross-sign mirror
    if (this_->facet == (unsigned __int8)idx)                         // asm 0x552DCD-0x552DD0: cmp [edi+45h],bl @69 (the u8 compare)
        return;
    RGE_Active_Sprite_List *sprite_list = this_->sprite_list;         // asm 0x552DD2: mov esi,[edi+24h] @36
    this_->facet = (unsigned __int8)idx;                              // asm 0x552DD5: mov [edi+45h],bl
    if (sprite_list != 0)                                             // asm 0x552DD8-0x552DDA
    {
        int y_ftol = (int)this_->world_y;                             // asm 0x552CDC-0x552DE1: fld [edi+4Ch] @76 / ftol (the y ftol'd
                                                                          // FIRST - pushed first = the LAST arg)
        int x_ftol = (int)this_->world_x;                             // asm 0x552DE6-0x552DEA: fld [edi+48h] @72 / ftol
        sprite_list->vfptr->set_facet(sprite_list, (unsigned __int8)idx, x_ftol, y_ftol);
                                                                          // asm 0x552DF0-0x552DF8: xor ax/al / mov al,bl / push y / push x /
                                                                          // push facet / call [ebp+20h] - the virtual dispatch slot 8 =
                                                                          // +0x20 (the S2a grounding: (__int16 facet, int x, int y))
    }
}
#pragma float_control(pop)

#pragma float_control(precise, on, push)   // rule (k): the pan/volume chains feed __ftol

// SEMANTIC: the 3D sound parameters. Only the local player's unit sounds positionally: the tile must be
// visible (15) or explored-with-fog-disabled (or unexplored-with-both-map-flags-clear); pan = the view-
// relative diagonal, volume = the screen-height term, both clamped to +-1000 with the volume compensation
// for the clamped pans; returns whether the sound is audible at all.
char __fastcall RGE_Static_Object__get_3d_sound_info_new(RGE_Static_Object *this_, DWORD dummy, int *pan, int *volume)
{
    int tx = (int)this_->world_x;                                     // asm 0x552A56-0x552A5E: fld @72 / ftol
    int ty = (int)this_->world_y;                                     // asm 0x552A5E-0x552A66: fld @76 / ftol
    RGE_Player *player = RGE_Base_Game__get_player(*base_game);
                                                                          // asm 0x552A68-0x552A70: mov ecx,base_game (the memory load = the
                                                                          // this); the DIRECT call (the standing grounding :690 @0x00428790)
    if (player == 0)                                                  // asm 0x552A77-0x552A79
        return 0;
    if (player->visible == 0)                                         // asm 0x552A7F-0x552A87: [edi+88h] visible @136
        return 0;
    if (tx < 0 || ty < 0)                                             // asm 0x552A8D-0x552A97: test/jl x2
    {
        *pan = 0;                                                     // asm loc_552C22
        *volume = -5000;                                              // asm: mov dword [edx], 0FFFFEC78h
        return 0;
    }
    RGE_Map *map = this_->owner->world->map;                          // asm 0x552A9D-0x552AA6: owner @24 / world @140 / map @52
    if (tx >= map->map_width || ty >= map->map_height)                // asm 0x552AA9-0x552AB1: [eax+8] / [eax+0Ch]
    {
        *pan = 0;
        *volume = -5000;
        return 0;
    }
    unsigned __int8 vis = RGE_Visible_Map__get_visible(player->visible, tx, ty);
                                                                          // asm 0x552AB3-0x552AB5: push ebp(row)/push ebx(col) - the MAP
                                                                          // COL/ROW convention; ecx = visible preset; the DIRECT call
                                                                          // (:2000 @0x615430 - routes to the owned get_visible_new)
    if (vis != 15 && vis != 128)                                      // asm 0x552ABA-0x552AC0: cmp al,15 jz; cmp al,80h jnz
    {
        if (vis != 0)                                                 // asm 0x552ADA-0x552ADC: test al,al; jnz -> the failure
        {
            *pan = 0;
            *volume = -5000;
            return 0;
        }
        if (map->map_visible_flag == 0 || map->fog_flag != 0)         // asm 0x552AEA-0x552AFC: [eax+0BF59h]=48985 /
                                                                          // [eax+0BF5Ah]=48986 (the offsets PINNED by this asm)
        {
            *pan = 0;
            *volume = -5000;
            return 0;
        }
    }
    else if (vis == 128)                                              // the explored branch: asm 0x552AC2-0x552AD8
    {
        if (map->fog_flag != 0)                                       // asm 0x552ACE: byte [edx+0BF5Ah]
        {
            *pan = 0;
            *volume = -5000;
            return 0;
        }
    }
    double dx = (double)this_->world_x - player->view_x;              // asm 0x552B1B-0x552B21: fld [edi+178h] view_x @376 / fsubr
                                                                          // world_x @72 - the 53-bit difference
    double dy = (double)this_->world_y - player->view_y;              // asm 0x552B24-0x552B2D: fld [edi+17Ch] view_y @380 / fsubr
                                                                          // world_y @76
    *pan = (int)((dy + dx) * this_->owner->world->sound_falloff_ratio);
                                                                          // asm 0x552B36-0x552B49: fld st / fadd st,st(2) (dy+dx) / fmul
                                                                          // [edx+0A0h] (sound_falloff_ratio @160 - NEW GROUNDING) / ftol /
                                                                          // *pan = eax
    int screen_term = (int)((double)this_->screen_y_offset * 2000.0f
        / (*base_game)->draw_system->ScreenWidth);
                                                                          // asm 0x552B4B-0x552B6F: movsx word [esi+36h] @54 / [ecx+68h]
                                                                          // draw_system @104 - NEW GROUNDING / fild / fmul flt_662B84
                                                                          // (44FA0000 = 2000.0f BINARY-VERIFIED raw 0x262B84) / fidiv
                                                                          // [edx+2Ch] ScreenWidth @44
    *volume = (int)((dy - dx) * this_->owner->world->sound_falloff_ratio + screen_term);
                                                                          // asm 0x552B72-0x552B7E: fxch / fsub st,st(2) (dy-dx) / fmul ratio /
                                                                          // faddp / ftol
    if (*volume <= 0)                                                 // asm 0x552B83-0x552B87: test eax,eax; jle
        *volume = *volume * 3;                                        // asm 0x552BB2: lea edx,[ecx+ecx*2]
    else
        *volume = 0;                                                  // asm 0x552BAA
    if (*pan < -1000)                                                 // asm 0x552BB9: cmp 0FFFFFC18h; jge skips
    {
        *volume += 2 * (3 * *pan + 3000);                             // asm 0x552BC1-0x552BD0: lea edx,[edx+edx*2+0BB8h] / shl 1
        *pan = -1000;                                                 // asm 0x552BD2
    }
    else if (*pan > 1000)                                             // asm 0x552BEC-0x552BF2: cmp 3E8h; jle skips
    {
        *volume += 2 * (3 * (1000 - *pan));                           // asm 0x552BF4-0x552C04
        *pan = 1000;                                                  // asm 0x552C06
    }
    return *volume > -5000;                                           // asm 0x552BE2/0x552C10: cmp 0FFFFEC78h; setnle - the ordered
                                                                          // integer > (the ints - no NaN class)
}
#pragma float_control(pop)


// ===================== BATCH S5-C (2026-11-27): the draw family =====================

// SEMANTIC: the 3D selection outline's back leg. Unlocks the draw area for GDI work, clips to the view
// panel, selects the stock null pen, and strokes two edges of the selection diamond via RGE_Map::get_point
// (the (-rx,-ry) corner to the (rx,-ry) corner at elevation 0), then restores.
void __fastcall RGE_Static_Object__draw_frame_3d_square_back_new(RGE_Static_Object *this_, DWORD dummy,
    TDrawArea *render_area, __int16 x, __int16 y)
{
    float rx = this_->master_obj->outline_radius_x;                   // asm 0x54D9A7: [eax+0xC4] @196
    float ry = this_->master_obj->outline_radius_y;                   // asm 0x54D9B3: [eax+0xC8] @204
    RGE_Map *map = this_->owner->world->map;                          // asm 0x54D99B-0x54D9A4: world @140 / map @52
    TPanel *panel = (*base_game)->vfptr->get_view_panel(*base_game);
                                                                          // asm 0x54D9AD-0x54D9C3: mov ecx,base_game / mov edx,[ecx] /
                                                                          // call [edx+30h] - the virtual dispatch slot 12 = +0x30
                                                                          // (RGE_Base_GameVtbl:16947, get_view_panel @16961 CONFIRMED)
    if (panel != 0)                                                   // asm 0x54D9C8-0x54D9CA
    {
        TDrawArea__Unlock(render_area, "stat_obj::draw_frame_3d_square_back");
                                                                          // asm 0x54D9D5-0x54D9DC: the DIRECT call (:1522); the caller tag
        if (TDrawArea__GetDc(render_area, "stat_obj::draw_frame_3d_square_back") != 0)
                                                                          // asm: the DIRECT call (:1523)
        {
            SelectClipRgn(render_area->DrawDc, panel->clip_rgn);      // asm 0x54DA00: ds:SelectClipRgn
            SelectObject(render_area->DrawDc, GetStockObject(6));     // asm 0x54DA08-0x54DA13: NULL_PEN
            __int16 sx0 = this_->screen_x_offset + x;                 // asm: mov dx,[esi+34h] @52 / add
            __int16 sy0 = this_->screen_y_offset + y;                 // asm: [esi+38h] @56 + y
            short px[2];
            short py[2];
            RGE_Map__get_point(map, &px[0], &py[0], rx, -ry, 0.0, sx0, sy0);
                                                                          // asm 0x54DA3B-0x54DA51: the DIRECT call (:2000); (rx, -ry)
            RGE_Map__get_point(map, &px[1], &py[1], rx, ry, 0.0, sx0, sy0);
                                                                          // asm 0x54DA56-0x54DA70: (rx, +ry)
            MoveToEx(render_area->DrawDc, px[0], py[0], 0);           // asm 0x54DA7F-0x54DA87
            LineTo(render_area->DrawDc, px[1], py[1]);                // asm 0x54DA97-0x54DA9D
            RGE_Map__get_point(map, &px[0], &py[0], rx, -ry, 0.0, sx0, sy0);
                                                                          // asm 0x54DABD: the leg-2 start
            RGE_Map__get_point(map, &px[1], &py[1], -rx, -ry, 0.0, sx0, sy0);
                                                                          // asm 0x54DAE1: v15 = -x_2 -> (-rx, -ry)
            MoveToEx(render_area->DrawDc, px[0], py[0], 0);           // asm 0x54DAF0-0x54DAF8
            LineTo(render_area->DrawDc, px[1], py[1]);
            SelectClipRgn(render_area->DrawDc, 0);                    // asm: the clip restore
            TDrawArea__ReleaseDc(render_area, "stat_obj::draw_frame_3d_square_back");
                                                                          // asm: the DIRECT call (:1524)
        }
        TDrawArea__Lock(render_area, "stat_obj::draw_frame_3d_square_back", 1);
                                                                          // asm: the DIRECT call (:1521)
    }
}

// SEMANTIC: the selection frame + hp bar. Computes the sprite frame's screen bounding box, strokes the
// rectangle (2px padding), and for damaged units (state <= 2, hp >= 1) draws the 26px hp bar: the red
// full-width trough and the level fill proportional to hp/max_hp.
void __fastcall RGE_Static_Object__draw_frame_new(RGE_Static_Object *this_, DWORD dummy,
    TDrawArea *render_area, __int16 x, __int16 y)
{
    __int16 min_x = 0, min_y = 0, max_x = 0, max_y = 0;               // asm: the four dword zero stores
    if (this_->sprite != 0)                                           // asm: [esi+1Ch] @28
    {
        int facetindex;
        if (this_->sprite_list != 0)                                  // asm: [esi+24h] @36
            facetindex = this_->sprite_list->vfptr->get_facetindex(this_->sprite_list, this_->sprite, this_->facet);
                                                                          // asm: slot 11 = +0x2C (the result STORED here - the
                                                                          // box_hit_test contrast)
        else
        {
            facetindex = 0;
            RGE_Sprite__get_facetindex(this_->sprite, this_->facet, 0, &facetindex);
                                                                          // asm: the DIRECT call (:3757 @0x54A2E0)
        }
        if (this_->sprite != 0)
        {
            int have_frame;
            if (this_->sprite_list != 0)
                have_frame = RGE_Active_Sprite_List__get_frame(this_->sprite_list, &min_x, &min_y,
                    &max_x, &max_y, this_->sprite, this_->facet);
                                                                          // asm: the DIRECT call (:574 @0x423600)
            else
                have_frame = RGE_Sprite__get_frame(this_->sprite, &min_x, &min_y, &max_x, &max_y,
                    this_->facet, 0);                                 // asm: the DIRECT call (:3763 @0x54AA50)
            if (have_frame != 0)
            {
                int fx0 = x + min_x + this_->screen_x_offset - 1;     // asm: v11
                int fy0 = y + min_y + this_->screen_y_offset - 1;     // asm: v12
                TDrawArea__DrawRect(render_area, fx0, fy0,
                    x + max_x + this_->screen_x_offset + 1,
                    y + max_y + this_->screen_y_offset + 1,
                    *byte_69B550);                                    // asm: the DIRECT call (:1538 @0x473E00); the color 0xFF
                if (this_->object_state <= 2)                         // asm: cmp byte [esi+58h],2; ja skips
                {
                    int hp_i = (int)this_->hp;                        // asm: the ftol render; the < 1 -> 0 clamp
                    if (hp_i < 1)
                        hp_i = 0;
                    int hp_max = this_->master_obj->hp;               // asm: v13 = the i16 master hp @50
                    if (hp_max > 0)
                    {
                        int bar_y = fy0 - 6;                          // asm: v14 = v12 - 6
                        TDrawArea__DrawRect(render_area, fx0, bar_y, fx0 + 26, bar_y + 5, *byte_69B550);
                        TDrawArea__FillRect(render_area, fx0 + 1, bar_y + 1, fx0 + 24, bar_y + 3, *color_red);
                                                                          // asm: the DIRECT call (:1542 @0x474160); color_red = 0x24
                        TDrawArea__FillRect(render_area, fx0 + 1, bar_y + 1,
                            25 * hp_i / hp_max + fx0, bar_y + 4, *byte_69B554);
                                                                          // asm: byte_69B554 = 0xF1; the INTEGER division 25*hp/max
                    }
                }
            }
        }
    }
}

// SEMANTIC: the master unit renderer. Arms the occlusion view flags from the master's occlusion bits, and
// for dopplegangers re-labels the draw as the DOPPLED object when the local player cannot see through the
// dopple mask; then the standard render: the selection capture dispatch (outline modes 2/3), the sprite-
// list draw (the shadow pair + the screen pair + the facet) or the 11-arg bare-sprite draw; the tail
// resets the view state on every exit.
void __fastcall RGE_Static_Object__draw_new(RGE_Static_Object *this_, DWORD dummy, TDrawArea *render_area,
    __int16 x, __int16 y, RGE_Color_Table *draw_color)
{
    *SDI_Object_ID = this_->id;                                       // asm-region: the pointer-to-slot store; id @16
    if ((this_->master_obj->occlusion_flag & 1) != 0)                 // asm 0x54C9F6: test [ecx+0A8h],dl - occlusion_flag @168
                                                                          // (the census: line 1275), bit 0
    {
        *VIEW_Enable_Occlusion = 1;                                   // asm: the slot stores (common.h:138-140; THE DEREF FORM)
        *VIEW_Occlusion_Color = this_->owner->color_table->UnitOutlineColor;
    }
    if ((this_->master_obj->occlusion_flag & 4) != 0 && this_->object_state < 2)
                                                                          // asm 0x54CA1A-0x54CA23: bit 2; state @88
    {
        *VIEW_Enable_Occlusion = 1;
        *VIEW_Occlusion_Color = this_->owner->color_table->UnitOutlineColor;
    }
    if ((this_->master_obj->occlusion_flag & 2) != 0)                 // asm 0x54CA43: bit 1
        *VIEW_Object_Occludes_Others = 1;
    int skip_render = 0;                                              // the asm's LABEL_27 jump inverted (the goto-free form)
    if (this_->type == 25)                                            // asm 0x54CA51: cmp byte [esi+5Eh],19h (type @94 = 25)
    {
        RGE_Doppleganger_Object *dopple = (RGE_Doppleganger_Object *)this_;
                                                                          // THE BASE-POINTER RULE: the .c's this[1].sprite->pict_name /
                                                                          // this[1].gbg_civ_override renders = the dopple extension fields
                                                                          // through the stepped base pointer; the mapping: CantSeeBits =
                                                                          // 180 + 28 = @208, doppled_object = 180 + 4 = @184 (the census:
                                                                          // the idb dopple map; THE RUNTIME-TYPE EVIDENCE = the type-25
                                                                          // branch itself)
        if (((1 << (this_->owner->world->curr_player & 0xFF)) & dopple->CantSeeBits) != 0)
                                                                          // asm 0x54CA6A: test [esi+0D0h],eax - the .c's LOBYTE(curr_player)
                                                                          // shift (the narrowing kept); curr_player @148 (the u8 read)
            skip_render = 1;                                          // asm: the LABEL_27 jump = the render skipped, the tail only
        else
        {
            if (dopple->doppled_object != 0)                          // asm 0x54CA7C-0x54CA98: [this+184] the test
                *SDI_Object_ID = dopple->doppled_object->id;          // asm: ->id @+16 (the typed deref - the raw (doppled+16) pun
                                                                          // WITHDRAWN)
            else
                *SDI_Object_ID = -1;                                  // asm: mov SDI,-1
        }
    }
    if (!skip_render && this_->tile != 0 && this_->object_state < 7)
                                                                          // asm: the render gate (the seen case jumps PAST it to the tail)
    {
        int capture;
        if (this_->selected != 0 || (*base_game)->outline_type == 3   // asm 0x54CAC6-0x54CACC: the OR-terms 1/2 join loc_54CAEC
            || ((this_->master_obj->draw_flag & 1) == 1               // asm 0x54CAD1-0x54CADD: draw_flag @192=0xC0 bit 0
                && (*base_game)->prog_mode == 7))                     // asm 0x552CADF-0x552CAE5: prog_mode @+0x1DC
            capture = 1;
        else
            capture = 0;
        if (capture != 0 && *SDI_Capture_Info != 0                    // asm 0x54CAEC-0x552CAF4: mov ecx,SDI_Capture_Info / test -
                                                                          // THE SLOT VALUE (the deref form)
            && (*base_game)->game_mode != 1)                          // asm 0x552CAF6-0x552CAFC: game_mode @+0x1E0 vs edx=1
        {
            switch ((*base_game)->outline_type)                       // asm 0x552CB05-0x552CB0A: the 4-case switch; THE ORIGINAL
                                                                          // TABLE (.lst jpt_54CB0A) = {default, default, capture, capture}
                                                                          // - the emitted table byte-compared at the gate
            {
            case 2:
            case 3:
                RGE_Static_Object__capture_frame_3d_square(this_, render_area, x, y);
                                                                          // asm 0x54CB16: the DIRECT call - UNOWNED (executes the original)
                break;
            default:
                break;
            }
        }
        *SDI_Draw_Line = y + this_->shadow_y_offset;                  // asm-region: the slot store; shadow_y_offset @58
        if (this_->sprite_list != 0)                                  // asm 0x54CB2D-region: [esi+24h]
        {
            this_->sprite_list->vfptr->draw(this_->sprite_list,
                this_->facet,
                x + this_->screen_x_offset, y + this_->screen_y_offset,
                x + this_->shadow_x_offset, y + this_->shadow_y_offset,
                draw_color, render_area);
                                                                          // asm 0x552CB2D-0x552CB5F: the staging order - the SCREEN pair
                                                                          // (the [esi+34h]+x / [esi+36h]+y group) nearest the facet
                                                                          // (args 2-3), the SHADOW pair ([esi+38h]/[esi+3Ah]) at args 4-5
                                                                          // (the .c's v10/v11 = screen, v12/v13 = shadow - the pairs were
                                                                          // SWAPPED in the first write: the air-unit shadow at the screen
                                                                          // position = the owner-reported symptom)
                                                                          // asm 0x552CB5F: the virtual dispatch slot 3 = +0x0C
        }
        else if (this_->sprite != 0)                                  // asm 0x552CB64-0x552CB69
        {
            RGE_Sprite__draw(this_->sprite, this_->facet, 0,
                x + this_->screen_x_offset, y + this_->screen_y_offset,
                x + this_->shadow_x_offset, y + this_->shadow_y_offset,
                draw_color, render_area, 0, 0);
                                                                          // asm 0x552CB9C: the DIRECT call - the 11-arg form
        }
    }
    *SDI_Object_ID = -1;                                              // asm 0x552CBA4
    *VIEW_Enable_Occlusion = 0;                                       // asm 0x552CBAE
    *VIEW_Object_Occludes_Others = 0;                                 // asm 0x552CBB5
}

// SEMANTIC: the flood-fill pathability stamp. On first_time: validates the seed against the window,
// allocates the width-entry row-pointer table (each row height bytes) and zero-fills it; otherwise uses
// the caller's table. Stamps the seed, and while the tile is passable floods to the four neighbours
// recursively; returns the table.
unsigned __int8 **__fastcall RGE_Static_Object__fill_pathable_map_new(RGE_Static_Object *this_, DWORD dummy,
    int x, int y, int width, int height, int offset_x, int offset_y,
    RGE_Map_Analysis *map_analysis, unsigned __int8 **pathable, unsigned __int8 first_time)
{
    unsigned __int8 **table;
    if (first_time != 0)                                              // asm: test first_time
    {
        if (x < 0 || y < 0)                                           // asm: the two early 0-return gates
            return 0;
        if (x >= width || y >= height || map_analysis == 0)           // asm: the window + the analysis gates
            return 0;
        table = pathable;
        if (table == 0)
        {
            table = (unsigned __int8 **)operator_new_internal(4 * width);
                                                                          // asm: operator_new(4*width) - the runtime-sized allocation
                                                                          // (the expression matches the original; the CRT wrapper form)
            if (width > 0)
            {
                for (int r = 0; r < width; r++)
                    table[r] = (unsigned __int8 *)operator_new_internal(height);
                                                                          // asm: the row allocations (the .c's do-while = the width loop;
                                                                          // the indexed form per the pointer-arithmetic rule)
            }
        }
        for (int r = 0; r < width; r++)
            for (int i = 0; i < height; i++)
                table[r][i] = 0;
                                                                          // asm: the zero-fill (the .c's for(;;++i)[i-1]=0 increment-
                                                                          // expression idiom = the full [0..height-1] fill - the value
                                                                          // semantics; the indexed form)
    }
    else
    {
        table = pathable;
    }
    table[x][y] = 1;                                                  // asm: the seed stamp
    if (!RGE_Map_Analysis__is_tile_impassable(map_analysis, offset_x + x, offset_y + y))
                                                                          // asm: the DIRECT call - the constant pinned at the gate
    {
        int cx = x;
        int cy = y;
        if (cx - 1 >= 0 && table[cx - 1][cy] == 0)                    // asm: the left guard (the .c's nested if = the && merge)
        {
            RGE_Static_Object__fill_pathable_map_new(this_, dummy, cx - 1, cy, width, height,
                offset_x, offset_y, map_analysis, table, 0);
                                                                          // asm: the SELF-CALL - the direct recursion to OUR replacement
                                                                          // (the hook-routed form: the asm calls its own entry)
            cy = y;
            cx = x;
        }
        if (cx + 1 < width && table[cx + 1][cy] == 0)                 // asm: the right guard
        {
            RGE_Static_Object__fill_pathable_map_new(this_, dummy, cx + 1, cy, width, height,
                offset_x, offset_y, map_analysis, table, 0);
            cy = y;
            cx = x;
        }
        if (cy - 1 >= 0 && table[cx][cy - 1] == 0)                    // asm: the up guard
        {
            RGE_Static_Object__fill_pathable_map_new(this_, dummy, cx, cy - 1, width, height,
                offset_x, offset_y, map_analysis, table, 0);
            cy = y;
            cx = x;
        }
        if (cy + 1 < height && table[cx][cy + 1] == 0)                // asm: the down guard
            RGE_Static_Object__fill_pathable_map_new(this_, dummy, cx, cy + 1, width, height,
                offset_x, offset_y, map_analysis, table, 0);
    }
    return table;                                                     // asm: eax = the table
}
// SEMANTIC: the unit save stream. The 30-field sequence in the original's order, all widths sizeof-exact
// (the staging locals for the two sub-field-width writes per the sizeof rule), then the pathing-group
// element array, the group id, the mROO byte, the sprite-list presence byte, the conditional list save,
// and the baked LOS-depth save extension (the los.cpp pair) at the tail.
void __fastcall RGE_Static_Object__save_new(RGE_Static_Object *this_, DWORD dummy, int outfile)
{
    rge_write(outfile, &this_->type, sizeof(this_->type));            // asm 0x54F55D: push 1; lea edx,[esi+5Eh] @94
    unsigned __int8 owner_id_b = (unsigned __int8)this_->owner->id;   // asm 0x54F564-0x552F56C: the 1-BYTE write from the
                                                                          // owner id base @160 - the staging local (the low byte)
    rge_write(outfile, &owner_id_b, sizeof(owner_id_b));              // asm 0x54F56C: push 1
    rge_write(outfile, &this_->master_obj->id, sizeof(this_->master_obj->id));
                                                                          // asm 0x54F57E: push 2; master+0x18 = the id @24 (i16)
    short sprite_id = (short)(this_->sprite != 0 ? this_->sprite->id : -1);
                                                                          // asm 0x54F58A-0x552F5A5: mov ax,[eax+82h] (the id @130
                                                                          // LOW WORD into the Buffer's low word) - the staging local
    rge_write(outfile, &sprite_id, sizeof(sprite_id));                // asm 0x54F5A5: push 2
    int inside_id = (this_->inside_obj != 0) ? this_->inside_obj->id : -1;
                                                                          // asm 0x54F5B2-0x552F5C2: the FULL int (the 4-byte write)
    rge_write(outfile, &inside_id, sizeof(inside_id));                // asm 0x54F5C6: push 4
    rge_write(outfile, &this_->hp, sizeof(this_->hp));                // asm 0x54F5D5: lea [esi+3Ch] @60
    rge_write(outfile, &this_->sp, sizeof(this_->sp));                // asm 0x54F5E1: [esi+40h] @64
    rge_write(outfile, &this_->shield_recharge_accumulator, sizeof(this_->shield_recharge_accumulator));
                                                                          // asm 0x54F5ED: [esi+8] @8
    rge_write(outfile, &this_->object_state, sizeof(this_->object_state));
                                                                          // asm 0x54F5F9: [esi+58h] @88
    rge_write(outfile, &this_->sleep_flag, sizeof(this_->sleep_flag)); // asm 0x54F605: [esi+59h] @89
    rge_write(outfile, &this_->dopple_flag, sizeof(this_->dopple_flag));
                                                                          // asm 0x54F611: [esi+5Ah] @90
    rge_write(outfile, &this_->goto_sleep_flag, sizeof(this_->goto_sleep_flag));
                                                                          // asm 0x54F61D: [esi+5Bh] @91
    rge_write(outfile, &this_->id, sizeof(this_->id));                // asm 0x54F629: [esi+10h] @16
    rge_write(outfile, &this_->facet, sizeof(this_->facet));          // asm 0x54F635: [esi+45h] @69
    rge_write(outfile, &this_->world_x, sizeof(this_->world_x));      // asm 0x54F641: [esi+48h] @72
    rge_write(outfile, &this_->world_y, sizeof(this_->world_y));      // asm 0x54F64D: [esi+4Ch] @76
    rge_write(outfile, &this_->world_z, sizeof(this_->world_z));      // asm 0x54F659: [esi+50h] @80
    rge_write(outfile, &this_->screen_x_offset, sizeof(this_->screen_x_offset));
                                                                          // asm 0x54F665: [esi+34h] @52
    rge_write(outfile, &this_->screen_y_offset, sizeof(this_->screen_y_offset));
                                                                          // asm 0x54F671: [esi+36h] @54
    rge_write(outfile, &this_->shadow_x_offset, sizeof(this_->shadow_x_offset));
                                                                          // asm 0x54F67D: [esi+38h] @56
    rge_write(outfile, &this_->shadow_y_offset, sizeof(this_->shadow_y_offset));
                                                                          // asm 0x54F689: [esi+3Ah] @58
    rge_write(outfile, &this_->attribute_type_held, sizeof(this_->attribute_type_held));
                                                                          // asm 0x54F695: [esi+5Ch] @92
    rge_write(outfile, &this_->attribute_amount_held, sizeof(this_->attribute_amount_held));
                                                                          // asm 0x54F6A1: [esi+54h] @84
    rge_write(outfile, &this_->worker_num, sizeof(this_->worker_num));
                                                                          // asm 0x54F6AD: [esi+5Fh] @95
    rge_write(outfile, &this_->curr_damage_percent, sizeof(this_->curr_damage_percent));
                                                                          // asm 0x54F6B9: [esi+44h] @68
    rge_write(outfile, &this_->damaged_lately_timer, sizeof(this_->damaged_lately_timer));
                                                                          // asm 0x54F6C5: [esi+78h] @120
    rge_write(outfile, &this_->underAttackValue, sizeof(this_->underAttackValue));
                                                                          // asm 0x54F6D1: [esi+86h] @134
    rge_write(outfile, &this_->gbg_civ_override, sizeof(this_->gbg_civ_override));
                                                                          // asm 0x54F6E3: [esi+4] @4
    rge_write(outfile, &this_->mPathingGroupSize, sizeof(this_->mPathingGroupSize));
                                                                          // asm 0x54F6F1: [esi+70h] @112
    for (int i = 0; i < this_->mPathingGroupSize; i++)                // asm 0x54F6F8-0x552F71E: the LIVE-RELOAD count loop
                                                                          // (mov eax,[ebp+0] per iteration - the size re-read per
                                                                          // the asm; mPathingGroup @108)
        rge_write(outfile, &this_->mPathingGroup[i], sizeof(this_->mPathingGroup[i]));
                                                                          // asm 0x54F704: push 4 per element
    rge_write(outfile, &this_->mGroupID, sizeof(this_->mGroupID));    // asm 0x54F722: [esi+88h] @136
    rge_write(outfile, &this_->mROOAlreadyCalled, sizeof(this_->mROOAlreadyCalled));
                                                                          // asm 0x54F731: [esi+8Ch] @140
    unsigned __int8 has_list = this_->sprite_list != 0;               // asm 0x54F743-0x552F748: test / setnz cl
    rge_write(outfile, &has_list, sizeof(has_list));                  // asm 0x54F74C: push 1
    if (this_->sprite_list != 0)                                      // asm 0x54F75A-0x552F75C: test esi,esi (the this_fix site)
        RGE_Active_Sprite_List__save(this_->sprite_list, outfile);    // asm 0x54F75E-0x552F761: the DIRECT call (:559 @0x423060)
    // baked: los.cpp:1034/1035 (los_write_save + los_write_save_this_fix)
    // @0x54F766/0x54F757 - the LOS-depth mod's save extension: the ei.theObj
    // chain flag + x/y/radius appended at the stream tail (the this-fix's
    // ebp=esi folded - our this_ IS the unit; the option gate preserved)
    if (is_los_save_option_enabled(this_))
        los_write_save2((UNIT_LOS_DATA *)this_->ei.theObj, outfile);
}
// SEMANTIC: the damage-sprite ladder. Computes the damage percentage (100 - hp*100/max_hp), walks the

// master's damage-sprite table (the 8-byte stride) marking the crossing indices for the new and the old
// percentages, plays the newly-crossed sprite (flag 0 = the list add at order 90, flag 1 = the random-
// position tile add via get_point, flag 2 = the full-sprite swap), removes the un-crossed old one, and
// stores the new percentage.
void __fastcall RGE_Static_Object__check_damage_sprites_new(RGE_Static_Object *this_, DWORD dummy)
{
    RGE_Master_Static_Object *master = this_->master_obj;             // asm: [esi+14h] @20
    int new_idx = -1;                                                 // asm: v28 = -1
    int old_idx = -1;                                                 // asm: v29 = -1
    short hp_max = master->hp;                                        // asm 0x54F20A: test ax,ax; jle exit (the i16 max @50)
    unsigned __int8 num = master->damage_sprite_num;                  // asm 0x54F219: test bl,bl; jz exit
    int pct_frac = (int)((double)this_->hp * 100.0f / hp_max);
                                                                          // asm 0x54F221-0x552F235: fld hp @60 / fmul flt_6565EC
                                                                          // (100.0f BINARY-VERIFIED raw 0x2565EC) / fild max (the i16 ->
                                                                          // the exact widening) / fdiv / ftol - the 53-bit chain, the
                                                                          // INT truncate (NO intermediate float store)
    unsigned __int8 new_pct = (unsigned __int8)(100 - pct_frac);      // asm 0x552F23C: sub dl,al - THE BYTE-WIDTH subtract (the
                                                                          // healed-unit over-max case wraps per the asm; a float form
                                                                          // would diverge: the negative-float->u8 conversion = 0 vs the
                                                                          // asm's wrap)
    if (new_pct == this_->curr_damage_percent)                        // asm 0x552F241: cmp dl,al; jz exit
        return;
    if (new_pct >= 100)                                               // asm 0x552F24D-0x552F251: cmp dl,64h; jnb -> store+exit
    {
        this_->curr_damage_percent = new_pct;
        return;
    }
    if (num == 0)                                                     // asm 0x552F257-0x552F259: test bl,bl; jbe (num==0)
        return;
    RGE_Damage_Sprite_Info *table = master->damage_sprites;           // asm: @master+164 (the census: damage_sprites line 1274)
    for (int i = 0; i < num; i++)                                     // asm 0x552F26x-0x552F29x: the stride-8 walk (the
                                                                          // RGE_Damage_Sprite_Info stride = 8: sprite@0, pct@4, flag@5)
    {
        if (table[i].damage_percent < new_pct)                        // asm 0x552F278-0x552F27A: cmp cl,dl; jb marks
            new_idx = i;
        if (table[i].damage_percent < this_->curr_damage_percent)     // asm 0x552F280-0x552F283: cmp cl,[esi+44h] @68
            old_idx = i;
    }
    if (new_idx == old_idx)                                           // asm 0x552F298-0x552F29A
    {
        this_->curr_damage_percent = new_pct;
        return;
    }
    if (new_idx > -1)                                                 // asm 0x552F2A0-0x552F2A3
    {
        RGE_Sprite *sprite = table[new_idx].sprite;
        if (sprite != 0)
        {
            if (table[new_idx].flag == 0)                             // asm 0x552F2BB-region: the flag @+5
            {
                if (this_->sprite_list != 0)
                    RGE_Active_Sprite_List__add_sprite(this_->sprite_list, sprite, 90, 0, 0, 0);
                                                                          // asm: the DIRECT call (:563); order 0x5A = 90
            }
            else if (table[new_idx].flag == 1)                        // asm 0x552F2C5-region
            {
                if (this_->sprite_list != 0)
                {
                    double ox = (double)rand_internal() * master->radius_x * (2.0f / (float)RAND_MAX_INTERNAL)
                        - master->radius_x;
                                                                          // asm 0x552F2EC-0x552F305: call _rand / fild / fmul radius_x /
                                                                          // fmul flt_656604 / fsub radius_x - the 53-bit chain;
                                                                          // flt_656604 = 0x38800100 = the float32(2/32767) = the folded
                                                                          // 2.0f/(float)RAND_MAX_INTERNAL (the RAND_MAX_INTERNAL rule;
                                                                          // rand = rand_internal - the RNG draw)
                    double oy = (double)rand_internal() * master->radius_y * (2.0f / (float)RAND_MAX_INTERNAL)
                        - master->radius_y;
                                                                          // asm 0x552F324-0x552F34D: the same chain (the SECOND draw)
                    double x_pos = ox + ((double)this_->world_x - (int)this_->world_x);
                                                                          // asm 0x552F30B-0x552F31E: ftol(world_x); st0 = (world_x -
                                                                          // ftol_x); faddp with v14 - the 53-bit JOIN (the one m32
                                                                          // rounding at the get_point store per rule (e))
                    double y_pos = oy + ((double)this_->world_y - (int)this_->world_y);
                                                                          // asm 0x552F34x-0x552F35x: the same
                    short sx[2];
                    RGE_Map__get_point(this_->owner->world->map, &sx[1], &sx[0],
                        (float)x_pos, (float)y_pos, this_->world_z, 0, 0);
                                                                          // asm 0x552F381: the arg stores = the ONE rounding of the chain
                                                                          // asm 0x552F381: the DIRECT call (:2000 @0x4965F0)
                    RGE_Active_Sprite_List__add_sprite(this_->sprite_list, sprite, 90,
                        sx[1], sx[0], 0);
                                                                          // asm 0x552F3BC: the DIRECT call (:563)
                }
            }
            else if (table[new_idx].flag == 2)                        // asm 0x552F2BE-0x552F2D3: the jz/jnz ladder
            {
                this_->vfptr->new_sprite(this_, sprite);              // asm 0x552F2D3: slot 44 = +0xB0
            }
        }
    }
    if (old_idx > -1)                                                 // asm 0x552F3C9-0x552F3CC
    {
        RGE_Sprite *old_sprite = master->damage_sprites[old_idx].sprite;
        if (old_sprite != 0)
        {
            unsigned __int8 oflag = master->damage_sprites[old_idx].flag;
            if (oflag <= 1)                                           // asm 0x552F3E2-0x552F3E5
            {
                if (this_->sprite_list != 0)
                    RGE_Active_Sprite_List__remove_sprite(this_->sprite_list, old_sprite);
                                                                          // asm 0x552F41F: the DIRECT call (:564)
            }
            else if (oflag == 2 && (new_idx < 0 || table[new_idx].flag != 2))
                                                                          // asm 0x552F3E7-0x552F3F5: cmp bl,2 jnz skips; [eax+edx*8+5]
                                                                          // (the flag @+5 NEW pin) != 2 required
            {
                this_->vfptr->new_sprite(this_, master->vfptr->get_sprite(master, this_));
                                                                          // asm 0x552F3FC/0x552F402: the master slot 1 + the new_sprite
                                                                          // slot 44 (routes to new_sprite_new)
                this_->curr_damage_percent = new_pct;                 // asm: the EARLY-return store
                return;
            }
        }
    }
    this_->curr_damage_percent = new_pct;                             // asm: the normal tail store (loc_54F424-0x552F42C)
}

// SEMANTIC: the recycle-in re-registration. Clears the runtime state, re-binds the owner/master, restores
// hp from the master, re-picks the held attribute, re-applies the creator attributes, re-registers the
// sprite, randomizes the facet for the rotating sprites, teleports onto the z, re-registers with the
// player (addObject), and plays the created sound for the local player when the combat comparison allows.
void __fastcall RGE_Static_Object__recycle_in_to_game_new(RGE_Static_Object *this_, DWORD dummy,
    RGE_Master_Static_Object *tobj, RGE_Player *obj_owner, float x, float y, float z)
{
    this_->old_sprite = 0;                                            // asm: @32
    this_->unitAIValue = 0;                                           // asm: @124
    this_->underAttackValue = 0;                                      // asm: @134
    this_->sprite = 0;                                                // asm: @28
    this_->owner = (TRIBE_Player *)obj_owner;                                         // asm: @24
    this_->master_obj = tobj;                                         // asm: @20
    this_->tile = 0;                                                  // asm: @40
    this_->inside_obj = 0;                                            // asm: @44
    this_->sp = 0.0f;                                                 // asm: @64
    this_->hp = (float)(int)tobj->hp;                                 // asm 0x54BE84: fild (the i16 hp @50 -> the exact widening)
    this_->sleep_flag = this_->hp > 0.0f;                             // asm 0x54BE96-0x552BEA0: test ah,41h; jnz -> 1 - THE
                                                                          // LITERAL > MATCH ROW (the NaN -> 0)
    this_->dopple_flag = tobj->master_type == 25;                     // asm 0x54BEAA: cmp al,19h
    this_->world_x = -999.0f;                                         // asm: @72
    this_->world_y = -999.0f;                                         // asm: @76
    this_->mGroupID = -1;                                             // asm: @136
    this_->attribute_type_held = -1;                                  // asm: @92
    this_->facet = 0;                                                 // asm: @69
    this_->world_z = 0.0f;                                            // asm: @80
    this_->screen_x_offset = 0;                                       // asm: @52
    this_->screen_y_offset = 0;                                       // asm: @54
    this_->shadow_x_offset = 0;                                       // asm: @56
    this_->shadow_y_offset = 0;                                       // asm: @58
    this_->selected = 0;                                              // asm: @70
    this_->worker_num = 0;                                            // asm: @95
    this_->object_state = 2;                                          // asm: @88 (ACTIVE)
    this_->curr_damage_percent = 0;                                   // asm: @68
    this_->goto_sleep_flag = 0;                                       // asm: @91
    this_->player_object_node = 0;                                    // asm: @116
    this_->shield_recharge_accumulator = 0.0f;                        // asm: @8
    this_->shield_hit_timer = 0.0f;                                   // asm: @12
    this_->CurrentObstructionData = 0;                                // asm: @96
    this_->ObstructionDataList = 0;                                   // asm: @100
    this_->mROOAlreadyCalled = 0;                                     // asm: @140
    this_->attribute_amount_held = 0.0f;                              // asm: @84
    for (size_t i = 0; i < _countof(tobj->attribute_flag); i++)       // asm 0x54BF19-0x552BF3C: the 3-iteration walk; the
                                                                          // LAST match wins (no break - the .c's do-while exact)
        if (tobj->attribute_flag[i] == 0 && tobj->attribute_type_held[i] != -1)
                                                                          // asm 0x552BF1E-0x552BF29: cmp [edx],bl; cmp di,0FFFFh
        {
            this_->attribute_type_held = tobj->attribute_type_held[i];
            this_->attribute_amount_held = tobj->attribute_amount_held[i];
        }
    for (size_t i = 0; i < _countof(tobj->attribute_flag); i++)       // asm 0x552BF4A-0x552BF94: the re-apply (the flag 1..2
                                                                          // gate + the type >= 0)
        if (tobj->attribute_flag[i] >= 1 && tobj->attribute_flag[i] <= 2
            && tobj->attribute_type_held[i] >= 0)
                                                                          // asm 0x552BF5D-0x552BF72: cmp cl,1 jb / cmp cl,2 ja /
                                                                          // cmp dx,bx jl (the type >= 0)
            this_->owner->vfptr->add_attribute_num(this_->owner,
                tobj->attribute_type_held[i], tobj->attribute_amount_held[i], 0);
                                                                          // asm 0x552BF7F: slot 32 = +0x80 (the POSITIVE amount)
    this_->world_x = x;                                               // asm: @72
    this_->world_y = y;                                               // asm: @76
    this_->world_z = z;                                               // asm: @80
    this_->vfptr->new_sprite(this_, this_->master_obj->vfptr->get_sprite(this_->master_obj, this_));
                                                                          // asm 0x552BFC1/0x552BFC7: the master slot 1 = +0x4; the
                                                                          // new_sprite slot 44 = +0xB0 (routes to new_sprite_new)
    this_->old_sprite = 0;                                            // asm: @32 (the SECOND store - the .c order)
    if (this_->sprite != 0 && (this_->sprite->flag & 4) != 0 && this_->sprite->facet_num > 0)
                                                                          // asm 0x552BFD3-0x552BFE4: sprite->flag bit 2 (the RGE_Sprite
                                                                          // census @+0x80); facet_num @+0x70 > 0
    {
        int facet = this_->sprite->facet_num * rand_internal() / RAND_MAX_INTERNAL;
                                                                          // asm 0x552BFE8-0x552BFF6: call _rand / mov eax,80010003h - the
                                                                          // magic-imul /32767 form (the RAND_MAX_INTERNAL rule; the RNG
                                                                          // draw); the int chain (the i16 count x the rand - no overflow)
        this_->facet = facet;                                         // asm: @69 (the i16 store of the int)
        if ((unsigned __int8)facet >= this_->sprite->facet_num)       // asm 0x552C010: cmp dx,[edi+70h]; jl skips
            this_->facet = (unsigned __int8)(this_->sprite->facet_num - 1);
                                                                          // asm: the LOBYTE clamp
    }
    this_->vfptr->teleport(this_, x, y, z);                                 // asm 0x552C031: slot 42 = +0xA8 - the 1-arg (float z) form
                                                                          // per the .c (the member decl pinned at the gate)
    this_->player_object_node = this_->owner->vfptr->addObject(this_->owner, this_,
        this_->sleep_flag, this_->dopple_flag);
                                                                          // asm 0x552C04B: slot 72 = +0x120 (the S2b grounding)
    this_->mPathingGroupSize = 0;                                     // asm: @112 - BEFORE the sound block (the setup contrast)
    if (*combat_comparison == 0 && this_->master_obj->created_sound != 0
        && this_->owner->id == this_->owner->world->curr_player)
                                                                          // asm 0x552C05D-0x552C080: the .data byte @0x6A35FC (the
                                                                          // binding item); created_sound (the master census); id @160 ==
                                                                          // curr_player @76
    {
        int civ = this_->vfptr->gbg_get_civ_override(this_);          // asm 0x552C088: slot 2 = +0x8
        RGE_Sound__play(this_->master_obj->created_sound, 1, 0, 0, (short)civ, -1);
                                                                          // asm 0x552C096: the DIRECT call (:3662 @0x544400)
    }
}

// SEMANTIC: the fresh construction. The Static-tier setup: type = 10, the owner/master binding, the hp
// from the master, the attribute pick + the creator re-apply, the id from the reusable/next pool, the
// sprite registration, the facet randomization, the teleport onto z, the player re-registration, the
// created sound, and the pathing-group reset (AFTER the sound - the recycle contrast).
int __fastcall RGE_Static_Object__setup_new(RGE_Static_Object *this_, DWORD dummy,
    RGE_Master_Static_Object *tobj, RGE_Player *obj_owner, float x, float y, float z)
{
    this_->type = 10;                                                 // asm: @94 (RGE_MASTER_TYPE_STATIC)
    this_->owner = (TRIBE_Player *)obj_owner;                                         // asm: @24
    this_->master_obj = tobj;                                         // asm: @20
    this_->tile = 0;                                                  // asm: @40
    this_->inside_obj = 0;                                            // asm: @44
    this_->objects = 0;                                               // asm: @48
    this_->sp = 0.0f;                                                 // asm: @64
    this_->hp = (float)(int)tobj->hp;                                 // asm: fild (the i16 hp @50)
    this_->sleep_flag = this_->hp > 0.0f;                             // asm 0x54C2E1-0x552C2ED: THE LITERAL > MATCH ROW
    this_->dopple_flag = tobj->master_type == 25;                     // asm 0x54C2F5: cmp al,19h
    this_->world_x = -999.0f;                                         // asm: @72
    this_->world_y = -999.0f;                                         // asm: @76
    this_->facet = 0;                                                 // asm: @69
    this_->world_z = 0.0f;                                            // asm: @80
    this_->screen_x_offset = 0;                                       // asm: @52
    this_->screen_y_offset = 0;                                       // asm: @54
    this_->shadow_x_offset = 0;                                       // asm: @56
    this_->shadow_y_offset = 0;                                       // asm: @58
    this_->selected = 0;                                              // asm: @70
    this_->worker_num = 0;                                            // asm: @95
    this_->object_state = 2;                                          // asm: @88
    this_->curr_damage_percent = 0;                                   // asm: @68
    this_->goto_sleep_flag = 0;                                       // asm: @91
    this_->damaged_lately_timer = 0;                                  // asm: @120
    this_->shield_recharge_accumulator = 0.0f;                        // asm: @8
    this_->shield_hit_timer = 0.0f;                                   // asm: @12
    this_->attribute_type_held = -1;                                  // asm: @92
    this_->attribute_amount_held = 0.0f;                              // asm: @84
    for (size_t i = 0; i < _countof(tobj->attribute_flag); i++)       // asm 0x54C356-0x552C374: the pick loop (the LAST match)
        if (tobj->attribute_flag[i] == 0 && tobj->attribute_type_held[i] != -1)
                                                                          // asm 0x552C356-0x552C361
        {
            this_->attribute_type_held = tobj->attribute_type_held[i];
            this_->attribute_amount_held = tobj->attribute_amount_held[i];
        }
    for (size_t i = 0; i < _countof(tobj->attribute_flag); i++)       // asm 0x552C382-0x552C3CC: the re-apply
        if (tobj->attribute_flag[i] >= 1 && tobj->attribute_flag[i] <= 2
            && tobj->attribute_type_held[i] >= 0)
                                                                          // asm 0x552C395-0x552C3AA
            this_->owner->vfptr->add_attribute_num(this_->owner,
                tobj->attribute_type_held[i], tobj->attribute_amount_held[i], 0);
                                                                          // asm 0x552C3B7: slot 32 = +0x80
    if (this_->master_obj->recyclable != 0)                           // asm 0x552C3DF-0x552C3E5: byte [ecx+0BCh] @188
        this_->id = RGE_Game_World__get_next_reusable_object_id((RGE_Game_World *)this_->owner->world);
                                                                          // asm 0x552C3F0: the DIRECT call (the constant at the gate)
    else
        this_->id = RGE_Game_World__get_next_object_id((RGE_Game_World *)this_->owner->world);
                                                                          // asm 0x552C400: the DIRECT call (the constant at the gate)
    this_->sprite_list = 0;                                           // asm: @36 (AFTER the id - the .c order)
    this_->world_x = x;                                               // asm: @72
    this_->world_y = y;                                               // asm: @76
    this_->world_z = z;                                               // asm: @80
    this_->vfptr->new_sprite(this_, this_->master_obj->vfptr->get_sprite(this_->master_obj, this_));
                                                                          // asm 0x552C428/0x552C42E: the master slot 1 + the new_sprite
                                                                          // slot 44
    this_->old_sprite = 0;                                            // asm: @32 (after the new_sprite - the .c order)
    if (this_->sprite != 0 && (this_->sprite->flag & 4) != 0 && this_->sprite->facet_num > 0)
                                                                          // asm 0x552C43A-0x552C44B
    {
        int facet = this_->sprite->facet_num * rand_internal() / RAND_MAX_INTERNAL;
                                                                          // asm 0x552C44F-0x552C45D: the magic-imul /32767 form
                                                                          // (RAND_MAX_INTERNAL; the RNG draw)
        this_->facet = facet;                                         // asm: @69
        if ((unsigned __int8)facet >= this_->sprite->facet_num)       // asm 0x552C477: cmp dx,[edi+70h]; jl skips
            this_->facet = (unsigned __int8)(this_->sprite->facet_num - 1);
                                                                          // asm: the LOBYTE clamp
    }
    this_->vfptr->teleport(this_, x, y, z);                                 // asm 0x552C498: slot 42 = +0xA8
    this_->player_object_node = this_->owner->vfptr->addObject(this_->owner, this_,
        this_->sleep_flag, this_->dopple_flag);
                                                                          // asm 0x552C4B2: slot 72 = +0x120
    if (*combat_comparison == 0 && this_->master_obj->created_sound != 0
        && this_->owner->id == this_->owner->world->curr_player)
                                                                          // asm 0x552C4C1-0x552C4DE
    {
        int civ = this_->vfptr->gbg_get_civ_override(this_);          // asm 0x552C4E8-region: slot 2 = +0x8
        RGE_Sound__play(this_->master_obj->created_sound, 1, 0, 0, (short)civ, -1);
                                                                          // asm 0x552C4F8-region: the DIRECT call (:3662)
    }
    this_->mPathingGroupSize = 0;                                     // asm: @112 - AFTER the sound block (the recycle contrast)
    return 1;                                                         // asm: mov eax,1 / retn 4
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
    // BATCH S3 (2026-11-27): the big-body tier opens - the smallest eight (77-249B)
    setHook((void*)0x0054BA60, RGE_Static_Object__gbg_isObjectSpriteDisplayed_new);
    setHook((void*)0x0054CC70, RGE_Static_Object__normal_draw_new);
    setHook((void*)0x005501A0, RGE_Static_Object__modify_new);
    setHook((void*)0x00550880, RGE_Static_Object__create_doppleganger_when_dying_new);
    setHook((void*)0x00551410, RGE_Static_Object__enter_obj_new);
    setHook((void*)0x005517F0, RGE_Static_Object__damage_new);
    setHook((void*)0x00551D10, RGE_Static_Object__hit_test_new);
    setHook((void*)0x00552510, RGE_Static_Object__logDebug_new);
    // BATCH S4 (2026-11-27): the mid tier (261-356B)
    setHook((void*)0x0054BAB0, RGE_Static_Object__is_invisible_new);
    setHook((void*)0x0054BC40, RGE_Static_Object__is_shielded_new);
    setHook((void*)0x0054FDA0, RGE_Static_Object__new_sprite_new);
    setHook((void*)0x00550730, RGE_Static_Object__remove_visible_resource_new);
    setHook((void*)0x00550970, RGE_Static_Object__set_object_state_new);
    setHook((void*)0x005515B0, RGE_Static_Object__explore_terrain_new);
    setHook((void*)0x005516E0, RGE_Static_Object__unexplore_terrain_new);
    setHook((void*)0x00552010, RGE_Static_Object__drop_held_objects_new);
    // BATCH S5-A (2026-11-27): transform + box_hit_test
    setHook((void*)0x005503B0, RGE_Static_Object__transform_new);
    setHook((void*)0x00551DF0, RGE_Static_Object__box_hit_test_new);
    // BATCH S5-B (2026-11-27): setOrientation + get_3d_sound_info
    setHook((void*)0x00552C60, RGE_Static_Object__setOrientation_new);
    setHook((void*)0x00552A50, RGE_Static_Object__get_3d_sound_info_new);
    // BATCH S5-C (2026-11-27): the draw family
    setHook((void*)0x0054D990, RGE_Static_Object__draw_frame_3d_square_back_new);
    setHook((void*)0x0054EB70, RGE_Static_Object__draw_frame_new);
    setHook((void*)0x0054C9E0, RGE_Static_Object__draw_new);
    setHook((void*)0x00551130, RGE_Static_Object__fill_pathable_map_new);
    // BATCH S6 (2026-11-27): save / check_damage_sprites / recycle_in_to_game / setup
    setHook((void*)0x0054F550, RGE_Static_Object__save_new);
    setHook((void*)0x0054F1F0, RGE_Static_Object__check_damage_sprites_new);
    setHook((void*)0x0054BE50, RGE_Static_Object__recycle_in_to_game_new);
    setHook((void*)0x0054C2A0, RGE_Static_Object__setup_new);
}
