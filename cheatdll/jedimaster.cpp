#include "stdafx.h"

#include "jedimaster.h"

short* masters = NULL;
int nMasters = 0;

short* padawans = NULL;
int nPadawans = 0;

__declspec(naked) void isMaster() //ax = ID
{
    __asm
    {
        push    ecx
        push    edx
        mov     ecx, nMasters
        mov     edx, masters
loopcont:
        test    ecx, ecx
        jz      loopend
        dec     ecx
        cmp     word ptr [edx + ecx * 2], ax
        jnz     loopcont
        xor     eax, eax
        inc     eax
        pop     edx
        pop     ecx
        ret
loopend:
        xor     eax, eax
        pop     edx
        pop     ecx
        ret
    }
}

__declspec(naked) void isPadawan() //ax = ID
{
    __asm
    {
        push    ecx
        push    edx
        mov     ecx, nPadawans
        mov     edx, padawans
loopcont_p:
        test    ecx, ecx
        jz      loopend_p
        dec     ecx
        cmp     word ptr [edx + ecx * 2], ax
        jnz     loopcont_p
        xor     eax, eax
        inc     eax
        pop     edx
        pop     ecx
        ret
loopend_p:
        xor     eax, eax
        pop     edx
        pop     ecx
        ret
    }
}

__declspec(naked) void jediMasterHook() //0054B1D0
{
    __asm
    {
        mov     ecx, [ecx + 14h]
        mov     ax, [ecx + 1Eh]
        cmp     ax, 32h
        jnz     nonjedi
        mov     ax, [ecx + 18h]
        call    isMaster
        ret
nonjedi:
        xor     eax, eax
        ret
    }
}

__declspec(naked) void jediPadawanHook() //0054B170
{
    __asm
    {
        mov     ecx, [ecx + 14h]
        mov     ax, [ecx + 1Eh]
        cmp     ax, 32h
        jnz     nonjedi_p
        mov     ax, [ecx + 1Ah]
        call    isPadawan
        ret
nonjedi_p:
        xor     eax, eax
        ret
    }
}

bool jediMasterHookInstalled = false;
bool jediPadawanHookInstalled = false;

void setJediMasterHooks(const char* prefix, const char* filename_master, const char* filename_padawan)
{
    log("Loading jedi master unit list");
    char full_filename_master[0x100];
    char full_filename_padawan[0x100];
    snprintf(full_filename_master, _countof(full_filename_master), "%s%s", prefix, filename_master);
    snprintf(full_filename_padawan, _countof(full_filename_padawan), "%s%s", prefix, filename_padawan);
    FILE* f = fopen(full_filename_master, "rt");
    if (f)
    {
        short id;
        nMasters = 0;
        if (masters)
        {
            free(masters);
            masters = NULL;
        }

        while (fscanf(f, "%hd", &id) > 0)
        {
            nMasters++;
            masters = (short*)realloc(masters, nMasters * sizeof(short));
            masters[nMasters - 1] = id;
        }

        fclose(f);

        if (!jediMasterHookInstalled)
        {
            setHook((void*)0x0054B1D0, jediMasterHook);
            jediMasterHookInstalled = true;
        }
    }
    else
        log("Warning: %s not found, using default settings", full_filename_master);

    log("Loading jedi padawan unit list");
    f = fopen(full_filename_padawan, "rt");
    if (f)
    {
        short id;
        nPadawans = 0;
        if (padawans)
        {
            free(padawans);
            padawans = NULL;
        }

        while (fscanf(f, "%hd", &id) > 0)
        {
            nPadawans++;
            padawans = (short*)realloc(padawans, nPadawans * sizeof(short));
            padawans[nPadawans - 1] = id;
        }

        fclose(f);

        if (!jediPadawanHookInstalled)
        {
            setHook((void*)0x0054B170, jediPadawanHook);
            jediPadawanHookInstalled = true;
        }
    }
    else
        log("Warning: %s not found, using default settings", full_filename_padawan);
}
