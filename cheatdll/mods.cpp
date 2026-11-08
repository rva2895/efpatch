#include "stdafx.h"
#include "mods.h"

std::map<std::pair<unsigned int, unsigned int>, std::wstring> modded_files;

unsigned __int8* __fastcall RESFILE_try_load_mod_file(unsigned int rType, unsigned int rId, int* size)
{
    auto it = modded_files.find(std::make_pair(rType, rId));
    if (it == modded_files.end())
        return NULL;

    FILE* f = _wfopen(it->second.c_str(), L"rb");
    if (!f)
        return NULL;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long len = ftell(f);
    if (len <= 0 || len > INT_MAX) { fclose(f); return NULL; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return NULL; }

    unsigned __int8* data = (unsigned __int8*)malloc_internal((size_t)len);
    if (!data) { fclose(f); return NULL; }

    size_t got = fread(data, (size_t)len, 1, f);
    fclose(f);
    if (got != 1) { free(data); return NULL; }

    *size = (int)len;
    return data;
}

unsigned int convert_extension_to_rtype(const std::wstring& ext)
{
    if (ext == L"slp") return 0x736C7020u;
    else if (ext == L"bin") return 0x62696E61u;
    else if (ext == L"wav") return 0x77617620u;
    else return 0;
}

unsigned __int8* __fastcall RESFILE_load_new(unsigned int rType, unsigned int rId,
    int* rLoadType, int* rDataSize)
{
    int file;
    int offset;
    unsigned __int8* mapped_file;
    int size;
    unsigned __int8* data;
    int read_result;

    *rLoadType = -1;
    *rDataSize = 0;

    if (data = RESFILE_try_load_mod_file(rType, rId, &size))
    {
        *rLoadType = 1;
        *rDataSize = size;
        return data;
    }

    if (!RESFILE_locate_resource(rType, rId, &file, &offset, &mapped_file, &size))
        return NULL;

    if (mapped_file)
    {
        *rLoadType = 0;
        *rDataSize = size;
        return &mapped_file[offset];
    }

    *rLoadType = 1;
    data = (unsigned __int8*)malloc_internal((size_t)size);
    *rDataSize = size;
    if (!data)
        return NULL;

    lseek_internal(file, (LONG)offset, 0);
    read_result = read_internal(file, data, (DWORD)size);
    if (read_result == size)
        return data;
    else
    {
        free_internal(data);
        return NULL;
    }
}

// ---------------------------------------------------------------------------
// Mod file enumeration
// ---------------------------------------------------------------------------

static std::wstring ToLowerAsciiW(const std::wstring& s)
{
    std::wstring out = s;
    for (wchar_t& c : out)
        if (c >= L'A' && c <= L'Z')
            c = (wchar_t)(c - L'A' + L'a');
    return out;
}

// "1000.slp" -> (1000, "slp"); "1000_foo.SLP" -> (1000, "slp"); "1000" / "1000." -> false
static bool ParseModFileName(const std::wstring& name, unsigned int& outId, std::wstring& outExt)
{
    if (name.empty() || name[0] < L'0' || name[0] > L'9')
        return false;

    size_t pos = 0;
    uint64_t id = 0;
    while (pos < name.size() && name[pos] >= L'0' && name[pos] <= L'9')
    {
        id = id * 10 + (uint64_t)(name[pos] - L'0');
        if (id > 0xFFFFFFFFull)
            return false;
        ++pos;
    }
    if (pos == 0)
        return false;

    size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos ||
        dot < pos ||
        dot + 1 >= name.size())
        return false;

    outId = (unsigned int)id;
    outExt = ToLowerAsciiW(name.substr(dot + 1));
    return true;
}

static void __cdecl ModDrsDirCallback(const wchar_t* filename, void* param)
{
    std::wstring* drsPath = (std::wstring*)param;

    unsigned int rId = 0;
    std::wstring ext;
    if (!ParseModFileName(filename, rId, ext))
        return;

    unsigned int rType = convert_extension_to_rtype(ext);
    if (rType == 0)
        return;

    std::wstring fullPath = *drsPath + L'\\' + filename;

    // Skip directories. _wfindfirst returns both; a directory named "1000.slp"
    // would otherwise be a stale map entry.
    DWORD attrs = GetFileAttributesW(fullPath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY))
        return;

    // Later mods in cd.mods override earlier ones (last write wins).
    modded_files[std::make_pair(rType, rId)] = fullPath;
}

void populate_modded_files_map()
{
    log("Loading mods...");

    modded_files.clear();

    for (const std::wstring& modName : cd.mods)
    {
        std::wstring drsPath = L"Mods\\" + modName + L"\\drs";
        std::wstring pattern = drsPath + L"\\*";
        findfirst_callback(pattern.c_str(), ModDrsDirCallback, &drsPath);

        log("Added mod: %s", WideToUTF8_c_str(modName));
    }
}

void setModsHooks()
{
    setHook((void*)0x004D5060, RESFILE_load_new);
    populate_modded_files_map();
}
