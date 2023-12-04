#include "stdafx.h"
#include "objpanel.h"
#include "resgenbldgs.h"

TRIBE_Panel_Object* objPanelPtr = NULL;

int itemCounter;

void* langDllPopup;
char* is2ndCol;

int currentPlayer;

float* resources;

__declspec(naked) void __stdcall objPanel_invalidate()
{
    __asm
    {
        mov     ecx, objPanelPtr
        test    ecx, ecx
        jz      _bad_objpanel_ptr
        push    2
        mov     eax, 004B5F90h
        call    eax
_bad_objpanel_ptr:
        ret
    }
}

__declspec(naked) void secondCol1() //005DB8B4
{
    __asm
    {
        mov     eax, [esi + 20h]
        add     edx, ebx
        push    ecx
        cmp     edi, 3
        jng     end1
        add     edx, 6Ch    //x offset for 2nd col
end1:
        mov     ecx, 005DB8BAh
        jmp     ecx
    }
}

__declspec(naked) void secondCol2() //005DB894
{
    __asm
    {
        mov     ecx, edi
        cmp     edi, 3
        jng     end2
        sub     ecx, 4
end2:
        lea     ecx, [ecx + ecx * 8]
        shl     eax, 2
        mov     edx, 005DB89Ah
        jmp     edx
    }
}

__declspec(naked) void secondColtxty() //005DBAC0
{
    __asm
    {
        mov     eax, [esp + 6Ch]   //level
        mov     ecx, eax
        cmp     eax, 3
        jng     endy
        sub     ecx, 4
endy:
        lea     ecx, [ecx + ecx * 8]
        mov     eax, 005DBAC7h
        jmp     eax
    }
}

__declspec(naked) void secondColtxtx() //005DBAE3
{
    __asm
    {
        mov     edx, [esp + 70h]   //level
        cmp     edx, 3
        jng     endx
        add     ecx, 6Ch
endx:
        mov     edx, [esi + 20h]
        push    ecx
        mov     eax, [edx + 38h]
        mov     ecx, 005DBAEAh
        jmp     ecx
    }
}

__declspec(naked) void objPanelHook() //005DB3CE
{
    __asm
    {
        mov     ecx, [ebp + 858h]
        push    ecx
        call    objPanel
        add     esp, 4
        mov     eax, 6A3684h
        mov     eax, [eax]
        mov     ecx, [eax + 17B4h]
        mov     edx, 005DB3D9h
        jmp     edx
    }
}

__declspec(naked) void obtainPtr() //005D98AE
{
    __asm
    {
        mov     objPanelPtr, ecx
        mov     ebp, ecx
        mov     eax, [ebp + 858h]
        test    eax, eax
        jz      curPend
        mov     eax, [eax + 18h]    //selected player
        mov     ecx, [ebp + 854h]   //current player
        cmp     eax, ecx
        jz      curP
        xor     eax, eax
        jmp     curPend
curP:
        xor     eax, eax
        inc     eax
        mov     ecx, [ecx + 0ACh]   //resources
        mov     resources, ecx
curPend:
        mov     currentPlayer, eax
        push    esi
        push    edi
        xor     edi, edi
        mov     eax, 005D98B4h
        jmp     eax
    }
}

__declspec(naked) void langDllList() //005DBB13
{
    __asm
    {
        mov     ebx, langDllPopup
        mov     [ebx + eax * 4], ecx
        pop     ebx
        mov     ecx, 005DBB1Bh
        jmp     ecx
    }
}

__declspec(naked) void langDllRead1() //005DF2D9
{
    __asm
    {
        mov     esi, langDllPopup
        mov     eax, [esi + eax * 4]
        mov     ebx, 005DF2E0h
        jmp     ebx
    }
}

__declspec(naked) void langDllRead2() //005DF2F3
{
    __asm
    {
        mov     esi, langDllPopup
        mov     eax, [esi + ecx * 4]
        mov     ebx, 005DF2FAh
        jmp     ebx
    }
}

/*
int secondColFlag = 0;

__declspec(naked) void popup1() //005DF2B4
{
    __asm
    {
        inc     eax
        add     ebp, 15h
        xor     edx, edx
        cmp     ebp, 54h
        jmp     noflag1
        inc     edx
noflag1:
        mov     secondColFlag, edx
        cmp     eax, ecx
        mov     edx, 005DF2BAh
        jmp     edx
    }
}
*/

int startEbp;

__declspec(naked) void popup2() //005DF279
{
    __asm
    {
        add     ecx, edx

        mov     edx, [esp + 28h]
        push    ebx
        mov     ebx, is2ndCol
        mov     bl, [ebx + edx]
        test    bl, bl
        pop     ebx
        jz      noflag2
        mov     edx, startEbp
        test    edx, edx
        jnz     alreadySet
        mov     startEbp, ebp
        xor     ebp, ebp
        //jmp     popupCont
alreadySet:
        //sub     ebp, edx
//popupCont:
        add     ecx, 6Ch
noflag2:
        add     eax, ebp
        mov     edx, [esi + 10h]
        mov     [esp + 10h], ecx
        add     eax, edx
        mov     edx, 005DF286h
        jmp     edx
    }
}

__declspec(naked) void setItemCounter() //005D98A0
{
    __asm
    {
        push    ebp
        xor     ebp, ebp
        mov     itemCounter, ebp
        mov     ebp, esp
        and     esp, 0FFFFFFF8h
        sub     esp, 190h
        mov     eax, 005D98ACh
        jmp     eax
    }
}

__declspec(naked) void incItemCounter() //005DB840
{
    __asm
    {
        sub     esp, 50h
        push    esi
        mov     esi, itemCounter
        inc     esi
        mov     itemCounter, esi
        mov     esi, ecx
        mov     eax, 005DB846h
        jmp     eax
    }
}

__declspec(naked) void clearStartEbp() //005DF253
{
    __asm
    {
        xor     eax, eax
        mov     startEbp, eax
        mov     eax, [esi + 838h]
        mov     edx, 005DF259h
        jmp     edx
    }
}

void langDllAlloc()
{
#ifdef _DEBUG
    log("LangDLLAlloc()");
#endif

    langDllPopup = malloc(sizeof(void*) * 0x10);
    is2ndCol = (char*)malloc(0x10);
    memset(is2ndCol, 0, 0x10);
    //is2ndCol [3] = 1;
    //is2ndCol [4] = 1;
}

void langDllFree()
{
    free(langDllPopup);
}

const DWORD vtables[] =
{
    0x00663110, //80
    0x006635B0, //70
    0x0065B808, //60
    0x00657544, //50
    0x00654E54, //40
    0x0065BE14, //30
    0x0065A2C0, //25
    0x006566A0, //20
    0x00662C8C, //10
    0
};

__declspec(naked) void spUIFix() //004FFF4C
{
    __asm
    {
        mov     ecx, [edi + 1280h]
        test    ecx, ecx
        jz      unit_not_ok
        mov     edx, offset vtables
        mov     eax, [ecx]
unit_loop:
        cmp     eax, [edx]
        jz      unit_ok
        add     edx, 4
        cmp     dword ptr [edx], 0
        jnz     unit_loop
unit_not_ok:
        mov     ecx, 00500218h
        jmp     ecx
unit_ok:
        mov     eax, 004FFF5Ah
        jmp     eax
    }
}

int __stdcall get_trawler_res_icon_id(RGE_Static_Object* unit)
{
    switch (unit->attribute_type_held)
    {
    case 0: //food
    case 15:
    case 16:
    case 17:
        return 2;
    case 1: //carbon
        return 0;
    case 2: //ore
        return 1;
    case 3: //nova
        return 3;
    default:
        return -1;
    }
}

__declspec(naked) void onTrawlerUI() //005DADA3
{
    __asm
    {
        push    edi
        call    get_trawler_res_icon_id
        cmp     eax, -1
        jz      trawler_unknown_res
        mov     ecx, eax
        mov     eax, 005DADB6h
        jmp     eax

trawler_unknown_res:
        xor     eax, eax
        mov     ecx, 2
        mov     edx, 005DADC4h
        jmp     edx
    }
}

void __stdcall get_weapon_new(RGE_Combat_Object* obj, __int16* cur_weapon_in, __int16* orig_weapon_in)
{
    *cur_weapon_in = 0;
    if (obj->master_obj->weapon)
    {
        bool is_aa_unit;
        switch (obj->master_obj->object_group)
        {
        case 9:     //aa turret
        case 16:    //aa destroyer 1
        case 33:    //aa mobile
        case 40:    //aa destroyer 2
        case 55:    //aa trooper
            is_aa_unit = true;
            break;
        default:
            is_aa_unit = false;
            break;
        }
        __int16 max_total = 0;
        __int16 max_filtered = 0;
        for (int i = 0; i < obj->master_obj->weapon_num; i++)
        {
            if (is_aa_unit)
            {
                if (obj->master_obj->weapon[i].type == 0)
                    if (obj->master_obj->weapon[i].value > max_filtered)
                        max_filtered = obj->master_obj->weapon[i].value;
            }
            else
            {
                if (obj->master_obj->weapon[i].type == 3 || obj->master_obj->weapon[i].type == 4)
                    if (obj->master_obj->weapon[i].value > max_filtered)
                        max_filtered = obj->master_obj->weapon[i].value;
            }
            if (obj->master_obj->weapon[i].value > max_total)
                max_total = obj->master_obj->weapon[i].value;
        }
        if (max_filtered > 0)
            *cur_weapon_in = max_filtered;
        else
            *cur_weapon_in = max_total;
    }
    *orig_weapon_in = obj->master_obj->orig_weapon;
}

__declspec(naked) void onGetWeapon() //00444BD0
{
    __asm
    {
        mov     eax, [esp + 4]
        mov     edx, [esp + 8]
        push    edx
        push    eax
        push    ecx
        call    get_weapon_new
        ret     8
    }
}

#pragma optimize( "s", on )
void fixObjPanelDrawFunction()
{
    setHook((void*)0x005DB8B4, secondCol1);
    setHook((void*)0x005DB894, secondCol2);

    setHook((void*)0x005DBAC0, secondColtxty);
    setHook((void*)0x005DBAE3, secondColtxtx);

    setHook((void*)0x005DB3CE, objPanelHook);

    setHook((void*)0x005D98AE, obtainPtr);
    writeByte(0x005DB860, 7);

    setHook((void*)0x005D98A0, setItemCounter);
    setHook((void*)0x005DB840, incItemCounter);

    writeByte(0x005DF2B7, 0x11);
    writeByte(0x005DF298, 0x10);

    langDllAlloc();

    //setHook ((void*)0x005DF2B4, popup1);
    setHook((void*)0x005DF279, popup2);
    setHook((void*)0x005DF253, clearStartEbp);

    setHook((void*)0x005DBB13, langDllList);
    setHook((void*)0x005DF2D9, langDllRead1);
    setHook((void*)0x005DF2F3, langDllRead2);

    //Trawler UI
    writeWord(0x005DADDC, 0x9051);
    setHook((void*)0x005DADA3, onTrawlerUI);

    setHook((void*)0x00444BD0, onGetWeapon);

    //scenario editor object panel
    writeNops(0x005489A7, 4);
    writeWord(0x005489B0, 0x9966);
    writeWord(0x005489B2, 0x5050);
}

void setObjectPanelHooks(int version)
{
    if (version == VER_EF)
        fixObjPanelDrawFunction();

    setHook((void*)0x004FFF4C, spUIFix);
}
#pragma optimize( "", on )
