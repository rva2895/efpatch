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

void __fastcall RESFILE_open_new_resource_file_new(char* resFileName, char* password, char* path, int open_mode)
{
    UNREFERENCED_PARAMETER(password);
    UNREFERENCED_PARAMETER(open_mode);
    char buf[MAX_PATH];
    sprintf_s(buf, _countof(buf), "%s%s", path, resFileName);
    resfile.add_resource_file(buf);
}

void RESFILE::add_resource_file(char* filename)
{
    log("%sattempting to add new resource file: %s", resfile_txt, filename);

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
    //bool found = false;
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

void set_call(DWORD addr)
{
    writeDword(addr + 1, (DWORD)RESFILE_load_new - addr - 5);
}

#pragma optimize( "s", on )
void setResFileHooks()
{
    //close
    writeByte(0x004D4E20, 0xC3);
    //decommit
    writeByte(0x004D52B0, 0xC3);

    setHook((void*)0x004D5060, RESFILE_load_new);
    setHook((void*)0x004D4C10, RESFILE_open_new_resource_file_new);

    //callers
    set_call(0x0042BD0B);
    set_call(0x0042BDEA);
    set_call(0x0045806B);
    set_call(0x004B7BE6);
    set_call(0x004D513F);
    set_call(0x004D521E);
    set_call(0x00542A29);
    set_call(0x00544DC1);
    set_call(0x005C626B);

}
#pragma optimize( "", on )
