#include "stdafx.h"

short* unconv = NULL;
int nUnconv = 0;

__declspec(naked) void isUnconvertable() //ebx = ID
{
    __asm
    {
        mov     eax, nUnconv
        mov     edx, unconv
loopcont:
        test    eax, eax
        jz      loopend
        dec     eax
        cmp     word ptr [edx + eax * 2], bx
        jnz     loopcont
        xor     eax, eax
        inc     eax
        ret
loopend:
        xor     eax, eax
        ret
    }
}

__declspec(naked) void unConvertHook() //00567EDC
{
    __asm
    {
        push    edx
        call    isUnconvertable
        pop     edx
        test    eax, eax
        jz      convertable
        mov     edx, 0056823Ch
        jmp     edx
convertable:
        mov     eax, 00567EF1h
        jmp     eax
    }
}

bool unconvHookInstalled = false;

void setConvertHooks(const char* prefix, const char* filename)
{
    log("Loading unconvertable unit list");
    char full_filename[0x100];
    sprintf(full_filename, "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        short id;
        nUnconv = 0;
        if (unconv)
        {
            free(unconv);
            unconv = NULL;
        }

        while (fscanf(f, "%hd", &id) > 0)
        {
            nUnconv++;
            unconv = (short*)realloc(unconv, nUnconv * sizeof(short));
            unconv[nUnconv - 1] = id;
        }

        fclose(f);

        if (!unconvHookInstalled)
        {
            setHook((void*)0x00567EDC, unConvertHook);
            unconvHookInstalled = true;
        }
    }
    else
        log("Warning: %s not found, using default settings", full_filename);
}
