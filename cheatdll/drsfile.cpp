#include "stdafx.h"

#include "drsfile.h"

DRS::DRS ()
{
	init = 0;
	loaded = 0;
	f = 0;
}

bool DRS::loadDRS (const char* filename)
{
	drsFile = new char [strlen(filename)+1];
	strcpy (drsFile, filename);

	if (!(f = fopen (filename, "rb")))
		return false;

	fread (&hdr, sizeof (DRS_header), 1, f);

	if ( strncmp (hdr.version, "1.00", 4) ||
		strncmp (hdr.ftype, "swbg", 4) )
	{
		fclose (f);
		return false;
	}

	tInfo = new DRS_tableInfo [hdr.nTables];
	for (int i = 0; i < hdr.nTables; i++)
		fread (&tInfo[i], sizeof (DRS_tableInfo), 1, f);

	tEntries = new DRS_tableEntry* [hdr.nTables];
	for (int i = 0; i < hdr.nTables; i++)
	{
		tEntries[i] = new DRS_tableEntry [tInfo[i].nFiles];
		fseek (f, tInfo[i].tableOffset, SEEK_SET);
		fread (tEntries[i], sizeof (DRS_tableEntry), tInfo[i].nFiles, f);
	}

	init = 1;
	loaded = 1;
	return true;
}

void* DRS::getFile (int id, int* size)
{
	void* data = 0;

	if (!f)
		return 0;

	for (int i = 0; i < hdr.nTables; i++)
		for (int j = 0; j < tInfo[i].nFiles; j++)
			if (tEntries[i][j].id == id)
			{
				data = malloc (tEntries[i][j].size);
				*size = tEntries[i][j].size;
				fseek (f, tEntries[i][j].offset, SEEK_SET);
				fread (data, tEntries[i][j].size, 1, f);
			}

	return data;
}

char* DRS::listFiles (int* count)
{
	int cur = 0;
	int len = 0;
	*count = 0;
	char* str = 0;

	for (int i = 0; i < hdr.nTables; i++)
		if (!strncmp (tInfo[i].ext, " pls", 4))
			for (int j = 0; j < tInfo[i].nFiles; j++)
			{
				if ((len - cur) < 10)
				{
					len += 100;
					str = (char*) realloc (str, len);
				}
				cur += sprintf (str + cur, "%d\0", tEntries[i][j].id);
				cur++;
				(*count) ++;
			}

	return str;
}

DRS::~DRS ()
{
	if (loaded)
		fclose (f);

	if (init)
	{
		for (int i = 0; i < hdr.nTables; i++)
			delete [] tEntries[i];

		delete [] tEntries;
		delete [] tInfo;

		delete [] drsFile;
	}
}

int DRS::getFileOffset (int id)
{
	for (int i = 0; i < hdr.nTables; i++)
		for (int j = 0; j < tInfo[i].nFiles; j++)
			if (tEntries[i][j].id == id)
				return tEntries[i][j].offset;
	return -1;
}

int DRS::getTableEntryOffset (int id)
{
	for (int i = 0; i < hdr.nTables; i++)
		for (int j = 0; j < tInfo[i].nFiles; j++)
			if (tEntries[i][j].id == id)
				return (tInfo[i].tableOffset + sizeof(DRS_tableEntry)*j);
	return -1;
}

void DRS::writeInt (int offset, int val)
{
	fclose (f);
	f = fopen (drsFile, "rb+");

	fseek (f, offset, SEEK_SET);
	fwrite (&val, sizeof(int), 1, f);

	fclose (f);
	f = fopen (drsFile, "rb");
}

void DRS::unload ()
{
	fclose (f);
	loaded = 0;
}

void DRS::load ()
{
	fopen (drsFile, "rb");
	loaded = 1;
}

void DRS::reload ()
{
	fclose (f);

	for (int i = 0; i < hdr.nTables; i++)
		delete [] tEntries[i];

	delete [] tEntries;
	delete [] tInfo;

	loadDRS (drsFile);
}
