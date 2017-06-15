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


//DAT file select
BOOL CALLBACK MainDlgProc2(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	case WM_CLOSE:
		{
			ExitProcess (0);
			break;
		}
	case WM_INITDIALOG:
		{
			log ("Version and DAT file select dialog opened");
			regGet (&cd);
			if (cd.gameVersion == CC)
				CheckDlgButton (hWndDlg, IDC_RADIO1_, BST_CHECKED);
			if (cd.gameVersion == EF)
				CheckDlgButton (hWndDlg, IDC_RADIO2_, BST_CHECKED);
			if (!cd.askAtStartup)
				CheckDlgButton (hWndDlg, IDC_CHECK1_, BST_CHECKED);

			log ("Looking for DATs...");
			WIN32_FIND_DATA fd;
			HANDLE hFile = FindFirstFile ("data\\genie*.dat", &fd);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				log ("FindFirstFile returned INVALID_HANDLE_VALUE");
			}
			else
			{
				do
				{
					log ("found %s", fd.cFileName);
					SendMessage (GetDlgItem (hWndDlg, IDC_LIST1_), LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
					//nFiles++;
					//filenames = (char**)realloc (filenames, sizeof(char*)*nFiles);
					//filenames [nFiles-1] = (char*) malloc (strlen(fd.cFileName)+1);
					//strcpy (filenames [nFiles-1], fd.cFileName);
					//filenames [nFiles-1] [strstr (fd.cFileName, ".rms") - fd.cFileName] = 0;
				}
				while (FindNextFile (hFile, &fd));
				int err = GetLastError ();
				if (err != ERROR_NO_MORE_FILES)
					log ("WARNING: FindNextFile(): unrecognised error %d", err);
				else
					log ("Finished listing files");
				FindClose (hFile);
			}

			break;
		}
	case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			switch (LOWORD(wParam))
			{
			case IDOK: //Run
				{
					cd.askAtStartup = !IsDlgButtonChecked (hWndDlg, IDC_CHECK1_);
					if (IsDlgButtonChecked (hWndDlg, IDC_RADIO1_))
					{
						cd.gameVersion = CC;
						regSet (&cd);
						EndDialog (hWndDlg, 0);
					}
					else if (IsDlgButtonChecked (hWndDlg, IDC_RADIO2_))
					{
					//	int id;
						
						int i = SendMessage (GetDlgItem (hWndDlg, IDC_LIST1_), LB_GETCURSEL, 0, 0);
						int len = SendMessage (GetDlgItem (hWndDlg, IDC_LIST1_), LB_GETTEXTLEN, i, 0);
						char* str = (char*) malloc (len+1);
						SendMessage (GetDlgItem (hWndDlg, IDC_LIST1_), LB_GETTEXT, i, (LPARAM)str);
						datFilename = (char*) malloc (len+1+strlen("data\\"));
						strcpy (datFilename, "data\\");
						strcat (datFilename, str);

						free (str);

						setInt (0x0048F0E5, (int)datFilename);

						cd.gameVersion = EF;
						regSet (&cd);
						EndDialog (hWndDlg, 0);
					}
					else
						MessageBox (hWndDlg, "Please select game version", "Error", MB_ICONERROR);

					break;
				}
			case IDCANCEL:
				{
					ExitProcess (0);
					break;
				}
			
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
