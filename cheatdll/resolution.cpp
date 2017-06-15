#include "stdafx.h"

#include "resolution.h"
#include "registry.h"
#include "drsfile.h"

struct drsTableHdr
{
	int type;
	int offset;
	int count;
};

int newXsize;
int newYsize;

void ResToFile (const char* res, const char* filename)
{
	char tmpBuf [255];
	HRSRC foundedRes = FindResource(GetModuleHandle ("efpatch.dll"), res, "RAW");
	HGLOBAL loadedRes = LoadResource (GetModuleHandle ("efpatch.dll"), foundedRes);
	void* data =  LockResource (loadedRes);
	unsigned long size = SizeofResource (GetModuleHandle ("efpatch.dll"), foundedRes);
	FILE* file = fopen (filename, "wb");
	if (file == NULL)
	{
		sprintf (tmpBuf, "Cannot create file: %s", filename);
		MessageBox (0, tmpBuf, "Error", MB_ICONERROR);
		//addLog ("Cannot create file. Setup aborted\x0D\x0A");
		//_endthread ();
	}
	fwrite (data, 1, size, file);
	fclose (file);
}

/*void drsCreate ()
{
	int off = 0;
	int pos = 0;
	drsTableHdr h;
	FILE* f = fopen ("data\\widescrn_x2.drs", "wb");
	fwrite ("Copyright (c) 2001 LucasArts Entertainment Company LLC\0\0\0\0\0"
		"1.00swbg\0\0\0\0\0\0\0\0\x02\0\0\0", 0x50, 1, f);
	fwrite (&off, 4, 1, f);
	h.type = 1651076705;
	h.count = 1;
	h.offset = 0;
	fwrite (&h, sizeof (drsTableHdr), 1, f);
	h.type = 1936486432;
	h.count = 9;
	h.offset = 0;
	fwrite (&h, sizeof (drsTableHdr), 1, f);
	pos = ftell (f);
}*/

/*void drsWrite (char* type, int id, void* data, int size)
{

}*/

//50032 - edit small
//50033 - edit medium
//50034 - edit large
//50100 - load small
//50101 - load medium
//50102 - load large

void createNewFiles (int y)
{
	if (y >= 1024)
		ResToFile ("IDD_DRS_TEMPLATE_1280", "data\\wide.drs");
	else
		ResToFile ("IDD_DRS_TEMPLATE_1024", "data\\wide.drs");
}

void* memSLP; //global variable to store loaded SLP

int oldSize;
int newSize;


void rd //read data from memSLP to mem
	(int offset,
	int len,
	void* mem)
{
	memcpy (mem, (void*)((char*)memSLP + offset), len);
}

/*int getDrsPos (HANDLE hFile, int id)
{
	unsigned long readwritten;
	//ReadFile (hFile, &_n, 4, &readwritten, 0);
	SetFilePointer (hFile, 
}*/

void* readDRSItemToMem (const char* filename, int id, int* size)
{
	FILE* f;// = fopen (filename, "rb");

	//if (!file)
	//	return 0;
	DRS drs;
	if (!drs.loadDRS (filename))
	{
		log ("Resolution: cannot load %s", filename);
		return 0;
	}

	int oldOffset;

	log ("Resolution: loading %s:%d...", filename, id);

	int entryOffset = drs.getTableEntryOffset (id);
	if (entryOffset == -1)
	{
		log ("Resolution: cannot load %d", id);
		return 0;
	}

	drs.unload ();

	f = fopen (filename, "rb+");
	fseek (f, entryOffset+4, SEEK_SET);

	fread (&oldOffset, 4, 1, f); //reading DRSItemDesc.offset
	fread (&oldSize, 4, 1, f); //reading DRSItemDesc.size

	//this does not look nice, but we need more memory than the actual file size,
	//because we will edit it

	void* m = malloc (oldSize*2);
	fseek (f, oldOffset, SEEK_SET);
	fread (m, 1, oldSize, f);

	fclose (f);

	*size = oldSize; //returning the size

	log ("Resolution: loaded %d: offset=%d, size=%d", id, oldOffset, oldSize);

	return m;
}

void writeDRS (const char* filename, int id, void* data, int size)
{
	//memSLP = readSLPtoMem ();

	HANDLE hFile = CreateFile (filename,
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (int x = GetLastError ()) //halt if can't open file...
	{
		MessageBox (0, "Can't open file", "Error", MB_ICONERROR);
		exit (0);
	}

	unsigned long readwritten;

	log ("Resolution: looking for id=%d in %s", id, filename);

	DRS drs;
	if (!drs.loadDRS (filename))
	{
		log ("Resolution: cannot load %s", filename);
		return;
	}

	int entryOffset = drs.getTableEntryOffset (id);
	if (entryOffset == -1)
	{
		log ("Resolution: cannot load %d", id);
		return;
	}

	drs.unload ();
	
	int newOffset = GetFileSize (hFile, 0);
	SetFilePointer (hFile, entryOffset+4, 0, FILE_BEGIN);
	WriteFile (hFile, &newOffset, 4, &readwritten, 0); //edit DRS item offset
	//newSize = oldSize;
	WriteFile (hFile, &size, 4, &readwritten, 0); //edit DRS item size
	SetFilePointer (hFile, newOffset, 0, FILE_BEGIN);
	WriteFile (hFile, data, size, &readwritten, 0); //write data at end of file
	CloseHandle (hFile);

	log ("Resolution: updated %d: offset=%d, size=%d", id, newOffset, size);
}

void parseSLP (int newH, int id, bool useWide)
{
	//For description of SLP format, see http://hg.boekabart.net/aoe2wide/src/192f34b66102e3f88c624a6cca6f6fcb2b0a5929/slp.txt?at=default
	
	log ("Resolution: stretching SLP id=%d...", id);

	int oldSize;

	int oldH;
	int oldV;
	int stretchAt; //X coordinate at which we will 'split' the image and insert repeating pixels

	if (newYsize >= 1024)
	{
		oldH = 1280;
		oldV = 1024;
		if (id == 51142)
			stretchAt = 545;
		else if (id == 51147)
			stretchAt = 535;
		else if (id == 51148)
			stretchAt = 530;
		else if (id == 51149)
			stretchAt = 526;
		else
			stretchAt = 525;
	}
	else if (newYsize >= 768)
	{
		oldH = 1024;
		oldV = 768;
		if (id == 51128)
			stretchAt = 431;
		else if (id == 51124)
			stretchAt = 424;
		else if (id == 51126)
			stretchAt = 423;
		else if (id == 51129)
			stretchAt = 424;
		else
			stretchAt = 416;
	}
	else //should never be reached, but just in case...
	{
		MessageBox (0, "Errrrrrrror", "\0", MB_ICONERROR);
		exit (0);
	}

	//if (useX1)
	//	memSLP = readDRSItemToMem (newDRSxPath, id, &oldSize);
	//else
	//	memSLP = readDRSItemToMem (newDRSPath, id, &oldSize);
	if (useWide)
		memSLP = readDRSItemToMem ("data\\widescrn_x2.drs", id, &oldSize);
	else
		memSLP = readDRSItemToMem ("data\\interfac_x2.drs", id, &oldSize);

	if (useWide)
	{
		if (newYsize >= 1080)
		{
			oldH = 1920;
			oldV = 1080;
		}
	}

	//memSLP = readSLPtoMem ();

	int X;
	int Y;
	rd (0x30, 4, &X);
	rd (0x34, 4, &Y);
	int ShapeDataOffsets;
	rd (0x20, 4, &ShapeDataOffsets);

	unsigned char command;
	int j = 0; //current row
	int i = 0; //offset in a current row
	int pxls = 0; // pixels
	int len;
	int brkPnt; //determines where we need to 'break' the copy pixels command to insert repeating pixels
	int addlen; //stores length of the pixel data after the 'breakpoint'

	unsigned char nextbyte;

	int addPxls = 0; //used to determine how many pixels we need to add TOTAL
	unsigned char addPxlsNow = 0; //determine how many pixels will be added using ONE FILL BLOCK COMMAND

	int* dataOffsets = (int*)((char*)memSLP + ShapeDataOffsets); //pointer to array of data pointers

	char szBuf [255]; //buffer for string operations

	newSize = oldSize;

if (!useWide)
for (j = 0; j < Y; j++) //for each horizontal line
{
	i = 0; //zero out the counters
	pxls = 0;

	if ((j > 100) && (id == 51149))             //fix for zann ui
		stretchAt = 518;

	while (1)
	{
		if (i > 10000)
		{
			MessageBox (0, "Too many...", "Scan Error", 0); //actually this should be rewritten...
			break;
		}
		if (pxls > stretchAt)
		{
			//normally, this condition should never be reached, however, just in case...

			if ((j + 1) < Y)
				dataOffsets[j+1] += newSize - oldSize;
			break;
		}

		rd (dataOffsets[j] + i, 1, &command);
		i++;
		if ( (command & 0x3) == 0x0 ) //read pixels (small)
		{
			len = command >> 2;
			i += len;
			pxls += len;
			if (pxls >= stretchAt)
			{
				i -= len;
				brkPnt = len - (pxls - stretchAt); //brkPnt must be at stretchAt absolute pixels exactly
				addlen = len - brkPnt;
				i--;
				len = brkPnt;
				command = len << 2; //update command for shorter length
				command = command & 0xFC;
				*((unsigned char*)memSLP + dataOffsets[j] + i) = command;
				i++;
				i += len; //move to 'breakpoint'
				rd (dataOffsets[j] + i, 1, &nextbyte); //read color to use to fill

				addPxls = newH - oldH;

				while (addPxls > 0)
				{
					if (addPxls > 0xFF) //fill block command supports up to 0xFF repeating pixels
					{
						addPxlsNow = 0xFF;
						addPxls -= addPxlsNow;
					}
					else
					{
						addPxlsNow = addPxls;
						addPxls = 0;
					}
					memmove ((unsigned char*)memSLP + dataOffsets[j] + i + 3, (unsigned char*)memSLP + dataOffsets[j] + i, newSize - dataOffsets[j] - i);
					newSize += 3;
					*((unsigned char*)memSLP + dataOffsets[j] + i) = 0x07; //fill block
					*((unsigned char*)memSLP + dataOffsets[j] + i + 1) = addPxlsNow;
					*((unsigned char*)memSLP + dataOffsets[j] + i + 2) = nextbyte; //color
					i += 3;
				}
				
				if (addlen != 0)
				{
					memmove ((unsigned char*)memSLP + dataOffsets[j] + i + 1, (unsigned char*)memSLP + dataOffsets[j] + i, newSize - dataOffsets[j] - i);
					newSize ++;
					command = addlen << 2;
					command = command & 0xFC;
					*((unsigned char*)memSLP + dataOffsets[j] + i) = command;
				}

				if ((j + 1) < Y)
					dataOffsets[j+1] += newSize - oldSize; //edit pointers in array of data pointers
				break;
			}
		}
		else if ( (command & 0x3) == 0x1 ) //skip pixels (small)
		{
			i = i;
			len = command >> 2;
			pxls += len;
		}
		else if ( (command & 0xF) == 0x2 ) //read pixels (big)
		{
			len = command;
			len = len & 0xF0;
			len = len << 4;
			rd (dataOffsets[j] + i, 1, &nextbyte);
			len += nextbyte;
			i++;
			i += len;
			pxls += len;
			if (pxls >= stretchAt)
			{
				i -= len;
				brkPnt = len - (pxls - stretchAt);
				addlen = len - brkPnt;
				i--;
				len = brkPnt;
				command = len / 0x100; //see SLP format description to understand how this works...
				command = command << 4;
				command = command | 0x2;
				i--;
				*((unsigned char*)memSLP + dataOffsets[j] + i) = command;
				i++;
				*((unsigned char*)memSLP + dataOffsets[j] + i) = len % 0x100;
				i++;
				i += len; //move to 'breakpoint'
				rd (dataOffsets[j] + i, 1, &nextbyte); //read color to use to fill

				addPxls = newH - oldH;

				while (addPxls > 0)
				{
					if (addPxls > 0xFF)
					{
						addPxlsNow = 0xFF;
						addPxls -= addPxlsNow;
					}
					else
					{
						addPxlsNow = addPxls;
						addPxls = 0;
					}
					memmove ((unsigned char*)memSLP + dataOffsets[j] + i + 3, (unsigned char*)memSLP + dataOffsets[j] + i, newSize - dataOffsets[j] - i);
					newSize += 3;
					*((unsigned char*)memSLP + dataOffsets[j] + i) = 0x07;
					*((unsigned char*)memSLP + dataOffsets[j] + i + 1) = addPxlsNow;
					*((unsigned char*)memSLP + dataOffsets[j] + i + 2) = nextbyte;
					i += 3;
				}

				if (addlen != 0)
				{
					memmove ((unsigned char*)memSLP + dataOffsets[j] + i + 2, (unsigned char*)memSLP + dataOffsets[j] + i, newSize - dataOffsets[j] - i);
					newSize += 2;
					command = addlen / 0x100;
					command = command << 4;
					command = command | 0x2;
					*((unsigned char*)memSLP + dataOffsets[j] + i) = command;
					i++;
					*((unsigned char*)memSLP + dataOffsets[j] + i) = addlen % 0x100;
				}
				//rd (dataOffsets[j] + i, 1, &nextbyte); //read color
				i--;

				

				if ((j + 1) < Y)
					dataOffsets[j+1] += newSize - oldSize;
				break;
			}
		}
		else if ( (command & 0xF) == 0x3 ) //skip pixels (big)
		{
			//rd (oldOffset + dataOffset1 + i, 1, &nextbyte);
			i++;
			//len = command >> 4;
			//len += nextbyte;
		}
		else if ( (command & 0xF) == 0x7 ) //fill block
		{
			len = command >> 4;
			if (len == 0)
			{
				rd (dataOffsets[j] + i, 1, &nextbyte);
				i++;
				len = nextbyte;
				pxls += len;
			}
			else
			{
				pxls += len;
			}

			if (pxls >= stretchAt)
			{
				rd (dataOffsets[j] + i, 1, &nextbyte); //read color
				i++;

				addPxls = newH - oldH;

				while (addPxls > 0)
				{
					if (addPxls > 0xFF)
					{
						addPxlsNow = 0xFF;
						addPxls -= addPxlsNow;
					}
					else
					{
						addPxlsNow = addPxls;
						addPxls = 0;
					}
					memmove ((unsigned char*)memSLP + dataOffsets[j] + i + 3, (unsigned char*)memSLP + dataOffsets[j] + i, newSize - dataOffsets[j] - i);
					newSize += 3;
					*((unsigned char*)memSLP + dataOffsets[j] + i) = 0x07;
					*((unsigned char*)memSLP + dataOffsets[j] + i + 1) = addPxlsNow; //for our 1152 res
					*((unsigned char*)memSLP + dataOffsets[j] + i + 2) = nextbyte;
					i += 3;
				}

				if ((j + 1) < Y)
					dataOffsets[j+1] += newSize - oldSize;
				break;
			}
			//i += len;
			i++;
		}
		else if ( (command & 0xF) == 0xF ) //End of line
		{
			//sprintf (szBuf, "Scan done. %d pixels in a row found. EOL at %X", pxls, i);
			//MessageBox (hWnd, szBuf, "Scan Done", 0);
			break;
		}
		else //should never be reached, but if it does, continuing patching may cause a lot of horrible errors...
		{
			sprintf (szBuf, "ERROR: Unrecognised command %X found at %X", command, dataOffsets[j]+i);
			MessageBox (0, szBuf, "Scan Error", MB_ICONERROR);
			exit (0);
			break;
		}
	}

}
	if (!useWide)
	{
		*(int*)((unsigned char*)memSLP+0x30) = newXsize; //Width
		*(int*)((unsigned char*)memSLP+0x3C) -= (newYsize - oldV);//hotspot_Y, which determines where to draw the main UI bar
	}
	//if (useX1)
	//	writeDRS (newDRSxPath, id, memSLP, newSize);
	//else
	//	writeDRS (newDRSPath, id, memSLP, newSize);
	writeDRS ("data\\wide.drs", id, memSLP, newSize);

	free (memSLP);

	log ("Resolution: SLP %d updated", id);
}

/*void wrt (int offset, int data) //write data to offset in file
{
	int d = data;
	fseek (file, offset, SEEK_SET);
	fwrite (&d, 4, 1, file);
}*/

bool patchEXE (int X, int Y) //needs to be completed...
{
	//file = fopen (mainEXE, "rb+");
	//if (file == 0)
	//	return false;
	log ("Patching EXE for resolution %dx%d...", X, Y);
	
	if (Y >= 1024)
	{
		setIntF(0x24B66, Y); //
		setIntF(0x24BB3, X); //
		setIntF(0x261FD, Y);
		setIntF(0x2623A, X); //ok
		setIntF(0x2680C, Y); //ok
		setIntF(0x26845, X); //ok
		setIntF(0x290A8, Y); //test
		setIntF(0x2AA3E, X); //ok
		setIntF(0x2AA52, X-340); //could be
		setIntF(0x5A365, X); //ok
		setIntF(0x5A3CE, X); //ok
		setIntF(0x5ACBA, X); //ok
		setIntF(0x5ACBF, Y); //ok
		setIntF(0x5ACFB, X+1); //ok
		setIntF(0x639FB, Y); //ok
		setIntF(0x63B9E, Y+1); //mm ok
		setIntF(0x63E9F, Y); //ok
		//float hMul = (float)Y / 600;
		//hMul = 0;
		//setInt (0x00463EAB, *(int*)&hMul);
		setIntF(0x6A916, X); //ok
		setIntF(0x6A920, Y-98); //ok
		setIntF(0x70D36, X); //ok
		setIntF(0x70DFB, X); //ok
		setIntF(0x70E04, Y); //ok
		setIntF(0x70E16, X+1); //ok
		setIntF(0x70E1F, Y+1); //ok
		setIntF(0x710D4, X); //ok
		setIntF(0x710DB, X+1); //check these 4
		setIntF(0x7114B, X+1);
		setIntF(0x711CD, X+1);
		setIntF(0x71247, X+1);
		setIntF(0xA9411, X); //looks wrong, check
		setIntF(0xB7E39, X); //check
		setIntF(0xBE147, X); //looks wrong, check
		setIntF(0xF670C, X); //ok
		setIntF(0xFDAF2, X+1); //mm ok
		setIntF(0xFDAFB, Y+1); //mm ok
		setIntF(0xFDB20, X); //ok
		setIntF(0xFDB29, Y); //ok
		setIntF(0x1003A8, X); //ok
		setIntF(0x1003D5, X); //ok
		setIntF(0x1003E3, X-298); //some item icon, check
		setIntF(0x1003E8, Y-340);
		//setIntF(0x1003E3, Y-42); //some item icon, old ver
		//setIntF(0x1003E8, X-340);
		setIntF(0x102852, X); //check this again
		setIntF(0x10C987, X); //ok
		setIntF(0x10E4CD, X); //ok
		setIntF(0x10E4D2, Y); //ok
		setIntF(0x10EDCC, X); //weird
		setIntF(0x1D853F, X); //most likely
		setIntF(0x1D98E9, X); //?
		//setIntF(0x1F6757, X); not resolution
	}
	else if (Y >= 768)
	{
		setIntF (0x24B66, X); //ok
		setIntF (0x24BAC, Y); //ok
		setIntF (0x24BB3, X+1); //ok
		setIntF (0x261FD, X); //ok
		setIntF (0x26217, Y); //ok
		setIntF (0x2623A, X+1); //not needed?
		setIntF (0x2680C, X); //ok
		setIntF (0x2683B, Y); //ok
		setIntF (0x26845, X+1); //not needed?
		//setIntF (0x290A8, X); //test. not res?
		setIntF (0x2AA37, X); //ok
		setIntF (0x2AA3E, X+1); //not needed?
		setIntF (0x2AA59, X-274); //-> should be ok
		setIntF (0x5A35E, X); //ok
		setIntF (0x5A365, X+1); //not needed?
		setIntF (0x5A3AA, X); //ok
		setIntF (0x5A3CE, X+1); //not needed?
		setIntF (0x5AC9F, X); //ok
		setIntF (0x5ACA4, Y); //ok
		setIntF (0x5ACBA, X+1); //not needed?
		setIntF (0x5ACBF, Y+1);
		setIntF (0x5ACFB, X+2); //?
		setIntF (0x63923, Y); //ok
		setIntF (0x639FB, Y+1); //block higher res
		setIntF (0x63B9E, Y+2); //block higher res
		setIntF (0x63E85, Y); //ok
		setIntF (0x63E9F, Y+1); //block higher res
		setIntF (0x6A90E, X); //ok
		setIntF (0x6A916, X+1); //block higher res
		setIntF (0x6A930, Y-103); //ok
		setIntF (0x70D2B, X); //ok
		setIntF (0x70D36, X+1); //block higher res
		setIntF (0x70DE0, X); //ok
		setIntF (0x70DE9, Y); //ok
		setIntF (0x70DFB, X+1); //block higher res
		setIntF (0x70E04, Y+1); //--
		setIntF (0x70E16, X+2); //--
		setIntF (0x70E1F, Y+2); //--
		setIntF (0x70EB5, X); //ok
		setIntF (0x70EBE, Y); //ok
		setIntF (0x70F5D, X); //ok
		setIntF (0x70F66, Y); //ok
		setIntF (0x7100B, X); //ok
		setIntF (0x71014, Y); //ok
		setIntF (0x7107E, X); //ok
		setIntF (0x710D4, X+1); //block higher res
		setIntF (0x710DB, X+2); //block higher res
		setIntF (0x71144, X); //ok?
		setIntF (0x7114B, X+2); //block higher res
		setIntF (0x711C6, X); //ok?
		setIntF (0x711CD, X+2); //block higher res
		setIntF (0x71240, X); //ok?
		setIntF (0x71247, X+2); //block higher res
		//setIntF(0xA9411, X+1); //not res
		setIntF (0xB7E14, X); //ok
		setIntF (0xB7E39, X+1); //ok
		//setIntF(0xBE147, X+1); //not res
		setIntF(0xF6705, X); //ok
		setIntF(0xF670C, X+1); //block higher res
		setIntF(0xFDAF2, X+2); //not needed?
		setIntF(0xFDAFB, Y+2);
		setIntF(0xFDB20, X+1);
		setIntF(0xFDB29, Y+1);
		setIntF(0xFDB6C, X); //ok
		setIntF(0xFDB75, Y); //ok
		//setIntF(0x1003A8, X+1); //test!!!         (!!!)
		setIntF(0x1003D5, X+1); //ok
		setIntF(0x1003FD, X); //ok
		//setIntF(0x1003E3:	D6	57
		//setIntF(0x1003E8:	AC	0D
		setIntF(0x1003FD, X); //ok
		setIntF(0x10040B, Y-35); //ok, but test
		//setIntF(0x100410, X  //same as above
		setIntF(0x102852, X+1); //ok
		setIntF(0x102865, X); //ok, but test surroundings
		setIntF(0x10C94A, X); //should be ok
		setIntF(0x10C987, X+1); //block higher res
		setIntF(0x10E4B2, X); //ok
		setIntF(0x10E4B7, Y); //ok
		setIntF(0x10E4CD, X+1); //block higher res
		setIntF(0x10E4D2, Y+1); //block higher res
		setIntF(0x10EDC5, X); //ok
		setIntF(0x10EDCC, X+1); //ok
		setIntF(0x1D8529, X); //ok
		setIntF(0x1D853F, X+1); //ok
		setIntF(0x1D98DA, X);
		setIntF(0x1D98E9, X+1);
		//setIntF(0x1F6757, X+1); //not res
	}
	else
	{
		MessageBox (0, "Resolutions lower than 768V are not supported", "Error", MB_ICONERROR);
	}

	//fseek (file, 0x299D9C, SEEK_SET); //drs reference
	//fwrite (newDRSref, strlen(newDRSref), 1, file);
	//fseek (file, 0x299F8C, SEEK_SET); //x drs reference
	//fwrite (newDRSxref, strlen(newDRSxref), 1, file);

	//fclose (file);

	if (Y >= 1024)
	{
		setByte (0x0042622F, 0x90);   //force 1280 screen setting
		setByte (0x00426230, 0x90);
		setByte (0x00477509, 0xEB);
	}
	else
	{
		setByte (0x00426203, 0x90);   //force 1024 screen setting
		setByte (0x00426204, 0x90);
		setByte (0x004774F5, 0xEB);
	}

	return true;
}

//char endbuf [4096]; //static memory allocation is bad, but 4096 must be enough...
void* drsRects;

int findRectOffset (const char* itemname)
{
	int i = 0;
	while (memcmp (itemname, (char*)drsRects+i, strlen (itemname)) != 0)
		i++;
	return i;
}

void patchRect (const char* itemName, //name of the control, open 53290 in interfac.drs for details
	bool addX, //control needs to be moved horizontally
	bool addY, //control needs to be moved vertically
	bool addW, //control needs to be stretched horisontally
	bool addH, //control needs to be stretched vertically
	int* size) //returns the new size of the DRS item
{
	log ("Resolution: patching rect \"%s\", params: %d, %d, %d, %d",
		itemName, addX, addY, addW, addH);

	char* endbuf = (char*) malloc (4096);

	int X800; //see 53290 in interfac.drs to understand how this works
	int Y800;
	int W800;
	int H800;
	int X1024;
	int Y1024;
	int W1024;
	int H1024;
	int X1280;
	int Y1280;
	int W1280;
	int H1280;
	char scanArg[128];
	char oldRects [128];
	char newRects [128];
	sprintf (scanArg, "%s\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%d\t%%s", itemName);
	sscanf ((char*)drsRects+findRectOffset (itemName), scanArg,
		&X800,
		&Y800,
		&W800,
		&H800,
		&X1024,
		&Y1024,
		&W1024,
		&H1024,
		&X1280,
		&Y1280,
		&W1280,
		&H1280,
		endbuf);
	sprintf (oldRects, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t", //tab value tab value...
		X800,
		Y800,
		W800,
		H800,
		X1024,
		Y1024,
		W1024,
		H1024,
		X1280,
		Y1280,
		W1280,
		H1280);
	if (addX)
		if (newYsize >= 1024)
			X1280 += newXsize - 1280;
		else if (newYsize >= 768)
			X1024 += newXsize - 1024;
		else
		{
			exit (1);
		}
	if (addY)
		if (newYsize >= 1024)
			Y1280 += newYsize - 1024;
		else if (newYsize >= 768)
			Y1024 += newYsize - 768;
		else
		{
			exit (1);
		}
	if (addW)
		if (newYsize >= 1024)
			W1280 += newXsize - 1280;
		else if (newYsize >= 768)
			W1024 += newXsize - 1024;
		else
		{
			exit (1);
		}
	if (addH)
		if (newYsize >= 1024)
			H1280 += newYsize - 1024;
		else if (newYsize >= 768)
			H1024 += newYsize - 768;
		else
		{
			exit (1);
		}
	sprintf (newRects, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t",
		X800,
		Y800,
		W800,
		H800,
		X1024,
		Y1024,
		W1024,
		H1024,
		X1280,
		Y1280,
		W1280,
		H1280);

	int dst;
	int src;
	int n;

	if (strlen (oldRects) != strlen (newRects)) //adjust data...
	{
		dst = (int)drsRects+findRectOffset (itemName)+strlen (itemName)+strlen (newRects)-strlen (oldRects);
		src = (int)drsRects+findRectOffset (itemName)+strlen (itemName);
		n =  *size - findRectOffset (itemName)+strlen (itemName);
		memmove ((void*)dst, (void*)src, n);
		*size += strlen(newRects)-strlen(oldRects);
		memcpy ((void*)src, newRects, strlen(newRects));
	}
	else //don't adjust data...
	{
		dst = (int)drsRects+findRectOffset (itemName)+strlen (itemName);
		memcpy ((void*)dst, newRects, strlen (newRects));
	}

	free (endbuf);

	log ("Resolution: rect patching done");
}


void patchResRects (int X, int Y)
{
	int size;
	drsRects = readDRSItemToMem ("data\\interfac_x2.drs", 53290, &size);
	
	newXsize = X;
	newYsize = Y;

	patchRect ("ToolBox", 0, 0, 1, 0, &size);
	patchRect ("Main Viewport with toolbox", 0, 0, 1, 1, &size);
	patchRect ("Main Viewport w/o toolbox", 0, 0, 1, 1, &size);
	patchRect ("Inventory Panel", 1, 1, 0, 0, &size);
	patchRect ("Map View", 1, 1, 0, 0, &size);
	patchRect ("Age Panel", 0, 1, 1, 0, &size);
	patchRect ("Objectives Changed Text", 1, 0, 0, 0, &size);
	patchRect ("FPS Panel", 1, 1, 0, 0, &size);
	patchRect ("Object Panel", 0, 1, 1, 0, &size);
	patchRect ("Action Buttons", 0, 1, 0, 0, &size);
	patchRect ("Objectives Button", 1, 1, 0, 0, &size);
	patchRect ("Tech Tree Button", 1, 1, 0, 0, &size);
	patchRect ("Chat Button", 1, 1, 0, 0, &size);
	patchRect ("Diplomacy Button", 1, 1, 0, 0, &size);
	patchRect ("Menu Button", 1, 1, 0, 0, &size);
	patchRect ("Flare Button", 1, 1, 0, 0, &size);
	patchRect ("Idle Villager Button", 1, 1, 0, 0, &size);
	patchRect ("Score Button", 1, 1, 0, 0, &size);
	patchRect ("Popup Help Button", 1, 1, 0, 0, &size);
	patchRect ("Advanced Button", 1, 1, 0, 0, &size);
	patchRect ("Normal Map Button", 1, 1, 0, 0, &size);
	patchRect ("Combat Map Button", 1, 1, 0, 0, &size);
	patchRect ("Resource Map Button", 1, 1, 0, 0, &size);
	patchRect ("Toggle MiniMap Terrain", 1, 1, 0, 0, &size);

	writeDRS ("data\\wide.drs", 53290, drsRects, size);
	
	free (drsRects);
}

void placeSLP (int id, bool wide)
{
	int size;
	void* mem;
	if (wide)
		mem = readDRSItemToMem ("data\\widescrn_x2.drs", id, &size);
	else
		mem = readDRSItemToMem ("data\\interfac_x2.drs", id, &size);
	writeDRS ("data\\wide.drs", id, mem, size);
	free (mem);
}

void patchResolution (int x, int y)
{
	log ("Resolution patch started, %dx%d", x, y);
	int size;
	void* drsResStr = readDRSItemToMem ("data\\wide.drs", 100000, &size);
	if (drsResStr)
	{
		int x_;
		int y_;
		sscanf ((char*)drsResStr, "%dx%d", &x_, &y_);
		free (drsResStr);
		if ((x_ == x) && (y_ == y))
		{
			log ("Resolution: Patch already exists");
			return;
		}
	}
	newXsize = x;
	newYsize = y;

	log ("Resolution: creating new files...");
	createNewFiles (y);

	log ("Resolution: patching rects...");
	
	patchResRects (newXsize, newYsize);

	log ("Resolution: updating resolution id");
	char resStr [20];
	sprintf (resStr, "%dx%d", x, y);
	writeDRS ("data\\wide.drs", 100000, resStr, strlen(resStr)+1);

	log ("Resolution: stretching SLPs...");

//50032 - edit small
//50033 - edit medium
//50034 - edit large
//50100 - load small
//50101 - load medium
//50102 - load large

	if (newXsize >= 1920)
	{
		placeSLP (50032, 0);
		placeSLP (50033, 0);
		placeSLP (50034, 1);
		placeSLP (50100, 0);
		placeSLP (50101, 0);
		placeSLP (50102, 1);
	}
	else
	{
		placeSLP (50032, 0);
		placeSLP (50033, 0);
		placeSLP (50034, 0);
		placeSLP (50100, 0);
		placeSLP (50101, 0);
		placeSLP (50102, 0);
	}
	
	if (newYsize >= 1024)
	{
		parseSLP (newXsize, 51141, 0);
		parseSLP (newXsize, 51142, 0);
		parseSLP (newXsize, 51143, 0);
		parseSLP (newXsize, 51144, 0);
		parseSLP (newXsize, 51145, 0);
		parseSLP (newXsize, 51146, 0);
		
		parseSLP (newXsize, 51147, 0);
		parseSLP (newXsize, 51148, 0);

		parseSLP (newXsize, 51149, 0);
	}
	else if (newYsize >= 768)
	{
		parseSLP (newXsize, 51121, 0);
		parseSLP (newXsize, 51122, 0);
		parseSLP (newXsize, 51123, 0);
		parseSLP (newXsize, 51124, 0);
		parseSLP (newXsize, 51125, 0);
		parseSLP (newXsize, 51126, 0);

		parseSLP (newXsize, 51127, 0);
		parseSLP (newXsize, 51128, 0);

		parseSLP (newXsize, 51129, 0);
	}
	log ("Resolution patch successfull");
}

void resolutionTool (int x, int y)
{
	int size;
	void* drsResStr = readDRSItemToMem ("data\\wide.drs", 100000, &size);
	if (!drsResStr)
		return;

	int x_;
	int y_;
	sscanf ((char*)drsResStr, "%dx%d", &x_, &y_);
	free (drsResStr);
	if ((x_ != x) || (y_ != y))
		return;

	patchEXE (x, y);
}
