#include "stdafx.h"
#include "spectate.h"

#include "registry.h"
#include "mpsetupscreen.h"
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <share.h>
#include <ws2tcpip.h>
#include <process.h>

extern CONFIG_DATA cd;
extern EXTRA_GAME_OPTIONS extra_options;

unsigned int up_read_next_time = 0;

int __stdcall up_read_do(TCommCommandLog* comm_log, int handle, void* buffer, unsigned int size)
{
    while (true)
    {
        int result = read_internal(handle, buffer, size);
        if (result == size)
        {
            up_read_next_time = 0;
            return result;
        }
        lseek_internal(comm_log->mFileHandle, -result, SEEK_CUR);
        int tmp_handle = _sopen(comm_log->mFileName, _O_BINARY | _O_APPEND | _O_WRONLY, _SH_DENYNO);
        if (tmp_handle >= 0)
        {
            _close(tmp_handle);
            return 0;
        }
        MSG Msg;
        while (PeekMessageA(&Msg, NULL, 0, 0, 1))
        {
            if (Msg.message == WM_QUIT)
            {
                PostMessageA(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                return 0;
            }
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }
        TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
        TMessagePanel* message_panel = NULL;
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
            if (message_panel && timeGetTime() > up_read_next_time && !message_panel->active)
            {
                //std::string message = get_string(1108);
                TMessagePanel__show_message(message_panel, 1, "Waiting for stream ...", cd.gameVersion ? 68 : 243, 0, NULL, 0, 0, 5000, NULL, -1);
                /*TPanel* main_view = (TPanel*)game_screen->main_view;
                if (main_view->have_focus)
                {
                    message_panel = game_screen->message_panel;
                    TPanel__set_redraw(main_view, 1);
                }
                */
                //message_panel->vfptr->set_active(message_panel, 1);
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

/*
TCommCommandLog* get_command_log()
{
    if ((*base_game) && (*base_game)->comm_handler && (*base_game)->comm_handler->mCommandLog)
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
    strcpy(header + 32, cd.gameVersion == VER_EF ? "mg2" : "mg1");
    strcpy(header + 64, TCommunications_Handler__GetPlayerName((*base_game)->comm_handler, RGE_Base_Game__getMyCommPlayer(*base_game)));

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
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, client_session, (void*)client_socket, 0, &threadID);
        if (hThread)
            CloseHandle(hThread);
    }

    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
*/

#define MSG_BUFFER_SIZE 0x1000

template<class T>
void stream_write(const T& t, char*& p)
{
    memcpy(p, &t, sizeof(T));
    p += sizeof(T);
}

struct spec_client_param
{
    std::string addr;
    std::string port;
};

volatile bool spec_client_set_active = false;
volatile bool spec_client_set_complete = true;

class spec_client_locals
{
public:
    bool WSA_started = false;
    int handle = -1;
    addrinfo* result = NULL;
    SOCKET ConnectSocket = INVALID_SOCKET;
    ~spec_client_locals()
    {
        if (handle != -1)
            _close(handle);

        if (result)
            freeaddrinfo(result);

        if (ConnectSocket != INVALID_SOCKET)
            closesocket(ConnectSocket);

        if (WSA_started)
            WSACleanup();

        spec_client_set_complete = true;
    };
};

unsigned int __stdcall spec_client(void* data)
{
    spec_client_param* param = (spec_client_param*)data;
    std::string addr = param->addr;
    std::string port = param->port;
    delete param;

    spec_client_locals locals;

    auto log_spec_client = [](const char* format, ...)
        {
            va_list ap;
            va_start(ap, format);
            std::string s = "Spectator client: " + std::string(format);
            vlog(s.c_str(), ap);
            va_end(ap);
        };

    if (!(*comm)->mCommandLog->mFileName[0])
    {
        log_spec_client("not logging");
        return 0;
    }

    locals.handle = _sopen((*comm)->mCommandLog->mFileName, _O_BINARY | _O_RDONLY, _SH_DENYNO);
    if (locals.handle == -1)
    {
        log_spec_client("cannot open stream");
        return 0;
    }
    
    WSADATA wsaData;
    addrinfo hints;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        log_spec_client("socket error");
        return 0;
    }

    locals.WSA_started = true;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(addr.c_str(), port.c_str(), &hints, &locals.result))
    {
        log_spec_client("socket error");
        return 0;
    }

    locals.ConnectSocket = socket(locals.result->ai_family, locals.result->ai_socktype, locals.result->ai_protocol);
    if (locals.ConnectSocket == INVALID_SOCKET)
    {
        log_spec_client("socket error");
        return 0;
    }

    if (connect(locals.ConnectSocket, locals.result->ai_addr, (int)locals.result->ai_addrlen) == SOCKET_ERROR)
    {
        log_spec_client("cannot connect to server (%s:%s)", addr.c_str(), port.c_str());
        return 0;
    }
    else
    {
        log_spec_client("connected to (%s:%s)", addr.c_str(), port.c_str());
    }

    freeaddrinfo(locals.result);
    locals.result = NULL;

    char msg[MSG_BUFFER_SIZE];
    char* p = msg;

    auto send_proto_header = [&]()
        {
            proto_header hdr;
            strncpy(hdr.name, "gbgspc\0", 8);
            hdr.proto_version = 1;
            hdr.reserved = 0;
            return send_data(locals.ConnectSocket, (char*)&hdr, sizeof(hdr));
        };

    auto send_msg = [&](enum MESSAGE_TYPE t, char* data, int len)
        {
            message_header msg_hdr;
            msg_hdr.message_type = t;
            msg_hdr.message_size = len;
            return send_data(locals.ConnectSocket, (char*)&msg_hdr, sizeof(msg_hdr))
                && send_data(locals.ConnectSocket, data, len);
        };

    auto send_client_type = [&](enum CLIENT_TYPE client_type)
        {
            p = msg;
            stream_write<uint32_t>(client_type, p);
            return send_msg(MESSAGE_TYPE_SET_CLIENT_TYPE, msg, sizeof(uint32_t));
        };

    auto send_game_info = [&]()
        {
            p = msg;
            stream_write<uint32_t>(cd.gameVersion, p);
            stream_write<uint32_t>(GAME_STATUS_PENDING, p);
            stream_write<uint64_t>(0, p);   //timestamp
            stream_write<uint32_t>(0, p);   //duration

            std::vector<std::string> players;
            for (int i = 1; i <= 8; i++)
            {
                char* name = TCommunications_Handler__GetPlayerName(*comm, i);
                if (name && name[0] && strcmp(name, " "))
                    players.emplace_back(name);
            }

            stream_write<uint32_t>(players.size(), p);
            for (auto it = players.begin(); it != players.end(); ++it)
            {
                uint32_t len = it->length() + 1;
                stream_write<uint32_t>(len, p);
                strlcpy(p, it->c_str(), len);
                p += len;
            }
            return send_msg(MESSAGE_TYPE_GAME_INFO, msg, p - msg);
        };

    auto send_rec_stream = [&](int len)
        {
            return send_msg(MESSAGE_TYPE_REC_STREAM, msg, len);
        };

    auto send_game_status = [&](enum GAME_STATUS status, uint32_t duration)
        {
            p = msg;
            stream_write<uint32_t>(status, p);
            stream_write<uint32_t>(duration, p);
            return send_msg(MESSAGE_TYPE_GAME_STATUS, msg, 2 * sizeof(uint32_t));
        };

    auto send_multiple_rec_stream = [&]()
        {
            int bytes_read;
            do
            {
                bytes_read = _read(locals.handle, msg, MSG_BUFFER_SIZE);
                if (bytes_read > 0 && !send_rec_stream(bytes_read))
                    return false;
            } while (bytes_read >= MSG_BUFFER_SIZE);
            return true;
        };

    if (!send_proto_header()
        || !send_client_type(CLIENT_TYPE_GAME)
        || !send_game_info()
        || !send_game_status(GAME_STATUS_PENDING, 0))
    {
        log_spec_client("lost connection to server");
        return 0;
    }

    unsigned int last_status_time = timeGetTime();
    unsigned int world_time = 0;
    unsigned int old_world_time = 0;

    auto get_world_time = [&world_time, &old_world_time]()
        {
            if (*base_game && (*base_game)->world)
                world_time = (*base_game)->world->world_time;
            if (world_time > old_world_time)
                old_world_time = world_time;
            return world_time < old_world_time ? old_world_time : world_time;
        };

    //send first
    log_spec_client("sending initial stream data");
    if (!send_multiple_rec_stream())
    {
        log_spec_client("lost connection to server");
        return 0;
    }
    log_spec_client("initial stream data sent");

    while (spec_client_set_active)
    {
        if (!send_multiple_rec_stream())
        {
            log_spec_client("lost connection to server");
            return 0;
        }
        unsigned int current_time = timeGetTime();
        if (current_time - last_status_time > 5000)
        {
            if (!send_game_status(GAME_STATUS_IN_PROGRESS, get_world_time()))
            {
                log_spec_client("lost connection to server");
                return 0;
            }

            last_status_time = current_time;
        }
        Sleep(200);
    }

    //send remaining
    log_spec_client("received stop request, sending remaining data");
    if (!send_multiple_rec_stream())
    {
        log_spec_client("lost connection to server");
        return 0;
    }

    //send game ended
    if (!send_game_status(GAME_STATUS_ENDED, get_world_time()))
        log_spec_client("lost connection to server");
    else
        log_spec_client("streaming complete");

    return 0;
}

void __stdcall stop_spec_client()
{
    log("Logging stopped, waiting for spectator client...");
    spec_client_set_active = false;
    while (!spec_client_set_complete)
        Sleep(10);

    log("Spectator client stopped");
}

void __stdcall stop_spec_client_no_wait_and_stop_logging()
{
    TCommCommandLog* comm_log = (*comm)->mCommandLog;
    if (spec_client_set_active && comm_log && comm_log->mLogging > 0)
    {
        TCommCommandLog__stopLogging(comm_log);
        log("Game ended, requested spectator client stop");
        spec_client_set_active = false;
    }
}

int __fastcall TCommCommandLog__startLogging_spec_new(TCommCommandLog* comm_log, DWORD dummy, char* name)
{
    UNREFERENCED_PARAMETER(dummy);

    stop_spec_client();
    int r = TCommCommandLog__startLogging(comm_log, name);
    if (r && extra_options.allow_spectators && TCommunications_Handler__IsHost(*comm))
    {
        spec_client_param* param = new spec_client_param;
        param->addr = "gbgrecs.servegame.com";
        //param->addr = "192.168.1.4";
        param->port = "53650";

        spec_client_set_active = true;
        spec_client_set_complete = false;
        log("Logging, started spectator client");
        unsigned int threadID;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, spec_client, param, 0, &threadID);
        if (hThread)
            CloseHandle(hThread);
    }
    else
    {
        spec_client_set_complete = true;
    }
    return r;
}

void __fastcall TCommCommandLog__stopLogging_spec_new(TCommCommandLog* comm_log)
{
    TCommCommandLog__stopLogging(comm_log);
    stop_spec_client();
}

__declspec(naked) void on_stop_logging_init() //0042E916
{
    __asm
    {
        mov     ecx, esi
        call    TCommCommandLog__stopLogging_spec_new
        mov     eax, 0042E950h
        jmp     eax
    }
}

__declspec(naked) void on_stop_logging_destructor() //0042EF23
{
    __asm
    {
        mov     ecx, esi
        call    TCommCommandLog__stopLogging_spec_new
        mov     eax, 0042EF64h
        jmp     eax
    }
}

__declspec(naked) void on_stop_logging_world_destructor() //0061B0B9
{
    __asm
    {
        mov     esi, ecx
        push    edi
        mov     [esp + 10h], esi
        call    stop_spec_client
        mov     ecx, esi
        mov     eax, 0061B0C0h
        jmp     eax
    }
}

__declspec(naked) void on_set_game_ended() //004F849F
{
    __asm
    {
        call    stop_spec_client_no_wait_and_stop_logging
        mov     eax, 1
        cmp     bl, 4
        mov     ecx, 004F84A7h
        jmp     ecx
    }
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

    setHookCall((void*)0x006205E2, TCommCommandLog__startLogging_spec_new);
    setHookCall((void*)0x004328B4, TCommCommandLog__stopLogging_spec_new);
    setHookCall((void*)0x0061EA23, TCommCommandLog__stopLogging_spec_new);
    setHookCall((void*)0x0061EA9A, TCommCommandLog__stopLogging_spec_new);
    setHookCall((void*)0x00620609, TCommCommandLog__stopLogging_spec_new);
    setHook((void*)0x0042E916, on_stop_logging_init);
    setHook((void*)0x0042EF23, on_stop_logging_destructor);
    setHook((void*)0x0061B0B9, on_stop_logging_world_destructor);

    setHook((void*)0x004F849F, on_set_game_ended);
}
#pragma optimize( "", on )
