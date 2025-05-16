#include "stdafx.h"

#include "configurator.h"
#include "registry.h"
#include "resource.h"
#include "resolution.h"
#include "palette.h"
#include "localisation.h"
#include "textrender.h"

#include <CommCtrl.h>

//std::vector<std::pair<std::string, std::string>> languages;

extern CONFIG_DATA cd;
extern const CONFIG_DATA cd_default;

void processIDOK(HWND hWnd)
{
    char buf[10];
    GetDlgItemText(hWnd, IDC_EDIT_DSH_NBUFS, buf, 10);
    sscanf_s(buf, "%d", &cd.nBufs);
    GetDlgItemText(hWnd, IDC_EDIT_DSH_DELAY, buf, 10);
    sscanf_s(buf, "%d", &cd.timeout);

    cd.useFPS = IsDlgButtonChecked(hWnd, IDC_CHECK_FPS);
    cd.useDShook = IsDlgButtonChecked(hWnd, IDC_CHECK_DSH);
    cd.askAtStartup = !IsDlgButtonChecked(hWnd, IDC_CHECK_ALWAYSRUN);
    cd.unlockResources = IsDlgButtonChecked(hWnd, IDC_CHECK_UNLOCKRES);
    cd.unlockObjects = IsDlgButtonChecked(hWnd, IDC_CHECK_UNLOCKOBJECTS);

    cd.crashReporting = IsDlgButtonChecked(hWnd, IDC_CHECK_CRASH);

    cd.largeMaps = IsDlgButtonChecked(hWnd, IDC_CHECK_MAPSIZE);

    if (cd.editorAutosave = IsDlgButtonChecked(hWnd, IDC_CHECK_EDITORAUTO))
    {
        char str[50];
        GetDlgItemText(hWnd, IDC_EDIT_EDITORAUTO, str, 50);
        sscanf_s(str, "%d", &cd.editorAutosaveInterval);
    }

    cd.windowMode = IsDlgButtonChecked(hWnd, IDC_CHECK_WNDMODE);

    if (IsDlgButtonChecked(hWnd, IDC_RADIO_X1))
        cd.gameVersion = VER_CC;
    if (IsDlgButtonChecked(hWnd, IDC_RADIO_X2))
        cd.gameVersion = VER_EF;

    if (IsDlgButtonChecked(hWnd, IDC_RADIO_EDITOR_LIST_1))
        cd.useAltCivLetter = 0;
    if (IsDlgButtonChecked(hWnd, IDC_RADIO_EDITOR_LIST_2))
        cd.useAltCivLetter = 1;

    if (cd.widescrnEnabled = IsDlgButtonChecked(hWnd, IDC_CHECK_WIDE))
    {
        char str[50];
        GetDlgItemText(hWnd, IDC_COMBO_SCREEN_SIZE, str, 50);
        sscanf_s(str, "%dx%d", &cd.xres, &cd.yres);
        cd.autoScreenSize = IsDlgButtonChecked(hWnd, IDC_CHECK_SCREEN_SIZE_AUTO);
    }

    cd.minimap7 = IsDlgButtonChecked(hWnd, IDC_CHECK_GREY);
    cd.largeText = IsDlgButtonChecked(hWnd, IDC_CHECK_LARGETEXT);
    cd.delinkVolume = IsDlgButtonChecked(hWnd, IDC_CHECK_MASTER_VOLUME);
    cd.keydown = IsDlgButtonChecked(hWnd, IDC_CHECK_KEYDOWN);

    cd.textRendering = IsDlgButtonChecked(hWnd, IDC_CHECK_TEXT_RENDERING);
    cd.chatBox = IsDlgButtonChecked(hWnd, IDC_CHECK_CHATBOX);
    cd.fog = IsDlgButtonChecked(hWnd, IDC_CHECK_FOG);

    //char* lang_str = (char*)malloc(0x100);
    //GetDlgItemText(hWnd, IDC_COMBO_LANG, lang_str, 0x100);
    //std::string lang_str_s = lang_str;
    //for (int i = 0; i < languages.size(); i++)
    //    if (languages[i].first == lang_str_s)
    //        cd.lang = languages[i].second;

    regSet(&cd);
}

void readSettingsToDialog(HWND hWnd)
{
    char buf[0x40];

    cd.useDShook = 0;

    CheckDlgButton(hWnd, IDC_CHECK_FPS, cd.useFPS);

    CheckDlgButton(hWnd, IDC_CHECK_DSH, cd.useDShook);

    CheckDlgButton(hWnd, IDC_CHECK_ALWAYSRUN, !cd.askAtStartup);
    CheckDlgButton(hWnd, IDC_CHECK_UNLOCKRES, cd.unlockResources);
    CheckDlgButton(hWnd, IDC_CHECK_UNLOCKOBJECTS, cd.unlockObjects);
    CheckDlgButton(hWnd, IDC_CHECK_EDITORAUTO, cd.editorAutosave);
    EnableWindow(GetDlgItem(hWnd, IDC_EDIT_EDITORAUTO), cd.editorAutosave);

    CheckDlgButton(hWnd, IDC_CHECK_CRASH, cd.crashReporting);

    CheckDlgButton(hWnd, IDC_CHECK_WIDE, cd.widescrnEnabled);
    EnableWindow(GetDlgItem(hWnd, IDC_COMBO_SCREEN_SIZE), cd.widescrnEnabled && !cd.autoScreenSize);
    EnableWindow(GetDlgItem(hWnd, IDC_CHECK_SCREEN_SIZE_AUTO), cd.widescrnEnabled);

    CheckDlgButton(hWnd, IDC_CHECK_WNDMODE, cd.windowMode);

    CheckDlgButton(hWnd, IDC_CHECK_MAPSIZE, cd.largeMaps);

    if (cd.gameVersion == VER_CC)
    {
        CheckDlgButton(hWnd, IDC_RADIO_X1, BST_CHECKED);
        CheckDlgButton(hWnd, IDC_RADIO_X2, BST_UNCHECKED);
    }
    if (cd.gameVersion == VER_EF)
    {
        CheckDlgButton(hWnd, IDC_RADIO_X1, BST_UNCHECKED);
        CheckDlgButton(hWnd, IDC_RADIO_X2, BST_CHECKED);
    }
    if (cd.useAltCivLetter)
    {
        CheckDlgButton(hWnd, IDC_RADIO_EDITOR_LIST_1, BST_UNCHECKED);
        CheckDlgButton(hWnd, IDC_RADIO_EDITOR_LIST_2, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(hWnd, IDC_RADIO_EDITOR_LIST_1, BST_CHECKED);
        CheckDlgButton(hWnd, IDC_RADIO_EDITOR_LIST_2, BST_UNCHECKED);
    }

    snprintf(buf, _countof(buf), "%d", cd.nBufs);
    SetDlgItemText(hWnd, IDC_EDIT_DSH_NBUFS, buf);
    snprintf(buf, _countof(buf), "%d", cd.timeout);
    SetDlgItemText(hWnd, IDC_EDIT_DSH_DELAY, buf);
    snprintf(buf, _countof(buf), "%d", cd.editorAutosaveInterval);
    SetDlgItemText(hWnd, IDC_EDIT_EDITORAUTO, buf);

    if (cd.widescrnEnabled)
    {
        snprintf(buf, _countof(buf), "%dx%d", cd.xres, cd.yres);
        SetDlgItemText(hWnd, IDC_COMBO_SCREEN_SIZE, buf);
        CheckDlgButton(hWnd, IDC_CHECK_SCREEN_SIZE_AUTO, cd.autoScreenSize);
    }

    CheckDlgButton(hWnd, IDC_CHECK_GREY, cd.minimap7);
    CheckDlgButton(hWnd, IDC_CHECK_LARGETEXT, cd.largeText);
    CheckDlgButton(hWnd, IDC_CHECK_MASTER_VOLUME, cd.delinkVolume);
    CheckDlgButton(hWnd, IDC_CHECK_KEYDOWN, cd.keydown);

    EnableWindow(GetDlgItem(hWnd, IDC_CHECK_DSH), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_EDIT_DSH_NBUFS), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_EDIT_DSH_DELAY), FALSE);

    if (!check_dwrite_available())
    {
        cd.textRendering = 0;
        EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXT_RENDERING), FALSE);
    }

    CheckDlgButton(hWnd, IDC_CHECK_TEXT_RENDERING, cd.textRendering);
    CheckDlgButton(hWnd, IDC_CHECK_CHATBOX, cd.chatBox);
    CheckDlgButton(hWnd, IDC_CHECK_FOG, cd.fog);

    //languages = query_languages();
    //for (int i = 0; i < languages.size(); i++)
    //    SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)languages[i].first.c_str());
}

void processDefaults(HWND hWnd)
{
    regSet(&cd_default);
    cd = cd_default;
    readSettingsToDialog(hWnd);
}

BOOL CALLBACK ConfigDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWndDlg, 0);
        break;
    case WM_INITDIALOG:
    {
        readSettingsToDialog(hWndDlg);
        //detectCC ();
        char curMode[0x40];
        int nMode = 0;
        DEVMODE devMode;
        DWORD prevW = 0;
        DWORD prevH = 0;
        //int prevBits = 0;
        while (EnumDisplaySettings(0, nMode, &devMode) != 0)
        {
            if (devMode.dmBitsPerPel == 32)
                if ((prevW != devMode.dmPelsWidth) || (prevH != devMode.dmPelsHeight))
                {
                    snprintf(curMode, _countof(curMode), "%lux%lu", devMode.dmPelsWidth, devMode.dmPelsHeight);
                    if ((devMode.dmPelsWidth >= 1024) && (devMode.dmPelsHeight >= 768))
                        SendMessage(GetDlgItem(hWndDlg, IDC_COMBO_SCREEN_SIZE), CB_ADDSTRING, 0, (LPARAM)curMode);
                    prevW = devMode.dmPelsWidth;
                    prevH = devMode.dmPelsHeight;
                    //prevBits = devMode.dmBitsPerPel;
                }
            nMode++;
        }
    }
    break;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
            switch (LOWORD(wParam))
            {
            case IDOK: //Patch
                if (IsDlgButtonChecked(hWndDlg, IDC_CHECK_WIDE))
                {
                    int x = 0;
                    int y = 0;
                    bool auto_screen_size = IsDlgButtonChecked(hWndDlg, IDC_CHECK_SCREEN_SIZE_AUTO);
                    if (!auto_screen_size)
                    {
                        char resolution[50];
                        GetDlgItemText(hWndDlg, IDC_COMBO_SCREEN_SIZE, resolution, 50);
                        int scan_result = sscanf_s(resolution, "%dx%d", &x, &y);
                        if (((x <= 0) || (y <= 0)) || (scan_result < 2))
                        {
                            MessageBox(hWndDlg, "Please select a valid resolution", "Error", MB_ICONERROR);
                            break;
                        }
                        if (y < 768)
                        {
                            MessageBox(hWndDlg, "Resolutions lower than 1024x768 are not supported", "Error", MB_ICONERROR);
                            break;
                        }
                        if (x < 1024)
                        {
                            MessageBox(hWndDlg, "Resolutions lower than 1024x768 are not supported", "Error", MB_ICONERROR);
                            break;
                        }
                    }
                    SetDlgItemText(hWndDlg, IDOK, "Saving...");
                    EnableMenuItem(GetSystemMenu(hWndDlg, FALSE), SC_CLOSE,
                        MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
                    EnableWindow(GetDlgItem(hWndDlg, IDOK), FALSE);
                    resolutionTool(
                        auto_screen_size ? GetSystemMetrics(SM_CXSCREEN) : x,
                        auto_screen_size ? GetSystemMetrics(SM_CYSCREEN) : y,
                        true,
                        false);
                }

                processIDOK(hWndDlg);
                PostMessage(GetParent(hWndDlg), WM_APP + 1, 0, 0);
                EndDialog(hWndDlg, 0);

                break;
            case IDC_BUTTON_CANCEL:
                EndDialog(hWndDlg, 0);

                break;
            case IDC_BUTTON_DEFAULT: //defaults
                processDefaults(hWndDlg);

                break;

                /*case IDC_BUTTON1:
                    {
                        GetDlgItemText (hWndDlg, IDC_COMBO1, buf, 255);
                        SetCurrentDirectory ("Random");
                        if (fileExists (buf))
                            SendMessage (GetDlgItem (hWndDlg, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)buf);
                        else
                            MessageBox (hWndDlg, "File not found", "Error", MB_ICONERROR);
                        SetCurrentDirectory ("..");

                        break;
                    }*/
            case IDC_CHECK_EDITORAUTO:
                EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT_EDITORAUTO),
                    IsDlgButtonChecked(hWndDlg, IDC_CHECK_EDITORAUTO));
                break;
            case IDC_CHECK_WIDE:
                EnableWindow(GetDlgItem(hWndDlg, IDC_COMBO_SCREEN_SIZE),
                    IsDlgButtonChecked(hWndDlg, IDC_CHECK_WIDE) && !IsDlgButtonChecked(hWndDlg, IDC_CHECK_SCREEN_SIZE_AUTO));
                EnableWindow(GetDlgItem(hWndDlg, IDC_CHECK_SCREEN_SIZE_AUTO),
                    IsDlgButtonChecked(hWndDlg, IDC_CHECK_WIDE));
                break;
            case IDC_CHECK_SCREEN_SIZE_AUTO:
                EnableWindow(GetDlgItem(hWndDlg, IDC_COMBO_SCREEN_SIZE),
                    IsDlgButtonChecked(hWndDlg, IDC_CHECK_WIDE) && !IsDlgButtonChecked(hWndDlg, IDC_CHECK_SCREEN_SIZE_AUTO));
                break;
            case IDC_CHECK_MAPSIZE:
                if (IsDlgButtonChecked(hWndDlg, IDC_CHECK_MAPSIZE))
                    CheckDlgButton(hWndDlg, IDC_CHECK_MAPSIZE,
                        MessageBox(hWndDlg,
                            "Large map support is experimental and may not work correctly. Enabling this option may cause problems even on standard sized maps. "
                            "It is recommended that you do not enable this for better experience.\n"
                            "\nAre you sure you want to enable large maps support?",
                            "Warning", MB_YESNO | MB_ICONEXCLAMATION) == 6 ? TRUE : FALSE);
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

#define IDD_DIALOG_CONFIG IDD_DIALOG_CONFIG_EF

void __stdcall launchConfigurator(HWND hWnd)
{
    installPalette();

    DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_CONFIG), hWnd, ConfigDlgProc);
}

extern "C" __declspec(dllexport) void __stdcall Configurator(HWND parent)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)
    regGet(&cd);
    launchConfigurator(parent);
}
