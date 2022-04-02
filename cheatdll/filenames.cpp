#include "stdafx.h"

#include <time.h>
#include "filenames.h"

char recCC[] = "recorded game - %Y-%m-%d_%H-%M-%S.mg1";
char rec_auto[] = "autoreplay.mg1";

const char timeOnly[] = "%Y-%m-%d_%H-%M-%S";

bool do_autoreplay = false;

void __stdcall printTime(char* s, const char* fmt)
{
    time_t rawtime;
    tm* tm_time;
    rawtime = time(0);
    tm_time = localtime(&rawtime);
    strftime(s, 256, fmt, tm_time);
}

void __stdcall make_rec_filename(char* s, const char* fmt)
{
    if (do_autoreplay)
    {
        strcpy_safe(s, 256, rec_auto);
        do_autoreplay = false;
    }
    else
        printTime(s, fmt);
}

__declspec(naked) void onRecFileName() //005ED479
{
    __asm
    {
        mov     ecx, offset recCC
        push    ecx
        lea     ecx, [esp + 10h]
        push    ecx
        call    make_rec_filename
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

extern int current_loaded_version;

int __stdcall do_autoreplay_init()
{
    if (BaseGame__singlePlayerGame(*base_game) && current_loaded_version == CURRENT_VERSION)
    {
        if (BaseGame__getRecordGame(*base_game))
            do_autoreplay = false;
        else
        {
            BaseGame__setRecordGame(*base_game, true);
            do_autoreplay = true;
        }
    }
    else
        do_autoreplay = false;
    
    return BaseGame__getRecordGame(*base_game);
}

__declspec(naked) void onStartCommandLog() //005ED41A
{
    __asm
    {
        call    do_autoreplay_init
        test    eax, eax
        mov     ecx, 005ED421h
        jmp     ecx
    }
}

void setup_rec_file_names(int ver)
{
    if (ver)    //EF
    {
        recCC[strlen(recCC) - 1] = '2';
        rec_auto[strlen(rec_auto) - 1] = '2';
    }
}

#pragma optimize( "s", on )
void setFileNameHooks(int ver) //00433877 - mp save filename
{
    setup_rec_file_names(ver);

    setHook((void*)0x005ED479, onRecFileName);
    setHook((void*)0x00433845, onMPSaveFileName);

    setHook((void*)0x005ED41A, onStartCommandLog);
}
#pragma optimize( "", on )
