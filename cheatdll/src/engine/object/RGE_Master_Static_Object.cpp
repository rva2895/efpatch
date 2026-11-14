#include "stdafx.h"
#include "RGE_Master_Static_Object.h"

// Replacement for RGE_Master_Static_Object::get_sprite
// Original: RGE_Master_Static_Object__get_sprite @ 0x0048B8B0 (idb_functions.h:1895)
// ABI: __thiscall — this_ in ECX, obj as first stack argument, RGE_Sprite* in EAX.
// Spelled as __fastcall per project convention (mousewheel.cpp:14, overlay.cpp:15).
RGE_Sprite* __fastcall RGE_Master_Static_Object__get_sprite_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    RGE_Sprite* result;
    TRIBE_World* world;
    RGE_Master_Static_Object* master_obj;

    if (obj
        && get_object_civ(this_) != obj->vfptr->gbg_get_civ_override(obj)   // IDA: BYTE1(this->object_flags)
        && this_->master_type <= RGE_MASTER_TYPE_COMBAT                      // IDA: <= 0x46u — combat bound, excludes building
        && (world = (*base_game)->world) != 0                                                 // IDA: base_game->world; slot deref, common.cpp:7
        && (master_obj = world->master_players[obj->vfptr->gbg_get_civ_override(obj)]->master_objects[this_->copy_id]) != 0)
    {
        result = master_obj->sprite;
    }
    else
    {
        result = this_->sprite;
    }
    return result;
}

// Replacement for RGE_Master_Static_Object::get_sprite2
// Original: RGE_Master_Static_Object__get_sprite2 @ 0x0048B920 (idb_functions.h:1896)
// Clone of get_sprite_new; returns the master copy's sprite2 (idb_types.h:1218).
RGE_Sprite* __fastcall RGE_Master_Static_Object__get_sprite2_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    RGE_Sprite* result;
    TRIBE_World* world;
    RGE_Master_Static_Object* master_obj;

    if (obj
        && get_object_civ(this_) != obj->vfptr->gbg_get_civ_override(obj)
        && this_->master_type <= RGE_MASTER_TYPE_COMBAT
        && (world = (*base_game)->world) != 0
        && (master_obj = world->master_players[obj->vfptr->gbg_get_civ_override(obj)]->master_objects[this_->copy_id]) != 0)
    {
        result = master_obj->sprite2;
    }
    else
    {
        result = this_->sprite2;
    }
    return result;
}

// Replacement for RGE_Master_Static_Object::get_death_sprite
// Original: RGE_Master_Static_Object__get_death_sprite @ 0x0048B990 (idb_functions.h:1897)
// Clone of get_sprite_new; returns the master copy's death_sprite (idb_types.h:1219).
RGE_Sprite* __fastcall RGE_Master_Static_Object__get_death_sprite_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    RGE_Sprite* result;
    TRIBE_World* world;
    RGE_Master_Static_Object* master_obj;

    if (obj
        && get_object_civ(this_) != obj->vfptr->gbg_get_civ_override(obj)
        && this_->master_type <= RGE_MASTER_TYPE_COMBAT
        && (world = (*base_game)->world) != 0
        && (master_obj = world->master_players[obj->vfptr->gbg_get_civ_override(obj)]->master_objects[this_->copy_id]) != 0)
    {
        result = master_obj->death_sprite;
    }
    else
    {
        result = this_->death_sprite;
    }
    return result;
}

// Replacement for RGE_Master_Static_Object::get_undead_sprite
// Original: RGE_Master_Static_Object__get_undead_sprite @ 0x0048BA00 (idb_functions.h:1898)
// Clone of get_sprite_new; returns the master copy's undead_sprite (idb_types.h:1220).
RGE_Sprite* __fastcall RGE_Master_Static_Object__get_undead_sprite_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Static_Object* obj)
{
    RGE_Sprite* result;
    TRIBE_World* world;
    RGE_Master_Static_Object* master_obj;

    if (obj
        && get_object_civ(this_) != obj->vfptr->gbg_get_civ_override(obj)
        && this_->master_type <= RGE_MASTER_TYPE_COMBAT
        && (world = (*base_game)->world) != 0
        && (master_obj = world->master_players[obj->vfptr->gbg_get_civ_override(obj)]->master_objects[this_->copy_id]) != 0)
    {
        result = master_obj->undead_sprite;
    }
    else
    {
        result = this_->undead_sprite;
    }
    return result;
}

// Replacement for RGE_Master_Static_Object::is_unit_in_group
// Original: RGE_Master_Static_Object__is_unit_in_group @ 0x0048BA70 (idb_functions.h:1899)
// asm: movsx ecx, word ptr [ecx+1Eh] — object_group (idb_types.h:1216), signed.
unsigned __int8 __fastcall RGE_Master_Static_Object__is_unit_in_group_new(RGE_Master_Static_Object* this_, DWORD dummy, int unit_group)
{
    return !unit_group || unit_group == object_group_to_unit_group(this_->object_group);
}

// Replacement for RGE_Master_Static_Object::is_unit_in_civ
// Original: RGE_Master_Static_Object__is_unit_in_civ @ 0x0048BAA0 (idb_functions.h:1900)
unsigned __int8 __fastcall RGE_Master_Static_Object__is_unit_in_civ_new(RGE_Master_Static_Object* this_, DWORD dummy, int civ)
{
    return !civ || !get_object_civ(this_) || get_object_civ(this_) == civ;
}

// Replacement for RGE_Master_Static_Object::get_unit_civ
// Original: RGE_Master_Static_Object__get_unit_civ @ 0x0048BAD0 (idb_functions.h:1901)
// No EDX dummy — zero stack arguments (precedent: land.cpp:4).
int __fastcall RGE_Master_Static_Object__get_unit_civ_new(RGE_Master_Static_Object* this_)
{
    return get_object_civ(this_);
}

// Replacement for RGE_Master_Static_Object::do_i_care_about_power
// Original: RGE_Master_Static_Object__do_i_care_about_power @ 0x0048BCE0 (idb_functions.h:1904)
// ABI: __thiscall — this_ in ECX, no stack arguments, u8 in AL. No EDX dummy (land.cpp:4).
// asm: movsx eax, word ptr [ecx+18h] — id (idb_types.h:1213), signed; jump table off_48BD38.
// RESTRUCTURE (owner-directed 2026-11-14): original used sub/jz chains and a 60-case
// jump table; both return 1 exactly for the ids enumerated below and 0 otherwise
// (pure function of id — equivalence by exhaustive case analysis, asm-verified).
unsigned __int8 __fastcall RGE_Master_Static_Object__do_i_care_about_power_new(RGE_Master_Static_Object* this_)
{
    switch (this_->id)
    {
    case 45:
    case 49:
    case 68:
    case 84:
    case 87:
    case 101:
    case 103:
    case 104:
    case 209:
    case 317:
    case 319:
    case 323:
    case 335:
    case 530:
    case 562:
    case 584:
        return 1;
    default:
        return 0;
    }
}

// langdll-patched sprite-name civ-suffix parse (fold of langdll_1..4 / 1_1..4_1
// thunks, langdll.cpp:6-148, owner context 2026-11-14): the last two chars of
// sprite->name encode the civ index — two-digit suffix "1X" (civs 10-15) →
// X - 39; single digit (civs 1-9) → X - 49. Signed char reads (thunk movsx).
static int sprite_name_civ_index(const RGE_Master_Static_Object* master)
{
    const char* name = master->sprite->name;
    int len = (int)strlen(name);
    return name[len - 2] == '1' ? name[len - 1] - 39 : name[len - 1] - 49;
}

// Replacement for RGE_Master_Static_Object::get_string_id
// Original: RGE_Master_Static_Object__get_string_id @ 0x0048BAE0 (idb_functions.h:1902)
// ABI: __thiscall — this_ in ECX, a2 first stack argument, int in EAX.
// civ_index: 0-based civ (civ-1); negative = derive from the sprite-name suffix
// (verified: TribeTechHelpScreen::make_all_zones pushes [local-player-or-object
// civ] - 1; UI callers push -1). BAKE-IN (setLangDllHooks fold, owner-approved):
// two-digit suffix parse; worker stride 2→9, base 100140→100136; commander/cargo/
// temple bases → 99937/99957/99917; cargo condition object_group==45 → id==931.
// RESTRUCTURE (owner-directed): chained conditions rewritten — equivalence by
// exhaustive path analysis (pure function, helper called iff civ_index < 0).
int __fastcall RGE_Master_Static_Object__get_string_id_new(RGE_Master_Static_Object* this_, DWORD dummy, int civ_index)
{
    // civ resolved either from the argument or the sprite-name suffix
    const int civ = (civ_index >= 0) ? civ_index : sprite_name_civ_index(this_);

    if (this_->object_group == 58)                        // ':' — standard unit entry
    {
        int entry_offset;
        int table_base;
        int entry_stride;
        if (civ >= 6)                                     // civs 7-15: language-DLL table
        {
            entry_offset = civ - 6;
            table_base   = 34600 + 0x10000;
            entry_stride = 9;
        }
        else                                              // civs 1-6: vanilla table
        {
            entry_offset = civ;
            table_base   = 100000;
            entry_stride = 6;
        }
        return entry_offset + table_base + entry_stride * (this_->string_id - 5001);
    }

    switch (this_->id)
    {
    case 434:                                             // commander
        return civ + 34401 + 0x10000;
    case 931:                                             // cargo trader (langdll-extended)
        return civ + 34421 + 0x10000;
    case 104:                                             // jedi/sith temple
        return civ + 34381 + 0x10000;
    default:
        return this_->string_id;
    }
}

// Replacement for RGE_Master_Static_Object::get_string_id2
// Original: RGE_Master_Static_Object__get_string_id2 @ 0x0048BBE0 (idb_functions.h:1903)
// ABI: __thiscall — this_ in ECX, a2 first stack argument, int in EAX.
// BAKE-IN: same fold as get_string_id_new, twin constants; '-'-case condition
// object_group==45 → id==931. RESTRUCTURE: same equivalence argument.
// NOTE: dispatch is mixed-key (object_group for ':'/'.', id for '-'/temple)
// — the original '-'-case routed on object_group==45; the langdll hook
// compares id==931 instead, so no single switch expresses it.
int __fastcall RGE_Master_Static_Object__get_string_id2_new(RGE_Master_Static_Object* this_, DWORD dummy, int civ_index)
{
    const int civ = (civ_index >= 0) ? civ_index : sprite_name_civ_index(this_);

    if (this_->object_group == 58)                        // ':' — standard unit entry
    {
        int entry_offset;
        int table_base;
        int entry_stride;
        if (civ >= 6)
        {
            entry_offset = civ - 6;
            table_base   = 34600 + 0x10000 + 1000;
            entry_stride = 9;
        }
        else
        {
            entry_offset = civ;
            table_base   = 101000;
            entry_stride = 6;
        }
        return entry_offset + table_base + entry_stride * (this_->string_id2 - 6001);
    }

    if (this_->object_group == 46)                        // '.' case
        return civ + 34401 + 0x10000 + 1000;

    if (this_->id == 931)                                 // '-' case (langdll-extended from object_group == 45)
        return civ + 34421 + 0x10000 + 1000;

    if (this_->id == 104)                                 // temple — original default case
        return civ + 34381 + 0x10000 + 1000;

    return this_->string_id2;
}

// .rdata vftable of RGE_Master_Static_Object
// (battlegrounds_x2.lst: .rdata:0065B40C RGE_Master_Static_Object__vftable)
static RGE_Master_Static_ObjectVtbl* const rge_master_static_object_vftable =
    (RGE_Master_Static_ObjectVtbl*)0x0065B40C;

// Replacement for RGE_Master_Static_Object::destructor
// Original: RGE_Master_Static_Object__destructor @ 0x0048D200 (idb_functions.h:1914).
// ABI: __thiscall — this_ in ECX, no stack arguments, void return. No EDX dummy.
// Frees routed via free_internal — verified same target as the binary's _free
// (0x00632CCA, common.cpp:329-330).
void __fastcall RGE_Master_Static_Object__destructor_new(RGE_Master_Static_Object* this_)
{
    this_->vfptr = rge_master_static_object_vftable;
    if (this_->name)
    {
        free_internal(this_->name);
        this_->name = 0;
    }
    if (this_->gbg_name2)
    {
        free_internal(this_->gbg_name2);
        this_->gbg_name2 = 0;
    }
    if (this_->damage_sprites)
    {
        free_internal(this_->damage_sprites);
        this_->damage_sprites = 0;
    }
}

// Replacement for RGE_Master_Static_Object::scalar_deleting_destructor
// Original: RGE_Master_Static_Object__scalar_deleting_destructor @ 0x0048BDB0
// (idb_functions.h:1906). Semantics = destructor(this) + conditional
// operator_delete; the original inlined the destructor body — folded into a
// destructor_new call (equivalence: identical vfptr write + 3 conditional frees,
// asm-verified). operator_delete_internal verified same target (0x00632B42,
// common.cpp:326-327).
void* __fastcall RGE_Master_Static_Object__scalar_deleting_destructor_new(RGE_Master_Static_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Static_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Master_Static_Object::RGE_Master_Static_Object (copy ctor)
// Original: RGE_Master_Static_Object__RGE_Master_Static_Object @ 0x0048BD80
// (idb_functions.h:1905). Forwards to the original setup (not replaced).
RGE_Master_Static_Object* __fastcall RGE_Master_Static_Object__RGE_Master_Static_Object_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Master_Static_Object* other_object, int do_setup)
{
    this_->vfptr = rge_master_static_object_vftable;
    if (do_setup)
        RGE_Master_Static_Object__setup(this_, other_object);
    return this_;
}

// Replacement for RGE_Master_Static_Object::RGE_Master_Static_Object2
// Original: RGE_Master_Static_Object__RGE_Master_Static_Object2 @ 0x0048BE20
// (idb_functions.h:1907). Forwards to the original setup2 (not replaced).
RGE_Master_Static_Object* __fastcall RGE_Master_Static_Object__RGE_Master_Static_Object2_new(RGE_Master_Static_Object* this_, DWORD dummy, int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    this_->vfptr = rge_master_static_object_vftable;
    if (do_setup)
        RGE_Master_Static_Object__setup2(this_, infile, sprites, sounds);
    return this_;
}

// Replacement for RGE_Master_Static_Object::RGE_Master_Static_Object3
// Original: RGE_Master_Static_Object__RGE_Master_Static_Object3 @ 0x0048BE50
// (idb_functions.h:1908). Forwards to the original setup3 (not replaced).
RGE_Master_Static_Object* __fastcall RGE_Master_Static_Object__RGE_Master_Static_Object3_new(RGE_Master_Static_Object* this_, DWORD dummy, _iobuf* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    this_->vfptr = rge_master_static_object_vftable;
    if (do_setup)
        RGE_Master_Static_Object__setup3(this_, infile, sprites, sounds, in_id);
    return this_;
}

// Replacement for RGE_Master_Static_Object::RGE_Master_Static_Object4
// Original: RGE_Master_Static_Object__RGE_Master_Static_Object4 @ 0x0048BE90
// (idb_functions.h:1909). Forwards to the original setup4 (not replaced).
RGE_Master_Static_Object* __fastcall RGE_Master_Static_Object__RGE_Master_Static_Object4_new(RGE_Master_Static_Object* this_, DWORD dummy, int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    this_->vfptr = rge_master_static_object_vftable;
    if (do_setup)
        RGE_Master_Static_Object__setup4(this_, infile, list, sprites, sounds);
    return this_;
}

// Replacement for RGE_Master_Static_Object::modify
// Original: RGE_Master_Static_Object__modify @ 0x0048D800 (idb_functions.h:1919)
// ABI: __thiscall — this_ in ECX, amount/flag on stack, retn 8. 22-case jump
// table (off_48D894). asm note: IDA renders the __ftol conversions as
// (__int64) casts; asm shows call __ftol (float→int32) + narrow store —
// replacements use (int) per asm.
void __fastcall RGE_Master_Static_Object__modify_new(RGE_Master_Static_Object* this_, DWORD dummy, float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 0:                                                  // hp
        this_->hp = (__int16)(int)amount;
        break;
    case 1:                                                  // los
        this_->los = amount;
        break;
    case 2:                                                  // obj_max
        this_->obj_max = (unsigned __int8)(int)amount;
        break;
    case 3:                                                  // radius_x
        this_->radius_x = amount;
        break;
    case 4:                                                  // radius_y
        this_->radius_y = amount;
        break;
    case 14:                                                 // attribute_max_amount
        this_->attribute_max_amount = (__int16)(int)amount;
        break;
    case 21:                                                 // attribute_amount_held[0]
        this_->attribute_amount_held[0] = (float)(__int16)(int)amount;
        break;
    default:
        return;
    }
}

// Replacement for RGE_Master_Static_Object::modify_delta
// Original: RGE_Master_Static_Object__modify_delta @ 0x0048D8D0 (idb_functions.h:1920)
// ABI: __thiscall — this_ in ECX, amount/flag on stack, retn 8. 22-case jump
// table (off_48D97C). Integer adds are 16-bit/8-bit wraparound adds of the
// __ftol result (add mem16,ax / add cl,al); float adds via fild/fadd/fstp.
void __fastcall RGE_Master_Static_Object__modify_delta_new(RGE_Master_Static_Object* this_, DWORD dummy, float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 0:                                                  // hp
        this_->hp += (__int16)(int)amount;
        break;
    case 1:                                                  // los
        this_->los = amount + this_->los;
        break;
    case 2:                                                  // obj_max
        this_->obj_max += (unsigned __int8)(int)amount;
        break;
    case 3:                                                  // radius_x
        this_->radius_x = amount + this_->radius_x;
        break;
    case 4:                                                  // radius_y
        this_->radius_y = amount + this_->radius_y;
        break;
    case 14:                                                 // attribute_max_amount
        this_->attribute_max_amount += (__int16)(int)amount;
        break;
    case 21:                                                 // attribute_amount_held[0]
        this_->attribute_amount_held[0] += (float)(__int16)(int)amount;
        break;
    default:
        return;
    }
}

// Replacement for RGE_Master_Static_Object::modify_percent
// Original: RGE_Master_Static_Object__modify_percent @ 0x0048D9C0 (idb_functions.h:1921)
// ABI: __thiscall — this_ in ECX, amount/flag on stack, retn 8. 22-case jump
// tables (off_48DAA8/byte_48DAC8). Percent math: (double)field * amount + 0.5,
// then __ftol + narrow store; float field case uses (float)(__int16)(int).
void __fastcall RGE_Master_Static_Object__modify_percent_new(RGE_Master_Static_Object* this_, DWORD dummy, float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 0:                                                  // hp
        this_->hp = (__int16)(int)((double)this_->hp * amount + 0.5);
        break;
    case 1:                                                  // los
        this_->los = amount * this_->los;
        break;
    case 2:                                                  // obj_max
        this_->obj_max = (unsigned __int8)(int)((double)this_->obj_max * amount + 0.5);
        break;
    case 3:                                                  // radius_x
        this_->radius_x = amount * this_->radius_x;
        break;
    case 4:                                                  // radius_y
        this_->radius_y = amount * this_->radius_y;
        break;
    case 14:                                                 // attribute_max_amount
        this_->attribute_max_amount = (__int16)(int)((double)this_->attribute_max_amount * amount + 0.5);
        break;
    case 21:                                                 // attribute_amount_held[0]
        this_->attribute_amount_held[0] = (float)(__int16)(int)(this_->attribute_amount_held[0] * amount + 0.5);
        break;
    default:
        return;
    }
}

// Replacement for RGE_Master_Static_Object::copy_vars
// Original: RGE_Master_Static_Object__copy_vars @ 0x0048D510 (idb_functions.h:1918)
// ABI: __thiscall — this_ in ECX, source first stack argument, retn 4.
// Field-by-field copy in the original assignment order. The three array
// do-loops (attribute_type_held/amount_held/flag) were pointer-arithmetic
// element copies — rewritten as indexed loops (equivalence trivial). The
// pseudocode assigned object_flags twice; the duplicate store is deduplicated.
void __fastcall RGE_Master_Static_Object__copy_vars_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Master_Static_Object* source)
{
    this_->master_type = source->master_type;
    this_->name = source->name;
    this_->gbg_name2 = source->gbg_name2;
    this_->unit_line = source->unit_line;
    this_->min_tech_level = source->min_tech_level;
    this_->string_id = source->string_id;
    this_->string_id2 = source->string_id2;
    this_->id = source->id;
    this_->copy_id = source->copy_id;
    this_->base_id = source->base_id;
    this_->object_group = source->object_group;
    this_->sprite = source->sprite;
    this_->sprite2 = source->sprite2;
    this_->death_sprite = source->death_sprite;
    this_->undead_sprite = source->undead_sprite;
    this_->undead_flag = source->undead_flag;
    this_->hp = source->hp;
    this_->los = source->los;
    this_->obj_max = source->obj_max;
    this_->radius_x = source->radius_x;
    this_->radius_y = source->radius_y;
    this_->radius_z = source->radius_z;
    this_->selected_sound = source->selected_sound;
    this_->created_sound = source->created_sound;
    this_->death_sound = source->death_sound;
    this_->damage_sound = source->damage_sound;
    this_->death_spawn_obj_id = source->death_spawn_obj_id;
    this_->sort_number = source->sort_number;
    this_->can_be_built_on = source->can_be_built_on;
    this_->button_pict = source->button_pict;
    this_->hide_in_scenario_editor = source->hide_in_scenario_editor;
    this_->portrait_pict = source->portrait_pict;
    this_->available = source->available;
    this_->disabled = source->disabled;
    this_->tile_req1 = source->tile_req1;
    this_->tile_req2 = source->tile_req2;
    this_->center_tile_req1 = source->center_tile_req1;
    this_->center_tile_req2 = source->center_tile_req2;
    this_->construction_radius_x = source->construction_radius_x;
    this_->construction_radius_y = source->construction_radius_y;
    this_->elevation_flag = source->elevation_flag;
    this_->fog_flag = source->fog_flag;
    this_->terrain = source->terrain;
    this_->movement_type = source->movement_type;
    for (int i = 0; i < 3; ++i)
        this_->attribute_type_held[i] = source->attribute_type_held[i];
    for (int i = 0; i < 3; ++i)
        this_->attribute_amount_held[i] = source->attribute_amount_held[i];
    this_->attribute_max_amount = source->attribute_max_amount;
    this_->attribute_rot = source->attribute_rot;
    this_->multiple_attribute_mod = source->multiple_attribute_mod;
    for (int i = 0; i < 3; ++i)
        this_->attribute_flag[i] = source->attribute_flag[i];
    this_->area_effect_object_level = source->area_effect_object_level;
    this_->combat_level = source->combat_level;
    this_->select_level = source->select_level;
    this_->map_draw_level = source->map_draw_level;
    this_->unit_level = source->unit_level;
    this_->map_color = source->map_color;
    this_->attack_reaction = source->attack_reaction;
    this_->convert_terrain_flag = source->convert_terrain_flag;
    this_->damage_sprite_num = source->damage_sprite_num;
    this_->damage_sprites = source->damage_sprites;
    this_->occlusion_flag = source->occlusion_flag;
    this_->obstruction_type = source->obstruction_type;
    this_->obstruction_class = source->obstruction_class;
    this_->object_flags = source->object_flags;
    this_->help_string_id = source->help_string_id;
    this_->help_page_id = source->help_page_id;
    this_->hotkey_id = source->hotkey_id;
    this_->recyclable = source->recyclable;
    this_->track_as_resource = source->track_as_resource;
    this_->create_doppleganger_on_death = source->create_doppleganger_on_death;
    this_->resource_group = source->resource_group;
    this_->draw_flag = source->draw_flag;
    this_->draw_color = source->draw_color;
    this_->outline_radius_x = source->outline_radius_x;
    this_->outline_radius_y = source->outline_radius_y;
    this_->outline_radius_z = source->outline_radius_z;
}

// Replacement for RGE_Master_Static_Object::copy_obj
// Original: RGE_Master_Static_Object__copy_obj @ 0x0048D390 (idb_functions.h:1917)
// ABI: __thiscall — this_ in ECX, source first stack argument, retn 4.
// Preserves 7 identity fields across the copy_vars bulk copy; strings copied
// via getstring (idb_functions.h:2281); damage_sprites via calloc_internal
// (verified same target as the binary's _calloc, 0x00632D33); sets source
// side effects (hide_in_scenario_editor = 1, base_id = this's original id).
void __fastcall RGE_Master_Static_Object__copy_obj_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Master_Static_Object* source)
{
    const __int16 saved_id = this_->id;
    const unsigned __int8 saved_hide = this_->hide_in_scenario_editor;
    const int saved_hotkey_id = this_->hotkey_id;
    const __int16 saved_base_id = this_->base_id;
    const __int16 saved_object_group = this_->object_group;
    const unsigned __int8 saved_available = this_->available;
    const unsigned __int8 saved_disabled = this_->disabled;

    if (this_->name)
        free_internal(this_->name);
    if (this_->gbg_name2)
        free_internal(this_->gbg_name2);
    if (this_->damage_sprites)
    {
        free_internal(this_->damage_sprites);
        this_->damage_sprites = 0;
    }

    RGE_Master_Static_Object__copy_vars(this_, source);

    this_->hide_in_scenario_editor = saved_hide;
    source->hide_in_scenario_editor = 1;
    if (this_->damage_sprite_num)
    {
        this_->damage_sprites = (RGE_Damage_Sprite_Info*)calloc_internal(this_->damage_sprite_num, 8);
        for (int i = 0; i < this_->damage_sprite_num; ++i)
            this_->damage_sprites[i] = source->damage_sprites[i];
    }
    else
    {
        this_->damage_sprites = 0;
    }

    this_->name = 0;
    getstring(&this_->name, source->name);
    this_->gbg_name2 = 0;
    if (source->gbg_name2)
        getstring(&this_->gbg_name2, source->gbg_name2);

    this_->base_id = saved_base_id;
    this_->object_group = saved_object_group;
    this_->available = saved_available;
    this_->id = saved_id;
    this_->disabled = saved_disabled;
    this_->hotkey_id = saved_hotkey_id;
    source->base_id = saved_id;
}

// Replacement for RGE_Master_Static_Object::small_save
// Original: RGE_Master_Static_Object__small_save @ 0x0048DAE0 (idb_functions.h:1922)
// ABI: __thiscall — this_ in ECX, outfile first stack argument, retn 4.
// rge_write = idb_functions.h:3005 (0x004D5790). Sizes sizeof-verified
// against field types (idb_types.h:1206-1281).
void __fastcall RGE_Master_Static_Object__small_save_new(RGE_Master_Static_Object* this_, DWORD dummy, int outfile)
{
    rge_write(outfile, &this_->master_type, sizeof(this_->master_type));
    rge_write(outfile, &this_->id, sizeof(this_->id));
    rge_write(outfile, &this_->copy_id, sizeof(this_->copy_id));
    rge_write(outfile, &this_->base_id, sizeof(this_->base_id));
    rge_write(outfile, &this_->object_group, sizeof(this_->object_group));
    rge_write(outfile, &this_->hotkey_id, sizeof(this_->hotkey_id));
    rge_write(outfile, &this_->available, sizeof(this_->available));
    rge_write(outfile, &this_->hide_in_scenario_editor, sizeof(this_->hide_in_scenario_editor));
    rge_write(outfile, &this_->hp, sizeof(this_->hp));
    rge_write(outfile, &this_->los, sizeof(this_->los));
    rge_write(outfile, &this_->obj_max, sizeof(this_->obj_max));
    rge_write(outfile, &this_->radius_x, sizeof(this_->radius_x));
    rge_write(outfile, &this_->radius_y, sizeof(this_->radius_y));
    rge_write(outfile, &this_->attribute_max_amount, sizeof(this_->attribute_max_amount));
    rge_write(outfile, this_->attribute_amount_held, sizeof(this_->attribute_amount_held[0]));   // first element only
    rge_write(outfile, &this_->disabled, sizeof(this_->disabled));
}

// Replacement for RGE_Master_Static_Object::save
// Original: RGE_Master_Static_Object__save @ 0x0048DBC0 (idb_functions.h:1923)
// ABI: __thiscall — this_ in ECX, outfile first stack argument, retn 4.
// Pointer fields saved as the pointed-to object's id (RGE_Sound::id @+12,
// RGE_Sprite::id @+130 — idb_types.h:2960 / 2972-3007); -1 when null.
// Name/gbg lengths: strlen+1 for non-empty names, else 0 — signed 16-bit
// gate preserved; sizes are sizeof-verified (damage-element write
// deliberately spans damage_percent + flag — adjacent fields).
void __fastcall RGE_Master_Static_Object__save_new(RGE_Master_Static_Object* this_, DWORD dummy, int outfile)
{
    __int16 created_sound_id = -1;
    __int16 selected_sound_id = -1;
    __int16 death_sound_id = -1;
    __int16 damage_sound_id = -1;
    __int16 sprite_id = -1;
    __int16 sprite2_id = -1;
    __int16 death_sprite_id = -1;
    __int16 undead_sprite_id = -1;
    if (this_->created_sound)
        created_sound_id = this_->created_sound->id;
    if (this_->selected_sound)
        selected_sound_id = this_->selected_sound->id;
    if (this_->death_sound)
        death_sound_id = this_->death_sound->id;
    if (this_->damage_sound)
        damage_sound_id = this_->damage_sound->id;
    if (this_->sprite)
        sprite_id = this_->sprite->id;
    if (this_->sprite2)
        sprite2_id = this_->sprite2->id;
    if (this_->death_sprite)
        death_sprite_id = this_->death_sprite->id;
    if (this_->undead_sprite)
        undead_sprite_id = this_->undead_sprite->id;

    unsigned __int16 name_len = 0;
    if (this_->name)
    {
        const int len = (int)strlen(this_->name) + 1;
        if ((__int16)(len - 1) > 0)
            name_len = (unsigned __int16)len;
    }
    unsigned __int16 gbg_name2_len = 0;
    if (this_->gbg_name2)
    {
        const int len = (int)strlen(this_->gbg_name2) + 1;
        if ((__int16)(len - 1) > 0)
            gbg_name2_len = (unsigned __int16)len;
    }

    rge_write(outfile, &this_->master_type, sizeof(this_->master_type));
    rge_write(outfile, &name_len, sizeof(name_len));
    rge_write(outfile, &this_->id, sizeof(this_->id));
    rge_write(outfile, &this_->string_id, sizeof(this_->string_id));
    rge_write(outfile, &this_->string_id2, sizeof(this_->string_id2));
    rge_write(outfile, &this_->object_group, sizeof(this_->object_group));
    rge_write(outfile, &sprite_id, sizeof(sprite_id));
    rge_write(outfile, &sprite2_id, sizeof(sprite2_id));
    rge_write(outfile, &death_sprite_id, sizeof(death_sprite_id));
    rge_write(outfile, &undead_sprite_id, sizeof(undead_sprite_id));
    rge_write(outfile, &this_->undead_flag, sizeof(this_->undead_flag));
    rge_write(outfile, &this_->hp, sizeof(this_->hp));
    rge_write(outfile, &this_->los, sizeof(this_->los));
    rge_write(outfile, &this_->obj_max, sizeof(this_->obj_max));
    rge_write(outfile, &this_->radius_x, sizeof(this_->radius_x));
    rge_write(outfile, &this_->radius_y, sizeof(this_->radius_y));
    rge_write(outfile, &this_->radius_z, sizeof(this_->radius_z));
    rge_write(outfile, &created_sound_id, sizeof(created_sound_id));
    rge_write(outfile, &damage_sound_id, sizeof(damage_sound_id));
    rge_write(outfile, &this_->death_spawn_obj_id, sizeof(this_->death_spawn_obj_id));
    rge_write(outfile, &this_->sort_number, sizeof(this_->sort_number));
    rge_write(outfile, &this_->can_be_built_on, sizeof(this_->can_be_built_on));
    rge_write(outfile, &this_->button_pict, sizeof(this_->button_pict));
    rge_write(outfile, &this_->hide_in_scenario_editor, sizeof(this_->hide_in_scenario_editor));
    rge_write(outfile, &this_->portrait_pict, sizeof(this_->portrait_pict));
    rge_write(outfile, &this_->available, sizeof(this_->available));
    rge_write(outfile, &this_->disabled, sizeof(this_->disabled));
    rge_write(outfile, &this_->tile_req1, sizeof(this_->tile_req1));
    rge_write(outfile, &this_->tile_req2, sizeof(this_->tile_req2));
    rge_write(outfile, &this_->center_tile_req1, sizeof(this_->center_tile_req1));
    rge_write(outfile, &this_->center_tile_req2, sizeof(this_->center_tile_req2));
    rge_write(outfile, &this_->construction_radius_x, sizeof(this_->construction_radius_x));
    rge_write(outfile, &this_->construction_radius_y, sizeof(this_->construction_radius_y));
    rge_write(outfile, &this_->elevation_flag, sizeof(this_->elevation_flag));
    rge_write(outfile, &this_->fog_flag, sizeof(this_->fog_flag));
    rge_write(outfile, &this_->terrain, sizeof(this_->terrain));
    rge_write(outfile, &this_->movement_type, sizeof(this_->movement_type));
    rge_write(outfile, &this_->attribute_max_amount, sizeof(this_->attribute_max_amount));
    rge_write(outfile, &this_->attribute_rot, sizeof(this_->attribute_rot));
    rge_write(outfile, &this_->area_effect_object_level, sizeof(this_->area_effect_object_level));
    rge_write(outfile, &this_->combat_level, sizeof(this_->combat_level));
    rge_write(outfile, &this_->select_level, sizeof(this_->select_level));
    rge_write(outfile, &this_->map_draw_level, sizeof(this_->map_draw_level));
    rge_write(outfile, &this_->unit_level, sizeof(this_->unit_level));
    rge_write(outfile, &this_->multiple_attribute_mod, sizeof(this_->multiple_attribute_mod));
    rge_write(outfile, &this_->map_color, sizeof(this_->map_color));
    rge_write(outfile, &this_->help_string_id, sizeof(this_->help_string_id));
    rge_write(outfile, &this_->help_page_id, sizeof(this_->help_page_id));
    rge_write(outfile, &this_->hotkey_id, sizeof(this_->hotkey_id));
    rge_write(outfile, &this_->recyclable, sizeof(this_->recyclable));
    rge_write(outfile, &this_->track_as_resource, sizeof(this_->track_as_resource));
    rge_write(outfile, &this_->create_doppleganger_on_death, sizeof(this_->create_doppleganger_on_death));
    rge_write(outfile, &this_->resource_group, sizeof(this_->resource_group));
    rge_write(outfile, &this_->occlusion_flag, sizeof(this_->occlusion_flag));
    rge_write(outfile, &this_->obstruction_type, sizeof(this_->obstruction_type));
    rge_write(outfile, &this_->obstruction_class, sizeof(this_->obstruction_class));
    rge_write(outfile, &this_->object_flags, sizeof(this_->object_flags));
    rge_write(outfile, &this_->draw_flag, sizeof(this_->draw_flag));
    rge_write(outfile, &this_->draw_color, sizeof(this_->draw_color));
    rge_write(outfile, &this_->outline_radius_x, sizeof(this_->outline_radius_x));
    rge_write(outfile, &this_->outline_radius_y, sizeof(this_->outline_radius_y));
    rge_write(outfile, &this_->outline_radius_z, sizeof(this_->outline_radius_z));
    for (int i = 0; i < 3; ++i)
    {
        rge_write(outfile, &this_->attribute_type_held[i], sizeof(this_->attribute_type_held[i]));
        rge_write(outfile, &this_->attribute_amount_held[i], sizeof(this_->attribute_amount_held[i]));
        rge_write(outfile, &this_->attribute_flag[i], sizeof(this_->attribute_flag[i]));
    }
    rge_write(outfile, &this_->damage_sprite_num, sizeof(this_->damage_sprite_num));
    if (this_->damage_sprite_num)
    {
        for (int i = 0; i < this_->damage_sprite_num; ++i)
        {
            RGE_Sprite* dmg_sprite = this_->damage_sprites[i].sprite;
            if (dmg_sprite)
                rge_write(outfile, &dmg_sprite->id, sizeof(dmg_sprite->id));
            else
            {
                __int16 missing = -1;
                rge_write(outfile, &missing, sizeof(missing));
            }
            // 2-byte write deliberately spans damage_percent + flag (adjacent fields)
            rge_write(outfile, &this_->damage_sprites[i].damage_percent,
                      sizeof(this_->damage_sprites[i].damage_percent) + sizeof(this_->damage_sprites[i].flag));
            rge_write(outfile, &this_->damage_sprites[i].flag, sizeof(this_->damage_sprites[i].flag));
        }
    }
    rge_write(outfile, &selected_sound_id, sizeof(selected_sound_id));
    rge_write(outfile, &death_sound_id, sizeof(death_sound_id));
    rge_write(outfile, &this_->attack_reaction, sizeof(this_->attack_reaction));
    rge_write(outfile, &this_->convert_terrain_flag, sizeof(this_->convert_terrain_flag));
    if ((__int16)name_len > 0)
        rge_write(outfile, this_->name, (__int16)name_len);          // runtime byte count (name incl. NUL)
    rge_write(outfile, &gbg_name2_len, sizeof(gbg_name2_len));
    if ((__int16)gbg_name2_len > 0)
        rge_write(outfile, this_->gbg_name2, (__int16)gbg_name2_len); // runtime byte count
    rge_write(outfile, &this_->unit_line, sizeof(this_->unit_line));
    rge_write(outfile, &this_->min_tech_level, sizeof(this_->min_tech_level));
    rge_write(outfile, &this_->copy_id, sizeof(this_->copy_id));
    rge_write(outfile, &this_->base_id, sizeof(this_->base_id));
}

// Replacement for RGE_Master_Static_Object::get_help_message
// Original: RGE_Master_Static_Object__get_help_message @ 0x0048EF30 (idb_functions.h:1929)
// ABI: __thiscall — this_ in ECX, no stack arguments, int in EAX. No EDX dummy.
int __fastcall RGE_Master_Static_Object__get_help_message_new(RGE_Master_Static_Object* this_)
{
    return this_->help_string_id;
}

// Replacement for RGE_Master_Static_Object::get_help_page
// Original: RGE_Master_Static_Object__get_help_page @ 0x0048EF40 (idb_functions.h:1930)
// ABI: __thiscall — this_ in ECX, no stack arguments, int in EAX. No EDX dummy.
int __fastcall RGE_Master_Static_Object__get_help_page_new(RGE_Master_Static_Object* this_)
{
    return this_->help_page_id;
}

// Replacement for RGE_Master_Static_Object::make_available
// Original: RGE_Master_Static_Object__make_available @ 0x0048EA40 (idb_functions.h:1925)
// ABI: __thiscall — this_ in ECX, on_off_flag first stack argument, void.
void __fastcall RGE_Master_Static_Object__make_available_new(RGE_Master_Static_Object* this_, DWORD dummy, unsigned __int8 on_off_flag)
{
    if (!this_->disabled)
        this_->available = on_off_flag;
}

// Replacement for RGE_Master_Static_Object::make_new_master
// Original: RGE_Master_Static_Object__make_new_master @ 0x0048D330 (idb_functions.h:1916)
// ABI: __thiscall — this_ in ECX, no stack arguments, RGE_Master_Static_Object* in EAX.
// sizeof(RGE_Master_Static_Object) = 208 = 0xD0 (mechanically verified from
// idb_types.h:1205-1281 — matches operator_new(0xD0); field offsets corroborate
// all asm accesses: hp@50, damage_sprites@164, attribute_max_amount@140...).
// Allocation via operator_new_internal — verified same target as the binary's
// operator_new (0x00632B9D, common.cpp:323-324). Forwards to the original setup.
// EXCEPTION FOLD: the original registers frame SEH (stru_680E20, catch-all)
// whose unwind proc (unknown_libname_177 @0x6423B0) frees the allocation and
// propagates — folded as the __except filter side effect; the except body is
// unreachable.
RGE_Master_Static_Object* __fastcall RGE_Master_Static_Object__make_new_master_new(RGE_Master_Static_Object* this_)
{
    RGE_Master_Static_Object* new_master = 0;
    __try
    {
        new_master = (RGE_Master_Static_Object*)operator_new_internal(sizeof(RGE_Master_Static_Object));
        if (new_master)
        {
            new_master->vfptr = rge_master_static_object_vftable;
            RGE_Master_Static_Object__setup(new_master, this_);
            return new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
        return 0;  // unreachable — CONTINUE_SEARCH propagates the exception
    }
    return 0;
}

// Replacement for RGE_Master_Static_Object::make_new_obj
// Original: RGE_Master_Static_Object__make_new_obj @ 0x0048D260 (idb_functions.h:1915)
// ABI: __thiscall — this_ in ECX, owner/x/y/z stack arguments, retn 10h.
// Grounding: RGE_Game_World__recycle_object_in_to_game (idb_functions.h:6092,
// 0x006241E0, __thiscall(world, u8 object_type)); recycle_in_to_game = vtable
// slot 29 ([edx+74h], RGE_Static_ObjectVtbl @3960); operator_new(0xB4) =
// sizeof(RGE_Static_Object) = 180 (mechanically verified); ctor =
// idb_functions.h:3774 (0x0054B2E0). EXCEPTION FOLD: original registers frame
// SEH (stru_680DF8, catch-all; unwind proc loc_642390 frees the allocation and
// propagates) — folded as the __except filter side effect.
RGE_Static_Object* __fastcall RGE_Master_Static_Object__make_new_obj_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    RGE_Static_Object* new_obj = 0;
    __try
    {
        if (this_->recyclable)
        {
            RGE_Static_Object* recycled = RGE_Game_World__recycle_object_in_to_game(owner->world, this_->master_type);
            if (recycled)
            {
                recycled->vfptr->recycle_in_to_game(recycled, this_, owner, x, y, z);
                return recycled;
            }
        }
        new_obj = (RGE_Static_Object*)operator_new_internal(sizeof(RGE_Static_Object));
        if (new_obj)
            result = RGE_Static_Object__RGE_Static_Object(new_obj, this_, owner, x, y, z, 1);
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
        return 0;  // unreachable — CONTINUE_SEARCH propagates the exception
    }
    return result;
}

// Replacement for RGE_Master_Static_Object::calc_base_damage_ability
// Original: RGE_Master_Static_Object__calc_base_damage_ability @ 0x0048EF50
// (idb_functions.h:1931). ABI: __thiscall — this_ in ECX (unused by the body),
// attacker first stack argument, retn 4, int in EAX. No EDX dummy.
// Sums positive weapon values from attacker->weapon (RGE_Armor_Weapon_Info:
// { __int16 type; __int16 value; } — idb_types.h:7412 — value @+2, element
// size 4), weapon_num entries (hoisted once, as in the original).
// asm nuance: the add-gate tests >= 0.0 — numerically identical to the
// pseudocode's > 0.0 (adding +0.0 is a no-op). Return conversion is __ftol
// (int) per the template rule, not (__int64).
int __fastcall RGE_Master_Static_Object__calc_base_damage_ability_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Master_Combat_Object* attacker)
{
    double sum = 0.0;
    const int count = attacker->weapon_num;
    if (attacker->weapon && count > 0)
    {
        for (int i = 0; i < count; ++i)
        {
            const double v = (double)attacker->weapon[i].value;
            if (v > 0.0)
                sum = sum + v;
        }
    }
    if (sum < 1.0)
        sum = 1.0;
    return (int)sum;
}

// Replacement for RGE_Master_Static_Object::alignment
// Original: RGE_Master_Static_Object__alignment @ 0x0048EA60 (idb_functions.h:1926)
// ABI: __thiscall — this_ in ECX, col/row/world/center_flag stack args,
// retn 10h, u8 result in AL. No EDX dummy.
// Grounding: world->map = RGE_Game_World::map @+52; map_width/map_height =
// int @+8/+12 in RGE_Map (idb_types.h:2892-2916, read via fild int→double);
// radius_x/y = float @+60/+64; epsilon = binary float constant 9.9999997e-6
// (flt_65B48C). RESTRUCTURE (owner-directed): the x87 flag-driven rounding
// decoded from asm — radius-integrality selects half-up rounding (bump at
// >= 0.5; the pseudocode's > 0.5 differs at exactly 0.5 — asm is ground
// truth) vs tile-center shift; row bumps past a tile edge return early.
unsigned __int8 __fastcall RGE_Master_Static_Object__alignment_new(RGE_Master_Static_Object* this_, DWORD dummy, float* col, float* row, RGE_Game_World* world, unsigned __int8 center_flag)
{
    RGE_Map* map = world->map;
    const float left = *col - this_->radius_x;
    float top = *row - this_->radius_y;
    float right = *col + this_->radius_x;
    double bottom = this_->radius_y + *row;
    const double map_max_w = (double)map->map_width - 9.9999997e-6f;
    const double map_max_h = (double)map->map_height - 9.9999997e-6f;

    // clamp left/top: negative edges pull position and right/bottom edges back
    if (left < 0.0)
    {
        right -= left;
        *col -= left;
    }
    if (top < 0.0)
    {
        bottom -= top;
        *row -= top;
    }
    // clamp right/bottom against the epsilon-reduced map bounds
    if (right >= map_max_w)
        *col = map_max_w - right + *col;
    if (bottom >= map_max_h)
        *row = map_max_h - bottom + *row;

    // RESTRUCTURE: rounding mode selected by radius integrality
    // (frac == 0 ⟺ radius == trunc16(radius); asm FPU C3 flag)
    if (center_flag)
    {
        if (this_->radius_x == (double)(__int16)(int)this_->radius_x)
        {
            double rounded = (double)(__int16)(int)*col;
            if (*col - rounded >= 0.5)                        // asm bumps at >= 0.5
                rounded += 1.0;
            *col = rounded;
        }
        else
        {
            *col = (double)(__int16)(int)*col + 0.5;          // tile-center shift
        }

        if (this_->radius_y == (double)(__int16)(int)this_->radius_y)
        {
            double rounded = (double)(__int16)(int)*row;
            if (*row - rounded >= 0.5)
            {
                *row = rounded + 1.0;
                return 1;                                     // row crossed a tile edge
            }
            *row = rounded;
        }
        else
        {
            *row = (double)(__int16)(int)*row + 0.5;
        }
    }
    return 1;
}

// Replacement for RGE_Master_Static_Object::alignment_box
// Original: RGE_Master_Static_Object__alignment_box @ 0x0048EC10 (idb_functions.h:1927)
// ABI: __thiscall — this_ in ECX, 9 stack args, retn 24h, u8 1 in AL. No EDX dummy.
// tile_half_height/width: __int16 in RGE_Map (idb_types.h:2915-2916, asm
// [map+0xBF3E]/[map+0xBF40], movsx → double). All (__int64) casts = __ftol
// (int) + i16 narrow store per the template rule. Callers: unit/wall outline
// drawing (lst XREFs).
unsigned __int8 __fastcall RGE_Master_Static_Object__alignment_box_new(RGE_Master_Static_Object* this_, DWORD dummy, RGE_Game_World* world, float col, float row, __int16* x1, __int16* y1, __int16* x2, __int16* y2, __int16* x3, __int16* y3, __int16* x4, __int16* y4)
{
    float cola = col - (double)(__int16)(int)col + this_->radius_x;   // tile-floor + radius
    RGE_Map* map = world->map;
    float rowa = row - (double)(__int16)(int)row + this_->radius_y;
    const double tile_half_width = (double)map->tile_half_width;
    const float rowa_plus_1 = rowa + 1.0f;
    const double tile_half_height = (double)map->tile_half_height;
    *x1 = (__int16)(int)((rowa + cola) * tile_half_width);
    *y1 = (__int16)(int)((rowa_plus_1 - cola) * tile_half_height);
    const float colb = cola - (this_->radius_x + this_->radius_x);
    *x2 = (__int16)(int)((rowa + colb) * tile_half_width);
    *y2 = (__int16)(int)((rowa_plus_1 - colb) * tile_half_height);
    const float rowb = rowa - (this_->radius_y + this_->radius_y);
    const float rowb_plus_1 = rowb + 1.0f;
    *x3 = (__int16)(int)((rowb + colb) * tile_half_width);
    *y3 = (__int16)(int)((rowb + 1.0f - colb) * tile_half_height);
    const float colc = this_->radius_x + this_->radius_x + colb;
    *x4 = (__int16)(int)((rowb + colc) * tile_half_width);
    *y4 = (__int16)(int)((rowb_plus_1 - colc) * tile_half_height);
    return 1;
}

void install_RGE_Master_Static_Object()
{
    setHook((void*)0x0048B8B0, RGE_Master_Static_Object__get_sprite_new);
    setHook((void*)0x0048B920, RGE_Master_Static_Object__get_sprite2_new);
    setHook((void*)0x0048B990, RGE_Master_Static_Object__get_death_sprite_new);
    setHook((void*)0x0048BA00, RGE_Master_Static_Object__get_undead_sprite_new);
    setHook((void*)0x0048BA70, RGE_Master_Static_Object__is_unit_in_group_new);
    setHook((void*)0x0048BAA0, RGE_Master_Static_Object__is_unit_in_civ_new);
    setHook((void*)0x0048BAD0, RGE_Master_Static_Object__get_unit_civ_new);
    setHook((void*)0x0048BAE0, RGE_Master_Static_Object__get_string_id_new);
    setHook((void*)0x0048BBE0, RGE_Master_Static_Object__get_string_id2_new);
    setHook((void*)0x0048BCE0, RGE_Master_Static_Object__do_i_care_about_power_new);
    setHook((void*)0x0048BD80, RGE_Master_Static_Object__RGE_Master_Static_Object_new);
    setHook((void*)0x0048BDB0, RGE_Master_Static_Object__scalar_deleting_destructor_new);
    setHook((void*)0x0048BE20, RGE_Master_Static_Object__RGE_Master_Static_Object2_new);
    setHook((void*)0x0048BE50, RGE_Master_Static_Object__RGE_Master_Static_Object3_new);
    setHook((void*)0x0048BE90, RGE_Master_Static_Object__RGE_Master_Static_Object4_new);
    setHook((void*)0x0048D200, RGE_Master_Static_Object__destructor_new);
    setHook((void*)0x0048D260, RGE_Master_Static_Object__make_new_obj_new);
    setHook((void*)0x0048D330, RGE_Master_Static_Object__make_new_master_new);
    setHook((void*)0x0048D390, RGE_Master_Static_Object__copy_obj_new);
    setHook((void*)0x0048D510, RGE_Master_Static_Object__copy_vars_new);
    setHook((void*)0x0048D800, RGE_Master_Static_Object__modify_new);
    setHook((void*)0x0048D8D0, RGE_Master_Static_Object__modify_delta_new);
    setHook((void*)0x0048D9C0, RGE_Master_Static_Object__modify_percent_new);
    setHook((void*)0x0048DAE0, RGE_Master_Static_Object__small_save_new);
    setHook((void*)0x0048DBC0, RGE_Master_Static_Object__save_new);
    setHook((void*)0x0048EA40, RGE_Master_Static_Object__make_available_new);
    setHook((void*)0x0048EA60, RGE_Master_Static_Object__alignment_new);
    setHook((void*)0x0048EC10, RGE_Master_Static_Object__alignment_box_new);
    setHook((void*)0x0048EF30, RGE_Master_Static_Object__get_help_message_new);
    setHook((void*)0x0048EF40, RGE_Master_Static_Object__get_help_page_new);
    setHook((void*)0x0048EF50, RGE_Master_Static_Object__calc_base_damage_ability_new);
}
