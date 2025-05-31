#include "stdafx.h"

#include "test.h"
#include "log.h"
#include "advcheat.h"
#include "rundll.h"
#include "oos.h"
#include "functionlist.h"
#include "advtriggereffect.h"
#include "worlddump.h"
#include "rmslog.h"
#include "resfile.h"
#include "chatcommand.h"

#include <process.h>
#include <MMSystem.h>
#include <vector>
#include <string>
#include <map>
#include <time.h>
#include <algorithm>

//extern RESFILE resfile;

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
        snprintf(triggerText, _countof(triggerText), "Trigger %d (name: %s; %d conditions, %d effects", n,
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

/*
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
        //call    MessageBoxA
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
*/

char* log_int_s = NULL;

__declspec(noinline) void __cdecl log_int(int unk1, char* fmt, ...)
{
    UNREFERENCED_PARAMETER(unk1);
    va_list ap;

    if (!log_int_s)
        return;
    
    if (!(((unsigned long)fmt >= 0x00689000) && ((unsigned long)fmt < 0x7A3A2C)))
        return;

    size_t len = strlen(fmt);

    if ((len < 8) || (len > 0x180))
        return;

    if (!strncmp("Act_Art", fmt, 7))
        return;

    va_start(ap, fmt);
    vsnprintf(log_int_s, LOG_INT_BUFFER_SIZE, fmt, ap);
    log_internal("%s", log_int_s);

    do_rms_logging(log_int_s);

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
        jb      fix_log_ret
        cmp     eax, 007A3A2Ch
        jnb     fix_log_ret
        jmp     log_int
fix_log_ret:
        pop     eax
        mov     log_int_retval, eax
        mov     eax, log_int_end_wr
        mov     [esp], eax
        jmp     log_int
        ret
    }
}

__declspec(naked) void nullsub_26_2()
{
    __asm
    {
        ret
    }
}

void __stdcall RGE_Action_List__add_action2(RGE_Action_List* list, RGE_Action* action)
{
    RGE_Action_Node* node;

    if (list->obj->object_state <= 2)
    {
        node = (RGE_Action_Node*)calloc_internal(1, 8);
        node->action = action;
        node->next = list->list;
        list->list = node;
        //action->vfptr->first_in_stack(action, 1u);
    }
    else if (action)
    {
        action->vfptr->__vecDelDtor(action, 1u);
    }
}

int* const rand_val = (int* const)0x0069EC98;

int rand_no_change()
{
    int rand_save = *rand_val;
    int result = rand_internal();
    *rand_val = rand_save;
    return result;
}

bool __stdcall onChat_2(int player_id, char* targets, char* s)
{
    UNREFERENCED_PARAMETER(targets);
    UNREFERENCED_PARAMETER(player_id);

    return check_chat_command(s);
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
        test    al, al
        jnz     skip_chat
        sub     esp, 118h
        mov     eax, 00438146h
        jmp     eax

skip_chat:
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
//const char* savegame_path = "savegame\\";
//const char* scenario_path = "scenario\\";

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

void __stdcall report_dynamic_tile_cache_size_sub(int disabled, int size)
{
    log("Dynamic tile cache: disabled = %d, size = %d", disabled, size);
}

__declspec(naked) void* report_dynamic_tile_cache_size() //00497192
{
    __asm
    {
        mov     eax, [esi + 0C0D0h]
        mov     ecx, [esi + 0C0C8h]
        push    eax
        push    ecx
        call    report_dynamic_tile_cache_size_sub

        mov     eax, [esi + 0C0C8h]
        mov     ecx, 00497198h
        jmp     ecx
    }
}

#define ID_TO_WATCH 5746

void __stdcall on_check_ownership_log(RGE_Action* action, RGE_Player* player)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    RGE_Action_Object* obj = action->obj;
    if (obj && obj->id == ID_TO_WATCH)
        log(" [T = %01d:%02d:%02d.%03d] ==> check_ownership: player = %d", h, m, s, ms, player->id);
}

__declspec(naked) void on_check_ownership() //00565182
{
    __asm
    {
        mov     eax, 00565410h
        call    eax
        push    eax
        push    eax
        push    esi
        call    on_check_ownership_log
        pop     eax
        mov     ecx, 00565187h
        jmp     ecx
    }
}

void __stdcall on_update_log(RGE_Action* action)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    RGE_Action_Object* obj = action->obj;
    if (obj && obj->id == ID_TO_WATCH)
        log(" [T = %01d:%02d:%02d.%03d] => update (%d): curr_state = %d", h, m, s, ms, action->action_type, action->state);
}

__declspec(naked) void on_update() //005650F0
{
    __asm
    {
        push    esi
        mov     esi, ecx
        push    edi

        push    ecx
        call    on_update_log

        mov     ecx, [esi + 34h]
        mov     eax, 005650F7h
        jmp     eax
    }
}

void __stdcall pause_game2()
{
    TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
    if (game_screen)
        TRIBE_Screen_Game__command_pause(game_screen);
}

void __stdcall on_action_list_update_log(RGE_Action_List* action_list)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    RGE_Action_Object* obj = action_list->obj;
    if (obj && obj->id == ID_TO_WATCH)
    {
        char b[0x100];
        b[0] = '\0';
        RGE_Action_Node* nd = action_list->list;
        while (nd)
        {
            sprintf(b + strlen(b), "%d (%d) ", nd->action->action_type, nd->action->state);
            nd = nd->next;
        }
        //log(" actions: %s", b);

        wt = get_worldtime();
        //if (wt >= 201170)
            //__debugbreak();
            //pause_game2();
        log(" [T = %01d:%02d:%02d.%03d] => update_action_list: %s", h, m, s, ms, b);
    }
}

__declspec(naked) void on_action_list_update() //00406B60
{
    __asm
    {
        push    ebx
        push    edi
        mov     edi, ecx
        push    edi
        call    on_action_list_update_log
        mov     eax, [edi + 8]

        mov     ecx, 00406B67h
        jmp     ecx
    }
}

void __stdcall on_action_list_update_call_log(void* action)
{

}

__declspec(naked) void on_action_list_update_call() //00406B70
{
    __asm
    {
        mov     ecx, [eax]
        push    ecx
        push    ecx
        call    on_action_list_update
        pop     ecx
        mov     eax, [ecx]
        call    dword ptr [eax + 28h]

        mov     ecx, 00406B77h
        jmp     ecx
    }
}

void __stdcall on_set_default_log(RGE_Static_Object* obj)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    if (obj && obj->id == ID_TO_WATCH)
    {
        log(" [T = %01d:%02d:%02d.%03d] => set_default", h, m, s, ms);
    }
}

__declspec(naked) void on_set_default() //004098C0
{
    __asm
    {
        push    esi
        call    on_set_default_log
        mov     edx, [esi]
        mov     ecx, esi
        call    dword ptr [edx + 390h]

        mov     ecx, 004098C6h
        jmp     ecx
    }
}


void __stdcall on_action_object_update_log(RGE_Static_Object* obj)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    if (obj && obj->id == ID_TO_WATCH)
    {
        log(" [T = %01d:%02d:%02d.%03d] => action_object_update, state = %d", h, m, s, ms, (int)obj->object_state);

        wt = get_worldtime();
        //if (wt >= 200728 /*201170*/)
            //__debugbreak();
            //pause_game2();
    }
}

__declspec(naked) void on_action_object_update() //00409874
{
    __asm
    {
        mov     esi, ecx
        push    esi
        call    on_action_object_update_log
        
        mov     ecx, esi
        mov     eax, 004A1A00h
        call    eax
        mov     ecx, 00409879h
        jmp     ecx
    }
}

bool __stdcall building_get_change_terrain_unconditional(TRIBE_Building_Object* obj)
{
    return true;
    if (obj->master_obj->id == 50)  //farm
    {
        return true;
        switch (obj->owner->world->map->map_row_offset[(int)obj->world_y][(int)obj->world_x].terrain_type)
        {
        case 4:
        case 59:
        case 142:
        case 235:
            return true;
        default:
            return false;
        }
    }
    else
        return false;
}

__declspec(naked) void on_building_change_terrain() //00557C31
{
    __asm
    {
        push    edi
        call    building_get_change_terrain_unconditional

        mov     ecx, [edi + 18h]
        test    al, al
        jnz     building_change_terrain_unconditional

        mov     bl, [esp + 10h]
        mov     eax, 00557C38h
        jmp     eax

building_change_terrain_unconditional:
        mov     eax, 00557CC9h
        jmp     eax
    }
}

void __stdcall state_log(RGE_Action_Gather* action, int state)
{
    chat("State: %d", state);
    //action->farm_x = -1;
    //action->farm_y = -1;
    //if (state == 243)
       // __debugbreak();
}

__declspec(naked) void on_state() //00404490
{
    __asm
    {
        mov    eax, [esp + 4]
        push   ecx
        push   eax
        push   ecx
        call   state_log
        pop    ecx
        _emit  0x64
        _emit  0xA1
        _emit  0x00
        _emit  0x00
        _emit  0x00
        _emit  0x00
        mov    edx, 00404496h
        jmp    edx
    }
}

void __stdcall log_player_update(RGE_Player* player)
{
    log("WT=%d, P%d, before update: rand=%d", player->world->world_time, player->id, rand_no_change());
    player->vfptr->update(player);
    log("WT=%d, P%d, after update: rand=%d", player->world->world_time, player->id, rand_no_change());
}

__declspec(naked) void log_on_player_update() //0061F85E
{
    __asm
    {
        push    ecx
        call    log_player_update
        mov     ecx, 0061F864h
        jmp     ecx
    }
}

void __stdcall log_object_list_update(RGE_Player_Object_List* ol)
{
    log("  OL: before update: rand=%d", rand_no_change());
    RGE_Player_Object_List__Update(ol);
    log("  OL: after update: rand=%d", rand_no_change());
}

__declspec(naked) void log_on_object_list_update() //004C1985
{
    __asm
    {
        push    ecx
        call    log_object_list_update
        mov     ecx, 004C198Ah
        jmp     ecx
    }
}

unsigned __int8 __stdcall log_object_update(RGE_Static_Object* obj)
{
    log("    obj=%d: before update: rand=%d", obj->id, rand_no_change());

    //if (obj->owner->world->world_time == 21990 && obj->id == 10730)
    //    __debugbreak();

    unsigned __int8 result = obj->vfptr->update(obj);
    log("    obj=%d: after update: rand=%d", obj->id, rand_no_change());
    return result;
}

__declspec(naked) void log_on_object_update() //004AF39B
{
    __asm
    {
        push    ecx
        call    log_object_update
        mov     ecx, 004AF3A1h
        jmp     ecx
    }
}

void __fastcall TRIBE_Game__calc_timing_text_new(TRIBE_Game* game)
{
    unsigned int wu; // ecx
    unsigned int v3; // ecx

    unsigned __int8* initialized_last_time = (unsigned __int8*)0x0078F6A4;
    unsigned int* last_time = (unsigned int*)0x0078F824;
    int* show_timing_max = (int*)0x0078F974;

    if ((*initialized_last_time & 1) == 0)
    {
        *initialized_last_time |= 1u;
        *last_time = timeGetTime();
    }
    *last_time = timeGetTime();
    wu = game->world_update_count;
    if (!wu)
        wu = 1;
    if (*show_timing_max)
        sprintf(
            game->timing_text2,
            "t%lu,f%lu,max,r(v%lu,m%lu,o%lu),s%lu,u%lu,c%lu,lg%lu,sp%lu,sm%lu,m%lu,p%lu,ds%lu,ol%lu",
            game->world_update_fps,
            game->view_update_fps,
            game->timings[6].last_max_time,     //render view
            game->timings[7].last_max_time,     //render map
            game->timings[21].last_max_time,    //render other
            game->timings[3].last_max_time,     //s \ paint
            game->timings[1].last_max_time,     //u \ world update
            game->timings[14].last_max_time,    //c \ comm cycle
            game->timings[12].last_max_time,    //lg \ draw
            game->timings[10].last_max_time,    //sp \ sound play
            game->timings[16].last_max_time,    //sm \ sound manager
            game->timings[13].last_max_time,    //m \ mouse
            game->timings[17].last_max_time,    //p \ pick
            game->timings[2].last_max_time,     //ds \ draw smth
            game->timings[29].last_max_time);   //ol \ object list
    else
        sprintf(
            game->timing_text2,
            "t%lu,fps:%lu,avg,r(v%lu,m%lu,o%lu),s%lu,u%lu,c%lu,lg%lu,sp%lu,ss%lu,sm%lu,m%lu,o%lu,p%lu",
            game->world_update_fps,
            game->view_update_fps,
            game->timings[6].last_time / wu,    //render view
            game->timings[7].last_time / wu,    //render map
            game->timings[21].last_time / wu,   //render other
            game->timings[3].last_time / wu,    //s \ paint
            game->timings[1].last_time / wu,    //u \ world update
            game->timings[14].last_time / wu,   //c \ comm cycle
            game->timings[12].last_time / wu,   //lg \ draw
            game->timings[10].last_time / wu,   //sp \ sound play
            game->timings[15].last_time / wu,   //ss \ nothing
            game->timings[16].last_time / wu,   //sm \ sound manager
            game->timings[13].last_time / wu,   //m \ mouse
            game->timings[4].last_time / wu,    //o \ nothing
            game->timings[17].last_time / wu);  //p \ pick
    v3 = game->view_update_count;
    if (!v3)
        v3 = 1;
    sprintf(game->timing_text, "avg view time=%lu, count=%lu", game->timings[6].last_time / v3, game->view_update_count);
}

void __stdcall do_update_players_start()
{

}

__declspec(naked) void update_players_start() //0061F741
{
    __asm
    {
        lea     edi, [ebp - 80h]
        rep stosd
        call    do_update_players_start

    }
}

/*
__declspec(naked) void test_rType() //00586F35
{
    __asm
    {
        cmp     edx, 4
        jb      good_rtype
        int     3

good_rtype:
        lea     eax, [eax + eax * 2]
        mov     ecx, [ecx + edx * 4 + 1F24h]
        mov     ebx, 00586F3Fh
        jmp     ebx
    }
}

void __stdcall TPanel__set_positioning_new(
    void* ret_addr,
    TPanel* this_,
    int position_mode_in,
    int left_border_in,
    int top_border_in,
    int right_border_in,
    int bottom_border_in,
    int min_wid_in,
    int max_wid_in,
    int min_hgt_in,
    int max_hgt_in,
    TPanel* left_panel_in,
    TPanel* top_panel_in,
    TPanel* right_panel_in,
    TPanel* bottom_panel_in)
{
    if (this_->parent_panel && this_->parent_panel->vfptr == (TPanelVtbl*)0x00661C50)
    {
        std::string panel_type;
        DWORD vtbl = (DWORD)this_->vfptr;
        switch (vtbl)
        {
        case 0x0065E170:
            panel_type = "TTextPanel";
            break;
        case 0x0065CD6C:
            panel_type = "TButtonPanel";
            break;
        case 0x0065D6B8:
            panel_type = "TListPanel";
            break;
        case 0x0065DB78:
            panel_type = "TScrollBarPanel";
            break;
        case 0x00668FFC:
            panel_type = "TRIBE_Diamond_Map_View";
            break;
        case 0x0065C668:
            panel_type = "TPanel";
            break;
        case 0x006691A4:
            panel_type = "TRIBE_Main_View";
            break;
        default:
            panel_type = std::to_string(vtbl);
            break;
        }

        log("Pos, r=0x%08X: type=%s, (%d,%d,%d,%d) (%d,%d)",
            ret_addr,
            panel_type.c_str(),
            left_border_in,
            top_border_in,
            right_border_in,
            bottom_border_in,
            max_wid_in,
            max_hgt_in);
    }

    TDrawArea* v15; // eax
    TPanelVtbl* v16; // edx

    this_->position_mode = position_mode_in;
    this_->right_border = right_border_in;
    this_->top_border = top_border_in;
    this_->max_wid = max_wid_in;
    this_->left_border = left_border_in;
    this_->min_wid = min_wid_in;
    this_->left_panel = left_panel_in;
    this_->bottom_border = bottom_border_in;
    this_->max_hgt = max_hgt_in;
    this_->bottom_panel = bottom_panel_in;
    v15 = this_->render_area;
    this_->min_hgt = min_hgt_in;
    this_->right_panel = right_panel_in;
    v16 = this_->vfptr;
    this_->top_panel = top_panel_in;
    if (v15)
        v16->handle_size(this_, v15->Width, v15->Height);
    else
        v16->handle_size(this_, 0, 0);
    if (this_->active)
        this_->vfptr->set_redraw(this_, 2);
}

__declspec(naked) void on_set_positioning()
{
    __asm
    {
        mov     eax, [esp]
        mov     [esp], ecx
        push    eax
        push    eax
        jmp     TPanel__set_positioning_new
    }
}
*/

void __fastcall TRIBE_Building_Object__findUnitsUnderGate_new(TRIBE_Building_Object* obj, DWORD dummy, int* units, int* numberUnits, int maxNumberUnits, int* numberAlliedUnits)
{
    UNREFERENCED_PARAMETER(dummy);

    int* const ignoreList_4 = (int* const)0x0077FBB0;

    int collisions;
    ObsRecord* ob_recs[20];

    ignoreList_4[0] = obj->id;
    ignoreList_4[1] = obj->linked_child[0]->id;
    ignoreList_4[2] = obj->linked_child[1]->id;
    (*OBSystem_Land)->vfptr->SetIgnoreList(*OBSystem_Land, 1, ignoreList_4, 3);
    (*OBSystem_Land)->vfptr->SetBufferRadius(
        *OBSystem_Land,
        1,
        0.4f);
    (*OBSystem_Land)->vfptr->CheckForCollisions(
        *OBSystem_Land,
        (obj->linked_child[0]->world_x),
        (obj->linked_child[0]->world_y),
        (obj->linked_child[1]->world_x),
        (obj->linked_child[1]->world_y),
        16,
        &collisions,
        ob_recs,
        20,
        -2,
        0,
        0);
    *numberUnits = 0;
    *numberAlliedUnits = 0;
    if (collisions > 0)
    {
        if (obj->id == 103138)
        {
            char s[0x100];
            s[0] = '\0';
            for (int i = 0; i < collisions; i++)
            {
                sprintf(s + strlen(s), "%d, ", ob_recs[i]->Object->id);
            }

            chat("Found %d units: %s", collisions, s);

        }

        for (int i = 0; i < collisions; i++)
        {
            if (*numberUnits >= maxNumberUnits)
                break;
            RGE_Static_Object* obj = ob_recs[i]->Object;
            if (obj
                && obj->master_obj->object_group != 6
                && obj->master_obj->object_group != 18
                && obj->master_obj->object_group != 8
                && obj->master_obj->object_group != 10
                && obj->master_obj->object_group != 9
                && ((obj->vfptr->maximumSpeed)(obj) >= 0.0000001f || obj->owner->id))
            {
                units[*numberUnits] = obj->id;
                ++*numberUnits;
                if (obj->owner->vfptr->isAlly(obj->owner, obj->owner->id) == 1)
                    ++*numberAlliedUnits;
            }
        }
    }
    (*OBSystem_Land)->vfptr->SetBufferRadius(*OBSystem_Land, 0, 0.0f);
}

void __cdecl setTestHook_atexit()
{
    free(log_int_s);
}

#pragma optimize( "s", on )
void setTestHook()
{
    //setHook((void*)0x00558520, TRIBE_Building_Object__findUnitsUnderGate_new);

    //writeNops(0x00493B3D, 2);
    //Sed UI panel
    //writeDword(0x005368BA, 700);
    //writeDword(0x005368BF, 700);

    //diamond map dimensions
    //writeDword(0x00536893, 800);
    //writeDword(0x0053688E, 800);
    //writeDword(0x00536889, 450);
    //writeDword(0x00536884, 450);

    //diamond map bitmap sizer
    //writeDword(0x005298C6, 50415);

    //setHook((void*)0x004B5D80, on_set_positioning);

    //setHook((void*)0x00586F35, test_rType);

    //writeNops(0x00517611, 6);
    //writeByte(0x00517611, 0xE9);

    //writeByte(0x005EBB65, 0xEB);

    //setHook((void*)0x005E95E0, TRIBE_Game__calc_timing_text_new);
    //setHook((void*)0x004AF380, RGE_Player_Object_List__Update_new);

    //setHook((void*)0x0061F85E, log_on_player_update);
    //setHook((void*)0x004C1985, log_on_object_list_update);
    //setHook((void*)0x004AF39B, log_on_object_update);

    //writeNops(0x00404F59, 2);

    //setHook((void*)0x00404490, on_state);
    //setHook((void*)0x00557C31, on_building_change_terrain);

    /*
    unsigned __int8* color_black = (unsigned __int8*)0x007A1C60;
    //*color_black = 1;
    int* group_debug_display = (int*)0x006ACCA4;
    *group_debug_display = 1;
    */

    /*setHook((void*)0x005650F0, on_update);
    setHook((void*)0x00565182, on_check_ownership);
    setHook((void*)0x00406B60, on_action_list_update);
    setHook((void*)0x004098C0, on_set_default);
    setHook((void*)0x00409874, on_action_object_update);*/

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

    //setHook((void*)0x0042C360, new_game_dev_mode);
    //setHook((void*)0x00428270, new_check_multi_copies);
    //writeDword(0x00659F80, 0x004B6120);
    //setHook((void*)0x0046D9B0, (void*)0x004B6120);
    //setHook((void*)0x005E7EC0, onStatusScreenCreate);
    //setHook((void*)0x004B4960, onBlankScreenCreate);
    //srand(timeGetTime());
    
    //setOOSHooks();
    //savegame path
    //writeDword(0x0048F566, (DWORD)savegame_path);

    //setHook((void*)0x005D0D59, repair_test);

    //RMS token count
    //setHook((void*)0x004E1D40, onRmsToken);

    /* !! conflicts trigger or condition !!
#ifdef _DEBUG
    setHook((void*)0x005F54D2, triggerLogHook);
    setHook((void*)0x005F4A77, effectLogHook);
#endif
    */

    //fix internal log
    log_int_s = (char*)malloc(LOG_INT_BUFFER_SIZE);

    writeDword(0x0040F1A6, (DWORD)nullsub_26_2);
    writeDword(0x0040F842, (DWORD)nullsub_26_2);
    writeDword(0x0040F885, (DWORD)nullsub_26_2);
    writeDword(0x0040F75B, (DWORD)nullsub_26_2);
    writeDword(0x0040EFBB, (DWORD)nullsub_26_2);
    writeDword(0x0057796E, (DWORD)nullsub_26_2);
    writeDword(0x00577A63, (DWORD)nullsub_26_2);
    writeDword(0x00577A89, (DWORD)nullsub_26_2);
    writeDword(0x00577AAF, (DWORD)nullsub_26_2);

    fix_function_call(0x0059B18B, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B1C6, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B1EA, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B4BE, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B524, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B56D, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B5AC, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B5D0, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B63D, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B675, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B73C, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B827, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B870, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B8AF, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B8C7, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B947, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B9D0, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059BACB, (DWORD)nullsub_26_2); //Entering task

    fix_function_call(0x004336D8, (DWORD)nullsub_26_2); //Starting turn
    //

    writeNops(0x004428F6, 5);
    writeNops(0x0061B129, 5);
    writeNops(0x005B944D, 5);
    writeNops(0x005A05DD, 5);
    writeNops(0x0050D0F3, 5);
    writeNops(0x00410D38, 5);
    writeNops(0x0042520B, 5);
    writeNops(0x004776BE, 5);
    writeNops(0x005A0559, 5);
    writeNops(0x005A0581, 5);
    writeNops(0x005A05DD, 5);

    setHook((void*)0x0060F920, log_int_wr);

#ifndef TARGET_VOOBLY
    setHook((void*)0x00438140, onChat);
#endif

    //
#ifdef _DEBUG
    //setHook((void*)0x004D5550, readDatHook);
#endif

    setHook((void*)0x00497192, report_dynamic_tile_cache_size);

    //action list
    //writeNops(0x004098FD, 5);

    efpatch_atexit(setTestHook_atexit);
}
#pragma optimize( "", on )
