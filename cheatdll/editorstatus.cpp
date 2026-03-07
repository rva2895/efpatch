#include "stdafx.h"
#include "editorstatus.h"
#include "registry.h"
#include "cliff.h"
#include <ddraw.h>

#define RECT_RIGHT_OFFSET 420

#define RECT_X 300
#define RECT_Y 40

void DrawText_outline(HDC hdc, RECT* r, const char* str)
{
    HANDLE hOld = SelectObject(hdc, RGE_Base_Game__get_font(*base_game, 26)->font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    r->left--;    r->top--; r->right--; r->bottom--;
    //DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->left += 2;    r->right += 2;
    //DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->top += 2;    r->bottom += 2;
    //DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->left -= 2;    r->right -= 2;
    DrawTextA(hdc, str, strlen(str), r, DT_LEFT);
    r->top -= 2;    r->bottom -= 2;
    r->left++;    r->top++; r->right++; r->bottom++;
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextA(hdc, str, strlen(str), r, DT_LEFT);
    SelectObject(hdc, hOld);
}

extern int cliff_type;
extern int terrain_paint_mode;
extern int placementSettings;

std::string rms_error_1;
std::string rms_error_2;

void __stdcall editorstatus_paint(TRIBE_Screen_Sed* screen_sed)
{
    IDirectDrawSurface* s = screen_sed->render_area->DrawSurface;

    HDC hdc;
    s->GetDC(&hdc);

    int r_resolution_shift = (*base_game)->prog_info->game_wid < 1280 ? 50 : 0;
    int r_left_shift = (cd.largeText ? 90 : 50) - r_resolution_shift;

    RECT r;
    r.left = (*base_game)->prog_info->game_wid - RECT_RIGHT_OFFSET - r_left_shift;
    r.right = (*base_game)->prog_info->game_wid - 68;
    r.top = cd.largeText ? 4 : 8;
    r.bottom = RECT_Y + 8;
    char buf[0x100];

    std::string status_On = get_string(1520);
    std::string status_Off = get_string(1521);

    std::string& status_collision = (placementSettings & 1) ? status_Off : status_On;
    std::string& status_grid = (placementSettings & 2) ? status_Off : status_On;

    snprintf(buf, _countof(buf), get_string(1522), //"Grid placement: %s\nCollision enabled: %s"
        status_grid.c_str(), status_collision.c_str());
    DrawText_outline(hdc, &r, buf);

    int cliff_string_id;
    switch (cliff_type)
    {
    case 0x108:
        cliff_string_id = 1680; //"Standard";
        break;
    case 3971:
        cliff_string_id = 1681; //"Gray 1";
        break;
    case 3981:
        cliff_string_id = 1682; //"Gray 2";
        break;
    case 3991:
        cliff_string_id = 1683; //"Brown";
        break;
    case 4196:
        cliff_string_id = 1684; //"Jungle";
        break;
    case 4206:
        cliff_string_id = 1685; //"Red";
        break;
    case 4216:
        cliff_string_id = 1686; //"Sand";
        break;
    case 4226:
        cliff_string_id = 1687; //"Snow";
        break;
    case 4236:
        cliff_string_id = 1688; //"Volcanic";
        break;
    default:
        cliff_string_id = 1531; //"Unknown"
        break;
    }

    std::string cliff_txt = get_string(cliff_string_id);

    /*char* status_terrain;
    if (terrain_paint_mode)
        status_terrain = status_On;
    else
        status_terrain = status_Off;*/

    snprintf(buf, _countof(buf), get_string(1523), cliff_txt.c_str()); //"Cliff: %s"

    r.right += 160 + r_left_shift;
    r.left += 160 + r_left_shift;
    DrawText_outline(hdc, &r, buf);

    //rms stuff
    r.left = 600;
    r.right = (*base_game)->prog_info->game_wid;
    snprintf(buf, _countof(buf), "%s%s", rms_error_1.c_str(), rms_error_2.c_str());
    DrawText_outline(hdc, &r, buf);
    //

    s->ReleaseDC(hdc);
}

const int cliff_types_ef[] = { 0x108, 3971, 3981, 3991, 4196, 4206, 4216, 4226, 4236, 0 };
int current_cliff_index = 0;
extern int cliff_type;
extern int placementSettings;

void __stdcall screen_sed_redraw(TRIBE_Screen_Sed* screen_sed)
{
    screen_sed->vfptr->handle_size(screen_sed, screen_sed->pnl_wid, screen_sed->pnl_hgt);
    screen_sed->vfptr->set_redraw(screen_sed, 1);
}

void __stdcall sed_keydown_jmp_ctrl_q_do(TRIBE_Screen_Sed* screen_sed)
{
    if (cd.gameVersion == VER_EF)
    {
        cliff_type = cliff_types_ef[++current_cliff_index];
        if (!cliff_type)
        {
            current_cliff_index = 0;
            cliff_type = cliff_types_ef[0];
        }
        setCliffType(cliff_type, (*base_game)->world->map);

        screen_sed_redraw(screen_sed);
    }
}

void __stdcall sed_keydown_jmp_ctrl_s_do(TRIBE_Screen_Sed* screen_sed)
{
    placementSettings++;
    if (placementSettings > 3)
        placementSettings = 0;

    screen_sed_redraw(screen_sed);
}

__declspec(naked) void sed_keydown_jmp_ctrl_q()
{
    __asm
    {
        push    esi
        call    sed_keydown_jmp_ctrl_q_do
        mov     eax, 0052E270h
        jmp     eax
    }
}

__declspec(naked) void sed_keydown_jmp_ctrl_s()
{
    __asm
    {
        push    esi
        call    sed_keydown_jmp_ctrl_s_do
        mov     eax, 0052E270h
        jmp     eax
    }
}

const DWORD sed_keydown_jmp[] =
{
    0x0052DECD,
    0x0052E02A,
    0x0052DFEA,
    0x0052DF6A,
    0x0052DEEA,
    0x0052DFAA,
    0x0052DF2A,
    0x0052E00A,
    0x0052DF0A,
    0x0052DF4A,
    0x0052DF8A,
    0x0052DFCA,
    0x0052E097,
    0x0052E075,
    0x0052E270,
    (DWORD)sed_keydown_jmp_ctrl_q,
    (DWORD)sed_keydown_jmp_ctrl_s
};

__declspec(naked) void screen_sed_draw_editorstatus() //00531271
{
    __asm
    {
        push    esi
        call    editorstatus_paint
        mov     ecx, [esi + 20h]
        call    TDrawArea__Unlock
        mov     eax, 00531276h
        jmp     eax
    }
}

__declspec(naked) void screen_sed_after_generate() //0052ED61
{
    __asm
    {
        mov     ecx, [edx + 34h]
        push    esi
        call    dword ptr [eax + 2Ch]
        push    ebp
        call    screen_sed_redraw
        mov     eax, 0052ED68h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setEditorStatusHooks()
{
    setHook((void*)0x00531271, screen_sed_draw_editorstatus);
    setHook((void*)0x0052ED61, screen_sed_after_generate);

    writeByte(0x0052E2B4 + 16, 15);
    writeByte(0x0052E2B4 + 18, 16);

    writeDword(0x0052DEC9, (DWORD)sed_keydown_jmp);
}
#pragma optimize( "", on )
