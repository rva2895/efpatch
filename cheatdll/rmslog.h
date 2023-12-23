#pragma once

#define LOG_INT_BUFFER_SIZE 0x200

void setRMSLogHooks();
void do_rms_logging(const char* s);
void __stdcall setup_rms_log();
void clear_rms_error();
