#include "stdafx.h"

#include "icons.h"

#define CIV_ICON_OFFSET (CIV_COUNT*4+4)

void* iconsPtr = 0;

void* iconsTechPtr;
void* iconsUnitPtr;
void* iconsBldgPtr;

__declspec(naked) void newIconsEditor ()
{
	__asm
	{
		push    esi
		push    edi

		push    CIV_ICON_OFFSET
		push    005337EFh
		call    setByte                  //buildings (load scenario routine)
		push    CIV_COUNT+1
		push    0053386Bh
		call    setByte                  //civ counter (loop counter) (load game routine)

		push    CIV_ICON_OFFSET*4
		call    ds:[malloc]              //allocate new buffer for icons
		mov     esi, eax
		mov     eax, iconsPtr            //test if need to free prev buffer
		test    eax, eax
		jz      cont
		push    eax
		call    ds:[free]
		add     esp, 4
cont:
		mov     iconsPtr, esi
		mov     iconsTechPtr, esi
		add     esi, CIV_ICON_OFFSET
		mov     iconsUnitPtr, esi
		lea     ebx, [esi + 4]
		add     esi, CIV_ICON_OFFSET
		mov     iconsBldgPtr, esi

		mov     eax, ebx

		add     esp, 14h

		pop     edi
		pop     esi
		push    00533796h
		ret
	}
}

void* techTreeBldg;
void* techTreeUnit;
void* techTreeTech;

int baseEbp;

void newIconsTechTreeBldg ();
void newIconsTechTreeUnit ();
void newIconsTechTreeTech ();

void loadTechTreeBldg ();
void loadTechTreeUnit ();
void loadTechTreeTech ();
void loadTechTreeTech2 ();

void setBaseEbp ();

__declspec(naked) void setBaseEbp () //004632E2
{
	__asm
	{
		lea     ebp, [esi + 1C74h]
		sub     ebp, 4
		mov     baseEbp, ebp
		add     ebp, 4
		push    004632E8h
		ret
	}
}

__declspec(naked) void newIconsTechTreeBldg () //00463332
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
		jz      jumploc
		push    0046333Bh
		ret
jumploc:
		push    463F15h
		ret
	}
}

__declspec(naked) void newIconsTechTreeUnit () //0046338E
{
	__asm
	{
		push    ecx
		mov     ecx, ebp
		sub     ecx, baseEbp
		add     ecx, techTreeUnit
		mov     [ecx], eax
		pop     ecx
		cmp     eax, ebx
		jz      jumploc
		push    00463397h
		ret
jumploc:
		push    463F15h
		ret
	}
}

__declspec(naked) void newIconsTechTreeTech () //004633EA
{
	__asm
	{
		push    ecx
		mov     ecx, ebp
		sub     ecx, baseEbp
		add     ecx, techTreeTech
		mov     [ecx], eax
		pop     ecx
		cmp     eax, ebx
		jz      jumploc
		push    004633F3h
		ret
jumploc:
		push    463F15h
		ret
	}
}

__declspec(naked) void loadTechTreeBldg () //0046B9D7
{
	__asm
	{
		push    ecx
		mov     ecx, techTreeBldg
		mov     edx, [ecx + edx*4]
		pop     ecx
		push    0046B9DEh
		ret
	}
}

__declspec(naked) void loadTechTreeUnit () //0046B9F4
{
	__asm
	{
		push    ecx
		mov     ecx, techTreeUnit
		mov     edx, [ecx + edx*4]
		pop     ecx
		push    0046B9FBh
		ret
	}
}

__declspec(naked) void loadTechTreeTech () //0046B9EB
{
	__asm
	{
		push    ecx
		mov     ecx, techTreeTech
		mov     edx, [ecx + edx*4]
		pop     ecx
		push    0046B9F2h
		ret
	}
}

__declspec(naked) void loadTechTreeTech2 () //0046BB18
{
	__asm
	{
		push    edx
		mov     edx, techTreeTech
		mov     ecx, [edx + ecx*4]
		pop     edx
		push    0046BB1Fh
		ret
	}
}

__declspec(naked) void newIconsGame ()
{
	__asm
	{
		push    esi
		push    edi

		mov     esi, setByte
		mov     edi, CIV_COUNT+1

		push    CIV_ICON_OFFSET
		push    004F3163h
		call    esi                      //buildings (load game routine)
		push    -CIV_ICON_OFFSET
		push    004F321Bh
		call    esi                      //techs (load game routine)
		push    edi
		push    004F3231h
		call    esi                      //civ counter (loop counter) (load game routine)
		push    edi
		push    00504F7Ch
		call    esi                      //something related to icons of items in buildings
		push    edi
		push    005044D8h
		call    esi                      //something related to icons of items in worker
		push    edi
		push    00504BC8h
		call    esi                      //something related to icons of items in worker
		push    edi
		push    00504948h
		call    esi                      //something related to icons of items in worker

		push    CIV_ICON_OFFSET*4
		call    ds:[malloc]              //allocate new buffer for icons
		mov     esi, eax
		mov     eax, iconsPtr            //test if need to free prev buffer
		test    eax, eax
		jz      cont
		push    eax
		call    ds:[free]
		add     esp, 4
cont:
		mov     iconsPtr, esi
		mov     iconsTechPtr, esi
		add     esi, CIV_ICON_OFFSET
		mov     iconsUnitPtr, esi
		lea     ebp, [esi + 4]
		add     esi, CIV_ICON_OFFSET
		mov     iconsBldgPtr, esi

		add     esp, 3Ch

		pop     edi
		pop     esi
		push    004F3117h
		ret
	}
}

__declspec(naked) void iconLoadUnitAvailInBldg ()
{
	__asm
	{
		mov     ecx, iconsUnitPtr
		mov     ecx, [ecx + eax * 4]
		push    005055AAh
		ret
	}
}

__declspec(naked) void iconLoadTechAvailInBldg ()
{
	__asm
	{
		mov     edx, iconsTechPtr
		mov     edx, [edx + ecx * 4]
		push    0050581Ah
		ret
	}
}

__declspec(naked) void iconLoadWorkerDefenceBldg ()
{
	__asm
	{
		mov     ecx, iconsBldgPtr
		mov     ecx, [ecx + eax * 4]
		push    00504CFEh
		ret
	}
}

__declspec(naked) void iconLoadWorkerMilitaryBldg ()
{
	__asm
	{
		mov     ecx, iconsBldgPtr
		mov     ecx, [ecx + eax * 4]
		push    00504A7Eh
		ret
	}
}

__declspec(naked) void iconLoadWorkerEconomyBldg ()
{
	__asm
	{
		mov     ecx, iconsBldgPtr
		mov     ecx, [ecx + eax * 4]
		push    00504608h
		ret
	}
}

__declspec(naked) void iconLoadUnitBeingCreated ()
{
	__asm
	{
		mov     eax, iconsUnitPtr
		mov     eax, [eax + ecx * 4]
		push    0050701Dh
		ret
	}
}

__declspec(naked) void iconLoadTechBeingResearched ()
{
	__asm
	{
		mov     eax, iconsTechPtr
		mov     eax, [eax + ecx * 4]
		push    00506FFDh
		ret
	}
}

__declspec(naked) void iconLoadUnitSelected ()
{
	__asm
	{
		mov     edx, iconsUnitPtr
		mov     edx, [edx + eax * 4]
		push    005D9BEDh
		ret
	}
}

__declspec(naked) void iconLoadBldgSelected ()
{
	__asm
	{
		mov     edx, iconsBldgPtr
		mov     edx, [edx + eax * 4]
		push    005D9C06h
		ret
	}
}

__declspec(naked) void iconLoadMultipleUnitIcons ()
{
	__asm
	{
		mov     edx, iconsUnitPtr
		mov     edx, [edx + eax * 4]
		push    005DD1E4h
		ret
	}
}

__declspec(naked) void iconLoadMultipleBldgIcons ()
{
	__asm
	{
		mov     edx, iconsBldgPtr
		mov     edx, [edx + eax * 4]
		push    005DD1F8h
		ret
	}
}

__declspec(naked) void iconGarrisonedUnitIcons ()
{
	__asm
	{
		mov     edx, iconsUnitPtr
		mov     edx, [edx + eax * 4]
		push    005DD8AFh
		ret
	}
}

__declspec(naked) void iconLoadEditorUnitIcon ()
{
	__asm
	{
		mov     edi, iconsUnitPtr
		mov     edi, [edi + edx * 4]
		push    0054847Ah
		ret
	}
}

__declspec(naked) void iconLoadEditorBldgIcon ()
{
	__asm
	{
		mov     cx, [ecx+1D6h]       //from the code
		mov     edi, iconsBldgPtr
		mov     edi, [edi + edx * 4]
		push    00548478h
		ret
	}
}

__declspec(naked) void iconLoadWorkerFarm ()
{
	__asm
	{
		mov     ecx, iconsBldgPtr
		mov     ecx, [ecx + eax * 4]
		push    00504849h
		ret
	}
}

__declspec(naked) void iconLoadTrawler ()
{
	__asm
	{
		mov     edx, iconsBldgPtr
		mov     edx, [edx + ecx * 4]
		push    005033E7h
		ret
	}
}

__declspec(naked) void iconLoadFoodProcFarm ()
{
	__asm
	{
		mov     ecx, iconsBldgPtr
		mov     eax, [ecx + eax * 4]
		push    005050E5h
		ret
	}
}

__declspec(naked) void iconLoadFoodProcFarmQueue ()
{
	__asm
	{
		mov     eax, iconsBldgPtr
		mov     eax, [eax + ecx * 4]
		push    00506FDDh
		ret
	}
}

__declspec(naked) void iconCCLevelUp ()
{
	__asm
	{
		mov     edx, iconsTechPtr
		mov     edx, [edx + ecx * 4]
		push    00505ACEh
		ret
	}
}

__declspec(naked) void iconTCMounted ()
{
	__asm
	{
		mov     edx, iconsUnitPtr
		mov     edx, [edx + ecx * 4]
		push    00505422h
		ret
	}
}

SET_HOOK_ASSIGN iconHooks [] =
{
	{(void*)0x004F3111, &newIconsGame},
	{(void*)0x00533790, &newIconsEditor},

	                                                            //civ is always in eax, unless stated otherwise
	{(void*)0x005055A3, &iconLoadUnitAvailInBldg},      //load icons of units available in this bldg to ecx +7
	{(void*)0x00505813, &iconLoadTechAvailInBldg},      //load icons of techs available in this bldg to edx +7, civ in ecx
	{(void*)0x00504CF7, &iconLoadWorkerDefenceBldg},    //load worker defence bldg icons
	{(void*)0x00504A77, &iconLoadWorkerMilitaryBldg},   //load worker military bldg icons
	{(void*)0x00504601, &iconLoadWorkerEconomyBldg},    //load worker economy bldg icons
	{(void*)0x00507016, &iconLoadUnitBeingCreated},     //load icon of unit being created to eax, civ in ecx
	{(void*)0x00506FF6, &iconLoadTechBeingResearched},  //load icon of tech being researched to eax, civ in ecx
	{(void*)0x005D9BE4, &iconLoadUnitSelected},         //load selected unit icon +9 to edx
	{(void*)0x005D9BFD, &iconLoadBldgSelected},         //load selected building icon +9
	{(void*)0x005DD1DD, &iconLoadMultipleUnitIcons},    //load multiple unit icons to edx +7
	{(void*)0x005DD1F1, &iconLoadMultipleBldgIcons},    //load multiple bldg icons to edx
	{(void*)0x005DD8A8, &iconGarrisonedUnitIcons},      //load garrisoned unit icons
	{(void*)0x00548457, &iconLoadEditorUnitIcon},       //load worker military bldg icons
	{(void*)0x0054846E, &iconLoadEditorBldgIcon},       //load worker military bldg icons

	{(void*)0x00504842, &iconLoadWorkerFarm},
	{(void*)0x005033E0, &iconLoadTrawler},
	{(void*)0x005050DE, &iconLoadFoodProcFarm},
	{(void*)0x00506FD6, &iconLoadFoodProcFarmQueue},

	{(void*)0x00505AC7, &iconCCLevelUp},

	{(void*)0x0050541B, &iconTCMounted},

	// TECH TREE

	{(void*)0x00463332, &newIconsTechTreeBldg},
	{(void*)0x0046338E, &newIconsTechTreeUnit},
	{(void*)0x004633EA, &newIconsTechTreeTech},

	{(void*)0x0046B9D7, &loadTechTreeBldg},
	{(void*)0x0046B9F4, &loadTechTreeUnit},
	{(void*)0x0046B9EB, &loadTechTreeTech},
	{(void*)0x0046BB18, &loadTechTreeTech2},

	{(void*)0x004632E2, &setBaseEbp}
};

void fixIconLoadingRoutines ()
{
#ifdef _DEBUG
	log ("Setting icon hooks...");
#endif

	for (int i = 0; i < (sizeof(iconHooks)/sizeof(iconHooks[0])); i++)
		setHook (iconHooks[i].addr, iconHooks[i].newAddr);

	techTreeBldg = malloc (CIV_ICON_OFFSET);
	techTreeUnit = malloc (CIV_ICON_OFFSET);
	techTreeTech = malloc (CIV_ICON_OFFSET);

	setByte (0x00463402, CIV_COUNT+1);
}
