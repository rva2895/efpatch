#include "stdafx.h"

#include "drs.h"
#include "registry.h"

const char aSwbg[] = "swbg";

/*char* aDrsWide[] =
{
    "sounds_x1.drs",
    "sounds_x2.drs",
    "graphics.drs",
    "graphics_x1.drs",
    "graphics_x2.drs",
    "terrain.drs",
    "terrain_x1.drs",
    "terrain_x2.drs",
    "wide.drs",
    "interfac_x2.drs",
    "interfac.drs",
    "interfac_x1.drs",
    "gamedata_x2.drs"
};

char* aDrsNoWide[] =
{
    "sounds_x1.drs",
    "sounds_x2.drs",
    "graphics.drs",
    "graphics_x1.drs",
    "graphics_x2.drs",
    "terrain.drs",
    "terrain_x1.drs",
    "terrain_x2.drs",
    "interfac_x2.drs",
    "interfac.drs",
    "interfac_x1.drs",
    "gamedata_x2.drs"
};*/

const char* aDrsWide[] =
{
    "sounds_x1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"sounds_x2.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"graphics_p1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"graphics_x1_p1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"graphics_x2.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"terrain_p1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"terrain_x1_p1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"terrain_x2.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"wide_p1.drs", //
    DATA_FOLDER_PREFIX_FROM_DATA"interfac_x2.drs", //
    DATA_FOLDER_PREFIX_FROM_DATA"interfac_p1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"interfac_x1_p1.drs",
    //"interfac_x2.drs",
    //"wide_p1.drs",
    DATA_FOLDER_PREFIX_FROM_DATA"gamedata_x2.drs"
};

const char* aDrsNoWide[] =
{
    "sounds_x1.drs",
    "sounds_x2.drs",
    "graphics_p1.drs",
    "graphics_x1_p1.drs",
    "graphics_x2.drs",
    "terrain_p1.drs",
    "terrain_x1_p1.drs",
    "terrain_x2.drs",
    "interfac_x2.drs", //
    "interfac_p1.drs",
    "interfac_x1_p1.drs",
    //"interfac_x2.drs",
    "gamedata_x2.drs"
};

const char* aDrsCCWide[] =
{
    "sounds_x1.drs",
    "graphics.drs",
    "graphics_x1.drs",
    //"terrgrid.drs",
    "terrain.drs",
    "terrain_x1.drs",
    "wide.drs", //
    "interfac.drs",
    "interfac_x1.drs",
    //"wide.drs",
    "gamedata_x1.drs"
};

int paramCntr = 1;

const int nDrsWide = sizeof(aDrsWide) / sizeof(aDrsWide[0]);
const int nDrsNoWide = sizeof(aDrsNoWide) / sizeof(aDrsNoWide[0]);
const int nDrsCCWide = sizeof(aDrsCCWide) / sizeof(aDrsCCWide[0]);

int nDrs;
const char** aDrs;

__declspec(naked) void loadDRS()
{
    __asm
    {
        xor     eax, eax
        mov     edx, paramCntr
        test    edx, edx
        setnz   al
        sub     edx, eax
        mov     paramCntr, edx
        push    eax
        mov     eax, [ebp + 24h]
        add     eax, 1467h
        push    eax
        mov     edx, offset aSwbg
        mov     eax, 4D4C10h
        call    eax
        ret
    }
}

__declspec(naked) void loadDRSHook() //005E4B78
{
    __asm
    {
        push    edi
        push    esi

        mov     edi, aDrs
        mov     eax, nDrs
        lea     esi, [edi + eax * 4]
cont:
        cmp     edi, esi
        jnb     end
        mov     ecx, [edi]
        add     edi, 4
        call    loadDRS
        jmp     cont
end:
        pop     esi
        pop     edi
        mov     ecx, 005E4C46h
        jmp     ecx
    }
}

__declspec(naked) void drsHeaderEnd() //004D4FE8
{
    __asm
    {
        mov     eax, [ecx + 50h]
        add     eax, ecx
        cmp     esi, eax
        jz      _outside
        mov     eax, [esp + 10h]
        mov     edx, [esi]
        cmp     edx, eax
        jnz     _outside
        mov     edx, 004D5020h
        jmp     edx
_outside:
        mov     edx, 004D4FF2h
        jmp     edx
    }
}

void setDRSLoadHooks(int ver, bool wide)
{
    if (wide)
    {
        switch (ver)
        {
        case VER_CC:
            nDrs = nDrsCCWide;
            aDrs = aDrsCCWide;
            break;
        case VER_EF:
            nDrs = nDrsWide;
            aDrs = aDrsWide;
            paramCntr = 2;
            break;
        default:
            break;
        }
    }
    else if (ver == VER_EF)
    {
        nDrs = nDrsNoWide;
        aDrs = aDrsNoWide;
        paramCntr = 2;
    }
    else
        return;

    setHook((void*)0x005E4B78, loadDRSHook);

    //heap corruption
    setHook((void*)0x004D4FE8, drsHeaderEnd);
}
