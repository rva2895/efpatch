#include "stdafx.h"
#include "mapcopy.h"

int mapcopy_x = 1;
int mapcopy_y = -1;
bool mapcopy = false;

__declspec(naked) void onMapCopyGetDimensions() //0052F0EB
{
    __asm
    {
        mov     mapcopy_x, eax
        mov     mapcopy_y, ecx
        cmp     eax, ecx
        jg      short loc_52F0F1
        mov     eax, ecx
loc_52F0F1:
        mov     edx, 0052F0F1h
        jmp     edx
    }
}

void (__thiscall* setShape) (void* this_, int x1, int y1, int x2, int y2) =
    (void (__thiscall*) (void*, int, int, int, int))0x0060AFC0;

void __stdcall fixShape(void* this_, float x, float y, short c)
{
    if (mapcopy)
    {
        int cx = (mapcopy_x + 1) % 2;
        int cy = (mapcopy_y + 1) % 2;
        setShape(this_, x - (mapcopy_x >> 1), y - (mapcopy_y >> 1), x + (mapcopy_x >> 1) - cx, y + (mapcopy_y >> 1) - cy);
    }
    else
        setShape(this_, x - (c >> 1), y - (c >> 1), x + (c >> 1), y + (c >> 1));
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

#pragma optimize( "s", on )
void setMapCopyHooks()
{
    setHook((void*)0x0052F0EB, onMapCopyGetDimensions);
    setHook((void*)0x0060CBD5, onSetShape);
    setHook((void*)0x0052D394, onChangeCurrentTool);
}
#pragma optimize( "", on )
