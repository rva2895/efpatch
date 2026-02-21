#include "stdafx.h"
#include "localisation.h"

#define MAX_LOADSTRING 0x1000

std::vector<std::pair<std::string, std::string>> query_languages()
{
    struct load_language_callback_param
    {
        std::vector<std::pair<std::string, std::string>> languages;
    } param;

    auto load_language_callback = [](const char* filename, void* param)
        {
            load_language_callback_param* p = (load_language_callback_param*)param;
            HMODULE dll;
            char s1[MAX_LOADSTRING];
            char s2[MAX_LOADSTRING];
            if (dll = efpatch_LoadStringTable(filename))
            {
                if (LoadString(dll, 10000, s1, MAX_LOADSTRING) && LoadString(dll, 10001, s2, MAX_LOADSTRING))
                {
                    p->languages.emplace_back(std::pair<std::string, std::string>(s1, s2));
                    log("Found language: %s (%s)", s1, s2);
                }
                FreeLibrary(dll);
            }
        };

    findfirst_callback("language_x2-*.dll", load_language_callback, &param);

    return param.languages;
}

const char* lang_base = "language_x2-en.dll";
char* lang_local = NULL;
const char* lang_mask = "language_x2-%s.dll";

void install_language(const std::string& lang)
{
    free(lang_local);
    lang_local = (char*)malloc(lang.length() + strlen(lang_mask) - 2 + 1);
    sprintf(lang_local, lang_mask, lang.c_str());
    writeDword(0x0042467D, (DWORD)lang_base);
    writeDword(0x005E40A3, (DWORD)lang_local);
    log("Language: %s, filename: %s", lang.c_str(), lang_local);
}

void __cdecl setLanguageDllHooks_atexit()
{
    free(lang_local);
}

void setLanguageDllHooks()
{
    writeByte(0x00424DE2, 0x90);
    setHookCall((void*)0x00424DE3, efpatch_LoadStringTable);
    writeByte(0x005E4B3F, 0x90);
    setHookCall((void*)0x005E4B40, efpatch_LoadStringTable);

    efpatch_atexit(setLanguageDllHooks_atexit);
}
