#include "stdafx.h"

#include <time.h>
#include "log.h"

#include <string>

//#ifdef _CC_COMPATIBLE
//#define CHEATDLL_NOLOG
//#endif

FILE* log_file = NULL;
bool loggingEnabled;

char* lastLogs[12];
int logged;

std::wstring logFileName;

void deleteOldLogs()
{
    char filename[MAX_PATH+5];

    //MessageBox (0, "aa", "sdsd", 0);

    time_t rawtime_current = time(NULL);
    time_t rawtime_old;

    tm time_old;
    memset(&time_old, 0, sizeof(tm));

    int nFiles = 0;
#ifdef _DEBUG
    log("Looking for old logs...");
#endif
    WIN32_FIND_DATA fd;
    HANDLE hFile = FindFirstFile("logs\\efpatch*.log", &fd);
    //int xxxx = GetLastError ();
    if (hFile == INVALID_HANDLE_VALUE)
    {
        log("FindFirstFile returned INVALID_HANDLE_VALUE");
    }
    else
    {
        do
        {
            sscanf(fd.cFileName, "efpatch_%d-%d-%d_%d-%d-%d.log",
                &time_old.tm_year, &time_old.tm_mon, &time_old.tm_mday,
                &time_old.tm_hour, &time_old.tm_min, &time_old.tm_sec);
            time_old.tm_year -= 1900;
            time_old.tm_mon -= 1;

            rawtime_old = mktime(&time_old);

            if (difftime(rawtime_current, rawtime_old) > 604800) //1 week
            {
                sprintf(filename, "logs\\%s", fd.cFileName);
                if (!DeleteFile(filename))
                {
                    log("ERROR: Cannot delete %s, error %d", fd.cFileName, GetLastError());
                }
                else
                {
#ifdef _DEBUG
                    log("Deleted %s", fd.cFileName);
#endif
                    nFiles++;
                }
            }
        } while (FindNextFile(hFile, &fd));
        int err = GetLastError();
        if (err != ERROR_NO_MORE_FILES)
            log("WARNING: FindNextFile(): unrecognised error %d", err);
#ifdef _DEBUG
        else
            log("Finished listing files");
#endif
        FindClose(hFile);
    }
    log("Deleted %d logs", nFiles);
}

BOOL DirectoryExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void initLog()
{
#ifndef CHEATDLL_NOLOG
    wchar_t timeBuf[100];
    //wchar_t fullpathBuf[100];
    time_t rawtime;
    tm* tm_time;
    rawtime = time(0);
    tm_time = localtime(&rawtime);
    wcsftime(timeBuf, 99, L"logs\\efpatch_%Y-%m-%d_%H-%M-%S.log", tm_time);

    logFileName = timeBuf;

    if (!DirectoryExists("Logs"))
        CreateDirectory("Logs", 0);

    log_file = _wfopen(timeBuf, L"wt");
    if (!log_file)
    {
        MessageBoxW(0, L"Cannot write log file. Logging disabled", timeBuf, 0);
        loggingEnabled = false;
    }
    else
    {
        log("Logging started");
        loggingEnabled = true;
    }

    for (int i = 0; i < 12; i++)
        lastLogs[i] = (char*)malloc(500);
    logged = 0;

    deleteOldLogs();
#endif
}

void closeLog()
{
#ifndef CHEATDLL_NOLOG
    if (log_file)
    {
        fclose(log_file);
        log_file = NULL;
    }
#endif
}

void flushLog()
{
#ifndef CHEATDLL_NOLOG
    if (log_file)
        fflush(log_file);
#endif
}

void putTime()
{
    SYSTEMTIME st;
    GetSystemTime(&st);

    fprintf(log_file, "[%02d:%02d:%02d.%03d] ",
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

//#ifndef TARGET_VOOBLY
void __cdecl log(const char* format, ...)
{
#ifndef CHEATDLL_NOLOG

    //closeLog ();
    //initLog ();
    flushLog();

    if (loggingEnabled)
    {
        putTime();
        fprintf(log_file, "[efpatch ] ");
        va_list ap;
        va_start(ap, format);
        vfprintf(log_file, format, ap);
        //memset (lastLogs [logged % 3], 0, 500);
        vsprintf(lastLogs[logged % 12], format, ap);
        //
#ifdef _DEBUG
        char lastStr[500];
        strcpy(lastStr, lastLogs[logged % 12]);
        strcat(lastStr, "\x0A");
        OutputDebugString(lastStr);
#endif
        //
        logged++;
        fputs("\x0A", log_file);
        va_end(ap);
    }
#endif
}
/*#else
extern void* voob_log;
extern IVoobly *g_pVoobly;
__declspec(naked) void __cdecl log(const char*, ...)
{
    __asm
    {
        mov     ecx, g_pVoobly
        mov     eax, voob_log
        jmp     eax
    }
}
#endif*/

void __cdecl log_internal(const char* format, ...)
{
#ifndef CHEATDLL_NOLOG

    //closeLog ();
    //initLog ();
    flushLog();

    if (loggingEnabled)
    {
        putTime();
        fprintf(log_file, "[internal] ");
        va_list ap;
        va_start(ap, format);
        vfprintf(log_file, format, ap);
        //memset (lastLogs [logged % 3], 0, 500);
        vsprintf(lastLogs[logged % 12], format, ap);
        //
#ifdef _DEBUG
        char lastStr[500];
        strcpy(lastStr, lastLogs[logged % 12]);
        strcat(lastStr, "\x0A");
        OutputDebugString(lastStr);
#endif
        //
        logged++;
        fputs("\x0A", log_file);
        va_end(ap);
    }
#endif
}
