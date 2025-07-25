#include "stdafx.h"
#include "techtree.h"

extern int current_loaded_version;

__declspec(naked) void tech_size_1() //005C2B20
{
    __asm
    {
        xor     ecx, ecx
        push    0D4h
        mov     cx, [esi + 18h]
        push    ecx
        call    calloc_internal
        mov     [esi + 1Ch], eax
        mov     ax, [esi + 18h]
        add     esp, 8
        xor     ebp, ebp
        test    ax, ax
        mov     ecx, 005C2B3Dh
        jmp     ecx
    }
}

__declspec(naked) void tech_size_2() //005C2CA3
{
    __asm
    {
        mov     ax, [esi + 18h]
        add     ebx, 0D4h
        mov     edx, 005C2CACh
        jmp     edx
    }
}

__declspec(naked) void tech_size_3() //005C306F
{
    __asm
    {
        mov     ax, [esi + 18h]
        xor     ebp, ebp
        test    ax, ax
        mov     edx, 005C3076h
        jmp     edx
    }
}

__declspec(naked) void tech_size_4() //005C3189
{
    __asm
    {
        mov     ax, [esi + 18h]
        add     ebx, 0D4h
        mov     edx, 005C3192h
        jmp     edx
    }
}

__declspec(naked) void tech_size_5() //005C25F0
{
    __asm
    {
        mov     ax, [esi + 18h]
        xor     ebp, ebp
        cmp     ax, bx
        mov     edx, 005C25F7h
        jmp     edx
    }
}

__declspec(naked) void tech_size_6() //005C264F
{
    __asm
    {
        mov     dx, [esi + 18h]
        add     edi, 0D4h
        mov     eax, 005C2658h
        jmp     eax
    }
}

__declspec(naked) void tech_size_7() //005C16CE
{
    __asm
    {
        mov     [esi], al
        mov     [esi + 8], al
        mov     [esi + 10h], ax
        mov     [esi + 18h], ax
        mov     edx, 005C16DAh
        jmp     edx
    }
}

__declspec(naked) void tech_size_8() //005C1756
{
    __asm
    {
        mov     dx, [ebx + 18h]
        mov     [ebp + 18h], dx
        mov     eax, 005C175Ch
        jmp     eax
    }
}

__declspec(naked) void tech_size_9() //005C1724
{
    __asm
    {
        mov     [ebp + 10h], ax
        mov     [ebp + 18h], ax
        mov     edx, 005C172Bh
        jmp     edx
    }
}

__declspec(naked) void tech_size_10() //005C179F
{
    __asm
    {
        mov     dx, [ebp + 18h]
        mov     [ebp + 14h], eax
        mov     eax, 005C17A5h
        jmp     eax
    }
}

__declspec(naked) void tech_size_11() //005C1ACD
{
    __asm
    {
        mov     ax, [ebp + 18h]
        mov     edi, [ebp + 1Ch]
        mov     ecx, 005C1AD3h
        jmp     ecx
    }
}

__declspec(naked) void tech_size_12() //005C1AEE
{
    __asm
    {
        mov     ax, [ebp + 18h]
        xor     esi, esi
        test    ax, ax
        mov     ecx, 005C1AF5h
        jmp     ecx
    }
}

__declspec(naked) void tech_size_13() //005C1BED
{
    __asm
    {
        mov     cx, [ebp + 18h]
        inc     eax
        add     esi, 0D4h
        mov     edx, 005C1BF7h
        jmp     edx
    }
}

__declspec(naked) void tech_size_14() //005C32BC
{
    __asm
    {
        mov     bx, [ecx + 18h]
        mov     edi, ebx
        test    edi, edi
        mov     ebx, 005C32C3h
        jmp     ebx
    }
}

__declspec(naked) void tech_size_15() //005C3931
{
    __asm
    {
        mov     ax, [esi + 18h]
        mov     ebx, [edi + 1D94h]
        xor     ebp, ebp
        test    ax, ax
        mov     ecx, 005C393Eh
        jmp     ecx
    }
}

__declspec(naked) void tech_size_16() //005C3987
{
    __asm
    {
        add     edi, 0D4h
        mov     ax, [ecx + 18h]
        mov     esi, 005C3990h
        jmp     esi
    }
}

__declspec(naked) void tech_size_17() //005C3B07
{
    __asm
    {
        mov     dx, [esi + 18h]
        mov     edi, edx
        test    edi, edi
        mov     edx, 005C3B0Eh
        jmp     edx
    }
}

__declspec(naked) void tech_size_18() //005C3CCE
{
    __asm
    {
        mov     dx, [esi + 18h]
        mov     edi, edx
        test    edi, edi
        mov     edx, 005C3CD5h
        jmp     edx
    }
}

__declspec(naked) void tech_size_19() //005C3D4B
{
    __asm
    {
        mov     dx, [eax + 18h]
        xor     ecx, ecx
        test    dx, dx
        mov     edx, 005C3D52h
        jmp     edx
    }
}

__declspec(naked) void tech_size_20() //005C3E95
{
    __asm
    {
        mov     dx, [esi + 18h]
        mov     edi, edx
        test    edi, edi
        mov     edx, 005C3E9Ch
        jmp     edx
    }
}

__declspec(naked) void tech_size_21() //005C3EFA
{
    __asm
    {
        mov     dx, [eax + 18h]
        add     edi, 0D4h
        cmp     ecx, edx
        mov     [esp + 14h], ecx
        mov     ecx, 005C3F09h
        jmp     ecx
    }
}

__declspec(naked) void tech_size_22() //005C401A
{
    __asm
    {
        mov     ax, [ecx + 18h]
        xor     esi, esi
        cmp     ax, dx
        mov     eax, 005C4021h
        jmp     eax
    }
}

__declspec(naked) void tech_size_23() //005C403F
{
    __asm
    {
        xor     eax, eax
        mov     ax, [ecx + 18h]
        cmp     esi, eax
        mov     eax, 005C4046h
        jmp     eax
    }
}

__declspec(naked) void tech_size_24() //005C40E2
{
    __asm
    {
        mov     ax, [ecx + 18h]
        xor     esi, esi
        test    ax, ax
        mov     edx, 005C40E9h
        jmp     edx
    }
}

__declspec(naked) void tech_size_25() //005C40FF
{
    __asm
    {
        mov     ax, [ecx + 18h]
        add     edx, 0D4h
        cmp     esi, eax
        mov     eax, 005C410Ah
        jmp     eax
    }
}

__declspec(naked) void tech_size_26() //005C4150
{
    __asm
    {
        mov     cx, [esi + 18h]
        xor     eax, eax
        test    ecx, ecx
        mov     edx, 005C4157h
        jmp     edx
    }
}

__declspec(naked) void tech_size_27() //005C41AD
{
    __asm
    {
        mov     cx, [esi + 18h]
        xor     edi, edi
        mov     ebx, [eax]
        mov     eax, 005C41B4h
        jmp     eax
    }
}

__declspec(naked) void tech_size_28() //005C4219
{
    __asm
    {
        mov     cx, [esi + 18h]
        xor     eax, eax
        test    ecx, ecx
        mov     edx, 005C4220h
        jmp     edx
    }
}

__declspec(naked) void tech_size_29() //005C42D5
{
    __asm
    {
        mov     cx, [edx + 18h]
        xor     eax, eax
        test    ecx, ecx
        mov     esi, 005C42DCh
        jmp     esi
    }
}

__declspec(naked) void tech_size_30() //005C4384
{
    __asm
    {
        xor     ecx, ecx
        mov     cx, [edx + 18h]
        test    ecx, ecx
        mov     edi, 005C438Bh
        jmp     edi
    }
}

__declspec(naked) void tech_size_31() //005C4487
{
    __asm
    {
        xor     ecx, ecx
        mov     cx, [edi + 18h]
        test    ecx, ecx
        push    005C448Eh
        ret
    }
}

__declspec(naked) void tech_size_32() //005C452B
{
    __asm
    {
        mov     dx, [ebp + 18h]
        xor     edi, edi
        xor     eax, eax
        test    edx, edx
        push    005C4534h
        ret
    }
}

__declspec(naked) void tech_size_33() //005C5096
{
    __asm
    {
        mov     ecx, [ecx + 24h]
        xor     edx, edx
        mov     dx, [ebp + 18h]
        push    005C509Eh
        ret
    }
}

__declspec(naked) void tech_size_34() //005C538F
{
    __asm
    {
        xor     ecx, ecx
        mov     cx, [esi + 18h]
        mov     edx, [edx + eax * 4]
        push    005C5397h
        ret
    }
}

__declspec(naked) void tech_size_35() //005C55D0
{
    __asm
    {
        xor     eax, eax
        push    ebx
        mov     ax, [ecx + 18h]
        push    005C55D6h
        ret
    }
}

__declspec(naked) void tech_size_36() //005C57B1
{
    __asm
    {
        xor     esi, esi
        mov     bx, [ebp + 18h]
        test    ebx, ebx
        push    005C57B8h
        ret
    }
}

__declspec(naked) void tech_size_37() //005C5980
{
    __asm
    {
        xor     edx, edx
        mov     bx, [ebp + 18h]
        test    ebx, ebx
        push    005C5987h
        ret
    }
}

__declspec(naked) void tech_size_38() //005C5B70
{
    __asm
    {
        xor     edx, edx
        mov     bx, [ebp + 18h]
        test    ebx, ebx
        push    005C5B77h
        ret
    }
}

__declspec(naked) void tech_size_39() //005C5C70
{
    __asm
    {
        xor     edx, edx
        push    esi
        mov     dx, [ecx + 18h]
        push    005C5C76h
        ret
    }
}

/////

__declspec(naked) void age_unit_size_1() //005C1839
{
    __asm
    {
        mov     ecx, [ebp + 4]
        mov     ax, [esi + ecx + 14h]
        test    ax, ax
        push    005C1842h
        ret
    }
}

__declspec(naked) void age_unit_size_2() //005C1865
{
    __asm
    {
        mov     cx, [eax + 14h]
        mov     edi, [eax + 18h]
        push    005C186Bh
        ret
    }
}

__declspec(naked) void age_unit_size_3() //005C2752
{
    __asm
    {
        mov     eax, [esi + 4]
        mov     ax, [eax + ebx + 14h]
        test    ax, ax
        push    005C275Bh
        ret
    }
}

__declspec(naked) void age_unit_size_4() //005C2774
{
    __asm
    {
        mov     edx, [esi + 4]
        xor     ecx, ecx
        mov     cx, [edx + ebx + 14h]
        push    005C277Dh
        ret
    }
}

__declspec(naked) void age_unit_size_5() //005C4595
{
    __asm
    {
        mov     cx, [ebx + 14h]
        xor     eax, eax
        push    ebp
        push    edi
        test    cx, cx
        push    005C459Eh
        ret
    }
}

__declspec(naked) void age_unit_size_6() //005C4665
{
    __asm
    {
        mov     dx, [ebx + 14h]
        inc     eax
        cmp     eax, edx
        push    005C466Bh
        ret
    }
}

__declspec(naked) void age_unit_size_7() //005C4675
{
    __asm
    {
        mov     cx, [ebx + 14h]
        xor     eax, eax
        test    cx, cx
        push    005C467Ch
        ret
    }
}

__declspec(naked) void age_unit_size_8() //005C46C9
{
    __asm
    {
        mov     cx, [ebx + 14h]
        inc     eax
        cmp     eax, ecx
        push    005C46CFh
        ret
    }
}

__declspec(naked) void age_unit_size_9() //005C2D77
{
    __asm
    {
        add     ecx, ebx
        mov     ax, [ecx + 14h]
        test    ax, ax
        push    005C2D7Eh
        ret
    }
}

__declspec(naked) void tech_size_1_load() //005C269D
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 1
        jge     tech_size_1_extended
        mov     eax, 1
        jmp     tech_size_1_continue
tech_size_1_extended:
        mov     eax, 2
tech_size_1_continue:
        lea     edx, [esi + 18h]
        push    eax
        mov     ecx, edi
        mov     eax, 005C26A4h
        jmp     eax
    }
}

__declspec(naked) void tech_size_2_load() //005C2742
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 1
        jge     tech_size_2_extended
        mov     eax, 1
        jmp     tech_size_2_continue
tech_size_2_extended:
        mov     eax, 2
tech_size_2_continue:
        mov     edx, [esi + 4]
        push    eax
        mov     ecx, edi
        mov     eax, 005C2749h
        jmp     eax
    }
}

__declspec(naked) void building_unit_size_load() //005C28ED
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 1
        jge     building_unit_size_load_extended
        mov     eax, 1
        jmp     building_unit_size_load_continue
building_unit_size_load_extended:
        mov     eax, 2
building_unit_size_load_continue:
        mov     edx, [esi + 0Ch]
        push    eax
        mov     ecx, edi
        mov     eax, 005C28F4h
        jmp     eax
    }
}

__declspec(naked) void building_unit_size_1() //005C28FD
{
    __asm
    {
        mov     eax, [esi + 0Ch]
        mov     ax, [ebx + eax + 18h]
        test    ax, ax
        push    005C2906h
        ret
    }
}

__declspec(naked) void building_unit_size_2() //005C291F
{
    __asm
    {
        mov     edx, [esi + 0Ch]
        xor     ecx, ecx
        mov     cx, [edx + ebx + 18h]
        push    005C2928h
        ret
    }
}

__declspec(naked) void building_unit_size_3() //005C1974
{
    __asm
    {
        mov     ecx, [ebp + 0Ch]
        mov     ax, [esi + ecx + 18h]
        test    ax, ax
        push    005C197Dh
        ret
    }
}

__declspec(naked) void building_unit_size_4() //005C199E
{
    __asm
    {
        add     eax, esi
        mov     cx, [eax + 18h]
        mov     edi, [eax + 1Ch]
        push    005C19A6h
        ret
    }
}

__declspec(naked) void building_unit_size_5() //005C2EB4
{
    __asm
    {
        mov     eax, [esi + 0Ch]
        lea     ecx, [ebx + eax]
        mov     ax, [ebx + eax + 18h]
        test    ax, ax
        push    005C2EC0h
        ret
    }
}

int __stdcall check_tech_ids(int16_t* tech_ids, int16_t tech_id)
{
    for (int i = 0; i < 300; i++)
        if (tech_ids[i] == tech_id)
            return 1;
    return 0;
}

__declspec(naked) void check_tech_ids_new() //00468E10
{
    __asm
    {
        mov     eax, [esp + 4]
        add     ecx, 0B64h
        push    eax
        push    ecx
        call    check_tech_ids
        ret     4
    }
}

__declspec(naked) void tech_ids_1() //004696C6
{
    __asm
    {
tech_ids_1_loop:
        cmp     [ecx], di
        jz      tech_ids_1_end
        inc     eax
        add     ecx, 2
        cmp     eax, 300
        jl      tech_ids_1_loop
        mov     eax, 004696D5h
        jmp     eax

tech_ids_1_end:
        mov     eax, 0046969Ah
        jmp     eax
    }
}

__declspec(naked) void tech_ids_2() //0046978E
{
    __asm
    {
tech_ids_2_loop:
        cmp     dword ptr [edx], 0
        jle     tech_ids_2_end
        inc     ecx
        add     edx, 2
        cmp     ecx, 300
        jl      tech_ids_2_loop
        jmp     tech_ids_2_complete

tech_ids_2_end:
        mov     edx, [esp + 40h]
        mov     edx, [edx]
        mov     [esi + ecx * 2 + 0B64h], dx

tech_ids_2_complete:
        mov     ecx, 004697AEh
        jmp     ecx
    }
}

__declspec(naked) void tech_ids_3() //004698BC
{
    __asm
    {
tech_ids_3_loop:
        cmp     dword ptr [ecx], 0
        jle     tech_ids_3_end
        inc     eax
        add     ecx, 2
        cmp     eax, 300
        jl      tech_ids_3_loop
        mov     dword ptr [esp + 10h], 1
        jmp     tech_ids_3_complete

tech_ids_3_end:
        mov     [esi + eax * 2 + 0B64h], bp
        mov     dword ptr [esp + 10h], 1
        
tech_ids_3_complete:
        mov     eax, 00469828h
        jmp     eax
    }
}

__declspec(noinline) Bld_Zone_Constructer* tech_tree_insert_bldg(TribeTechHelpScreen* tech_tree, Bld_Zone_Constructer* insert_point, int id, int* bldg_zone_cntr)
{
    TRIBE_Buildings* buildings = TRIBE_Tech_Tree__get_building_info(tech_tree->player_tech_tree_help, id);
    if (buildings)
    {
        switch (id)
        {
        case 598:
        case 79:
        case 234:
        case 235:
        case 236:
        case 196:
        case 72:
        case 487:
        case 117:
        case 155:
        case 195:
        case 5080:
        case 1869:
            buildings->node_type = 6;
            break;
        default:
            break;
        }
        TribeTechHelpScreen__calculate_num_bld_zones_by_bld(tech_tree, buildings->building_id);
        int prereg = TRIBE_Tech_Tree__get_age_prereg2(tech_tree->player_tech_tree_help, buildings);
        TRIBE_Ages* ages = TRIBE_Tech_Tree__get_age_info(tech_tree->player_tech_tree_help, prereg);
        if (ages)
        {
            insert_point = TribeTechHelpScreen__insert_building_zone_constructs(tech_tree, buildings->building_id, bldg_zone_cntr, insert_point, buildings, ages);
            TribeTechHelpScreen__insert_building_id(tech_tree, id);
        }
    }
    return insert_point;
}

int __fastcall TribeTechHelpScreen__make_bld_zone_constructor_list_new(TribeTechHelpScreen* tech_tree)
{
    TRIBE_Ages* ages = TRIBE_Tech_Tree__get_age_info(tech_tree->player_tech_tree_help, 1);
    if (!ages)
        return -1;

    TribeTechHelpScreen__calculate_num_bld_zones_by_age(tech_tree, ages);
    TRIBE_Buildings* buildings = TRIBE_Tech_Tree__get_building_info(tech_tree->player_tech_tree_help, 109);
    if (!buildings)
        return -1;

    int bld_zone_cntr = 0;

    Bld_Zone_Constructer* insert_point = TribeTechHelpScreen__insert_building_zone_constructs(tech_tree, buildings->building_id, &bld_zone_cntr, 0, buildings, ages);
    TribeTechHelpScreen__insert_building_id(tech_tree, 109);
    TribeTechHelpScreen__calculate_num_bld_zones_by_bld(tech_tree, buildings->building_id);
    
    if (current_loaded_version >= 8)
    {
        int n = tech_tree->player_tech_tree_help->number_buildings;
        for (int i = 0; i < n; i++)
        {
            int id = tech_tree->player_tech_tree_help->tech_tree_buildings[i].building_id;
            if (id != 109)
                insert_point = tech_tree_insert_bldg(tech_tree, id == 87 ? 0 : insert_point, id, &bld_zone_cntr);
        }
    }
    else
    {
        //insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 621, &bld_zone_cntr);

        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 49, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 317, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 103, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 209, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 584, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 323, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 562, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 84, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 319, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 68, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 50, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, 0, 87, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 12, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 101, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 276, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 45, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 199, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 1576, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 598, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 79, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 234, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 235, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 236, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 196, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 72, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 487, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 117, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 155, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 195, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 335, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 104, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 82, &bld_zone_cntr);
        insert_point = tech_tree_insert_bldg(tech_tree, insert_point, 70, &bld_zone_cntr);
    }
    return bld_zone_cntr;
}

extern TShape** techTreeBldg;
extern TShape** techTreeUnit;
extern TShape** techTreeTech;

void __stdcall techtree_set_button_pics(TribeTechHelpScreen* tech_tree)
{
    TribePopUpHelp* popup_help = tech_tree->popUpHelp1;
    if (!popup_help)
        return;

    TShape** tech_tree_tech_pics = techTreeTech;
    TShape** tech_tree_bldg_pics = techTreeBldg;
    TShape** tech_tree_unit_pics = techTreeUnit;

    int master_player_id = tech_tree->current_tech_tree_master_player_id > 0 ? tech_tree->current_tech_tree_master_player_id : 1;

    popup_help->button_pics_bld = tech_tree_bldg_pics[master_player_id];
    popup_help->button_pics_tech = tech_tree_tech_pics[master_player_id];
    popup_help->button_pics_unit = tech_tree_unit_pics[master_player_id];
    popup_help->node_zone_height = 64;
    popup_help->node_zone_width = 64;
    popup_help->technode_popup_pics = tech_tree->tech_node_pic;
    popup_help->button_pic_height = 36;
    popup_help->button_pic_width = 36;
    popup_help->button_node_offset_x = 14;
    popup_help->button_node_offset_y = 3;
}

__declspec(naked) void on_set_techtree_button_pic() //00463E38
{
    __asm
    {
        mov     dword ptr [esi + 0E68h], 3
        push    eax
        push    ecx
        push    esi
        call    techtree_set_button_pics
        pop     ecx
        pop     eax
        mov     edx, 00463E42h
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setTechTreeHooks()
{
    //tech size
    //writeByte(0x005C26A1, 2); //changed to asm
    setHook((void*)0x005C269D, tech_size_1_load);
    writeByte(0x005C2CF1, 2);
    
    //age unit size
    //writeByte(0x005C2746, 2); //changed to asm
    setHook((void*)0x005C2742, tech_size_2_load);
    writeByte(0x005C2D68, 2);
    writeDword(0x005C2D84, 0xFFFF);
    writeDword(0x005C275E, 0xFFFF);
    writeDword(0x005C1845, 0xFFFF);

    //building unit size
    setHook((void*)0x005C28ED, building_unit_size_load);
    writeByte(0x005C2EA8, 2);
    writeDword(0x005C2EC6, 0xFFFF);
    writeDword(0x005C2909, 0xFFFF);
    writeDword(0x005C1980, 0xFFFF);

    setHook((void*)0x005C2B20, tech_size_1);
    setHook((void*)0x005C2CA3, tech_size_2);
    setHook((void*)0x005C306F, tech_size_3);
    setHook((void*)0x005C3189, tech_size_4);
    setHook((void*)0x005C25F0, tech_size_5);
    setHook((void*)0x005C264F, tech_size_6);
    setHook((void*)0x005C16CE, tech_size_7);
    setHook((void*)0x005C1756, tech_size_8);
    setHook((void*)0x005C1724, tech_size_9);
    setHook((void*)0x005C179F, tech_size_10);
    setHook((void*)0x005C1ACD, tech_size_11);
    setHook((void*)0x005C1AEE, tech_size_12);
    setHook((void*)0x005C1BED, tech_size_13);
    setHook((void*)0x005C32BC, tech_size_14);
    setHook((void*)0x005C3931, tech_size_15);
    setHook((void*)0x005C3987, tech_size_16);
    setHook((void*)0x005C3B07, tech_size_17);
    setHook((void*)0x005C3CCE, tech_size_18);
    setHook((void*)0x005C3D4B, tech_size_19);
    setHook((void*)0x005C3E95, tech_size_20);
    setHook((void*)0x005C3EFA, tech_size_21);
    setHook((void*)0x005C401A, tech_size_22);
    setHook((void*)0x005C403F, tech_size_23);
    setHook((void*)0x005C40E2, tech_size_24);
    setHook((void*)0x005C40FF, tech_size_25);
    setHook((void*)0x005C4150, tech_size_26);
    setHook((void*)0x005C41AD, tech_size_27);
    setHook((void*)0x005C4219, tech_size_28);
    setHook((void*)0x005C42D5, tech_size_29);
    setHook((void*)0x005C4384, tech_size_30);
    setHook((void*)0x005C4487, tech_size_31);
    setHook((void*)0x005C452B, tech_size_32);
    setHook((void*)0x005C5096, tech_size_33);
    setHook((void*)0x005C538F, tech_size_34);
    setHook((void*)0x005C55D0, tech_size_35);
    setHook((void*)0x005C57B1, tech_size_36);
    setHook((void*)0x005C5980, tech_size_37);
    setHook((void*)0x005C5B70, tech_size_38);
    setHook((void*)0x005C5C70, tech_size_39);

    setHook((void*)0x005C1839, age_unit_size_1);
    setHook((void*)0x005C1865, age_unit_size_2);
    setHook((void*)0x005C2752, age_unit_size_3);
    setHook((void*)0x005C2774, age_unit_size_4);
    setHook((void*)0x005C4595, age_unit_size_5);
    setHook((void*)0x005C4665, age_unit_size_6);
    setHook((void*)0x005C4675, age_unit_size_7);
    setHook((void*)0x005C46C9, age_unit_size_8);
    setHook((void*)0x005C2D77, age_unit_size_9);

    setHook((void*)0x005C28FD, building_unit_size_1);
    setHook((void*)0x005C291F, building_unit_size_2);
    setHook((void*)0x005C1974, building_unit_size_3);
    setHook((void*)0x005C199E, building_unit_size_4);
    setHook((void*)0x005C2EB4, building_unit_size_5);

    //inserted tech ids
    setHook((void*)0x00468E10, check_tech_ids_new);
    setHook((void*)0x004696C6, tech_ids_1);
    setHook((void*)0x0046978E, tech_ids_2);
    setHook((void*)0x004698BC, tech_ids_3);

    //tech level skip extra lines
    writeByte(0x005C5332, 0xEB);

    //popup help requirements
    setHook((void*)0x00463E38, on_set_techtree_button_pic);

    setHook((void*)0x00464F10, TribeTechHelpScreen__make_bld_zone_constructor_list_new);
}
#pragma optimize( "", on )
