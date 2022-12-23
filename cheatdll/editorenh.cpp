#include "stdafx.h"
#include "editorenh.h"
#include "triggerdesc.h"
#include "effect_command.h"

//bit 1: restriction, bit 2: collision
int placementSettings = 0;

int version_for_editor;

__declspec(naked) void noTerrainRestrictionHook() //00618FEE
{
    __asm
    {
        mov     ecx, placementSettings
        test    ecx, 1                    //no terrain restriction
        jnz     _place_unit
        and     eax, 0FFh
        jz      _place_unit
//_no_place_unit:
        mov     ecx, 0061922Eh
        jmp     ecx
_place_unit:
        mov     ecx, 00618FF9h
        jmp     ecx
    }
}

__declspec(naked) void noGridHook() //00618F90
{
    __asm
    {
        mov     edx, placementSettings
        test    edx, 2                    //no grid
        jnz     _no_grid
        mov     edx, [edi]
        push    eax
        mov     eax, [ebx + 108h]
        mov     ecx, 00618F99h
        jmp     ecx
_no_grid:
        mov     ecx, 00618FA9h
        jmp     ecx
    }
}

__declspec(naked) void removeUnitsFix() //0049539F
{
    __asm
    {
        mov     ecx, placementSettings
        test    ecx, 1
        jnz     _no_remove
        mov     ecx, [ebx]
        mov     eax, [ecx + 14h]
        mov     ax, [eax + 18h]
        mov     edx, 004953A8h
        jmp     edx
_no_remove:
        mov     edx, 0049549Bh   //A0
        jmp     edx
    }
}

__declspec(naked) void __stdcall window_setRect(void*, int, int, int, int)
{
    __asm
    {
        pop     eax        //ret addr
        pop     ecx        //this
        push    eax
        mov     edx, [ecx]
        jmp     dword ptr [edx + 28h]
    }
}

__declspec(naked) void __stdcall window_setText(void*, const char*, int)
{
    __asm
    {
        mov     eax, [esp + 0Ch]
        mov     ecx, [esp + 4]
        push    eax
        mov     edx, [esp + 4 + 8]
        push    edx
        mov     eax, 004D0AF0h
        call    eax
        ret     0Ch
    }
}

const char szAmount[] = "Amount";
const char szTimer[] = "Timer";
const char szAllianceState[] = "Alliance State";
const char szTargetPlayer[] = "Target Player";
const char szQuantity[] = "Quantity";
const char szOptions[] = "Options";

__declspec(naked) void __fastcall flush_ai_trigger_dropdown(void*)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        lea     edx, [esi + 0ED0h]
        mov     ecx, esi
        push    edx
        mov     eax, 00428520h
        call    eax

        mov     ecx, [esi + 918h]
        lea     eax, [esi + 0ED0h]
        push    eax
        push    ecx
        mov     ecx, esi
        mov     eax, 00529E50h
        call    eax

        pop     esi
        ret
    }
}

void(__thiscall* window_dropdown_addText) (void*, const char*, int) =
    (void(__thiscall*) (void*, const char*, int)) 0x004C82A0;

extern char** terrain_names;
extern int terrains_loaded;

void* __stdcall getEffectParams_hook(void* _this, effect* e)
{
    //quantity
    window_setText(*(void**)((int)_this + 0xE68), szQuantity, 0);
    //quantity hlp
    window_setRect(*(void**)((int)_this + 0xE68), 0x181, 0x60, 0x96, 0x14);
    //quantity type
    window_setRect(*(void**)((int)_this + 0xE6C), 0x181, 0x74, 0x5A, 0x14);
    switch (e->id)
    {
    case 2:            //research technology
        flush_ai_trigger_dropdown(_this);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Default", -1);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Allow stacking", 1);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Ignore civ restrictions", 2);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Stacking/ignore civ restrictions", 3);
        //ai trigger hlp
        window_setRect(*(void**)((int)_this + 0xECC), 0x190, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xECC), szOptions, 0);
        //ai trigger
        window_setRect(*(void**)((int)_this + 0xED0), 0x190, 0x16, 0xC8, 0x14);
        break;
    case 0xA:        //ai script goal
        writeByte(0x0052A8F6, 0x8D);
        writeByte(0x0052A8F7, 0x4C);
        writeDword(0x0052A8F8, 0x0A6A1024);
        flush_ai_trigger_dropdown(_this);
        setHook((void*)0x0052A8F6, (void*)0x0052A943);    //remove ai trigger dropdown init
        //ai trigger hlp
        window_setRect(*(void**)((int)_this + 0xECC), 4, 0x3C, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xECC), "AI Trigger Number", 0);
        //ai trigger
        window_setRect(*(void**)((int)_this + 0xED0), 4, 0x50, 0xC8, 0x14);
        break;
    case 0x2A:        //change unit data
    case 0x2B:        //change property object
    case 0x2D:        //change unit variable
        //message hlp
        window_setRect(*(void**)((int)_this + 0xEB4), 568, 2, 0x96, 0x14);
        //message
        window_setRect(*(void**)((int)_this + 0xEB8), 568, 0x16, 0x14A, 0x3C);
        //obj list type hlp
        window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
        //obj list type
        window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
        break;
    case 0x2E:        //terrain
        flush_ai_trigger_dropdown(_this);
        for (int i = 0; i < terrains_loaded; i++)
            if (((i < 104) || (i > 128)) && (i != 53))
                window_dropdown_addText(*(void**)((int)_this + 0xED0), terrain_names[i], i);
        //ai trigger hlp
        window_setRect(*(void**)((int)_this + 0xECC), 0x182, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xECC), "Terrain", 0);
        //ai trigger
        window_setRect(*(void**)((int)_this + 0xED0), 0x182, 0x16, 0xC8, 0x14);
        break;
    /*case 0x30:        //command
        flush_ai_trigger_dropdown(_this);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), get_command_name(RGE_COMMAND_STOP), RGE_COMMAND_STOP);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), get_command_name(RGE_COMMAND_GUARD), RGE_COMMAND_GUARD);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), get_command_name(RGE_COMMAND_FOLLOW), RGE_COMMAND_FOLLOW);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), get_command_name(RGE_COMMAND_FORMATION), RGE_COMMAND_FORMATION);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), get_command_name(TRIBE_COMMAND_RESEARCH), TRIBE_COMMAND_RESEARCH);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), get_command_name(TRIBE_COMMAND_ATTACK_GROUND), TRIBE_COMMAND_ATTACK_GROUND);
        //ai trigger hlp
        window_setRect(*(void**)((int)_this + 0xECC), 0x238, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xECC), "Command", 0);
        //ai trigger
        window_setRect(*(void**)((int)_this + 0xED0), 0x238, 0x16, 0xC8, 0x14);
        //quantity hlp
        window_setRect(*(void**)((int)_this + 0xE68), 0x238, 0x60, 0x96, 0x14);
        //quantity type
        window_setRect(*(void**)((int)_this + 0xE6C), 0x238, 0x74, 0x5A, 0x14);
        break;*/
    case 0x12:        //change ownership
        //obj list type hlp
        window_setRect(*(void**)((int)_this + 0xE58), 0x181, 0x67, 0x96, 0x14);
        //obj list type
        window_setRect(*(void**)((int)_this + 0xE5C), 0x181, 0x7B, 0x96, 0x14);
        break;
    case 0x16:      //freeze unit
        flush_ai_trigger_dropdown(_this);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Freeze unit", -1);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Aggressive", 1);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Defensive", 2);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "Stand Ground", 3);
        window_dropdown_addText(*(void**)((int)_this + 0xED0), "No Attack", 4);
        //ai trigger hlp
        window_setRect(*(void**)((int)_this + 0xECC), 0x238, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xECC), "Stance", 0);
        //ai trigger
        window_setRect(*(void**)((int)_this + 0xED0), 0x238, 0x16, 0xC8, 0x14);
        break;
    default:
        //timer hlp
        window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 2, 0x96, 0x14);
        //timer
        window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x16, 0x5A, 0x14);
        //obj list type hlp
        window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
        //obj list type
        window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
        //message hlp
        window_setRect(*(void**)((int)_this + 0xEB4), 0xE6, 0x41, 0x96, 0x14);
        //message
        window_setRect(*(void**)((int)_this + 0xEB8), 0xE6, 0x55, 0x14A, 0x3C);
        break;
    }
    return _this;
}

__declspec(naked) void __fastcall flush_ai_signal_dropdown(void*)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        lea     edx, [esi + 0ED8h]
        mov     ecx, esi
        push    edx
        mov     eax, 00428520h
        call    eax

        mov     ecx, [esi + 918h]
        lea     eax, [esi + 0ED8h]
        push    eax
        push    ecx
        mov     ecx, esi
        mov     eax, 00529E50h
        call    eax

        pop     esi
        ret
    }
}

const char* var_names[] =
{
    "HP",
    "HPPercent",
    "SP",
    "SPPercent",
    "ReloadCooldown",
    "ReloadCooldownPercent",
    "Resources",
    "Garrison",
    "MiscCounter1",
    "MiscCounter2",
    "MiscCounter3",
    "MiscCounter4",
    "MiscCounter5"
    //"Action"
};

const char* civ_names[] =
{
    "Galactic Empire",
    "Gungans",
    "Rebel Alliance",
    "Royal Naboo",
    "Trade Federation",
    "Wookies",
    "Republic",
    "Confederacy",
    "Zann Consortium",
    "Geonosians",
    "First Order",
    "Resistance",
    "Imperial Remnant",
    "New Republic",
    "Black Sun"
};

void* __stdcall getConditionParams_hook(void* _this, condition* c)
{
    int civ_count = version_for_editor == VER_EF ? CIV_COUNT : 8;

    switch (c->id)
    {
    case 5:            //objects in area
        flush_ai_signal_dropdown(_this);
        window_dropdown_addText(*(void**)((int)_this + 0xED8), "Default", -1);
        window_dropdown_addText(*(void**)((int)_this + 0xED8), "Ungarrisoned only", 1);
        window_dropdown_addText(*(void**)((int)_this + 0xED8), "Garrisoned only", 2);
        //ai signal hlp
        window_setRect(*(void**)((int)_this + 0xED4), 0x240, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xED4), szOptions, 0);
        //ai signal
        window_setRect(*(void**)((int)_this + 0xED8), 0x240, 0x16, 0xC8, 0x14);
        break;
    case 0xC:        //ai signal
        writeByte(0x0052A977, 0x8D);
        writeDword(0x0052A978, 0x6A102444);
        flush_ai_signal_dropdown(_this);
        setHook((void*)0x0052A977, (void*)0x0052A9C4);
        //ai signal hlp
        window_setRect(*(void**)((int)_this + 0xED4), 4, 0x3C, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xED4), "AI Signal Value", 0);
        //ai signal
        window_setRect(*(void**)((int)_this + 0xED8), 4, 0x50, 0xC8, 0x14);
        break;
    case 0x1A:        //alliance state
        //timer hlp
        window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 0x32, 0x96, 0x14);
        window_setText(*(void**)((int)_this + 0xE28), szTargetPlayer, 0);
        //timer
        window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x46, 0x5A, 0x14);

        flush_ai_signal_dropdown(_this);
        window_dropdown_addText(*(void**)((int)_this + 0xED8), "Ally", 0);
        window_dropdown_addText(*(void**)((int)_this + 0xED8), "Neutral", 1);
        window_dropdown_addText(*(void**)((int)_this + 0xED8), "Enemy", 3);
        //ai signal hlp
        window_setRect(*(void**)((int)_this + 0xED4), 0x190, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xED4), "Alliance State", 0);
        //ai signal
        window_setRect(*(void**)((int)_this + 0xED8), 0x190, 0x16, 0xC8, 0x14);
        break;
    case 0x1B:        //var GE
    case 0x1C:        //var E
        flush_ai_signal_dropdown(_this);
        for (int i = 0; i < sizeof(var_names) / sizeof(var_names[0]); i++)
            window_dropdown_addText(*(void**)((int)_this + 0xED8), var_names[i], i);

        //timer hlp
        window_setRect(*(void**)((int)_this + 0xE28), 0x240, 0x3C, 0x96, 0x14);
        window_setText(*(void**)((int)_this + 0xE28), szAmount, 0);
        //timer
        window_setRect(*(void**)((int)_this + 0xE2C), 0x240, 0x50, 0x5A, 0x14);
        //ai signal hlp
        window_setRect(*(void**)((int)_this + 0xED4), 0x240, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xED4), "Variable", 0);
        //ai signal
        window_setRect(*(void**)((int)_this + 0xED8), 0x240, 0x16, 0xC8, 0x14);
        //quantity
        window_setText(*(void**)((int)_this + 0xE68), szQuantity, 0);

        //
        //window_setText(*(void**)((int)_this + 0xED8), szQuantity, 0);

        break;
    case 0x1D:        //player civ
        flush_ai_signal_dropdown(_this);
        for (int i = 0; i < civ_count; i++)
            window_dropdown_addText(*(void**)((int)_this + 0xED8), civ_names[i], i + 1);

        //ai signal hlp
        window_setRect(*(void**)((int)_this + 0xED4), 0x240, 2, 0xC8, 0x14);
        window_setText(*(void**)((int)_this + 0xED4), "Civilization", 0);
        //ai signal
        window_setRect(*(void**)((int)_this + 0xED8), 0x240, 0x16, 0xC8, 0x14);
        break;
    default:
        //timer hlp
        window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 2, 0x96, 0x14);
        window_setText(*(void**)((int)_this + 0xE28), szTimer, 0);
        //window_setText(*(void**)((int)_this + 0xE28), 0, 10723);
        //timer
        window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x16, 0x5A, 0x14);
        //obj list type hlp
        window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
        //obj list type
        window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
        //quantity
        window_setText(*(void**)((int)_this + 0xE68), szQuantity, 0);
        break;
    }
    return _this;
}

__declspec(naked) void getEffectParams_new()
{
    __asm
    {
        mov     eax, [esp + 4] //params
        push    eax
        push    ecx
        call    getEffectParams_hook
        mov     ecx, eax
        mov     eax, 0053C920h
        jmp     eax
    }
}

__declspec(naked) void getConditionParams_new()
{
    __asm
    {
        mov     eax, [esp + 4] //params
        push    eax
        push    ecx
        call    getConditionParams_hook
        mov     ecx, eax
        mov     eax, 0053C310h
        jmp     eax
    }
}

__declspec(naked) void editor_selection_box() //0052DED4
{
    __asm
    {
        cmp     byte ptr [eax + 70h], 3
        setnz   dl
        or      dl, 2
        mov     [eax + 70h], dl
        mov     edx, 0052DEE2h
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setEditorEnhHooks(int ver)
{
    setHook((void*)0x00618FEE, noTerrainRestrictionHook);
    setHook((void*)0x00618F90, noGridHook);
    setHook((void*)0x0049539F, removeUnitsFix);

#ifndef _CC_COMPATIBLE
    setHook((void*)0x0052A977, (void*)0x0052A9C4);    //remove ai signal dropdown init
    setHook((void*)0x0052A8F6, (void*)0x0052A943);    //remove ai trigger dropdown init

    writeDword(0x0053C2E4, (DWORD)&getEffectParams_new - 0x0053C2E8);
    writeDword(0x0053DD6B, (DWORD)&getConditionParams_new - 0x0053DD6F);
    writeDword(0x0053C26F, (DWORD)&getConditionParams_new - 0x0053C273);
#endif

    writeByte(0x00496A0B, 0xEB); //terrain paint crash

    setHook((void*)0x0052DED4, editor_selection_box);
    writeData(0x0052E03D, "\xB8\x01\x00\x00\x00", 5);
    //writeByte(0x0052E04D, 0x10);

    version_for_editor = ver;
}
#pragma optimize( "", on )
