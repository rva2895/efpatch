#pragma once
#ifndef __IUSERPATCH_H_
#define __IUSERPATCH_H_

#ifndef BUILD_VERSION
#define BUILD_VERSION( l1,l2,l3,l4 )  ( ( l1 << 24 ) | ( l2 << 16 ) | ( l3 << 8 ) | ( l4 << 0 ) )
#endif

#define EXE_PATCH_VERSION "3.3"
#define USERPATCH_VERSION "EXE Patch v3.3.0 (" EXE_PATCH_VERSION ") " __DATE__ " " __TIME__

struct UserPatchConfig_t
{
    bool bUPResolutionEnhancements;    // Voobly-style widescreen will be used if
                                    // this is false

    bool bDisableXMLLoading;        // This is a "regular" AoC game. Disable
                                    // xml loader for safety incase there are
                                    // stray .xml files.

    bool bForceXMLVooblyModDir;        // Use XML from a voobly mod directory
    const char *VooblyModDirPath;    // path to use
    DWORD clientVersion;            // Version of client
    bool bExtendedHotkeys;            // Enable/disable UserPatch Hotkeys
};

//-----------------------------------------------------------------------------
// Purpose: Voobly interface usable by UserPatch library
//-----------------------------------------------------------------------------
class IVoobly
{
public:
    // Memory patching
    virtual void Write(DWORD dest, const void *pSrc, int len) = 0;
    virtual void Write(DWORD dest, DWORD data) = 0;
    virtual void Write(DWORD dest, const char *pHexString) = 0;

    // Logging
    virtual void ChatMessage(const char *from, const char *msgfmt, ...) = 0;
    virtual void Log(const char *fmt, ...) = 0;

    // Write a long jump at dest to jump to pTargetFunc
    virtual void WriteJump(DWORD dest, void *pTargetFunc) = 0;

    // Stream this spectator data to spectators
    virtual void SpectatorData(void *pData, int len) = 0;

    // End of spectator data
    virtual void SpectatorDone() = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Userpatch interfaces (simple for now ;))
//-----------------------------------------------------------------------------
class IUserPatch
{
public:
    // Init userpatch. Returns true on success.
    virtual bool Init(struct UserPatchConfig_t &config) = 0;

    // Called when the users sends a chat messages. Return true to prevent
    // the message from being sent to game chat system.
    virtual bool OnChatMessage(const char *text) = 0;
};

#ifdef INTEGRATED_USERPATCH
void GetUPInterface(IUserPatch **ppUserPatch, IVoobly *pVoobly);
#endif

#endif
