#include "stdafx.h"
#include "moddialog.h"
#include "registry.h"
#include <CommCtrl.h>

extern CONFIG_DATA cd;

HWND CreateListView(HWND hwndParent)
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rcClient;                       // The parent window's client area.

    GetClientRect(hwndParent, &rcClient);

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindow(WC_LISTVIEW,
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LVS_REPORT | LVS_EDITLABELS,
        0, 0,
        100, 100,
        hwndParent,
        NULL,
        NULL,
        NULL);

    ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_CHECKBOXES);

    return (hWndListView);
}

HWND hModList;

BOOL CALLBACK ModDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWndDlg, 0);
        break;
    case WM_INITDIALOG:
    {
        hModList = CreateListView(hWndDlg);
    }
    break;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
            switch (LOWORD(wParam))
            {
            case IDOK: //Patch

                EndDialog(hWndDlg, 0);

                break;
            case IDC_BUTTON_CANCEL:
                EndDialog(hWndDlg, 0);

                break;

            default:
                break;
            }
        else
            break;
    default:
        return false;
    }
    return true;
}