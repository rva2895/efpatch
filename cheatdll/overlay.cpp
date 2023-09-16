#include "stdafx.h"
#include "overlay.h"

/*
void* overlay_ptr = NULL;

int pos_x = 400;
int pos_y = 600;
int size_x = 1000;
int size_y = 300;

HBRUSH br_black;
bool brushes_loaded = false;

bool overlay_state = false;

__declspec(naked) void onOverlayInit() //004F583C
{
    __asm
    {
        //set rect
        mov     ecx, overlay_ptr
        push    size_y
        push    size_x
        push    pos_y
        push    pos_x
        mov     eax, 004B5F60h
        call    eax
        //
        mov     ecx, overlay_ptr
        mov     eax, 005DF850h
        call    eax
        mov     ecx, overlay_ptr
        push    0
        push    0
        mov     eax, 005DF880h
        call    eax
        //
        lea     edi, [esi + 11A0h]
        mov     ecx, 004F5842h
        jmp     ecx
    }
}

__declspec(naked) int __fastcall get_gametime()
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     ecx, [ecx + 17B4h]
        mov     ecx, [ecx + 126Ch]
        mov     eax, [ecx + 10h]
        ret
    }
}

void print_graph(HDC hdc, std::vector<std::vector<std::pair<int, int>>> data)
{

}



class TABLE
{
private:
    std::string title;
    std::vector<std::string> row_names;
    std::vector<std::string> column_names;
    std::vector<std::vector<float>> values;
    typedef struct
    {
        int x;
    } column_info;
    void print_text(HDC hdc, int x, int y, int w, int h, const std::string& text)
    {
        RECT r;
        r.left = x;
        r.right = x + w;
        r.top = y;
        r.bottom = y + h;
        DrawText(hdc, text.c_str(), text.length(), &r, NULL);
    }
public:
    void add_column(const std::string& column_title, const std::vector<float>& column_values)
    {
        column_names.push_back(column_title);
        values.push_back(column_values);
    }
    void add_row(const std::string& row_title)
    {
        row_names.push_back(row_title);
    }
    void print(HDC hdc, int x, int y, int w, int h, int col_width, int col_height)
    {
        //print row names
        int y_offset = y + col_height;
        for (auto it = row_names.begin(); it != row_names.end(); ++it)
        {
            print_text(hdc, x, y_offset, col_width, col_height, *it);
            y_offset += col_height;
        }
        //print columns
        int col_index = 0;
        int x_offset = x + col_width;
        for (auto col_it = column_names.begin(); col_it != column_names.end(); ++col_it)
        {
            y_offset = y;
            
            print_text(hdc, x_offset, y_offset, col_width, col_height, *col_it);
            for (auto row_it = values[col_index].begin(); row_it != values[col_index].end(); ++row_it)
            {
                y_offset += col_height;
                int v = *row_it;
                char s[0x10];
                snprintf(s, _countof(s), "%d", v);
                print_text(hdc, x_offset, y_offset, col_width, col_height, s);
            }
            x_offset += col_width;
            col_index++;
        }
    }
};

void __stdcall window_overlay_draw2(HDC hdc)
{
    if (!brushes_loaded)
    {
        br_black = CreateSolidBrush(RGB(32, 32, 32));
        //br_black = CreateHatchBrush(HS_DIAGCROSS, RGB(32, 32, 32));
        brushes_loaded = true;
    }

    //get_player_units(get_player(1));

    RECT r;
    r.left = 0;
    r.top = 0;
    r.right = size_x;
    r.bottom = size_y;
    SetBkMode(hdc, TRANSPARENT);
    FillRect(hdc, &r, br_black);
    
    SetTextColor(hdc, RGB(255, 255, 255));
    char b[0x100];
    //sprintf(b, "%d players", );
    //TextOut(hdc, 0, 0, b, strlen(b));

    TABLE t;
    //resources
    std::vector<float>carbon;
    std::vector<float>food;
    std::vector<float>nova;
    std::vector<float>ore;

    std::vector<float>carbon_workers;
    std::vector<float>food_workers;
    std::vector<float>nova_workers;
    std::vector<float>ore_workers;

    for (int i = 1; i < (*base_game)->world->player_num; i++)
    {
        TRIBE_Player* player = (*base_game)->world->players[i];
        t.add_row(player->name);
        
        food.push_back(player->attributes[0]);
        carbon.push_back(player->attributes[1]);
        ore.push_back(player->attributes[2]);
        nova.push_back(player->attributes[3]);

        int carbon_workers_count = 0;
        int food_workers_count = 0;
        int nova_workers_count = 0;
        int ore_workers_count = 0;

        for (int j = 0; j < player->objects->Number_of_objects; j++)
        {
            if (player->objects->List[j]->master_obj->object_group == 58)
            {
                switch (player->objects->List[j]->master_obj->id)
                {
                case 56:    //fisher
                case 57:
                case 120:   //forager
                case 354:
                case 122:   //hunter
                case 216:
                case 214:   //farmer
                case 259:
                case 590:   //herder
                case 592:
                    food_workers_count++;
                    break;
                case 123:   //carbon collector
                case 218:
                    carbon_workers_count++;
                    break;
                case 124:   //ore collector
                case 220:
                    ore_workers_count++;
                    break;
                case 579:   //nova collector
                case 581:
                    nova_workers_count++;
                    break;
                default:
                    break;
                }
            }
        }

        carbon_workers.push_back(carbon_workers_count);
        food_workers.push_back(food_workers_count);
        nova_workers.push_back(nova_workers_count);
        ore_workers.push_back(ore_workers_count);
    }

    t.add_column("Carbon", carbon);
    t.add_column("Food", food);
    t.add_column("Nova", nova);
    t.add_column("Ore", ore);

    t.add_column("Carbon workers", carbon_workers);
    t.add_column("Food workers", food_workers);
    t.add_column("Nova workers", nova_workers);
    t.add_column("Ore workers", ore_workers);

    
    r.left += 5;
    r.top += 5;
    r.right -= 5;
    r.bottom -= 5;
    t.print(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, 100, 20);

}

__declspec(naked) void window_overlay_render()
{
    __asm
    {
        sub     esp, 154h
        push    ebp
        push    esi
        mov     esi, ecx
        xor     ebp, ebp
        push    edi
        mov     ecx, [esi + 138h]
        cmp     ecx, ebp
        jnz     short loc_5E0023
        pop     edi
        pop     esi
        xor     eax, eax
        pop     ebp
        add     esp, 154h
        retn

loc_5E0023:
        lea     edi, [esi + 13Ch]
        push    1
        push    edi
        mov     [esp + 14h], ebp
        mov     eax, 00473090h
        call    eax
        mov     ecx, [esi + 138h]
        push    0
        mov     eax, 00472B60h
        call    eax
        test    eax, eax
        jnz     short loc_5E0053
        pop     edi
        pop     esi
        pop     ebp
        add     esp, 154h
        retn

loc_5E0053:
        mov     ecx, [esi + 138h]
        push    ebx
        push    edi
        mov     eax, 00473370h
        call    eax
        mov     ecx, [esi + 138h]
        mov     eax, [esi + 134h]
        push    eax
        mov     edx, [ecx + 38h]
        push    edx
        call    SelectClipRgn
        mov     ecx, [esi + 138h]
        mov     eax, [esi + 0F8h]
        push    eax
        mov     edx, [ecx + 38h]
        push    edx
        call    SelectObject
        mov     [esp + 24h], eax
        mov     eax, [esi + 138h]
        push    1
        mov     ecx, [eax + 38h]
        push    ecx
        call    SetBkMode
        //
        mov     eax, [esi + 138h]
        mov     ecx, [eax + 38h]
        push    ecx
        call    window_overlay_draw2
        //
        mov     ecx, [esi + 138h]
        mov     eax, [esp + 24h]
        push    eax
        mov     edx, [ecx + 38h]
        push    edx
        call    SelectObject
        mov     eax, [esi + 138h]
        xor     edi, edi
        push    edi
        mov     ecx, [eax + 38h]
        push    ecx
        call    SelectClipRgn
        mov     ecx, [esi + 138h]
        push    0
        mov     eax, 00472BB0h
        call    eax
        mov     eax, [esi + 14h]
        mov     edx, [esi + 90h]
        mov     ecx, [esi + 18h]
        mov     [esi + 14Ch], edi
        mov     [esi + 150h], edi
        mov     edi, [esi + 94h]
        dec     eax
        mov     [esi + 15Ch], edx
        dec     ecx
        add     edx, eax
        mov     [esi + 160h], edi
        mov     [esi + 154h], eax
        mov     eax, [esi + 104h]
        mov     [esi + 158h], ecx
        mov     [esi + 164h], edx
        mov     edx, [esi + 12Ch]
        add     edi, ecx
        mov     ecx, [esi + 10Ch]
        mov     [esi + 168h], edi
        pop     ebx
        mov     [esi + 130h], edx
        mov     [esi + 108h], eax
        mov     [esi + 110h], ecx
        mov     dword ptr [esi + 16Ch], 1
        pop     edi
        pop     esi
        mov     eax, 1
        pop     ebp
        add     esp, 154h
        retn
    }
}

__declspec(naked) void window_overlay_draw()
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        sub     esp, 12Ch
        push    esi
        mov     esi, ecx
        mov     eax, [esi + 20h]
        test    eax, eax
        jz      loc_5DFE21
        mov     eax, [esi + 70h]
        test    eax, eax
        jz      loc_5DFE21
        mov     eax, [esi + 6Ch]
        test    eax, eax
        jz      loc_5DFE21
        mov     edx, [esi + 0E4h]
        mov     eax, [esi]
        inc     edx
        push    0
        mov     [esi + 0E4h], edx
        call    dword ptr [eax + 34h]
        mov     ecx, [esi + 10Ch]
        mov     edx, [esi + 124h]
        cmp     ecx, 1
        jnz     short loc_5DF9FA
        mov     [esi + 12Ch], edx
        jmp     short loc_5DFA10

loc_5DF9FA:
        add     edx, 4
        mov     eax, 0CCCCCCCDh
        mul     edx
        shr     edx, 2
        lea     eax, [edx + edx * 4]
        mov     [esi + 12Ch], eax

loc_5DFA10:
        mov     eax, [esi + 138h]
        test    eax, eax
        jz      short loc_5DFA70
        mov     edx, [esi + 12Ch]
        mov     eax, [esi + 130h]
        cmp     edx, eax
        jnz     short loc_5DFA4C
        mov     eax, [esi + 104h]
        mov     edx, [esi + 108h]
        cmp     eax, edx
        jnz     short loc_5DFA4C
        cmp     ecx, [esi + 110h]
        jnz     short loc_5DFA4C
        mov     al, [esi + 0F7h]
        test    al, al
        jz      short loc_5DFA70

loc_5DFA4C:
        mov     ecx, esi
        call    window_overlay_render        //new func
        mov     al, [esi + 0F7h]
        test    al, al
        jz      short loc_5DFA70
        mov     edx, [esi]
        mov     ecx, esi
        mov     byte ptr [esi + 0F7h], 0
        call    dword ptr [edx + 38h]
        pop     esi
        mov     esp, ebp
        pop     ebp
        retn

loc_5DFA70:
        mov     eax, [esi + 16Ch]
        test    eax, eax
        jz      short loc_5DFACE
        mov     ecx, [esi + 138h]
        test    ecx, ecx
        jz      short loc_5DFACE
        push    1
        push    1
        mov     eax, 00473BD0h
        call    eax
        mov     edx, [esi + 160h]
        mov     eax, [esi + 15Ch]
        lea     ecx, [esi + 14Ch]
        push    1
        push    ecx
        mov     ecx, [esi + 20h]
        push    edx
        push    eax
        push    ecx
        mov     ecx, [esi + 138h]
        mov     eax, 00473590h
        call    eax
        mov     ecx, [esi + 138h]
        push    1
        push    0
        mov     eax, 00473BD0h
        call    eax
        mov     edx, [esi]
        mov     ecx, esi
        call    dword ptr [edx + 38h]

loc_5DFACE:
loc_5DFE21:
        pop     esi
        mov     esp, ebp
        pop     ebp
        retn
    }
}

void __stdcall replace_overlay_vtable()
{
    void* old_vtable = *(void**)overlay_ptr;
    void* new_vtable = malloc(0x184);
    memcpy(new_vtable, old_vtable, 0x184);
    ((DWORD*)new_vtable)[15] = (DWORD)&window_overlay_draw;
    *(void**)overlay_ptr = new_vtable;

    overlay_state = false;
}

void __stdcall delete_vtable()
{
    free(*(void**)overlay_ptr);
    *(void**)overlay_ptr = (void*)0x00668478;

    overlay_state = false;
}

__declspec(naked) void onOverlayCreate() //004F45C1
{
    __asm
    {
        mov     eax, 005DF500h
        call    eax
        push    eax
        //
        push    178h
        mov     eax, 00632B9Dh
        call    eax
        add     esp, 4
        mov     overlay_ptr, eax
        mov     ecx, [esi + 10BCh]
        mov     edx, [esi + 20h]
        push    ecx
        push    edx
        mov     ecx, eax
        mov     eax, 005DF500h
        call    eax
        call    replace_overlay_vtable
        //
        pop     eax
        mov     ecx, 004F45CAh
        jmp     ecx
    }
}

__declspec(naked) void onOverlayDelete() //004F5F52
{
    __asm
    {
        mov     ecx, overlay_ptr
        test    ecx, ecx
        jz      _no_overlay_delete
        push    ecx
        call    delete_vtable
        pop     ecx
        mov     eax, [ecx]
        push    1
        call    dword ptr [eax]
        xor     eax, eax
        mov     overlay_ptr, eax
_no_overlay_delete:
        //
        mov     dword ptr [esi + 119Ch], 0
        mov     edi, 004F5F5Ch
        jmp     edi
    }
}


#pragma optimize( "s", on )
void setOverlayHooks()
{
    setHook((void*)0x004F45C1, onOverlayCreate);
    setHook((void*)0x004F5F52, onOverlayDelete);
    setHook((void*)0x004F583C, onOverlayInit);
}
#pragma optimize( "", on )


__declspec(naked) void __stdcall overlay_on()
{
    __asm
    {
        mov     ecx, overlay_ptr
        push    0
        push    2
        mov     eax, 005DF880h
        call    eax
        ret
    }
}

__declspec(naked) void __stdcall overlay_off()
{
    __asm
    {
        mov     ecx, overlay_ptr
        push    0
        push    0
        mov     eax, 005DF880h
        call    eax
        ret
    }
}

void overlay_switch()
{
    if (overlay_ptr)
    {
        if (overlay_state)
        {
            overlay_off();
            overlay_state = false;
        }
        else
        {
            overlay_on();
            overlay_state = true;
        }
    }
}
*/
