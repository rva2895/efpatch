#include "stdafx.h"
#include "log.h"
#include <time.h>

FILE* log_file = NULL;
bool loggingEnabled = false;

std::wstring logFileName;

void deleteOldLogs()
{
    struct delete_logs_callback_param
    {
        int nFiles;
        time_t rawtime_current;
    } param;
    param.nFiles = 0;
    param.rawtime_current = time(NULL);

    auto delete_logs_callback = [](const char* filename, void* param)
        {
            delete_logs_callback_param* p = (delete_logs_callback_param*)param;
            tm time_old;
            memset(&time_old, 0, sizeof(tm));
            sscanf_s(filename, "efpatch_%d-%d-%d_%d-%d-%d.log",
                &time_old.tm_year, &time_old.tm_mon, &time_old.tm_mday,
                &time_old.tm_hour, &time_old.tm_min, &time_old.tm_sec);
            time_old.tm_year -= 1900;
            time_old.tm_mon -= 1;

            time_t rawtime_old = mktime(&time_old);
            if (difftime(p->rawtime_current, rawtime_old) > 604800) //1 week
            {
                char filename_with_path[MAX_PATH];
                snprintf(filename_with_path, _countof(filename_with_path), "logs\\%s", filename);
                if (!DeleteFile(filename_with_path))
                {
                    log("Error: cannot delete %s", filename);
                }
                else
                {
                    p->nFiles++;
                }
            }
        };

    findfirst_callback("logs\\efpatch*.log", delete_logs_callback, &param);

    log("Deleted %d logs", param.nFiles);
}

BOOL DirectoryExistsW(LPCWSTR szPath)
{
    DWORD dwAttrib = GetFileAttributesW(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void initLog()
{
    wchar_t timeBuf[100];
    time_t rawtime;
    tm* tm_time;
    rawtime = time(NULL);
    tm_time = localtime(&rawtime);
    wcsftime(timeBuf, _countof(timeBuf) - 1, L"logs\\efpatch_%Y-%m-%d_%H-%M-%S.log", tm_time);

    logFileName = timeBuf;

    if (!DirectoryExistsW(L"Logs"))
        CreateDirectoryW(L"Logs", 0);

    log_file = _wfopen(timeBuf, L"wt");
    if (!log_file)
    {
        loggingEnabled = false;
        MessageBoxW(0, L"Cannot write log file. Logging disabled", timeBuf, 0);
    }
    else
    {
        loggingEnabled = true;
        log("Logging started");
#ifdef TARGET_VOOBLY
#ifdef VOOBLY_EF
        log(EFPATCH_VERSION);
#else
        log(USERPATCH_VERSION);
#endif
#else
        log(EFPATCH_VERSION);
#endif
        log("Configuration: %s", EFPATCH_CURRENT_CONFIG);
        deleteOldLogs();
    }
}

void closeLog()
{
    if (log_file)
    {
        log("Logging stopped");
        fclose(log_file);
        log_file = NULL;
    }
}

void __cdecl log_do(bool is_internal, const char* format, va_list ap)
{
    char buf[0x1000];
    SYSTEMTIME st;
    GetSystemTime(&st);
    vsnprintf(buf, _countof(buf) - 1, format, ap);
    if (loggingEnabled)
    {
        fprintf(log_file, "[%02d:%02d:%02d.%03d] %s %s\n",
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
            is_internal ? "[internal]" : "[efpatch ]", buf);
        fflush(log_file);
    }
#ifdef _DEBUG
    strcat_s(buf, _countof(buf), "\n");
    OutputDebugString(buf);
#endif
}

void __cdecl log(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log_do(false, format, ap);
    va_end(ap);
}

void __cdecl vlog(const char* format, va_list ap)
{
    log_do(false, format, ap);
}

void __cdecl log_internal(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log_do(true, format, ap);
    va_end(ap);
}
