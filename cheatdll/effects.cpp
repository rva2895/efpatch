#include "stdafx.h"

#include "advtriggereffect.h"
#include "editpropertyobject.h"
#include "effectUnitVar.h"

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

        mov     ebx, [edx + 0BCh]       //terrain effect
        mov     [ebx + 7], al           //player

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
        jnz     _snapview_noobject
        mov     ecx, [esp + 134h]          //location_object
        test    ecx, ecx
        jz      _snapview_noobject
        mov     eax, [ecx + 14h]
        cmp     byte ptr [eax + 4], 3Ch    //type 60
        jz      _snapview_noobject
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
_snapview_noobject:
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
        jnz     _scrollview_noobject
        mov     ecx, [esp + 134h]          //location_object
        test    ecx, ecx
        jz      _scrollview_noobject
        mov     eax, [ecx + 14h]
        cmp     byte ptr [eax + 4], 3Ch    //type 60
        jz      _scrollview_noobject
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
_scrollview_noobject :
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
        jz      _snapscroll_no_change_location
        xor     ecx, ecx
        dec     ecx
        mov     [edi + 44h], ecx
        mov     [edi + 48h], ecx
_snapscroll_no_change_location:
        mov     ecx, 005F3DB1h
        jmp     ecx
    }
}

void snapscroll_setJMP(DWORD jmp_location)
{
    writeDword(jmp_location + 1, (DWORD)&snapscroll_finish - (jmp_location + 5));
}

const char aChangeGlobalUnit[] = "Change Unit Property Object";
const char aExplore[] = "Explore Area";
const char aUnitVar[] = "Change Unit Variable";
const char aTerrain[] = "Change Terrain";
const char aDefeat[] = "Declare Defeat";
const char aBreakpoint[] = "Breakpoint";

__declspec(naked) void triggerDisplayHook()
{
    __asm
    {
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

#ifdef _DEBUG
        mov     ecx, [edi + 0E24h]      //breakpoint
        push    30h
        push    offset aBreakpoint
        mov     eax, 4C82A0h
        call    eax
#endif

        mov     eax, 007B23ACh
        jmp     eax
    }
}//2B02FC

extern int* mapptr;

__declspec(naked) void effectExploreArea()
{
    __asm
    {
        mov     ebp, mapptr
        mov     edx, [edi + 50h]    //y1
        mov     esi, [edi + 58h]    //y2
        //sub     ebx, edx          //rect x size - 1

        shl     edx, 2
        shl     esi, 2
        add     edx, ebp          //y1 coloumn, x1 to x2
        add     esi, ebp          //y2 coloumn, x1 to x2
        //mov     esi, edx

x_cont:
        //edx: current coloumn
        mov     ebp, [edx]

        mov     eax, [edi + 4Ch]
        //mov     eax, [esi+ecx]
y_cont:
        mov     ecx, [edi + 28h]
        mov     ebx, 10000h
        shl     ebx, cl
        or      [ebp + eax * 4], ebx
        inc     eax
        cmp     eax, [edi + 54h]
        jle     y_cont

        cmp     edx, esi
        jge     x_end
        add     edx, 4
        jmp     x_cont
x_end:
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

loc_5F3AAD:                             // CODE XREF: ProcessTriggerEffect+1061j

        mov     eax, [edi + 6Ch]
        push    eax
        mov     eax, [esi]
        push    eax
        call    effectUnitVarActual

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

int (__thiscall* map_updateBlend)(void *map, int, int, int x1, int y1, int x2, int y2, int, int, int) =
    (int(__thiscall*) (void *map, int, int, int x1, int y1, int x2, int y2, int, int, int))0x00495F80;

void __stdcall effectTerrain_2(void* map, int x1, int x2, int y1, int y2, char t)
{
    char** tiles = *(char***)((DWORD)map + 0xBF18);
    int map_max = *(int*)((DWORD)map + 8) - 1;
    if (x1 < 0) x1 = 0;
    if (x2 < 0) x2 = 0;
    if (y1 < 0) y1 = 0;
    if (y2 < 0) y2 = 0;
    if (x1 > map_max) x1 = map_max;
    if (x2 > map_max) x2 = map_max;
    if (y1 > map_max) y1 = map_max;
    if (y2 > map_max) y2 = map_max;
    for (int j = y1; j <= y2; j++)
    {
        char* col = *(tiles + j);
        for (int i = x1; i <= x2; i++)
        {
            col[i * 32 + 5] = t;
        }
    }
    map_updateBlend(map, 0, 0, x1, y1, x2, y2, 0, 0, 0);
}

__declspec(naked) void effectTerrain()
{
    __asm
    {
        mov     eax, 006A3684h
        mov     eax, [eax]
        mov     eax, [eax + 420h]
        mov     eax, [eax + 34h]
        push    [edi + 0Ch]
        push    [edi + 58h]
        push    [edi + 50h]
        push    [edi + 54h]
        push    [edi + 4Ch]
        push    eax
        call    effectTerrain_2
        
        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}

__declspec(naked) void effectDefeat()
{
    __asm
    {
        mov     ecx, [esp + 14h] //player
        test    ecx, ecx
        jz      _defeat_end
        mov     edx, [ecx]
        push    2
        call    dword ptr [edx + 8]
_defeat_end:
        mov     ecx, 005F3DB1h
        jmp     ecx
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

#pragma optimize( "s", on )
void setEffectHooks()
{
    DWORD adrBuf = ((DWORD)&advTriggerEffect - 0x007B3199);

    //writeByte(0x007B3188, 0x90); //change unit data - remove type check
    //writeByte(0x007B3189, 0x90);

    //int adrSetVarEffect = (int)&setVarEffect;

    setHook((void*)0x007B2A9B, effectParams);
    setHook((void*)0x007B2388, triggerDisplayHook);

    //setHook ((void*)0x007B2ABF, &setVarHook);

    int nEffects = 0x30;
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
#ifdef _DEBUG
    writeDword(0x007B22FC, (DWORD)&effectBreakpoint);
#endif // _DEBUG
    
    writeDword(0x007B2240 + 29 * 4, (DWORD)&effectSnapView_new);
    writeDword(0x007B2240 + 15 * 4, (DWORD)&effectScrollView_new);
    for (DWORD p = 0x005F5B09; p < 0x005F5B1B; p++)
        writeByte(p, 0x90);
    for (DWORD p = 0x005F5AEE; p < 0x005F5B00; p++)
        writeByte(p, 0x90);

    snapscroll_setJMP(0x005F374F+1);
    snapscroll_setJMP(0x005F3768);
    snapscroll_setJMP(0x005F3773+1);
    snapscroll_setJMP(0x005F37AD+1);
    snapscroll_setJMP(0x005F37BC);
}
#pragma optimize( "", on )
