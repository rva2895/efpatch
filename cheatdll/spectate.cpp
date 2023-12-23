#include "stdafx.h"
#include "spectate.h"

#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <share.h>
#include <ws2tcpip.h>
#include <process.h>

#pragma comment (lib, "Ws2_32.lib")

unsigned int up_read_next_time = 0;

int __stdcall up_read_do(TCommCommandLog* comm_log, int handle, void* buffer, unsigned int size)
{
    int result; // eax

    TMessagePanel* message_panel; // [esp+0h] [ebp-20h]
    struct tagMSG Msg; // [esp+4h] [ebp-1Ch] BYREF

    while (true)
    {
        result = read_internal(handle, buffer, size);
        if (result == size)
        {
            up_read_next_time = 0;
            return result;
        }
        lseek_internal(comm_log->mFileHandle, -result, 1);
        int tmp_handle = _sopen(comm_log->mFileName, 32777, 32, 0); //_sopen("file", _O_BINARY | _O_APPEND | _O_WRONLY, _SH_DENYWR, 0);
        if (tmp_handle >= 0)
        {
            _close(tmp_handle);
            return 0;
        }
        while (PeekMessageA(&Msg, 0, 0, 0, 1u))
        {
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }
        TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
        message_panel = NULL;
        if (game_screen)
        {
            TPanel* main_view = (TPanel*)game_screen->main_view;
            if (main_view->have_focus)
            {
                message_panel = game_screen->message_panel;
                TPanel__set_redraw(main_view, 1);
            }
        }
        if (up_read_next_time)
        {
            DWORD current_time = timeGetTime();
            if (message_panel)
            {
                if (current_time > up_read_next_time && !message_panel->active)
                {
                    std::string message = get_string(1108);
                    TMessagePanel__show_message(message_panel, 1, message.c_str(), 0xF3u, 0, 0, 0, 0, -1, 0, -1);
                }
            }
        }
        else
        {
            up_read_next_time = (timeGetTime() + 5000) | 1;
        }
    }
    return 0;
}

__declspec(naked) void up_read()
{
    __asm
    {
        mov     eax, [esp + 4]
        mov     ecx, [esp + 8]
        mov     edx, [esp + 0Ch]
        push    edx
        push    ecx
        push    eax
        push    esi
        call    up_read_do
        ret
    }
}

void set_up_read_call(DWORD addr)
{
    writeDword(addr + 1, (DWORD)&up_read - (addr + 5));
}

bool send_data(SOCKET s, char* data, int len)
{
    int total = 0;
    int bytesleft = len;

    while (total < len)
    {
        int iSendResult = send(s, data + total, bytesleft, NULL);

        if (iSendResult == SOCKET_ERROR)
        {
            return false;
        }
        total += iSendResult;
        bytesleft -= iSendResult;
    }
    return true;
}

TCommCommandLog* get_command_log()
{
    if ((*base_game)->comm_handler && (*base_game)->comm_handler->mCommandLog)
        return (*base_game)->comm_handler->mCommandLog;
    else
        return NULL;
}

unsigned __stdcall client_session(void* data)
{
    SOCKET client_socket = (SOCKET)data;
    // Process the client.

    TCommCommandLog* log = NULL;

    while (!log)
    {
        Sleep(2000);
        log = get_command_log();
    }

    while (!log->mLogging && !log->mReplaying)
    {
        Sleep(2000);
    }

    char header[256];
    memset(header, 0, sizeof(header));
    strcpy(header, "swgbcc");
    strcpy(header + 32, "mg1");
    strcpy(header + 64, "player_name");

    if (!send_data(client_socket, header, sizeof(header)))
    {
        closesocket(client_socket);
        return 0;
    }

    int handle = _sopen(log->mFileName, _O_BINARY | _O_RDONLY, _SH_DENYNO, 0);
    if (handle == -1)
    {
        closesocket(client_socket);
        return 0;
    }

    char buf[4096];
    int bytes_read;
    bool sending_data = true;
    while (sending_data)
    {
        bytes_read = _read(handle, buf, 4096);
        if (!send_data(client_socket, buf, bytes_read))
        {
            _close(handle);
            closesocket(client_socket);
            return 0;
        }
        if (bytes_read < 4096)
        {
            if ((bytes_read == 0) && !log->mLogging && !log->mReplaying)
                sending_data = false;
            Sleep(200);
        }
    }

    _close(handle);
    closesocket(client_socket);
    return 0;
}

unsigned __stdcall spec_server(void* data)
{
    SOCKET server;
    WSADATA wsaData;
    addrinfo hints;
    addrinfo* result = NULL;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, "53754", &hints, &result))
    {
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET client_socket;
    while ((client_socket = accept(ListenSocket, NULL, NULL)))
    {
        unsigned threadID;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &client_session, (void*)client_socket, 0, &threadID);
    }

    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}

#pragma optimize( "s", on )
void setSpectateHooks()
{
    set_up_read_call(0x0042EC09);
    set_up_read_call(0x0042EC28);
    set_up_read_call(0x0042EC59);
    set_up_read_call(0x0042EC73);
    set_up_read_call(0x0042EC90);
    set_up_read_call(0x0042ECAB);
    set_up_read_call(0x0042ED0F);
    set_up_read_call(0x0042ED5C);
    set_up_read_call(0x0042ED79);
    set_up_read_call(0x0042EDF2);
    set_up_read_call(0x0042EEE7);
    set_up_read_call(0x0042F411);
    set_up_read_call(0x0042F469);
    set_up_read_call(0x0042F4A2);
    set_up_read_call(0x0042F4CA);
    set_up_read_call(0x0042F4FF);
    set_up_read_call(0x0042F690);
    set_up_read_call(0x0042F6DD);
    set_up_read_call(0x0042F715);
    set_up_read_call(0x0042F736);
    set_up_read_call(0x0042F74D);
    set_up_read_call(0x0042F9A4);
    set_up_read_call(0x0042F9EF);
    set_up_read_call(0x0042FA29);
    set_up_read_call(0x0042FA44);
    set_up_read_call(0x0042FA5F);
    set_up_read_call(0x0042FA7A);
    set_up_read_call(0x0042FA95);
    set_up_read_call(0x0042FAB0);
    set_up_read_call(0x0042FC80);
    set_up_read_call(0x0042FCCD);
    set_up_read_call(0x0042FD01);
    set_up_read_call(0x0042FD18);
    set_up_read_call(0x0042FD2F);
    set_up_read_call(0x0042FEB9);
    set_up_read_call(0x0042FF06);
    set_up_read_call(0x0042FF3A);
    set_up_read_call(0x0042FF51);
    set_up_read_call(0x0042FF6B);
    set_up_read_call(0x00430329);
    set_up_read_call(0x004303A1);

    unsigned threadID;
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &spec_server, NULL, 0, &threadID);
}
#pragma optimize( "", on )
