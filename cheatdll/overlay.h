#pragma once

struct panel_size
{
    int left_border_in;
    int top_border_in;
    int right_border_in;
    int bottom_border_in;
    int min_wid_in;
    int max_wid_in;
    int min_hgt_in;
    int max_hgt_in;
};

struct TRIBE_Panel_Screen_Overlay_User_Callbacks
{
    void (*render_to_image_buffer)(void* user_data, TDrawArea* render_area, RECT* render_rect, HRGN clip_region);
    bool (*need_redraw)(void* user_data);
    panel_size (*handle_size)(void* user_data);
    void* (*create)();
    void (*destroy)(void* user_data);
};

#pragma pack(push, 8)
struct TRIBE_Panel_Screen_Overlay
{
    TPanelVtbl* vfptr;
    TPanel* previousPanelValue;
    TPanel* previousModalPanelValue;
    int pnl_x;
    int pnl_y;
    int pnl_wid;
    int pnl_hgt;
    char* panelNameValue;
    TDrawArea* render_area;
    RECT clip_rect;
    TPanel__PositionMode position_mode;
    TPanel__RedrawMode need_redraw;
    TPanel* curr_child;
    TPanel* parent_panel;
    TPanel* left_panel;
    TPanel* top_panel;
    TPanel* right_panel;
    TPanel* bottom_panel;
    PanelNode* node;
    PanelNode* first_child_node;
    PanelNode* last_child_node;
    TPanel* tab_prev_panel;
    TPanel* tab_next_panel;
    int mouse_captured;
    int active;
    int visible;
    int tab_stop;
    int have_focus;
    int overlapping_children;
    int handle_mouse_input;
    int just_drawn;
    int enabled;
    HRGN clip_rgn;
    RECT render_rect;
    int left_border;
    int top_border;
    int right_border;
    int bottom_border;
    int min_wid;
    int max_wid;
    int min_hgt;
    int max_hgt;
    int mouse_hold_interval;
    int mouse_move_tolerance;
    int mouse_down_x;
    int mouse_down_y;
    int mouse_down_ctrl;
    int mouse_down_shift;
    unsigned int mouse_down_time;
    int error_code;
    int z_order;
    int display_changed_count;
    int help_string_id;
    int help_page_id;
    unsigned __int8 mouse_action;
    unsigned __int8 mouse_down_button;
    unsigned __int8 panel_type;
    unsigned __int8 color;
    unsigned __int8 fill_in_background;
    unsigned __int8 clip_to_parent;
    unsigned __int8 draw_rect2_flag;
    unsigned __int8 need_restore;

    //TRIBE_Panel_Screen_Overlay specifics below

    TDrawArea* ImageBuffer;
    RECT ImageRect;
    RECT ScreenRect;
    HRGN image_clip_region;

    TRIBE_Panel_Screen_Overlay_User_Callbacks user_callbacks;
    void* user_data;
};
#pragma pack(pop)

void setOverlayHooks();
//void overlay_switch();


void register_screen_overlay(const TRIBE_Panel_Screen_Overlay_User_Callbacks& user_callbacks);
