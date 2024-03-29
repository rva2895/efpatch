#include "stdafx.h"
#include "icons.h"

#define CIV_ICON_OFFSET (CIV_COUNT*4+4)

void* iconsPtr = 0;

void* iconsTechPtr;
void* iconsUnitPtr;
void* iconsBldgPtr;

__declspec(naked) void newIconsEditor()
{
    __asm
    {
        push    esi

        push    CIV_ICON_OFFSET*4
        call    malloc                   //allocate new buffer for icons
        add     esp, 4
        mov     esi, eax
        mov     eax, iconsPtr            //test if need to free prev buffer
        test    eax, eax
        jz      cont_editor
        push    eax
        call    free
        add     esp, 4
cont_editor:
        mov     iconsPtr, esi
        mov     iconsTechPtr, esi
        add     esi, CIV_ICON_OFFSET
        mov     iconsUnitPtr, esi
        lea     ebx, [esi + 4]
        add     esi, CIV_ICON_OFFSET
        mov     iconsBldgPtr, esi

        mov     eax, ebx

        pop     esi
        mov     ebx, 00533796h
        jmp     ebx
    }
}

void* techTreeBldg;
void* techTreeUnit;
void* techTreeTech;

int baseEbp;

void newIconsTechTreeBldg();
void newIconsTechTreeUnit();
void newIconsTechTreeTech();

void loadTechTreeBldg();
void loadTechTreeUnit();
void loadTechTreeTech();
void loadTechTreeTech2();

void setBaseEbp();

__declspec(naked) void setBaseEbp() //004632E2
{
    __asm
    {
        lea     ebp, [esi + 1C74h]
        mov     ecx, ebp
        sub     ecx, 4
        mov     baseEbp, ecx
        mov     ecx, 004632E8h
        jmp     ecx
    }
}

__declspec(naked) void newIconsTechTreeBldg() //00463332
{
    __asm
    {
        push    ecx
        mov     ecx, ebp
        sub     ecx, baseEbp
        add     ecx, techTreeBldg
        mov     [ecx], eax
        pop     ecx
        cmp     eax, ebx
        jz      jumploc_bldg
        mov     ecx, 0046333Bh
        jmp     ecx
jumploc_bldg:
        mov     edi, 00463F15h
        jmp     edi
    }
}

__declspec(naked) void newIconsTechTreeUnit() //0046338E
{
    __asm
    {
        mov     ecx, ebp
        sub     ecx, baseEbp
        add     ecx, techTreeUnit
        mov     [ecx], eax
        cmp     eax, ebx
        jz      jumploc_unit
        mov     ecx, 00463397h
        jmp     ecx
jumploc_unit:
        mov     edi, 00463F15h
        jmp     edi
    }
}

__declspec(naked) void newIconsTechTreeTech() //004633EA
{
    __asm
    {
        mov     ecx, ebp
        sub     ecx, baseEbp
        add     ecx, techTreeTech
        mov     [ecx], eax
        cmp     eax, ebx
        jz      jumploc_tech
        mov     ecx, 004633F3h
        jmp     ecx
jumploc_tech:
        mov     edi, 00463F15h
        jmp     edi
    }
}

__declspec(naked) void loadTechTreeBldg() //0046B9D7
{
    __asm
    {
        push    ecx
        mov     ecx, techTreeBldg
        mov     edx, [ecx + edx * 4]
        pop     ecx
        mov     [esp + 20h], edx
        mov     edx, 0046B9FFh
        jmp     edx
    }
}

__declspec(naked) void loadTechTreeUnit() //0046B9F4
{
    __asm
    {
        push    ecx
        mov     ecx, techTreeUnit
        mov     edx, [ecx + edx * 4]
        pop     ecx
        mov     [esp + 20h], edx
        mov     edx, 0046B9FFh
        jmp     edx
    }
}

__declspec(naked) void loadTechTreeTech() //0046B9EB
{
    __asm
    {
        push    ecx
        mov     ecx, techTreeTech
        mov     edx, [ecx + edx * 4]
        pop     ecx
        mov     [esp + 20h], edx
        mov     edx, 0046B9FFh
        jmp     edx
    }
}

__declspec(naked) void loadTechTreeTech2() //0046BB18
{
    __asm
    {
        mov     edi, techTreeTech
        mov     ecx, [edi + ecx * 4]
        mov     edi, 0046BB1Fh
        jmp     edi
    }
}

__declspec(naked) void newIconsGame()
{
    __asm
    {
        push    esi

        push    CIV_ICON_OFFSET*4
        call    malloc                   //allocate new buffer for icons
        add     esp, 4
        mov     esi, eax
        mov     eax, iconsPtr            //test if need to free prev buffer
        test    eax, eax
        jz      cont_game
        push    eax
        call    free
        add     esp, 4
cont_game:
        mov     iconsPtr, esi
        mov     iconsTechPtr, esi
        add     esi, CIV_ICON_OFFSET
        mov     iconsUnitPtr, esi
        lea     ebp, [esi + 4]
        add     esi, CIV_ICON_OFFSET
        mov     iconsBldgPtr, esi

        pop     esi
        mov     ecx, 004F3117h
        jmp     ecx
    }
}

__declspec(naked) void iconLoadUnitAvailInBldg()
{
    __asm
    {
        mov     ecx, iconsUnitPtr
        mov     ecx, [ecx + eax * 4]
        mov     eax, 005055AAh
        jmp     eax
    }
}

__declspec(naked) void iconLoadTechAvailInBldg()
{
    __asm
    {
        mov     edx, iconsTechPtr
        mov     edx, [edx + ecx * 4]
        mov     ecx, 0050581Ah
        jmp     ecx
    }
}

__declspec(naked) void iconLoadWorkerDefenceBldg()
{
    __asm
    {
        mov     ecx, iconsBldgPtr
        mov     ecx, [ecx + eax * 4]
        mov     eax, 00504CFEh
        jmp     eax
    }
}

__declspec(naked) void iconLoadWorkerMilitaryBldg()
{
    __asm
    {
        mov     ecx, iconsBldgPtr
        mov     ecx, [ecx + eax * 4]
        mov     eax, 00504A7Eh
        jmp     eax
    }
}

__declspec(naked) void iconLoadWorkerEconomyBldg()
{
    __asm
    {
        mov     ecx, iconsBldgPtr
        mov     ecx, [ecx + eax * 4]
        mov     eax, 00504608h
        jmp     eax
    }
}

__declspec(naked) void iconLoadUnitBeingCreated()
{
    __asm
    {
        mov     eax, iconsUnitPtr
        mov     eax, [eax + ecx * 4]
        mov     esi, 0050701Dh
        jmp     esi
    }
}

__declspec(naked) void iconLoadTechBeingResearched()
{
    __asm
    {
        mov     eax, iconsTechPtr
        mov     eax, [eax + ecx * 4]
        mov     esi, 00506FFDh
        jmp     esi
    }
}

__declspec(naked) void iconLoadUnitSelected()
{
    __asm
    {
        mov     edx, iconsUnitPtr
        mov     edx, [edx + eax * 4]
        mov     ecx, 005D9BEDh
        jmp     ecx
    }
}

__declspec(naked) void iconLoadBldgSelected()
{
    __asm
    {
        mov     edx, iconsBldgPtr
        mov     edx, [edx + eax * 4]
        mov     ecx, 005D9C06h
        jmp     ecx
    }
}

__declspec(naked) void iconLoadMultipleUnitIcons()
{
    __asm
    {
        mov     edx, iconsUnitPtr
        mov     edx, [edx + eax * 4]
        mov     ebx, 005DD1E4h
        jmp     ebx
    }
}

__declspec(naked) void iconLoadMultipleBldgIcons()
{
    __asm
    {
        mov     edx, iconsBldgPtr
        mov     edx, [edx + eax * 4]
        mov     ebx, 005DD1F8h
        jmp     ebx
    }
}

__declspec(naked) void iconGarrisonedUnitIcons()
{
    __asm
    {
        mov     edx, iconsUnitPtr
        mov     edx, [edx + eax * 4]
        mov     ecx, 005DD8AFh
        jmp     ecx
    }
}

__declspec(naked) void iconLoadEditorUnitIcon()
{
    __asm
    {
        mov     edi, iconsUnitPtr
        mov     edi, [edi + edx * 4]
        mov     eax, 0054847Ah
        jmp     eax
    }
}

__declspec(naked) void iconLoadEditorBldgIcon()
{
    __asm
    {
        mov     cx, [ecx + 1D6h]       //from the code
        mov     edi, iconsBldgPtr
        mov     edi, [edi + edx * 4]
        mov     eax, 00548478h
        jmp     eax
    }
}

__declspec(naked) void iconLoadWorkerFarm()
{
    __asm
    {
        mov     ecx, iconsBldgPtr
        mov     ecx, [ecx + eax * 4]
        mov     eax, 00504849h
        jmp     eax
    }
}

__declspec(naked) void iconLoadTrawler()
{
    __asm
    {
        mov     edx, iconsBldgPtr
        mov     edx, [edx + ecx * 4]
        mov     ecx, 005033E7h
        jmp     ecx
    }
}

__declspec(naked) void iconLoadFoodProcFarm()
{
    __asm
    {
        mov     ecx, iconsBldgPtr
        mov     eax, [ecx + eax * 4]
        mov     edx, 005050E5h
        jmp     edx
    }
}

__declspec(naked) void iconLoadFoodProcFarmQueue()
{
    __asm
    {
        mov     eax, iconsBldgPtr
        mov     eax, [eax + ecx * 4]
        mov     esi, 00506FDDh
        jmp     esi
    }
}

__declspec(naked) void iconCCLevelUp()
{
    __asm
    {
        mov     edx, iconsTechPtr
        mov     edx, [edx + ecx * 4]
        mov     ecx, 00505ACEh
        jmp     ecx
    }
}

__declspec(naked) void iconTCMounted()
{
    __asm
    {
        mov     edx, iconsUnitPtr
        mov     edx, [edx + ecx * 4]
        mov     ecx, 00505422h
        jmp     ecx
    }
}

int __fastcall get_icon_tc_mounted_tech(int civ, int normal_id)
{
    switch (civ)
    {
    case 9:
        return 613; //avail-taniz
    case 10:
        return 742; //avail-tanio
    case 11:
        return 813; //avail-tanifo
    case 12:
        return 873; //avail-tanirs
    case 13:
        return 1001; //avail-taniir
    case 14:
        return 1051; //avail-taninr
    case 15:
        return 1101; //avail-tanibs
    default:
        return normal_id;
    }
}

__declspec(naked) void iconTCMounted_id_tech() //005051E0
{
    __asm
    {
        mov     edx, [esp + ecx * 4 + 3Ch]
        call    get_icon_tc_mounted_tech
        mov     ecx, [ebp + 1D94h]
        push    eax
        mov     edx, 005051EBh
        jmp     edx
    }
}

int __fastcall get_icon_tc_mounted_unit(int civ, int normal_id)
{
    switch (civ)
    {
    case 9:
        return 1748; //unit-taniz
    case 10:
        return 4056; //unit-tanio
    case 11:
        return 4586; //unit-tanifo
    case 12:
        return 4703; //unit-tanirs
    case 13:
        return 5248; //unit-taniir
    case 14:
        return 5321; //unit-taninr
    case 15:
        return 5384; //unit-tanibs
    default:
        return normal_id;
    }
}

__declspec(naked) void iconTCMounted_id_unit() //00505276
{
    __asm
    {
        mov     edx, [esp + eax * 4 + 3Ch]
        mov     ebx, ecx
        mov     ecx, eax
        call    get_icon_tc_mounted_unit
        mov     ecx, ebx
        mov     ebx, eax
        lea     edx, [esp + 32h]
        mov     eax, 0050527Eh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void fixIconLoadingRoutines()
{
    setHook((void*)0x004F3111, newIconsGame);
    setHook((void*)0x00533790, newIconsEditor);

    writeByte(0x005337EF, CIV_ICON_OFFSET);                   //buildings (load scenario routine)
    writeByte(0x0053386B, CIV_COUNT + 1);                     //civ counter (loop counter) (load game routine)

    writeByte(0x004F3163, CIV_ICON_OFFSET);                   //buildings (load game routine)
    writeByte(0x004F321B, -CIV_ICON_OFFSET);                  //techs (load game routine)
    writeByte(0x004F3231, CIV_COUNT + 1);                     //civ counter (loop counter) (load game routine)
    writeByte(0x00504F7C, CIV_COUNT + 1);                     //icons of items in buildings
    writeByte(0x005044D8, CIV_COUNT + 1);                     //icons of items in worker
    writeByte(0x00504BC8, CIV_COUNT + 1);                     //items in worker
    writeByte(0x00504948, CIV_COUNT + 1);                     //items in worker

    //civ is always in eax, unless stated otherwise
    setHook((void*)0x005055A3, iconLoadUnitAvailInBldg);      //load icons of units available in this bldg to ecx +7
    setHook((void*)0x00505813, iconLoadTechAvailInBldg);      //load icons of techs available in this bldg to edx +7, civ in ecx
    setHook((void*)0x00504CF7, iconLoadWorkerDefenceBldg);    //load worker defence bldg icons
    setHook((void*)0x00504A77, iconLoadWorkerMilitaryBldg);   //load worker military bldg icons
    setHook((void*)0x00504601, iconLoadWorkerEconomyBldg);    //load worker economy bldg icons
    setHook((void*)0x00507016, iconLoadUnitBeingCreated);     //load icon of unit being created to eax, civ in ecx
    setHook((void*)0x00506FF6, iconLoadTechBeingResearched);  //load icon of tech being researched to eax, civ in ecx
    setHook((void*)0x005D9BE4, iconLoadUnitSelected);         //load selected unit icon +9 to edx
    setHook((void*)0x005D9BFD, iconLoadBldgSelected);         //load selected building icon +9
    setHook((void*)0x005DD1DD, iconLoadMultipleUnitIcons);    //load multiple unit icons to edx +7
    setHook((void*)0x005DD1F1, iconLoadMultipleBldgIcons);    //load multiple bldg icons to edx
    setHook((void*)0x005DD8A8, iconGarrisonedUnitIcons);      //load garrisoned unit icons
    setHook((void*)0x00548457, iconLoadEditorUnitIcon);       //load worker military bldg icons
    setHook((void*)0x0054846E, iconLoadEditorBldgIcon);       //load worker military bldg icons

    setHook((void*)0x00504842, iconLoadWorkerFarm);
    setHook((void*)0x005033E0, iconLoadTrawler);
    setHook((void*)0x005050DE, iconLoadFoodProcFarm);
    setHook((void*)0x00506FD6, iconLoadFoodProcFarmQueue);

    setHook((void*)0x00505AC7, iconCCLevelUp);

    setHook((void*)0x0050541B, iconTCMounted);

    setHook((void*)0x005051E0, iconTCMounted_id_tech);
    setHook((void*)0x00505276, iconTCMounted_id_unit);

    // TECH TREE

    setHook((void*)0x00463332, newIconsTechTreeBldg);
    setHook((void*)0x0046338E, newIconsTechTreeUnit);
    setHook((void*)0x004633EA, newIconsTechTreeTech);

    setHook((void*)0x0046B9D7, loadTechTreeBldg);
    setHook((void*)0x0046B9F4, loadTechTreeUnit);
    setHook((void*)0x0046B9EB, loadTechTreeTech);
    setHook((void*)0x0046BB18, loadTechTreeTech2);

    setHook((void*)0x004632E2, setBaseEbp);

    techTreeBldg = malloc(CIV_ICON_OFFSET);
    techTreeUnit = malloc(CIV_ICON_OFFSET);
    techTreeTech = malloc(CIV_ICON_OFFSET);

    writeByte(0x00463402, CIV_COUNT + 1);

    //new EF icon offsets
    writeDword(0x00463314, 53300);    //bldg
    writeDword(0x00463370, 53330);    //unit
    writeDword(0x004633CC, 53360);    //tech

    writeDword(0x004F3143, 53300);    //bldg
    writeDword(0x004F319F, 53330);    //unit
    writeDword(0x004F31FB, 53360);    //tech

    writeDword(0x005337CB, 53300);    //bldg
    writeDword(0x00533830, 53330);    //unit
    //writeDword(0x004F31FB, 53360);    //tech

    writeByte(0x00502D4C, CIV_COUNT + 1);
}
#pragma optimize( "", on )
