#include "stdafx.h"

#include "conditions.h"
#include "triggerdesc.h"
#include "casts.h"
#include "autosave.h"

#define NEW_COND 7

#define ALLIANCE_ALLY 0
#define ALLIANCE_NEUTRAL 1
#define ALLIANCE_ENEMY 3

void conditionPerMilleChance();
void conditionAreaExplored();
void conditionAlliance();
void conditionVariable();
void conditionPlayerCiv();
void conditionOR();

const DWORD condJMPTable[] =
{
    0x5F1E53,
    0x5F1EA8,
    0x5F1F25,
    0x5F1F9B,
    0x5F2054,
    0x5F22C5,
    0x5F22E4,
    0x5F2312,
    0x5F2343,
    0x5F2374,
    0x5F2396,
    0x5F23DB,
    0x5F2411,
    0x5F2435,
    0x5F24DB,
    0x5F2540,
    0x5F25A5,
    0x5F25D2,
    0x5F25FE,
    0x5F1FE6,
    0x5F20F5,
    0x5F2227,
    0x5F1E25,
    (DWORD)conditionPerMilleChance,
    (DWORD)conditionAreaExplored,
    (DWORD)conditionAlliance,
    (DWORD)conditionVariable,
    (DWORD)conditionVariable,
    (DWORD)conditionPlayerCiv,
    (DWORD)conditionOR
};

__declspec(naked) void condNotMet()
{
    __asm
    {
        mov     ebx, 005F2626h
        //mov     ebx, 005F260Eh
        jmp     ebx
    }
}

__declspec(naked) void condMet()
{
    __asm
    {
        mov     ebx, 005F1E95h
        jmp     ebx
    }
}

__declspec(naked) void conditionPerMilleChance()
{
    __asm
    {
        call    rand_internal
        xor     edx, edx
        mov     edi, 3E8h
        idiv    edi
        mov     edi, [esi + 0Ch]
        cmp     edi, edx
        jg      cYes
        jmp     condNotMet
cYes:
        jmp     condMet
    }
}

#define NEW_COND_OFFSET_VAL (0x60 + 4 * NEW_COND)

__declspec(naked) void trigger_system_destroy_conditions() //005F5385
{
    __asm
    {
        add     esp, 4
        cmp     edi, NEW_COND_OFFSET_VAL
        mov     eax, 005F538Bh
        jmp     eax
    }
}

__declspec(naked) void trigger_system_setup_shared_info_1() //005F5500
{
    __asm
    {
        push    esi
        push    edi
        mov     esi, ecx
        push    NEW_COND_OFFSET_VAL
        mov     eax, 005F5506h
        jmp     eax
    }
}

__declspec(naked) void trigger_system_setup_shared_info_2() //005F5543
{
    __asm
    {
        add     edi, 4
        cmp     edi, NEW_COND_OFFSET_VAL
        mov     eax, 005F5549h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setConditionNumbers()
{
    setHook((void*)0x005F5385, trigger_system_destroy_conditions);
    setHook((void*)0x005F5500, trigger_system_setup_shared_info_1);
    setHook((void*)0x005F5543, trigger_system_setup_shared_info_2);

    writeByte(0x0053BD77, 0x18 + NEW_COND);

    writeByte(0x005F5565, 0x17 + NEW_COND);
    writeByte(0x005F554E, 0x17 + NEW_COND);

    writeDword(0x005F1E21, (DWORD)condJMPTable);
    writeByte(0x005F1E17, 0x16 + NEW_COND);
}
#pragma optimize( "", on )

__declspec(naked) void condParams() //005F5DD1
{
    __asm
    {
        mov     cl, 1
        mov     eax, [esi + 8]
        mov     edx, [eax + 60h]    //new condition 1
        mov     [edx + 0], cl

        mov     eax, [esi + 8]
        mov     edx, [eax + 64h]    //new condition 2
        mov     [edx + 0], cl
        mov     [edx + 5], cl
        mov     [edx + 9], cl
        mov     [edx + 0Ah], cl
        mov     [edx + 0Bh], cl
        mov     [edx + 0Ch], cl

        mov     eax, [esi + 8]
        mov     edx, [eax + 68h]    //new condition alliance state
        mov     [edx + 5], cl
        mov     [edx + 7], cl
        mov     [edx + 0Fh], cl

        mov     eax, [esi + 8]
        mov     edx, [eax + 6Ch]    //new condition var GE
        mov     cl, 2
        mov     [edx + 0], cl
        mov     [edx + 2], cl
        mov     [edx + 4], cl
        mov     [edx + 5], cl
        mov     [edx + 7], cl
        mov     [edx + 9], cl
        mov     [edx + 0Ah], cl
        mov     [edx + 0Bh], cl
        mov     [edx + 0Ch], cl
        mov     [edx + 0Dh], cl
        mov     [edx + 0Eh], cl
        mov     [edx + 0Fh], cl

        mov     eax, [esi + 8]
        mov     edx, [eax + 70h]    //new condition var E
        mov     [edx + 0], cl
        mov     [edx + 2], cl
        mov     [edx + 4], cl
        mov     [edx + 5], cl
        mov     [edx + 7], cl
        mov     [edx + 9], cl
        mov     [edx + 0Ah], cl
        mov     [edx + 0Bh], cl
        mov     [edx + 0Ch], cl
        mov     [edx + 0Dh], cl
        mov     [edx + 0Eh], cl
        mov     [edx + 0Fh], cl

        mov     eax, [esi + 8]
        mov     edx, [eax + 74h]    //new condition civ
        mov     [edx + 5], cl
        mov     [edx + 0Fh], cl

        mov     eax, [esi + 8]
        mov     edx, [eax + 14h]    //old condition objects in area
        mov     [edx + 0Fh], cl     //ai trigger
        mov     [edx + 9], cl       //area
        mov     [edx + 0Ah], cl
        mov     [edx + 0Bh], cl
        mov     [edx + 0Ch], cl

        mov     ecx, [esi + 0Ch]
        mov     edx, [ecx + 74h]
        mov     al, 2
        mov     ecx, 005F5DD7h
        jmp     ecx
    }
}

int invBtnPtr;

__declspec(naked) void funcCreateWrapper()
{
    __asm
    {
        mov     eax, [esp + 4]
        push    esi
        push    edi
        mov     edi, [esp + 10h]
        push    0
        push    1
        push    0
        push    0
        push    0
        push    0
        push    edi
        mov     esi, ecx
        push    eax
        mov     eax, 004b9520h
        call    eax
        test    eax, eax
        jnz     short loc_4ED558
        pop     edi
        pop     esi
        retn    8
        //---------------------------------------------------------------------------

loc_4ED558:                      //       ; CODE XREF: sub_4ED530+21j
        mov     ecx, [edi]
        push    0
        push    1
        mov     eax, 004B7450h
        call    eax
        mov     ecx, [edi]
        push    0
        mov     edx, [ecx]
        call    dword ptr [edx + 14h]
        mov     eax, [esi + 320h]
        test    eax, eax
        jz      short loc_4ED5B5
        xor     eax, eax
        xor     ecx, ecx
        mov     al, [esi + 32Dh]
        mov     cl, [esi + 32Ch]
        xor     edx, edx
        push    eax
        mov     dl, [esi + 32Bh]
        push    ecx
        xor     eax, eax
        push    edx
        mov     al, [esi + 32Ah]
        xor     ecx, ecx
        mov     cl, [esi + 329h]
        xor     edx, edx
        mov     dl, [esi + 328h]
        push    eax
        push    ecx
        mov     ecx, [edi]
        push    edx
        push    3
        mov     eax, 004C5190h
        call    eax

loc_4ED5B5:             //                ; CODE XREF: sub_4ED530+44j
        pop     edi
        mov     eax, 1
        pop     esi
        retn    8
    }
}

//constructor?
__declspec(naked) void inv1() //00529A06
{
    __asm
    {
        mov     ecx, esi
        mov     eax, 00531E80h
        call    eax
        mov     eax, [esi + 918h]
        lea     edi, invBtnPtr
        push    edi
        push    eax
        mov     ecx, esi
        call    funcCreateWrapper
        test    eax, eax
        jz      short loc_7E22C4
        mov     ecx, invBtnPtr
        push    12h
        push    12h
        push    2
        mov     edx, [ecx]
        push    0BAh
        call    dword ptr [edx + 28h]

loc_7E22C4:
        //
        call    editor_enter
        //
        mov     eax, 00529A0Dh
        jmp     eax
    }
}

//destructor?
__declspec(naked) void inv2() //0052ABD7
{
    __asm
    {
        lea     eax, invBtnPtr
        mov     ecx, esi
        push    eax
        mov     eax, 00428520h
        call    eax
        //
        call    editor_exit
        //
        lea     eax, [esi + 964h]
        mov     ecx, 0052ABDDh
        jmp     ecx
    }
}

__declspec(naked) void inv3() //0053BDA0
{
    __asm
    {
        mov     ecx, invBtnPtr
        push    0
        mov     edx, [ecx]
        call    dword ptr [edx + 14h]

        mov     ecx, [esi + 0E1Ch]
        mov     edx, 0053BDA6h
        jmp     edx
    }
}

__declspec(naked) void inv4() //0053C37C
{
    __asm
    {
        movsx   eax, byte ptr [edi + 2Ch]
        neg     eax
        mov     ecx, invBtnPtr
        push    eax
        mov     edx, [ecx]
        call    dword ptr [edx + 104h]
        mov     ecx, invBtnPtr
        push    1
        mov     edx, [ecx]
        call    dword ptr [edx + 14h]
        mov     ecx, edi
        //call    sub_553EA0 -> 419150
        mov     eax, [ecx + 4]

        mov     ecx, edi
        mov     eax, 00419150h
        call    eax
        mov     ecx, 0053C383h
        jmp     ecx
    }
}

__declspec(naked) void inv5() //0053E014
{
    __asm
    {
        mov     ecx, invBtnPtr
        mov     eax, 004C5280h
        call    eax
        neg     eax
        mov     [ebx + 2Ch], al

        mov     ecx, ebx
        mov     eax, 00419150h
        call    eax
        mov     esi, 0053E01Bh
        jmp     esi
    }
}

__declspec(naked) void inv6() //005F2756
{
    __asm
    {
        test    cl, cl
        jnz     cont
        cmp     ebp, 8
        jz      short loc_7E236B
        mov     dword ptr [ebx], 0FFFFFFFFh
        //
loc_7E236B:                     //        ; CODE XREF: sub_437120+3AB243j
        inc     ebp
        add     ebx, 4
        cmp     ebp, 10h
        jge     jmpFar
        xor     edx, edx
        mov     eax, 005F274Bh
        jmp     eax
jmpFar:
        mov     eax, 005F2817h
        jmp     eax
cont:
        mov     eax, 005F275Eh
        jmp     eax
    }
}

/*
char invCond;

__declspec(naked) void invProcessCond() //005F4A0F
{
    __asm
    {
        mov     ecx, [ecx + edi * 4]
        mov     al, [ecx + 2Ch]
        mov     invCond, al
        mov     eax, 005F1DE0h
        call    eax
        cmp     al, 2
        jz      loc_5F4A2D
        add     al, invCond
        jnz     loc_5F4A63
        mov     ecx, 005F4A23h
        jmp     ecx
loc_5F4A63:
        mov     ecx, 005F4A63h
        jmp     ecx
loc_5F4A2D:
        mov     ecx, 005F4A2Dh
        jmp     ecx
    }
}
*/

#define CONDITION_OR_ID 30

void __stdcall TRIBE_Trigger__update_new(TRIBE_Trigger* trigger, int time)
{
    if (trigger->state == 1)
    {
        trigger->timer += time;

        bool result = true;

        for (int i = 0; i < trigger->condition_num; i++)
        {
            if (trigger->condition_list[trigger->condition_order[i]]->condition == CONDITION_OR_ID)
            {
                if (result)
                    break;
                else
                    result = true;
            }
            else if (result)
            {
                __int8 v = trigger->condition_list[trigger->condition_order[i]]->values[8] & 0xFF;
                __int8 r = TRIBE_Trigger_Condition__update(trigger->condition_list[trigger->condition_order[i]], trigger->timer, trigger->system->world);
                if (r == 2) //bad condition
                {
                    trigger->state = 3;
                    if (trigger->description_display_during_game && !trigger->system->objectives_updated)
                    {
                        (*base_game)->vfptr->notification(*base_game, 138, -1, -1, -1, 0);
                        trigger->system->objectives_updated = 1;
                    }
                    result = false;
                    break;
                }
                r += v;
                if (r)
                    result = false;
            }
        }

        if (result)
        {
            for (int i = 0; i < trigger->effect_num; i++)
                TRIBE_Trigger_Effect__update(trigger->effect_list[i], trigger->system->world);

            trigger->timer = 0;
            if (!trigger->loopable)
            {
                trigger->state = 2;
                if (trigger->description_display_during_game && !trigger->system->objectives_updated)
                {
                    (*base_game)->vfptr->notification(*base_game, 138, -1, -1, -1, 0);
                    trigger->system->objectives_updated = 1;
                }
            }
        }
    }
}

__declspec(naked) void on_trigger_system_update() //005F54D6
{
    __asm
    {
        mov     ecx, [ecx + edi * 4]
        push    ecx
        call    TRIBE_Trigger__update_new
        mov     ecx, 005F54DEh
        jmp     ecx
    }
}

extern int* mapptr;

__declspec(naked) void conditionAreaExplored()
{
    __asm
    {
        mov     ebp, mapptr
        mov     edx, [esi + 34h]    //y1
        mov     edi, [esi + 3Ch]    //y2
        //sub     ebx, edx          //rect x size - 1

        shl     edx, 2
        shl     edi, 2
        add     edx, ebp          //y1 coloumn, x1 to x2
        add     edi, ebp          //y2 coloumn, x1 to x2

        xor     ecx, ecx
        //mov     esi, edx

x_cont:
        //edx: current coloumn
        mov     ebp, [edx]

        mov     eax, [esi + 30h]
        //mov     eax, [esi+ecx]
        push    edx
y_cont:
        shl     ecx, 10h
        mov     cl, [esi + 20h]
        mov     ebx, 10000h
        shl     ebx, cl
        shr     ecx, 10h
        test    [ebp + eax * 4], ebx
        setnz   bl
        movzx   ebx, bl
        add     ecx, ebx
        inc     eax
        cmp     eax, [esi + 38h]
        jle     y_cont
        pop     edx
        cmp     edx, edi
        jge     x_end
        add     edx, 4
        jmp     x_cont
x_end:
        //ecx = n tiles
        mov     eax, [esi + 38h]
        sub     eax, [esi + 30h]
        inc     eax            //eax = xl
        mov     edx, [esi + 3Ch]
        sub     edx, [esi + 34h]
        inc     edx            //edx = yl
        mul     edx
        mov     ebx, [esi + 0Ch]
        mul     ebx
        mov     ebx, 100
        div     ebx
        //eax = n tiles required
        cmp     ecx, eax
        jl      _not_enough_tiles
        jmp     condMet
_not_enough_tiles:
        jmp     condNotMet
    }
}

__declspec(naked) void conditionAlliance()
{
    __asm
    {
        mov     ecx, [esp + 38h]    //player ptr
        mov     eax, [esi + 28h]    //timer = target player
        push    eax
        mov     eax, 004C2C60h
        call    eax
        mov     cl, [esi + 48h]
        cmp     al, cl
        jz      _alliance_same
        jmp     condNotMet
_alliance_same:
        jmp     condMet
    }
}

bool __fastcall compare_null(int, int) { return false; }
bool __fastcall compare_ge(int v1, int v2) { return v1 >= v2; }
bool __fastcall compare_g(int v1, int v2) { return v1 > v2; }
bool __fastcall compare_e(int v1, int v2) { return v1 == v2; }

//////////////

float __fastcall getval_null(RGE_Static_Object*, int) { return 0; }
float __fastcall getval_hp(RGE_Static_Object* unit, int) { return unit->hp; }
float __fastcall getval_sp(RGE_Static_Object* unit, int) { return unit->sp; }
float __fastcall getval_resources(RGE_Static_Object* unit, int) { return unit->attribute_amount_held; }
float __fastcall getval_reload(RGE_Static_Object* unit, int) { return unit->master_obj->master_type >= 50 ? ((RGE_Combat_Object*)unit)->attack_timer : 0.0f; }
//float __stdcall getval_constr(UNIT* unit) { return *(float*)((int)unit + 0x230); }
float __fastcall getval_civoverride(RGE_Static_Object* unit, int) { return unit->gbg_civ_override; }

float __fastcall getval_kills(RGE_Static_Object* unit, int)
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    return ud ? ud->kills : 0;
}

float __fastcall getval_facet(RGE_Static_Object* unit, int)
{
    return unit->sprite ? unit->facet * 16 / unit->sprite->facet_num : 0;
}

float __fastcall getval_hp_percent(RGE_Static_Object* unit, int)
{
    return unit->hp / (float)unit->master_obj->hp * 100;
}

float __fastcall getval_sp_percent(RGE_Static_Object* unit, int)
{
    return unit->sp / (float)unit->master_obj->hp * 100;
}

float __fastcall getval_garrison(RGE_Static_Object* unit, int)
{
    RGE_New_Object_List* objects = unit->objects;
    return objects ? objects->Number_of_objects : 0.0f;
}

float __fastcall getval_reload_percent(RGE_Static_Object* unit, int)
{
    if (unit->master_obj->master_type < 50)
        return 0.0f;
    else
    {
        RGE_Combat_Object* combat_obj = (RGE_Combat_Object*)unit;
        return combat_obj->attack_timer / combat_obj->master_obj->speed_of_attack * 100;
    }
}

/*float __stdcall getval_constr_percent(UNIT* unit)
{
    int propObj = *(int*)((int)unit + 0x14);
    if (*(char*)(propObj + 4) == 80)
    {
        float trainTime = *(short*)(propObj + 0x18E);
        return getval_constr(unit) / trainTime * 100;
    }
    else
        return 100;
}*/

float __fastcall getval_counter(RGE_Static_Object* unit, int c)
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    if (ud && (ud->countersUsed))
        switch (c)
        {
        case 1:
            return ud->miscCounter1;
            break;
        case 2:
            return ud->miscCounter2;
            break;
        case 3:
            return ud->miscCounter3;
            break;
        case 4:
            return ud->miscCounter4;
            break;
        case 5:
            return ud->miscCounter5;
            break;
        default:
            return 0;
            break;
        }
    else
        return 0;
}

/*
float __fastcall getval_action(RGE_Static_Object* unit, int task_id)
{
    if (unit->master_obj->master_type >= 40)
    {
        RGE_Action_Object* act_obj = (RGE_Action_Object*)unit;
        RGE_Action_Node* node = act_obj->actions->list;
        while (node)
        {
            if (node->action->action_type == task_id)
            {
                char b[0x100];
                node->action->vfptr->get_state_name(node->action, b);
                chat(b);
                return task_id;
            }
            node = node->next;
        }
    }
}
*/

bool __stdcall conditionVariable_actual(condition* c, RGE_Player* player, RGE_Static_Object* object)
{
    RGE_Static_Object* units[0x1000];
    memset(units, 0, 0x1000 * 4);

    bool(__fastcall* compare) (int, int) = compare_null;

    switch (c->id)
    {
    case 0x1B:
        compare = compare_ge;
        break;
    case 0x1C:
        compare = compare_e;
        break;
    default:
        break;
    }

    int n;
    if (object)
    {
        n = 1;
        units[0] = object;
    }
    else
        n = RGE_Player_Object_List__Count(player->objects, c->obj_list_type, c->obj_group, 2,
            c->area_x1, c->area_y1, c->area_x2, c->area_y2, c->obj_type, units, 0);

    float(__fastcall* getval) (RGE_Static_Object*, int) = getval_null;

    int sub_value = 0;

    switch (c->ai_signal)
    {
    case 0:
        getval = getval_hp;
        break;
    case 1:
        getval = getval_hp_percent;
        break;
    case 2:
        getval = getval_sp;
        break;
    case 3:
        getval = getval_sp_percent;
        break;
    case 4:
        getval = getval_reload;
        break;
    case 5:
        getval = getval_reload_percent;
        break;
    case 6:
        getval = getval_resources;
        break;
    case 7:
        getval = getval_garrison;
        break;
    case 8:
        getval = getval_counter;
        sub_value = 1;
        break;
    case 9:
        getval = getval_counter;
        sub_value = 2;
        break;
    case 10:
        getval = getval_counter;
        sub_value = 3;
        break;
    case 11:
        getval = getval_counter;
        sub_value = 4;
        break;
    case 12:
        getval = getval_counter;
        sub_value = 5;
        break;
    case 13:
        getval = getval_kills;
        break;
    case 14:
        getval = getval_facet;
        break;
    case 15:
        getval = getval_civoverride;
        break;
    default:
        break;
    }

    int cnt = 0;

    for (int i = 0; i < n; i++)
        if (!(c->trigger & 1) ^ compare(getval(units[i], sub_value), c->timer))
            cnt++;

    return !(c->trigger & 1) ^ cnt >= c->quantity;
}

__declspec(naked) void conditionVariable()
{
    __asm
    {
        mov     eax, [esp + 14h]    //object
        mov     ecx, [esp + 38h]    //player
        push    eax
        push    ecx
        push    esi                 //condition
        call    conditionVariable_actual
        test    al, al
        jnz     condMet
        jmp     condNotMet
    }
}

__declspec(naked) void conditionPlayerCiv()
{
    __asm
    {
        mov     ecx, [esp + 38h]    //player
        mov     al, [ecx + 161h]
        mov     ecx, [esi + 48h]
        cmp     al, cl
        jz      condMet
        jmp     condNotMet
    }
}

__declspec(naked) void conditionOR()
{
    __asm
    {
        jmp     condMet
    }
}

void* unitContainter_countUnits_ungarrisoned;
void* unitContainter_countUnits_garrisoned;

void reloc(void* src, void* dst)
{
#ifndef TARGET_VOOBLY
    *(unsigned long*)src = (unsigned long)dst - ((unsigned long)src + 4);
#else
    writeDword((DWORD)src, (DWORD)dst - ((DWORD)src + 4));
#endif
}

__declspec(naked) void unitContainter_countUnits_wrapper()
{
    __asm
    {
        mov     eax, [esi + 48h]
        test    eax, eax
        jz      _count_default
        cmp     eax, 1
        jz      _count_ungarrisoned
        cmp     eax, 2
        jz      _count_garrisoned
_count_default:
        mov     eax, 004AF980h
        jmp     eax
_count_ungarrisoned:
        mov     eax, unitContainter_countUnits_ungarrisoned
        jmp     eax
_count_garrisoned:
        mov     eax, unitContainter_countUnits_garrisoned
        jmp     eax
    }
}

extern void* new_memory_pages;

void* const count_objects_limit_normal_continue = (void* const)0x004AFAB0;  //+0x130
void* const count_objects_limit_normal_stop = (void* const)0x004AFACC;      //+0x14C
void* count_objects_limit_garrisoned_continue;
void* count_objects_limit_garrisoned_stop;
void* count_objects_limit_ungarrisoned_continue;
void* count_objects_limit_ungarrisoned_stop;

__declspec(naked) void count_objects_limit_normal() //004AFAA9
{
    __asm
    {
        add     eax, 4
        mov     [esp + 14h], ecx
        cmp     ecx, 5400
        jge     stop_counting
        mov     ecx, count_objects_limit_normal_continue
        jmp     ecx

stop_counting:
        mov     ecx, count_objects_limit_normal_stop
        jmp     ecx
    }
}

__declspec(naked) void count_objects_limit_garrisoned() //004AFAA9
{
    __asm
    {
        add     eax, 4
        mov     [esp + 14h], ecx
        cmp     ecx, 5400
        jge     stop_counting
        mov     ecx, count_objects_limit_garrisoned_continue
        jmp     ecx

stop_counting:
        mov     ecx, count_objects_limit_garrisoned_stop
        jmp     ecx
    }
}

__declspec(naked) void count_objects_limit_ungarrisoned() //004AFAA9
{
    __asm
    {
        add     eax, 4
        mov     [esp + 14h], ecx
        cmp     ecx, 5400
        jge     stop_counting
        mov     ecx, count_objects_limit_ungarrisoned_continue
        jmp     ecx

stop_counting:
        mov     ecx, count_objects_limit_ungarrisoned_stop
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void make_counter_functions()
{
    setHook((void*)0x004AFAA9, count_objects_limit_normal);

    DWORD r;
    unitContainter_countUnits_ungarrisoned = new_memory_pages;
    ReadProcessMemory(GetCurrentProcess(), (void*)0x004AF980, unitContainter_countUnits_ungarrisoned, 0x160, &r);
    unitContainter_countUnits_garrisoned = (char*)unitContainter_countUnits_ungarrisoned + 0x160;
    memcpy(unitContainter_countUnits_garrisoned, unitContainter_countUnits_ungarrisoned, 0x160);

    reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0AB, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0B9, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0D9, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0E7, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x12A, count_objects_limit_ungarrisoned);
    count_objects_limit_ungarrisoned_continue = (void*)((DWORD)unitContainter_countUnits_ungarrisoned + 0x130);
    count_objects_limit_ungarrisoned_stop = (void*)((DWORD)unitContainter_countUnits_ungarrisoned + 0x14C);

    reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0AB, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0B9, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0D9, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0E7, (void*)0x00632BAC);
    reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x12A, count_objects_limit_garrisoned);
    count_objects_limit_garrisoned_continue = (void*)((DWORD)unitContainter_countUnits_garrisoned + 0x130);
    count_objects_limit_garrisoned_stop = (void*)((DWORD)unitContainter_countUnits_garrisoned + 0x14C);

    setHook((char*)unitContainter_countUnits_ungarrisoned + 0x0A3, (char*)unitContainter_countUnits_ungarrisoned + 0x134);
    setHook((char*)unitContainter_countUnits_garrisoned + 0x0D1, (char*)unitContainter_countUnits_garrisoned + 0x134);

#ifndef TARGET_VOOBLY
    VirtualProtect((void*)0x005F207E, 0x10, PAGE_READWRITE, &r);
    reloc((void*)0x005F207E, &unitContainter_countUnits_wrapper);
    VirtualProtect((void*)0x005F207E, 0x10, PAGE_EXECUTE_READ, &r);

    VirtualProtect(unitContainter_countUnits_ungarrisoned, 0x1000, PAGE_EXECUTE_READ, &r);
#else
    reloc((void*)0x005F207E, &unitContainter_countUnits_wrapper);
#endif
}

void setConditionHooks()
{
    setConditionNumbers();
    setHook((void*)0x005F5DD1, condParams);
    setHook((void*)0x00529A06, inv1);
    setHook((void*)0x0052ABD7, inv2);
    setHook((void*)0x0053BDA0, inv3);
    setHook((void*)0x0053C37C, inv4);
    setHook((void*)0x0053E014, inv5);
    setHook((void*)0x005F2756, inv6);

    //setHook((void*)0x005F4A0F, invProcessCond);
    setHook((void*)0x005F54D6, on_trigger_system_update);


    make_counter_functions();
}
#pragma optimize( "", on )
