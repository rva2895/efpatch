#include "stdafx.h"
#include "casts.h"
#include <iostream>
#include <vector>
#include <algorithm>

#include "processunit.h"

//TODO:
//         1: process getDamage funcion [done?]
//         2: add attacked units to vector [done]
//         3: save/load [done]
//         4: remove deleted units from vector [done]
//         5: remove units with expired effects from map [deprecated]
//         6: detect effects (speed) [done?]
//         7: detect effects (stealth) [done]
//         8: detect effects (reload time)
//         9: detect effects (hp drain) [done]

int old_save_file_ver;

int anyEffectsActive(UNIT_EXTRA* ud)
{
	return (ud->speedReductionEnabled || ud->stealthOffEnabled ||
		ud->hpDrainEnabled || ud->hpDrainPercentEnabled || ud->reloadTimeEnabled);
}

bool nonZeroCounters(UNIT_EXTRA* ud)
{
	//return ((ud->miscCounter1 > 0) ||
	//	(ud->miscCounter2 > 0) ||
	//	(ud->miscCounter3 > 0) ||
	//	(ud->miscCounter4 > 0) ||
	//	(ud->miscCounter5 > 0));
	return ud->countersUsed;
}

void __stdcall processUnitExtra(UNIT* unit, int timerRes)
{
	UNIT_EXTRA* ud = getUnitExtra(unit);
	if (ud)
	{
		if (ud->speedReductionEnabled)
		{
			ud->speedReductionTime -= *(float*)&timerRes;
			if (ud->speedReductionTime <= 0)
			{
				ud->speedReductionTime = 0;
				ud->speedReductionEnabled = 0;
			}
		}
		if (ud->stealthOffEnabled)
		{
			ud->stealthOffTime -= *(float*)&timerRes;
			if (ud->stealthOffTime <= 0)
			{
				ud->stealthOffTime = 0;
				ud->stealthOffEnabled = 0;
			}
		}
		if (ud->reloadTimeEnabled)
		{
			ud->reloadTimeTime -= *(float*)&timerRes;
			if (ud->reloadTimeTime <= 0)
			{
				ud->reloadTimeTime = 0;
				ud->reloadTimeEnabled = 0;
			}
		}
		if (ud->hpDrainEnabled)
		{
			ud->hpDrainTime -= *(float*)&timerRes;
			if (ud->hpDrainTime <= 0)
			{
				ud->hpDrainTime = 0;
				ud->hpDrainEnabled = 0;
			}
			else
			{
				float maxHP = unit->prop_object->hit_points;
				float* hp = &unit->hp;
				ud->hpDrainLeftover += *(float*)&timerRes * ud->hpDrainPerSecond;
				float intp;
				modf(ud->hpDrainLeftover, &intp);
				if (abs(intp) > 0.0)
				{
					ud->hpDrainLeftover -= intp;
					*hp -= intp;
					if (*hp > maxHP)
						*hp = maxHP;
				}
			}
		}
		if (ud->hpDrainPercentEnabled)
		{
			ud->hpDrainPercentTime -= *(float*)&timerRes;
			if (ud->hpDrainPercentTime <= 0)
			{
				ud->hpDrainPercentTime = 0;
				ud->hpDrainPercentEnabled = 0;
			}
			else
			{
				float maxHP = unit->prop_object->hit_points;
				float* hp = &unit->hp;
				ud->hpDrainPercentLeftover +=
					*(float*)&timerRes * ud->hpDrainPercentPerSecond*maxHP;
				float intp;
				modf(ud->hpDrainPercentLeftover, &intp);
				if (abs(intp) > 0.0)
				{
					ud->hpDrainPercentLeftover -= intp;
					*hp -= intp;
					if (*hp > maxHP)
						*hp = maxHP;
				}
			}
		}
		
		if (!anyEffectsActive(ud))
		{
			if (!nonZeroCounters(ud))
			{
				removeUnitExtra(unit);
#ifdef _DEBUG
				log("Removed expired unit 0x%X", unit);
#endif
			}
#ifdef _DEBUG
			else
				log("Unit 0x%X expired, but there are non-zero counters", unit);
#endif
		}
	}
}

__declspec(naked) void processUnitHook() //00444DA0, 0054EF00
{
	__asm
	{
		//mov     eax, [ecx+18h]
		//mov     eax, [eax+8Ch]
		//mov     eax, dword ptr [eax+0A8h]
		//push    esi
		//mov     esi, ecx
		//push    eax
		//push    ecx
		//call    processUnitExtra
		//sub     esp, 8
		//push    00444DA6h
		//ret

		push    ecx
		push    ebx
		xor     ebx, ebx
		mov     eax, [ecx + 18h]
		mov     eax, [eax + 8Ch]
		mov     eax, dword ptr [eax + 0A8h]
		push    esi
		mov     esi, ecx
		push    eax
		push    ecx
		call    processUnitExtra
		mov     eax, 0054EF07h
		jmp     eax
	}
}

int __stdcall getSpeedModifier(void* unit) //returns 32 bit float
{
	int modifier = 0x3F800000; //1.0f
	UNIT_EXTRA* ud = getUnitExtra(unit);
	if (ud)
	{
		if (ud->speedReductionEnabled)
			modifier = *(int*)&(ud->speedReductionModifier);
	}

	return modifier;
}

int __stdcall getStealthOff(void* unit) //1 - stealth off enabled
{
	int status = 0;
	UNIT_EXTRA* ud = getUnitExtra(unit);
	if (ud)
	{
		if (ud->stealthOffEnabled)
			status = 1;
	}

	return status;
}

void* targetUnit;
int isDamage;

__declspec(naked) void getDamageGetUnit() //004449C3
{
	__asm
	{
		mov     edx, [esp + 10h]
		cmp     edx, 00444847h
		setz    dl
		movsx   edx, dl
		mov     isDamage, edx
		mov     targetUnit, ecx
		mov     edx, [ecx + 14h]
		mov     eax, [ecx + 28h]
		push    ebx
		push    ebp
		mov     ebp, 004449CBh
		jmp     ebp
	}
}

void __stdcall specialDamage(void* unit, short type, int damage, int armor)
{
	if (!unit)
	{
		if (!isDamage)
			return;
		unit = targetUnit;
	}

	if (type == 1000)      //death star fix
		return;

	void* propObj = *(void**)((int)unit + 0x14);
	if (*(unsigned char*)((int)propObj + 4) < 70) //type 70+
		return;

#ifdef _DEBUG
	int ltype = type;
	log("---SPECIAL DAMAGE---: type %d, dmg %d, armor %d", ltype, damage, armor);
#endif

	UNIT_EXTRA* ud;

	ud = getUnitExtra(unit);
	if (!ud)
	{
		ud = new UNIT_EXTRA;
		memset(ud, 0, sizeof(UNIT_EXTRA));
		addUnitExtra(unit, ud);
#ifdef _DEBUG
		log("Created new UNIT_EXTRA for unit %d", unit);
#endif
	}
#ifdef _DEBUG
	else
		log("Loaded existing UNIT_EXTRA for unit %d", unit);
#endif

	if (armor == 1000)
		armor = 0;

	switch (type)
	{
	case 41:        //speed reduction, value
		ud->speedReductionEnabled = 1;
		ud->speedReductionModifier = 1.0f - (damage * (100.0f - (float)armor) / 100.0f) / 100.0f;
		break;
	case 42:
		ud->speedReductionEnabled = 1;
		ud->speedReductionTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
		break;
	case 43:
		ud->stealthOffEnabled = 1;
		break;
	case 44:
		ud->stealthOffTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
		break;
	case 45:
		//ud->stealthOffEnabled = 1;
		break;
	case 46:
		//ud->stealthOffTime = (float)(damage-armor) / 1000;
		break;
	case 47:
		ud->hpDrainEnabled = 1;
		ud->hpDrainPerSecond = (damage * (100.0f - (float)armor) / 100.0f) / 60.0f;
		ud->hpDrainLeftover = 0;
		break;
	case 48:
		ud->hpDrainTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
		break;
	case 49:
		ud->hpDrainPercentEnabled = 1;
		ud->hpDrainPercentPerSecond = (damage * (100.0f - (float)armor) / 100.0f) / 6000.0f;
		ud->hpDrainPercentLeftover = 0;
		break;
	case 50:
		ud->hpDrainPercentTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
		break;
	default:
		break;
	}
}

__declspec(naked) void onGetDamage() //00444A7C
{
	__asm
	{
		cmp     dx, 41
		jl      _end
		movsx   eax, word ptr [esi + 2]   //damage
		mov     ecx, [esp + 28h]          //armor
		push    0
		fst     [esp]
		push    edx                       //save edx
		push    ecx                       //armor
		push    eax                       //damage
		push    edx                       //type
		push    0                         //unit - 0 means use targetUnit
		call    specialDamage
		pop     edx
		fld     [esp]
		fstp    st
		add     esp, 4
		mov     eax, 00444AA9h
		jmp     eax
_end:
		movsx   eax, word ptr [esi + 2]
		mov     [esp + 1Ch], eax
		mov     eax, 00444A84h
		jmp     eax
	}
}

__declspec(naked) void speed1() //0044B7E2
{
	__asm
	{
		push    eax
		push    ecx
		push    edx
		push    esi
		call    getSpeedModifier
		sub     esp, 4
		mov     [esp], eax
		fld     dword ptr[esp]
		mov     edx, [esp + 4]
		mov     ecx, [esp + 8]
		mov     eax, [esp + 0Ch]
		fld     dword ptr [eax + 0D0h]
		fmul    st(1), st
		fstp    st
		add     esp, 10h
		mov     eax, 0044B7E8h
		jmp     eax
	}
}

__declspec(naked) void speed2() //004A28C0
{
	__asm
	{
		push    ecx
		push    ecx
		call    getSpeedModifier
		mov     ecx, [esp]
		mov     [esp], eax
		//fld     dword ptr [esp]
		//add     esp, 4

		mov     edx, [ecx + 1Ch]
		test    edx, edx
		jz      short loc_4A28E4
		fld     dword ptr [edx + 74h]
		_emit   0xDC//fcomp   ds:dbl_6546B0
		_emit   0x1D
		_emit   0xB0
		_emit   0x46
		_emit   0x65
		_emit   0x00
		fnstsw  ax
		test    ah, 41h
		jnz     short loc_4A28E4
		mov     eax, [ecx + 14h]
		fld     dword ptr [esp]
		fld     dword ptr [eax + 0D0h]
		fmul    st(1), st
		fstp    st
		add     esp, 4
		fmul    dword ptr [edx + 74h]
		retn
		//.text:004A28E4 ; ---------------------------------------------------------------------------
		//.text:004A28E4
loc_4A28E4:
		mov     ecx, [ecx + 14h]
		fld     dword ptr [esp]
		fld     dword ptr [ecx + 0D0h]
		fmul    st(1), st
		fstp    st
		add     esp, 4
		retn
	}
}

__declspec(naked) void speed3() //004A3EAD
{
	__asm
	{
		push    esi
		call    getSpeedModifier
		mov     [esp - 4], eax
		//fld     dword ptr [esp]
		//add     esp, 4

		mov     ecx, [esi + 1Ch]
		test    ecx, ecx
		jz      short loc_4A3ED2
		fld     dword ptr [ecx + 74h]
		_emit   0xDC//fcomp   ds:dbl_6546B0
		_emit   0x1D
		_emit   0xB0
		_emit   0x46
		_emit   0x65
		_emit   0x00
		fnstsw  ax
		test    ah, 41h
		jnz     short loc_4A3ED2
		mov     eax, [esi + 14h]
		fld     dword ptr [esp - 4]
		fld     dword ptr [eax + 0D0h]
		fmul    st(1), st
		fstp    st
		fmul    dword ptr [ecx + 74h]
		jmp     __end
		//.text:004A28E4 ; ---------------------------------------------------------------------------
		//.text:004A28E4
loc_4A3ED2:
		mov     ecx, [esi + 14h]
		fld     dword ptr [esp - 4]
		fld     dword ptr [ecx + 0D0h]
		fmul    st(1), st
		fstp    st
__end:
		mov     eax, 004A3EDBh
		jmp     eax
	}
}

__declspec(naked) void speed4() //004A6250
{
	__asm
	{
		push    ecx
		push    ecx
		call    getSpeedModifier
		mov     ecx, [esp]
		mov     [esp], eax
		//fld     dword ptr [esp]
		//add     esp, 4

		mov     edx, [ecx + 1Ch]
		test    edx, edx
		jz      short loc_4A6276
		fld     dword ptr [edx + 74h]
		_emit   0xDC//fcomp   ds:dbl_6546B0
		_emit   0x1D
		_emit   0xB0
		_emit   0x46
		_emit   0x65
		_emit   0x00
		fnstsw  ax
		test    ah, 41h
		jnz     short loc_4A6276
		mov     eax, [ecx + 14h]
		fld     dword ptr [esp]
		fld     dword ptr [eax + 0D0h]
		fmul    st(1), st
		fstp    st
		add     esp, 4
		fmul    dword ptr [edx + 74h]
		retn    4
		//---------------------------------------------------------------------------
loc_4A6276:
		mov     ecx, [ecx + 14h]
		fld     dword ptr [esp]
		fld     dword ptr [ecx + 0D0h]
		fmul    st(1), st
		fstp    st
		add     esp, 4
		retn    4
	}
}

__declspec(naked) void stealth1() //0054BB42
{
	__asm
	{
		push    esi
		call    getStealthOff
		test    eax, eax
		jnz     _stOff
		mov     eax, [esi + 14h]
		test    byte ptr [eax + 0ACh], 4
		mov     ecx, 0054BB4Ch
		jmp     ecx
_stOff:
		pop     edi
		pop     esi
		xor     al, al
		pop     ebx
		retn    4
	}
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall writeSaveFile(int id, void* buffer, int size)
{
	__asm
	{
		mov     ecx, [esp + 4]
		mov     edx, [esp + 8]
		mov     eax, [esp + 0Ch]
		push    eax
		mov     eax, 004D5790h
		call    eax
		retn    0Ch
	}
}

__declspec(naked) void __stdcall readSaveFile(int id, void* buffer, int size)
{
	__asm
	{
		mov     ecx, [esp + 4]
		mov     edx, [esp + 8]
		mov     eax, [esp + 0Ch]
		push    eax
		mov     eax, 004D5550h
		call    eax
		retn    0Ch
	}
}

void __stdcall readUnitExtra(void* unit, int id)
{
	char flag = 0;
	UNIT_EXTRA* ud;

	if (!old_save_file_ver)
	{
		readSaveFile(id, &flag, 1);
		if (flag)
		{
			ud = new UNIT_EXTRA;
			readSaveFile(id, ud, sizeof(UNIT_EXTRA));
			
			addUnitExtra(unit, ud);
#ifdef _DEBUG
			log("Loaded unit extra for unit 0x%X from save", unit);
#endif
		}
	}
}

void __stdcall writeUnitExtra(void* unit, int id)
{
	char flag = 0;

	UNIT_EXTRA* ud = getUnitExtra(unit);
	if (ud)
	{
		flag = 1;
		writeSaveFile(id, &flag, 1);

		writeSaveFile(id, ud, sizeof(UNIT_EXTRA));
		
#ifdef _DEBUG
		log("Saved unit extra for unit %d", unit);
#endif
	}
	else
	{
		flag = 0;
		writeSaveFile(id, &flag, 1);
	}
}

__declspec(naked) void writeSaveHook() //004AF323
{
	__asm
	{
		mov     ecx, [eax + edi * 4]
		push    ebx
		push    ecx
		push    ebx
		mov     edx, [ecx]
		call    dword ptr [edx + 0A0h]
		call    writeUnitExtra
		mov     eax, 004AF32Fh
		jmp     eax
	}
}

__declspec(naked) void readSaveHook() //004AEEEE
{
	__asm
	{
		mov     ecx, esi
		call    dword ptr [edx]
		push    edi
		push    eax
		call    readUnitExtra
		mov     ecx, 004AEED4h
		jmp     ecx
	}
}

int __stdcall strcmp_wr(char* s1, char* s2)
{
	return strcmp(s1, s2);
}

const char oldVer[] = "VER 9.4";
const char newVer[] = "VER 9.6";

__declspec(naked) void verLoadHook() //0061D9A5
{
	__asm
	{
		lea     esi, [esp + 18h]
		mov     ecx, offset oldVer
		push    ecx
		push    esi
		call    strcmp_wr
		test    eax, eax
		jnz     ver_cont
		mov     eax, 1
		mov     old_save_file_ver, eax
		mov     eax, 0061D9F1h
		jmp     eax
ver_cont:
		mov     ecx, offset newVer
		push    ecx
		push    esi
		call    strcmp_wr
		test    eax, eax
		jnz     bad_ver
		mov     old_save_file_ver, eax
		mov     eax, 0061D9F1h
		jmp     eax
bad_ver:
		mov     eax, 0061D9DBh
		jmp     eax
	}
}

__declspec(naked) void verSaveHook() //00620583
{
	__asm
	{
		push    8
		push    offset newVer
		push    esi
		call    writeSaveFile
		mov     ecx, 00620591h
		jmp     ecx
	}
}

#ifdef _DEBUG
void __stdcall extraLog(UNIT_EXTRA* e, void* unit)
{
	log("REMOVING EXTRA: 0x%X, unit 0x%X", e, unit);
	if ((int)e % 4)
		log("=== WARNING: NOT ALIGNED!!!");
}
#endif

__declspec(naked) void __fastcall removeUnitExtra(void* unit)
{
	__asm
	{
		mov     eax, [ecx + 78h]
		and     eax, 0FFFFFF00h
		mov     al, [ecx + 87h]
		test    eax, eax
		jz      _noFree
		push    ecx
		push    eax
		//
#ifdef _DEBUG
		push    ecx
		push    eax
		call    extraLog
#endif
		//
		call    ds:[free]
		add     esp, 4
		pop     ecx
		and     dword ptr [ecx + 78h], 0FFh
		and     byte ptr [ecx + 87h], 0
_noFree:
		ret
	}
}

__declspec(naked) void destructorHook() //00408D20, 00550730
{
	__asm
	{
		//push    ecx
		//call    removeUnitExtra
		//pop     ecx
		//push    0FFFFFFFFh
		//push    00640A78h
		//push    00408D27h
		//ret

		sub     esp, 28h
		push    esi
		mov     esi, ecx
		call    removeUnitExtra
		mov     eax, 00550736h
		jmp     eax
	}
}

__declspec(naked) void constructorHook() //0054B312
{
	__asm
	{
		mov     [esi + 78h], eax
		mov     [esi + 7Ch], eax
		mov     [esi + 84h], eax
		push    0054B327h
		ret
	}
}

__declspec(naked) void saveReadFix() //0054B635
{
	__asm
	{
		mov     [esi + 78h], eax
		mov     [esi + 7Ch], eax
		mov     [esi + 84h], eax
		push    0054B64Ah
		ret
	}
}

__declspec(naked) UNIT_EXTRA* __fastcall getUnitExtra(void* unit)
{
	__asm
	{
		mov     eax, [ecx + 78h]
		and     eax, 0FFFFFF00h
		mov     al, [ecx + 87h]
		ret
	}
}

__declspec(naked) void __fastcall addUnitExtra(void* unit, UNIT_EXTRA* ud)
{
	__asm
	{
		mov     eax, edx
		and     edx, 0FFFFFF00h   //bytes 1,2,3
		and     eax, 0FFh         //byte 0
		add     ecx, 78h
		or      dword ptr [ecx], edx
		add     ecx, 0Fh
		mov     [ecx], al
		ret
	}
}
#pragma warning(pop)

int cookie;
int retAddr_;
int testAddr;

__declspec(naked) void readDatTest1 ()
{
	__asm
	{
		mov     cookie, edx
		mov     eax, [esp]
		mov     retAddr_, eax
		mov     eax, testAddr
		mov     [esp], eax

		push    ebp
		mov     ebp, esp
		sub     esp, 0Ch
		push    004D5556h
		ret
	}
}
__declspec(naked) void readDatTest2 ()
{
	__asm
	{
		mov     ecx, cookie
		mov     ecx, [ecx]
		cmp     ecx, 41CA97D7h
		jnz     _good
		int     3
_good:
		mov     ecx, retAddr_
		jmp     ecx
	}
}

void setCastHooks()
{
	//testAddr = (int)&readDatTest2;
	//setHook ((void*)0x004D5550, readDatTest1);
	//

	setHook((void*)0x00444A7C, onGetDamage);
	setHook((void*)0x004449C3, getDamageGetUnit);

	setHook((void*)0x0044B7E2, speed1);
	setHook((void*)0x004A28C0, speed2);
	setHook((void*)0x004A3EAD, speed3);
	setHook((void*)0x004A6250, speed4);

	setHook((void*)0x0054BB42, stealth1);

	setHook((void*)0x004AF323, writeSaveHook);
	setHook((void*)0x004AEEEE, readSaveHook);
	setHook((void*)0x0061D9A5, verLoadHook);
	setHook((void*)0x00620583, verSaveHook);

	setHook((void*)0x00550730, destructorHook);
	setHook((void*)0x0054B312, constructorHook);
	setHook((void*)0x0054B635, saveReadFix);

	setHook((void*)0x0054EF00, processUnitHook);

	//setHook ((void*)0x0058A170, oldNullsub);
}
