#pragma once

struct condition
{
    void*       vfptr;
    int         id;
    int         unk1;
    int         quantity;
    int         attrib_list;
    int         obj;
    int         nextobj;
    int         obj_list_type;
    int         player;
    int         tech;
    int         timer;
    int         trigger;
    int         area_x1;
    int         area_y1;
    int         area_x2;
    int         area_y2;
    int         obj_group;
    int         obj_type;
    int         ai_signal;
};

struct effect
{
    void*       vfptr;
    int         id;
    int         unk1;
    int         ai_trigger_number;
    int         quantity;
    int         attrib_list;
    int         alliance;
    int         obj;
    int         obj2;
    int         obj_list;
    int         source_player;
    int         target_player;
    int         tech;
    int         message;
    int         sound;
    int         timer;
    int         trigger;
    int         location_x;
    int         location_y;
    int         area_x1;
    int         area_y1;
    int         area_x2;
    int         area_y2;
    int         obj_group;
    int         obj_type;
    int         number;
    int         unk2;
    char*       str;
};

struct trigger
{
    int         enabled;        //0
    int         unk04;
    int         unk08;
    char*       desc;
    int         unk10;            //10
    char*       name;
    condition** conditions;
    int         unk1C;
    int         n_conditions;    //20
    int         unk24;
    effect**    effects;
    int         unk2C;
    int         n_effects;        //30
    int         unk34;
    int         unk38;
    int         activated_at;
};
