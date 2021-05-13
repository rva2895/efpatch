#include "stdafx.h"
#include "statusscreen.h"

const char* aGameScreen = (char*)0x00689ABC;
const char* aCombatComparis = (char*)0x00699ED8;
const char* aBlankScreen = (char*)0x0068976C;
const char* aStatusScreen = (char*)0x006899A8;
const char* del_game_info_str = "Destroying world ...";

__declspec(naked) void onStatusLoadSave() //005EC5F9
{
    __asm
    {
        push    edi
        mov     edi, 004B4D60h //PanelSystem__destroyPanelIfExists
        push    aGameScreen
        mov     ecx, 006ADBB8h
        mov     [ebp + 17B4h], ebx
        call    edi
        push    aCombatComparis
        mov     ecx, 006ADBB8h
        call    edi

        push    ebx
        push    offset aBlankScreen
        mov     ecx, 006ADBB8h
        mov     eax, 004B4960h
        call    eax
        push    offset aStatusScreen
        mov     ecx, 006ADBB8h
        call    edi

        push    ebx
        push    ebx
        push    0FFFFFFFFh
        push    ebx
        push    del_game_info_str
        mov     ecx, ebp
        mov     eax, 005E7EC0h
        call    eax

        pop     edi

        mov     ecx, [ebp + 420h]
        mov     eax, [ecx]
        call    dword ptr [eax + 0D0h]

        mov     eax, 005EC64Ah
        jmp     eax
    }
}

__declspec(naked) void onStatusQuitGame() //005E8EF8
{
    __asm
    {
        push    edi
        push    ecx

        mov     edi, 004B4D60h //PanelSystem__destroyPanelIfExists

        push    ebx
        push    offset aBlankScreen
        mov     ecx, 006ADBB8h
        mov     eax, 004B4960h
        call    eax
        push    offset aStatusScreen
        mov     ecx, 006ADBB8h
        call    edi

        push    ebx
        push    ebx
        push    0FFFFFFFFh
        push    ebx
        push    del_game_info_str
        mov     ecx, ebp
        mov     eax, 005E7EC0h //statusScreen_create
        call    eax

        pop     ecx
        mov     eax, [ecx]
        call    dword ptr [eax + 0D0h]

        push    offset aStatusScreen
        mov     ecx, 006ADBB8h
        call    edi

        push    offset aBlankScreen
        mov     ecx, 006ADBB8h
        call    edi

        pop     edi

        mov     eax, 005E8F00h
        jmp     eax
    }
}

void* screen_this = 0;

const char* starting_0 = "Starting a new game ...\n\nStarting ...\n";
const char* starting = "Starting a new game ...\n\nStarting ...\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

int line_counter = 14;
char status_buffer[0x2000];

void __stdcall setup_full_string()
{
    strcpy(status_buffer, starting_0);
    for (int i = 0; i < line_counter; i++)
        strcpy(status_buffer + strlen(status_buffer), "\n");
}

__declspec(naked) void onGetThis_newGame() //005EBCCA
{
    __asm
    {
        mov     screen_this, ebp
        pop     eax
        push    starting
        mov     eax, 14
        mov     line_counter, eax
        mov     eax, 005E7EC0h
        call    eax
        call    setup_full_string
        mov     eax, 005EBCCFh
        jmp     eax
    }
}

char* __fastcall get_full_string(char* txt)
{
    //strcpy(status_buffer, starting_0);
    strcpy(status_buffer + strlen(status_buffer) - line_counter, txt);
    for (int i = 0; i < line_counter; i++)
        strcpy(status_buffer + strlen(status_buffer), "\n");
    line_counter--;
    return status_buffer;
    //return txt;
}

__declspec(naked) void __fastcall change_status_screen_using_this(const char* txt)
{
    __asm
    {
        mov     eax, screen_this
        test    eax, eax
        jz      no_screen_this
        xor     eax, eax
        push    eax
        push    eax
        push    0FFFFFFFFh
        push    eax
        //push    ecx
        call    get_full_string
        push    eax
        mov     ecx, screen_this
        mov     eax, 005E7EC0h //statusScreen_create
        call    eax
no_screen_this:
        ret
    }
}

const char* creating_world = "Creating World ...";
const char* creating_base_world = "Creating BaseWorld ...";
const char* creating_base_world_new_map = "Creating BaseWorld (new map) ...";
const char* creating_base_world_gaia = "Creating BaseWorld (gaia) ...";
const char* creating_base_world_players = "BaseWorld (players) ...";
const char* creating_base_world_generating_map = "BaseWorld (generating map) ...";

__declspec(naked) void onStatusNewGame_World_newGame() //005ED2AE
{
    __asm
    {
        push    ecx
        push    edx

        mov     ecx, creating_world
        call    change_status_screen_using_this

        pop     edx
        pop     ecx
        call    dword ptr [edx + 0ECh]
        mov     ecx, 005ED2B4h
        jmp     ecx
    }
}

__declspec(naked) void onStatusNewGame_BaseWorld_newGame() //006004F1
{
    __asm
    {
        push    ecx

        mov     ecx, creating_base_world
        call    change_status_screen_using_this

        pop     ecx
        mov     eax, 0061DF00h
        call    eax
        mov     ecx, 006004F6h
        jmp     ecx
    }
}

__declspec(naked) void onStatusNewGame_BaseWorld_newGame_new_map() //0061E09D
{
    __asm
    {
        push    ecx

        mov     ecx, creating_base_world_new_map
        call    change_status_screen_using_this

        pop     ecx
        mov     eax, 004906D0h
        call    eax
        mov     ecx, 0061E0A2h
        jmp     ecx
    }
}

__declspec(naked) void onStatusNewGame_BaseWorld_newGame_gaia() //0061E0E0
{
    __asm
    {
        push    eax

        mov     ecx, creating_base_world_gaia
        call    change_status_screen_using_this

        pop     eax
        mov     ecx, ebp
        call    dword ptr [eax + 64h]
        mov     ecx, 0061E0E5h
        jmp     ecx
    }
}

__declspec(naked) void onStatusNewGame_BaseWorld_newGame_players() //0061E108
{
    __asm
    {
        push    edx

        mov     ecx, creating_base_world_players
        call    change_status_screen_using_this

        pop     edx
        mov     ecx, ebp
        push    esi
        call    dword ptr [edx + 6Ch]
        mov     ecx, 0061E10Eh
        jmp     ecx
    }
}

__declspec(naked) void onStatusNewGame_BaseWorld_newGame_generating_map() //0061E3BF
{
    __asm
    {
        push    edx

        mov     ecx, creating_base_world_generating_map
        call    change_status_screen_using_this

        pop     edx
        mov     ecx, ebp
        call    dword ptr [edx + 70h]
        mov     ecx, 0061E3C4h
        jmp     ecx
    }
}

__declspec(naked) void onRemoveThis_newGame() //005EBCD3
{
    __asm
    {
        mov     eax, 005ECA30h
        call    eax
        xor     ecx, ecx
        mov     screen_this, ecx
        mov     ecx, 005EBCD8h
        jmp     ecx
    }
}

//generate
const char* generating_map_blend = "BaseWorld (generating map) (blend) ...";
const char* generating_map_cliff = "BaseWorld (generating map) (cliff) ...";
const char* generating_map_connection = "BaseWorld (generating map) (connection) ...";
const char* generating_map_elevation = "BaseWorld (generating map) (elevation) ...";
const char* generating_map_land = "BaseWorld (generating map) (land) ...";
const char* generating_map_shore = "BaseWorld (generating map) (shore) ...";

__declspec(naked) void onStatus_generate_blend() //004DDD40
{
    __asm
    {
        push    esi
        mov     esi, ecx

        mov     ecx, generating_map_blend
        call    change_status_screen_using_this

        push    edi
        mov     al, [esi + 1DDFh]
        mov     ecx, 004DDD4Ah
        jmp     ecx
    }
}

__declspec(naked) void onStatus_generate_cliff() //004D76B0
{
    __asm
    {
        push    esi
        push    edi
        mov     esi, ecx
        mov     eax, 004D7700h
        call    eax

        mov     ecx, generating_map_cliff
        call    change_status_screen_using_this

        mov     ecx, 004D76B9h
        jmp     ecx
    }
}

__declspec(naked) void onStatus_generate_connection() //004E42B0
{
    __asm
    {
        sub     esp, 8
        push    ebp
        mov     ebp, ecx
        mov     eax, 004D6B40h
        call    eax

        mov     ecx, generating_map_connection
        call    change_status_screen_using_this

        mov     ecx, 004E42BBh
        jmp     ecx
    }
}

__declspec(naked) void onStatus_generate_elevation() //004D9950
{
    __asm
    {
        push    ecx
        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     ebp, ecx
        mov     eax, 004D6B40h
        call    eax

        mov     ecx, generating_map_elevation
        call    change_status_screen_using_this

        mov     ecx, 004D995Ch
        jmp     ecx
    }
}

__declspec(naked) void onStatus_generate_land() //004DA380
{
    __asm
    {
        push    esi
        push    edi
        mov     esi, ecx
        mov     eax, 004D6B40h
        call    eax

        mov     ecx, generating_map_land
        call    change_status_screen_using_this

        mov     ecx, 004DA389h
        jmp     ecx
    }
}

__declspec(naked) void onStatus_generate_shore() //004E5F76
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     ebx, ecx
        mov     eax, 004E69E0h
        call    eax

        mov     ecx, generating_map_shore
        call    change_status_screen_using_this

        mov     ecx, 004E5F81h
        jmp     ecx
    }
}

void setStatusScreenHooks()
{
    setHook((void*)0x005EC5F9, onStatusLoadSave);
    setHook((void*)0x005E8EF8, onStatusQuitGame);

    setHook((void*)0x005EBCCA, onGetThis_newGame);
    setHook((void*)0x005EBCD3, onRemoveThis_newGame);
    setHook((void*)0x005ED2AE, onStatusNewGame_World_newGame);
    setHook((void*)0x006004F1, onStatusNewGame_BaseWorld_newGame);
    setHook((void*)0x0061E09D, onStatusNewGame_BaseWorld_newGame_new_map);
    setHook((void*)0x0061E0E0, onStatusNewGame_BaseWorld_newGame_gaia);
    setHook((void*)0x0061E108, onStatusNewGame_BaseWorld_newGame_players);
    setHook((void*)0x0061E3BF, onStatusNewGame_BaseWorld_newGame_generating_map);

    setHook((void*)0x004DDD40, onStatus_generate_blend);
    setHook((void*)0x004D76B0, onStatus_generate_cliff);
    setHook((void*)0x004E42B0, onStatus_generate_connection);
    setHook((void*)0x004D9950, onStatus_generate_elevation);
    setHook((void*)0x004DA380, onStatus_generate_land);
    setHook((void*)0x004E5F76, onStatus_generate_shore);
}

//005ED2AE: world::new_game