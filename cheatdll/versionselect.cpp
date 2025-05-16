#include "stdafx.h"

#include "versionselect.h"
#include "resource.h"
#include "configurator.h"
#include "registry.h"

extern CONFIG_DATA cd;

BOOL CALLBACK VersionSelectDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_APP + 1: //update settings
        if (cd.gameVersion == VER_CC)
        {
            CheckDlgButton(hWndDlg, IDC_RADIO_X1, BST_CHECKED);
            CheckDlgButton(hWndDlg, IDC_RADIO_X2, BST_UNCHECKED);
        }
        else if (cd.gameVersion == VER_EF)
        {
            CheckDlgButton(hWndDlg, IDC_RADIO_X1, BST_UNCHECKED);
            CheckDlgButton(hWndDlg, IDC_RADIO_X2, BST_CHECKED);
        }

        if (cd.askAtStartup)
            CheckDlgButton(hWndDlg, IDC_CHECK_ALWAYSRUN, BST_UNCHECKED);
        else
            CheckDlgButton(hWndDlg, IDC_CHECK_ALWAYSRUN, BST_CHECKED);
        break;
    case WM_CLOSE:
        ExitProcess(0);
        break;
    case WM_INITDIALOG:
        log("Version select dialog opened");
        if (cd.gameVersion == VER_CC)
            CheckDlgButton(hWndDlg, IDC_RADIO_X1, BST_CHECKED);
        if (cd.gameVersion == VER_EF)
            CheckDlgButton(hWndDlg, IDC_RADIO_X2, BST_CHECKED);
        if (!cd.askAtStartup)
            CheckDlgButton(hWndDlg, IDC_CHECK_ALWAYSRUN, BST_CHECKED);

        break;
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == BN_CLICKED)
            switch (LOWORD(wParam))
            {
            case IDOK: //Run
                cd.askAtStartup = !IsDlgButtonChecked(hWndDlg, IDC_CHECK_ALWAYSRUN);
                if (IsDlgButtonChecked(hWndDlg, IDC_RADIO_X1))
                {
                    cd.gameVersion = VER_CC;
                    regSet(&cd);
                    EndDialog(hWndDlg, 0);
                }
                else if (IsDlgButtonChecked(hWndDlg, IDC_RADIO_X2))
                {
                    cd.gameVersion = VER_EF;
                    regSet(&cd);
                    EndDialog(hWndDlg, 0);
                }
                else
                    MessageBox(hWndDlg, "Please select game version", "Error", MB_ICONERROR);

                break;
            case IDCANCEL:
                ExitProcess(0);
                break;
            case IDC_BUTTON_CONFIG:
                launchConfigurator(hWndDlg);
                break;

            default:
                break;
            }
        else
            break;
    }
    default:
        return FALSE;
    }
    return TRUE;
}
