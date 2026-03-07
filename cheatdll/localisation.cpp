#include "stdafx.h"
#include "localisation.h"

std::vector<std::pair<std::wstring, std::wstring>> query_languages()
{
    struct load_language_callback_param
    {
        std::vector<std::pair<std::wstring, std::wstring>> languages;
    } param;

    auto load_language_callback = [](const wchar_t* filename, void* param)
        {
            load_language_callback_param* p = (load_language_callback_param*)param;
            HMODULE dll;
            wchar_t s1[MAX_LOADSTRING];
            wchar_t s2[MAX_LOADSTRING];
            if (dll = efpatch_LoadStringTable(filename))
            {
                if (LoadString(dll, 10000, s1, MAX_LOADSTRING) && LoadString(dll, 10001, s2, MAX_LOADSTRING))
                {
                    p->languages.emplace_back(std::pair<std::wstring, std::wstring>(s1, s2));
                    log("Found language dll: %s: %s (%s)", WideToUTF8_c_str(filename), WideToUTF8_c_str(s1), WideToUTF8_c_str(s2));
                }
                FreeLibrary(dll);
            }
        };

    findfirst_callback(L"language_x2-*.dll", load_language_callback, &param);

    return param.languages;
}

const char* lang_local = NULL;

void install_language(const std::wstring& lang)
{
    static std::string lang_local_s(WideToUTF8(DATA_FOLDER_PREFIX_FROM_ROOT L"..\\language_x2-" + lang + L".dll"));
    static std::string lang_base_s(WideToUTF8(DATA_FOLDER_PREFIX_FROM_ROOT L"..\\language_x2-en.dll"));

    writeDword(0x0042467D, (DWORD)lang_base_s.c_str());
    writeDword(0x005E40A3, (DWORD)lang_local_s.c_str());
    lang_local = lang_local_s.c_str();
    log("Language: %s, filename: %s", WideToUTF8_c_str(lang), lang_local);
}

#pragma optimize( "s", on )
void setLanguageDllHooks()
{
    writeByte(0x00424DE2, 0x90);
    setHookCall((void*)0x00424DE3, (HMODULE(__stdcall*)(LPCSTR))efpatch_LoadStringTable);
    writeByte(0x005E4B3F, 0x90);
    setHookCall((void*)0x005E4B40, (HMODULE(__stdcall*)(LPCSTR))efpatch_LoadStringTable);
}
#pragma optimize( "", on )
