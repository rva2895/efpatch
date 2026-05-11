#include "stdafx.h"
#include "overlay.h"

int (__fastcall* const nullsub_33)() = (int(__fastcall* const)())0x00408B20;
int (__fastcall* const nullsub_32)() = (int(__fastcall* const)())0x00408B10;
int (__fastcall* const nullsub_22)() = (int(__fastcall* const)())0x00408B40;
int (__fastcall* const nullsub_23)() = (int(__fastcall* const)())0x00408B00;
int (__fastcall* const nullsub_7)() = (int(__fastcall* const)())0x004BF920;

void* __fastcall TRIBE_Panel_Screen_Overlay__vector_deleting_destructor(TRIBE_Panel_Screen_Overlay* this_, DWORD dummy, unsigned int a2);
void __fastcall TRIBE_Panel_Screen_Overlay__draw(TRIBE_Panel_Screen_Overlay* this_);
int __fastcall TRIBE_Panel_Screen_Overlay__handle_idle(TRIBE_Panel_Screen_Overlay* this_);
void __fastcall TRIBE_Panel_Screen_Overlay__get_true_render_rect(TRIBE_Panel_Screen_Overlay* this_, DWORD dummy, RECT* drawRect);

const DWORD TRIBE_Panel_Screen_Overlay__vftable[] =
{
    (DWORD)TRIBE_Panel_Screen_Overlay__vector_deleting_destructor,
    (DWORD)TPanel__setup,
    (DWORD)TPanel__set_rect,
    (DWORD)TPanel__set_rect2,
    (DWORD)TPanel__set_color,
    (DWORD)TPanel__set_active,
    0x004557A0,
    0x004B5E30,
    (DWORD)TPanel__set_positioning,
    0x004B5F20,
    (DWORD)TPanel__set_fixed_position,
    (DWORD)TPanel__set_redraw,
    (DWORD)TPanel__set_overlapped_redraw,
    (DWORD)TPanel__draw_setup,
    (DWORD)TPanel__draw_finish,
    (DWORD)TRIBE_Panel_Screen_Overlay__draw,
    (DWORD)TPanel__draw_rect,
    (DWORD)TPanel__draw_offset,
    (DWORD)TPanel__draw_rect2,
    (DWORD)TPanel__draw_offset2,
    (DWORD)TPanel__paint,
    (DWORD)TPanel__wnd_proc,
    (DWORD)TRIBE_Panel_Screen_Overlay__handle_idle,
    (DWORD)TPanel__handle_size,
    (DWORD)TPanel__handle_paint,
    (DWORD)TPanel__handle_key_down,
    (DWORD)TPanel__handle_char,
    (DWORD)nullsub_26,
    (DWORD)nullsub_26,
    (DWORD)nullsub_26,
    (DWORD)nullsub_26,
    (DWORD)TPanel__handle_mouse_down,
    (DWORD)TPanel__handle_mouse_move,
    (DWORD)TPanel__handle_mouse_up,
    (DWORD)TPanel__handle_mouse_dbl_click,
    (DWORD)TPanel__handle_mouse_wheel,
    (DWORD)nullsub_33,
    (DWORD)TPanel__handle_mouse_xbuttons,
    (DWORD)nullsub_33,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_32,
    (DWORD)nullsub_22,
    (DWORD)nullsub_23,
    (DWORD)TPanel__action,
    (DWORD)TRIBE_Panel_Screen_Overlay__get_true_render_rect,
    (DWORD)TPanel__is_inside,
    (DWORD)TPanel__set_focus,
    (DWORD)TPanel__set_tab_order,
    (DWORD)TPanel__set_tab_order2,
    (DWORD)TPanel__renderArea,
    (DWORD)TPanel__get_help_info,
    (DWORD)TPanel__stop_sound_system,
    (DWORD)TPanel__restart_sound_system,
    (DWORD)nullsub_7,
    (DWORD)TPanel__handle_reactivate,
    (DWORD)TPanel__pointing_at,
    1,
    2,
    4,
    8,
    0x10,
    0x20,
    0x40,
    0x80,
    0x100,
    0x200,
    0x400,
    0x800,
    0x1000,
    0x2000,
    0x4000,
    0x8000,
    0x10000,
    0x20000,
    0x40000,
    0x80000,
    0x100000,
    0x200000,
    0x400000,
    0x800000,
    0x1000000,
    0x2000000,
    0x4000000,
    0x8000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000
};

void TRIBE_Panel_Screen_Overlay__destroy_image_buffer(TRIBE_Panel_Screen_Overlay* this_)
{
    if (this_->image_clip_region)
    {
        DeleteObject(this_->image_clip_region);
        this_->image_clip_region = NULL;
    }

    if (this_->ImageBuffer)
    {
        TDrawArea__destructor(this_->ImageBuffer);
        operator_delete_internal(this_->ImageBuffer);
        this_->ImageBuffer = NULL;
    }
}

void __stdcall TRIBE_Panel_Screen_Overlay__destructor(TRIBE_Panel_Screen_Overlay* this_)
{
    this_->user_callbacks.destroy(this_->user_data);

    this_->vfptr = (TPanelVtbl*)TRIBE_Panel_Screen_Overlay__vftable;

    TRIBE_Panel_Screen_Overlay__destroy_image_buffer(this_);

    TPanel__destructor((TPanel*)this_);
}

void* __fastcall TRIBE_Panel_Screen_Overlay__vector_deleting_destructor(TRIBE_Panel_Screen_Overlay* this_, DWORD dummy, unsigned int a2)
{
    UNREFERENCED_PARAMETER(dummy);

    TRIBE_Panel_Screen_Overlay__destructor(this_);
    if ((a2 & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

void __fastcall TRIBE_Panel_Screen_Overlay__get_true_render_rect(TRIBE_Panel_Screen_Overlay* this_, DWORD dummy, RECT* drawRect)
{
    UNREFERENCED_PARAMETER(dummy);

    RECT test_rect = { 500, 10, 800, 110 };

    if (this_->ImageBuffer && this_->render_area && this_->visible && this_->active)
        *drawRect = this_->ScreenRect;
    else
        *drawRect = this_->render_rect;
}

void __fastcall TRIBE_Panel_Screen_Overlay__draw(TRIBE_Panel_Screen_Overlay* this_)
{
    int v2; // ebx
    TPanel* v3; // ecx
    int v4; // ebp
    int v5; // ebx
    __int64 v6; // rax
    int i; // edi
    TShape* v8; // ecx
    int v9; // eax
    int v10; // [esp-14h] [ebp-30h]
    int v11; // [esp+8h] [ebp-14h]
    float* v12; // [esp+Ch] [ebp-10h]
    HGDIOBJ h; // [esp+14h] [ebp-8h]
    UINT align; // [esp+18h] [ebp-4h]

    v2 = 0;
    if (!this_->render_area || !this_->visible || !this_->active)
    {
        this_->need_redraw = 0;
        return;
    }
    v3 = this_->parent_panel;
    if (v3)
    {
        v3->vfptr->draw_rect(v3, &this_->clip_rect);
        this_->ScreenRect = this_->clip_rect;
    }

    ++this_->display_changed_count;
    this_->vfptr->draw_setup((TPanel*)this_, 0);

    if (this_->need_restore)
    {
        TDrawArea__PtrClear(this_->ImageBuffer, &this_->ImageRect, 1);
        this_->user_callbacks.render_to_image_buffer(this_->user_data, this_->ImageBuffer, &this_->ImageRect, this_->image_clip_region);
    }

    TDrawArea__SetTrans(this_->ImageBuffer, 1, 1);
    TDrawArea__Copy(this_->ImageBuffer, this_->render_area, this_->clip_rect.left, this_->clip_rect.top, &this_->ImageRect, 1);
    TDrawArea__SetTrans(this_->ImageBuffer, 0, 1);

    this_->need_restore = 0;
    this_->vfptr->draw_finish((TPanel*)this_);

}

int __fastcall TRIBE_Panel_Screen_Overlay__handle_idle(TRIBE_Panel_Screen_Overlay* this_)
{
    TPanel__handle_idle((TPanel*)this_);

    int v2 = (*base_game)->prog_mode;
    if (v2 != 4 && v2 != 6 && v2 != 7 && v2 != 5)
        return 0;

    if (this_->user_callbacks.need_redraw(this_->user_data))
    {
        this_->vfptr->set_redraw((TPanel*)this_, 1);
        this_->parent_panel->vfptr->set_redraw(this_->parent_panel, 1);
        this_->need_restore = 1;
    }

    return 0;
}

TRIBE_Panel_Screen_Overlay* __stdcall TRIBE_Panel_Screen_Overlay__TRIBE_Panel_Screen_Overlay(
    TRIBE_Panel_Screen_Overlay* this_,
    TDrawArea* render_area_in,
    TPanel* parent_panel_in
)
{
    TPanel__TPanel((TPanel*)this_);
    this_->vfptr = (TPanelVtbl*)TRIBE_Panel_Screen_Overlay__vftable;

    TPanel__setup((TPanel*)this_, render_area_in, parent_panel_in, 0, 0, 0, 0, 0);

    this_->ImageBuffer = NULL;
    this_->image_clip_region = NULL;
    this_->user_callbacks = { NULL };
    this_->user_data = NULL;

    this_->vfptr->set_active((TPanel*)this_, 1);
    this_->vfptr->set_redraw((TPanel*)this_, 1);
    //this_->need_restore = 1;

    return this_;
}

void TRIBE_Panel_Screen_Overlay__register_callbacks(TRIBE_Panel_Screen_Overlay* this_, const TRIBE_Panel_Screen_Overlay_User_Callbacks& user_callbacks)
{
    this_->user_callbacks = user_callbacks;
}

std::vector<TRIBE_Panel_Screen_Overlay*> created_overlays;
std::vector<TRIBE_Panel_Screen_Overlay_User_Callbacks> registered_overlays;

void __stdcall create_overlay_panels(TDrawArea* render_area_in, TRIBE_Screen_Game* game_screen)
{
    for (auto it = registered_overlays.begin(); it != registered_overlays.end(); ++it)
    {
        TRIBE_Panel_Screen_Overlay* overlay = (TRIBE_Panel_Screen_Overlay*)operator_new_internal(sizeof(TRIBE_Panel_Screen_Overlay));
        created_overlays.push_back(overlay);
        TRIBE_Panel_Screen_Overlay__TRIBE_Panel_Screen_Overlay(overlay, render_area_in, (TPanel*)game_screen->main_view);
        TRIBE_Panel_Screen_Overlay__register_callbacks(overlay, *it);
        overlay->user_data = overlay->user_callbacks.create();
    }
}

void __stdcall destroy_overlay_panels()
{
    for (auto it = created_overlays.begin(); it != created_overlays.end(); ++it)
    {
        (*it)->vfptr->__vecDelDtor((TPanel*)*it, 1);
    }
    created_overlays.clear();
}

void __stdcall handle_overlay_size()
{
    for (auto it = created_overlays.begin(); it != created_overlays.end(); ++it)
    {
        TRIBE_Panel_Screen_Overlay* overlay = *it;
        panel_size size = overlay->user_callbacks.handle_size(overlay);
        TPanel__set_positioning((TPanel*)overlay, 7,
            size.left_border_in, size.top_border_in, size.right_border_in, size.bottom_border_in,
            size.min_wid_in, size.max_wid_in, size.min_hgt_in, size.max_hgt_in,
            0, 0, 0, 0);

        TRIBE_Panel_Screen_Overlay__destroy_image_buffer(overlay);
        overlay->ImageBuffer = (TDrawArea*)operator_new_internal(sizeof(TDrawArea));
        TDrawArea__TDrawArea(overlay->ImageBuffer, "overlay draw area", 0);
        TDrawArea__Init(overlay->ImageBuffer, overlay->render_area->DrawSystem, overlay->pnl_wid-1, overlay->pnl_hgt-1, 0, 0);
        overlay->ImageRect = { 0, 0, overlay->pnl_wid, overlay->pnl_hgt };
        overlay->image_clip_region = CreateRectRgn(0, 0, overlay->pnl_wid-1, overlay->pnl_hgt-1);

        overlay->need_restore = 1;
    }
}

__declspec(naked) void on_create_overlay_panels() //004F364F
{
    __asm
    {
        push    esi
        push    edx
        call    create_overlay_panels
        mov     ecx, edi
        call    TRIBE_Panel_Inven__TRIBE_Panel_Inven
        mov     ecx, 004F365Ah
        jmp     ecx
    }
}

__declspec(naked) void on_destroy_overlay_panels() //004F5E9D
{
    __asm
    {
        call    destroy_overlay_panels
        mov     ecx, [esi + 10C8h]
        mov     eax, 004F5EA3h
        jmp     eax
    }
}

__declspec(naked) void on_game_screen_handle_size() //004F6A42
{
    __asm
    {
        call    dword ptr [edx + 24h]
        call    handle_overlay_size
        mov     ecx, [esi + 18h]
        mov     edx, 004F6A48h
        jmp     edx
    }
}

void register_screen_overlay(const TRIBE_Panel_Screen_Overlay_User_Callbacks& user_callbacks)
{
    registered_overlays.emplace_back(user_callbacks);
}

#pragma optimize( "s", on )
void setOverlayHooks()
{
    setHook((void*)0x004F364F, on_create_overlay_panels);
    setHook((void*)0x004F5E9D, on_destroy_overlay_panels);
    setHook((void*)0x004F6A42, on_game_screen_handle_size);
}
#pragma optimize( "", on )

/*
void* overlay_ptr = NULL;

int pos_x = 400;
int pos_y = 600;
int size_x = 1000;
int size_y = 300;

HBRUSH br_black;
bool brushes_loaded = false;

bool overlay_state = false;

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
        DrawTextA(hdc, text.c_str(), text.length(), &r, NULL);
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
