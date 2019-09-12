#pragma once

void initLog();
void closeLog();
void flushLog();
void __cdecl log(const char* format, ...);
void __cdecl log_internal(const char* format, ...);

//#define CHEATDLL_NOLOG
