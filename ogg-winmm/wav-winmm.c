/*
 * Copyright (c) 2012 Toni Spets <toni.spets@iki.fi>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <windows.h>
#include <stdio.h>
#include "player.h"

#define MAGIC_DEVICEID 0xBEEF
#define MAX_TRACKS 99

struct track_info
{
    char path[MAX_PATH];    // full path to ogg
    unsigned int length;    // seconds
    unsigned int position;  // seconds
};

static struct track_info tracks[MAX_TRACKS];

struct play_info
{
    int first;
    int last;
};

#ifdef _DEBUG
    #define dprintf(...) if (fh) { fprintf(fh, __VA_ARGS__); fflush(NULL); }
    FILE *fh = NULL;
#else
    #define dprintf(...)
#endif

int playing = 0;
HANDLE player = NULL;
int firstTrack = -1;
int lastTrack = 0;
int numTracks = 0;
char music_path[2048];
int time_format = MCI_FORMAT_TMSF;
CRITICAL_SECTION cs;

int player_main(struct play_info *info)
{
    int first = info->first;
    int last = info->last;
    int current = first;

    playing = 1;

    while (current <= last && playing)
    {
        dprintf("Next track: %s\r\n", tracks[current].path);
        playing = plr_play(tracks[current].path);

        while (1)
        {
            if (plr_pump() == 0)
                break;

            if (!playing)
            {
                return 0;
            }
        }

        current++;
    }

    playing = 0;
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
#ifdef _DEBUG
        fh = fopen("winmm.txt", "w");
#endif
        GetModuleFileName(hinstDLL, music_path, sizeof music_path);

        memset(tracks, 0, sizeof tracks);

        InitializeCriticalSection(&cs);

        char *last = strrchr(music_path, '\\');
        if (last)
        {
            *last = '\0';
        }
        strncat(music_path, "\\MUSIC", sizeof music_path - 1);

        dprintf("ogg-winmm music directory is %s\r\n", music_path);
        dprintf("ogg-winmm searching tracks...\r\n");

        unsigned int position = 0;

        for (int i = 0; i < MAX_TRACKS; i++)
        {
            snprintf(tracks[i].path, sizeof tracks[i].path, "%s\\Track%02d.ogg", music_path, i);
            tracks[i].length = plr_length(tracks[i].path);
            tracks[i].position = position;

            if (tracks[i].length < 4)
            {
                tracks[i].path[0] = '\0';
                position += 4; // missing tracks are 4 second data tracks for us
            }
            else
            {
                if (firstTrack == -1)
                {
                    firstTrack = i;
                }

                dprintf("Track %02d: %02d:%02d @ %d seconds\r\n", i, tracks[i].length / 60, tracks[i].length % 60, tracks[i].position);
                numTracks++;
                lastTrack = i;
                position += tracks[i].length;
            }
        }

        dprintf("Emulating total of %d CD tracks.\r\n\r\n", numTracks);
    }

#ifdef _DEBUG
    if (fdwReason == DLL_PROCESS_DETACH)
    {
        if (fh)
        {
            fclose(fh);
            fh = NULL;
        }
    }
#endif

    return TRUE;
}

MCIERROR WINAPI fake_mciSendCommandA(MCIDEVICEID IDDevice, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam)
{
    char cmdbuf[1024];

    dprintf("mciSendCommandA(IDDevice=%p, uMsg=%p, fdwCommand=%p, dwParam=%p)\r\n", IDDevice, uMsg, fdwCommand, dwParam);

    if (fdwCommand & MCI_NOTIFY)
    {
        dprintf("  MCI_NOTIFY\r\n");
    }

    if (fdwCommand & MCI_WAIT)
    {
        dprintf("  MCI_WAIT\r\n");
    }

    if (uMsg == MCI_OPEN)
    {
        LPMCI_OPEN_PARMS parms = (LPVOID)dwParam;

        dprintf("  MCI_OPEN\r\n");

        if (fdwCommand & MCI_OPEN_ALIAS)
        {
            dprintf("    MCI_OPEN_ALIAS\r\n");
        }

        if (fdwCommand & MCI_OPEN_SHAREABLE)
        {
            dprintf("    MCI_OPEN_SHAREABLE\r\n");
        }

        if (fdwCommand & MCI_OPEN_TYPE_ID)
        {
            dprintf("    MCI_OPEN_TYPE_ID\r\n");

            if (LOWORD(parms->lpstrDeviceType) == MCI_DEVTYPE_CD_AUDIO)
            {
                dprintf("  Returning magic device id for MCI_DEVTYPE_CD_AUDIO\r\n");
                parms->wDeviceID = MAGIC_DEVICEID;
                return 0;
            }
        }

        if (fdwCommand & MCI_OPEN_TYPE && !(fdwCommand & MCI_OPEN_TYPE_ID))
        {
            dprintf("    MCI_OPEN_TYPE\r\n");
            dprintf("        -> %s\r\n", parms->lpstrDeviceType);

            if (strcmp(parms->lpstrDeviceType, "cdaudio") == 0)
            {
                dprintf("  Returning magic device id for MCI_DEVTYPE_CD_AUDIO\r\n");
                parms->wDeviceID = MAGIC_DEVICEID;
                return 0;
            }
        }

    }

    if (IDDevice == MAGIC_DEVICEID || IDDevice == 0 || IDDevice == 0xFFFFFFFF)
    {
        if (uMsg == MCI_SET)
        {
            LPMCI_SET_PARMS parms = (LPVOID)dwParam;

            dprintf("  MCI_SET\r\n");

            if (fdwCommand & MCI_SET_TIME_FORMAT)
            {
                dprintf("    MCI_SET_TIME_FORMAT\r\n");

                time_format = parms->dwTimeFormat;

                if (parms->dwTimeFormat == MCI_FORMAT_BYTES)
                {
                    dprintf("      MCI_FORMAT_BYTES\r\n");
                }

                if (parms->dwTimeFormat == MCI_FORMAT_FRAMES)
                {
                    dprintf("      MCI_FORMAT_FRAMES\r\n");
                }

                if (parms->dwTimeFormat == MCI_FORMAT_HMS)
                {
                    dprintf("      MCI_FORMAT_HMS\r\n");
                }

                if (parms->dwTimeFormat == MCI_FORMAT_MILLISECONDS)
                {
                    dprintf("      MCI_FORMAT_MILLISECONDS\r\n");
                }

                if (parms->dwTimeFormat == MCI_FORMAT_MSF)
                {
                    dprintf("      MCI_FORMAT_MSF\r\n");
                }

                if (parms->dwTimeFormat == MCI_FORMAT_SAMPLES)
                {
                    dprintf("      MCI_FORMAT_SAMPLES\r\n");
                }

                if (parms->dwTimeFormat == MCI_FORMAT_TMSF)
                {
                    dprintf("      MCI_FORMAT_TMSF\r\n");
                }
            }
        }

        if (uMsg == MCI_CLOSE)
        {
            dprintf("  MCI_CLOSE\r\n");

            if (player)
            {
                TerminateThread(player, 0);
            }

            playing = 0;
            player = NULL;
        }

        if (uMsg == MCI_PLAY)
        {
            LPMCI_PLAY_PARMS parms = (LPVOID)dwParam;
            static struct play_info info = { -1, -1 };

            dprintf("  MCI_PLAY\r\n");

            if (fdwCommand & MCI_FROM)
            {
                dprintf("    dwFrom: %d\r\n", parms->dwFrom);

                // FIXME: rounding to nearest track
                if (time_format == MCI_FORMAT_TMSF)
                {
                    info.first = MCI_TMSF_TRACK(parms->dwFrom);

                    dprintf("      TRACK  %d\n", MCI_TMSF_TRACK(parms->dwFrom));
                    dprintf("      MINUTE %d\n", MCI_TMSF_MINUTE(parms->dwFrom));
                    dprintf("      SECOND %d\n", MCI_TMSF_SECOND(parms->dwFrom));
                    dprintf("      FRAME  %d\n", MCI_TMSF_FRAME(parms->dwFrom));
                }
                else if (time_format == MCI_FORMAT_MILLISECONDS)
                {
                    info.first = 0;

                    for (int i = 0; i < MAX_TRACKS; i++)
                    {
                        // FIXME: take closest instead of absolute
                        if (tracks[i].position == parms->dwFrom / 1000)
                        {
                            info.first = i;
                        }
                    }

                    dprintf("      mapped milliseconds to %d\n", info.first);
                }
                else
                {
                    // FIXME: not really
                    info.first = parms->dwFrom;
                }

                if (info.first < firstTrack)
                    info.first = firstTrack;

                if (info.first > lastTrack)
                    info.first = lastTrack;

                info.last = info.first;
            }

            if (fdwCommand & MCI_TO)
            {
                dprintf("    dwTo:   %d\r\n", parms->dwTo);

                if (time_format == MCI_FORMAT_TMSF)
                {
                    info.last = MCI_TMSF_TRACK(parms->dwTo);

                    dprintf("      TRACK  %d\n", MCI_TMSF_TRACK(parms->dwTo));
                    dprintf("      MINUTE %d\n", MCI_TMSF_MINUTE(parms->dwTo));
                    dprintf("      SECOND %d\n", MCI_TMSF_SECOND(parms->dwTo));
                    dprintf("      FRAME  %d\n", MCI_TMSF_FRAME(parms->dwTo));
                }
                else if (time_format == MCI_FORMAT_MILLISECONDS)
                {
                    info.last = info.first;

                    for (int i = info.first; i < MAX_TRACKS; i ++)
                    {
                        // FIXME: use better matching
                        if (tracks[i].position + tracks[i].length > parms->dwFrom / 1000)
                        {
                            info.last = i;
                            break;
                        }
                    }

                    dprintf("      mapped milliseconds to %d\n", info.last);
                }
                else
                    info.last = parms->dwTo;

                if (info.last < info.first)
                    info.last = info.first;

                if (info.last > lastTrack)
                    info.last = lastTrack;
            }

            if (info.first && (fdwCommand & MCI_FROM))
            {
                if (player)
                {
                    TerminateThread(player, 0);
                }

                playing = 0;
                player = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)player_main, (void *)&info, 0, NULL);
            }
        }

        if (uMsg == MCI_STOP)
        {
            dprintf("  MCI_STOP\r\n");
            playing = 0;
        }

        if (uMsg == MCI_STATUS)
        {
            LPMCI_STATUS_PARMS parms = (LPVOID)dwParam;

            dprintf("  MCI_STATUS\r\n");

            parms->dwReturn = 0;

            if (fdwCommand & MCI_TRACK)
            {
                dprintf("    MCI_TRACK\r\n");
                dprintf("      dwTrack = %d\r\n", parms->dwTrack);
            }

            if (fdwCommand & MCI_STATUS_ITEM)
            {
                dprintf("    MCI_STATUS_ITEM\r\n");

                if (parms->dwItem == MCI_STATUS_CURRENT_TRACK)
                {
                    dprintf("      MCI_STATUS_CURRENT_TRACK\r\n");
                }

                if (parms->dwItem == MCI_STATUS_LENGTH)
                {
                    dprintf("      MCI_STATUS_LENGTH\r\n");

                    int seconds = tracks[parms->dwTrack].length;

                    if (seconds)
                    {
                        if (time_format == MCI_FORMAT_MILLISECONDS)
                        {
                            parms->dwReturn = seconds * 1000;
                        }
                        else
                        {
                            parms->dwReturn = MCI_MAKE_MSF(seconds / 60, seconds % 60, 0);
                        }
                    }
                }

                if (parms->dwItem == MCI_CDA_STATUS_TYPE_TRACK)
                {
                    dprintf("      MCI_CDA_STATUS_TYPE_TRACK\r\n");
                }

                if (parms->dwItem == MCI_STATUS_MEDIA_PRESENT)
                {
                    dprintf("      MCI_STATUS_MEDIA_PRESENT\r\n");
                    parms->dwReturn = lastTrack > 0;
                }

                if (parms->dwItem == MCI_STATUS_NUMBER_OF_TRACKS)
                {
                    dprintf("      MCI_STATUS_NUMBER_OF_TRACKS\r\n");
                    parms->dwReturn = numTracks + 1;
                }

                if (parms->dwItem == MCI_STATUS_POSITION)
                {
                    dprintf("      MCI_STATUS_POSITION\r\n");

                    if (fdwCommand & MCI_TRACK)
                    {
                        // FIXME: implying milliseconds
                        //parms->dwReturn = tracks[parms->dwTrack].position * 1000;
                    }
					parms->dwReturn = 3;
                }

                if (parms->dwItem == MCI_STATUS_MODE)
                {
                    dprintf("      MCI_STATUS_MODE\r\n");
                    dprintf("        we are %s\r\n", playing ? "playing" : "NOT playing");

                    parms->dwReturn = playing ? MCI_MODE_PLAY : MCI_MODE_STOP;
                }

                if (parms->dwItem == MCI_STATUS_READY)
                {
                    dprintf("      MCI_STATUS_READY\r\n");
                }

                if (parms->dwItem == MCI_STATUS_TIME_FORMAT)
                {
                    dprintf("      MCI_STATUS_TIME_FORMAT\r\n");
                }

                if (parms->dwItem == MCI_STATUS_START)
                {
                    dprintf("      MCI_STATUS_START\r\n");
                }
            }

            dprintf("  dwReturn %d\n", parms->dwReturn);

        }

        return 0;
    }

    /* fallback */
    return MCIERR_UNRECOGNIZED_COMMAND;
}

// this is really fugly but for christ sake why did anyone use it?!
MCIERROR WINAPI fake_mciSendStringA(LPCTSTR cmd, LPTSTR ret, UINT cchReturn, HANDLE hwndCallback)
{
    printf("MCI: %s\n", cmd);

    if (strstr(cmd, "sysinfo"))
    {
        strcpy(ret, "cd");
        return 0;
    }

    if (strstr(cmd, "stop cd"))
    {
        fake_mciSendCommandA(MAGIC_DEVICEID, MCI_STOP, 0, NULL);
        return 0;
    }

    int from = -1, to = -1;
    if (sscanf(cmd, "play cd from %d to %d", &from, &to) == 2)
    {
        static MCI_PLAY_PARMS parms;
        parms.dwFrom = from;
        parms.dwTo = to;
        fake_mciSendCommandA(MAGIC_DEVICEID, MCI_PLAY, MCI_FROM|MCI_TO, &parms);
        return 0;
    }

    return 0;
}

UINT WINAPI fake_auxGetNumDevs()
{
    dprintf("fake_auxGetNumDevs()\r\n");
    return 1;
}

MMRESULT WINAPI fake_auxGetDevCapsA(UINT_PTR uDeviceID, LPAUXCAPS lpCaps, UINT cbCaps)
{
    dprintf("fake_auxGetDevCapsA(uDeviceID=%08X, lpCaps=%p, cbCaps=%08X\n", uDeviceID, lpCaps, cbCaps);

    lpCaps->wMid = 2 /*MM_CREATIVE*/;
    lpCaps->wPid = 401 /*MM_CREATIVE_AUX_CD*/;
    lpCaps->vDriverVersion = 1;
    strcpy(lpCaps->szPname, "ogg-winmm virtual CD");
    lpCaps->wTechnology = AUXCAPS_CDAUDIO;
    lpCaps->dwSupport = AUXCAPS_VOLUME;

    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI fake_auxGetVolume(UINT uDeviceID, LPDWORD lpdwVolume)
{
    dprintf("fake_auxGetVolume(uDeviceId=%08X, lpdwVolume=%p)\r\n", uDeviceID, lpdwVolume);
    *lpdwVolume = 0xFFFFFFFF; // 0x00000000;
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI fake_auxSetVolume(UINT uDeviceID, DWORD dwVolume)
{
    static DWORD oldVolume = -1;
    char cmdbuf[256];

    dprintf("fake_auxSetVolume(uDeviceId=%08X, dwVolume=%08X)\r\n", uDeviceID, dwVolume);

    if (dwVolume == oldVolume)
    {
        return MMSYSERR_NOERROR;
    }

    oldVolume = dwVolume;

    unsigned short left = LOWORD(dwVolume);
    unsigned short right = HIWORD(dwVolume);

    dprintf("    left : %ud (%04X)\n", left, left);
    dprintf("    right: %ud (%04X)\n", right, right);

    plr_volume((left / 65535.0f) * 100);

    return MMSYSERR_NOERROR;
}
