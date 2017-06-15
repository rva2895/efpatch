#include "stdafx.h"

#include "terrain.h"

__declspec(naked) int terrain1 ()
{
	__asm
	{
		push    esi
		mov     esi, 4C82D0h
		mov     ecx, [edi]
		push    0
		push    297Dh
		call    esi
		mov     ecx, [edi]
		push    1
		push    2980h
		call    esi
		mov     ecx, [edi]
		push    2
		push    10926
		call    esi
		mov     ecx, [edi]
		push    3
		push    2993h
		call    esi
		mov     ecx, [edi]
		push    4
		push    2983h
		call    esi
		mov     ecx, [edi]
		push    5
		push    299Ah
		call    esi
		mov     ecx, [edi]
		push    6
		push    10622
		call    esi
		mov     ecx, [edi]
		push    7
		push    10920
		call    esi
		mov     ecx, [edi]
		push    8
		push    10921
		call    esi
		mov     ecx, [edi]
		push    9
		push    2991h
		call    esi
		mov     ecx, [edi]
		push    0Ah
		push    297Fh
		call    esi
		mov     ecx, [edi]
		push    0Bh
		push    2992h
		call    esi
		mov     ecx, [edi]
		push    0Ch
		push    2990h
		call    esi
		mov     ecx, [edi]
		push    0Dh
		push    2981h
		call    esi
		mov     ecx, [edi]
		push    0Eh
		push    2998h
		call    esi
		mov     ecx, [edi]
		push    0Fh
		push    2A2Dh
		call    esi
		mov     ecx, [edi]
		push    10h
		push    2A2Eh
		call    esi
		mov     ecx, [edi]
		push    11h
		push    29A8h
		call    esi
		mov     ecx, [edi]
		push    12h
		push    2999h
		call    esi
		mov     ecx, [edi]
		push    13h
		push    2984h
		call    esi
		mov     ecx, [edi]
		push    14h
		push    2982h
		call    esi
		mov     ecx, [edi]
		push    15h
		push    29A9h
		call    esi
		mov     ecx, [edi]
		push    16h
		push    2985h
		call    esi
		mov     ecx, [edi]
		push    17h
		push    2994h
		call    esi
		mov     ecx, [edi]
		push    18h
		push    2995h
		call    esi
		mov     ecx, [edi]
		push    19h
		push    2996h
		call    esi
		mov     ecx, [edi]
		push    1Ah
		push    299Bh
		call    esi
		mov     ecx, [edi]
		push    1Bh
		push    10927
		call    esi
		mov     ecx, [edi]
		push    1Ch
		push    10929
		call    esi
		mov     ecx, [edi]
		push    1Dh
		push    10922
		call    esi
		mov     ecx, [edi]
		push    1Eh
		push    10925
		call    esi
		mov     ecx, [edi]
		push    1Fh
		push    10924
		call    esi
		mov     ecx, [edi]
		push    20h
		push    29AAh
		call    esi
		mov     ecx, [edi]
		push    21h
		push    10668
		call    esi
		mov     ecx, [edi]
		push    22h
		push    29ABh
		call    esi
		mov     ecx, [edi]
		push    23h
		push    29ADh
		call    esi
		mov     ecx, [edi]
		push    24h
		push    10928
		call    esi
		mov     ecx, [edi]
		push    25h
		push    10930
		call    esi
		mov     ecx, [edi]
		push    26h
		push    29B5h
		call    esi
		mov     ecx, [edi]
		push    27h
		push    29B6h
		call    esi
		mov     ecx, [edi]
		push    28h
		push    10931
		call    esi
		mov     ecx, [edi]
		push    29h
		push    10790
		call    esi
		mov     ecx, [edi]
		push    2Ah
		push    2A27h
		call    esi
		mov     ecx, [edi]
		push    2Bh
		push    2A28h
		call    esi
		mov     ecx, [edi]
		push    2Ch
		push    2A29h
		call    esi
		mov     ecx, [edi]
		push    2Dh
		push    2A2Ah
		call    esi
		mov     ecx, [edi]
		push    2Eh
		push    2A2Bh
		call    esi
		mov     ecx, [edi]
		push    2Fh
		push    2A2Ch
		call    esi
		mov     ecx, [edi]
		push    30h
		push    10923
		call    esi
		mov     ecx, [edi]
		push    31h
		push    2A2Fh
		call    esi
		mov     ecx, [edi]
		push    32h
		push    2A30h
		call    esi
		mov     ecx, [edi]
		push    33h
		push    29B7h
		call    esi
		mov     ecx, [edi]
		push    34h
		push    29B8h
		call    esi
		//////////////////
		mov     ecx, [edi]
		push    55
		push    1h
		call    esi
		//mov     ecx, [edi]
		//push    36h
		//push    2h
		//call    esi
		//////////////
		pop     esi
		//mov     ecx, [edi]
		//push    35h
		//push    1389h
		//call    terrFunc1
		push    52A388h
		ret
	}
}

__declspec(naked) int terrain2 ()
{
	__asm
	{
		//mov     ecx, ebx
		//push    0
		//push    297Dh
		//call    terrainFunc2
		//push    53B53Bh
		//ret
		//mov     ecx, ebx
		//push    1
		//push    2980h
		//call    terrainFunc2
		push    esi
		mov     esi, 53AB20h
		mov     ecx, ebx
		push    2
		push    10926
		call    esi
		mov     ecx, ebx
		push    3
		push    2993h
		call    esi
		//mov     ecx, ebx
		//push    4
		//push    2983h
		//call    terrainFunc2
		mov     ecx, ebx
		push    5
		push    299Ah
		call    esi
		//mov     ecx, ebx
		//push    6
		//push    10933
		//call    terrainFunc2
		mov     ecx, ebx
		push    7
		push    10920
		call    esi
		mov     ecx, ebx
		push    8
		push    10921
		call    esi
		//mov     ecx, ebx
		//push    9
		//push    2991h
		//call    terrainFunc2
		//mov     ecx, ebx
		//push    0Ah
		//push    297Fh
		//call    terrainFunc2
		mov     ecx, ebx
		push    0Bh
		push    2992h
		call    esi
		//mov     ecx, ebx
		////push    0Ch
		//push    2990h
		//call    terrainFunc2
		//mov     ecx, ebx
		//push    0Dh
		//push    2981h
		//call    terrainFunc2
		mov     ecx, ebx
		push    0Eh
		push    2998h
		call    esi
		mov     ecx, ebx
		push    0Fh
		push    2A2Dh
		call    esi
		mov     ecx, ebx
		push    10h
		push    2A2Eh
		call    esi
		mov     ecx, ebx
		push    11h
		push    29A8h
		call    esi
		mov     ecx, ebx
		push    12h
		push    2999h
		call    esi
		//mov     ecx, ebx
		//push    13h
		//push    2984h
		//call    terrainFunc2
		//mov     ecx, ebx
		//push    14h
		//push    2982h
		//call    terrainFunc2
		mov     ecx, ebx
		push    15h
		push    29A9h
		call    esi
		//mov     ecx, ebx
		//push    16h
		//push    2985h
		//call    terrainFunc2
		//mov     ecx, ebx
		//push    17h
		//push    2994h
		//call    terrainFunc2
		//mov     ecx, ebx
		//push    18h
		//push    2995h
		//call    terrainFunc2
		mov     ecx, ebx
		push    19h
		push    2996h
		call    esi
		mov     ecx, ebx
		push    1Ah
		push    299Bh
		call    esi
		mov     ecx, ebx
		push    1Bh
		push    10927
		call    esi
		mov     ecx, ebx
		push    1Ch
		push    10929
		call    esi
		mov     ecx, ebx
		push    1Dh
		push    10922
		call    esi
		mov     ecx, ebx
		push    1Eh
		push    10925
		call    esi
		mov     ecx, ebx
		push    1Fh
		push    10924
		call    esi
		mov     ecx, ebx
		push    20h
		push    29AAh
		call    esi
		mov     ecx, ebx
		push    21h
		push    10668
		call    esi
		mov     ecx, ebx
		push    22h
		push    29ABh
		call    esi
		mov     ecx, ebx
		push    23h
		push    29ADh
		call    esi
		mov     ecx, ebx
		push    24h
		push    10928
		call    esi
		mov     ecx, ebx
		push    25h
		push    10930
		call    esi
		mov     ecx, ebx
		push    26h
		push    29B5h
		call    esi
		mov     ecx, ebx
		push    27h
		push    29B6h
		call    esi
		mov     ecx, ebx
		push    28h
		push    10931
		call    esi
		mov     ecx, ebx
		push    29h
		push    10790
		call    esi
		mov     ecx, ebx
		push    2Ah
		push    2A27h
		call    esi
		mov     ecx, ebx
		push    2Bh
		push    2A28h
		call    esi
		mov     ecx, ebx
		push    2Ch
		push    2A29h
		call    esi
		mov     ecx, ebx
		push    2Dh
		push    2A2Ah
		call    esi
		mov     ecx, ebx
		push    2Eh
		push    2A2Bh
		call    esi
		mov     ecx, ebx
		push    2Fh
		push    2A2Ch
		call    esi
		mov     ecx, ebx
		push    30h
		push    10923
		call    esi
		mov     ecx, ebx
		push    31h
		push    2A2Fh
		call    esi
		mov     ecx, ebx
		push    32h
		push    2A30h
		call    esi
		mov     ecx, ebx
		push    33h
		push    29B7h
		call    esi
		mov     ecx, ebx
		push    34h
		push    29B8h
		call    esi
		//////////
		mov     ecx, ebx
		push    55
		push    1h
		call    esi
		//mov     ecx, ebx
		//push    36h
		//push    2h
		//call    esi
			///////////
		pop     esi
		//mov     ecx, ebx
		//push    35h
		//push    1389h
		//call    terrainFunc2

		/*mov     ecx, ebx
		push    37h
		push    13
		call    terrainFunc2*/

		push    53B53Bh
		ret
	}
}

void setTerrainLoadHooks ()
{
#ifdef _DEBUG
	log ("Setting terrain list load hooks...");
#endif

	setHook ((void*)0x0052A30A, &terrain1);
	setHook ((void*)0x0053B3C1, &terrain2);
}
