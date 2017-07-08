#include "stdafx.h"

#include "versionselect.h"
#include "resource.h"
#include "configurator.h"
#include "registry.h"

extern CONFIG_DATA cd;

char* datFilename;

BOOL CALLBACK MainDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	case WM_APP+1:
		regGet (&cd);
		if (cd.gameVersion == CC)
		{
			CheckDlgButton (hWndDlg, IDC_RADIO1_, BST_CHECKED);
			CheckDlgButton (hWndDlg, IDC_RADIO2_, BST_UNCHECKED);
		}
		else if (cd.gameVersion == EF)
		{
			CheckDlgButton (hWndDlg, IDC_RADIO2_, BST_CHECKED);
			CheckDlgButton (hWndDlg, IDC_RADIO1_, BST_UNCHECKED);
		}

		if (cd.askAtStartup)
			CheckDlgButton (hWndDlg, IDC_CHECK1_, BST_UNCHECKED);
		else
			CheckDlgButton (hWndDlg, IDC_CHECK1_, BST_CHECKED);
		break;
	case WM_CLOSE:
		ExitProcess (0);
		break;
	case WM_INITDIALOG:
		log ("Version select dialog opened");
		regGet (&cd);
		if (cd.gameVersion == CC)
			CheckDlgButton (hWndDlg, IDC_RADIO1_, BST_CHECKED);
		if (cd.gameVersion == EF)
			CheckDlgButton (hWndDlg, IDC_RADIO2_, BST_CHECKED);
		if (!cd.askAtStartup)
			CheckDlgButton (hWndDlg, IDC_CHECK1_, BST_CHECKED);

		break;
	case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			switch (LOWORD(wParam))
			{
			case IDOK: //Run
				regGet (&cd);
				cd.askAtStartup = !IsDlgButtonChecked (hWndDlg, IDC_CHECK1_);
				if (IsDlgButtonChecked (hWndDlg, IDC_RADIO1_))
				{
					cd.gameVersion = CC;
					regSet (&cd);
					EndDialog (hWndDlg, 0);
				}
				else if (IsDlgButtonChecked (hWndDlg, IDC_RADIO2_))
				{
					cd.gameVersion = EF;
					regSet (&cd);
					EndDialog (hWndDlg, 0);
				}
				else
					MessageBox (hWndDlg, "Please select game version", "Error", MB_ICONERROR);

				break;
			case IDCANCEL:
				ExitProcess (0);
				break;
			case IDC_BUTTON1_:
				launchConfigurator (hWndDlg);
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
