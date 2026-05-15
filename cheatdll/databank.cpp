#include "stdafx.h"
#include "databank.h"

extern int current_loaded_version;

struct databank_entry
{
    int x1;
    int pic1;
    int pic2;
    std::string title;
    std::string filename;
    int sound_id;
};

std::vector<databank_entry> databank_entries;

// Helper function to check if a string starts with a space
bool startsWithSpace(const std::string& str)
{
    return !str.empty() && str[0] == ' ';
}

// Helper function to remove leading spaces for comparison
std::string removeLeadingSpaces(const std::string& str)
{
    size_t start = str.find_first_not_of(' ');
    return (start == std::string::npos) ? "" : str.substr(start);
}

// Locale-aware comparison function using WinAPI CompareString
bool compareTitles(const databank_entry& a, const databank_entry& b)
{
    std::string title_a = removeLeadingSpaces(a.title);
    std::string title_b = removeLeadingSpaces(b.title);

    // Convert to wide strings for CompareString using UTF8ToWide
    std::wstring wtitle_a = UTF8ToWide(title_a);
    std::wstring wtitle_b = UTF8ToWide(title_b);

    // Use current user locale for comparison
    int result = CompareString(
        LOCALE_USER_DEFAULT,    // Use current user's locale
        NORM_IGNORECASE,        // Case-insensitive comparison
        wtitle_a.c_str(),       // First string
        (int)wtitle_a.length(), // Length of first string
        wtitle_b.c_str(),       // Second string
        (int)wtitle_b.length()  // Length of second string
    );

    // CompareString returns CSTR_LESS_THAN (1), CSTR_EQUAL (2), or CSTR_GREATER_THAN (3)
    return result == CSTR_LESS_THAN;
}

void sortDatabankEntriesWithinGroups()
{
    if (databank_entries.empty()) return;

    // Find separator positions (title == " ")
    std::vector<size_t> separator_positions;
    for (size_t i = 0; i < databank_entries.size(); ++i)
    {
        if (databank_entries[i].title == " ")
        {
            separator_positions.push_back(i);
        }
    }

    // Total number of groups = number of separators + 1
    size_t total_groups = separator_positions.size() + 1;

    // Process each group between separators
    size_t group_start = 0;
    size_t current_group = 0;

    for (size_t sep_idx = 0; sep_idx <= separator_positions.size(); ++sep_idx)
    {
        size_t group_end;

        if (sep_idx < separator_positions.size())
        {
            group_end = separator_positions[sep_idx];  // separator position is the end of the group
        }
        else
        {
            group_end = databank_entries.size();  // last group goes to the end
        }

        if (group_start >= group_end)
        {
            group_start = (sep_idx < separator_positions.size()) ? separator_positions[sep_idx] + 1 : group_end;
            current_group++;
            continue;
        }

        // Skip sorting for the second-to-last group (group index = total_groups - 2)
        bool skip_sorting = (total_groups >= 2 && current_group == total_groups - 2);

        if (!skip_sorting)
        {
            size_t sort_start = group_start;

            // For groups after the first group (sep_idx >= 1), check if there's a header
            // Headers have no leading space and should stay first
            if (sep_idx >= 1 && group_start < group_end &&
                !startsWithSpace(databank_entries[group_start].title))
            {
                sort_start = group_start + 1;
            }

            // Sort the entries in the group using CompareString
            if (sort_start < group_end)
            {
                std::sort(databank_entries.begin() + sort_start,
                    databank_entries.begin() + group_end,
                    compareTitles);
            }
        }

        // Move to next group (after the separator)
        if (sep_idx < separator_positions.size())
        {
            group_start = separator_positions[sep_idx] + 1;
        }
        current_group++;
    }
}

void __stdcall databank_load_new(TTextPanel* panel)
{
    FILE* f = fopen("history\\databank.txt", "rt");
    if (f)
    {
        int x1;
        int pic1;
        int pic2;
        char filename[MAX_PATH];
        int string_id;
        int sound_id;
        int id = 0;
        while (fscanf(f, "%d,%d,%d,%d,%d,%s",
            &x1, &pic1, &pic2, &string_id, &sound_id, &filename) == 6)
        {
            databank_entry e = { x1, pic1, pic2, get_string(string_id), filename, sound_id };
            databank_entries.emplace_back(e);
        }
        fclose(f);

        sortDatabankEntriesWithinGroups();

        for (int i = 0; i < databank_entries.size(); i++)
        {
            TTextPanel__append_line(panel, databank_entries[i].title.c_str(), i, NULL);
        }
    }
}

void __stdcall databank_delete_new()
{
    databank_entries.clear();
}

int __stdcall databank_get_pic(int line, int* x1, int* pic1, int* pic2)
{
    // return 3 on success
    if (line >= 0 && line < databank_entries.size())
    {
        *x1 = databank_entries[line].x1;
        *pic1 = databank_entries[line].pic1;
        *pic2 = databank_entries[line].pic2;
        return 3;
    }
    else
    {
        return -1;
    }
}

void __stdcall databank_get_txt(int line, char* txt)
{
    if (line >= 0 && line < databank_entries.size())
    {
        std::string s("history\\" + std::string(get_string(10001)) + "\\" + databank_entries[line].filename);
        strlcpy(txt, s.c_str(), 252);
    }
}

void __stdcall databank_get_sound(int line, char* txt)
{
    if (line >= 0 && line < databank_entries.size())
    {
        std::string s("stream\\ef_databank" + std::to_string(databank_entries[line].sound_id) + ".mp3");
        strlcpy(txt, s.c_str(), 30);
    }
}

__declspec(naked) void databank_create() //00509B01
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 10 //1.5.6
        jl      databank_load_legacy
        mov     ecx, [ebp + 0]
        push    ecx
        call    databank_load_new
        mov     eax, 00509B39h
        jmp     eax

databank_load_legacy:
        push    4F56h
        mov     eax, 00509B06h
        jmp     eax
    }
}

__declspec(naked) void databank_destroy() //0050A024
{
    __asm
    {
        call    databank_delete_new
        mov     ecx, esi
        mov     dword ptr [esp + 14h], 0FFFFFFFFh
        mov     eax, 0050A02Eh
        jmp     eax
    }
}

__declspec(naked) void databank_pic() //0050A147
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 10 //1.5.6
        jl      databank_pic_legacy
        lea     ecx, [esp + 14h]
        lea     edx, [esp + 10h]
        lea     eax, [esp + 1Ch]
        push    ecx
        push    edx
        push    eax
        push    ebp
        call    databank_get_pic
        mov     ecx, 0050A172h
        jmp     ecx

databank_pic_legacy:
        lea     ecx, [esp + 14h]
        lea     edx, [esp + 10h]
        mov     eax, 0050A14Fh
        jmp     eax
    }
}

__declspec(naked) void databank_txt_filename() //0050A279
{
    __asm
    {
        mov     ecx, current_loaded_version
        cmp     ecx, 9
        mov     eax, 19811
        jge     new_databank_txt_filename_offset
        add     eax, 600

new_databank_txt_filename_offset:
        cmp     ecx, 10 //1.5.6
        jl      databank_txt_legacy
        xor     edi, edi
        lea     eax, [esp + 84h]
        push    eax
        push    ebp
        call    databank_get_txt
        sub     esp, 0Ch
        mov     ecx, 0050A29Ch
        jmp     ecx

databank_txt_legacy:
        lea     ecx, [ebp + eax]
        mov     eax, 0050A27Fh
        jmp     eax
    }
}

__declspec(naked) void databank_sound_filename() //0050A375
{
    __asm
    {
        lea     ecx, [esp + 18h]
        mov     eax, current_loaded_version
        cmp     eax, 10 //1.5.6
        jl      databank_sound_legacy
        push    ecx
        push    ebp
        call    databank_get_sound
        mov     edx, base_game
        mov     edx, [edx]
        mov     ecx, 0050A38Fh
        jmp     ecx

databank_sound_legacy:
        inc     ebp
        mov     eax, 0050A37Ah
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setDatabankHooks()
{
    //databank txt filename
    //writeDword(0x0050A27B, 19811);
    setHook((void*)0x0050A279, databank_txt_filename);

    setHook((void*)0x00509B01, databank_create);
    setHook((void*)0x0050A024, databank_destroy);
    setHook((void*)0x0050A147, databank_pic);
    setHook((void*)0x0050A375, databank_sound_filename);
}
#pragma optimize( "", on )
