#include "stdafx.h"
#include "mapcopy.h"

bool mapcopy = false;

void __stdcall fixShape(RGE_View* view, float x, float y, short c)
{
    if (mapcopy)
    {
        int mapcopy_x = view->map->map_copy_data->width;
        int mapcopy_y = view->map->map_copy_data->height;
        int cx = (mapcopy_x + 1) % 2;
        int cy = (mapcopy_y + 1) % 2;
        RGE_View__set_selection_area(view, x - (mapcopy_x >> 1), y - (mapcopy_y >> 1), x + (mapcopy_x >> 1) - cx, y + (mapcopy_y >> 1) - cy);
    }
    else
        RGE_View__set_selection_area(view, x - (c >> 1), y - (c >> 1), x + (c >> 1), y + (c >> 1));
}

__declspec(naked) void onSetShape() //0060CBC1
{
    __asm
    {
        mov     eax, [esp + 18h]    //x
        mov     ecx, [esp + 1Ch]    //y
        push    ebx
        push    ecx
        push    eax
        push    edi
        call    fixShape
        mov     ebx, 0060CBEFh
        jmp     ebx
    }
}

__declspec(naked) void onChangeCurrentTool() //0052D394
{
    __asm
    {
        mov     [esi + 8ECh], eax
        cmp     eax, 4
        setz    cl
        mov     mapcopy, cl
        mov     edx, 0052D39Ah
        jmp     edx
    }
}

void __stdcall do_map_copy_brush(RGE_Map* map, RGE_Map_Copy_Object_Data* od, float x, float y)
{
    unsigned __int8 is_annex = (od->owner >> 16) & 0xFF;
    unsigned __int8 facet = (od->owner >> 24);
    if (!is_annex)
    {
        RGE_Player* player = map->game_world->players[od->owner & 0xFF];
        RGE_Static_Object* obj = player->vfptr->make_scenario_obj(player, x, y, 0.0f, od->id & 0xFFFF, 2, od->angle);
        if (facet != 0xFF)
            RGE_Static_Object__set_facet(obj, facet);
    }
}

__declspec(naked) void on_do_map_copy_brush() //004936CC
{
    __asm
    {
        mov     eax, [esp + 20h]
        mov     ecx, [esp + 24h]
        push    eax
        push    ecx
        push    edx
        push    edi
        call    do_map_copy_brush
        mov     eax, 004936F6h
        jmp     eax
    }
}

__declspec(naked) void on_map_copy_change_player() //0053B712
{
    __asm
    {
        mov     al, [eax + 4]
        cmp     al, 50h
        jz      short loc_53B71D
        cmp     al, 46h
        jnz     short loc_53B720

loc_53B71D:
        mov     [edx + 4], bl

loc_53B720:
        mov     edx, 0053B720h
        jmp     edx
    }
}

__declspec(naked) void on_map_copy_copy_map_section() //00493B2E
{
    __asm
    {
        mov     edx, [ecx + 14h]
        mov     bl, [edx + 4]
        mov     edx, [esi + 0BF88h]
        cmp     bl, 30
        jbe     short loc_493B4E
        cmp     bl, 80
        mov     bl, 0
        jnz     map_copy_copy_map_section_not_bldg
        cmp     dword ptr [ecx + 1DCh], 0
        setnz   bl

map_copy_copy_map_section_not_bldg:
        mov     edx, [edx + 0Ch]
        mov     ecx, [ecx + 0C8h]
        mov     [edx + eax + 8], ecx
        mov     byte ptr [edx + eax + 7], 0FFh //facet
        mov     [edx + eax + 6], bl //is_annex
        jmp     short loc_493B59

loc_493B4E:
        mov     edx, [edx + 0Ch]
        mov     cl, [ecx + 45h]
        mov     [edx + eax + 7], cl //facet
        mov     byte ptr [edx + eax + 6], 0 //is_annex
        mov     dword ptr [edx + eax + 8], 0

loc_493B59:
        mov     edx, 00493B59h
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setMapCopyHooks()
{
    setHook((void*)0x0060CBD5, onSetShape);
    setHook((void*)0x0052D394, onChangeCurrentTool);

    setHook((void*)0x004936CC, on_do_map_copy_brush);
    setHook((void*)0x0053B712, on_map_copy_change_player);
    setHook((void*)0x00493B2E, on_map_copy_copy_map_section);
}
#pragma optimize( "", on )
