#pragma once

#include "stdafx.h"

struct DRS_header
{
	char copyright [0x3C];
	char version [4];
	char ftype [12];
	long nTables;
	long off1stFile;
};

struct DRS_tableInfo
{
	char ext [4];
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
	FILE* f;

	char* drsFile;

	bool init;
	bool loaded;

	DRS_header hdr;
	DRS_tableInfo* tInfo;
	DRS_tableEntry** tEntries;

public:
	DRS ();
	//DRS (const char* filename);
	~DRS ();

	char* listFiles (int* count);
	void* getFile (int id, int* size);
	int getFileOffset (int id);
	int getTableEntryOffset (int id);

	void writeInt (int offset, int val);

	bool loadDRS (const char* filename);

	void unload ();
	void load ();
	void reload ();
};
