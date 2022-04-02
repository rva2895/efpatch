#pragma once
#include "structs.h"

#ifdef VOOBLY_EF
#define DATA_FOLDER_PREFIX_FROM_ROOT "Voobly Mods\\SWBGCC\\Data Mods\\Expanding Fronts Data\\Data\\"
#define DATA_FOLDER_PREFIX_FROM_DATA "..\\Voobly Mods\\SWBGCC\\Data Mods\\Expanding Fronts Data\\Data\\"
#else
#define DATA_FOLDER_PREFIX_FROM_ROOT "data\\"
#define DATA_FOLDER_PREFIX_FROM_DATA ""
#endif

#ifdef TARGET_VOOBLY
#define DLL_NAME "userpatch.dll"
#else
#define DLL_NAME "efpatch.dll"
#endif

void __cdecl writeByte(DWORD addr, BYTE val);
void __cdecl writeWord(DWORD addr, WORD val);
void __cdecl writeDword(DWORD addr, DWORD val);
void __cdecl writeByteF(DWORD addr, BYTE val);
void __cdecl writeDwordF(DWORD addr, DWORD val);
void __cdecl setHook(void* addr, void* newAddr);
void __cdecl writeData(DWORD addr, const void* data, int len);
//void __cdecl setHookRestorable (void* addr, void* newAddr, void* oldData);
//void __cdecl restoreHook (void* addr, void* oldData);

void* __stdcall getCurrentPlayer();
void* __fastcall get_player(int);

int __stdcall language_dll_load(UINT id, char* buf, int nmax);

errno_t __cdecl strcpy_safe(char* dest, size_t size, const char* source);

int __stdcall getWindowX();
int __stdcall getWindowY();

int __stdcall get_sn(void* player, int sn);

int __stdcall player_get_n_selection(void* player);
UNIT** __stdcall player_get_selection(void* player);

struct BYTE_ASSIGN
{
    DWORD  addr;
    BYTE val;
};

struct INT_ASSIGN
{
    DWORD  addr;
    DWORD  val;
};

struct SET_HOOK_ASSIGN
{
    void* addr;
    void* newAddr;
};

void __stdcall sendChat(char* s, int p);
void __cdecl chat(char* format, ...);

float __stdcall player_get_camera_x(void* player);
float __stdcall player_get_camera_y(void* player);

void* __stdcall get_TRIBE_Command();

void* __stdcall get_main_view();
void __stdcall unit_change_ownership(UNIT* unit, void* new_owner);

void* __stdcall get_top_panel();

int __stdcall getMapSize();

extern void** const base_game;
extern void** const panel_system;

extern void** const comm;
extern void** const chat_p;


extern void* (__thiscall* const BaseWorld__object)(void* this_, int oID);

extern void* (__thiscall* const BaseGame__get_player)(void* globalPtr);
extern int (__thiscall* const BaseGame__playerID)(void* this_, int player_index);
extern bool (__thiscall* const BaseGame__allowCheatCodes)(void* this_);
extern bool (__thiscall* const BaseGame__singlePlayerGame)(void* this_);
extern bool (__thiscall* const BaseGame__getRecordGame)(void* this_);
extern void (__thiscall* const BaseGame__setRecordGame)(void* this_, bool v);

extern void (__thiscall* const Game__show_status_message)(void* this_, char* messageIn, char* info_file, int info_id, int show_settings, int use_logo_background);
extern void (__thiscall* const Game__close_status_message)(void* this_);
extern void (__thiscall* const Game__set_player)(void* this_, __int16 new_player_id);
extern int (__thiscall* const Game__playerColor)(void* this_, int player_num);

extern void (__thiscall* const RGE_Map__set_terrain)(void* this_, void* obj_owner, void* gworld, __int16 mapcol1, __int16 maprow1, __int16 mapcol2, __int16 maprow2, unsigned __int8 terrain, unsigned __int8 set_flag, int delete_obj);

extern void* (__thiscall* const GameScreen__find_next_idle_unit)(void* this_, int last_object_id);
extern void* (__thiscall* const GameScreen__find_next_idle_military_unit)(void* this_, int last_object_id);

extern void* (__thiscall* const WorldPlayer__find_obj)(void* this_, __int16 obj_id, void* after_obj, __int16 obj_id_2);

extern int (__thiscall* const WorldPlayerBase__select_object)(void* this_, void* unit, int play_sound);
extern int (__thiscall* const WorldPlayerBase__select_one_object)(void* this_, void* unit, int play_sound);
extern int (__thiscall* const WorldPlayerBase__unselect_object)(void* this_);
extern int (__thiscall* const WorldPlayerBase__set_view_loc)(void* player, float x, float y, int spectatingView);

extern int (__thiscall* const GameSoundEffectsManager__playSound)(void* this_, int soundId, int pan, int volume);

extern int (__thiscall* const RGE_Command__submit)(void* command, void* order, int order_size, int issuer);

extern void* (__cdecl* const calloc_internal)(size_t number, size_t size);
extern void (__cdecl* const free_internal)(void* memory);

extern int (__thiscall* const unit_detach)(UNIT* unit);
extern void __stdcall kill_unit(UNIT* unit);

extern void (__fastcall* const deflate_write)(void* outfile, void* buffer, unsigned int size);
extern void (__fastcall* const deflate_read)(void* infile, void* buffer, unsigned int size);

extern int (__thiscall* const RGE_View__display_object_selection)(void* this_, int id, int duration, int select_type, int reset_type);

extern int (__thiscall* const TPanelSystem__destroyPanel)(void* this_, char* n);
extern void (__thiscall* const TEasy_Panel__popupOKDialog)(void* this_, char* text, char* panel_title, int wid, int hgt, int centered);
