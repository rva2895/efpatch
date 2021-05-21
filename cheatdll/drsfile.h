#pragma once

#include "stdafx.h"

#define DRS_BIN 0x62696E61
#define DRS_SLP 0x736C7020

struct DRS_header
{
    char copyright[0x3C];
    char version[4];
    char ftype[12];
    long nTables;
    long off1stFile;
};

struct DRS_tableInfo
{
    char ext[4];
    long tableOffset;
    long nFiles;
};

struct DRS_tableEntry
{
    long id;
    long offset;
    long size;
};

class DRS
{
private:
    //FILE* f;

    char* drsFile;

    bool init;

    DRS_header hdr;
    DRS_tableInfo* tInfo;
    DRS_tableEntry** tEntries;

    FILE* load(bool write);
    void unload(FILE* f);

public:
    DRS();
    ~DRS();

    void setFileName(const char* filename);
    void addFile(void* data, int size, int id, unsigned long table);

    char* listFiles(int* count);
    void* getFile(int id, int* size);
    int extractFiles();
    int getFileOffset(int id);
    int getTableEntryOffset(int id);

    void writeInt(int offset, int val);

    bool loadDRS(const char* filename);
    bool writeDRS();
};
