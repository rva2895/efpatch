#include "stdafx.h"
#include "drsfile.h"

FILE* DRS::load(bool write)
{
    if (drsFile)
        return fopen(drsFile, write ? "rb+" : "rb");
    else
        return NULL;
}

void DRS::unload(FILE* f)
{
    if (f)
        fclose(f);
}

DRS::DRS()
{
    init = false;
    hdr.nTables = 0;
    hdr.off1stFile = 0;
    tInfo = NULL;
    tEntries = NULL;
    memset(hdr.copyright, 0, 0x3C);
    strcpy(hdr.copyright,
        "Copyright(c) 2001 LucasArts Entertainment Company LLC\x1A");
    strncpy(hdr.version, "1.00", 4);
    memset(hdr.ftype, 0, 12);
    strcpy(hdr.ftype, "swbg");
    drsFile = NULL;
}

bool DRS::loadDRS(const char* filename)
{
    setFileName(filename);

    FILE* f = load(false);
    if (!f)
        return false;

    fread(&hdr.copyright, sizeof(hdr.copyright), 1, f);
    fread(&hdr.version, sizeof(hdr.version), 1, f);
    fread(&hdr.ftype, sizeof(hdr.ftype), 1, f);
    fread(&hdr.nTables, sizeof(hdr.nTables), 1, f);
    fread(&hdr.off1stFile, sizeof(hdr.off1stFile), 1, f);

    if (strncmp(hdr.version, "1.00", 4) ||
        strncmp(hdr.ftype, "swbg", 4))
    {
        unload(f);
        return false;
    }

    tInfo = new DRS_tableInfo[hdr.nTables];
    for (int i = 0; i < hdr.nTables; i++)
    {
        fread(&tInfo[i].ext, sizeof(tInfo[i].ext), 1, f);
        fread(&tInfo[i].tableOffset, sizeof(tInfo[i].tableOffset), 1, f);
        fread(&tInfo[i].nFiles, sizeof(tInfo[i].nFiles), 1, f);

        tInfo[i].capacity = tInfo[i].nFiles;
    }

    tEntries = new DRS_tableEntry*[hdr.nTables];
    for (int i = 0; i < hdr.nTables; i++)
    {
        tEntries[i] = new DRS_tableEntry[tInfo[i].nFiles];
        fseek(f, tInfo[i].tableOffset, SEEK_SET);
        for (int j = 0; j < tInfo[i].nFiles; j++)
        {
            fread(&tEntries[i][j].id, sizeof(tEntries[i][j].id), 1, f);
            fread(&tEntries[i][j].offset, sizeof(tEntries[i][j].offset), 1, f);
            fread(&tEntries[i][j].size, sizeof(tEntries[i][j].size), 1, f);

            tEntries[i][j].data = NULL;
        }
    }

    init = true;
    unload(f);
    return true;
}

bool DRS::writeDRS()
{
    char align_data[4];
    memset(align_data, 0, 4);

    //calculate file size
    int fsize = 0x54 + 0xC * hdr.nTables;
    for (int i = 0; i < hdr.nTables; i++)
    {
        tInfo[i].tableOffset = fsize;
        fsize += tInfo[i].nFiles * 0xC;
    }
    int current_pos = fsize; //files start here
    hdr.off1stFile = current_pos;
    for (int i = 0; i < hdr.nTables; i++)
        for (int j = 0; j < tInfo[i].nFiles; j++)
            fsize += tEntries[i][j].size;
    //set file size
    HANDLE f1 = CreateFile(drsFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    bool result = SetFilePointer(f1, fsize, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER;
    result = result && SetEndOfFile(f1);
    CloseHandle(f1);
    
    if (result)
    {
        FILE* f = load(true);
        if (f)
        {
            //write files
            fseek(f, current_pos, SEEK_SET);
            for (int i = 0; i < hdr.nTables; i++)
                for (int j = 0; j < tInfo[i].nFiles; j++)
                {
                    fwrite(tEntries[i][j].data, tEntries[i][j].size, 1, f);
                    tEntries[i][j].offset = current_pos;
                    current_pos += tEntries[i][j].size;
                    //align
                    if (current_pos % 4)
                    {
                        int align_off = 4 - current_pos % 4;
                        fwrite(align_data, 1, align_off, f);
                        current_pos += align_off;
                    }
                }
            //write header
            fseek(f, 0, SEEK_SET);
            fwrite(&hdr.copyright, sizeof(hdr.copyright), 1, f);
            fwrite(&hdr.version, sizeof(hdr.version), 1, f);
            fwrite(&hdr.ftype, sizeof(hdr.ftype), 1, f);
            fwrite(&hdr.nTables, sizeof(hdr.nTables), 1, f);
            fwrite(&hdr.off1stFile, sizeof(hdr.off1stFile), 1, f);
            //write table headers
            for (int i = 0; i < hdr.nTables; i++)
            {
                fwrite(&tInfo[i].ext, sizeof(tInfo[i].ext), 1, f);
                fwrite(&tInfo[i].tableOffset, sizeof(tInfo[i].tableOffset), 1, f);
                fwrite(&tInfo[i].nFiles, sizeof(tInfo[i].nFiles), 1, f);
            }
            //write table data
            for (int i = 0; i < hdr.nTables; i++)
                for (int j = 0; j < tInfo[i].nFiles; j++)
                {
                    fwrite(&tEntries[i][j].id, sizeof(tEntries[i][j].id), 1, f);
                    fwrite(&tEntries[i][j].offset, sizeof(tEntries[i][j].offset), 1, f);
                    fwrite(&tEntries[i][j].size, sizeof(tEntries[i][j].size), 1, f);
                }
            unload(f);
        }
        else
            result = false;
    }
    return result;
}

void* DRS::getFile(int id, int* size)
{
    void* data = NULL;
    *size = 0;
    FILE* f = load(false);
    if (f)
    {
        for (int i = 0; i < hdr.nTables; i++)
            for (int j = 0; j < tInfo[i].nFiles; j++)
                if (tEntries[i][j].id == id)
                {
                    data = malloc(tEntries[i][j].size);
                    *size = tEntries[i][j].size;
                    fseek(f, tEntries[i][j].offset, SEEK_SET);
                    fread(data, tEntries[i][j].size, 1, f);
                }
        unload(f);
    }

    return data;
}

int DRS::extractFiles()
{
    void* data;
    char* ext;
    char filename[0x100];
    FILE* g;
    int size_total = 0;

    FILE* f = load(false);
    if (!f)
        return 0;

    for (int i = 0; i < hdr.nTables; i++)
    {
        switch (*(unsigned long*)&tInfo[i].ext)
        {
        case 0x62696E61:
            ext = ".bin";
            break;
        case 0x736C7020:
            ext = ".slp";
            break;
        case 0x77617620:
            ext = ".wav";
            break;
        default:
            ext = "";
            break;
        }
        for (int j = 0; j < tInfo[i].nFiles; j++)
        {
            data = malloc(tEntries[i][j].size);
            size_total += tEntries[i][j].size;
            fseek(f, tEntries[i][j].offset, SEEK_SET);
            fread(data, tEntries[i][j].size, 1, f);
            sprintf(filename, "%d%s", tEntries[i][j].id, ext);
            HANDLE f1 = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            SetFilePointer(f1, tEntries[i][j].size, 0, FILE_BEGIN);
            SetEndOfFile(f1);
            CloseHandle(f1);
            g = fopen(filename, "rb+");
            if (!g)
            {
                free(data);
                return 0;
            }
            fwrite(data, tEntries[i][j].size, 1, g);
            fclose(g);
            free(data);
        }
    }
    unload(f);
    return size_total;
}

char* DRS::listFiles(int* count)
{
    int cur = 0;
    int len = 0;
    *count = 0;
    char* str = 0;

    for (int i = 0; i < hdr.nTables; i++)
        if (!strncmp(tInfo[i].ext, " pls", 4))
            for (int j = 0; j < tInfo[i].nFiles; j++)
            {
                if ((len - cur) < 10)
                {
                    len += 100;
                    str = (char*)realloc(str, len);
                }
                cur += sprintf(str + cur, "%d\0", tEntries[i][j].id);
                cur++;
                (*count)++;
            }

    return str;
}

DRS::~DRS()
{
    if (init)
    {
        for (int i = 0; i < hdr.nTables; i++)
        {
            for (int j = 0; j < tInfo[i].nFiles; j++)
                free(tEntries[i][j].data);
            delete[] tEntries[i];
        }

        delete[] tEntries;
        delete[] tInfo;
    }

    delete[] drsFile;
}

void DRS::setFileName(const char* filename)
{
    delete[] drsFile;

    int size = GetFullPathName(filename, 0, NULL, NULL);
    drsFile = new char[size];
    GetFullPathName(filename, size, drsFile, NULL);
}

void DRS::addFile(void* data, int size, int id, unsigned long table)
{
    init = true;
    //check tables
    int i = 0;
    while (i < hdr.nTables && *(unsigned long*)&tInfo[i].ext != table)
        i++;
    if (i >= hdr.nTables)
    {
        //add new table
        hdr.nTables++;
        DRS_tableInfo* t2 = new DRS_tableInfo[hdr.nTables];
        memcpy(t2, tInfo, sizeof(DRS_tableInfo)*(hdr.nTables - 1));
        delete[] tInfo;
        tInfo = t2;
        *(unsigned long*)&tInfo[hdr.nTables - 1].ext = table;
        tInfo[hdr.nTables - 1].nFiles = 0;
        tInfo[hdr.nTables - 1].tableOffset = 0;
        tInfo[hdr.nTables - 1].capacity = 1000;
        //
        DRS_tableEntry** t3 = new DRS_tableEntry*[hdr.nTables];
        memcpy(t3, tEntries, sizeof(DRS_tableEntry*)*(hdr.nTables - 1));
        delete[] tEntries;
        tEntries = t3;
        tEntries[hdr.nTables - 1] = new DRS_tableEntry[tInfo[hdr.nTables - 1].capacity];
        memset(tEntries[hdr.nTables - 1], 0, sizeof(DRS_tableEntry) * tInfo[hdr.nTables - 1].capacity);
    }

    if (tInfo[i].nFiles >= tInfo[i].capacity)
    {
        tInfo[i].capacity += 1000;
        DRS_tableEntry* new_t = new DRS_tableEntry[tInfo[i].capacity];
        memset(new_t, 0, sizeof(DRS_tableEntry) * tInfo[i].capacity);
        memcpy(new_t, tEntries[i], sizeof(DRS_tableEntry) * tInfo[i].nFiles);
        delete[] tEntries[i];
        tEntries[i] = new_t;
    }

    int j = 0;
    for (; j < tInfo[i].nFiles; j++)
        if (tEntries[i][j].id >= id)
            break;

    if (tEntries[i][j].id != id)
    {
        for (int k = tInfo[i].nFiles; k > j; k--)
            tEntries[i][k] = tEntries[i][k - 1];
        tInfo[i].nFiles++;
    }
    else
        free(tEntries[i][j].data);

    tEntries[i][j].id = id;
    tEntries[i][j].size = size;
    tEntries[i][j].offset = 0;
    tEntries[i][j].data = malloc(size);
    memcpy(tEntries[i][j].data, data, size);
}

int DRS::getFileOffset(int id)
{
    for (int i = 0; i < hdr.nTables; i++)
        for (int j = 0; j < tInfo[i].nFiles; j++)
            if (tEntries[i][j].id == id)
                return tEntries[i][j].offset;
    return -1;
}

int DRS::getTableEntryOffset(int id)
{
    for (int i = 0; i < hdr.nTables; i++)
        for (int j = 0; j < tInfo[i].nFiles; j++)
            if (tEntries[i][j].id == id)
                return (tInfo[i].tableOffset + sizeof(DRS_tableEntry)*j);
    return -1;
}

void DRS::writeInt(int offset, int val)
{
    FILE* f = load(true);
    if (f)
    {
        fseek(f, offset, SEEK_SET);
        fwrite(&val, sizeof(int), 1, f);
        unload(f);
    }
}
