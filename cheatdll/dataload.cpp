#include "stdafx.h"
#include "dataload.h"

#include "convert.h"
#include "resgenbldgs.h"
#include "expldroid.h"
#include "jedimaster.h"

extern int current_save_game_version;
char* data_file_name;
char* data_prefix;
int current_loaded_version = 1;

int* current_handle = (int*)0x0077EF8C;
int* compression_buffers = (int*)0x0077EF98;
int* file_buffers = (int*)0x0077F0EC;
int* current = (int*)0x0077EFA4;
int* buffers = (int*)0x0077EF9C;
int* point = (int*)0x0077EF94;
int* flags = (int*)0x0077EF90;
int* file_size = (int*)0x0077F0E8;
int* rge_fio_bl = (int*)0x0077EFA0;
int* rge_unknown = (int*)0x0077ED88;

int save_game__handle;
int save_game__compression_buffers;
int save_game__file_buffers;
int save_game__current;
int save_game__buffers;
int save_game__point;
int save_game__flags;
int save_game__file_size;
int save_game__rge_fio_bl;
int save_game__rge_unknown;

int (__thiscall* Game__load_game_data)(void* this_) =
    (int (__thiscall*)(void*))0x005E92A0;

void setup_data_file_name(const char* data_file)
{
    strcpy(data_file_name, data_file);
}

__declspec(naked) void onDataFileName() //0048F0FB
{
    __asm
    {
        mov     [esp + 1Ch], esi
        mov     data_file_name, esi
        mov     esi, edi
        mov     edi, 0048F101h
        jmp     edi
    }
}

__declspec(naked) void __stdcall unload_game_data() //
{
    __asm
    {
        push    esi
        mov     esi, BaseGame_bg
        mov     esi, [esi]
        mov     ecx, [esi + 420h]
        test    ecx, ecx
        jz      no_data_unload
        mov     edx, [ecx]
        push    1
        call    dword ptr [edx + 0B8h]
no_data_unload:
        mov     dword ptr [esi + 420h], 0
        pop     esi
        retn
    }
}

char* ground_to_air_path;
char* jedi_holo_path;

void do_setup_dat_file()
{
    //return;
    //(*(void**)((uint8_t*)*BaseGame_bg + 0x420)) = NULL;
    Game__show_status_message(*BaseGame_bg, "Loading game information ...", NULL, 0, 0, 1);

    save_game__handle = *current_handle;
    save_game__compression_buffers = *compression_buffers;
    save_game__file_buffers = *file_buffers;
    save_game__current = *current;
    save_game__buffers = *buffers;
    save_game__point = *point;
    save_game__flags = *flags;
    save_game__file_size = *file_size;
    save_game__rge_fio_bl = *rge_fio_bl;
    save_game__rge_unknown = *rge_unknown;

    unload_game_data();
    Game__load_game_data(*BaseGame_bg);

    *current_handle = save_game__handle;
    *compression_buffers = save_game__compression_buffers;
    *file_buffers = save_game__file_buffers;
    *current = save_game__current;
    *buffers = save_game__buffers;
    *point = save_game__point;
    *flags = save_game__flags;
    *file_size = save_game__file_size;
    *rge_fio_bl = save_game__rge_fio_bl;
    *rge_unknown = save_game__rge_unknown;

    //clear legacy jedi-holo and ground-to-air
    memset((void*)0x007B1000, 0, 0x1000);

    sprintf(ground_to_air_path, "%sground-to-air.txt", data_prefix);
    sprintf(jedi_holo_path, "%sjedi-holo.txt", data_prefix);

    writeDword(0x007B2046, (DWORD)jedi_holo_path);
    writeDword(0x007B21B6, (DWORD)ground_to_air_path);

    initExplDroid(data_prefix, "expl.txt");
    setJediMasterHooks(data_prefix, "master.txt", "padawan.txt");
    initBldgResProdList(data_prefix, "resgen.txt");
    setConvertHooks(data_prefix, "unconv.txt");

    Game__show_status_message(*BaseGame_bg, "Loading game ...", NULL, 0, 0, 0);
    //Game__close_status_message(*BaseGame_bg);
}

void __stdcall setup_dat_file_on_del_game_info()
{
    if (current_loaded_version != 1)
    {
        //MessageBox(0, "del_game_info, reload", "test", 0);
        setup_data_file_name("data\\genie_x2.dat");
        current_loaded_version = 1;
        data_prefix = "data\\";
        do_setup_dat_file();
    }
    //else
    //    MessageBox(0, "del_game_info, no reload", "test", 0);
}

void __stdcall setup_dat_file_on_new_game()
{
    if (current_loaded_version != 1)
    {
        //MessageBox(0, "new_game, reload", "test", 0);
        setup_data_file_name("data\\genie_x2.dat");
        current_loaded_version = 1;
        data_prefix = "data\\";
        do_setup_dat_file();
    }
    //else
    //    MessageBox(0, "new_game, no reload", "test", 0);
}

__declspec(naked) void on_del_game_info() //0061D020
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, ecx
        xor     ebx, ebx
        call    setup_dat_file_on_del_game_info
        mov     ecx, 0061D026h
        jmp     ecx
    }
}

bool load_second_attempt = false;

bool setup_dat_file()
{
    switch (current_save_game_version)
    {
    case 0:
        setup_data_file_name("data\\old\\1.4.0\\genie_x2.dat");
        data_prefix = "data\\old\\1.4.0\\";
        break;
    case 1:
        setup_data_file_name("data\\genie_x2.dat");
        data_prefix = "data\\";
        break;
    default:
        setup_data_file_name("data\\genie_x2.dat");
        data_prefix = "data\\";
        break;
    }
    if (current_loaded_version != current_save_game_version)
    {
        //MessageBox(0, "setup_dat_file, reload", "test", 0);
        current_loaded_version = current_save_game_version;
        do_setup_dat_file();
        //current_loaded_version = current_save_game_version;
        //return false;
        return !load_second_attempt;
    }
    else
    {
        //MessageBox(0, "setup_dat_file, no reload", "test", 0);
        return false;
    }
}

int load_arg_1;
int load_arg_2;

__declspec(naked) void on_call_load_new_game() //005EC79E
{
    __asm
    {
        mov     eax, [esp]
        mov     load_arg_1, eax
        mov     eax, [esp + 4]
        mov     load_arg_2, eax
        xor     eax, eax
        mov     load_second_attempt, al
        call    dword ptr [edx + 0E8h]
        and     eax, 0FFh
        test    eax, eax
        jnz     continue_load
        inc     eax
        mov     load_second_attempt, al
        mov     eax, load_arg_2
        push    eax
        mov     eax, load_arg_1
        push    eax
        mov     ecx, [ebp + 420h]
        mov     edx, [ecx]
        
        call    dword ptr [edx + 0E8h]
        xor     ecx, ecx
        mov     load_second_attempt, cl

continue_load:
        mov     ecx, 005EC7A4h
        jmp     ecx
    }
}

__declspec(naked) void on_new_game() //005ED2AE
{
    __asm
    {
        push    ecx
        push    edx
        call    setup_dat_file_on_new_game
        pop     edx
        pop     ecx
        call    dword ptr [edx + 0ECh]
        mov     ebx, 005ED2B4h
        jmp     ebx
    }
}

__declspec(naked) void on_start_game() //005EBAD4
{
    __asm
    {
        sub     esp, 200h
        push    eax
        push    ecx
        call    setup_dat_file_on_new_game
        pop     ecx
        pop     eax
        mov     edx, 005EBADAh
        jmp     edx
    }
}

void setDataLoadHooks()
{
    setHook((void*)0x0048F0FB, onDataFileName);
    //setHook((void*)0x0061D020, on_del_game_info);
    setHook((void*)0x005EC79E, on_call_load_new_game);
    setHook((void*)0x005EBAD4, on_start_game);

    ground_to_air_path = (char*)malloc(0x100);
    jedi_holo_path = (char*)malloc(0x100);
}
