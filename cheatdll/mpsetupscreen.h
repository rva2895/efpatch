#pragma once

#pragma pack(push, 8)
struct TribeMPSetupScreenExtraOptionsDialog
{
    TDialogPanelVtbl* vfptr;
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
    int ideal_width;
    int ideal_height;
    char info_file_name[260];
    int info_id;
    TShape* background_pic;
    TShape* background_pic2;
    HPALETTE palette;
    char cursor_file[260];
    int cursor_id;
    int background_pos;
    int use_bevels;
    int use_outline_bevels;
    unsigned __int8 bevel_color1;
    unsigned __int8 bevel_color2;
    unsigned __int8 bevel_color3;
    unsigned __int8 bevel_color4;
    unsigned __int8 bevel_color5;
    unsigned __int8 bevel_color6;
    unsigned int text_color1;
    unsigned int text_color2;
    unsigned int focus_color1;
    unsigned int focus_color2;
    unsigned int state_color1;
    unsigned int state_color2;
    unsigned int label_color1;
    unsigned int label_color2;
    int label_style;
    char popup_info_file_name[260];
    int popup_info_id;
    TShape* button_pics;
    RGE_Color_Table* shadow_color_table;
    int shadow_amount;
    unsigned __int8 background_color1;
    unsigned __int8 background_color2;
    int enable_ime;
    unsigned __int8 help_mode;
    int stock_brush;
    HBRUSH brush;
    unsigned int brush_color;
    TDrawArea* shadow_area;
    int allow_shadow_area;
    int saved_mouse_mode;
    int rollover_panel_always_active;
    TTextPanel* rollover_text_panel;
    int rollover_num;
    TPanel* rollover_panel[70];
    int rollover_string[70];
    int rollover_sound[70];
    TPanel* last_rollover_panel;
    int popup_dialogs_use_parent_text_colors;
    HWND dlg_wnd;
    TPanel* save_parent_child;
    TPanel* save_parent;
    int text_font_id;
    int input_font_id;
    int button_font_id;
    int button_sound_id;

    //TribeMPSetupScreenExtraOptionsDialog specific fields below

    TTextPanel* title;
    TTextPanel* option_allow_spectators_title;
    TTextPanel* option_hide_names_title;

    TButtonPanel* option_allow_spectators_button;
    TButtonPanel* option_hide_names_button;

    TButtonPanel* ok_button;
    TButtonPanel* cancel_button;
};
#pragma pack(pop)

struct EXTRA_GAME_OPTIONS
{
    bool allow_spectators;
    bool hide_names;
    uint32_t magic;
};

#define EXTRA_OPTIONS_MAGIC 0x669955AAu

void setMPSetupScreenHooks(int ver);
