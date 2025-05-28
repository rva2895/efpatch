#include "stdafx.h"

#include "advtriggereffect.h"
#include "editpropertyobject.h"
#include "effectUnitVar.h"
#include "effect_command.h"

__declspec(naked) void effectParams()
{
    __asm
    {
        mov     [ebx + 6], al
        mov     [ebx + 7], al
        mov     [ebx + 0Ah], al

        mov     ebx, [edx + 0B0h]       //explore effect
        mov     [ebx + 7], al           //player
        mov     [ebx + 10h], al         //area
        mov     [ebx + 11h], al
        mov     [ebx + 12h], al
        mov     [ebx + 13h], al

        mov     ebx, [edx + 0B4h]       //unit var effect
        mov     [ebx + 4], al           //obj
        mov     [ebx + 5], al
        mov     [ebx + 6], al           //obj list
        mov     [ebx + 7], al           //player
        mov     [ebx + 0Ah], al         //message
        mov     [ebx + 10h], al         //area
        mov     [ebx + 11h], al
        mov     [ebx + 12h], al
        mov     [ebx + 13h], al
        mov     [ebx + 14h], al         //obj group
        mov     [ebx + 15h], al         //obj type

        mov     ebx, [edx + 0B8h]       //terrain effect
        mov     [ebx + 0h], al          //terrain id
        mov     [ebx + 10h], al         //area
        mov     [ebx + 11h], al
        mov     [ebx + 12h], al
        mov     [ebx + 13h], al

        mov     ebx, [edx + 0BCh]       //declare defeat effect
        mov     [ebx + 7], al           //player

        mov     ebx, [edx + 0C0h]       //copy obj effect
        mov     [ebx + 1], al           //quantity
        mov     [ebx + 6], al           //obj list
        mov     [ebx + 7], al           //player

        mov     ebx, [edx + 0C4h]       //transform obj effect
        mov     [ebx + 1], al           //quantity
        mov     [ebx + 4], al           //obj
        mov     [ebx + 5], al
        mov     [ebx + 6], al           //obj list
        mov     [ebx + 7], al           //player
        mov     [ebx + 10h], al         //area
        mov     [ebx + 11h], al
        mov     [ebx + 12h], al
        mov     [ebx + 13h], al
        mov     [ebx + 14h], al         //obj group
        mov     [ebx + 15h], al         //obj type

        mov     ebx, [edx + 0C8h]       //teleport obj effect
        mov     [ebx + 4], al           //obj
        mov     [ebx + 5], al
        mov     [ebx + 6], al           //obj list
        mov     [ebx + 7], al           //player
        mov     [ebx + 0Eh], al         //x
        mov     [ebx + 0Fh], al         //y
        mov     [ebx + 10h], al         //area
        mov     [ebx + 11h], al
        mov     [ebx + 12h], al
        mov     [ebx + 13h], al
        mov     [ebx + 14h], al         //obj group
        mov     [ebx + 15h], al         //obj type

        //
        
        //mov     ebx, [edx + 0C0h]       //command effect
        //mov     [ebx + 0h], al          //command id
        //mov     [ebx + 1], al           //quantity
        //mov     [ebx + 4], al           //obj
        //mov     [ebx + 5], al
        //mov     [ebx + 6], al           //obj list
        //mov     [ebx + 7], al           //player
        //mov     [ebx + 0Eh], al         //x
        //mov     [ebx + 0Fh], al         //y
        //mov     [ebx + 10h], al         //area
        //mov     [ebx + 11h], al
        //mov     [ebx + 12h], al
        //mov     [ebx + 13h], al
        //mov     [ebx + 14h], al         //obj group
        //mov     [ebx + 15h], al         //obj type

        //

        mov     ebx, [edx + 78h]        //snap view
        mov     [ebx + 0Eh], al         //x
        mov     [ebx + 0Fh], al         //y
        mov     [ebx + 4], al           //set object fix
        mov     [ebx + 5], al

        mov     ebx, [edx + 40h]        //scroll view
        mov     [ebx + 0Eh], al         //x
        mov     [ebx + 0Fh], al         //y
        mov     [ebx + 4], al           //set object fix
        mov     [ebx + 5], al

        mov     ebx, [edx + 4Ch]        //patrol
        mov     [ebx + 4], al
        mov     [ebx + 5], al
        mov     [ebx + 6], al
        mov     [ebx + 7], al
        mov     [ebx + 10h], al
        mov     [ebx + 11h], al
        mov     [ebx + 12h], al
        mov     [ebx + 13h], al
        mov     [ebx + 14h], al
        mov     [ebx + 15h], al

        mov     ebx, [edx + 58h]        //freeze unit (attack stance)
        mov     [ebx + 0h], al

        mov     ebx, [edx + 8h]         //research tech
        mov     [ebx + 0h], al

        //mov     ebx, [edx + 2Ch]  //create object
        //mov     [ebx + 0h], al

        mov     ebx, [edx + 0B4h]
        //mov     [ebx+4], al
        //mov     [ebx+6], al
        //mov     [ebx+7], al
        //mov     [ebx+9], al
        //mov     [ebx+10h], al
        //mov     [ebx+11h], al
        //mov     [ebx+12h], al
        //mov     [ebx+13h], al
        //mov     [ebx+14h], al
        mov     [ebx + 15h], al

        mov     eax, [esi + 8]
        mov     cl, 1
        mov     edx, 005F5580h
        jmp     edx
    }
}

int snapscroll_changed_location;

__declspec(naked) void effectSnapView_new()
{
    __asm
    {
        mov     ecx, [edi + 44h]
        cmp     ecx, -1
        jnz     snapview_noobject
        mov     ecx, [esp + 134h]          //location_object
        test    ecx, ecx
        jz      snapview_noobject
        mov     eax, [ecx + 14h]
        cmp     byte ptr [eax + 4], 3Ch    //type 60
        jz      snapview_noobject
        mov     esi, 00632BACh             //ftol
        fld     dword ptr [ecx + 48h]
        call    esi
        fstp    st
        mov     [edi + 44h], eax
        fld     dword ptr [ecx + 4Ch]
        call    esi
        fstp    st
        mov     [edi + 48h], eax
        xor     ecx, ecx
        inc     ecx
        mov     snapscroll_changed_location, ecx
snapview_noobject:
        mov     ecx, 005F376Dh
        jmp     ecx
    }
}

__declspec(naked) void effectScrollView_new()
{
    __asm
    {
        mov     ecx, [edi + 44h]
        cmp     ecx, -1
        jnz     scrollview_noobject
        mov     ecx, [esp + 134h]          //location_object
        test    ecx, ecx
        jz      scrollview_noobject
        mov     eax, [ecx + 14h]
        cmp     byte ptr [eax + 4], 3Ch    //type 60
        jz      scrollview_noobject
        mov     esi, 00632BACh             //ftol
        fld     dword ptr [ecx + 48h]
        call    esi
        fstp    st
        mov     [edi + 44h], eax
        fld     dword ptr [ecx + 4Ch]
        call    esi
        fstp    st
        mov     [edi + 48h], eax
        xor     ecx, ecx
        inc     ecx
        mov     snapscroll_changed_location, ecx
scrollview_noobject:
        mov     ecx, 005F3749h
        jmp     ecx
    }
}

__declspec(naked) void snapscroll_finish()
{
    __asm
    {
        mov     ecx, snapscroll_changed_location
        test    ecx, ecx
        jz      snapscroll_no_change_location
        xor     ecx, ecx
        dec     ecx
        mov     [edi + 44h], ecx
        mov     [edi + 48h], ecx
snapscroll_no_change_location:
        mov     ecx, 005F3DB1h
        jmp     ecx
    }
}

void snapscroll_setJMP(DWORD jmp_location)
{
    writeDword(jmp_location + 1, (DWORD)&snapscroll_finish - (jmp_location + 5));
}

const char aChangeOwnMaster[] = "Change Own Object Master";
const char aChangeGlobalUnit[] = "Change Player Object Master";
const char aExplore[] = "Explore Area";
const char aUnitVar[] = "Change Object Variable";
const char aTerrain[] = "Change Terrain";
const char aDefeat[] = "Declare Defeat";
const char aCommand[] = "Command";
const char aCopyObj[] = "Copy Object Master";
const char aTransformObj[] = "Transform Object";
const char aTeleportObj[] = "Teleport Object";
const char aBreakpoint[] = "Breakpoint";

__declspec(naked) void triggerDisplayHook()
{
    __asm
    {
        mov     ecx, [edi + 0E24h]
        push    2Ah
        push    offset aChangeOwnMaster
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    2Bh
        push    offset aChangeGlobalUnit
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    2Ch
        push    offset aExplore
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    2Dh
        push    offset aUnitVar
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    2Eh
        push    offset aTerrain
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    2Fh
        push    offset aDefeat
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    30h
        push    offset aCopyObj
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    31h
        push    offset aTransformObj
        mov     eax, 4C82A0h
        call    eax

        mov     ecx, [edi + 0E24h]
        push    32h
        push    offset aTeleportObj
        mov     eax, 4C82A0h
        call    eax

        //mov     ecx, [edi + 0E24h]
        //push    30h
        //push    offset aCommand
        //mov     eax, 4C82A0h
        //call    eax

#ifdef _DEBUG
        mov     ecx, [edi + 0E24h]      //breakpoint
        push    33h
        push    offset aBreakpoint
        mov     eax, 4C82A0h
        call    eax
#endif

        mov     eax, 007B23ACh
        jmp     eax
    }
}//2B02FC

void __stdcall do_effect_explore_area(effect* e, RGE_Player* player)
{
    if (player)
    {
        bool update_map = player->id == (*base_game)->world->curr_player;
        RGE_Visible_Map__reveal_terrain_sq(player->visible, e->area_x1, e->area_y1, e->area_x2, e->area_y2, update_map, 0);
    }
}

__declspec(naked) void effectExploreArea()
{
    __asm
    {
        mov     eax, [esp + 14h]
        push    eax
        push    edi
        call    do_effect_explore_area
        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

__declspec(naked) void effectUnitVar()
{
    __asm
    {
        mov     eax, [esp + 10h]        // ; jumptable 005F2B53 case 23
        xor     ebp, ebp
        cmp     eax, ebx
        jle     endLoc                  // ; jumptable 005F2B53 default case
        lea     esi, [esp + 134h]

loc_5F3AAD:                             // CODE XREF: ProcessTriggerEffect+1061

        mov     eax, [edi + 6Ch]
        push    0
        push    0
        push    1
        push    eax
        mov     eax, [esi]
        push    eax
        call    effectUnitVar_do_multi_line_effect

        mov     eax, [esp + 10h]
        inc     ebp
        add     esi, 4
        cmp     ebp, eax
        jl      loc_5F3AAD

endLoc:
        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

/*
__declspec(naked) void effectCommand()
{
    __asm
    {
        mov     eax, [esp + 10h]    //unit count
        xor     ebp, ebp
        cmp     eax, ebx
        jle     endLoc_command
        lea     esi, [esp + 134h]   //object list

        mov     ecx, [esp + 14h]    //player
        mov     edx, [edi + 48h]    //location y
        push    edx
        mov     edx, [edi + 44h]    //location x
        push    edx
        mov     edx, [esp + 4Ch]    //target obj
        push    edx
        push    eax
        push    esi
        mov     eax, [edi + 0Ch]    //command
        push    ecx
        push    eax
        mov     eax, [edi + 10h]    //qty
        push    eax
        call    do_effect_command

endLoc_command:
        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}
*/

void __stdcall do_effect_terrain(effect* e)
{
    RGE_Map__set_terrain((*base_game)->world->map, NULL, NULL, e->area_x1, e->area_y1, e->area_x2, e->area_y2, e->ai_trigger_number, 1, 0);
    TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
    if (game_screen)
        game_screen->map_view->vfptr->set_redraw(game_screen->map_view, 2);
}

__declspec(naked) void effectTerrain()
{
    __asm
    {
        push    edi
        call    do_effect_terrain

        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

void __stdcall do_effect_defeat(RGE_Player* player)
{
    if (player)
        player->vfptr->set_game_status(player, 2);
}

__declspec(naked) void effectDefeat()
{
    __asm
    {
        mov     ecx, [esp + 14h] //player
        push    ecx
        call    do_effect_defeat

        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

void __stdcall do_effect_copy_obj(RGE_Player* player, int dst, int src)
{
    if (player)
        player->vfptr->copy_obj(player, dst, src);
}

__declspec(naked) void effectCopyObj()
{
    __asm
    {
        
        mov     ecx, [esp + 14h] //player
        mov     eax, [edi + 24h] //dest
        mov     edx, [edi + 10h] //quantity (source)
        push    edx
        push    eax
        push    ecx
        call    do_effect_copy_obj

        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

void __stdcall do_effect_transform_obj(RGE_Player* player, RGE_Static_Object** obj_list, int obj_count, int target_id)
{
    RGE_Master_Static_Object* target_master;
    if (player &&
        target_id >= 0 &&
        target_id < player->master_object_num &&
        (target_master = player->master_objects[target_id]))
    {
        for (int i = 0; i < obj_count; i++)
            obj_list[i]->vfptr->transform(obj_list[i], target_master);
    }
}

__declspec(naked) void effectTransformObj()
{
    __asm
    {
        mov     ecx, [esp + 14h] //player
        mov     eax, [esp + 10h] //obj count
        lea     ebp, [esp + 134h] //object list
        mov     edx, [edi + 10h] //target obj
        push    edx
        push    eax
        push    ebp
        push    ecx
        call    do_effect_transform_obj

        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

void __stdcall do_effect_teleport_obj(RGE_Player* player, RGE_Static_Object** obj_list, int obj_count, int x, int y)
{
    if (player)
    {
        float x1 = x + 0.5f;
        float y1 = y + 0.5f;

        for (int i = 0; i < obj_count; i++)
        {
            RGE_Static_Object* obj = obj_list[i];
            obj->vfptr->teleport(obj_list[i], x1, y1, 0.0f);
        }

        int obj_id_list_stack[0x100];
        int* obj_id_list_heap = NULL;
        int* obj_id_list;
        if (obj_count > _countof(obj_id_list_stack))
        {
            int* obj_id_list_heap = new int[obj_count];
            obj_id_list = obj_id_list_heap;
        }
        else
            obj_id_list = obj_id_list_stack;

        for (int i = 0; i < obj_count; i++)
            obj_id_list[i] = obj_list[i]->id;

        BUnitGroup* g;
        if (obj_count > 1 &&
            (g = RGE_Game_World__getUnitGroup(
                player->world,
                RGE_Game_World__commonUnitGroupID(player->world, obj_id_list, obj_count))) &&
            g->mNumberUnits == obj_count)
        {
            BUnitGroup__commandStop(g);
        }
        else
        {
            for (int i = 0; i < obj_count; i++)
            {
                RGE_Static_Object* obj = obj_list[i];
                if (obj->mGroupID != -1 &&
                    (g = RGE_Game_World__getUnitGroup(player->world, obj->mGroupID)))
                {
                    BUnitGroup__removeUnit(g, obj->id);
                }
                RGE_Static_Object__removeAllFromPathingGroup(obj);
                obj->vfptr->stop(obj);
                obj->vfptr->stopAllMovement(obj);
            }
        }

        if (obj_count > _countof(obj_id_list_stack))
            delete[] obj_id_list_heap;
    }
}

__declspec(naked) void effectTeleportObj()
{
    __asm
    {

        mov     ecx, [esp + 14h] //player
        mov     eax, [esp + 10h] //obj count
        lea     ebp, [esp + 134h] //object list
        mov     edx, [edi + 48h] //y
        push    edx
        mov     edx, [edi + 44h] //x
        push    edx
        push    eax
        push    ebp
        push    ecx
        call    do_effect_teleport_obj

        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

__declspec(naked) void effectBreakpoint()
{
    __asm
    {
        int     3
        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

void (__cdecl* const alloca_probe_internal) () = (void (__cdecl* const)())0x006347F0;

__declspec(naked) void effect_update_alloca_fix() //005F2AF5
{
    __asm
    {
        mov     eax, 55D4h
        call    alloca_probe_internal
        mov     eax, 005F2AFBh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setEffectHooks()
{
    DWORD adrBuf = ((DWORD)&advTriggerEffect - 0x007B3199);

    //writeByte(0x007B3188, 0x90); //change unit data - remove type check
    //writeByte(0x007B3189, 0x90);

    //int adrSetVarEffect = (int)&setVarEffect;

    setHook((void*)0x007B2A9B, effectParams);
    setHook((void*)0x007B2376, triggerDisplayHook);

    //setHook ((void*)0x007B2ABF, &setVarHook);

    int nEffects = 0x33;
    //int nEffects = 0x31;
#ifdef _DEBUG
    nEffects++;
#endif // _DEBUG

    writeByte(0x005F2B4C, nEffects); //effect count, old = 2d
    //writeByte (0x0053BD37, 0x28);

    writeByte(0x005F5575, (nEffects + 1) * 4);
    writeByte(0x005F550C, (nEffects + 1) * 4);

    writeByte(0x005F53AF, (nEffects + 1) * 4 - 8);

    writeDword(0x007B22E8, (DWORD)&changePropertyObjectHook); //changepropobj effect
    //WriteProcessMemory (GetCurrentProcess (), (void*)0x7B22EC, &adrSetVarEffect, 4, 0); //setvar effect

    writeDword(0x007B3195, (DWORD)adrBuf); //wtf???

    writeDword(0x007B22EC, (DWORD)&effectExploreArea);
    writeDword(0x007B22F0, (DWORD)&effectUnitVar);
    writeDword(0x007B22F4, (DWORD)&effectTerrain);
    writeDword(0x007B22F8, (DWORD)&effectDefeat);
    writeDword(0x007B22FC, (DWORD)&effectCopyObj);
    writeDword(0x007B2300, (DWORD)&effectTransformObj);
    writeDword(0x007B2304, (DWORD)&effectTeleportObj);
    //writeDword(0x007B22FC, (DWORD)&effectCommand);
#ifdef _DEBUG
    writeDword(0x007B2308, (DWORD)&effectBreakpoint);
#endif // _DEBUG
    
    writeDword(0x007B2240 + 29 * 4, (DWORD)&effectSnapView_new);
    writeDword(0x007B2240 + 15 * 4, (DWORD)&effectScrollView_new);

    writeNops(0x005F5B09, 0x005F5B1B - 0x005F5B09);
    writeNops(0x005F5AEE, 0x005F5B00 - 0x005F5AEE);

    snapscroll_setJMP(0x005F374F+1);
    snapscroll_setJMP(0x005F3768);
    snapscroll_setJMP(0x005F3773+1);
    snapscroll_setJMP(0x005F37AD+1);
    snapscroll_setJMP(0x005F37BC);

    setHook((void*)0x005F2AF5, effect_update_alloca_fix);

    //Trigger object overflow fix
    //writeByte(0x5F2AF8, 0x65);
    //writeByte(0x5F2AF9, 0x11);
    //writeByte(0x5F2B02, 0x65);
    //writeByte(0x5F2B03, 0x11);
    //writeByte(0x5F2C5F, 0x65);
    //writeByte(0x5F2C60, 0x11);
    //writeByte(0x5F3DB5, 0x65);
    //writeByte(0x5F3DB6, 0x11);
    //writeByte(0x5F3DC8, 0x65);
    //writeByte(0x5F3DC9, 0x11);

    writeDword(0x005F2B01, 0x55F0);
    
    writeDword(0x005F2C5E, 0x55EC);
    writeDword(0x005F3DB4, 0x55E4);
    writeDword(0x005F3DC7, 0x55E0);

    setHook((void*)0x0053BD3A, (void*)0x007B2340);
    writeByte(0x0053CE86, 0xEB);
    setHook((void*)0x005F557B, (void*)0x007B2A00);
    writeDword(0x005F2B56, 0x007B2240);

    log("Building effect lookup table...");
    build_unit_master_variable_lookup_table();
}
#pragma optimize( "", on )
