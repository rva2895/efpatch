#include "stdafx.h"
#include "aiunitcount.h"

extern int current_save_game_version;

__declspec(naked) void ctr1() //004C2B7B
{
    __asm
    {
        //mov     bp, [esi+ebp*2+1038h]
        mov     eax, [esi + 1038h]
        mov     bp, [eax + ebp * 2]
        mov     eax, 004C2B83h
        jmp     eax
    }
}

__declspec(naked) void ctr2() //004C43EA
{
    __asm
    {
        //add     [ecx+eax*2+1038h], si
        mov     esi, [ecx + 1038h]
        lea     eax, [esi + eax * 2]
        mov     esi, [esp + 18h]
        add     word ptr [eax], si
        mov     eax, 004C43F6h
        jmp     eax
    }
}

__declspec(naked) void ctr3() //00576D60
{
    __asm
    {
        //cmp     word ptr [ecx+edi*2+1038h], 1
        mov     esi, [ecx + 1038h]
        cmp     word ptr [esi + edi * 2], 1
        mov     esi, 00576D69h
        jmp     esi
    }
}

__declspec(naked) void ctr4() //00576E7A
{
    __asm
    {
        //cmp     word ptr [ecx+esi*2+1038h], 1
        mov     ebx, [ecx + 1038h]
        cmp     word ptr [ebx + esi * 2], 1
        mov     ebx, 00576E83h
        jmp     ebx
    }
}

__declspec(naked) void ctr5() //0057AF02
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+1038h]
        mov     edx, [ecx + 1038h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     ecx, 0057AF0Ah
        jmp     ecx
    }
}

__declspec(naked) void ctr6() //0057AF30
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+1038h]
        mov     esi, [eax + 1038h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057AF38h
        jmp     eax
    }
}

__declspec(naked) void ctr7() //0057BA0A
{
    __asm
    {
        //movsx   ecx, word ptr [edi+eax*2+1038h]
        mov     ecx, [edi + 1038h]
        movsx   ecx, word ptr [ecx + eax * 2]
        add     esi, ecx
        mov     ecx, 0057BA14h
        jmp     ecx
    }
}

__declspec(naked) void ctr8() //0057BA27
{
    __asm
    {
        //movsx   esi, word ptr [edi+edx*2+1038h]
        mov     esi, [edi + 1038h]
        movsx   esi, word ptr [esi + edx * 2]
        mov     eax, 0057BA2Fh
        jmp     eax
    }
}

__declspec(naked) void ctr9() //0057BD9C
{
    __asm
    {
        //movsx   ecx, word ptr [edi+eax*2+1038h]
        mov     ecx, [edi + 1038h]
        movsx   ecx, word ptr [ecx + eax * 2]
        add     esi, ecx
        mov     ecx, 0057BDA6h
        jmp     ecx
    }
}

__declspec(naked) void ctr10() //0057BDB9
{
    __asm
    {
        //movsx   esi, word ptr [edi+edx*2+1038h]
        mov     esi, [edi + 1038h]
        movsx   esi, word ptr [esi + edx * 2]
        mov     eax, 0057BDC1h
        jmp     eax
    }
}

__declspec(naked) void ctr11() //0057CE91
{
    __asm
    {
        //movsx   eax, word ptr [ebp+esi*2+1038h]
        mov     eax, [ebp + 1038h]
        movsx   eax, word ptr [eax + esi * 2]
        mov     ecx, 0057CE99h
        jmp     ecx
    }
}

__declspec(naked) void ctr12() //0057CEC5
{
    __asm
    {
        //movsx   edi, word ptr [ebp+esi*2+1038h]
        mov     edi, [ebp + 1038h]
        movsx   edi, word ptr [edi + esi * 2]
        mov     eax, 0057CECDh
        jmp     eax
    }
}

__declspec(naked) void ctr13() //0057DB28
{
    __asm
    {
        //movsx   eax, word ptr [ebp+esi*2+1038h]
        mov     eax, [ebp + 1038h]
        movsx   eax, word ptr [eax + esi * 2]
        mov     ecx, 0057DB30h
        jmp     ecx
    }
}

__declspec(naked) void ctr14() //0057DB5A
{
    __asm
    {
        //movsx   edi, word ptr [ebp+esi*2+1038h]
        mov     edi, [ebp + 1038h]
        movsx   edi, word ptr [edi + esi * 2]
        mov     eax, 0057DB62h
        jmp     eax
    }
}

__declspec(naked) void ctr15() //0057E5C9
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+1038h]
        mov     edx, [ecx + 1038h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     eax, 0057E5D1h
        jmp     eax
    }
}

__declspec(naked) void ctr16() //0057E5EA
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+1038h]
        mov     esi, [eax + 1038h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057E5F2h
        jmp     eax
    }
}

__declspec(naked) void ctr17() //005B2065
{
    __asm
    {
        //cmp     word ptr [ecx+10DCh], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0A4h], 0
        mov     edx, 005B206Dh
        jmp     edx
    }
}

__declspec(naked) void ctr18() //005B208B
{
    __asm
    {
        //cmp     word ptr [ecx+1112h], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0DAh], 0
        mov     edx, 005B2093h
        jmp     edx
    }
}

__declspec(naked) void ctr19() //005B20B1
{
    __asm
    {
        //cmp     word ptr [ecx+1CCCh], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0C94h], 0
        mov     edx, 005B20B9h
        jmp     edx
    }
}

__declspec(naked) void ctr20() //005B20CD
{
    __asm
    {
        //cmp     word ptr [ecx+1CCAh], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0C92h], 0
        mov     edx, 005B20D5h
        jmp     edx
    }
}

//score counters

//turrets
__declspec(naked) void ctr_score_combat_1() //004FF815
{
    __asm
    {
        //movsx   eax, word ptr [ecx+1CCCh]
        //movsx   ecx, word ptr [ecx+1CCAh]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0C94h] //class 10
        movsx   ecx, word ptr [ecx + 0C92h] //class 9
        add     eax, ecx
        mov     ecx, 004FF825h
        jmp     ecx
    }
}

//ships
__declspec(naked) void ctr_score_combat_2() //004FF898
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1CDAh]
        //movsx   edx, word ptr [ecx + 1CD8h]
        //movsx   edx, word ptr [ecx + 1CD6h]
        //movsx   edx, word ptr [ecx + 1CD2h]
        //movsx   ecx, word ptr [ecx + 1CCEh]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CA2h] //class 17
        movsx   edx, word ptr [ecx + 0CA0h] //class 16
        add     eax, edx
        movsx   edx, word ptr [ecx + 0C9Eh] //class 15
        add     eax, edx
        movsx   edx, word ptr [ecx + 0C9Ah] //class 13
        add     eax, edx
        movsx   edx, word ptr [ecx + 0C96h] //class 11
        add     eax, edx
        mov     ecx, 004FF8C3h
        jmp     ecx
    }
}

//hvy weapons
__declspec(naked) void ctr_score_combat_3() //004FF93A
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D00h]
        //movsx   edx, word ptr [ecx + 1CFEh]
        //movsx   edx, word ptr [ecx + 1CFCh]
        //movsx   edx, word ptr [ecx + 1CFAh]
        //movsx   edx, word ptr [ecx + 1CF8h]
        //movsx   ecx, word ptr [ecx + 19E6h]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CC8h] //class 36
        movsx   edx, word ptr [ecx + 0CC6h] //class 35
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CC4h] //class 34
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CC2h] //class 33
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CC0h] //class 32
        add     eax, edx
        movsx   edx, word ptr [ecx + 09AEh] //id 1239, explosive droid
        add     eax, edx
        mov     ecx, 004FF96Eh
        jmp     ecx
    }
}

//submarines
__declspec(naked) void ctr_score_combat_4() //004FF9E5
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D08h]
        //movsx   edx, word ptr [ecx + 1D06h]
        //movsx   edx, word ptr [ecx + 1D04h]
        //movsx   ecx, word ptr [ecx + 1D02h]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CD0h] //class 40
        movsx   edx, word ptr [ecx + 0CCEh] //class 39
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CCCh] //class 38
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CCAh] //class 37
        add     eax, edx
        mov     ecx, 004FFA07h
        jmp     ecx
    }
}

//aircraft
__declspec(naked) void ctr_score_combat_5() //004FFA7E
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D38h]
        //movsx   edx, word ptr [ecx + 1D36h]
        //movsx   edx, word ptr [ecx + 1D34h]
        //movsx   edx, word ptr [ecx + 1D2Eh]
        //movsx   edx, word ptr [ecx + 1D18h]
        //movsx   ecx, word ptr [ecx + 1D0Eh]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0D00h] //class 64
        movsx   edx, word ptr [ecx + 0CFEh] //class 63
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CFCh] //class 62
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CF6h] //class 59
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CE0h] //class 48
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CD6h] //class 43
        add     eax, edx
        mov     ecx, 004FFAB2h
        jmp     ecx
    }
}

//mechs
__declspec(naked) void ctr_score_combat_6() //004FFB29
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D22h]
        //movsx   ecx, word ptr [ecx + 1D16h]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CEAh] //class 53
        movsx   edx, word ptr [ecx + 0CDEh] //class 47
        add     eax, edx
        mov     ecx, 004FFB39h
        jmp     ecx
    }
}

//troopers
__declspec(naked) void ctr_score_combat_7() //004FFBB0
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D28h]
        //movsx   edx, word ptr [ecx + 1D26h]
        //movsx   edx, word ptr [ecx + 1D20h]
        //movsx   ecx, word ptr [ecx + 1D1Ah]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CF0h] //class 56
        movsx   edx, word ptr [ecx + 0CEEh] //class 55
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CE8h] //class 52
        add     eax, edx
        movsx   edx, word ptr [ecx + 0CE2h] //class 49
        add     eax, edx
        mov     ecx, 004FFBD2h
        jmp     ecx
    }
}

//bounty hunters
__declspec(naked) void ctr_score_combat_8() //004FFC49
{
    __asm
    {
        //movsx   eax, word ptr [eax + 1D10h]
        mov     ecx, [eax + 1038h]
        movsx   eax, word ptr [ecx + 0CD8h] //class 44
        mov     ecx, 004FFC50h
        jmp     ecx
    }
}

//jedi
__declspec(naked) void ctr_score_combat_9() //004FFCC7
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D1Eh]
        //movsx   ecx, word ptr [ecx + 1D1Ch]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CE6h] //class 51
        movsx   edx, word ptr [ecx + 0CE4h] //class 50
        add     eax, edx
        mov     ecx, 004FFCD7h
        jmp     ecx
    }
}

//medics
__declspec(naked) void ctr_score_combat_10() //004FFD4E
{
    __asm
    {
        //movsx   eax, word ptr [eax + 1D24h]
        mov     ecx, [eax + 1038h]
        movsx   eax, word ptr [ecx + 0CECh] //class 54
        mov     ecx, 004FFD55h
        jmp     ecx
    }
}

//workers
__declspec(naked) void ctr_score_combat_11() //004FFDCC
{
    __asm
    {
        //movsx   eax, word ptr [ecx + 1D2Ch]
        //movsx   edx, word ptr [ecx + 1D12h]
        //movsx   ecx, word ptr [ecx + 1CD4h]
        mov     ecx, [ecx + 1038h]
        movsx   eax, word ptr [ecx + 0CF4h] //class 58
        movsx   edx, word ptr [ecx + 0CDAh] //class 45
        add     eax, edx
        movsx   edx, word ptr [ecx + 0C9Ch] //class 14
        add     eax, edx
        mov     ecx, 004FFDE5h
        jmp     ecx
    }
}

//mobile shields
__declspec(naked) void ctr_score_combat_12() //004FFE5C
{
    __asm
    {
        //movsx   eax, word ptr [eax + 1D2Ah]
        mov     ecx, [eax + 1038h]
        movsx   eax, word ptr [ecx + 0CF2h] //class 57
        mov     ecx, 004FFE63h
        jmp     ecx
    }
}

__declspec(naked) void ctr1_class_count() //004C440D
{
    __asm
    {
        //add     [ecx + eax * 2 + 1CB8h], si
        mov     edi, [ecx + 1038h]
        add     [edi + eax * 2 + 1600 * 2], si
        mov     edi, 004C4415h
        jmp     edi
    }
}

//ctr 2
_declspec(naked) void ctr2_monument_1() //004FEEBE
{
    __asm
    {
        //movsx   eax, word ptr [edx + 518h]
        mov     eax, [edx + 2F0h]
        movsx   eax, word ptr [eax + 276 * 2]
        mov     edx, 004FEEC5h
        jmp     edx
    }
}

_declspec(naked) void ctr2_monument_2() //004FEE35
{
    __asm
    {
        //movsx   eax, word ptr [edx + 518h]
        mov     eax, [edx + 2F0h]
        movsx   eax, word ptr [eax + 276 * 2]
        mov     edx, 004FEE3Ch
        jmp     edx
    }
}

__declspec(naked) void ctr2_class_count() //004C42FD
{
    __asm
    {
        //add     [ecx + eax * 2 + 0F70h], si
        mov     edi, [ecx + 2F0h]
        add     [edi + eax * 2 + 1600 * 2], si
        mov     edi, 004C4305h
        jmp     edi
    }
}

__declspec(naked) void ctr2_1() //004C2B5C
{
    __asm
    {
        //mov     bx, [esi+ebp*2+2F0h]
        mov     ecx, [esi + 2F0h]
        mov     bx, [ecx + ebp * 2]
        mov     ecx, 004C2B64h
        jmp     ecx
    }
}

__declspec(naked) void ctr2_2() //004C42DA
{
    __asm
    {
        //add     [ecx+eax*2+2F0h], si
        mov     esi, [ecx + 2F0h]
        lea     eax, [esi + eax * 2]
        mov     esi, [esp + 18h]
        add     word ptr [eax], si
        mov     eax, 004C42E6h
        jmp     eax
    }
}

__declspec(naked) void ctr2_3() //0057B022
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+2F0h]
        mov     edx, [ecx + 2F0h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     ecx, 0057B02Ah
        jmp     ecx
    }
}

__declspec(naked) void ctr2_4() //0057B050
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+2F0h]
        mov     esi, [eax + 2F0h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057B058h
        jmp     eax
    }
}

__declspec(naked) void ctr2_5() //0057BB2A
{
    __asm
    {
        //movsx   ecx, word ptr [edi+eax*2+2F0h]
        mov     ecx, [edi + 2F0h]
        movsx   ecx, word ptr [ecx + eax * 2]
        add     esi, ecx
        mov     ecx, 0057BB34h
        jmp     ecx
    }
}

__declspec(naked) void ctr2_6() //0057BB47
{
    __asm
    {
        //movsx   esi, word ptr [edi+edx*2+2F0h]
        mov     esi, [edi + 2F0h]
        movsx   esi, word ptr [esi + edx * 2]
        mov     eax, 0057BB4Fh
        jmp     eax
    }
}

__declspec(naked) void ctr2_7() //0057E739
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+2F0h]
        mov     edx, [ecx + 2F0h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     ecx, 0057E741h
        jmp     ecx
    }
}

__declspec(naked) void ctr2_8() //0057E75A
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+2F0h]
        mov     esi, [eax + 2F0h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057E762h
        jmp     eax
    }
}

__declspec(naked) void ctr2_9() //00596695
{
    __asm
    {
        mov     ecx, [ecx + 2F0h]
        cmp     word ptr [ecx + ebp * 2], ax
        mov     ecx, 0059669Dh
        jmp     ecx
    }
}

__declspec(naked) void ctr2_10() //005CEDE4
{
    __asm
    {
        mov     eax, [ebp + 2F0h]
        cmp     word ptr [eax + 434 * 2], 0
        mov     ecx, 005CEDECh
        jmp     ecx
    }
}

__declspec(naked) void ctr2_unit_type_count_total_workers() //0057E69D
{
    __asm
    {
        //movsx   ecx, word ptr [eax+0FE4h]
        mov     ecx, [eax + 2F0h]
        movsx   ecx, word ptr [ecx + 1658 * 2]
        mov     eax, 0057E6A4h
        jmp     eax
    }
}

__declspec(naked) void ctr1_unit_type_count_workers() //0057E52D
{
    __asm
    {
        //movsx   ecx, word ptr [eax+1D2Ch]
        mov     ecx, [eax + 1038h]
        movsx   ecx, word ptr [ecx + 1658 * 2]
        mov     eax, 0057E534h
        jmp     eax
    }
}

#define UNIT_COUNT_FOR_COUNTERS 0x2000

void __stdcall setup_player_counters(void* player)
{
    uint8_t* builders_per_type = (uint8_t*)malloc(UNIT_COUNT_FOR_COUNTERS);
    memset(builders_per_type, 1, UNIT_COUNT_FOR_COUNTERS);
    *(uint8_t**)((uint8_t*)player + 0x103C) = builders_per_type;

    int16_t* counter_1038 = (int16_t*)malloc(UNIT_COUNT_FOR_COUNTERS * sizeof(int16_t));
    memset(counter_1038, 0, UNIT_COUNT_FOR_COUNTERS * sizeof(int16_t));
    *(int16_t**)((uint8_t*)player + 0x1038) = counter_1038 + 0x100;

    int16_t* counter_2F0 = (int16_t*)malloc(UNIT_COUNT_FOR_COUNTERS * sizeof(int16_t));
    memset(counter_2F0, 0, UNIT_COUNT_FOR_COUNTERS * sizeof(int16_t));
    *(int16_t**)((uint8_t*)player + 0x2F0) = counter_2F0 + 0x100;
}

uint8_t* __stdcall get_player_counter_builders_per_type(void* player)
{
    return *(uint8_t**)((uint8_t*)player + 0x103C);
}

int16_t* __stdcall get_player_counter_1038(void* player)
{
    return *(int16_t**)((uint8_t*)player + 0x1038);
}

int16_t* __stdcall get_player_counter_2F0(void* player)
{
    return *(int16_t**)((uint8_t*)player + 0x2F0);
}

void __stdcall read_player_counters(void* player, int infile)
{
    int unit_count;
    rge_read(infile, &unit_count, sizeof(unit_count));

    setup_player_counters(player);

    if (current_save_game_version >= 1) //new loader
    {
        rge_read(infile, get_player_counter_2F0(player) - 0x100, unit_count * sizeof(int16_t));
        rge_read(infile, get_player_counter_1038(player) - 0x100, unit_count * sizeof(int16_t));
        /*int n_specified_builders_per_type;
        rge_read(infile, &n_specified_builders_per_type, sizeof(n_specified_builders_per_type));
        for (int i = 0; i < n_specified_builders_per_type; i++)
        {
            int16_t unit_id;
            uint8_t builders;
            rge_read(infile, &unit_id, sizeof(unit_id));
            rge_read(infile, &builders, sizeof(builders));
            get_player_counter_builders_per_type(player)[unit_id] = builders;
        }*/
        rge_read(infile, get_player_counter_builders_per_type(player), unit_count);
    }
    else //old loader
    {
        rge_read(infile, get_player_counter_2F0(player) - 0x80, unit_count * sizeof(int16_t));
        rge_read(infile, (uint8_t*)get_player_counter_2F0(player) + 1600 * 2, 0xC8);
        rge_read(infile, get_player_counter_1038(player) - 0x80, unit_count * sizeof(int16_t));
        rge_read(infile, get_player_counter_builders_per_type(player), unit_count);
        //char dummy[0xC8];
        //rge_read(infile, dummy, 0xc8);
        rge_read(infile, (uint8_t*)get_player_counter_1038(player) + 1600 * 2, 0xC8);
    }
}

void __stdcall write_player_counters(void* player, int outfile)
{
    int unit_count = UNIT_COUNT_FOR_COUNTERS;
    rge_write(outfile, &unit_count, sizeof(unit_count));

    rge_write(outfile, get_player_counter_2F0(player) - 0x100, UNIT_COUNT_FOR_COUNTERS * sizeof(int16_t));
    rge_write(outfile, get_player_counter_1038(player) - 0x100, UNIT_COUNT_FOR_COUNTERS * sizeof(int16_t));
    rge_write(outfile, get_player_counter_builders_per_type(player), UNIT_COUNT_FOR_COUNTERS);
}

void __stdcall delete_player_counters(void* player)
{
    uint8_t* builders_per_type = get_player_counter_builders_per_type(player);
    free(builders_per_type);
    int16_t* counter_1038 = get_player_counter_1038(player);
    if (counter_1038)
        free(counter_1038 - 0x100);
    int16_t* counter_2F0 = get_player_counter_2F0(player);
    if (counter_2F0)
        free(counter_2F0 - 0x100);
}

__declspec(naked) void ctrInit_v2() //004BFF07
{
    __asm
    {
        lea     edi, [ebp + 1CB8h]
        rep stosd

        push    ebp
        call    setup_player_counters

        mov     eax, 004BFF0Fh
        jmp     eax
    }
}

__declspec(naked) void ctrReadSave_v2() //004BF665
{
    __asm
    {
        push    ebp
        push    esi
        call    read_player_counters
        mov     eax, 004BF6ADh
        jmp     eax
    }
}

__declspec(naked) void ctrWriteSave_v2() //004C1C55
{
    __asm
    {
        push    esi
        push    ebx
        call    write_player_counters
        mov     eax, 004C1C9Dh
        jmp     eax
    }
}

__declspec(naked) void ctrFree_v2() //004C0157
{
    __asm
    {
        push    esi
        call    delete_player_counters
        mov     esi, [esi + 0C4h]
        mov     eax, 004C015Dh
        jmp     eax
    }
}

__declspec(naked) void ctr1PrefabFix() //0057AF3F
{
    __asm
    {
        mov     esi, [eax + 1038h]
        movsx   ecx, word ptr [esi + 8Ch]  //prefab count
        movsx   esi, word ptr [esi + 7D2h] //underwater prefab count
        add     esi, ecx
        mov     eax, 0057AF4Fh
        jmp     eax
    }
}

__declspec(naked) void ctr2PrefabFix() //0057B05F
{
    __asm
    {
        mov     esi, [eax + 2F0h]
        movsx   ecx, word ptr [esi + 8Ch]  //prefab count
        movsx   esi, word ptr [esi + 7D2h] //underwater prefab count
        //add     esi, ecx
        mov     eax, 0057B06Dh
        jmp     eax
    }
}

__declspec(naked) void ctr2CCFix() //004C0745
{
    __asm
    {
        //cmp     word ptr [ecx+3CAh], 0
        mov     ecx, [ecx + 2F0h]
        cmp     word ptr [ecx + 0DAh], 0
        mov     esi, 004C074Dh
        jmp     esi
    }
}

void __stdcall setup_info_ai_counters(void* info_ai)
{
    int32_t* counter = (int32_t*)malloc(UNIT_COUNT_FOR_COUNTERS * sizeof(int32_t));
    memset(counter, 0, UNIT_COUNT_FOR_COUNTERS * sizeof(int32_t));
    *(int32_t**)((uint8_t*)info_ai + 0x3D8) = counter;
}

int32_t* __stdcall get_info_ai_counter(void* info_ai)
{
    return *(int32_t**)((uint8_t*)info_ai + 0x3D8);
}

void __stdcall read_info_ai_counters(void* info_ai, int infile)
{
    int unit_count;
    rge_read(infile, &unit_count, sizeof(unit_count));

    setup_info_ai_counters(info_ai);

    rge_read(infile, get_info_ai_counter(info_ai), unit_count * sizeof(int32_t));
}

void __stdcall write_info_ai_counters(void* info_ai, int outfile)
{
    int unit_count = UNIT_COUNT_FOR_COUNTERS;
    rge_write(outfile, &unit_count, sizeof(unit_count));

    rge_write(outfile, get_info_ai_counter(info_ai), UNIT_COUNT_FOR_COUNTERS * sizeof(int32_t));
}

void __stdcall delete_info_ai_counters(void* info_ai)
{
    int32_t* counter = get_info_ai_counter(info_ai);
    free(counter);
}

__declspec(naked) void infoAiCntrInit_v2() //0058397A
{
    __asm
    {
        push    esi
        call    setup_info_ai_counters
        or      eax, 0FFFFFFFFh
        mov     [esi + 1CD8h], ebx
        mov     edx, 4
        mov     ecx, 00583983h
        jmp     ecx
    }
}

__declspec(naked) void infoAiCntrReadSave_v2() //00584927
{
    __asm
    {
        push    ebp
        push    esi
        call    read_info_ai_counters
        mov     eax, 00584943h
        jmp     eax
    }
}

__declspec(naked) void infoAiCntrWriteSave_v2() //005865C2
{
    __asm
    {
        push    esi
        push    ebx
        call    write_info_ai_counters
        mov     eax, 005865DEh
        jmp     eax
    }
}

__declspec(naked) void infoAiCntrFree_v2() //005854B7
{
    __asm
    {
        push    esi
        call    delete_info_ai_counters
        mov     ecx, [esp + 20h]
        pop     edi
        pop     esi
        mov     eax, 005854BDh
        jmp     eax
    }
}

__declspec(naked) void infoAiCntr_1() //0058A87B
{
    __asm
    {
        mov     ecx, [esi + 3D8h]
        lea     eax, [ecx + edx * 4]
        inc     dword ptr [eax]
        mov     ebx, 0058A88Ch
        jmp     ebx
    }
}

__declspec(naked) void infoAiCntr_2() //0058AA0C
{
    __asm
    {
        mov     ecx, [esi + 3D8h]
        lea     eax, [ecx + edx * 4]
        dec     dword ptr [eax]
        mov     eax, 0058AA1Dh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setAIUnitCountHooks()
{
    setHook((void*)0x004C2B7B, ctr1);
    setHook((void*)0x004C43EA, ctr2);
    setHook((void*)0x00576D60, ctr3);
    setHook((void*)0x00576E7A, ctr4);
    setHook((void*)0x0057AF02, ctr5);
    setHook((void*)0x0057AF30, ctr6);
    setHook((void*)0x0057BA0A, ctr7);
    setHook((void*)0x0057BA27, ctr8);
    setHook((void*)0x0057BD9C, ctr9);
    setHook((void*)0x0057BDB9, ctr10);
    setHook((void*)0x0057CE91, ctr11);
    setHook((void*)0x0057CEC5, ctr12);
    setHook((void*)0x0057DB28, ctr13);
    setHook((void*)0x0057DB5A, ctr14);
    setHook((void*)0x0057E5C9, ctr15);
    setHook((void*)0x0057E5EA, ctr16);

    setHook((void*)0x005B2065, ctr17);
    setHook((void*)0x005B208B, ctr18);
    setHook((void*)0x005B20B1, ctr19);
    setHook((void*)0x005B20CD, ctr20);

    setHook((void*)0x004FF815, ctr_score_combat_1);
    setHook((void*)0x004FF898, ctr_score_combat_2);
    setHook((void*)0x004FF93A, ctr_score_combat_3);
    setHook((void*)0x004FF9E5, ctr_score_combat_4);
    setHook((void*)0x004FFA7E, ctr_score_combat_5);
    setHook((void*)0x004FFB29, ctr_score_combat_6);
    setHook((void*)0x004FFBB0, ctr_score_combat_7);
    setHook((void*)0x004FFC49, ctr_score_combat_8);
    setHook((void*)0x004FFCC7, ctr_score_combat_9);
    setHook((void*)0x004FFD4E, ctr_score_combat_10);
    setHook((void*)0x004FFDCC, ctr_score_combat_11);
    setHook((void*)0x004FFE5C, ctr_score_combat_12);
    setHook((void*)0x004C440D, ctr1_class_count);

    setHook((void*)0x004FEEBE, ctr2_monument_1);
    setHook((void*)0x004FEE35, ctr2_monument_2);
    setHook((void*)0x004C42FD, ctr2_class_count);

    setHook((void*)0x004C2B5C, ctr2_1);
    setHook((void*)0x004C42DA, ctr2_2);
    setHook((void*)0x0057B022, ctr2_3);
    setHook((void*)0x0057B050, ctr2_4);
    setHook((void*)0x0057BB2A, ctr2_5);
    setHook((void*)0x0057BB47, ctr2_6);
    setHook((void*)0x0057E739, ctr2_7);
    setHook((void*)0x0057E75A, ctr2_8);
    setHook((void*)0x00596695, ctr2_9);
    setHook((void*)0x005CEDE4, ctr2_10);

    setHook((void*)0x004BFF07, ctrInit_v2);
    setHook((void*)0x004BF665, ctrReadSave_v2);
    setHook((void*)0x004C1C55, ctrWriteSave_v2);
    setHook((void*)0x004C0157, ctrFree_v2);

    setHook((void*)0x0057AF3F, ctr1PrefabFix);
    setHook((void*)0x0057B05F, ctr2PrefabFix);

    setHook((void*)0x004C0745, ctr2CCFix);

    setHook((void*)0x0058A87B, infoAiCntr_1);
    setHook((void*)0x0058AA0C, infoAiCntr_2);

    setHook((void*)0x0058397A, infoAiCntrInit_v2);
    setHook((void*)0x00584927, infoAiCntrReadSave_v2);
    setHook((void*)0x005865C2, infoAiCntrWriteSave_v2);
    setHook((void*)0x005854B7, infoAiCntrFree_v2);

    writeDword(0x004C4251, UNIT_COUNT_FOR_COUNTERS);
    writeDword(0x004C4361, UNIT_COUNT_FOR_COUNTERS);

    setHook((void*)0x0057E52D, ctr1_unit_type_count_workers);
    setHook((void*)0x0057E69D, ctr2_unit_type_count_total_workers);
}
#pragma optimize( "", on )
