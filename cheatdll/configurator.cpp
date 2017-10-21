#include "stdafx.h"

#include "configurator.h"
#include "registry.h"
#include "resource.h"
#include "resolution.h"

#include <CommCtrl.h>

void processIDOK (HWND hWnd)
{
	CONFIG_DATA cd;
	regGet(&cd);
#ifndef _CHEATDLL_CC
	char buf[10];
	GetDlgItemText(hWnd, IDC_EDIT1, buf, 10);
	sscanf(buf, "%d", &cd.nBufs);
	GetDlgItemText(hWnd, IDC_EDIT2, buf, 10);
	sscanf(buf, "%d", &cd.timeout);
#endif
	cd.useFPS = IsDlgButtonChecked(hWnd, IDC_CHECK_FPS);
	cd.useDShook = IsDlgButtonChecked(hWnd, IDC_CHECK_DSH);
	cd.askAtStartup = !IsDlgButtonChecked(hWnd, IDC_CHECK_ALWAYSRUN);
	cd.unlockResources = IsDlgButtonChecked(hWnd, IDC_CHECK_UNLOCKRES);

	cd.animatedWater = IsDlgButtonChecked(hWnd, IDC_CHECK_ANIMATED_WATER);

	cd.largeMaps = IsDlgButtonChecked(hWnd, IDC_CHECK_MAP);

	if (cd.editorAutosave = IsDlgButtonChecked(hWnd, IDC_CHECK_EDITORAUTO))
	{
		char str[50];
		GetDlgItemText(hWnd, IDC_EDIT_EDITORAUTO, str, 50);
		sscanf(str, "%d", &cd.editorAutosaveInterval);
	}

	cd.windowMode = IsDlgButtonChecked(hWnd, IDC_CHECK_WNDMODE);

	if (IsDlgButtonChecked (hWnd, IDC_RADIO1))
		cd.gameVersion = CC;
	if (IsDlgButtonChecked (hWnd, IDC_RADIO2))
		cd.gameVersion = EF;

	if (IsDlgButtonChecked (hWnd, IDC_RADIO3))
		cd.useAltCivLetter = 0;
	if (IsDlgButtonChecked (hWnd, IDC_RADIO4))
		cd.useAltCivLetter = 1;

	if (cd.widescrnEnabled = IsDlgButtonChecked (hWnd, IDC_CHECK_WIDE))
	{
		char str [50];
		GetDlgItemText(hWnd, IDC_COMBO1, str, 50);
		sscanf(str, "%dx%d", &cd.xres, &cd.yres);
	}

	regSet (&cd);
}

void getSettings (HWND hWnd)
{
	char buf[50];
	CONFIG_DATA cd;
	regGet (&cd);

#ifdef _CHEATDLL_CC
	cd.gameVersion = CC;
	cd.largeMaps = 0;
	cd.useDShook = 0;
	cd.nBufs = 0;
	cd.timeout = 0;
#endif

	CheckDlgButton (hWnd, IDC_CHECK_FPS, cd.useFPS);
	CheckDlgButton (hWnd, IDC_CHECK_DSH, cd.useDShook);
	CheckDlgButton (hWnd, IDC_CHECK_ALWAYSRUN, !cd.askAtStartup);
	CheckDlgButton(hWnd, IDC_CHECK_UNLOCKRES, cd.unlockResources);
	CheckDlgButton(hWnd, IDC_CHECK_EDITORAUTO, cd.editorAutosave);

	CheckDlgButton(hWnd, IDC_CHECK_ANIMATED_WATER, cd.animatedWater);

	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_EDITORAUTO), cd.editorAutosave);

	CheckDlgButton (hWnd, IDC_CHECK_WIDE, cd.widescrnEnabled);
	CheckDlgButton (hWnd, IDC_CHECK_WNDMODE, cd.windowMode);

	CheckDlgButton(hWnd, IDC_CHECK_MAP, cd.largeMaps);

	if (cd.gameVersion == CC)
	{
		CheckDlgButton (hWnd, IDC_RADIO1, BST_CHECKED);
		CheckDlgButton (hWnd, IDC_RADIO2, BST_UNCHECKED);
	}
	if (cd.gameVersion == EF)
	{
		CheckDlgButton (hWnd, IDC_RADIO1, BST_UNCHECKED);
		CheckDlgButton (hWnd, IDC_RADIO2, BST_CHECKED);
	}
	if (cd.useAltCivLetter)
	{
		CheckDlgButton (hWnd, IDC_RADIO3, BST_UNCHECKED);
		CheckDlgButton (hWnd, IDC_RADIO4, BST_CHECKED);
	}
	else
	{
		CheckDlgButton (hWnd, IDC_RADIO3, BST_CHECKED);
		CheckDlgButton (hWnd, IDC_RADIO4, BST_UNCHECKED);
	}

#ifdef _CHEATDLL_CC
	EnableWindow(GetDlgItem(hWnd, IDC_RADIO2), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_CHECK_DSH), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT1), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT2), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_CHECK_MAP), FALSE);
#else
	sprintf(buf, "%d", cd.nBufs);
	SetDlgItemText (hWnd, IDC_EDIT1, buf);
	sprintf(buf, "%d", cd.timeout);
	SetDlgItemText (hWnd, IDC_EDIT2, buf);
#endif
	sprintf(buf, "%d", cd.editorAutosaveInterval);
	SetDlgItemText(hWnd, IDC_EDIT_EDITORAUTO, buf);

	if (cd.widescrnEnabled)
	{
		sprintf (buf, "%dx%d", cd.xres, cd.yres);
		SetDlgItemText (hWnd, IDC_COMBO1, buf);
	}
}

extern CONFIG_DATA cd_default;

void processDefaults (HWND hWnd)
{
	regSet (&cd_default);
	getSettings (hWnd);
}

BOOL CALLBACK ConfigDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
    switch (message)
	{
	case WM_CLOSE:
		EndDialog (hWndDlg, 0);
		break;
	case WM_INITDIALOG:
		{
			//SendMessage (GetDlgItem (hWndDlg, IDC_BUTTON_ASSOC), BCM_SETSHIELD, 0, TRUE);

			getSettings (hWndDlg);
			//detectCC ();
			char curMode [50];
			int nMode = 0;
			DEVMODE devMode;
			DWORD prevW = 0;
			DWORD prevH = 0;
			int prevBits = 0;
			while (EnumDisplaySettings (0, nMode, &devMode) != 0)
			{
				if (devMode.dmBitsPerPel == 32)
					if ( (prevW != devMode.dmPelsWidth) || (prevH != devMode.dmPelsHeight) )
					{
						sprintf (curMode, "%dx%d", devMode.dmPelsWidth, devMode.dmPelsHeight);
						if ( (devMode.dmPelsWidth >= 1024) && (devMode.dmPelsHeight >= 768) )
							SendMessage (GetDlgItem (hWndDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)curMode);
						prevW = devMode.dmPelsWidth;
						prevH = devMode.dmPelsHeight;
						prevBits = devMode.dmBitsPerPel;
					}
				nMode ++;
			}
		}
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)
		switch (LOWORD(wParam))
		{
		case IDOK: //Patch
			char resolution [50];
			int x;
			int y;
			if (IsDlgButtonChecked (hWndDlg, IDC_CHECK_WIDE))
			{
				GetDlgItemText (hWndDlg, IDC_COMBO1, resolution, 50);
				x = 0; y = 0;
				int scan_result = sscanf (resolution, "%dx%d", &x, &y);
				if ( ((x <= 0) || (y <= 0)) || (scan_result < 2) )
				{
					MessageBox (hWndDlg, "Please select a valid resolution", "Error", MB_ICONERROR);
					break;
				}
				if (y < 768)
				{
					MessageBox (hWndDlg, "Resolutions lower than 768V are not supported", "Error", MB_ICONERROR);
					break;
				}
				if (x < 1024)
				{
					MessageBox (hWndDlg, "Resolutions lower than 1024H are not supported", "Error", MB_ICONERROR);
					break;
				}
				SetDlgItemText (hWndDlg, IDOK, "Saving...");
				EnableMenuItem (GetSystemMenu(hWndDlg, FALSE), SC_CLOSE,
					MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				EnableWindow (GetDlgItem (hWndDlg, IDOK), FALSE);
				patchResolution (x, y);
			}

			processIDOK (hWndDlg);
			PostMessage (GetParent (hWndDlg), WM_APP+1, 0, 0);
			EndDialog (hWndDlg, 0);

			break;
		case IDC_BUTTON2:
			EndDialog (hWndDlg, 0);

			break;
		case IDC_BUTTON3: //defaults
			processDefaults (hWndDlg);

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

void __stdcall launchConfigurator(HWND hWnd)
{
	DialogBox(GetModuleHandle("efpatch.dll"), MAKEINTRESOURCE(IDD_DIALOG_CONFIG), hWnd, ConfigDlgProc);
}

extern "C" __declspec(dllexport) void __stdcall Configurator(HWND parent)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)
	launchConfigurator(parent);
}
