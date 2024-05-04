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

const char szAmount[] = "Amount";
const char szOptions[] = "Options";

extern char** terrain_names;
extern int terrains_loaded;

void init_ai_trigger_dropdown(TDropDownPanel* ai_trigger_dropdown)
{
    char b[0x100];
    std::string ai_trigger_str = get_string(10708); //"AI Trigger"
    for (int i = 1; i <= 256; i++)
    {
        sprintf(b, "%s %d", ai_trigger_str.c_str(), i);
        TDropDownPanel__append_line(ai_trigger_dropdown, b, i);
    }
}

void* __stdcall getEffectParams_hook(TRIBE_Screen_Sed* scr_sed, effect* e)
{
    //set_rect -> set_fixed_position
    //set_text -> set_text3

    //panels
    TTextPanel* ai_trigger_text = *(TTextPanel**)((DWORD)scr_sed + 0xECC);
    TDropDownPanel* ai_trigger_dropdown = *(TDropDownPanel**)((DWORD)scr_sed + 0xED0);

    TTextPanel* message_text = *(TTextPanel**)((DWORD)scr_sed + 0xEB4);
    TEditPanel* message_edit = *(TEditPanel**)((DWORD)scr_sed + 0xEB8);

    TTextPanel* obj_list_type_text = *(TTextPanel**)((DWORD)scr_sed + 0xE58);
    TDropDownPanel* obj_list_type_dropdown = *(TDropDownPanel**)((DWORD)scr_sed + 0xE5C);

    TTextPanel* timer_text = *(TTextPanel**)((DWORD)scr_sed + 0xE28);
    TEditPanel* timer_edit = *(TEditPanel**)((DWORD)scr_sed + 0xE2C);

    TTextPanel* quantity_text = *(TTextPanel**)((DWORD)scr_sed + 0xE68);
    TEditPanel* quantity_edit = *(TEditPanel**)((DWORD)scr_sed + 0xE6C);

    //quantity
    quantity_text->vfptr->set_text2(quantity_text, 10731); //"Quantity"
    quantity_text->vfptr->set_fixed_position(quantity_text, 0x181, 0x60, 0x96, 0x14);
    quantity_edit->vfptr->set_fixed_position(quantity_edit, 0x181, 0x74, 0x5A, 0x14);

    switch (e->id)
    {
    case 2:            //research technology
        TDropDownPanel__empty_list(ai_trigger_dropdown);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Default", -1);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Allow stacking", 1);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Ignore civ restrictions", 2);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Stacking/ignore civ restrictions", 3);

        ai_trigger_text->vfptr->set_fixed_position(ai_trigger_text, 0x190, 2, 0xC8, 0x14);
        ai_trigger_text->vfptr->set_text3(ai_trigger_text, szOptions, NULL);

        ai_trigger_dropdown->vfptr->set_fixed_position(ai_trigger_dropdown, 0x190, 0x16, 0xC8, 0x14);
        break;
    case 0xA:        //ai script goal
        TDropDownPanel__empty_list(ai_trigger_dropdown);
        init_ai_trigger_dropdown(ai_trigger_dropdown);

        ai_trigger_text->vfptr->set_fixed_position(ai_trigger_text, 4, 0x3C, 0xC8, 0x14);
        ai_trigger_text->vfptr->set_text2(ai_trigger_text, 10739); //"AI Trigger Number"

        ai_trigger_dropdown->vfptr->set_fixed_position(ai_trigger_dropdown, 4, 0x50, 0xC8, 0x14);
        break;
    case 0x2A:        //change own object master
    case 0x2B:        //change player object master
    case 0x2D:        //change object variable
        message_text->vfptr->set_fixed_position(message_text, 568, 2, 0x96, 0x14);

        message_edit->vfptr->set_fixed_position(message_edit, 568, 0x16, 0x14A, 0x3C);

        obj_list_type_text->vfptr->set_fixed_position(obj_list_type_text, 0xE6, 0x3C, 0x96, 0x14);

        obj_list_type_dropdown->vfptr->set_fixed_position(obj_list_type_dropdown, 0xE6, 0x50, 0x96, 0x14);
        break;
    case 0x2E:        //terrain
        TDropDownPanel__empty_list(ai_trigger_dropdown);
        for (int i = 0; i < terrains_loaded; i++)
            if (((i < 104) || (i > 128)) && (i != 53))
                TDropDownPanel__append_line(ai_trigger_dropdown, terrain_names[i], i);

        ai_trigger_text->vfptr->set_fixed_position(ai_trigger_text, 0x182, 2, 0xC8, 0x14);
        ai_trigger_text->vfptr->set_text2(ai_trigger_text, 10684); //"Terrain"

        ai_trigger_dropdown->vfptr->set_fixed_position(ai_trigger_dropdown, 0x182, 0x16, 0xC8, 0x14);
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
        obj_list_type_text->vfptr->set_fixed_position(obj_list_type_text, 0x181, 0x67, 0x96, 0x14);

        obj_list_type_dropdown->vfptr->set_fixed_position(obj_list_type_dropdown, 0x181, 0x7B, 0x96, 0x14);
        break;
    case 0x16:      //freeze unit
        TDropDownPanel__empty_list(ai_trigger_dropdown);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Freeze unit", -1);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Aggressive", 1);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Defensive", 2);
        TDropDownPanel__append_line(ai_trigger_dropdown, "Stand Ground", 3);
        TDropDownPanel__append_line(ai_trigger_dropdown, "No Attack", 4);

        ai_trigger_text->vfptr->set_fixed_position(ai_trigger_text, 0x238, 2, 0xC8, 0x14);
        ai_trigger_text->vfptr->set_text3(ai_trigger_text, "Stance", NULL);

        ai_trigger_dropdown->vfptr->set_fixed_position(ai_trigger_dropdown, 0x238, 0x16, 0xC8, 0x14);
        break;
    default:

        timer_text->vfptr->set_fixed_position(timer_text, 0xE6, 2, 0x96, 0x14);

        timer_edit->vfptr->set_fixed_position(timer_edit, 0xE6, 0x16, 0x5A, 0x14);

        obj_list_type_text->vfptr->set_fixed_position(obj_list_type_text, 0xE6, 0x3C, 0x96, 0x14);

        obj_list_type_dropdown->vfptr->set_fixed_position(obj_list_type_dropdown, 0xE6, 0x50, 0x96, 0x14);

        message_text->vfptr->set_fixed_position(message_text, 0xE6, 0x41, 0x96, 0x14);

        message_edit->vfptr->set_fixed_position(message_edit, 0xE6, 0x55, 0x14A, 0x3C);
        break;
    }
    return scr_sed;
}

void init_ai_signal_dropdown(TDropDownPanel* ai_signal_dropdown)
{
    char b[0x100];
    std::string ai_signal_str = get_string(10773); //"AI Signal"
    for (int i = 0; i < 256; i++)
    {
        sprintf(b, "%s %d", ai_signal_str.c_str(), i);
        TDropDownPanel__append_line(ai_signal_dropdown, b, i);
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
    "MiscCounter5",
    "Kills"
};

void* __stdcall getConditionParams_hook(TRIBE_Screen_Sed* scr_sed, condition* c)
{
    int civ_count = version_for_editor == VER_EF ? CIV_COUNT : 8;

    //panels
    TTextPanel* ai_signal_text = *(TTextPanel**)((DWORD)scr_sed + 0xED4);
    TDropDownPanel* ai_signal_dropdown = *(TDropDownPanel**)((DWORD)scr_sed + 0xED8);

    TTextPanel* timer_text = *(TTextPanel**)((DWORD)scr_sed + 0xE28);
    TEditPanel* timer_edit = *(TEditPanel**)((DWORD)scr_sed + 0xE2C);

    TTextPanel* obj_list_type_text = *(TTextPanel**)((DWORD)scr_sed + 0xE58);
    TDropDownPanel* obj_list_type_dropdown = *(TDropDownPanel**)((DWORD)scr_sed + 0xE5C);

    TTextPanel* quantity_text = *(TTextPanel**)((DWORD)scr_sed + 0xE68);
    //TEditPanel* quantity_edit = *(TEditPanel**)((DWORD)scr_sed + 0xE6C);

    switch (c->id)
    {
    case 5:            //objects in area
        TDropDownPanel__empty_list(ai_signal_dropdown);
        TDropDownPanel__append_line(ai_signal_dropdown, "Default", -1);
        TDropDownPanel__append_line(ai_signal_dropdown, "Ungarrisoned only", 1);
        TDropDownPanel__append_line(ai_signal_dropdown, "Garrisoned only", 2);
        //ai signal hlp
        ai_signal_text->vfptr->set_fixed_position(ai_signal_text, 0x240, 2, 0xC8, 0x14);
        ai_signal_text->vfptr->set_text3(ai_signal_text, szOptions, NULL);
        //ai signal
        ai_signal_dropdown->vfptr->set_fixed_position(ai_signal_dropdown, 0x240, 0x16, 0xC8, 0x14);
        break;
    case 0xC:        //ai signal
        TDropDownPanel__empty_list(ai_signal_dropdown);
        init_ai_signal_dropdown(ai_signal_dropdown);
        //ai signal hlp
        ai_signal_text->vfptr->set_fixed_position(ai_signal_text, 4, 0x3C, 0xC8, 0x14);
        ai_signal_text->vfptr->set_text2(ai_signal_text, 10772); //"AI Signal Value"
        //ai signal
        ai_signal_dropdown->vfptr->set_fixed_position(ai_signal_dropdown, 4, 0x50, 0xC8, 0x14);
        break;
    case 0x1A:        //alliance state
        //timer hlp
        timer_text->vfptr->set_fixed_position(timer_text, 0xE6, 0x32, 0x96, 0x14);
        timer_text->vfptr->set_text2(timer_text, 10733); //"Target Player"
        //timer
        timer_edit->vfptr->set_fixed_position(timer_edit, 0xE6, 0x46, 0x5A, 0x14);

        TDropDownPanel__empty_list(ai_signal_dropdown);
        TDropDownPanel__append_line2(ai_signal_dropdown, 10701, 0); //"Ally"
        TDropDownPanel__append_line2(ai_signal_dropdown, 10702, 1); //"Neutral"
        TDropDownPanel__append_line2(ai_signal_dropdown, 10703, 3); //"Enemy"
        //ai signal hlp
        ai_signal_text->vfptr->set_fixed_position(ai_signal_text, 0x190, 2, 0xC8, 0x14);
        ai_signal_text->vfptr->set_text2(ai_signal_text, 10738); //"Alliance State"
        //ai signal
        ai_signal_dropdown->vfptr->set_fixed_position(ai_signal_dropdown, 0x190, 0x16, 0xC8, 0x14);
        break;
    case 0x1B:        //var GE
    case 0x1C:        //var E
        TDropDownPanel__empty_list(ai_signal_dropdown);
        for (int i = 0; i < sizeof(var_names) / sizeof(var_names[0]); i++)
            TDropDownPanel__append_line(ai_signal_dropdown, var_names[i], i);

        //timer hlp
        timer_text->vfptr->set_fixed_position(timer_text, 0x240, 0x3C, 0x96, 0x14);
        timer_text->vfptr->set_text3(timer_text, szAmount, 0);
        //timer
        timer_edit->vfptr->set_fixed_position(timer_edit, 0x240, 0x50, 0x5A, 0x14);
        //ai signal hlp
        ai_signal_text->vfptr->set_fixed_position(ai_signal_text, 0x240, 2, 0xC8, 0x14);
        ai_signal_text->vfptr->set_text3(ai_signal_text, "Variable", 0);
        //ai signal
        ai_signal_dropdown->vfptr->set_fixed_position(ai_signal_dropdown, 0x240, 0x16, 0xC8, 0x14);
        //quantity
        quantity_text->vfptr->set_text2(quantity_text, 10731); //"Quantity"

        //
        //window_setText(*(void**)((int)_this + 0xED8), szQuantity, 0);

        break;
    case 0x1D:        //player civ
        TDropDownPanel__empty_list(ai_signal_dropdown);
        for (int i = 0; i < civ_count; i++)
            TDropDownPanel__append_line2(ai_signal_dropdown, 10231 + i, i + 1);

        //ai signal hlp
        ai_signal_text->vfptr->set_fixed_position(ai_signal_text, 0x240, 2, 0xC8, 0x14);
        ai_signal_text->vfptr->set_text2(ai_signal_text, 10257); //"Civilization"
        //ai signal
        ai_signal_dropdown->vfptr->set_fixed_position(ai_signal_dropdown, 0x240, 0x16, 0xC8, 0x14);
        break;
    default:
        //timer hlp
        timer_text->vfptr->set_fixed_position(timer_text, 0xE6, 2, 0x96, 0x14);
        timer_text->vfptr->set_text2(timer_text, 10723); //"Timer"
        //window_setText(*(void**)((int)_this + 0xE28), 0, 10723);
        //timer
        timer_edit->vfptr->set_fixed_position(timer_edit, 0xE6, 0x16, 0x5A, 0x14);
        //obj list type hlp
        obj_list_type_text->vfptr->set_fixed_position(obj_list_type_text, 0xE6, 0x3C, 0x96, 0x14);
        //obj list type
        obj_list_type_dropdown->vfptr->set_fixed_position(obj_list_type_dropdown, 0xE6, 0x50, 0x96, 0x14);
        //quantity
        quantity_text->vfptr->set_text2(quantity_text, 10731); //"Quantity"
        break;
    }
    return scr_sed;
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
