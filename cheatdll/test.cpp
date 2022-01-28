#include "stdafx.h"

#include "test.h"
#include "log.h"
#include "advcheat.h"
#include "rundll.h"
#include "oos.h"
#include "functionlist.h"
#include "worlddump.h"

#include <process.h>
#include <MMSystem.h>
#include <vector>
#include <string>
#include <map>
#include <time.h>
#include <algorithm>

#include "resfile.h"

extern RESFILE resfile;

int current_worldtime = 0;

#ifdef _DEBUG

int effectsFired = 0;
int triggerBegun = 0;
char triggerText[256];

void __cdecl effectLog(int n, char* ef)
{
    if (triggerBegun)
    {
        log("%s", triggerText);
        triggerBegun = 0;
    }
    log("---> Effect %d (%d)", n, *(int*)(ef + 4));
}

__declspec(naked) int effectLogHook() //005F4A77
{
    __asm
    {
        mov     eax, [edx]
        mov     ecx, [ecx + edi * 4]
        push    eax
        push    ecx
        push    edi
        call    effectLog
        pop     edi
        pop     ecx
        push    005F4A7Dh
        ret
    }
}

void __cdecl triggerLog(int n, char* tr)
{
    triggerBegun = 1;

    char szName[] = "Untitled";
    //*szName = 0;
    char* name;
    if (*(char**)(tr + 0x14))
        name = *(char**)(tr + 0x14);
    else
        name = szName;

    if (*tr == 1)
        sprintf(triggerText, "Trigger %d (name: %s; %d conditions, %d effects", n,
            name, *(int*)(tr + 0x20), *(int*)(tr + 0x30));
}

__declspec(naked) int triggerLogHook() //005F54D2
{
    __asm
    {
        mov     ecx, [esi + 10h]
        push    ebx
        mov     ecx, [ecx + edi * 4]
        push    ecx
        push    edi
        call    triggerLog
        pop     edi
        pop     ecx
        push    005F54D9h
        ret
    }
}
#endif

int retSave;
int data;

const char title[] = "DAT";
const char text[] = "read";

__declspec(naked) int onReadDat()
{
    __asm
    {
        mov     ecx, data
        mov     ecx, [ecx]
        cmp     cx, 9852
        jnz     _end
        //int     3
        //push    eax
        //push    0
        //push    offset title
        //push    offset text
        //push    0
        //call    ds:[MessageBoxA]
        //pop     eax
_end:
        mov     ecx, retSave
        jmp     ecx
    }
}

__declspec(naked) int readDatHook() //004D5550
{
    __asm
    {
        mov     eax, [esp]
        mov     retSave, eax
        mov     data, edx
        mov     eax, onReadDat
        mov     [esp], eax
        push    ebp
        mov     ebp, esp
        sub     esp, 0Ch
        push    004D5556h
        ret
    }
}

FILE* rms_f = NULL;

__declspec(naked) void rms_fptr() //004E1801
{
    __asm
    {
        mov     rms_f, eax
        mov     eax, [ebp + 45E28h]
        mov     edi, 004E1807h
        jmp     edi
    }
}

__declspec(naked) void rms_fptr_close() //004E1951
{
    __asm
    {
        xor     eax, eax
        mov     rms_f, eax
        mov     eax, [ebp + 3F6FCh]
        mov     ecx, 004E1957h
        jmp     ecx
    }
}

__int32 (__cdecl* ftell2)(FILE *File) =
    (__int32 (__cdecl*) (FILE*))0x006361B6;

int (__cdecl* fseek2)(FILE *File, __int32 Offset, int Origin) =
    (int (__cdecl*) (FILE*, __int32, int))0x00635E6D;

size_t (__cdecl* fread2)(void *DstBuf, size_t ElementSize, size_t Count, FILE *File) =
    (size_t (__cdecl*) (void*, size_t, size_t, FILE*))0x006360CE;

int (__cdecl* fscanf2)(FILE *File, const char *Format, ...) =
    (int (__cdecl*) (FILE*, const char*, ...))0x0063481F;

char* (__cdecl* fgets2)(char *Buf, int MaxCount, FILE *File) =
    (char* (__cdecl*) (char*, int, FILE*))0x0063303A;

extern std::string rms_error_1;
extern std::string rms_error_2;

extern bool isEditor;

extern bool editorstatus_isValid;
bool rms_first_error = false;

char* log_int_s = NULL;
__declspec(noinline) void __cdecl log_int(int unk1, char* fmt, ...)
{
    UNREFERENCED_PARAMETER(unk1);
    va_list ap;
    __int32 pos;
    //void* d;
    int lines;
    if (!log_int_s)
        log_int_s = (char*)malloc(0x200);
    
    if (!(((unsigned long)fmt >= 0x00689000) && ((unsigned long)fmt < 0x7A3A2C)))
        return;

    size_t len = strlen(fmt);

    if ((len < 8) || (len > 0x180))
        return;

    if (!strncmp("Act_Art", fmt, 7))
        return;

    //
    char b[0x100];
    bool rms_flag = false;

    if (!strcmp(fmt, "------Opening script (%s)."))
    {
        //rms_begin = true;
        rms_error_1 = "RMS: No errors";
        rms_error_2.clear();
        if (isEditor)
            update_editor_bk();
        rms_first_error = true;
    }

    if (rms_f && (rms_f != (FILE*)-1))
    {
        pos = ftell2(rms_f);
        if (pos > 0)
        {
            //d = malloc(pos);
            fseek2(rms_f, 0, SEEK_SET);
            lines = 0;
            do
            {
                fgets2(log_int_s, 0x200, rms_f);
                lines++;
            } while (ftell2(rms_f) < pos);

            //free(d);
            fseek2(rms_f, pos, SEEK_SET);
            log("** RMS, line %d:\n\n%s", lines, log_int_s);
            if (rms_first_error)
            {
                sprintf(b, "RMS: line %d: %s", lines, log_int_s);
                rms_error_1 = b;
                rms_flag = true;
                rms_first_error = false;
            }
        }
    }
    //

    va_start(ap, fmt);
    vsprintf(log_int_s, fmt, ap);
    log_internal("%s", log_int_s);
    if (rms_flag)
    {
        sprintf(b, "%s", log_int_s);
        rms_error_2 = b;
        rms_flag = false;
    }
    va_end(ap);
}

void* log_int_retval;

__declspec(naked) void log_int_end_wr()
{
    __asm
    {
        mov     eax, log_int_retval
        sub     esp, 4
        jmp     eax
    }
}

__declspec(naked) void log_int_wr()
{
    __asm
    {
        mov     eax, [esp + 8]
        cmp     eax, 00689000h
        jb      _fix_log_ret
        cmp     eax, 007A3A2Ch
        jnb     _fix_log_ret
        jmp     log_int
_fix_log_ret:
        pop     eax
        mov     log_int_retval, eax
        mov     eax, log_int_end_wr
        mov     [esp], eax
        jmp     log_int
        ret
    }
}

__declspec(naked) void nullsub_26()
{
    __asm
    {
        ret
    }
}

int check_file(char* file, int player)
{
    char s[0x100];
    sprintf(s, "Checking %s...", file);
    sendChat(s, player);
    FILE* f = fopen(file, "rb");
    if (!f)
    {
        sprintf(s, "Failed to open %s!", file);
        sendChat(s, player);
        return 0;
    }
    char b;
    char sum = 0;
    while (fread(&b, 1, 1, f) > 0)
        sum += b;
    fclose(f);
    sprintf(s, "File %s: %d", file, sum);
    sendChat(s, player);
    return sum;
}

void thread_proc(void* p)
{
    int player = (int)p;
    int sum = 0;
    sum += check_file("battlegrounds_x1.exe", player);
    sum += check_file(DLL_NAME, player);
    sum += check_file("language.dll", player);
    sum += check_file("language_x1.dll", player);
    sum += check_file("language_x2.dll", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"gamedata.drs", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"gamedata_x1.drs", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"gamedata_x2.drs", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"expl.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"ground-to-air.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"jedi-holo.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"master.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"padawan.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"resgen.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"unconv.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"terrain.txt", player);
    char s[0x100];
    sprintf(s, "Integrity check complete, checksum = %d", sum);
    sendChat(s, player);
}

//extern int memory_temp;

int __fastcall get_gametime2()
{
    void* game_screen = *(void**)((uint8_t*)(*base_game) + 0x17B4);
    if (game_screen)
    {
        void* world = *(void**)((uint8_t*)game_screen + 0x126C);
        if (world)
            return *(int*)((uint8_t*)world + 0x10);
        else
        {
            chat("World does not exist");
            return -1;
        }
    }
    else
    {
        chat("Game screen does not exist");
        return -1;
    }
}

//DWORD performance_time = 0;

extern float* __fastcall player_getResources2(void*);
extern void __stdcall make_oos_dump();

//extern std::vector<FUNCTION_HOOK*> function_hooks;

struct
{
    bool operator() (FUNCTION_HOOK* fh1, FUNCTION_HOOK* fh2) const
    {
        return !(*fh1 < *fh2);
    }
} compare_call_counts_object;

extern unsigned int dump_objects(const char* filename);
extern int max_worldtime;

int __stdcall onChat_2(int player_id, char* targets, char* s)
{
    UNREFERENCED_PARAMETER(targets);
    UNREFERENCED_PARAMETER(player_id);
    
    if (!strcmp(s, "/version"))
    {
        sendChat(EFPATCH_VERSION, -1);
        return 1;
    }
    /*else if (!strcmp(s, "/test"))
    {
        sendChat("Started integrity check", player_id);
        _beginthread(thread_proc, 0, (void*)player_id);
        return 1;
    }
    else if (!strcmp(s, "/memory"))
    {
        sendChat("Memory: set to 0x100000", player_id);
        memory_temp = 0x100000;
        return 1;
    }*/
    
    /*else if (!strcmp(s, "/dump-world"))
    {
        srand(timeGetTime());
        unsigned int r = rand();
        char name[MAX_PATH];
        sprintf(name, "rge_dump_%08X.txt", r);
        chat("Dumping world to %s ...", name);
        dump_objects(name);
        chat("Dump complete");
        return 1;
    }
    else if (!strcmp(s, "/worldtime"))
    {
        chat("Worldtime = %d", get_gametime2());
        return 1;
    }
    
    else if (strstr(s, "/set-max"))
    {
        char d[0x100];
        int t;
        sscanf(s, "%s %d", d, &t);
        max_worldtime = t;
        chat("Set max worldtime to %d", t);
        return 1;
    }*/
    
    /*
    else if (!strcmp(s, "/make-oos"))
    {
        float* r = player_getResources2(get_player(0));
        r[0] += 100.0f;
        chat("Caused out of sync");
        return 1;
    }
    else if (!strcmp(s, "/dump-all"))
    {
        make_oos_dump();
        return 1;
    }
    */
    
    /*else if (strstr(s, "/obj") || strstr(s, "/object"))
    {
        char d[0x100];
        int id;
        sscanf(s, "%s %d", d, &id);
        void* base_world = *(void**)((char*)*base_game + 0x420);
        if (base_world)
        {
            UNIT* unit = (UNIT*)BaseWorld__object(base_world, id);
            if (unit)
            {
                void* player = getCurrentPlayer();
                WorldPlayerBase__unselect_object(player);
                WorldPlayerBase__select_object(player, unit, 1);
            }
            else
                chat("Invalid object id");
        }

        return true;
    }*/
    
    /*else if (strstr(s, "/load-all"))
    {
        int ext_types[] = {0x736C7020, 0x77617620, 0x62696E61};
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 100000; j++)
            {
                int size;
                resfile.get_resource(ext_types[i], j, &size);
            }
        }
        chat("Loaded all DRS resources");
        return 1;
    }*/
    
    /*
    else if (strstr(s, "/cs"))
    {
        WORLD_DUMP wd;
        wd.update_cs();
        chat("CS=%u", wd.get_cs());
        return true;
    }
    /*else if (!strcmp(s, "/random-test"))
    {
        chat("Testing random generator...");
        do_random_test();
        return 1;
    }*/
    /*else if (!strcmp(s, "/set-zero-count"))
    {
        for (auto it = function_hooks.begin(), it_end = function_hooks.end(); it != it_end; ++it)
            (*it)->set_zero_call_count();
        chat("Set call counts to zero");
        return 1;
    }
    else if (!strcmp(s, "/print-call-count"))
    {
        srand(timeGetTime());
        unsigned int r = rand();
        char name[MAX_PATH];
        sprintf(name, "call_count_%08X.txt", r);
        //chat("Sorting vector ...");
        std::sort(function_hooks.begin(), function_hooks.end(), compare_call_counts_object);
        FILE* f = fopen(name, "wt");
        if (f)
        {
            for (auto it = function_hooks.begin(), it_end = function_hooks.end(); it != it_end; ++it)
                fprintf(f, "0x%X - %lld - %lld ns\n", (*it)->get_address(), (*it)->get_call_count(), (*it)->get_total_time());
            fclose(f);
            chat("Printed call counts to %s", name);
        }
        else
            chat("Error writing file!");
        return 1;
    }*/
    /*else if (!strcmp(s, "/time-start"))
    {
        performance_time = timeGetTime();
        chat("Set start time");
        return 1;
    }
    else if (!strcmp(s, "/time-stop"))
    {
        chat("Elapsed %d ms", timeGetTime() - performance_time);
        return 1;
    }
    else if (!strcmp(s, "/render-off"))
    {
        chat("Render OFF");
        unsigned char instr = 0xC3;
        SIZE_T w;
        WriteProcessMemory(GetCurrentProcess(), (void*)0x00651BC0, &instr, 1, &w);
        return 1;
    }
    else if (!strcmp(s, "/render-on"))
    {
        chat("Render ON");
        unsigned char instr = 0x55;
        SIZE_T w;
        WriteProcessMemory(GetCurrentProcess(), (void*)0x00651BC0, &instr, 1, &w);
        return 1;
    }*/
    else
        return 0;
}

__declspec(naked) void onChat() //00438140
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 10h]
        push    eax
        mov     eax, [esp + 10h]
        push    eax
        mov     eax, [esp + 10h]
        push    eax
        call    onChat_2
        pop     ecx
        test    eax, eax
        jnz     __skip_chat
        sub     esp, 118h
        mov     eax, 00438146h
        jmp     eax
__skip_chat:
        xor     eax, eax
        ret     0Ch
    }
}

void __stdcall rms_token_log(char char5, char char4, char char3, char char2, int id, char char1, char* token, void* caller)
{
    log("RMS: (%d, %d, %d, %d, %d) - @0x%p: %d = %s", (int)char1, (int)char2, (int)char3, (int)char4, (int)char5, caller, id, token);
}

__declspec(naked) void onRmsToken() //004E1D40
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        push    ecx
        mov     eax, [ebp + 4] //caller
        mov     ecx, [ebp + 8] //token
        push    eax
        push    ecx
        mov     eax, [ebp + 0Ch] //char1
        mov     ecx, [ebp + 10h] //id
        push    eax
        push    ecx
        mov     eax, [ebp + 14h] //char2
        mov     eax, [ebp + 18h] //char3
        push    eax
        push    ecx
        mov     eax, [ebp + 1Ch] //char4
        mov     eax, [ebp + 20h] //char5
        push    eax
        push    ecx
        call    rms_token_log
        pop     ecx
        leave

        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     edi, [esp + 14h]
        mov     eax, 004E1D48h
        jmp     eax
    }
}

void fix_function_call(DWORD addr, DWORD f)
{
    writeDword(addr, f - (addr + 4));
}

/*
std::vector<void*> addresses;

std::vector<void*> adr_test;

void __stdcall add_addr(void* a)
{
    for (auto it = addresses.begin(); it != addresses.end(); ++it)
        if (*it == a)
            return;
    addresses.push_back(a);
}

extern bool time_collect;
extern bool time_stage_find;

int left;
int right;

volatile bool answer_flag = false;
volatile bool answer = false;

void msg_box(const char* s, int formal)
{
    UNREFERENCED_PARAMETER(formal);
    MessageBox(0, s, "Message", 0);
}

bool check(int l, int r)
{
    left = l;
    right = r;
    answer_flag = false;
    char s[0x100];
    sprintf(s, "Checking %d - %d, condition met? [/yes] [/no]", l, r);
    msg_box(s, 0);
    while (!answer_flag)
        ;
    return answer;
}

// A iterative binary search function. It returns 
// location of x in given array arr[l..r] if present, 
// otherwise -1 
int search_function(const std::vector<void*>& arr)
{
    int l = 0;
    int r = arr.size() - 1;
    while (l <= r)
    {
        int m = l + (r - l) / 2;

        // Check if x is present at mid 
        
        if (check(m, m))
            return m;

        // If x greater, ignore left half 
        if (check(m, r))
            l = m + 1;

        // If x is smaller, ignore right half 
        else
            r = m - 1;
    }

    // if we reach here, then element was 
    // not present 
    return -1;
}

DWORD adr_time;
BYTE jump_time[6];
BYTE old_time[6];

DWORD adr_text;
BYTE jump_text[6];
BYTE old_text[6];

DWORD last_call = 0;

int __stdcall Intercept_DrawTextA(HDC hdc, LPCSTR s, int c, LPRECT r, UINT format)
{
    //DrawTextA()
    DWORD written;
    WriteProcessMemory(GetCurrentProcess(), (void*)adr_text,
        (void*)old_text, 6, &written);

    if (c == -1)
        c = strlen(s);

    int x = r->left;
    int y = r->top;
    if (format & DT_BOTTOM)
        y = r->bottom - 15;

    //if (strstr(s, "for the"))
    //    __debugbreak();

    int retval;
    if (format & DT_RIGHT)
        retval = DrawTextA(hdc, s, c, r, format);
    else
        retval = TextOutA(hdc, x, y, s, c);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
        (void*)jump_text, 6, &written);

    return retval;
}

void interceptTextOut()
{
    DWORD written;
    HINSTANCE hinst = GetModuleHandle("user32.dll");
    adr_text = (DWORD)GetProcAddress(hinst, "DrawTextA");

    log("DrawTextA found at %X", adr_text);

    jump_text[0] = 0x68;
    *(DWORD*)(jump_text + 1) = (DWORD)&Intercept_DrawTextA;
    jump_text[5] = 0xC3;

    ReadProcessMemory(GetCurrentProcess(), (void*)adr_text,
        (void*)old_text, 6, &written);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
        (void*)jump_text, 6, &written);
}
*/

/*
float old_val;
float new_val;

float glitched_res = 0;

void __stdcall check_res()
{
    if (old_val < new_val)
        glitched_res += (new_val - old_val);
}

__declspec(naked) void repair_test() //005D0D59
{
    __asm
    {
        fmul    dword ptr [esp+10h]
        fsubr   dword ptr [ecx]
        
        push    eax
        push    ecx
        push    edx
        mov     eax, [ecx]
        mov     old_val, eax

        fstp    dword ptr [ecx]
        
        mov     eax, [ecx]
        mov     new_val, eax

        call    check_res

        pop     edx
        pop     ecx
        pop     eax

        push    005D0D61h
        ret
    }
}
*/

/*struct function_data
{
    BYTE original_data[11];
};

//std::map<std::pair<DWORD, function_data>> functions;
std::map<DWORD, function_data> functions;

DWORD __stdcall record_function_call(DWORD caller, DWORD function)
{

}

DWORD __stdcall record_function_return()
{

}

__declspec(naked) void on_function_return()
{
    __asm
    {

    }
}

__declspec(naked) void on_function_call()
{
    __asm
    {
        push    ecx
        push    edx
        mov     eax, [esp + 0Ch] //function
        mov     ecx, [esp + 10h] //caller
        push    eax
        push    ecx
        call    record_function_call
        push    eax
        push    edi
        push    esi
        push    0C8763E65h
        sub     esp, 100h
        mov     [esp], on_function_return
        lea     esi, [esp + 120h]
        lea     edi, [esp + 4]
        mov     ecx, 60
        movsd
        mov     eax, [esp + 118h]
        jmp     eax
    }
}

void __stdcall add_function(DWORD addr)
{
    function_data fd;
    DWORD r;
    ReadProcessMemory(GetCurrentProcess(), (void*)addr, fd.original_data, 11, &r);
    writeByte(addr, 0xE8); //call
    writeDword(addr + 1, 0); //$+0
    writeByte(addr + 5, 0x68); //push
    writeDword(addr + 6, (DWORD)&on_function_call); //address
    writeByte(addr + 10, 0xC3); //ret
}*/

//int control_source;
//int control_target = 1;
//bool control_initiated = false;

//for takecontrol
/*__declspec(naked) void onSync() //00432DFF
{
    __asm
    {
        mov      al, control_initiated
        test     al, al
        jz       _no_control
        push     ecx
        push     control_source
        call     takeControl
        pop      ecx
        mov      eax, 004496A0h
        call     eax
        mov      eax, control_target
        push     eax
        call     takeControl
        mov      eax, 00432E04h
        jmp      eax
_no_control:
        mov      eax, 004496A0h
        call     eax
        mov      eax, 00432E04h
        jmp      eax
    }
}*/

__declspec(naked) void onStatusScreenCreate() //005E7EC0
{
    __asm
    {
        ret     14h
    }
}

__declspec(naked) void onBlankScreenCreate() //004B4960
{
    __asm
    {
        ret     8
    }
}

__declspec(naked) int new_game_dev_mode() //0042C360
{
    __asm
    {
        mov     eax, 1
        ret
    }
}

__declspec(naked) int new_check_multi_copies() //00428270
{
    __asm
    {
        mov     eax, 1
        ret
    }
}

//const char* savegame_path = "savegame\\test\\recs\\";
const char* savegame_path = "savegame\\";

void* (__thiscall* TShape__TShape2)(void* this_, char* filename, int resource_file_id) =
    (void* (__thiscall*) (void*, char*, int))0x00542870;

int (__thiscall* TShape__shape_draw)(void* this_, void* drawarea, int x, int y, int slpFrame, void* color_table) =
    (int (__thiscall*)(void*, void*, int, int, int, void*))0x005430C0;


std::map<void*, std::pair<void*, size_t>> allocations;

void* __fastcall new_malloc(size_t size, void* from)
{
    void* memory = malloc(size);
    memset(memory, 0, size);
    allocations[memory] = std::make_pair(from, size);
    return memory;
}

void __fastcall new_free(void* memory)
{
    auto it = allocations.find(memory);
    if (it != allocations.end())
    {
        free(memory);
        allocations.erase(it);
    }
}

size_t total_mem = 0;

void memory_print()
{
    total_mem = 0;
    log("Not freed allocations:");
    for (auto it = allocations.begin(); it != allocations.end(); ++it)
    {
        log("0x%p, %u bytes", it->second.first, it->second.second);
        total_mem += it->second.second;
    }
    log("Total: %u bytes", total_mem);
}

__declspec(naked) void* __cdecl malloc_wr(size_t size) //0063328C
{
    __asm
    {
        mov     edx, [esp]
        mov     ecx, [esp + 4]
        call    new_malloc
        ret
    }
}

__declspec(naked) void* __cdecl calloc_wr(size_t number, size_t size) //00632D33
{
    __asm
    {
        mov     edx, [esp]
        mov     ecx, [esp + 4]
        imul    ecx, [esp + 8]
        call    new_malloc
        ret
    }
}

__declspec(naked) void* __cdecl new_wr(size_t size) //00632B9D
{
    __asm
    {
        mov     edx, [esp]
        mov     ecx, [esp + 4]
        call    new_malloc
        ret
    }
}

__declspec(naked) void* __cdecl free_wr(size_t size) //00632CCA
{
    __asm
    {
        mov     ecx, [esp + 4]
        call    new_free
        ret
    }
}

__declspec(naked) void* __cdecl delete_wr(size_t size) //00632B42
{
    __asm
    {
        mov     ecx, [esp + 4]
        call    new_free
        ret
    }
}

#pragma optimize( "s", on )
void setTestHook()
{
    //validate IAT 00654000 - 00654440
    
/*#ifdef TARGET_VOOBLY
    void* IAT_data = malloc(0x440);
    ReadProcessMemory(GetCurrentProcess(), (void*)0x00654000, IAT_data, 0x440, NULL);
    writeData(0x00654000, IAT_data, 0x440);
#endif*/

    //malloc log
    /*setHook((void*)0x0063328C, malloc_wr);
    setHook((void*)0x00632D33, calloc_wr);
    setHook((void*)0x00632B9D, new_wr);
    setHook((void*)0x00632CCA, free_wr);
    setHook((void*)0x00632B42, delete_wr);*/

    //pathing
    //writeDword(0x004BF23D, 0);
    //writeDword(0x004BF98E, 0);

    //setHook((void*)0x0042C360, new_game_dev_mode);
    //setHook((void*)0x00428270, new_check_multi_copies);
    //writeDword(0x00659F80, 0x004B6120);
    //setHook((void*)0x0046D9B0, (void*)0x004B6120);
    //setHook((void*)0x005E7EC0, onStatusScreenCreate);
    //setHook((void*)0x004B4960, onBlankScreenCreate);
    //srand(timeGetTime());
    
    //setOOSHooks();
    //savegame path
    writeDword(0x0048F566, (DWORD)savegame_path);

    //writeByte(0x0042FAD2, 0xEB);

    //setHook((void*)0x005D0D59, repair_test);
    //setHook((void*)0x00432DFF, onSync);

    //RMS token count
    //setHook((void*)0x004E1D40, onRmsToken);

#ifdef _DEBUG
    setHook((void*)0x005F54D2, triggerLogHook);
    setHook((void*)0x005F4A77, effectLogHook);
#endif

    //fix internal log
    writeDword(0x0040F1A6, (DWORD)nullsub_26);
    writeDword(0x0040F842, (DWORD)nullsub_26);
    writeDword(0x0040F885, (DWORD)nullsub_26);
    writeDword(0x0040F75B, (DWORD)nullsub_26);
    writeDword(0x0040EFBB, (DWORD)nullsub_26);
    writeDword(0x0057796E, (DWORD)nullsub_26);
    writeDword(0x00577A63, (DWORD)nullsub_26);
    writeDword(0x00577A89, (DWORD)nullsub_26);
    writeDword(0x00577AAF, (DWORD)nullsub_26);

    fix_function_call(0x0059B18B, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B1C6, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B1EA, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B4BE, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B524, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B56D, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B5AC, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B5D0, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B63D, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B675, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B73C, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B827, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B870, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B8AF, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B8C7, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B947, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059B9D0, (DWORD)nullsub_26); //Entering task
    fix_function_call(0x0059BACB, (DWORD)nullsub_26); //Entering task

    fix_function_call(0x004336D8, (DWORD)nullsub_26); //Starting turn
    //
    writeByte(0x004428F6, 0x90);
    writeDword(0x004428F7, 0x90909090);
    writeByte(0x0061B129, 0x90);
    writeDword(0x0061B12A, 0x90909090);
    writeByte(0x005B944D, 0x90);
    writeDword(0x005B944E, 0x90909090);
    writeByte(0x005A05DD, 0x90);
    writeDword(0x005A05DE, 0x90909090);
    writeByte(0x0050D0F3, 0x90);
    writeDword(0x0050D0F4, 0x90909090);
    writeDword(0x00410D38, 0x90909090);
    writeByte(0x00410D3C, 0x90);
    writeByte(0x0042520B, 0x90);
    writeDword(0x0042520C, 0x90909090);
    writeDword(0x004776BE, 0x90909090);
    writeByte(0x004776C2, 0x90);
    setHook((void*)0x0060F920, log_int_wr);

    setHook((void*)0x004E1801, rms_fptr);
    setHook((void*)0x004E1951, rms_fptr_close);

#ifndef TARGET_VOOBLY
    setHook((void*)0x00438140, onChat);
#endif

    //
#ifdef _DEBUG
    //setHook((void*)0x004D5550, readDatHook);
#endif
}
#pragma optimize( "", on )
