#include "stdafx.h"

#include "drsfile.h"

DRS::DRS()
{
    init = 0;
    loaded = 0;
    f = NULL;
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
    drsFile = new char[strlen(filename) + 1];
    strcpy(drsFile, filename);

    if (!(f = fopen(filename, "rb")))
        return false;

    fread(&hdr, sizeof(DRS_header), 1, f);

    if (strncmp(hdr.version, "1.00", 4) ||
        strncmp(hdr.ftype, "swbg", 4))
    {
        fclose(f);
        return false;
    }

    tInfo = new DRS_tableInfo[hdr.nTables];
    for (int i = 0; i < hdr.nTables; i++)
        fread(&tInfo[i], sizeof(DRS_tableInfo), 1, f);

    tEntries = new DRS_tableEntry*[hdr.nTables];
    for (int i = 0; i < hdr.nTables; i++)
    {
        tEntries[i] = new DRS_tableEntry[tInfo[i].nFiles];
        fseek(f, tInfo[i].tableOffset, SEEK_SET);
        fread(tEntries[i], sizeof(DRS_tableEntry), tInfo[i].nFiles, f);
    }

    init = 1;
    loaded = 1;
    return true;
}

bool DRS::writeDRS()
{
    //calculate file size
    int fsize = 0x54 + 0xC * hdr.nTables;
    for (int i = 0; i < hdr.nTables; i++)
    {
        tInfo[i].tableOffset = fsize;
        fsize += tInfo[i].nFiles * 0xC;
    }
    current_pos = fsize; //files start here
    hdr.off1stFile = current_pos;
    for (int i = 0; i < hdr.nTables; i++)
        for (int j = 0; j < tInfo[i].nFiles; j++)
            fsize += tEntries[i][j].size;
    //set file size
    HANDLE f1 = CreateFile(drsFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    bool result = SetFilePointer(f1, fsize, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER;
    result = result && SetEndOfFile(f1);
    CloseHandle(f1);
    if (f)
        fclose(f);
    if (result)
    {
        f = fopen(drsFile, "rb+");
        //write files
        fseek(f, current_pos, SEEK_SET);
        for (int i = 0; i < hdr.nTables; i++)
            for (int j = 0; j < tInfo[i].nFiles; j++)
            {
                fwrite((void*)tEntries[i][j].offset, 1, tEntries[i][j].size, f);
                free((void*)tEntries[i][j].offset);
                tEntries[i][j].offset = current_pos;
                current_pos += tEntries[i][j].size;
            }
        //write header
        fseek(f, 0, SEEK_SET);
        fwrite(&hdr, sizeof(DRS_header), 1, f);
        //write table headers
        for (int i = 0; i < hdr.nTables; i++)
            fwrite(&tInfo[i], sizeof(DRS_tableInfo), 1, f);
        //write table data
        for (int i = 0; i < hdr.nTables; i++)
            for (int j = 0; j < tInfo[i].nFiles; j++)
                fwrite(&tEntries[i][j], sizeof(DRS_tableEntry), 1, f);
        fclose(f);
    }
    loaded = false;
    return result;
}

void* DRS::getFile(int id, int* size)
{
    void* data = 0;

    if (!f)
        return 0;

    for (int i = 0; i < hdr.nTables; i++)
        for (int j = 0; j < tInfo[i].nFiles; j++)
            if (tEntries[i][j].id == id)
            {
                data = malloc(tEntries[i][j].size);
                *size = tEntries[i][j].size;
                fseek(f, tEntries[i][j].offset, SEEK_SET);
                fread(data, tEntries[i][j].size, 1, f);
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
                return 0;
            fwrite(data, tEntries[i][j].size, 1, g);
            fclose(g);
            free(data);
        }
    }
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
    if (loaded)
        fclose(f);

    if (init)
    {
        for (int i = 0; i < hdr.nTables; i++)
            delete[] tEntries[i];

        delete[] tEntries;
        delete[] tInfo;

        delete[] drsFile;
    }
}

void DRS::setFileName(const char * filename)
{
    drsFile = new char[strlen(filename) + 1];
    strcpy(drsFile, filename);
    init = true;
}

void DRS::addFile(void* data, int size, int id, unsigned long table)
{
    init = 1;
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
        //
        DRS_tableEntry** t3 = new DRS_tableEntry*[hdr.nTables];
        memcpy(t3, tEntries, sizeof(DRS_tableEntry*)*(hdr.nTables - 1));
        delete[] tEntries;
        tEntries = t3;
        tEntries[hdr.nTables - 1] = new DRS_tableEntry[10000];
    }
    ///
    //tInfo[i].nFiles++;
    int k = tInfo[i].nFiles;
    for (; k > 0; k--)
    {
        if (id < tEntries[i][k - 1].id)
            tEntries[i][k] = tEntries[i][k - 1];
        else
            break;
    }
    tEntries[i][k].id = id;
    tEntries[i][k].size = size;
    tEntries[i][k].offset = (long)malloc(size);
    memcpy((void*)tEntries[i][k].offset, data, size);
    tInfo[i].nFiles++;
    ///

    /*tEntries[i][tInfo[i].nFiles - 1].id = id;
    tEntries[i][tInfo[i].nFiles - 1].size = size;
    tEntries[i][tInfo[i].nFiles - 1].offset = (long)malloc(size);
    memcpy((void*)tEntries[i][tInfo[i].nFiles - 1].offset, data, size);*/
    //that's all?..
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
    fclose(f);
    f = fopen(drsFile, "rb+");

    fseek(f, offset, SEEK_SET);
    fwrite(&val, sizeof(int), 1, f);

    fclose(f);
    f = fopen(drsFile, "rb");
}

void DRS::unload()
{
    fclose(f);
    loaded = 0;
}

void DRS::load()
{
    fopen(drsFile, "rb");
    loaded = 1;
}

void DRS::reload()
{
    fclose(f);

    for (int i = 0; i < hdr.nTables; i++)
        delete[] tEntries[i];

    delete[] tEntries;
    delete[] tInfo;

    loadDRS(drsFile);
}
