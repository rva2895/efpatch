#include "stdafx.h"
#include "setup.h"
#include "resource.h"
#include "commctrl.h"
#include "unzip.h"
#include <process.h>

#define WRITE_BUFFER_SIZE 0x4000

HWND hWndAssetsDlg = NULL;

bool verify_zip_path(const char* p)
{
    while ((*p) != '\0')
    {
        if (((*p) == '/') || ((*p) == '\\'))
            return false;
        p++;
    }
    return true;
}

bool archive_extract(unzFile z_file, const char* dest_dir, void* buf)
{
    unz_global_info gi;
    int err = unzGetGlobalInfo(z_file, &gi);
    if (err != UNZ_OK)
        return false;

    size_t uncompressed_size_total = 0;
    int size_written = 0;
    int write_counter = 0;

    //get file size
    for (int i = 0; i < gi.number_entry; i++)
    {
        unz_file_info fi;
        char filename_inzip[MAX_PATH];
        err = unzGetCurrentFileInfo(
            z_file,
            &fi,
            filename_inzip,
            _countof(filename_inzip),
            NULL,
            0,
            NULL,
            0);
        if (err != UNZ_OK)
            return false;

        uncompressed_size_total += fi.uncompressed_size;

        if (i != gi.number_entry - 1)
        {
            err = unzGoToNextFile(z_file);
            if (err != UNZ_OK)
                return false;
        }
    }

    //extract files

    err = unzGoToFirstFile(z_file);
    if (err != UNZ_OK)
        return false;

    for (int i = 0; i < gi.number_entry; i++)
    {
        unz_file_info fi;
        char filename_inzip[MAX_PATH];
        err = unzGetCurrentFileInfo(
            z_file,
            &fi,
            filename_inzip,
            _countof(filename_inzip),
            NULL,
            0,
            NULL,
            0);
        if (err != UNZ_OK)
            return false;

        if (!verify_zip_path(filename_inzip))
            return false;

        err = unzOpenCurrentFile(z_file);
        if (err != UNZ_OK)
            return false;

        std::string filename(dest_dir);
        filename += filename_inzip;

        FILE* f = _wfopen(UTF8ToWide_c_str(filename), L"wb");
        if (!f)
            return false;

        std::wstring* wParam = new std::wstring(UTF8ToWide(filename_inzip));
        PostMessage(hWndAssetsDlg, WM_APP_STATUS_CURRENT_FILE, (WPARAM)wParam, 0);

        int size;

        do
        {
            size = unzReadCurrentFile(z_file, buf, WRITE_BUFFER_SIZE);
            if (size < 0)
            {
                fclose(f);
                return false;
            }
            else if (size > 0)
            {
                fwrite(buf, size, 1, f);
                size_written += size;
                write_counter++;
                if (write_counter % 256 == 0)
                    PostMessage(hWndAssetsDlg, WM_APP_STATUS_PROGRESS, (int)((float)size_written / uncompressed_size_total * 100.0f), 0);
            }

        } while (size > 0);

        fclose(f);

        err = unzCloseCurrentFile(z_file);
        if (err != UNZ_OK)
            return false;

        if (i != gi.number_entry - 1)
        {
            err = unzGoToNextFile(z_file);
            if (err != UNZ_OK)
                return false;
        }
    }

    return true;
}

bool install_assets(const std::wstring& archive)
{
    int count = WideCharToMultiByte(CP_ACP, 0, archive.c_str(), -1, NULL, 0, NULL, NULL);
    std::string archive_acp(count, 0);
    WideCharToMultiByte(CP_ACP, 0, archive.c_str(), -1, &archive_acp[0], count, NULL, NULL);
    archive_acp.resize(strlen(archive_acp.c_str()));

    unzFile z_file = unzOpen(archive_acp.c_str());

    if (!z_file)
        return false;

    void* buf = malloc(WRITE_BUFFER_SIZE);

    bool result = archive_extract(z_file, WideToUTF8_c_str(DATA_FOLDER_PREFIX_FROM_ROOT), buf);
    free(buf);
    unzClose(z_file);

    return result;
}

bool append_file(const std::wstring& first, const std::wstring& second)
{
    FILE* f = _wfopen(first.c_str(), L"rb+");
    if (!f)
        return false;

    FILE* g = _wfopen(second.c_str(), L"rb");
    if (!g)
    {
        fclose(f);
        return false;
    }

    fseek(f, 0, SEEK_END);
    void* buf = malloc(WRITE_BUFFER_SIZE);
    size_t size;
    while ((size = fread(buf, 1, WRITE_BUFFER_SIZE, g)) > 0)
    {
        fwrite(buf, size, 1, f);
    }
    fclose(f);
    fclose(g);

    return true;
}

std::wstring merge_archive(const std::wstring& first, const std::wstring& second)
{
    wchar_t temp_name[MAX_PATH];
    wchar_t temp_path[MAX_PATH];
    GetTempPath(_countof(temp_path), temp_path);
    GetTempFileName(temp_path, L"efp", 0, temp_name);

    CopyFile(first.c_str(), temp_name, FALSE);
    if (append_file(temp_name, second))
        return temp_name;
    else
        return L"";
}

bool test_assets_mod(const std::wstring& mod_name, const std::wstring& archive_name)
{
    std::wstring s(VOOBLY_LOCAL_MOD_PATH + mod_name + L"\\" + archive_name);
    return file_exists(s.c_str());
}

void display_assets_error(const std::wstring& mod_name)
{
    std::wstring err_msg(L"Error: cannot open \"" + mod_name + L"\" mod\n\n"
        L"To play Expanding Fronts, download and enable \"" VOOBLY_ASSETS_MOD_NAME L"\" and \"" VOOBLY_EXT_ASSETS_MOD_NAME L"\" local mods.");
    MessageBox(hWndAssetsDlg, err_msg.c_str(), L"Error", MB_ICONERROR);
    exit(0);
}

void display_assets_install_error()
{
    MessageBox(hWndAssetsDlg, L"Assets install error", L"Error", MB_ICONERROR);
    exit(0);
}

unsigned int __stdcall patch_assets(void*)
{
    std::wstring assets_archive = merge_archive(
        VOOBLY_LOCAL_MOD_PATH VOOBLY_ASSETS_MOD_NAME L"\\" VOOBLY_ASSETS_ARCHIVE_NAME,
        VOOBLY_LOCAL_MOD_PATH VOOBLY_EXT_ASSETS_MOD_NAME L"\\" VOOBLY_EXT_ASSETS_ARCHIVE_NAME
    );

    if (assets_archive.length() == 0)
        display_assets_install_error();

    bool assets_install_result = install_assets(assets_archive);
    DeleteFile(assets_archive.c_str());

    if (!assets_install_result)
        display_assets_install_error();

    PostMessage(hWndAssetsDlg, WM_CLOSE, NULL, NULL);
    return 0;
}

BOOL CALLBACK AssetsDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    wchar_t s[0x100];
    unsigned int assets_tid;
    HANDLE hThread;
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWndDlg, 0);
        return TRUE;
    case WM_INITDIALOG:
        hWndAssetsDlg = hWndDlg;
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, L"Setting up assets...");
        SendMessage(GetDlgItem(hWndDlg, IDC_PROGRESS_PALETTE), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        hThread = (HANDLE)_beginthreadex(NULL, 0, patch_assets, NULL, 0, &assets_tid);
        if (hThread)
            CloseHandle(hThread);
        return TRUE;
    case WM_APP_STATUS_PROGRESS: //status set progress
        SendMessage(GetDlgItem(hWndDlg, IDC_PROGRESS_PALETTE), PBM_SETPOS, wParam, 0);
        return TRUE;
    case WM_APP_STATUS_CURRENT_FILE: //status set current file
        _snwprintf(s, _countof(s), L"Extracting %s...", ((std::wstring*)wParam)->c_str());
        delete (std::wstring*)wParam;
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, s);
        return TRUE;
    default:
        break;
    }
    return FALSE;
}

void do_assets_setup()
{
    if (!file_exists(DATA_FOLDER_PREFIX_FROM_ROOT L"graphics_x2.drs") ||
        !file_exists(DATA_FOLDER_PREFIX_FROM_ROOT L"sounds_x2.drs"))
    {
        if (!test_assets_mod(VOOBLY_ASSETS_MOD_NAME, VOOBLY_ASSETS_ARCHIVE_NAME))
            display_assets_error(VOOBLY_ASSETS_MOD_NAME);
        if (!test_assets_mod(VOOBLY_EXT_ASSETS_MOD_NAME, VOOBLY_EXT_ASSETS_ARCHIVE_NAME))
            display_assets_error(VOOBLY_EXT_ASSETS_MOD_NAME);

        DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_PALETTE), NULL, AssetsDlgProc);
    }
}
