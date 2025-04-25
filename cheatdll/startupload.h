#pragma once

bool setStartupLoadHooks();
char* __stdcall load_game_or_scen_abs_path_sub(bool scen, char* dest, const char* name);
