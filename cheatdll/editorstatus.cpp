#include "stdafx.h"

#include "editorstatus.h"
#include <ddraw.h>
#include <string>

extern bool isEditor;

TPanel* window_editorbk = 0;

bool editorstatus_isValid = false;
bool background_initialized = false;

HFONT hFont;

#define RECT_RIGHT_OFFSET 380

#define RECT_X 300
#define RECT_Y 40

extern int placementSettings;

void initBackground()
{
    hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

    background_initialized = true;
    editorstatus_isValid = false;
}

void DrawText_outline(HDC hdc, RECT* r, const char* str)
{
    HANDLE hOld = SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    r->left--;    r->top--; r->right--; r->bottom--;
    //DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->left += 2;    r->right += 2;
    //DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->top += 2;    r->bottom += 2;
    //DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->left -= 2;    r->right -= 2;
    DrawText(hdc, str, strlen(str), r, DT_LEFT);
    r->top -= 2;    r->bottom -= 2;
    r->left++;    r->top++; r->right++; r->bottom++;
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawText(hdc, str, strlen(str), r, DT_LEFT);
    SelectObject(hdc, hOld);
}

const char status_On[] = "On";
const char status_Off[] = "Off";

extern int cliff_type;
extern int terrain_paint_mode;

std::string rms_error_1;
std::string rms_error_2;

void __stdcall paintOnScreen(LPDIRECTDRAWSURFACE7 s)
{
    if (!background_initialized)
        initBackground();

    HDC hdc;
    s->GetDC(&hdc);

    RECT r;
    r.left = getWindowX() - RECT_RIGHT_OFFSET;
    r.right = RECT_X + r.left;
    r.top = 8;
    r.bottom = RECT_Y + 8;
    char buf[0x100];
    const char* status_grid = status_On;
    const char* status_collision = status_On;
    if (placementSettings & 1)
        status_collision = status_Off;
    if (placementSettings & 2)
        status_grid = status_Off;

    snprintf(buf, _countof(buf), "Grid placement: %s\nCollision enabled: %s", status_grid, status_collision);
    DrawText_outline(hdc, &r, buf);

    char* cliff_txt;
    switch (cliff_type)
    {
    case 0x108:
        cliff_txt = "Standard";
        break;
    case 3971:
        cliff_txt = "Gray 1";
        break;
    case 3981:
        cliff_txt = "Gray 2";
        break;
    case 3991:
        cliff_txt = "Brown";
        break;
    case 4196:
        cliff_txt = "Jungle";
        break;
    case 4206:
        cliff_txt = "Red";
        break;
    case 4216:
        cliff_txt = "Sand";
        break;
    case 4226:
        cliff_txt = "Snow";
        break;
    case 4236:
        cliff_txt = "Volcanic";
        break;
    default:
        cliff_txt = "Error!";
        break;
    }

    /*char* status_terrain;
    if (terrain_paint_mode)
        status_terrain = status_On;
    else
        status_terrain = status_Off;*/

    snprintf(buf, _countof(buf), "Cliff: %s", cliff_txt);

    r.right += 160;
    r.left += 160;
    DrawText_outline(hdc, &r, buf);

    //rms stuff
    r.left = 600;
    r.right = getWindowX();
    snprintf(buf, _countof(buf), "%s%s", rms_error_1.c_str(), rms_error_2.c_str());
    DrawText_outline(hdc, &r, buf);
    //

    editorstatus_isValid = true;

    s->ReleaseDC(hdc);
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __fastcall window_getSurface(void* wnd)
{
    __asm
    {
        mov     eax, [ecx + 3Ch]    //surface
        ret
    }
}
#pragma warning(pop)

__declspec(naked) void hook2() //00531271
{
    __asm
    {
        mov     window_editorbk, esi
        push    ecx
        call    window_getSurface
        push    eax
        call    paintOnScreen
        pop     ecx
        mov     eax, 00531276h
        push    eax
        mov     eax, 00472B20h
        jmp     eax
    }
}

void setEditorStatusHooks()
{
    setHook((void*)0x00531271, hook2);
}
