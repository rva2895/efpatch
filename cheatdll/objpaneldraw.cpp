#include "stdafx.h"

#include "objpanel.h"
#include "resgenbldgs.h"

extern int itemCounter;
extern char* is2ndCol;

extern int numberOfResProducers;
extern resGen* resProducersData;

extern int currentPlayer;
extern float* resources;

extern void* objPanelPtr;

extern int(__thiscall* objPanelDrawItem) (void*, int, int, int, int, int, int, int, int);

void __cdecl objPanel(void* unit)
{
	//50731 - SLP in interfac.drs
	//if (unit)
	//	objPanelDrawItem (5, 2, 0, 1, 1, 1, 1, 42013);

	if (!currentPlayer)
		return;

	memset(is2ndCol, 0, 0x10);
	//is2ndCol [itemCounter] = 1;
	//is2ndCol [itemCounter+1] = 1;
	int cntr = 4;
	int localItemCounter = itemCounter;
	int val;

	void* propObj = *(void**)((int)unit + 0x14);
	short id = *(short*)((int)propObj + 0x18);

	int worker = 0;
	int frame;
	int langID;

	//RESGEN

	float res;

	for (int i = 0; i < numberOfResProducers; i++)
	{
		if (id == resProducersData[i].unitID)
		{
			if (resProducersData[i].useControlRes)
				res = resources[resProducersData[i].controlResID];
			else
				res = ((float)resProducersData[i].constantResAmount);

			if (resProducersData[i].resID == 0)       //food
			{
				frame = 2;
				langID = 42045;
			}
			else if (resProducersData[i].resID == 1)  //carbon
			{
				frame = 0;
				langID = 42046;
			}
			else if (resProducersData[i].resID == 2)  //ore
			{
				frame = 1;
				langID = 42047;
			}
			else if (resProducersData[i].resID == 3)  //nova
			{
				frame = 3;
				langID = 42048;
			}
			else                                      //unknown
			{
				frame = 5;
				langID = 42049;
			}

			val = res * 100;
			objPanelDrawItem(objPanelPtr, localItemCounter++, frame, 6, val, 0, 0, 0, langID);
		}
	}

	/*switch (id) //workers
	{
	case 56: //fisher
	case 57:
		frame = 1;
		langID = 1;
		worker = 1;
	case 120: //forager
	case 354:
		frame = 1;
		langID = 1;
		worker = 1;
	case 122: //hunter
	case 216:
		frame = 1;
		langID = 1;
		worker = 1;
	case 123: //carbon collector
	case 218:
		frame = 1;
		langID = 1;
		worker = 1;
	case 124: //ore gatherer
	case 220:
		frame = 1;
		langID = 1;
		worker = 1;
	case 214: //farmer
	case 259:
		frame = 1;
		langID = 1;
		worker = 1;
	case 579: //nova gatherer
	case 581:
		frame = 1;
		langID = 1;
		worker = 1;
	case 590: //herder
	case 592:
		frame = 1;
		langID = 1;
		worker = 1;
	}

	if (worker)
	{
		workrate = *(float*)((int)propObj+0x110);
		val = workrate * 100;
		objPanelDrawItem (localItemCounter++, frame, 6, val, 0, 0, 0, langID);
	}*/

	if (*(unsigned char*)((int)propObj + 4) >= 70)
	{
		float speed = *(float*)((int)propObj + 0xD0);
		if (speed > 0.0f)
		{
			val = speed * 100;
			is2ndCol[localItemCounter++] = 1;
			objPanelDrawItem(objPanelPtr, cntr++, 24, 6, val, 0, 0, 0, 42042);
		}

		float reloadtime = *(float*)((int)propObj + 0x150);
		short proj = *(short*)((int)propObj + 0x154);
		short cls = *(short*)((int)propObj + 0x01E);
		if ((reloadtime > 0.0f) && ((proj != -1) || (cls != 18)))
		{
			val = reloadtime * 100;
			is2ndCol[localItemCounter++] = 1;
			objPanelDrawItem(objPanelPtr, cntr++, 30, 6, val, 0, 0, 0, 42043);
		}

		float blastr = *(float*)((int)propObj + 0x148);
		if (blastr > 0.0f)
		{
			val = blastr * 100;
			is2ndCol[localItemCounter++] = 1;
			objPanelDrawItem(objPanelPtr, cntr++, 28, 6, val, 0, 0, 0, 42044);
		}
	}
}
