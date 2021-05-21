#include "stdafx.h"
#include "rec.h"
#include "advcheat.h"

__declspec(naked) int __stdcall isRec()
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     ecx, [ecx + 17B4h]
        test    ecx, ecx
        jz      _no_rec
        mov     ecx, [ecx + 126Ch]
        test    ecx, ecx
        jz      _no_rec
        mov     ecx, [ecx + 0Ch]
        test    ecx, ecx
        jz      _no_rec
        mov     edx, [ecx + 1614h]
        mov     eax, [edx + 4]
        test    eax, eax
        setg    al
        movzx   eax, al
        ret
_no_rec:
        xor     eax, eax
        ret
    }
}

__declspec(naked) void onMenuInit() //0045E425
{
    __asm
    {
        cmp     dword ptr [esp + 18h], 1
        jz      _contMenu
        call    isRec
        mov     [esp + 18h], eax
_contMenu:
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     eax, 0045E42Bh
        jmp     eax
    }
}

char* chatStr = (char*)0x007A20C0;
char* chatStr_temp;

void __stdcall recPrintChat(bool our)
{
    if (!our)
    {
        strcpy(chatStr_temp, chatStr);
        sprintf(chatStr + 3, "%s", chatStr_temp + 3);
    }
}

__declspec(naked) void onRecChat() //0061FCD0
{
    __asm
    {
        cmp     [edx + 538h], eax
        setz    al
        push    eax
        call    recPrintChat
        mov     eax, 0061FCD8h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setRecHooks()
{
    chatStr_temp = (char*)malloc(0x400);

    setHook((void*)0x0045E425, onMenuInit);
    setHook((void*)0x0061FCD0, onRecChat);
}
#pragma optimize( "", on )

void recSwitch(int p)
{
    if (isRec())
    {
        WorldPlayerBase__unselect_object(getCurrentPlayer());
        takeControl(p);
    }
}
