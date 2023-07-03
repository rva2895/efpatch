#include "stdafx.h"
#include "rms.h"
#include "maplist.h"
#include <vector>
#include <map>

char** filenames;
int nFiles;

std::vector<std::string> rms_files;
extern std::map<int, MAP_INFO> map_list;

const RMS_ASSIGN rms_assign[] =
{
    {38, 13544},
    {39, 13545},
    {40, 13546},
    {41, 13547},
    {42, 13548},
    {43, 13549},
    {44, 13550},
    {45, 13551},
    {46, 13552},
    {47, 13553},
    {48, 13554},
    {49, 13555},
    {50, 13556},
    //
    {55, 13563},
    {56, 13564},
    {57, 13565},
    {58, 13566},
    {59, 13567},
    {60, 13568}
};

void list_rms_files()
{
    log("Listing RMS files...");

    if (!rms_files.empty())
        rms_files.clear();

    WIN32_FIND_DATA fd;
    HANDLE hFile = FindFirstFile("random\\*.rms", &fd);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        log("FindFirstFile returned INVALID_HANDLE_VALUE");
    }
    else
    {
        do
        {
            std::string filename(fd.cFileName);
            filename.erase(filename.length() - 4);
            rms_files.push_back(filename);
        } while (FindNextFile(hFile, &fd));
        DWORD err = GetLastError();
        if (err != ERROR_NO_MORE_FILES)
            log("WARNING: FindNextFile(): unrecognised error %u", err);
        else
            log("Finished listing files");
        FindClose(hFile);
    }
    log("Found %zu RMS files", rms_files.size());
}

void __stdcall setup_editor_rms_list(TDropDownPanel* dropdown)
{
    for (int i = 0; i < _countof(rms_assign); i++)
        TDropDownPanel__append_line2(dropdown, rms_assign[i].string_id, rms_assign[i].id);

    for (auto it = map_list.begin(); it != map_list.end(); ++it)
        TDropDownPanel__append_line2(dropdown, it->second.string_id, it->first);
    
    list_rms_files();
    int rms_index = 1000;
    for (auto it = rms_files.begin(); it != rms_files.end(); ++it)
        TDropDownPanel__append_line(dropdown, it->c_str(), rms_index++);
}

__declspec(naked) void on_editor_rms_list() //0052A2CC
{
    __asm
    {
        mov     ecx, [esi]
        push    ecx
        call    setup_editor_rms_list
        mov     eax, 0052AAD5h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setRmsEditorHooks()
{
    setHook((void*)0x0052A2CC, on_editor_rms_list);
}
#pragma optimize( "", on )
