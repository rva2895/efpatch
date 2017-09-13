#include "stdafx.h"

#include "editorstatus.h"
#include <ddraw.h>

LPDIRECTDRAWSURFACE7* s = 0;
LPDIRECTDRAWSURFACE7* s2 = 0;
LPDIRECTDRAWSURFACE7* s_old = 0;

extern HWND hWnd_main;

extern bool isEditor;

int counter_ = 0;

HDC hdc_background;
HDC hdc_full;

HBITMAP hbm_background;
HBITMAP hbm_full;
HANDLE h_background;
HANDLE h_full;

HFONT hFont;

bool editorstatus_isValid = false;
bool background_initialised = false;

#define RECT_RIGHT_OFFSET 380

#define RECT_X 300
#define RECT_Y 40

extern int placementSettings;

void initBackground(HDC hdc)
{
	hdc_background = CreateCompatibleDC(hdc);
	hdc_full = CreateCompatibleDC(hdc);
	hbm_background = CreateCompatibleBitmap(hdc, RECT_X, RECT_Y);
	hbm_full = CreateCompatibleBitmap(hdc, RECT_X, RECT_Y);

	h_background = SelectObject(hdc_background, hbm_background);

	BitBlt(hdc_background, 0, 0, RECT_X, RECT_Y, hdc, getWindowX() - RECT_RIGHT_OFFSET, 5, SRCCOPY);

	hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

	background_initialised = true;
	editorstatus_isValid = false;

	SelectObject(hdc_background, h_background);
}

void DrawText_outline(HDC hdc, RECT* r, const char* str)
{
	HANDLE hOld = SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 0, 0));
	r->left--;	r->top--; r->right--; r->bottom--;
	//DrawText(hdc, str, strlen(str), r, DT_LEFT);
	r->left += 2;	r->right += 2;
	//DrawText(hdc, str, strlen(str), r, DT_LEFT);
	r->top += 2;	r->bottom += 2;
	//DrawText(hdc, str, strlen(str), r, DT_LEFT);
	r->left -= 2;	r->right -= 2;
	DrawText(hdc, str, strlen(str), r, DT_LEFT);
	r->top -= 2;	r->bottom -= 2;
	r->left++;	r->top++; r->right++; r->bottom++;
	SetTextColor(hdc, RGB(255, 255, 255));
	DrawText(hdc, str, strlen(str), r, DT_LEFT);
	SelectObject(hdc, hOld);
}

char status_On[] = "On";
char status_Off[] = "Off";

extern int cliff_type;

void paintOnScreen(HDC hdc)
{
	if (!background_initialised)
		initBackground(hdc);

	h_background = SelectObject(hdc_background, hbm_background);
	h_full = SelectObject(hdc_full, hbm_full);

	if (!editorstatus_isValid)
	{
		counter_++;

		BitBlt(hdc_full, 0, 0, RECT_X, RECT_Y, hdc_background, 0, 0, SRCCOPY);

		RECT r;
		r.left = 2;
		r.right = RECT_X - 2;
		r.top = 2;
		r.bottom = RECT_Y - 2;
		char buf[0x100];
		char* status_grid = status_On;
		char* status_collision = status_On;
		if (placementSettings & 1)
			status_collision = status_Off;
		if (placementSettings & 2)
			status_grid = status_Off;

		sprintf(buf, "Grid placement: %s\nCollision enabled: %s", status_grid, status_collision);

		r.right -= 160;
		DrawText_outline(hdc_full, &r, buf);

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
		default:
			cliff_txt = "Error!";
			break;
		}

		sprintf(buf, "Cliff: %s", cliff_txt);
		r.right += 160;
		r.left += 160;
		DrawText_outline(hdc_full, &r, buf);

		editorstatus_isValid = true;
	}

	BitBlt(hdc, getWindowX() - RECT_RIGHT_OFFSET, 5, RECT_X, RECT_Y, hdc_full, 0, 0, SRCCOPY);

	SelectObject(hdc_background, h_background);
	SelectObject(hdc_full, h_full);
}

bool paint_test()
{
	HDC hdc;
	if (isEditor)
	{
		if (s_old)
			s = s_old;
		if (!*s)
			s = s2;

		(*s)->GetDC(&hdc);
		paintOnScreen(hdc);
		(*s)->ReleaseDC(hdc);

		return true;
	}
	return false;
}

int surf_flag;
int edit_screen_flag = 0;

__declspec(naked) void edit_screen_hook() //00531012
{
	__asm
	{
		//mov		eax, 1
		//mov		edit_screen_flag, eax
		//mov		eax, s2
		//mov		s, eax
		mov		dword ptr[esp + 1010h], 0FFFFFFFFh
		push	0053101Dh
		ret
	}
}

__declspec(naked) void createsurface_hook() //00472D56, 0047188E
{
	__asm
	{
		push    edi
		mov		s2, edi
		push    edx
		push    eax
		call	getWindowY
		cmp		dword ptr[edx + 8], eax
		setz	al
		and		eax, 0FFh
		mov		surf_flag, eax
		call    dword ptr[ecx + 18h]
		mov		ecx, surf_flag
		test	ecx, ecx
		jz		_not_surface
		//mov		ecx, edit_screen_flag
		//test	ecx, ecx
		//jnz		_not_surface
		mov		ecx, s2
		mov		edx, s
		mov		s, ecx
		mov		s_old, edx
		_not_surface :
		//mov		ecx, 0
		//mov		edit_screen_flag, ecx
		//push	00471894h
		push	00472D5Ch
		ret
	}
}

void setEditorStatusHooks()
{
	setHook((void*)0x00472D56, &createsurface_hook);
	setHook((void*)0x00531012, &edit_screen_hook);
}
