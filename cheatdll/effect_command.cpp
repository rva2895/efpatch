#include "stdafx.h"
#include "effect_command.h"

const char* __stdcall get_command_name(int c)
{
    switch (c)
    {
    case RGE_COMMAND_ORDER:
        return "Order";
    case RGE_COMMAND_STOP:
        return "Stop";
    case RGE_COMMAND_AI_ORDER:
        return "AI Order";
    case RGE_COMMAND_MOVE:
        return "Move";
    case RGE_COMMAND_AI_MOVE:
        return "AI Move";
    case RGE_COMMAND_RESIGN:
        return "Resign";
    case RGE_COMMAND_WAYPOINT:
        return "Waypoint";
    case RGE_COMMAND_GROUP_WAYPOINT:
        return "Group Waypoint";
    case RGE_COMMAND_GROUP_AI_ORDER:
        return "Group AI Order";
    case RGE_COMMAND_STANCE:
        return "Unit AI State";
    case RGE_COMMAND_GUARD:
        return "Guard";
    case RGE_COMMAND_FOLLOW:
        return "Follow";
    case RGE_COMMAND_PATROL:
        return "Patrol";
    case RGE_COMMAND_SCOUT:
        return "Scout";
    case RGE_COMMAND_FORMATION:
        return "Formation";
    case RGE_COMMAND_BREAK_FORMATION:
        return "Break Formation";
    case RGE_COMMAND_WHEEL_FORMATION:
        return "Wheel Formation";
    case RGE_COMMAND_ABOUT_FACE_FORMATION:
        return "About Face Formation";
    case RGE_COMMAND_MULTIPLAYER_SAVE:
        return "Multiplayer Save";
    case RGE_COMMAND_FORMATION_PARAMETERS:
        return "Formation Parameters";
    case RGE_COMMAND_AUTO_FORMATION:
        return "Auto Formation";
    case RGE_COMMAND_LOCK_FORMATION:
        return "Lock Formation";
    case RGE_COMMAND_GROUP_MULTI_WAYPOINTS:
        return "Group Multi Waypoints";
    case RGE_COMMAND_CHAPTER:
        return "Chapter";

    case TRIBE_COMMAND_MAKE:
        return "Make";
    case TRIBE_COMMAND_RESEARCH:
        return "Research";
    case TRIBE_COMMAND_FOUNDATION:
        return "Foundation";
    case TRIBE_COMMAND_GAME:
        return "Game";
    case TRIBE_COMMAND_WALL:
        return "Wall";
    case TRIBE_COMMAND_DELETE:
        return "Delete";
    case TRIBE_COMMAND_ATTACK_GROUND:
        return "Attack Ground";
    case TRIBE_COMMAND_TRIBUTE:
        return "Tribute";
    case TRIBE_COMMAND_REPAIR:
        return "Repair";
    case TRIBE_COMMAND_UNLOAD:
        return "Unload";
    case TRIBE_COMMAND_CONVERT:
        return "Convert";
    case TRIBE_COMMAND_GATE:
        return "Gate";
    case TRIBE_COMMAND_FLARE:
        return "Flare";
    case TRIBE_COMMAND_LOAD:
        return "Load";
    case TRIBE_COMMAND_DIPLOMACY:
        return "Diplomacy";
    case TRIBE_COMMAND_TRAIN:
        return "Train";
    case TRIBE_COMMAND_GATHER_POINT:
        return "Gather Point";
    case TRIBE_COMMAND_RETREAT_POINT:
        return "Retreat Point";
    case TRIBE_COMMAND_SELL:
        return "Sell";
    case TRIBE_COMMAND_BUY:
        return "Buy";
    case TRIBE_COMMAND_OFFBOARD_TRADE:
        return "Offboard Trade";
    case TRIBE_COMMAND_TRANSFORM:
        return "Transform";
    case TRIBE_COMMAND_DROP_RELIC:
        return "Drop Holocron";
    case TRIBE_COMMAND_TOWNBELL:
        return "Town Center Alarm";
    case TRIBE_COMMAND_BACK_TO_WORK:
        return "Back to Work";
    default:
        return "Unknown";
    }
}

class MEMORY_STREAM
{
private:
    uint8_t* memory;
    uint8_t* ptr;
    size_t size;
public:
    explicit MEMORY_STREAM()
    {
        /*size = 0x100;
        memory = (uint8_t*)malloc(size);
        ptr = memory;
        */
        memory = NULL;
        ptr = NULL;
        size = 0;
    };
    ~MEMORY_STREAM()
    {
        free(memory);
    };
    template<class T>
    void add(T x)
    {
        /*if (size - (ptr - memory) < sizeof(T))
        {
            size *= 2;
            size_t diff = ptr - memory;
            memory = (uint8_t*)realloc(memory, size);
            ptr = memory + diff;
        }*/
        memcpy(ptr, &x, sizeof(T));
        ptr += sizeof(T);
    }
    void* get_memory()
    {
        return memory;
    };
    void alloc(size_t new_size)
    {
        size = new_size;
        memory = (uint8_t*)malloc(size);
        ptr = memory;
    }
};

uint8_t get_player_number(void* player)
{
    return *(uint32_t*)((uint8_t*)player + 0xA0);
}

void set_target_obj(void* order, RGE_Static_Object* object)
{
    *((int*)order + 1) = object->id;
}

void set_count(void* order, int count)
{
    *((BYTE*)order + 1) = (BYTE)count;
}

void set_type(void* order, BYTE type)
{
    *((BYTE*)order) = type;
}

void set_objects(void* order, RGE_Static_Object** objects, int count)
{
    int* p = (int*)order + 2;
    for (int i = 0; i < count; i++)
        *p++ = objects[i]->id;
}

void create_command_stop(RGE_Static_Object** objects, int count, MEMORY_STREAM& m)
{
    size_t size = sizeof(uint8_t) * 2 + sizeof(int) * count;
    m.alloc(size);
    m.add((uint8_t)RGE_COMMAND_STOP);
    m.add((uint8_t)count);
    for (int i = 0; i < count; i++)
        m.add((int)objects[i]->id);
}

void create_command_guard(RGE_Static_Object** objects, int count, RGE_Static_Object* target, MEMORY_STREAM& m)
{
    size_t size = sizeof(uint8_t) * 2 + sizeof(uint16_t) + sizeof(int) * count;
    m.alloc(size);
    m.add((uint8_t)RGE_COMMAND_GUARD);
    m.add((uint8_t)count);
    m.add((uint16_t)0);
    m.add((int)target->id);
    for (int i = 0; i < count; i++)
        m.add((int)objects[i]->id);
}

void create_command_follow(RGE_Static_Object** objects, int count, RGE_Static_Object* target, MEMORY_STREAM& m)
{
    size_t size = sizeof(uint8_t) * 2 + sizeof(uint16_t) + sizeof(int) * count;
    m.alloc(size);
    m.add((uint8_t)RGE_COMMAND_FOLLOW);
    m.add((uint8_t)count);
    m.add((uint16_t)0);
    m.add((int)target->id);
    for (int i = 0; i < count; i++)
        m.add((int)objects[i]->id);
}

void create_command_formation(RGE_Static_Object** objects, int count, RGE_Player* player, int type, MEMORY_STREAM& m)
{
    size_t size = sizeof(uint8_t) * 2 + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(int) * count;
    m.alloc(size);
    m.add((uint8_t)RGE_COMMAND_FORMATION);
    m.add((uint8_t)count);
    m.add((uint16_t)get_player_number(player));
    m.add((uint32_t)type);
    for (int i = 0; i < count; i++)
        m.add((int)objects[i]->id);
}

void create_command_research(RGE_Player* player, int tech, RGE_Static_Object* target, MEMORY_STREAM& m)
{
    size_t size = sizeof(uint32_t) * 3 + sizeof(uint16_t) + sizeof(int16_t);
    m.alloc(size);
    m.add((uint32_t)TRIBE_COMMAND_RESEARCH);
    m.add((uint32_t)target->id);
    m.add((uint16_t)get_player_number(player));
    m.add((int16_t)tech);
    m.add((uint32_t)0xFFFFFFFF);
}

void create_command_attack_ground(RGE_Static_Object** objects, int count, float x, float y, MEMORY_STREAM& m)
{
    size_t size = sizeof(uint8_t) * 2 + sizeof(uint16_t) + sizeof(int) * count + sizeof(float) * 2;
    m.alloc(size);
    m.add((uint8_t)TRIBE_COMMAND_ATTACK_GROUND);
    m.add((uint8_t)count);
    m.add((uint16_t)0);
    m.add((float)x);
    m.add((float)y);
    for (int i = 0; i < count; i++)
        m.add((int)objects[i]->id);
}

void __declspec(nothrow) __stdcall do_effect_command(int qty, int command, RGE_Player* player, RGE_Static_Object** objects, int count, RGE_Static_Object* target_obj, int x, int y)
{
    float fx;
    float fy;
    if (target_obj)
    {
        fx = target_obj->world_x;
        fy = target_obj->world_y;
    }
    else
    {
        fx = (float)x + 0.5f;
        fy = (float)y + 0.5f;
    }

    void* tribe_command = get_TRIBE_Command();

    MEMORY_STREAM m;

    switch (command)
    {
    case RGE_COMMAND_STOP:
        create_command_stop(objects, count, m);
        break;
    case RGE_COMMAND_GUARD:
        create_command_guard(objects, count, target_obj, m);
        break;
    case RGE_COMMAND_FOLLOW:
        create_command_follow(objects, count, target_obj, m);
        break;
    case RGE_COMMAND_FORMATION:
        create_command_formation(objects, count, player, qty, m);
        break;
    case TRIBE_COMMAND_RESEARCH:
        create_command_research(player, qty, target_obj, m);
        break;
    case TRIBE_COMMAND_ATTACK_GROUND:
        create_command_attack_ground(objects, count, fx, fy, m);
        break;
    default:
        return;
    }

    TRIBE_Command__do_command((TRIBE_Command*)tribe_command, m.get_memory());
}
