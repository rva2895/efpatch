#include "stdafx.h"
#include "localisation.h"

#define MAX_LOADSTRING 0x1000

std::vector<std::pair<std::string, std::string>> query_languages()
{
    std::vector<std::pair<std::string, std::string>> languages;
    int nFiles = 0;
    WIN32_FIND_DATA fd;
    HANDLE hFile = FindFirstFile("language_x2-*.dll", &fd);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        log("FindFirstFile returned INVALID_HANDLE_VALUE");
    }
    else
    {
        char* s1 = (char*)malloc(MAX_LOADSTRING);
        char* s2 = (char*)malloc(MAX_LOADSTRING);
        do
        {
            HMODULE dll;
            if (dll = LoadLibrary(fd.cFileName))
                if (LoadString(dll, 10000, s1, MAX_LOADSTRING))
                    if (LoadString(dll, 10001, s2, MAX_LOADSTRING))
                    {
                        languages.push_back(std::pair<std::string, std::string>(s1, s2));
                        log("Found language: %s (%s)", s1, s2);
                        nFiles++;
                    }
        } while (FindNextFile(hFile, &fd));
        free(s1);
        free(s2);
        int err = GetLastError();
        if (err != ERROR_NO_MORE_FILES)
            log("WARNING: FindNextFile(): unrecognised error %d", err);
        else
            log("Finished listing files");
        FindClose(hFile);
    }
    log("Found %d language dll files", nFiles);
    return languages;
}

const char* lang_base = "language_x2.dll";
char* lang_local;
const char* lang_mask = "language_x2-%s.dll";

void install_language(std::string lang)
{
    lang_local = (char*)malloc(lang.length() + strlen(lang_mask) - 2 + 1);
    sprintf(lang_local, lang_mask, lang.c_str());
    writeDword(0x0042467D, (DWORD)lang_base);
    writeDword(0x005E40A3, (DWORD)lang_local);
    log("Language: %s, filename: %s", lang.c_str(), lang_local);
}
