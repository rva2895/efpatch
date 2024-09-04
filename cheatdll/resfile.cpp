#include "stdafx.h"
#include "resfile.h"
#include "drsfile.h"

#include <algorithm>

RESFILE resfile;

const char resfile_txt[] = "RESFILE: ";

void* __fastcall RESFILE_load_new(unsigned int rType, unsigned int rId, int* rLoadType, int* rDataSize)
{
    void* data = resfile.get_resource(rType, rId, rDataSize);
    if (data)
        *rLoadType = 0;
    else
        *rLoadType = -1;
    return data;
}

unsigned __int8* data_ptr_retval;

int __fastcall RESFILE_locate_resource_new(unsigned int rType, unsigned int rId, int* file, int* offset, unsigned __int8** mapped_file, int* size)
{
    void* data = resfile.get_resource(rType, rId, size);
    if (data)
    {
        *offset = 0;
        data_ptr_retval = (unsigned __int8*)data;
        *mapped_file = data_ptr_retval;
        return 1;
    }
    else
        return 0;
}

void __fastcall RESFILE_open_new_resource_file_new(char* resFileName, char* password, char* path, int open_mode)
{
    UNREFERENCED_PARAMETER(password);
    UNREFERENCED_PARAMETER(open_mode);
    char buf[MAX_PATH];
    snprintf(buf, _countof(buf), "%s%s", path, resFileName);
    resfile.add_resource_file(buf);
}

void RESFILE::add_resource_file(char* filename)
{
    DRS_header hdr;
    DRS_tableInfo* tInfo;
    DRS_tableEntry** tEntries;

    int resfile_id = res_files.size();

    FILE* f = fopen(filename, "rb");
    if (!f)
        return;

    res_files.push_back(f);

    fread(&hdr.copyright, sizeof(hdr.copyright), 1, f);
    fread(&hdr.version, sizeof(hdr.version), 1, f);
    fread(&hdr.ftype, sizeof(hdr.ftype), 1, f);
    fread(&hdr.nTables, sizeof(hdr.nTables), 1, f);
    fread(&hdr.off1stFile, sizeof(hdr.off1stFile), 1, f);

    if (strncmp(hdr.version, "1.00", 4) ||
        strncmp(hdr.ftype, "swbg", 4))
    {
        return;
    }

    tInfo = new DRS_tableInfo[hdr.nTables];
    for (int i = 0; i < hdr.nTables; i++)
    {
        fread(&tInfo[i].ext, sizeof(tInfo[i].ext), 1, f);
        fread(&tInfo[i].tableOffset, sizeof(tInfo[i].tableOffset), 1, f);
        fread(&tInfo[i].nFiles, sizeof(tInfo[i].nFiles), 1, f);

        tInfo[i].capacity = tInfo[i].nFiles;
    }

    tEntries = new DRS_tableEntry * [hdr.nTables];
    for (int i = 0; i < hdr.nTables; i++)
    {
        tEntries[i] = new DRS_tableEntry[tInfo[i].nFiles];
        fseek(f, tInfo[i].tableOffset, SEEK_SET);

        bool found = false;
        RESFILE_group* g;
        for (auto it = groups.begin(); it != groups.end(); ++it)
            if ((*it).type == *(unsigned long*)tInfo[i].ext)
            {
                found = true;
                g = &*it;
            }
        if (!found)
        {
            g = &groups.emplace_back();
            g->type = *(unsigned long*)tInfo[i].ext;
        }

        for (int j = 0; j < tInfo[i].nFiles; j++)
        {
            fread(&tEntries[i][j].id, sizeof(tEntries[i][j].id), 1, f);
            fread(&tEntries[i][j].offset, sizeof(tEntries[i][j].offset), 1, f);
            fread(&tEntries[i][j].size, sizeof(tEntries[i][j].size), 1, f);

            tEntries[i][j].data = NULL;

            RESFILE_entry entry;
            entry.resfile_id = resfile_id;
            entry.offset = tEntries[i][j].offset;
            entry.size = tEntries[i][j].size;
            entry.data = NULL;
            entry.reference_count = 0;
            g->entries[tEntries[i][j].id] = entry;
        }
    }

    for (int i = 0; i < hdr.nTables; i++)
    {
        for (int j = 0; j < tInfo[i].nFiles; j++)
            free(tEntries[i][j].data);
        delete[] tEntries[i];
    }

    delete[] tEntries;
    delete[] tInfo;

    log("%sadded new resource file: %s", resfile_txt, filename);
}

void* RESFILE::get_resource(unsigned int ext, int id, int* size)
{
    void* data = NULL;
    *size = 0;
    for (auto it = groups.begin(); it != groups.end(); ++it)
        if ((*it).type == ext)
        {
            //found = true;
            auto entry_it = (*it).entries.find(id);
            if (entry_it != (*it).entries.end())
            {
                RESFILE_entry& entry = entry_it->second;
                if (!entry.data)
                {
                    FILE* f = res_files[entry.resfile_id];
                    entry.data = malloc(entry.size);
                    fseek(f, entry.offset, SEEK_SET);
                    fread(entry.data, entry.size, 1, f);
                }
                entry.reference_count++;
                data = entry.data;
                *size = entry.size;
            }
        }
    return data;
}

void RESFILE::calc()
{
    int aux_size = 0;
    int size_total = 0;
    int size_total_unref = 0;
    for (auto it = groups.begin(); it != groups.end(); ++it)
    {
        unsigned int ext = (*it).type;
        int size = 0;
        int size_unref = 0;
        for (auto it_sub = (*it).entries.begin(); it_sub != (*it).entries.end(); ++it_sub)
        {
            if (it_sub->second.data)
            {
                size += it_sub->second.size;
                if (it_sub->second.reference_count == 0)
                    size_unref += it_sub->second.size;
            }
            aux_size += sizeof(it_sub->first);
            aux_size += sizeof(it_sub->second);
        }
        size_total += size;
        size_total_unref += size_unref;
        chat("%ssize report: ext %d = %d MB, unref = %d MB", resfile_txt, ext, size / 1024 / 1024, size_unref / 1024 / 1024);
    }
    chat("%ssize report: total = %d MB, unref = %d MB", resfile_txt, size_total / 1024 / 1024, size_total_unref / 1024 / 1024);
}

void RESFILE::collect_garbage()
{
    for (auto it = groups.begin(); it != groups.end(); ++it)
    {
        for (auto it_sub = (*it).entries.begin(); it_sub != (*it).entries.end(); ++it_sub)
        {
            if (it_sub->second.data)
            {
                if (it_sub->second.reference_count == 0)
                {
                    free(it_sub->second.data);
                    it_sub->second.data = NULL;
                }
            }
        }
    }
}

void RESFILE::close_resource(unsigned int ext, int id)
{
    for (auto it = groups.begin(); it != groups.end(); ++it)
        if ((*it).type == ext)
        {
            //found = true;
            auto entry_it = (*it).entries.find(id);
            if (entry_it != (*it).entries.end())
            {
                RESFILE_entry& entry = entry_it->second;
                entry.reference_count--;
            }
        }
}

void set_call(DWORD addr)
{
    writeDword(addr + 1, (DWORD)RESFILE_load_new - addr - 5);
}

int resource_id;

void __stdcall resfile_close_resource(unsigned int ext, int id)
{
    resfile.close_resource(ext, id);
}

__declspec(naked) void on_palette_load() //00458066
{
    __asm
    {
        mov     ecx, 62696E61h
        mov     resource_id, edx
        mov     eax, 0045806Bh
        jmp     eax
    }
}

__declspec(naked) void on_palette_load_2() //0045827A
{
    __asm
    {
        
        push    edi
        call    free_internal
        add     esp, 4
        cmp     dword ptr [esp + 14h], 1
        jz      loc_45828A

        push    resource_id
        push    62696E61h
        call    resfile_close_resource

        loc_45828A:
        mov     eax, 0045828Ah
        jmp     eax
    }
}

__declspec(naked) void on_panel_setup() //004B80DE
{
    __asm
    {
        cmp     dword ptr [esp + 18h], 1
        jnz     loc_4B80EE_v2

        push    edi
        call    free_internal
        add     esp, 4

        jmp     loc_4B80EE

loc_4B80EE_v2:
        mov     eax, [ebp + 204h]
        push    eax
        push    62696E61h
        call    resfile_close_resource

loc_4B80EE:
        mov     eax, 004B80EEh
        jmp     eax
    }
}

int __fastcall RESFILE_Extract_to_File_new(unsigned int rType, unsigned int rId, char* file_name, FILE** file)
{
    CHAR PathName[260]; // [esp+10h] [ebp-104h] BYREF

    int load_type;
    int load_size;
    unsigned __int8* data;

    int result;

    *file = 0;
    data = RESFILE_load(rType, rId, &load_type, &load_size);
    if (data)
    {
        if (!GetTempPathA(0x104u, PathName))
            strcpy(PathName, ".");
        if (GetTempFileNameA(PathName, "temp", 0, file_name))
        {
            *file = fopen_internal(file_name, "wb+");
            if (*file)
            {
                if (load_size > 0)
                {
                    fwrite_internal(data, load_size, 1, *file);
                    fseek_internal(*file, 0, 0);
                }
            }
        }
        if (load_type == 1)
            free(data);
        else
            resfile_close_resource(rType, rId);
        result = *file != NULL;
    }
    return result;
}

int __fastcall RESFILE_Make_File_new(unsigned int rType, unsigned int rId, char* fileName)
{
    int result;

    int load_type;
    int load_size;
    unsigned __int8* data;

    FILE* file;

    data = RESFILE_load(rType, rId, &load_type, &load_size);
    if (data)
    {
        file = fopen(fileName, "rb");
        if (file)
        {
            fclose(file);
            file = NULL;
        }
        else
        {
            file = fopen(fileName, "wb+");
            if (file)
            {
                if (load_size > 0)
                {
                    fwrite(data, load_size, 1u, file);
                    fseek(file, 0, 0);
                }
                fclose(file);
            }
        }
        if (load_type == 1)
            free(data);
        else
            resfile_close_resource(rType, rId);
        result = file != NULL;
    }
    return result;
}

__declspec(naked) void on_shape_load() //00542A29
{
    __asm
    {
        mov     edi, edx
        call    RESFILE_load
        mov     [ebp + 0Ch], eax
        cmp     [ebp + 4], 1
        jz      loc_542A31

        mov     [ebp + 4], edi

loc_542A31:
        mov     eax, 00542A31h
        jmp     eax
    }
}

int __fastcall RESFILE_Decommit_Mapped_Memory_new(unsigned __int8* ResData, int resSize)
{
    //resSize = res_id
    resfile_close_resource(0x736C7020, resSize);
    return 0;
}

#pragma optimize( "s", on )
void setResFileHooks()
{
    //close
    writeByte(0x004D4E20, 0xC3);
    //decommit
    //writeByte(0x004D52B0, 0xC3);

    //setHook((void*)0x004D5060, RESFILE_load_new);
    setHook((void*)0x004D4C10, RESFILE_open_new_resource_file_new);
    setHook((void*)0x004D4F30, RESFILE_locate_resource_new);
    setHook((void*)0x004D5120, RESFILE_Extract_to_File_new);
    setHook((void*)0x004D5210, RESFILE_Make_File_new);
    setHook((void*)0x004D52B0, RESFILE_Decommit_Mapped_Memory_new);

    //callers
    /*set_call(0x0042BD0B);
    set_call(0x0042BDEA);
    set_call(0x0045806B);
    set_call(0x004B7BE6);
    set_call(0x004D513F);
    set_call(0x004D521E);
    set_call(0x00542A29);
    set_call(0x00544DC1);
    set_call(0x005C626B);*/

    //writeDword(0x004D50B0, 1);

    setHook((void*)0x00458066, on_palette_load);
    setHook((void*)0x0045827A, on_palette_load_2);
    setHook((void*)0x004B80DE, on_panel_setup);
    setHook((void*)0x00542A29, on_shape_load);

}
#pragma optimize( "", on )
