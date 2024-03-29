#include "stdafx.h"

#include "textcolor.h"

__declspec(naked) int colors1() //put on 004F9C5C
{
    __asm
    {
        test    eax, eax
        jz      find
        mov     esi, 6
        mov     eax, 004F9C65h
        jmp     eax
find:
        push    ebx
        push    edi
        call    getColor
        add     esp, 4
        cmp     eax, -1
        jnz     setColor
        pop     ebx
        mov     eax, 004F9C71h
        jmp     eax
setColor:
        //AH - color palette index
        //AL - strlen
        //mov   ebx, eax
        //shr   ebx, 8
        pop     ebx
        mov     byte ptr [esp + 14h], ah
        cbw
        cwde
        mov     esi, eax
        mov     byte ptr [esp + 20h], 0
        mov     eax, 004F9CB1h
        jmp     eax
    }
}

__declspec(naked) int colors2() //put on 004F967A
{
    __asm
    {
        test    eax, eax
        jz      find
        mov     byte ptr [esi], 0FBh
        mov     eax, 004F9681h
        jmp     eax
find:
        push    ebx
        mov     edx, [ebx]
        push    edx
        call    getColor
        add     esp, 4
        cmp     eax, -1
        jnz     setColor
        pop     ebx
        mov     eax, 004F9695h
        jmp     eax
setColor:
        //AH - color palette index
        //AL - strlen
        //mov   ebx, eax
        //shr   ebx, 8
        pop     ebx
        mov     byte ptr [esi], ah
        cbw
        cwde
        mov     byte ptr [edi], 0
        mov     ecx, [ebx]
        mov     edi, 004F968Bh
        jmp     edi
    }
}

struct COLOR_NAME_ASSIGN
{
    int color;
    char* name;
};

const COLOR_NAME_ASSIGN COLOR_NAMES[] =
{
    7,   "WHITE",
    0,   "BLACK",
    244, "GOLD",
    6,   "SILVER",
    11,  "SLATE",
    118, "PINK",
    //32,  "MAROON",
    30,  "MAROON",
    15,  "BROWN",
    104, "TAN",
    47,  "SAND",
    140, "CREAM",
    46,  "RUST",
    238, "LIME",
    158, "OLIVE",
    179, "FOREST",
    103, "MINT",
    132, "SEABLUE",
    22,  "SKYBLUE",
    16,  "NAVY",
    113, "PLUM",
    //new
    6,   "GRAY",
    129, "VIOLET",
    254, "CRIMSON",
    207, "SEAGREEN",
    251, "TEAL",
    251, "CYAN"
};

int getIndexByName(char* name)
{
    for (int i = 0; i < (sizeof(COLOR_NAMES) / sizeof(COLOR_NAME_ASSIGN)); i++)
        if (!strcmp(COLOR_NAMES[i].name, name))
            return COLOR_NAMES[i].color;
    return -1;
}

#define MAX_STR_N 20

int getColor(char* str)
{
    if (str[0] != '<')      //no color specifier - return error
        return -1;

    int isIndex = 1;
    int c;
    char nStr[MAX_STR_N];
    char* nStrp = nStr;

    char* strp = str;
    while (*strp != '>')
    {
        if (!*strp)
            return -1;      //unexpected end of string - return error
        strp++;
    }
    int len = strp - str + 1;
    if (len >= MAX_STR_N)
        return -1;          //string exceeds limit - return error

    strp = str + 1;

    while (*strp != '>')    //get color name
    {
        if (!*strp)
            return -1;      //unexpected end of string - return error
        *nStrp++ = *strp++;
    }
    *nStrp = 0;

    if ((*nStr == 'C') || (*nStr == 'c'))
    {                       //color index is entered
        nStrp = nStr;
        while (*++nStrp)
            if ((*nStrp < '0') || (*nStrp > '9'))
                isIndex = 0;
    }
    else
        isIndex = 0;

    if (isIndex)
    {
        sscanf_s(nStr + 1, "%d", &c);
        if ((c < 0) || (c > 255))
            return -1;      //invalid index - return error
        else
        {
            c <<= 8;
            c += len;
            return c;
        }
    }
    else
    {
        c = getIndexByName(nStr);
        if (c == -1)
            return -1;      //color not found - return error
        else
        {
            c <<= 8;
            c += len;
            return c;       //return color and strlen
        }
    }
}

#pragma optimize( "s", on )
void setTextColorHooks()
{
    setHook((void*)0x004F9C5C, colors1);
    setHook((void*)0x004F967A, colors2);

    writeByte(0x004F2B50, 6);
}
#pragma optimize( "", on )
