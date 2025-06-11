#pragma once

void initLog();
void closeLog();
//void flushLog();
void __cdecl log(const char* format, ...);
void __cdecl log_internal(const char* format, ...);
void __cdecl vlog(const char* format, va_list ap);
