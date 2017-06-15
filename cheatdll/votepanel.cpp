#include "stdafx.h"

#include "votepanel.h"

//votes:
//0 - wait
//1 - nonexistent
//2 - continue
//3 - save
//4 - dropped
//5 - waiting (?)

__declspec(naked) void getGameCanContinue_fix () //0046F030
{
	__asm
	{
		push    ecx
		push    ebx
		//first: count players
		xor     edi, edi
		xor     ecx, ecx
		xor     eax, eax
count_players:
		mov     ebx, dword ptr [edx]
		cmp     ebx, 5
		jz      not_enough_votes  //waiting for vote: don't continue
		cmp     ebx, 1   //only count if exists
		setnz   al
		add     edi, eax
		cmp     ebx, 4   //don't count if dropped
		setz    al
		sub     edi, eax
		cmp     ebx, 2   //voted continue
		setz    al
		add     ecx, eax
		inc     esi
		add     edx, 4
		cmp     esi, 9
		jl      count_players
		//edi: players count
		//ecx: voted continue
		xor     eax, eax
		shr     edi, 1
		adc     edi, eax
		//edi - votes required
		cmp     ecx, edi
		jl      not_enough_votes
		//can continue
		pop     ebx
		pop     ecx
		push    0046F055h
		ret
not_enough_votes:
		pop     ebx
		pop     ecx
		push    0046F1AEh
		ret
	}
}

__declspec(naked) void getGameCanSaveAndExit_fix () //0046F1CE
{
	__asm
	{
		push    edi
		push    ebx
		push    esi
		//first: see if can continue
		xor     esi, esi
		xor     edi, edi
		xor     eax, eax
count_players2:
		mov     ebx, dword ptr [ecx]
		cmp     ebx, 5
		jz      not_enough_votes2  //waiting for vote: don't continue
		cmp     ebx, 1   //only count if exists
		setnz   al
		add     edi, eax
		cmp     ebx, 4   //don't count if dropped
		setz    al
		sub     edi, eax
		cmp     ebx, 2   //voted continue
		setz    al
		add     esi, eax
		inc     edx
		add     ecx, 4
		cmp     edx, 9
		jl      count_players2
		//edi: players count
		//esi: voted continue
		xor     eax, eax
		shr     edi, 1
		adc     edi, eax
		//edi - votes required
		cmp     esi, edi
		jl      not_enough_votes2
		//can continue - can't save
		pop     esi
		pop     ebx
		pop     edi
		push    0046F231h
		ret
not_enough_votes2:

		pop     esi
		pop     ebx

		lea     ecx, [ebx+8ECh]
		mov     edx, 1

		push    ebx
		push    esi

		//can't continue: try save
		xor     edi, edi
		xor     eax, eax
		xor     esi, esi
count_players3:
		mov     ebx, dword ptr [ecx]
		cmp     ebx, 5
		jz      not_enough_votes3  //waiting for vote: don't continue
		cmp     ebx, 1   //only count if exists
		setnz   al
		add     edi, eax
		cmp     ebx, 4   //don't count if dropped
		setz    al
		sub     edi, eax
		cmp     ebx, 3   //voted save
		setz    al
		add     esi, eax
		inc     edx
		add     ecx, 4
		cmp     edx, 9
		jl      count_players3
		//edi: players count
		//esi: voted save
		xor     eax, eax
		shr     edi, 1
		adc     edi, eax
		//edi - votes required
		cmp     esi, edi
		jl      not_enough_votes3

		//can save
		pop     esi
		pop     ebx
		pop     edi
		push    0046F1E7h
		ret
not_enough_votes3:
		pop     esi
		pop     ebx
		pop     edi
		push    0046F231h
		ret
	}
}

__declspec(naked) void statusCheck_fix () //0046EC0E
{
	__asm
	{
		xor     esi, esi
		//inc     esi
_check_status:
		mov     ecx, 006A35E0h
		mov     ecx, [ecx]
		push    esi
		mov     eax, 43AF40h
		call    eax
		cmp     eax, 2
		setz    al
		and     eax, 1
		adc     edi, eax
		inc     esi
		cmp     esi, 9
		jl      _check_status
		push    0046EC28h
		ret
	}
}

void setVotePanelHooks ()
{
	setHook ((void*)0x0046F030, &getGameCanContinue_fix);
	setHook ((void*)0x0046F1CE, &getGameCanSaveAndExit_fix);

	setHook ((void*)0x0046EC0E, &statusCheck_fix);

	setByte (0x0046E072, 9);
	//setByte (0x0043AF44, 0x40);
}
