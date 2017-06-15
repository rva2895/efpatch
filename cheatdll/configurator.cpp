#include "stdafx.h"

#include "configurator.h"
#include "registry.h"
#include "resource.h"
#include "resolution.h"

#include <CommCtrl.h>

void processIDOK (HWND hWnd)
{
	char buf [10];
	CONFIG_DATA cd;
	GetDlgItemText (hWnd, IDC_EDIT1, buf, 10);
	sscanf (buf, "%d", &cd.nBufs);
	GetDlgItemText (hWnd, IDC_EDIT2, buf, 10);
	sscanf (buf, "%d", &cd.timeout);
	cd.useFPS = IsDlgButtonChecked (hWnd, IDC_CHECK1);
	cd.useDShook = IsDlgButtonChecked (hWnd, IDC_CHECK2);
	cd.askAtStartup = !IsDlgButtonChecked (hWnd, IDC_CHECK3);
	cd.unlockResources = IsDlgButtonChecked (hWnd, IDC_CHECK4);
	cd.genieAsk = IsDlgButtonChecked (hWnd, IDC_CHECK5);
	cd.windowMode = IsDlgButtonChecked (hWnd, IDC_CHECK7);

	if (IsDlgButtonChecked (hWnd, IDC_RADIO1))
		cd.gameVersion = CC;
	if (IsDlgButtonChecked (hWnd, IDC_RADIO2))
		cd.gameVersion = EF;

	if (IsDlgButtonChecked (hWnd, IDC_RADIO3))
		cd.useAltCivLetter = 0;
	if (IsDlgButtonChecked (hWnd, IDC_RADIO4))
		cd.useAltCivLetter = 1;

	cd.widescrnEnabled = IsDlgButtonChecked (hWnd, IDC_CHECK6);
	if (IsDlgButtonChecked (hWnd, IDC_CHECK6))
	{
		char str [50];
		cd.widescrnEnabled = 1;
		GetDlgItemText (hWnd, IDC_COMBO1, str, 50);
		sscanf (str, "%dx%d", &cd.xres, &cd.yres);
	}

	regSet (&cd);
}

void getSettings (HWND hWnd)
{
	char buf[50];
	CONFIG_DATA cd;
	regGet (&cd);

	CheckDlgButton (hWnd, IDC_CHECK1, cd.useFPS);
	CheckDlgButton (hWnd, IDC_CHECK2, cd.useDShook);
	CheckDlgButton (hWnd, IDC_CHECK3, !cd.askAtStartup);
	CheckDlgButton (hWnd, IDC_CHECK4, cd.unlockResources);
	CheckDlgButton (hWnd, IDC_CHECK5, cd.genieAsk);
	CheckDlgButton (hWnd, IDC_CHECK6, cd.widescrnEnabled);
	CheckDlgButton (hWnd, IDC_CHECK7, cd.windowMode);

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
	sprintf (buf, "%d", cd.nBufs);
	SetDlgItemText (hWnd, IDC_EDIT1, buf);
	sprintf (buf, "%d", cd.timeout);
	SetDlgItemText (hWnd, IDC_EDIT2, buf);
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
			int prevW = 0;
			int prevH = 0;
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
			if (IsDlgButtonChecked (hWndDlg, IDC_CHECK6))
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
		//case IDC_BUTTON_ASSOC:


		//	break;
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

void launchConfigurator (HWND parent)
{
	DialogBox (GetModuleHandle("efpatch.dll"), MAKEINTRESOURCE(IDD_DIALOG_CONFIG), parent, ConfigDlgProc);
}
