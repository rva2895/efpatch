#include "stdafx.h"
#include "setup.h"
#include "resource.h"
#include "commctrl.h"
#include "unzip.h"
#include <process.h>

#define WRITE_BUFFER_SIZE 0x4000

HWND hWndAssetsDlg;

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

        FILE* f = fopen(filename.c_str(), "wb");
        if (!f)
            return false;

        char* wParam = make_str_copy(filename_inzip);
        PostMessage(hWndAssetsDlg, WM_USER + 3, (WPARAM)wParam, 0);

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
                    PostMessage(hWndAssetsDlg, WM_USER + 2, (int)((float)size_written / uncompressed_size_total * 100.0f), 0);
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

bool install_assets(const std::string& archive)
{
    unzFile z_file = unzOpen(archive.c_str());

    if (!z_file)
        return false;

    void* buf = malloc(WRITE_BUFFER_SIZE);

    bool result = archive_extract(z_file, DATA_FOLDER_PREFIX_FROM_ROOT, buf);
    free(buf);
    unzClose(z_file);

    return result;
}

bool append_file(const std::string& first, const std::string& second)
{
    FILE* f = fopen(first.c_str(), "rb+");
    if (!f)
        return false;

    FILE* g = fopen(second.c_str(), "rb");
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

std::string merge_archive(const std::string& first, const std::string& second)
{
    char temp_name[MAX_PATH];
    char temp_path[MAX_PATH];
    GetTempPath(_countof(temp_path), temp_path);
    GetTempFileName(temp_path, "efp", 0, temp_name);

    CopyFile(first.c_str(), temp_name, FALSE);
    if (append_file(temp_name, second))
        return temp_name;
    else
        return "";
}

bool test_assets_mod(const std::string& mod_name, const std::string& archive_name)
{
    std::string s(VOOBLY_LOCAL_MOD_PATH + mod_name + "\\" + archive_name);
    return file_exists(s.c_str());
}

void display_assets_error(const std::string& mod_name)
{
    std::string err_msg("Error: cannot open \"" + mod_name + "\" mod\n\n"
        "To play Expanding Fronts, download and enable \"" VOOBLY_ASSETS_MOD_NAME "\" and \"" VOOBLY_EXT_ASSETS_MOD_NAME "\" local mods.");
    MessageBox(hWndAssetsDlg, err_msg.c_str(), "Error", MB_ICONERROR);
    exit(0);
}

void display_assets_install_error()
{
    MessageBox(hWndAssetsDlg, "Assets install error", "Error", MB_ICONERROR);
    exit(0);
}

unsigned int __stdcall patch_assets(void*)
{
    std::string assets_archive = merge_archive(
        VOOBLY_LOCAL_MOD_PATH VOOBLY_ASSETS_MOD_NAME "\\" VOOBLY_ASSETS_ARCHIVE_NAME,
        VOOBLY_LOCAL_MOD_PATH VOOBLY_EXT_ASSETS_MOD_NAME "\\" VOOBLY_EXT_ASSETS_ARCHIVE_NAME
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
    char s[0x100];
    unsigned int assets_tid;
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWndDlg, 0);
        break;
    case WM_INITDIALOG:
        hWndAssetsDlg = hWndDlg;
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, "Setting up assets...");
        SendMessage(GetDlgItem(hWndDlg, IDC_PROGRESS_PALETTE), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        _beginthreadex(NULL, 0, patch_assets, NULL, 0, &assets_tid);
        break;
    case WM_USER + 2: //status set progress
        SendMessage(GetDlgItem(hWndDlg, IDC_PROGRESS_PALETTE), PBM_SETPOS, wParam, 0);
        break;
    case WM_USER + 3: //status set current file
        snprintf(s, _countof(s), "Extracting %s...", (char*)wParam);
        free((char*)wParam);
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, s);
        break;
    default:
        return false;
    }
    return true;
}

void do_assets_setup()
{
    if (!file_exists(DATA_FOLDER_PREFIX_FROM_ROOT "graphics_x2.drs") ||
        !file_exists(DATA_FOLDER_PREFIX_FROM_ROOT "sounds_x2.drs"))
    {
        if (!test_assets_mod(VOOBLY_ASSETS_MOD_NAME, VOOBLY_ASSETS_ARCHIVE_NAME))
            display_assets_error(VOOBLY_ASSETS_MOD_NAME);
        if (!test_assets_mod(VOOBLY_EXT_ASSETS_MOD_NAME, VOOBLY_EXT_ASSETS_ARCHIVE_NAME))
            display_assets_error(VOOBLY_EXT_ASSETS_MOD_NAME);

        DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_PALETTE), NULL, AssetsDlgProc);
    }
}
