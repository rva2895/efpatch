#pragma once
#include "stdafx.h"

class FUNCTION_HOOK
{
private:
    uint8_t old_start[6];
    void* address;
    void* f_on_call_data;
    void* f_on_return_data;
    void* original_return_address;
    long long call_count;

    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    long long total_time;

public:
    explicit FUNCTION_HOOK(void*);
    void* get_address();
    void* get_original_return_address();
    long long get_call_count();
    long long get_total_time();

    void before_call(void**);
    void after_call();

    void set_zero_call_count();

    bool operator<(const FUNCTION_HOOK&) const;
};

void setFunctionListHooks();
