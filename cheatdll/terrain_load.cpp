#include "stdafx.h"

#include "terrain.h"

int t_ver;

void (__thiscall* terrain1_load) (void* this_, int language_dll_id, int terrain) =
	(void(__thiscall*) (void*, int, int))0x004C82D0;

void __stdcall terrain1_(void* this_)
{
	terrain1_load(this_, 0x297D, 0);
	terrain1_load(this_, 0x2980, 1);
	terrain1_load(this_, 10926, 2);
	terrain1_load(this_, 0x2993, 3);
	terrain1_load(this_, 0x2983, 4);
	terrain1_load(this_, 0x299A, 5);
	terrain1_load(this_, 10622, 6);
	terrain1_load(this_, 10920, 7);
	terrain1_load(this_, 10921, 8);
	terrain1_load(this_, 0x2991, 9);
	terrain1_load(this_, 0x297F, 10);
	terrain1_load(this_, 0x2992, 11);
	terrain1_load(this_, 0x2990, 12);
	terrain1_load(this_, 0x2981, 13);
	terrain1_load(this_, 0x2998, 14);
	terrain1_load(this_, 0x2A2D, 15);
	terrain1_load(this_, 0x2A2E, 16);
	terrain1_load(this_, 0x29A8, 17);
	terrain1_load(this_, 0x2999, 18);
	terrain1_load(this_, 0x2984, 19);
	terrain1_load(this_, 0x2982, 20);
	terrain1_load(this_, 0x29A9, 21);
	terrain1_load(this_, 0x2985, 22);
	terrain1_load(this_, 0x2994, 23);
	terrain1_load(this_, 0x2995, 24);
	terrain1_load(this_, 0x2996, 25);
	terrain1_load(this_, 0x299B, 26);
	terrain1_load(this_, 10927, 27);
	terrain1_load(this_, 10929, 28);
	terrain1_load(this_, 10922, 29);
	terrain1_load(this_, 10925, 30);
	terrain1_load(this_, 10924, 31);
	terrain1_load(this_, 0x29AA, 32);
	terrain1_load(this_, 10668, 33);
	terrain1_load(this_, 0x29AB, 34);
	terrain1_load(this_, 0x29AD, 35);
	terrain1_load(this_, 10928, 36);
	terrain1_load(this_, 10930, 37);
	terrain1_load(this_, 0x29B5, 38);
	terrain1_load(this_, 0x29B6, 39);
	terrain1_load(this_, 10931, 40);
	terrain1_load(this_, 10790, 41);
	terrain1_load(this_, 0x2A27, 42);
	terrain1_load(this_, 0x2A28, 43);
	terrain1_load(this_, 0x2A29, 44);
	terrain1_load(this_, 0x2A2A, 45);
	terrain1_load(this_, 0x2A2B, 46);
	terrain1_load(this_, 0x2A2C, 47);
	terrain1_load(this_, 10923, 48);
	terrain1_load(this_, 0x2A2F, 49);
	terrain1_load(this_, 0x2A30, 50);
	terrain1_load(this_, 0x29B7, 51);
	terrain1_load(this_, 0x29B8, 52);
	//terrain1_load(this_, 10931, 53);
	//terrain1_load(this_, 10931, 54);
	//terrain1_load(this_, 1, 55);
	//terrain1_load(this_, 2, 56);
	if (t_ver == VER_EF)
	{
		terrain1_load(this_, 2707, 55);
		terrain1_load(this_, 2724, 56);
		terrain1_load(this_, 2708, 57);
		terrain1_load(this_, 2709, 58);
		terrain1_load(this_, 2710, 59);
		terrain1_load(this_, 2711, 60);
		terrain1_load(this_, 2712, 61);
		terrain1_load(this_, 2713, 62);
		terrain1_load(this_, 2714, 63);
		terrain1_load(this_, 2715, 64);
		terrain1_load(this_, 2716, 65);
		terrain1_load(this_, 2717, 66);
		terrain1_load(this_, 2718, 67);
		terrain1_load(this_, 2719, 68);
		terrain1_load(this_, 2720, 69);
		terrain1_load(this_, 2721, 70);
		terrain1_load(this_, 2723, 71);
		terrain1_load(this_, 2722, 72);

		//
		terrain1_load(this_, 2725, 93);
		terrain1_load(this_, 2726, 94);
		terrain1_load(this_, 2727, 95);
		terrain1_load(this_, 2728, 96);
		terrain1_load(this_, 2729, 97);
		terrain1_load(this_, 2730, 98);
		terrain1_load(this_, 2731, 99);
		terrain1_load(this_, 2732, 100);
		terrain1_load(this_, 2733, 101);
		terrain1_load(this_, 2734, 102);
		terrain1_load(this_, 2735, 103);
		terrain1_load(this_, 2736, 54);

		//trees:
		terrain1_load(this_, 22516, 73);
		terrain1_load(this_, 23330, 74);
		terrain1_load(this_, 23331, 75);
		terrain1_load(this_, 214, 76);
		terrain1_load(this_, 215, 77);
		terrain1_load(this_, 22587, 78);
		terrain1_load(this_, 23046, 79);
		terrain1_load(this_, 219, 80);
		terrain1_load(this_, 22515, 81);
		terrain1_load(this_, 213, 82);
		terrain1_load(this_, 217, 83);
		terrain1_load(this_, 216, 84);
		terrain1_load(this_, 23731, 85);
		terrain1_load(this_, 218, 86);
		//
		terrain1_load(this_, 22739, 87);
		terrain1_load(this_, 22741, 88);
		terrain1_load(this_, 22736, 89);
		terrain1_load(this_, 827, 90);
		terrain1_load(this_, 828, 92);
	}
}

__declspec(naked) int terrain1 ()
{
	__asm
	{
		mov     ecx, [edi]
		push    ecx
		call    terrain1_
		mov     ecx, 0052A388h
		jmp     ecx
	}
}

void (__thiscall* terrain2_load) (void* this_, int language_dll_id, int terrain) =
	(void(__thiscall*) (void*, int, int))0x0053AB20;

void __stdcall terrain2_(void* this_)
{
	terrain2_load(this_, 10926, 2);
	terrain2_load(this_, 0x2993, 3);
	terrain2_load(this_, 0x299A, 5);
	terrain2_load(this_, 10920, 7);
	terrain2_load(this_, 10921, 8);
	terrain2_load(this_, 0x2992, 0xB);
	terrain2_load(this_, 0x2998, 0xE);
	terrain2_load(this_, 0x2A2D, 0xF);
	terrain2_load(this_, 0x2A2E, 0x10);
	terrain2_load(this_, 0x29A8, 0x11);
	terrain2_load(this_, 0x2999, 0x12);
	terrain2_load(this_, 0x29A9, 0x15);
	terrain2_load(this_, 0x2996, 0x19);
	terrain2_load(this_, 0x299B, 0x1A);
	terrain2_load(this_, 10927, 0x1B);
	terrain2_load(this_, 10929, 0x1C);
	terrain2_load(this_, 10922, 0x1D);
	terrain2_load(this_, 10925, 0x1E);
	terrain2_load(this_, 10924, 0x1F);
	terrain2_load(this_, 0x29AA, 0x20);
	terrain2_load(this_, 10668, 0x21);
	terrain2_load(this_, 0x29AB, 0x22);
	terrain2_load(this_, 0x29AD, 0x23);
	terrain2_load(this_, 10928, 0x24);
	terrain2_load(this_, 10930, 0x25);
	terrain2_load(this_, 0x29B5, 0x26);
	terrain2_load(this_, 0x29B6, 0x27);
	terrain2_load(this_, 10931, 0x28);
	terrain2_load(this_, 10790, 0x29);
	terrain2_load(this_, 0x2A27, 0x2A);
	terrain2_load(this_, 0x2A28, 0x2B);
	terrain2_load(this_, 0x2A29, 0x2C);
	terrain2_load(this_, 0x2A2A, 0x2D);
	terrain2_load(this_, 0x2A2B, 0x2E);
	terrain2_load(this_, 0x2A2C, 0x2F);
	terrain2_load(this_, 10923, 0x30);
	terrain2_load(this_, 0x2A2F, 0x31);
	terrain2_load(this_, 0x2A30, 0x32);
	terrain2_load(this_, 0x29B7, 0x33);
	terrain2_load(this_, 0x29B8, 0x34);

	if (t_ver == VER_EF)	//EF terrains
	{
		terrain2_load(this_, 2707, 55);
		terrain2_load(this_, 2724, 56);
		terrain2_load(this_, 2708, 57);
		terrain2_load(this_, 2709, 58);
		terrain2_load(this_, 2710, 59);
		terrain2_load(this_, 2711, 60);
		terrain2_load(this_, 2712, 61);
		terrain2_load(this_, 2713, 62);
		terrain2_load(this_, 2714, 63);
		terrain2_load(this_, 2715, 64);
		terrain2_load(this_, 2716, 65);
		terrain2_load(this_, 2717, 66);
		terrain2_load(this_, 2718, 67);
		terrain2_load(this_, 2719, 68);
		terrain2_load(this_, 2720, 69);
		terrain2_load(this_, 2721, 70);
		terrain2_load(this_, 2723, 71);
		terrain2_load(this_, 2722, 72);
		//
		terrain2_load(this_, 2725, 93);
		terrain2_load(this_, 2726, 94);
		terrain2_load(this_, 2727, 95);
		terrain2_load(this_, 2728, 96);
		terrain2_load(this_, 2729, 97);
		terrain2_load(this_, 2730, 98);
		terrain2_load(this_, 2731, 99);
		terrain2_load(this_, 2732, 100);
		terrain2_load(this_, 2733, 101);
		terrain2_load(this_, 2734, 102);
		terrain2_load(this_, 2735, 103);
		terrain2_load(this_, 2736, 54);

		//trees:
		terrain2_load(this_, 22516, 73);
		terrain2_load(this_, 23330, 74);
		terrain2_load(this_, 23331, 75);
		terrain2_load(this_, 214, 76);
		terrain2_load(this_, 215, 77);
		terrain2_load(this_, 22587, 78);
		terrain2_load(this_, 23046, 79);
		terrain2_load(this_, 219, 80);
		terrain2_load(this_, 22515, 81);
		terrain2_load(this_, 213, 82);
		terrain2_load(this_, 217, 83);
		terrain2_load(this_, 216, 84);
		terrain2_load(this_, 23731, 85);
		terrain2_load(this_, 218, 86);
		//
		terrain2_load(this_, 22739, 87);
		terrain2_load(this_, 22741, 88);
		terrain2_load(this_, 22736, 89);
		terrain2_load(this_, 827, 90);
		terrain2_load(this_, 828, 92);
	}
}

__declspec(naked) int terrain2()
{
	__asm
	{
		mov     ecx, ebx
		push    ecx
		call    terrain2_
		mov     ecx, 0053B53Bh
		jmp     ecx
	}
}

void setTerrainLoadHooks(int ver)
{
	t_ver = ver;

	setHook((void*)0x0052A30A, terrain1);
	setHook((void*)0x0053B3C1, terrain2);
}
