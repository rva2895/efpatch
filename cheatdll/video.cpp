#include "stdafx.h"

#include "video.h"

__declspec(naked) void on_f1()
{
	__asm
	{

	}
}

void setVideoHooks()
{
	//
	HMODULE m = LoadLibrary("dxmci.dll");
	DWORD d = (DWORD)GetProcAddress(m, "MCIWndCreateA2");

	writeDword(0x005E832A, d - 0x005E832E);
	//((HWND(__cdecl*) (HWND, HINSTANCE, DWORD, LPCSTR)) (d)) (0, 0, 0,
	//	"avi\\test.mp4");
	//MessageBox(0, "OK", "OK", 0);
	writeByte(0x005F0C28, 32);
	writeByte(0x005F0C10, 32);
	/*
#define V_X 640
#define V_Y 480

	writeDword(0x005F0C2F, V_X);
	writeDword(0x005F0C2A, V_Y);
	writeDword(0x005F0C17, V_X);
	writeDword(0x005F0C12, V_Y);

	//writeDword(0x005F0B50, 0x000001B8);
	//writeWord(0x005F0B54, 0xC300);
	//

	writeDword(0x005F0C33, 0x000001B8);
	writeByte(0x005F0C37, 0x00);
	writeDword(0x005F0C27, 0x90909090);
	writeDword(0x005F0C2B, 0x90909090);
	writeDword(0x005F0C2F, 0x90909090);

	writeDword(0x005F0C83, 0x000001B8);
	writeByte(0x005F0C87, 0x00);
	writeDword(0x005F0C77, 0x90909090);
	writeDword(0x005F0C7B, 0x90909090);
	writeDword(0x005F0C7F, 0x90909090);

	writeDword(0x005F0CD8, 0x000001B8);
	writeByte(0x005F0CDC, 0x00);
	writeDword(0x005F0CCB, 0x90909090);
	writeDword(0x005F0CCF, 0x90909090);
	writeDword(0x005F0CD3, 0x90909090);

	writeDword(0x005F0D26, 0x000001B8);
	writeByte(0x005F0D2A, 0x00);
	writeDword(0x005F0D1A, 0x90909090);
	writeDword(0x005F0D1E, 0x90909090);
	writeDword(0x005F0D22, 0x90909090);

	writeDword(0x005F0D6C, 0x000001B8);
	writeByte(0x005F0D70, 0x00);
	writeDword(0x005F0D60, 0x90909090);
	writeDword(0x005F0D64, 0x90909090);
	writeDword(0x005F0D68, 0x90909090);*/
}
