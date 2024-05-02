#include "stdafx.h"
#include "TCommunications_Handler2.h"

#include <queue>

__declspec(naked) void nullsub_0_arg()
{
    __asm
    {
        xor     eax, eax
        ret
    }
}

__declspec(naked) void nullsub_0_arg_ret_1()
{
    __asm
    {
        mov     eax, 1
        ret
    }
}

__declspec(naked) void nullsub_1_arg()
{
    __asm
    {
        xor     eax, eax
        ret     4
    }
}

__declspec(naked) void nullsub_2_arg()
{
    __asm
    {
        xor     eax, eax
        ret     8
    }
}

__declspec(naked) void nullsub_3_arg()
{
    __asm
    {
        xor     eax, eax
        ret     0Ch
    }
}

__declspec(naked) void nullsub_4_arg()
{
    __asm
    {
        xor     eax, eax
        ret     10h
    }
}

__declspec(naked) void nullsub_5_arg()
{
    __asm
    {
        xor     eax, eax
        ret     14h
    }
}

__declspec(naked) void nullsub_7_arg()
{
    __asm
    {
        xor     eax, eax
        ret     1Ch
    }
}

__declspec(naked) void nullsub_10_arg()
{
    __asm
    {
        xor     eax, eax
        ret     28h
    }
}

__declspec(naked) void nullsub_9_arg()
{
    __asm
    {
        xor     eax, eax
        ret     24h
    }
}

struct Command
{
    int id;
    int turn;
    int size;
    void* command;
    int issuer;
    explicit Command(int id_, void* command_, int size_, int issuer_, int turn_)
    {
        id = id_;
        turn = turn_;
        size = size_;
        issuer = issuer_;
        command = malloc(size);
        memcpy(command, command_, size);
    }
    explicit Command(Command&& other)
    {
        id = other.id;
        command = other.command;
        size = other.size;
        issuer = other.issuer;
        turn = other.turn;
        other.command = NULL;
    }
    ~Command()
    {
        free(command);
    }
};

class CommandQueue
{
private:
    std::queue<Command> q = {};
    int next_id = 0;
    void* command_buffer = NULL;
    int command_buffer_size = 1;
public:
    CommandQueue()
    {
        command_buffer = malloc(command_buffer_size);
    }
    ~CommandQueue()
    {
        free(command_buffer);
    }
    void insert_command(void* command, int size, int issuer, int turn)
    {
        //Command c(next_id++, command, size, issuer, turn);
        q.push(Command(next_id++, command, size, issuer, turn));
    }

    void* get_command(int turn, int* size)
    {
        if (q.empty())
        {
            size = 0;
            return NULL;
        }

        Command& c = q.front();
        if (c.turn == turn)
        {
            *size = c.size;
            while (c.size > command_buffer_size)
            {
                command_buffer_size *= 2;
                free(command_buffer);
                command_buffer = malloc(command_buffer_size);
            }
            memcpy(command_buffer, c.command, c.size);
            //free(c.command);
            q.pop();
            return command_buffer;
        }
        else
        {
            size = 0;
            return NULL;
        }
    }

    void clear()
    {
        q = {};
        next_id = 0;
    }
};

class NewCommHandler
{
private:
    struct CommPlayerData
    {
        std::string name = "";
        int humanity = 0;
    } comm_player_data [10];

    CommandQueue command_queue;

    unsigned int last_cycle_time = 0;
    unsigned int last_turn_time = 0;
    int current_turn = 0;
    int current_cycle = 0;
    int cycles_in_turn = 50;
    int cycles_in_current_turn = 0;

    TCommCommandLog* comm_log;

    bool pause_state = false;
    HWND window_handle = NULL;
    int me = 1;

public:
    void set_command_log(TCommCommandLog* l)
    {
        comm_log = l;
    }

    void set_window_handle(HWND hwnd)
    {
        window_handle = hwnd;
    }

    void send_window_message(WPARAM wParam, LPARAM lParam)
    {
        if (window_handle)
            PostMessageA(window_handle, WM_USER, wParam, lParam);
    }

    void set_humanity(int player_id, int humanity)
    {
        comm_player_data[player_id - 1].humanity = humanity;
    }

    int get_humanity(int player_id)
    {
        return comm_player_data[player_id - 1].humanity;
    }

    int game_cycle(unsigned int wt)  //return world time increment
    {
        if (pause_state)
            return 0;

        unsigned int current_time = timeGetTime();
        if (current_time - last_cycle_time > 16)
        {
            last_cycle_time = current_time;
            current_cycle++;
            cycles_in_current_turn++;
            if (cycles_in_current_turn >= cycles_in_turn)
            {
                cycles_in_current_turn = 0;
                current_turn++;
            }
            return 16;
        }
        else
            return 0;
    }

    const char* get_player_name(int player_id)
    {
        return comm_player_data[player_id - 1].name.c_str();
    }

    void set_player_name(int player_id, const char* name)
    {
        comm_player_data[player_id - 1].name = name;
    }

    void new_command(void* command, int size, int issuer)
    {
        if (comm_log->mReplaying <= 0)
            command_queue.insert_command(command, size, issuer, current_turn+3);
    }

    void* get_command()
    {
        void* command = NULL;
        int size = 0;
        if (comm_log->mReplaying > 0)
        {
            unsigned int wt = 0;
            int retval = TCommCommandLog__getNextLoggedCommand(comm_log, &command, (unsigned int*)&size, &wt);

            bool stop_replaying = false;
            TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
            if (retval == 2)
            {
                TCommCommandLog__stopReplaying(comm_log);
                TEasy_Panel* panel = (TEasy_Panel*)TPanelSystem__getTop(panel_system);
                if (panel)
                    TEasy_Panel__popupOKDialog2(panel, 9014, 0, 450, 100, 1); //Sequence error replaying game. Replaying stopped.
                game_screen->done_replaying = 1;
            }
            else if (retval == 0)
            {
                TCommCommandLog__stopReplaying(comm_log);
                game_screen->done_replaying = 1;
            }
            else if (retval != 1 && retval != 4)
            {
                TCommCommandLog__stopReplaying(comm_log);
                TEasy_Panel* panel = (TEasy_Panel*)TPanelSystem__getTop(panel_system);
                if (panel)
                    TEasy_Panel__popupOKDialog(panel, "Unknown error replaying game. Replaying stopped.", 0, 450, 100, 1);
                game_screen->done_replaying = 1;
            }
        }
        else
            command = command_queue.get_command(current_turn, &size);

        
        if (comm_log->mLogging > 0)
        {
            if (command && !TCommCommandLog__logCommand(comm_log, command, size, (*base_game)->world->world_time))
            {
                TCommCommandLog__stopLogging(comm_log);
                TEasy_Panel* panel = (TEasy_Panel*)TPanelSystem__getTop(panel_system);
                if (panel)
                    TEasy_Panel__popupOKDialog2(panel, 9011, 0, 450, 100, 1); //Error logging game. Logging stopped.
            }
        }

        return command;
    }

    void game_over()
    {
        command_queue.clear();
        current_turn = 0;
        current_cycle = 0;
        cycles_in_current_turn = 0;

        pause_state = false;
        me = 1;
    }

    void toggle_pause()
    {
        pause_state = !pause_state;
        if (pause_state)
            send_window_message(6050, me);
        else
            send_window_message(6051, me);  
    }

    int is_paused()
    {
        return pause_state;
    }
};

class TCommunications_Handler2
{
private:
    NewCommHandler* comm_handler;

public:
    TCommunications_Handler2()
    {
        comm_handler = new NewCommHandler;
    }

    ~TCommunications_Handler2()
    {
        delete comm_handler;
        comm_handler = NULL;
    }

    void set_command_log(TCommCommandLog* l)
    {
        comm_handler->set_command_log(l);
    }

    void set_window_handle(HWND hwnd)
    {
        comm_handler->set_window_handle(hwnd);
    }

    char* __thiscall WaitingOnNamedInfo(unsigned int PlayerNo)
    {
        return "NOT IMPLEMENTED";
    };

    char* __thiscall getCommLogInfo()
    {
        return "NOT IMPLEMENTED";
    }

    char* __thiscall GetPlayerName(unsigned int PlayerNo)
    {
        return (char*)comm_handler->get_player_name(PlayerNo);
    };

    int __thiscall GetPlayerHumanity(unsigned int PlayerNo)
    {
        return comm_handler->get_humanity(PlayerNo);
    }

    void __thiscall SetPlayerHumanity(unsigned int PlayerNo, int Type)
    {
        comm_handler->set_humanity(PlayerNo, Type);
    }

    int __thiscall IsPlayerHuman(unsigned int PlayerNo)
    {
        int humanity = comm_handler->get_humanity(PlayerNo);
        return humanity == 2 || humanity == 5;
    }

    int __thiscall IsPlayerComputer(unsigned int PlayerNo)
    {
        int humanity = comm_handler->get_humanity(PlayerNo);
        return humanity == 4;
    }

    int __thiscall DoCycle(unsigned int WorldTime)
    {
        return comm_handler->game_cycle(WorldTime);
    }

    void __cdecl show_not_impl_msg(bool msg_box, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        char debug_str[500];
        vsnprintf(debug_str, _countof(debug_str) - 1, format, ap);
        log("CommHand: %s", debug_str);
        va_end(ap);

        if (msg_box)
            MessageBox(NULL, debug_str, "CommHand - NOT IMPLEMENTED", 0);
    }

    void __thiscall GameOver()
    {
        comm_handler->game_over();
        //show_not_impl_msg(true, "GameOver()");
    }

    void __thiscall StopIfSyncFail(int b)
    {
        show_not_impl_msg(false, "StopIfSyncFail(%d)", b);
    }

    void __thiscall ShowSyncChatMsgs(int b)
    {
        show_not_impl_msg(false, "ShowSyncChatMsgs(%d)", b);
    }

    void __thiscall SetSpeedControl(int b)
    {
        show_not_impl_msg(false, "SetSpeedControl(%d)", b);
    }

    void __thiscall DestroyMultiplayerGame()
    {
        show_not_impl_msg(true, "DestroyMultiplayerGame()");
    }

    void* __thiscall get_command()
    {
        return comm_handler->get_command();
    }

    unsigned __int8 __thiscall new_command(void* Command, int Size, int issuer)
    {
        comm_handler->new_command(Command, Size, issuer);
        return 0;
    }

    void __thiscall LastWorldRandom(int lwr)
    {
        //show_not_impl_msg(false, "LastWorldRandom(%d)", lwr);
    }

    void __thiscall SaveInitialGameHeader()
    {
        show_not_impl_msg(true, "SaveInitialGameHeader()");
    }

    void __thiscall incrementVoteCount()
    {
        show_not_impl_msg(true, "incrementVoteCount()");
    }

    void __thiscall getDefaultMultiplayerSaveFilename(char* file_name, char* savetitle, int ext)
    {
        show_not_impl_msg(true, "getDefaultMultiplayerSaveFilename(%s, %s, %d)",
            file_name ? file_name : "NULL", savetitle ? savetitle : "NULL", ext);
    }

    void __thiscall ReceiveGameMessages()
    {
        //show_not_impl_msg(false, "ReceiveGameMessages()");
    }

    void __thiscall ShutdownGameMessages()
    {
        show_not_impl_msg(true, "ShutdownGameMessages()");
    }

    void __thiscall NotifyWindow(int msg)
    {
        //show_not_impl_msg(true, "NotifyWindow(%d)", msg);
        comm_handler->send_window_message(msg, 0);
    }

    void __thiscall NotifyWindowParam(int msg, int lparam)
    {
        //show_not_impl_msg(true, "NotifyWindowParam(%d, %d)", msg, lparam);
        comm_handler->send_window_message(msg, lparam);
    }

    int __thiscall verifyNametable()
    {
        show_not_impl_msg(true, "verifyNametable()");
        return 0;
    }

    int __thiscall GetPlayerStopTurn(unsigned int player)
    {
        show_not_impl_msg(true, "GetPlayerStopTurn(%u)", player);
        return 0;
    }

    HRESULT __thiscall SendSharedData(int SendImmediately, int needsToBeSent)
    {
        //show_not_impl_msg(false, "SendSharedData(%d, %d)", SendImmediately, needsToBeSent);
        return 0;
    }

    void __thiscall LogCommentsOnHost(char* comments)
    {
        show_not_impl_msg(true, "LogCommentsOnHost(%s)", comments ? comments : "NULL");
    }

    void __thiscall SendGroupChatMsg(char* Text)
    {
        show_not_impl_msg(true, "SendGroupChatMsg(%s)", Text ? Text : "NULL");
    }

    void __thiscall SendChatMsgAll(char* Text)
    {
        show_not_impl_msg(true, "SendChatMsgAll(%s)", Text ? Text : "NULL");
    }

    int __thiscall TXChat(unsigned int GamerID, unsigned __int8* DestMap, char* Text)
    {
        //show_not_impl_msg(true, "TXChat(%d, %s, %s)",
        //    GamerID, DestMap ? DestMap : (unsigned __int8*)"NULL", Text ? Text : "NULL");
        return 0;
    }

    void __thiscall SendSpeedChange(unsigned int Frames, unsigned int Granularity, unsigned int turn)
    {
        show_not_impl_msg(true, "SendSpeedChange(%u, %u, %u)", Frames, Granularity, turn);
    }

    void __thiscall ExitPregame()
    {
        show_not_impl_msg(true, "ExitPregame()");
    }

    void __thiscall SendIResignMsg()
    {
        show_not_impl_msg(true, "SendIResignMsg()");
    }

    int __thiscall AllPlayersAcknowledged()
    {
        show_not_impl_msg(true, "AllPlayersAcknowledged()");
        return 0;
    }

    int __thiscall SendChecksumMessage(unsigned int worldtime, unsigned int random, int csum1, int csum2, int csum3, int csum4, int csum5)
    {
        show_not_impl_msg(true, "SendChecksumMessage(%d, %d, %d, %d, %d, %d, %d)", worldtime, random, csum1, csum2, csum3, csum4, csum5);
        return 0;
    }

    int __thiscall CountWaitingMessages()
    {
        show_not_impl_msg(true, "CountWaitingMessages()");
        return 0;
    }

    int __thiscall GetCommunicationsStatus()
    {
        show_not_impl_msg(false, "GetCommunicationsStatus()");
        return 0;
    }

    int __thiscall UnlinkToLevel(int lvl)
    {
        show_not_impl_msg(false, "UnlinkToLevel(%d)", lvl);
        return 0;
    }

    int __thiscall AnalyzeCommunicationsStatus()
    {
        show_not_impl_msg(true, "AnalyzeCommunicationsStatus()");
        return 0;
    }

    int __thiscall IsPaused()
    {
        //show_not_impl_msg(false, "IsPaused()");
        return comm_handler->is_paused();
    }

    int __thiscall TogglePauseGame()
    {
        //show_not_impl_msg(false, "TogglePauseGame()");
        comm_handler->toggle_pause();
        return 0;
    }

    unsigned int __thiscall GetRandomSeed()
    {
        //show_not_impl_msg(true, "GetRandomSeed()");
        return 0;
    }

    int __thiscall WhoAmI()
    {
        //show_not_impl_msg(false, "WhoAmI()");
        return 1;
    }

    int __thiscall IsHost()
    {
        //show_not_impl_msg(false, "IsHost()");
        return 1;
    }

    unsigned int __thiscall GetPlayerLatency(unsigned int pnum)
    {
        //show_not_impl_msg(true, "GetPlayerLatency(%d)", pnum);
        return 0;
    }

    int __thiscall IsLobbyLaunched()
    {
        show_not_impl_msg(true, "IsLobbyLaunched()");
        return 0;
    }

    int __thiscall LaunchLobbyGame()
    {
        show_not_impl_msg(true, "LaunchLobbyGame()");
        return 0;
    }

    void __thiscall SetRestoringGameTitle(char* Game, _GUID* guid, unsigned int checksum)
    {
        wchar_t szGuidW[40];
        char szGuidA[40];
        StringFromGUID2(*guid, szGuidW, _countof(szGuidW));
        WideCharToMultiByte(CP_ACP, 0, szGuidW, -1, szGuidA, 40, NULL, NULL);
        show_not_impl_msg(true, "SetRestoringGameTitle(%s, %s, %u)",
            Game ? Game : "NULL", szGuidA, checksum);
    }

    int __thiscall SetGameTitle(char* Game, unsigned int Time)
    {
        show_not_impl_msg(true, "SetGameTitle(%s, %u)", Game ? Game : "NULL", Time);
        return 0;
    }

    void __thiscall SetMyPlayerName(char* Name)
    {
        show_not_impl_msg(true, "SetMyPlayerName(%s)", Name ? Name : "NULL");
    }

    void __thiscall SetPlayerName(unsigned int PlayerNo, char* Name)
    {
        comm_handler->set_player_name(PlayerNo, Name);
    }

    HWND__* __thiscall SetWindowHandle(HWND__* hwnd)
    {
        //show_not_impl_msg(true, "SetWindowHandle(%p)", hwnd);
        comm_handler->set_window_handle(hwnd);
        return hwnd;
    }

    int __thiscall CreateMultiplayerGame(_GUID* guid, unsigned int checksum, int enableDPP)
    {
        wchar_t szGuidW[40];
        char szGuidA[40];
        StringFromGUID2(*guid, szGuidW, _countof(szGuidW));
        WideCharToMultiByte(CP_ACP, 0, szGuidW, -1, szGuidA, 40, NULL, NULL);
        show_not_impl_msg(true, "CreateMultiplayerGame(%s, %u, %d)", szGuidA, checksum, enableDPP);
        return 0;
    }

    HRESULT __thiscall InitJoinGame(int localIPGame, char* specificIPAddress)
    {
        show_not_impl_msg(true, "InitJoinGame(%d, %s)", localIPGame, specificIPAddress ? specificIPAddress : "NULL");
        return 0;
    }

    int __thiscall JoinMultiplayerGame(_GUID* GameGuid)
    {
        wchar_t szGuidW[40];
        char szGuidA[40];
        StringFromGUID2(*GameGuid, szGuidW, _countof(szGuidW));
        WideCharToMultiByte(CP_ACP, 0, szGuidW, -1, szGuidA, 40, NULL, NULL);
        show_not_impl_msg(true, "JoinMultiplayerGame(%s)", szGuidA);
        return 0;
    }

    void __thiscall ClearPlayersAcknowledged()
    {
        show_not_impl_msg(true, "ClearPlayersAcknowledged()");
    }

    void __thiscall SetServiceGUID(GUID inGuid)
    {
        wchar_t szGuidW[40];
        char szGuidA[40];
        StringFromGUID2(inGuid, szGuidW, _countof(szGuidW));
        WideCharToMultiByte(CP_ACP, 0, szGuidW, -1, szGuidA, 40, NULL, NULL);
        show_not_impl_msg(true, "SetServiceGUID(%s)", szGuidA);
    }

    void __thiscall UpdatePlayers()
    {
        show_not_impl_msg(true, "UpdatePlayers()");
    }

    int __thiscall SetMyReadiness(
        int Ready,
        unsigned int User1,
        unsigned int User2,
        unsigned int User3,
        unsigned int User4,
        unsigned int User5,
        unsigned int User6,
        unsigned int User7,
        unsigned __int8 recordGame,
        unsigned int savedPos)
    {
        show_not_impl_msg(true, "SetMyReadiness(%d, (%u, %u, %u, %u, %u, %u, %u), %u, %u",
            Ready, User1, User2, User3, User4, User5, User6, User7, recordGame, savedPos);
        return 0;
    }

    void __thiscall GetClientReadiness(
        unsigned int PlayerNo,
        int* Ready,
        unsigned int* User1,
        unsigned int* User2,
        unsigned int* User3,
        unsigned int* User4,
        unsigned int* User5,
        unsigned int* User6,
        unsigned int* User7)
    {
        show_not_impl_msg(true, "GetClientReadiness(%d, ...)", PlayerNo);
        *Ready = 1; *User1 = 1; *User2 = 1; *User3 = 1; *User4 = 1; *User5 = 1; *User6 = 1; *User7 = 1;
    }

    int __thiscall IsPlayerReady(unsigned int PlayerNo)
    {
        show_not_impl_msg(true, "IsPlayerReady(%d)", PlayerNo);
        return 1;
    }

    int __thiscall ComputerPlayerGameStart()
    {
        //show_not_impl_msg(false, "ComputerPlayerGameStart()");
        return 1;
    }

    int __thiscall GameStarted()
    {
        show_not_impl_msg(true, "GameStarted()");
        return 1;
    }

    int __thiscall MultiplayerGameStart()
    {
        show_not_impl_msg(false, "MultiplayerGameStart()");
        return 1;
    }

    int __thiscall AllPlayersReady()
    {
        show_not_impl_msg(true, "AllPlayersReady()");
        return 1;
    }

    int __thiscall Kick(unsigned int PlayerNo)
    {
        show_not_impl_msg(true, "Kick(%d)", PlayerNo);
        return 0;
    }

    int __thiscall SetMyGameOptions(char* vpData, unsigned int dwSize)
    {
        show_not_impl_msg(true, "SetMyGameOptions(dwSize=%u)", dwSize);
        return 1;
    }

    void* __thiscall GetMyGameOptions(unsigned int* dwSize)
    {
        show_not_impl_msg(true, "GetMyGameOptions(...)", dwSize);
        *dwSize = 0;
        return NULL;
    }

    unsigned int __thiscall GetHostPlayerNumber()
    {
        show_not_impl_msg(true, "GetHostPlayerNumber()");
        return 1;
    }

    int __thiscall SendZoneMessage(char* Data, unsigned int dwSize)
    {
        show_not_impl_msg(true, "SendZoneMessage(dwSize=%u)", dwSize);
        return 1;
    }

    void __thiscall SetSpeedV1(unsigned int v1)
    {
        show_not_impl_msg(true, "SetSpeedV1(%u)", v1);
    }

    int __thiscall IsPlayerOutOfSync(unsigned int pnum, unsigned int worldturn)
    {
        show_not_impl_msg(true, "IsPlayerOutOfSync(%u, %u)", pnum, worldturn);
        return 0;
    }

    void __thiscall clearOldReceivedFile()
    {
        show_not_impl_msg(false, "clearOldReceivedFile()");
    }

    void __thiscall StopAllXfers()
    {
        show_not_impl_msg(false, "StopAllXfers()");
    }

    int __thiscall ReceivingFileCheck()
    {
        show_not_impl_msg(false, "ReceivingFileCheck()");
        return 0;
    }

    int __thiscall SendingFileCheck()
    {
        show_not_impl_msg(false, "SendingFileCheck()");
        return 0;
    }

    int __thiscall GetPlayerPercentageComplete(unsigned int forPlayer)
    {
        show_not_impl_msg(true, "GetPlayerPercentageComplete(%u)", forPlayer);
        return 0;
    }

    void __thiscall sendVoteMessage(void* msg)
    {
        show_not_impl_msg(true, "sendVoteMessage(...)");
    }

    void __thiscall clearVoteDialogPtr()
    {
        show_not_impl_msg(true, "clearVoteDialogPtr()");
    }

    void __thiscall activateVotingDialogForSaveAndExit(char* fname, unsigned int checksum, int fromPlayer)
    {
        show_not_impl_msg(true, "activateVotingDialogForSaveAndExit(%s, %u, %d)",
            fname ? fname : "NULL", checksum, fromPlayer);
    }

    int __thiscall IsMultiplayer()
    {
        show_not_impl_msg(true, "IsMultiplayer()");
        return 0;
    }

    unsigned int __thiscall GetSavedMyPosition()
    {
        show_not_impl_msg(true, "GetSavedMyPosition()");
        return 0;
    }

    unsigned int __thiscall GetSavedPlayerPosition(unsigned int p)
    {
        show_not_impl_msg(true, "GetSavedPlayerPosition(%u)", p);
        return 0;
    }

    void __thiscall setRestoringGame(int val)
    {
        show_not_impl_msg(true, "setRestoringGame(%d)", val);
    }

    int __thiscall GetRestoringGame()
    {
        show_not_impl_msg(false, "GetRestoringGame()");
        return 0;
    }

    void __thiscall setCurrentHeader(char* headerFileName)
    {
        show_not_impl_msg(true, "setCurrentHeader(%s)", headerFileName ? headerFileName : "NULL");
    }

    void __thiscall OutOfSyncSave()
    {
        show_not_impl_msg(true, "OutOfSyncSave()");
    }

    void __thiscall SaveMultiplayerGameHeader(char* saveFileName, int retrieveHeader)
    {
        show_not_impl_msg(true, "SaveMultiplayerGameHeader(%s, %d)", saveFileName ? saveFileName : "NULL", retrieveHeader);
    }

    int __thiscall getPlayerHeaderIndex(cHeaderInfo* hinfo, int slotNum)
    {
        show_not_impl_msg(true, "getPlayerHeaderIndex(..., %d)", slotNum);
        return 0;
    }

    void __thiscall restoreChatGroups()
    {
        show_not_impl_msg(true, "restoreChatGroups()");
    }

    void __thiscall SetMeToRecvSavegame(int yesno)
    {
        show_not_impl_msg(true, "SetMeToRecvSavegame(%d)", yesno);
    }

    void __thiscall getGUIDandChecksum(_GUID* guid, unsigned int* checksum)
    {
        memset(guid, 0, sizeof(_GUID));
        *checksum = 0;
        show_not_impl_msg(true, "getGUIDandChecksum(...)");
    }

    void __thiscall ReadyToReceive(int yesno)
    {
        show_not_impl_msg(false, "ReadyToReceive(%d)", yesno);
    }

    void __thiscall SetSaveChecksums(unsigned int checksum)
    {
        show_not_impl_msg(true, "SetSaveChecksums(%u)", checksum);
    }

    void __thiscall NotifyStartSaving(int notifyOthers, char* name)
    {
        show_not_impl_msg(true, "NotifyStartSaving(%d, %s)", notifyOthers, name ? name : "NULL");
    }

    int __thiscall getNeedToRecvFile()
    {
        show_not_impl_msg(true, "getNeedToRecvFile()");
        return 0;
    }

    void __thiscall SetNeedScenario(int tf, char* scenarioName)
    {
        show_not_impl_msg(false, "SetNeedScenario(%d, %s)", tf, scenarioName ? scenarioName : "NULL");
    }

    int __thiscall GetNeedScenario()
    {
        show_not_impl_msg(true, "GetNeedScenario()");
        return 0;
    }

    void __thiscall SendAroundScenario(char* fileName)
    {
        show_not_impl_msg(true, "SendAroundScenario(%s)", fileName ? fileName : "NULL");
    }

    int __thiscall sendPregameMessage(void* pregameMsg, int toPlayer)
    {
        show_not_impl_msg(true, "sendPregameMessage(..., %d)", toPlayer);
        return 0;
    }

    void __thiscall pollReceieve()
    {
        //show_not_impl_msg(false, "pollReceieve()");
    }

    int __thiscall PregameCycle()
    {
        //show_not_impl_msg(false, "PregameCycle()");
        return 0;
    }

    int __thiscall getGameOutcomeForPlayer(int index)
    {
        show_not_impl_msg(true, "getGameOutcomeForPlayer(%d)", index);
        return 0;
    }

    void __thiscall setGameOutcomeForPlayer(int index, int outcome)
    {
        show_not_impl_msg(true, "setGameOutcomeForPlayer(%d, %d)", index, outcome);
    }

    void __thiscall SetSavedPos(int player, int pos)
    {
        show_not_impl_msg(true, "SetSavedPos(%d, %d)", player, pos);
    }
};

__declspec(naked) void TCommunications_Handler2__WaitingOnNamedInfo()
{
    __asm jmp TCommunications_Handler2::WaitingOnNamedInfo;
}

__declspec(naked) void TCommunications_Handler2__getCommLogInfo()
{
    __asm jmp TCommunications_Handler2::getCommLogInfo;
}

__declspec(naked) void TCommunications_Handler2__GetPlayerName()
{
    __asm jmp TCommunications_Handler2::GetPlayerName;
}

__declspec(naked) void TCommunications_Handler2__GetPlayerHumanity()
{
    __asm jmp TCommunications_Handler2::GetPlayerHumanity;
}

__declspec(naked) void TCommunications_Handler2__SetPlayerHumanity()
{
    __asm jmp TCommunications_Handler2::SetPlayerHumanity;
}

__declspec(naked) void TCommunications_Handler2__IsPlayerHuman()
{
    __asm jmp TCommunications_Handler2::IsPlayerHuman;
}

__declspec(naked) void TCommunications_Handler2__IsPlayerComputer()
{
    __asm jmp TCommunications_Handler2::IsPlayerComputer;
}

__declspec(naked) void TCommunications_Handler2__DoCycle()
{
    __asm jmp TCommunications_Handler2::DoCycle;
}

__declspec(naked) void TCommunications_Handler2__GameOver()
{
    __asm jmp TCommunications_Handler2::GameOver;
}

__declspec(naked) void TCommunications_Handler2__StopIfSyncFail()
{
    __asm jmp TCommunications_Handler2::StopIfSyncFail;
}

__declspec(naked) void TCommunications_Handler2__ShowSyncChatMsgs()
{
    __asm jmp TCommunications_Handler2::ShowSyncChatMsgs;
}

__declspec(naked) void TCommunications_Handler2__SetSpeedControl()
{
    __asm jmp TCommunications_Handler2::SetSpeedControl;
}

__declspec(naked) void TCommunications_Handler2__DestroyMultiplayerGame()
{
    __asm jmp TCommunications_Handler2::DestroyMultiplayerGame;
}

__declspec(naked) void TCommunications_Handler2__get_command()
{
    __asm jmp TCommunications_Handler2::get_command;
}

__declspec(naked) void TCommunications_Handler2__new_command()
{
    __asm jmp TCommunications_Handler2::new_command;
}

__declspec(naked) void TCommunications_Handler2__LastWorldRandom()
{
    __asm jmp TCommunications_Handler2::LastWorldRandom;
}

__declspec(naked) void TCommunications_Handler2__SaveInitialGameHeader()
{
    __asm jmp TCommunications_Handler2::SaveInitialGameHeader;
}

__declspec(naked) void TCommunications_Handler2__incrementVoteCount()
{
    __asm jmp TCommunications_Handler2::incrementVoteCount;
}

__declspec(naked) void TCommunications_Handler2__getDefaultMultiplayerSaveFilename()
{
    __asm jmp TCommunications_Handler2::getDefaultMultiplayerSaveFilename;
}

__declspec(naked) void TCommunications_Handler2__ReceiveGameMessages()
{
    __asm jmp TCommunications_Handler2::ReceiveGameMessages;
}

__declspec(naked) void TCommunications_Handler2__ShutdownGameMessages()
{
    __asm jmp TCommunications_Handler2::ShutdownGameMessages;
}

__declspec(naked) void TCommunications_Handler2__NotifyWindow()
{
    __asm jmp TCommunications_Handler2::NotifyWindow;
}

__declspec(naked) void TCommunications_Handler2__verifyNametable()
{
    __asm jmp TCommunications_Handler2::verifyNametable;
}

__declspec(naked) void TCommunications_Handler2__NotifyWindowParam()
{
    __asm jmp TCommunications_Handler2::NotifyWindowParam;
}

__declspec(naked) void TCommunications_Handler2__GetPlayerStopTurn()
{
    __asm jmp TCommunications_Handler2::GetPlayerStopTurn;
}

__declspec(naked) void TCommunications_Handler2__SendSharedData()
{
    __asm jmp TCommunications_Handler2::SendSharedData;
}

__declspec(naked) void TCommunications_Handler2__LogCommentsOnHost()
{
    __asm jmp TCommunications_Handler2::LogCommentsOnHost;
}

__declspec(naked) void TCommunications_Handler2__SendGroupChatMsg()
{
    __asm jmp TCommunications_Handler2::SendGroupChatMsg;
}

__declspec(naked) void TCommunications_Handler2__SendChatMsgAll()
{
    __asm jmp TCommunications_Handler2::SendChatMsgAll;
}

__declspec(naked) void TCommunications_Handler2__TXChat()
{
    __asm jmp TCommunications_Handler2::TXChat;
}

__declspec(naked) void TCommunications_Handler2__SendSpeedChange()
{
    __asm jmp TCommunications_Handler2::SendSpeedChange;
}

__declspec(naked) void TCommunications_Handler2__ExitPregame()
{
    __asm jmp TCommunications_Handler2::ExitPregame;
}

__declspec(naked) void TCommunications_Handler2__SendIResignMsg()
{
    __asm jmp TCommunications_Handler2::SendIResignMsg;
}

__declspec(naked) void TCommunications_Handler2__AllPlayersAcknowledged()
{
    __asm jmp TCommunications_Handler2::AllPlayersAcknowledged;
}

__declspec(naked) void TCommunications_Handler2__SendChecksumMessage()
{
    __asm jmp TCommunications_Handler2::SendChecksumMessage;
}

__declspec(naked) void TCommunications_Handler2__CountWaitingMessages()
{
    __asm jmp TCommunications_Handler2::CountWaitingMessages;
}

__declspec(naked) void TCommunications_Handler2__GetCommunicationsStatus()
{
    __asm jmp TCommunications_Handler2::GetCommunicationsStatus;
}

__declspec(naked) void TCommunications_Handler2__UnlinkToLevel()
{
    __asm jmp TCommunications_Handler2::UnlinkToLevel;
}

__declspec(naked) void TCommunications_Handler2__AnalyzeCommunicationsStatus()
{
    __asm jmp TCommunications_Handler2::AnalyzeCommunicationsStatus;
}

__declspec(naked) void TCommunications_Handler2__IsPaused()
{
    __asm jmp TCommunications_Handler2::IsPaused;
}

__declspec(naked) void TCommunications_Handler2__TogglePauseGame()
{
    __asm jmp TCommunications_Handler2::TogglePauseGame;
}

__declspec(naked) void TCommunications_Handler2__GetRandomSeed()
{
    __asm jmp TCommunications_Handler2::GetRandomSeed;
}

__declspec(naked) void TCommunications_Handler2__WhoAmI()
{
    __asm jmp TCommunications_Handler2::WhoAmI;
}

__declspec(naked) void TCommunications_Handler2__IsHost()
{
    __asm jmp TCommunications_Handler2::IsHost;
}

__declspec(naked) void TCommunications_Handler2__GetPlayerLatency()
{
    __asm jmp TCommunications_Handler2::GetPlayerLatency;
}

__declspec(naked) void TCommunications_Handler2__IsLobbyLaunched()
{
    __asm jmp TCommunications_Handler2::IsLobbyLaunched;
}

__declspec(naked) void TCommunications_Handler2__LaunchLobbyGame()
{
    __asm jmp TCommunications_Handler2::LaunchLobbyGame;
}

__declspec(naked) void TCommunications_Handler2__SetRestoringGameTitle()
{
    __asm jmp TCommunications_Handler2::SetRestoringGameTitle;
}

/*__declspec(naked) void TCommunications_Handler2__ParseRestoringGameTitle()
{
    __asm jmp TCommunications_Handler2::DoCycle;
}*/

__declspec(naked) void TCommunications_Handler2__SetGameTitle()
{
    __asm jmp TCommunications_Handler2::SetGameTitle;
}

__declspec(naked) void TCommunications_Handler2__SetMyPlayerName()
{
    __asm jmp TCommunications_Handler2::SetMyPlayerName;
}

__declspec(naked) void TCommunications_Handler2__SetPlayerName()
{
    __asm jmp TCommunications_Handler2::SetPlayerName;
}

__declspec(naked) void TCommunications_Handler2__SetWindowHandle()
{
    __asm jmp TCommunications_Handler2::SetWindowHandle;
}

__declspec(naked) void TCommunications_Handler2__CreateMultiplayerGame()
{
    __asm jmp TCommunications_Handler2::CreateMultiplayerGame;
}

__declspec(naked) void TCommunications_Handler2__InitJoinGame()
{
    __asm jmp TCommunications_Handler2::InitJoinGame;
}

__declspec(naked) void TCommunications_Handler2__JoinMultiplayerGame()
{
    __asm jmp TCommunications_Handler2::JoinMultiplayerGame;
}

__declspec(naked) void TCommunications_Handler2__ClearPlayersAcknowledged()
{
    __asm jmp TCommunications_Handler2::ClearPlayersAcknowledged;
}

__declspec(naked) void TCommunications_Handler2__SetServiceGUID()
{
    __asm jmp TCommunications_Handler2::SetServiceGUID;
}

__declspec(naked) void TCommunications_Handler2__UpdatePlayers()
{
    __asm jmp TCommunications_Handler2::UpdatePlayers;
}

__declspec(naked) void TCommunications_Handler2__SetMyReadiness()
{
    __asm jmp TCommunications_Handler2::SetMyReadiness;
}

__declspec(naked) void TCommunications_Handler2__GetClientReadiness()
{
    __asm jmp TCommunications_Handler2::GetClientReadiness;
}

__declspec(naked) void TCommunications_Handler2__IsPlayerReady()
{
    __asm jmp TCommunications_Handler2::IsPlayerReady;
}

__declspec(naked) void TCommunications_Handler2__ComputerPlayerGameStart()
{
    __asm jmp TCommunications_Handler2::ComputerPlayerGameStart;
}

__declspec(naked) void TCommunications_Handler2__GameStarted()
{
    __asm jmp TCommunications_Handler2::GameStarted;
}

__declspec(naked) void TCommunications_Handler2__MultiplayerGameStart()
{
    __asm jmp TCommunications_Handler2::MultiplayerGameStart;
}

__declspec(naked) void TCommunications_Handler2__AllPlayersReady()
{
    __asm jmp TCommunications_Handler2::AllPlayersReady;
}

__declspec(naked) void TCommunications_Handler2__Kick()
{
    __asm jmp TCommunications_Handler2::Kick;
}

__declspec(naked) void TCommunications_Handler2__SetMyGameOptions()
{
    __asm jmp TCommunications_Handler2::SetMyGameOptions;
}

__declspec(naked) void TCommunications_Handler2__GetMyGameOptions()
{
    __asm jmp TCommunications_Handler2::GetMyGameOptions;
}

__declspec(naked) void TCommunications_Handler2__GetHostPlayerNumber()
{
    __asm jmp TCommunications_Handler2::GetHostPlayerNumber;
}

__declspec(naked) void TCommunications_Handler2__SendZoneMessage()
{
    __asm jmp TCommunications_Handler2::SendZoneMessage;
}

__declspec(naked) void TCommunications_Handler2__SetSpeedV1()
{
    __asm jmp TCommunications_Handler2::SetSpeedV1;
}

__declspec(naked) void TCommunications_Handler2__IsPlayerOutOfSync()
{
    __asm jmp TCommunications_Handler2::IsPlayerOutOfSync;
}

__declspec(naked) void TCommunications_Handler2__clearOldReceivedFile()
{
    __asm jmp TCommunications_Handler2::clearOldReceivedFile;
}

__declspec(naked) void TCommunications_Handler2__StopAllXfers()
{
    __asm jmp TCommunications_Handler2::StopAllXfers;
}

__declspec(naked) void TCommunications_Handler2__ReceivingFileCheck()
{
    __asm jmp TCommunications_Handler2::ReceivingFileCheck;
}

__declspec(naked) void TCommunications_Handler2__SendingFileCheck()
{
    __asm jmp TCommunications_Handler2::SendingFileCheck;
}

__declspec(naked) void TCommunications_Handler2__GetPlayerPercentageComplete()
{
    __asm jmp TCommunications_Handler2::GetPlayerPercentageComplete;
}

__declspec(naked) void TCommunications_Handler2__sendVoteMessage()
{
    __asm jmp TCommunications_Handler2::sendVoteMessage;
}

__declspec(naked) void TCommunications_Handler2__clearVoteDialogPtr()
{
    __asm jmp TCommunications_Handler2::clearVoteDialogPtr;
}

__declspec(naked) void TCommunications_Handler2__activateVotingDialogForSaveAndExit()
{
    __asm jmp TCommunications_Handler2::activateVotingDialogForSaveAndExit;
}

__declspec(naked) void TCommunications_Handler2__IsMultiplayer()
{
    __asm jmp TCommunications_Handler2::IsMultiplayer;
}

__declspec(naked) void TCommunications_Handler2__GetSavedMyPosition()
{
    __asm jmp TCommunications_Handler2::GetSavedMyPosition;
}

__declspec(naked) void TCommunications_Handler2__GetSavedPlayerPosition()
{
    __asm jmp TCommunications_Handler2::GetSavedPlayerPosition;
}

__declspec(naked) void TCommunications_Handler2__setRestoringGame()
{
    __asm jmp TCommunications_Handler2::setRestoringGame;
}

__declspec(naked) void TCommunications_Handler2__GetRestoringGame()
{
    __asm jmp TCommunications_Handler2::GetRestoringGame;
}

__declspec(naked) void TCommunications_Handler2__setCurrentHeader()
{
    __asm jmp TCommunications_Handler2::setCurrentHeader;
}

__declspec(naked) void TCommunications_Handler2__OutOfSyncSave()
{
    __asm jmp TCommunications_Handler2::OutOfSyncSave;
}

__declspec(naked) void TCommunications_Handler2__SaveMultiplayerGameHeader()
{
    __asm jmp TCommunications_Handler2::SaveMultiplayerGameHeader;
}

__declspec(naked) void TCommunications_Handler2__getPlayerHeaderIndex()
{
    __asm jmp TCommunications_Handler2::getPlayerHeaderIndex;
}

__declspec(naked) void TCommunications_Handler2__restoreChatGroups()
{
    __asm jmp TCommunications_Handler2::restoreChatGroups;
}

__declspec(naked) void TCommunications_Handler2__SetMeToRecvSavegame()
{
    __asm jmp TCommunications_Handler2::SetMeToRecvSavegame;
}

__declspec(naked) void TCommunications_Handler2__getGUIDandChecksum()
{
    __asm jmp TCommunications_Handler2::getGUIDandChecksum;
}

__declspec(naked) void TCommunications_Handler2__ReadyToReceive()
{
    __asm jmp TCommunications_Handler2::ReadyToReceive;
}

__declspec(naked) void TCommunications_Handler2__SetSaveChecksums()
{
    __asm jmp TCommunications_Handler2::SetSaveChecksums;
}

__declspec(naked) void TCommunications_Handler2__NotifyStartSaving()
{
    __asm jmp TCommunications_Handler2::NotifyStartSaving;
}

__declspec(naked) void TCommunications_Handler2__getNeedToRecvFile()
{
    __asm jmp TCommunications_Handler2::getNeedToRecvFile;
}

__declspec(naked) void TCommunications_Handler2__SetNeedScenario()
{
    __asm jmp TCommunications_Handler2::SetNeedScenario;
}

__declspec(naked) void TCommunications_Handler2__GetNeedScenario()
{
    __asm jmp TCommunications_Handler2::GetNeedScenario;
}

__declspec(naked) void TCommunications_Handler2__SendAroundScenario()
{
    __asm jmp TCommunications_Handler2::SendAroundScenario;
}

__declspec(naked) void TCommunications_Handler2__sendPregameMessage()
{
    __asm jmp TCommunications_Handler2::sendPregameMessage;
}

__declspec(naked) void TCommunications_Handler2__pollReceieve()
{
    __asm jmp TCommunications_Handler2::pollReceieve;
}

__declspec(naked) void TCommunications_Handler2__PregameCycle()
{
    __asm jmp TCommunications_Handler2::PregameCycle;
}

__declspec(naked) void TCommunications_Handler2__getGameOutcomeForPlayer()
{
    __asm jmp TCommunications_Handler2::getGameOutcomeForPlayer;
}

__declspec(naked) void TCommunications_Handler2__setGameOutcomeForPlayer()
{
    __asm jmp TCommunications_Handler2::setGameOutcomeForPlayer;
}

__declspec(naked) void TCommunications_Handler2__SetSavedPos()
{
    __asm jmp TCommunications_Handler2::SetSavedPos;
}

TCommunications_Handler2* __stdcall TCommunications_Handler2_init2(TCommunications_Handler2* b, HWND__* WinHandle, unsigned __int8 MaxPlayers, TChat* ChatHandle, _GUID)
{
    //void* b = malloc(0x47D0);
    memset(b, 0, 0x47D0);
    TCommunications_Handler2* comm2 = new (b) TCommunications_Handler2;
    TCommunications_Handler* old_comm = (TCommunications_Handler*)comm2;
    void* cl = malloc(0x574);
    memset(cl, 0, 0x574);
    old_comm->mCommandLog = TCommCommandLog__TCommCommandLog((TCommCommandLog*)cl);
    comm2->set_command_log(old_comm->mCommandLog);
    comm2->set_window_handle(WinHandle);

    void* sp = malloc(0x1168);
    memset(sp, 0, 0x1168);
    old_comm->Speed = (RGE_Communications_Speed*)sp;

    return comm2;
}

__declspec(naked) void TCommunications_Handler2_init()
{
    __asm
    {
        mov     eax, [esp]
        mov     [esp], ecx
        push    eax
        jmp     TCommunications_Handler2_init2
    }
}

class TCommunications_Services2
{
private:
    struct Service
    {
        std::string name;
        GUID guid;
    };
    std::vector<Service> services;
public:
    TCommunications_Services2()
    {
        services.clear();
        Service s;
        s.name = "Test Service";
        memset(&s.guid, 0, sizeof(GUID));
        s.guid.Data4[7] = 1;
        services.push_back(s);
    }
    ~TCommunications_Services2()
    {
        services.clear();
    }

    int __thiscall GetServiceCount()
    {
        return services.size();
    }

    const char* __thiscall GetServiceName(int id)
    {
        return services[id].name.c_str();
    }

    _GUID* __thiscall GetServiceGUID(_GUID* result, int id)
    {
        *result = services[id].guid;
        return result;
    }

    int __thiscall GetServiceActive(int id)
    {
        return 1;
    }
};

TCommunications_Services2* __fastcall TCommunications_Services2__init(TCommunications_Services2* b)
{
    memset(b, 0, 0x2584);
    TCommunications_Services2* services2 = new (b) TCommunications_Services2;
    return services2;
}

__declspec(naked) void TCommunications_Services2__GetServiceCount()
{
    __asm jmp TCommunications_Services2::GetServiceCount
}

__declspec(naked) void TCommunications_Services2__GetServiceName()
{
    __asm jmp TCommunications_Services2::GetServiceName
}

__declspec(naked) void TCommunications_Services2__GetServiceGUID()
{
    __asm jmp TCommunications_Services2::GetServiceGUID
}

__declspec(naked) void TCommunications_Services2__GetServiceActive()
{
    __asm jmp TCommunications_Services2::GetServiceActive
}

int __stdcall RGE_Communications_Speed2__GetPlayerLatencyForDisplay(int PlayerNo)
{
    return 0;
}

int __stdcall mp_start_game_check(TRIBE_Game* game)
{
    TRIBE_Game__setMapType(game, 0xE);
    RGE_Base_Game__setMultiplayerGame((RGE_Base_Game*)game, 1);

    TCommunications_Handler__SetPlayerName(*comm, 1, "New Comm Player");
    TCommunications_Handler__SetPlayerName(*comm, 2, "New Comm Computer Player");

    TRIBE_Game__setPlayerColor(game, 0, 3);
    TRIBE_Game__setPlayerColor(game, 1, 4);

    TCommunications_Handler__SetPlayerHumanity(*comm, 1, 2);
    TCommunications_Handler__SetPlayerHumanity(*comm, 2, 4);

    RGE_Game_Info__set_multi_player_game_options((*base_game)->player_game_info);

    //game->tribe_game_options.playerColorValue[2] = 2;

    TRIBE_Game__start_game(game, 0);
    //TRIBE_Game__close_game_screens(game, 0);
    //TRIBE_Game__create_game_screen(game);
    //TCommunications_Handler__NotifyWindow(game->comm_handler, 0x17CB);



    return 1;
}

__declspec(naked) void start_game_hook() //005E566D
{
    __asm
    {
        push    ebp
        call    mp_start_game_check
        test    eax, eax
        jnz     finish_start_game
        mov     ecx, [ebp + 24h]
        cmp     [ecx + 890h], ebx

        mov     eax, 005E5676h
        jmp     eax

finish_start_game:
        mov     eax, 005E5693h
        jmp     eax
    }
}

void setTCommunications_Handler2hooks()
{
    setHook((void*)0x005E566D, start_game_hook);

    setHook(*RGE_Communications_Speed__GetPlayerLatencyForDisplay, RGE_Communications_Speed2__GetPlayerLatencyForDisplay);

    setHook(*TCommunications_Services__TCommunications_Services, TCommunications_Services2__init);
    setHook(*TCommunications_Services__GetServiceCount, TCommunications_Services2__GetServiceCount);
    setHook(*TCommunications_Services__GetServiceName, TCommunications_Services2__GetServiceName);
    setHook(*TCommunications_Services__GetServiceGUID, TCommunications_Services2__GetServiceGUID);
    setHook(*TCommunications_Services__GetServiceActive, TCommunications_Services2__GetServiceActive);


    setHook(*TCommunications_Handler__GameOver, TCommunications_Handler2__GameOver);
    setHook(*TCommunications_Handler__WaitingOnNamedInfo, TCommunications_Handler2__WaitingOnNamedInfo);
    setHook(*TCommunications_Handler__StopIfSyncFail, TCommunications_Handler2__StopIfSyncFail);
    setHook(*TCommunications_Handler__ShowSyncChatMsgs, TCommunications_Handler2__ShowSyncChatMsgs);
    setHook(*TCommunications_Handler__SetSpeedControl, TCommunications_Handler2__SetSpeedControl);
    setHook(*TCommunications_Handler__TCommunications_Handler, TCommunications_Handler2_init);
    setHook(*TCommunications_Handler__destructor, nullsub_0_arg);
    setHook(*TCommunications_Handler__DestroyMultiplayerGame, TCommunications_Handler2__DestroyMultiplayerGame);
    setHook(*TCommunications_Handler__get_command, TCommunications_Handler2__get_command);
    setHook(*TCommunications_Handler__new_command, TCommunications_Handler2__new_command);
    setHook(*TCommunications_Handler__LastWorldRandom, TCommunications_Handler2__LastWorldRandom);
    setHook(*TCommunications_Handler__SaveInitialGameHeader, TCommunications_Handler2__SaveInitialGameHeader);
    setHook(*TCommunications_Handler__DoCycle, TCommunications_Handler2__DoCycle);
    setHook(*TCommunications_Handler__incrementVoteCount, TCommunications_Handler2__incrementVoteCount);
    setHook(*TCommunications_Handler__getDefaultMultiplayerSaveFilename, TCommunications_Handler2__getDefaultMultiplayerSaveFilename);
    setHook(*TCommunications_Handler__getCommLogInfo, TCommunications_Handler2__getCommLogInfo);
    setHook(*TCommunications_Handler__ReceiveGameMessages, TCommunications_Handler2__ReceiveGameMessages);
    setHook(*TCommunications_Handler__ShutdownGameMessages, TCommunications_Handler2__ShutdownGameMessages);
    setHook(*TCommunications_Handler__verifyNametable, TCommunications_Handler2__verifyNametable);
    setHook(*TCommunications_Handler__NotifyWindow, TCommunications_Handler2__NotifyWindow);
    setHook(*TCommunications_Handler__NotifyWindowParam, TCommunications_Handler2__NotifyWindowParam);
    setHook(*TCommunications_Handler__GetPlayerStopTurn, TCommunications_Handler2__GetPlayerStopTurn);
    setHook(*TCommunications_Handler__SendSharedData, TCommunications_Handler2__SendSharedData);
    setHook(*TCommunications_Handler__LogCommentsOnHost, TCommunications_Handler2__LogCommentsOnHost);
    setHook(*TCommunications_Handler__SendGroupChatMsg, TCommunications_Handler2__SendGroupChatMsg);
    setHook(*TCommunications_Handler__SendChatMsgAll, TCommunications_Handler2__SendChatMsgAll);
    setHook(*TCommunications_Handler__TXChat, TCommunications_Handler2__TXChat);
    setHook(*TCommunications_Handler__SendSpeedChange, TCommunications_Handler2__SendSpeedChange);
    setHook(*TCommunications_Handler__ExitPregame, TCommunications_Handler2__ExitPregame);
    setHook(*TCommunications_Handler__SendIResignMsg, TCommunications_Handler2__SendIResignMsg);
    setHook(*TCommunications_Handler__AllPlayersAcknowledged, TCommunications_Handler2__AllPlayersAcknowledged);
    setHook(*TCommunications_Handler__SendChecksumMessage, TCommunications_Handler2__SendChecksumMessage);
    setHook(*TCommunications_Handler__CountWaitingMessages, TCommunications_Handler2__CountWaitingMessages);
    setHook(*TCommunications_Handler__GetCommunicationsStatus, TCommunications_Handler2__GetCommunicationsStatus);
    setHook(*TCommunications_Handler__UnlinkToLevel, TCommunications_Handler2__UnlinkToLevel);
    setHook(*TCommunications_Handler__AnalyzeCommunicationsStatus, TCommunications_Handler2__AnalyzeCommunicationsStatus);
    setHook(*TCommunications_Handler__IsPaused, TCommunications_Handler2__IsPaused);
    setHook(*TCommunications_Handler__TogglePauseGame, TCommunications_Handler2__TogglePauseGame);
    setHook(*TCommunications_Handler__GetRandomSeed, TCommunications_Handler2__GetRandomSeed);
    setHook(*TCommunications_Handler__WhoAmI, TCommunications_Handler2__WhoAmI);
    setHook(*TCommunications_Handler__IsHost, TCommunications_Handler2__IsHost);
    setHook(*TCommunications_Handler__GetPlayerHumanity, TCommunications_Handler2__GetPlayerHumanity);
    setHook(*TCommunications_Handler__SetPlayerHumanity, TCommunications_Handler2__SetPlayerHumanity);
    setHook(*TCommunications_Handler__IsPlayerHuman, TCommunications_Handler2__IsPlayerHuman);
    setHook(*TCommunications_Handler__IsPlayerComputer, TCommunications_Handler2__IsPlayerComputer);
    setHook(*TCommunications_Handler__GetPlayerLatency, TCommunications_Handler2__GetPlayerLatency);
    setHook(*TCommunications_Handler__GetPlayerName, TCommunications_Handler2__GetPlayerName);
    setHook(*TCommunications_Handler__IsLobbyLaunched, TCommunications_Handler2__IsLobbyLaunched);
    setHook(*TCommunications_Handler__LaunchLobbyGame, TCommunications_Handler2__LaunchLobbyGame);
    setHook(*TCommunications_Handler__SetRestoringGameTitle, TCommunications_Handler2__SetRestoringGameTitle);
    //setHook(*TCommunications_Handler__ParseRestoringGameTitle, TCommunications_Handler2__ParseRestoringGameTitle);
    setHook(*TCommunications_Handler__SetGameTitle, TCommunications_Handler2__SetGameTitle);
    setHook(*TCommunications_Handler__SetMyPlayerName, TCommunications_Handler2__SetMyPlayerName);
    setHook(*TCommunications_Handler__SetPlayerName, TCommunications_Handler2__SetPlayerName);
    setHook(*TCommunications_Handler__SetWindowHandle, TCommunications_Handler2__SetWindowHandle);
    setHook(*TCommunications_Handler__CreateMultiplayerGame, TCommunications_Handler2__CreateMultiplayerGame);
    setHook(*TCommunications_Handler__InitJoinGame, TCommunications_Handler2__InitJoinGame);
    setHook(*TCommunications_Handler__JoinMultiplayerGame, TCommunications_Handler2__JoinMultiplayerGame);
    setHook(*TCommunications_Handler__ClearPlayersAcknowledged, TCommunications_Handler2__ClearPlayersAcknowledged);
    setHook(*TCommunications_Handler__GetDPInterface, nullsub_0_arg);            //!!!!!!!!!!!!!!!!
    setHook(*TCommunications_Handler__SetServiceGUID, TCommunications_Handler2__SetServiceGUID);
    setHook(*TCommunications_Handler__UpdatePlayers, TCommunications_Handler2__UpdatePlayers);
    setHook(*TCommunications_Handler__SetMyReadiness, TCommunications_Handler2__SetMyReadiness);
    setHook(*TCommunications_Handler__GetClientReadiness, TCommunications_Handler2__GetClientReadiness);
    setHook(*TCommunications_Handler__IsPlayerReady, TCommunications_Handler2__IsPlayerReady);
    setHook(*TCommunications_Handler__ComputerPlayerGameStart, TCommunications_Handler2__ComputerPlayerGameStart);
    setHook(*TCommunications_Handler__GameStarted, TCommunications_Handler2__GameStarted);
    setHook(*TCommunications_Handler__MultiplayerGameStart, TCommunications_Handler2__MultiplayerGameStart);
    setHook(*TCommunications_Handler__AllPlayersReady, TCommunications_Handler2__AllPlayersReady);
    setHook(*TCommunications_Handler__Kick, TCommunications_Handler2__Kick);
    setHook(*TCommunications_Handler__SetMyGameOptions, TCommunications_Handler2__SetMyGameOptions);
    setHook(*TCommunications_Handler__GetMyGameOptions, TCommunications_Handler2__GetMyGameOptions);
    setHook(*TCommunications_Handler__GetHostPlayerNumber, TCommunications_Handler2__GetHostPlayerNumber);
    setHook(*TCommunications_Handler__SendZoneMessage, TCommunications_Handler2__SendZoneMessage);
    setHook(*TCommunications_Handler__SetSpeedV1, TCommunications_Handler2__SetSpeedV1);
    setHook(*TCommunications_Handler__IsPlayerOutOfSync, TCommunications_Handler2__IsPlayerOutOfSync);
    setHook(*TCommunications_Handler__clearOldReceivedFile, TCommunications_Handler2__clearOldReceivedFile);
    setHook(*TCommunications_Handler__StopAllXfers, TCommunications_Handler2__StopAllXfers);
    setHook(*TCommunications_Handler__ReceivingFileCheck, TCommunications_Handler2__ReceivingFileCheck);
    setHook(*TCommunications_Handler__SendingFileCheck, TCommunications_Handler2__SendingFileCheck);
    setHook(*TCommunications_Handler__GetPlayerPercentageComplete, TCommunications_Handler2__GetPlayerPercentageComplete);
    setHook(*TCommunications_Handler__sendVoteMessage, TCommunications_Handler2__sendVoteMessage);
    setHook(*TCommunications_Handler__clearVoteDialogPtr, TCommunications_Handler2__clearVoteDialogPtr);
    setHook(*TCommunications_Handler__activateVotingDialogForSaveAndExit, TCommunications_Handler2__activateVotingDialogForSaveAndExit);
    setHook(*TCommunications_Handler__IsMultiplayer, TCommunications_Handler2__IsMultiplayer);
    setHook(*TCommunications_Handler__GetSavedMyPosition, TCommunications_Handler2__GetSavedMyPosition);
    setHook(*TCommunications_Handler__GetSavedPlayerPosition, TCommunications_Handler2__GetSavedPlayerPosition);
    setHook(*TCommunications_Handler__setRestoringGame, TCommunications_Handler2__setRestoringGame);
    setHook(*TCommunications_Handler__GetRestoringGame, TCommunications_Handler2__GetRestoringGame);
    setHook(*TCommunications_Handler__setCurrentHeader, TCommunications_Handler2__setCurrentHeader);
    setHook(*TCommunications_Handler__OutOfSyncSave, TCommunications_Handler2__OutOfSyncSave);
    setHook(*TCommunications_Handler__SaveMultiplayerGameHeader, TCommunications_Handler2__SaveMultiplayerGameHeader);
    setHook(*TCommunications_Handler__getPlayerHeaderIndex, TCommunications_Handler2__getPlayerHeaderIndex);
    setHook(*TCommunications_Handler__restoreChatGroups, TCommunications_Handler2__restoreChatGroups);
    setHook(*TCommunications_Handler__SetMeToRecvSavegame, TCommunications_Handler2__SetMeToRecvSavegame);
    setHook(*TCommunications_Handler__getGUIDandChecksum, TCommunications_Handler2__getGUIDandChecksum);
    setHook(*TCommunications_Handler__ReadyToReceive, TCommunications_Handler2__ReadyToReceive);
    setHook(*TCommunications_Handler__SetSaveChecksums, TCommunications_Handler2__SetSaveChecksums);
    setHook(*TCommunications_Handler__NotifyStartSaving, TCommunications_Handler2__NotifyStartSaving);
    setHook(*TCommunications_Handler__getNeedToRecvFile, TCommunications_Handler2__getNeedToRecvFile);
    setHook(*TCommunications_Handler__SetNeedScenario, TCommunications_Handler2__SetNeedScenario);
    setHook(*TCommunications_Handler__GetNeedScenario, TCommunications_Handler2__GetNeedScenario);
    setHook(*TCommunications_Handler__SendAroundScenario, TCommunications_Handler2__SendAroundScenario);
    setHook((void*)0x004417A0, nullsub_0_arg);  //TCommunications_Handler::StopSendAroundScenario
    setHook(*TCommunications_Handler__sendPregameMessage, TCommunications_Handler2__sendPregameMessage);
    setHook(*TCommunications_Handler__pollReceieve, TCommunications_Handler2__pollReceieve);
    setHook((void*)0x004421E0, nullsub_0_arg);  //TCommunications_Handler::requestRMS
    setHook((void*)0x00442210, nullsub_0_arg);  //TCommunications_Handler::stopRequestingRMS
    setHook(*TCommunications_Handler__PregameCycle, TCommunications_Handler2__PregameCycle);
    setHook(*TCommunications_Handler__getGameOutcomeForPlayer, TCommunications_Handler2__getGameOutcomeForPlayer);
    setHook(*TCommunications_Handler__setGameOutcomeForPlayer, TCommunications_Handler2__setGameOutcomeForPlayer);
    setHook(*TCommunications_Handler__SetSavedPos, TCommunications_Handler2__SetSavedPos);

    //setHook(*TCommunications_Handler__GameOver, nullsub_0_arg);
}
