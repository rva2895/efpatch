#pragma once

enum GAME_STATUS
{
    GAME_STATUS_UNKNOWN = 0,
    GAME_STATUS_PENDING = 1,
    GAME_STATUS_IN_PROGRESS = 2,
    GAME_STATUS_ENDED = 3
};

struct proto_header
{
    char name[8];
    uint32_t proto_version;
    uint32_t reserved;
};

struct message_header
{
    uint32_t message_type;
    uint32_t message_size;
};

enum CLIENT_TYPE
{
    CLIENT_TYPE_GAME = 0,
    CLIENT_TYPE_SPECTATOR = 1
};

enum MESSAGE_TYPE
{
    //common messages
    MESSAGE_TYPE_RESERVED = 0,
    MESSAGE_TYPE_REC_STREAM = 1,
    MESSAGE_TYPE_SET_CLIENT_TYPE = 2,
    MESSAGE_TYPE_GAME_INFO = 3,
    MESSAGE_TYPE_GAME_STATUS = 4,
    //game client messages
    // - none
    //spectator client messages
    MESSAGE_TYPE_GAME_LIST_REQUEST = 100,
    MESSAGE_TYPE_GAME_LIST_RESPONSE = 101,
    MESSAGE_TYPE_REC_STREAM_REQUEST = 102,
    MESSAGE_TYPE_GAME_INFO_REQUEST = 103,
    MESSAGE_TYPE_GAME_STATUS_REQUEST = 104
};

void setSpectateHooks();
