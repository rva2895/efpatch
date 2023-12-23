#include "stdafx.h";
#include "rmslog.h";
#include "rundll.h"
#include <stack>
#include <algorithm>

extern std::string rms_error_1;
extern std::string rms_error_2;

extern bool isEditor;

bool rms_first_error;

std::stack<FILE*> open_rms_files;

FILE* __stdcall open_rms_files_push(FILE* f)
{
    open_rms_files.push(f);
    return f;
}

void __stdcall open_rms_files_pop()
{
    open_rms_files.pop();
}

__declspec(naked) void rms_fptr() //004E17FF
{
    __asm
    {
        push    eax
        call    open_rms_files_push
        mov     edx, eax
        mov     eax, [ebp + 45E28h]
        mov     edi, 004E1807h
        jmp     edi
    }
}

__declspec(naked) void rms_fptr_close() //004E1951
{
    __asm
    {
        call    open_rms_files_pop
        mov     eax, [ebp + 3F6FCh]
        mov     ecx, 004E1957h
        jmp     ecx
    }
}

void do_rms_logging(const char* s)
{
    FILE* rms_f = NULL;
    if (!open_rms_files.empty())
        rms_f = open_rms_files.top();

    if (rms_f && (rms_f != (FILE*)-1) && !strncmp(s, "ERROR", 5))
    {
        __int32 pos = ftell_internal(rms_f);
        if (pos > 0)
        {
            char temp[LOG_INT_BUFFER_SIZE];
            fseek_internal(rms_f, 0, SEEK_SET);
            int lines = 0;
            do
            {
                fgets_internal(temp, LOG_INT_BUFFER_SIZE, rms_f);
                lines++;
            } while (ftell_internal(rms_f) < pos);

            fseek_internal(rms_f, pos, SEEK_SET);
            std::string temp_2(temp);
            std::replace(std::begin(temp_2), std::end(temp_2), '\t', ' ');
            log("** RMS, line %d:\n\n%s", lines, temp_2.c_str());
            if (rms_first_error)
            {
                char b[0x100];
                snprintf(b, _countof(b), "RMS: line %d: %s", lines, temp_2.c_str());
                rms_error_1 = b;
                rms_first_error = false;
                snprintf(b, _countof(b), "%s", s);
                rms_error_2 = b;
            }
        }
    }
}

void clear_rms_error()
{
    rms_error_1.clear();
    rms_error_2.clear();
}

void __stdcall setup_rms_log()
{
    clear_rms_error();
    rms_error_1 = "RMS: No errors";
    if (isEditor)
        update_editor_bk();
    rms_first_error = true;
}

const char generic_rms_error_txt[] = "ERROR: Syntax error";

void __stdcall on_command_parse_check(RGE_RMM_Script_Controller* script, FILE* infile, RGE_RMM_Token* command)
{
    unsigned __int8 result = RGE_RMM_Script_Controller__do_command(script, infile, command);
    if (!result)
    {
        log("%s", generic_rms_error_txt);
        do_rms_logging(generic_rms_error_txt);
    }
}

__declspec(naked) void on_command_parse() //004E1941
{
    __asm
    {
        push    ebp
        call    on_command_parse_check
        mov     eax, 004E1885h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setRMSLogHooks()
{
    setHook((void*)0x004E17FF, rms_fptr);
    setHook((void*)0x004E1951, rms_fptr_close);

    setHook((void*)0x004E1941, on_command_parse);
}
#pragma optimize( "", on )
