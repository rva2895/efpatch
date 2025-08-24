#include "stdafx.h"
#include "mousewheel.h"

UINT nScrollLines;

void get_wheel_scroll_lines()
{
    if (!SystemParametersInfoA(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0))
    {
        nScrollLines = 3;
    }
}

int __fastcall TTextPanel__mouse_wheel_action_new(TTextPanel* this_, DWORD dummy, unsigned int msg, unsigned int wParam, int lParam)
{
    UNREFERENCED_PARAMETER(dummy);
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(lParam);

    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (zDelta != 0)
    {
        if (nScrollLines != WHEEL_PAGESCROLL)
        {
            TTextPanel__scroll(this_, 0, (zDelta < 0 ? 1 : -1) * nScrollLines, 1);
        }
        else
        {
            TTextPanel__scroll(this_, zDelta < 0 ? 4 : 5, 0, 1);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void setMouseWheelHooks()
{
    get_wheel_scroll_lines();

    //mouse wheel list panel scroll
    setHook((void*)0x004D40B0, TTextPanel__mouse_wheel_action_new);
    setHook((void*)0x004CBAA0, TTextPanel__mouse_wheel_action_new);
}
