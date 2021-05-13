#include "stdafx.h"

#include <time.h>
#include "filenames.h"

#define __REC_FILENAME__ "recorded game - %Y-%m-%d_%H-%M-%S.mg1"

char recCC[sizeof(__REC_FILENAME__)];
char timeOnly[] = "%Y-%m-%d_%H-%M-%S";

void __stdcall printTime(char* s, const char* fmt)
{
    time_t rawtime;
    tm* tm_time;
    rawtime = time(0);
    tm_time = localtime(&rawtime);
    strftime(s, 256, fmt, tm_time);
}

__declspec(naked) void onRecFileName() //005ED479
{
    __asm
    {
        mov     ecx, offset recCC
        push    ecx
        lea     ecx, [esp + 10h]
        push    ecx
        call    printTime
        mov     edx, [esi + 24h]
        mov     eax, 005ED53Eh
        jmp     eax
    }
}

__declspec(naked) void onMPSaveFileName() //00433845
{
    __asm
    {
        mov     eax, offset timeOnly
        push    eax
        push    ecx
        call    printTime
        add     esp, 8
        mov     ecx, 00433853h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setFileNameHooks(int ver) //00433877 - mp save filename
{
    strcpy(recCC, __REC_FILENAME__);

    if (ver)    //EF
        recCC[strlen(recCC) - 1] = '2';

    setHook((void*)0x005ED479, onRecFileName);
    setHook((void*)0x00433845, onMPSaveFileName);
}
#pragma optimize( "", on )
