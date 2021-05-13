#pragma once

struct condition
{
    void*    vfptr;
    int        id;
    int        unk1;
    int        quantity;
    int        attrib_list;
    int        obj;
    int        nextobj;
    int        obj_list_type;
    int        player;
    int        tech;
    int        timer;
    int        trigger;
    int        area_x1;
    int        area_y1;
    int        area_x2;
    int        area_y2;
    int        obj_group;
    int        obj_type;
    int        ai_signal;
};

struct effect
{
    void*    vfptr;
    int        id;
    int        unk1;
    int        ai_trigger_number;
    int        quantity;
    int        attrib_list;
    int        alliance;
    int        obj;
    int        obj2;
    int        obj_list;
    int        source_player;
    int        target_player;
    int        tech;
    int        message;
    int        sound;
    int        timer;
    int        trigger;
    int        location_x;
    int        location_y;
    int        area_x1;
    int        area_y1;
    int        area_x2;
    int        area_y2;
    int        obj_group;
    int        obj_type;
    int        number;
    int        unk2;
    char*    str;
};

struct prop_object
{
    void*    _vfptr;
    BYTE    type;
    int        unk008;
    int        unk00C;
    short    Unitline;
    char    MinTechLevel;
    short    LanguageDLLName;
    short    LanguageDLLCreation;
    short    id1;
    short    id2;
    short    id3;
    short    unit_class;
    void*    standing_graphics;
    int        unk024;
    void*    dying_graphics;
    int        unk02C;
    char    death_mode;
    short    hit_points;
    float    line_of_sight;
    char    garrison_capacity;
    float    collision_size_x;
    float    collision_size_y;
    float    collision_size_z;
    int        unk048;
    int        unk04C;
    int        unk050;
    int        unk054;
    short    dead_unit_id;
    char    placement_mode;
    char    air_mode;
    short    icon_id;
    short    hide_in_editor;
    short    unk060;
    char    unk062;
    char    unk063;
    short    placement_side_terrain_1;
    short    placement_side_terrain_2;
    short    placement_terrain_1;
    short    placement_terrain_2;
    float    clearance_size_x;
    float    clearance_size_y;
    char    hill_mode;
    char    fog_visibility;
    short    terrain_restriction;
    char    fly_mode;
    short    resource_storage_type_1;
    short    resource_storage_type_2;
    short    resource_storage_type_3;
    float    resource_1_storage;
    float    resource_2_storage;
    float    resource_3_storage;
    short    resource_capacity;
    float    resource_decay; //ok
    float    unknown3A;
    char    unk098;
    char    unk099;
    char    unk09A;
    char    blast_defense_level;
    char    unknown2int_mode_minimap_mode;
    char    interaction_mode;
    char    minimap_mode;
    char    command_id;
    char    minimap_color;
    char    attack_mode;
    char    unknown2;
    int        unknown0A4;
    char    selection_mask;
    char    selection_shape_type;
    char    selection_shape;
    char    padding0AB;
    char    unit_attribute;
    char    civilization;
    short    attribute_leftover;
    int        language_dll_help;
    int        language_dll_hotkey_text;
    int        language_dll_hotkey;
    char    unselectable;
    char    enable_auto_gather;
    char    auto_gather_mode;
    char    auto_gather_id;
    char    selection_effect;
    char    selection_color;
    float    selection_shape_size_x;
    float    selection_shape_size_y;
    float    selection_shape_size_z;
    //type20+
    float    speed;
    //type30+
    void*    walking_graphics;
    int        walking_graphics_2;
    float    rotation_speed;
    char    unk0E0;
    short    tracking_unit;
    char    tracking_unit_used;
    float    tracking_unit_density;
    char    unk0EC;
    float    rotations_in_radians_1;
    float    rotations_in_radians_2;
    float    rotations_in_radians_3;
    float    rotations_in_radians_4;
    float    rotations_in_radians_5;
    //type40+
    int        unk104;
    short    action_when_discovered;
    float    search_radius;
    float    work_rate_1;
    float    work_rate_2;
    short    drop_site_1;
    short    drop_size_2;
    char    task_swap_id;
    void*    attack_sound;
    void*    move_sound;
    char    animal_mode;
    //type50+
    void*    attack_graphics;
    short    default_armor;
    short    armors_count;
    void*    armors_ptr;
    short    attacks_count;
    void*    attacks_ptr;
    short    unk140;
    float    range;
    float    blast_radius;
    char    blast_level;
    float    reload_time_1;
    short    projectile_unit_id;
    short    accuracy;
    char    tower_mode;
    short    frame_delay;
    float    projectile_displacement_x;
    float    projectile_displacement_y;
    float    projectile_displacement_z;
    float    min_range;
    short    displayed_melee_armor;
    short    displayed_attack;
    float    displayed_range;
    float    reload_time_2;
    float    accuracy_error_radius;
    //type70+
    short    cost_1_type;
    short    cost_1_amount;
    short    cost_1_is_paid;
    short    cost_2_type;
    short    cost_2_amount;
    short    cost_2_is_paid;
    short    cost_3_type;
    short    cost_3_amount;
    short    cost_3_is_paid;
    short    train_time;
    short    train_location;
    char    train_button;
    short    displayed_pierce_armor;
    int        unk198;
    int        unk19C;
    char    creatable_type;
    float    min_dupl_missiles;
    char    max_dupl_missiles;
    float    missile_spawning_area_width;
    float    missile_spawning_area_length;
    float    missile_spawning_area_randomness;
    short    missile_dupl_unit;
    void*    special_graphics;
    char    special_ability;
    char    hero_mode;

    //TODO: fill
};

struct vector3
{
    float x;
    float y;
    float z;
};

struct UNIT
{
    void* _vfptr;        //0
    int unk04;
    int unk08;
    int unk0C;
    int ordinal;        //10
    prop_object* prop_object;
    void* player;
    int unk1C;
    int unk20;            //20
    void* graphics_object;
    int unk28;
    int unk2C;
    int unk30;            //30
    int unk34;
    int unk38;
    float hp;
    float sp;            //40
    char unk44;
    char rotation;
    float x;
    float y;
    int unk50;            //50
    float resources;
    int unk58;
    int unk5C;
    int unk60;            //60
    int unk64;
    int unk68;
    int unk6C;
    int unk70;            //70
    int unk74;
    int unk78;
    int unk7C;
    int unk80;            //80
    int unk84;
    int unk88;
    int unk8C;
    int unk90;            //90
    int unk94;
    int unk98;
    int unk9C;
    int unkA0;            //A0
    int unkA4;
    int unkA8;
    int unkAC;
    int unkB0;            //B0
    int unkB4;
    int unkB8;
    int unkBC;
    int unkC0;            //C0
    int unkC4;
    int unkC8;
    int unkCC;
    int unkD0;            //D0
    int unkD4;
    int unkD8;
    int unkDC;
    int unkE0;            //E0
    int unkE4;
    int unkE8;
    int unkEC;
    int unkF0;            //F0
    int unkF4;
    int unkF8;
    int unkFC;
    int unk100;            //100
    int unk104;
    int unk108;
    int unk10C;
    int unk110;            //110
    int unk114;
    int unk118;
    int unk11C;
    vector3* unk120;    //120
    //TODO: fill

    __declspec(noinline) float __thiscall unit_distance_to_position_new(float x_to, float y_to, float z_to)
    {
        float x_coll; // st7@1
        float y_coll; // st6@1
        float result; // st7@9
        float xd; // [sp+4h] [bp+4h]@1
        float yd; // [sp+4h] [bp+4h]@5

        x_coll = x - x_to;
        y_coll = y - y_to;
        if (x_coll < 0.0)
            x_coll = -x_coll;
        if (y_coll < 0.0)
            y_coll = -y_coll;
        xd = x_coll - prop_object->collision_size_x;
        yd = y_coll - prop_object->collision_size_y;
        if (xd < 0.0)
            xd = 0.0;
        if (yd < 0.0)
            yd = 0.0;
        result = sqrt(xd * xd + yd * yd);
        if (result < 0.0)
            result = 0.0;

        //cntrd++;
        //if (cntrd % 100000 == 0)
        //    chat("Called 100000 times since last msg, total = %d", cntrd);

        return result;
    }

    __declspec(noinline) int __thiscall unit_unknown_moving_update_end()
    {
        return unk120 && sqrt(unk120->x*unk120->x + unk120->y*unk120->y + unk120->z*unk120->z) > 0.0;
    }
};

struct trigger
{
    int enabled;        //0
    int unk04;
    int unk08;
    char* desc;
    int unk10;            //10
    char* name;
    condition** conditions;
    int unk1C;
    int n_conditions;    //20
    int unk24;
    effect** effects;
    int unk2C;
    int n_effects;        //30
    int unk34;
    int unk38;
    int activated_at;
};
