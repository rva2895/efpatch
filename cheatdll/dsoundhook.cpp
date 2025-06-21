#include "stdafx.h"

#include <dsound.h>
#include "dsoundhook.h"

int BUFCOUNT; //number of additional buffers
int PLAY_DELAY; //minimum allowed delay between instances of the same sound

struct buffer
{
    bool requiredDuplicates;
    bool stopped;
    long startTime;
    LPDIRECTSOUNDBUFFER mainBuf;
    LPDIRECTSOUNDBUFFER bufs[20];
};

bool playhookinstalled = 0;
bool dshookinstalled = 0;

//CRITICAL_SECTION cs;

buffer* buffers;
int buffer_index = 0;

unsigned long curBuf; //pointer to buffer whose duplicates we will create

int getIforThisBuf (LPDIRECTSOUNDBUFFER dsbuffer)
{
    int l = 0;
    //log ("Looking for this buffer in array... ");
    while (buffers[l].mainBuf != dsbuffer)
    {
        l++;
        if (l >= 1000)
            return -1;
    }
    //log ("found required buffer at %d", l);
    return l;
}

void InterceptFunctionsIDS (); //functions which install hooks
void InterceptFunctionsIDSB();
void InterceptDSoundCreate ();

HRESULT __stdcall Intercept_Play //new functions
    (DWORD ds_obj_id,
    DWORD dwReserved1,
    DWORD dwPriority,
    DWORD dwFlags);

HRESULT __stdcall Intercept_Stop
    (DWORD ds_obj_id);

HRESULT __stdcall Intercept_CreateSoundBuffer(
    DWORD ds_obj_id,
    DWORD pcDSBufferDesc,
    DWORD ppDSBuffer,
    DWORD pUnkOuter);

HRESULT __stdcall Intercept_DirectSoundCreate(
        DWORD lpcGuidDevice,
        DWORD ppDS8,
        DWORD pUnkOuter);

//struct jmp_far
//{
//    BYTE instr_push;  //здесь будет код инструкции push
//    DWORD arg;         //аргумент push
//    BYTE  instr_ret;    //здесь будет код инструкции ret
//};

BYTE old_play[6]; //область для хранения 6-ти затираемых байт начала функции
BYTE old_stop[6];
BYTE old_createsoundbuffer[6];
BYTE old_directsoundcreate[6];
DWORD adr_Play; //будущий адрес оригинальной функции
DWORD adr_CreateSoundBuffer;
DWORD adr_DuplicateSoundBuffer;
DWORD adr_GetStatus;
DWORD adr_Stop;
DWORD adr_DirectSoundCreate;
DWORD written; //вспомогательная переменная
BYTE jump_play[6]; //здесь будет машинный код инструкции перехода
BYTE jump_stop[6];
BYTE jump_createsoundbuffer[6];
BYTE jump_directsoundcreate[6];


void initDsoundhook ()
{
    log ("Initializing DirectSound Hook...");

    buffers = (buffer*) malloc (sizeof(buffer)*1000);

    InterceptDSoundCreate ();
}

void InterceptDSoundCreate (void)
{
    HINSTANCE hinst = GetModuleHandle ("dsound.dll");
    adr_DirectSoundCreate = (DWORD)GetProcAddress (hinst, "DirectSoundCreate");

    log ("DirectSoundCreate found at %X", adr_DirectSoundCreate);

    jump_directsoundcreate[0] = 0x68;
    *(DWORD*)(jump_directsoundcreate+1) = (DWORD)&Intercept_DirectSoundCreate;
    jump_directsoundcreate[5] = 0xC3;

    ReadProcessMemory(GetCurrentProcess(),(void*) adr_DirectSoundCreate,
        (void*)old_directsoundcreate, 6, &written);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_DirectSoundCreate,
        (void*)jump_directsoundcreate, 6, &written);

    log ("Hook on DirectSoundCreate successfully installed");

    InterceptFunctionsIDS();
}



void InterceptFunctionsIDS(void)
{
    //StopThreads(1);

    //сначала получим абсолютный адрес функции для перехвата

    // Зададим машинный код инструкции перехода, который затем впишем
    // в начало полученного адреса:
    jump_play[0] = 0x68;
    *(DWORD*)(jump_play+1) = (DWORD)&Intercept_Play;
    jump_play[5] = 0xC3;

    jump_stop[0] = 0x68;
    *(DWORD*)(jump_stop+1) = (DWORD)&Intercept_Stop;
    jump_stop[5] = 0xC3;

    jump_createsoundbuffer[0] = 0x68;
    *(DWORD*)(jump_createsoundbuffer+1) = (DWORD)&Intercept_CreateSoundBuffer;
    jump_createsoundbuffer[5] = 0xC3;

    //Прочитаем и сохраним первые оригинальные 6 байт стандартной API функции

    ReadProcessMemory(GetCurrentProcess(),(void*) adr_CreateSoundBuffer,
        (void*)old_createsoundbuffer, 6, &written);

    //Запишем команду перехода на нашу функцию поверх этих 6-ти байт


    WriteProcessMemory(GetCurrentProcess(), (char*)adr_CreateSoundBuffer,
        (void*)jump_createsoundbuffer, 6, &written);

    //StopThreads(0);
    log ("Hook on CreateSoundBuffer successfully installed");
}


void InterceptFunctionsIDSB(void)
{
    ReadProcessMemory(GetCurrentProcess(),(void*) adr_Play,
        (void*)old_play, 6, &written);
    
    WriteProcessMemory(GetCurrentProcess(), (char*)adr_Play,
        (void*)jump_play, 6, &written);
    
    ReadProcessMemory(GetCurrentProcess(),(void*) adr_Stop,
        (void*)old_stop, 6, &written);
    
    WriteProcessMemory(GetCurrentProcess(), (char*)adr_Stop,
        (void*)jump_stop, 6, &written);

    log ("Hooks on Play and Stop successfully installed");
}


HRESULT __stdcall Intercept_DirectSoundCreate(
        DWORD lpcGuidDevice,
        DWORD ppDS8,
        DWORD pUnkOuter
)
{
    //StopThreads (1);
    //InitializeCriticalSection (&cs);
    
    log ("DirectSoundCreate called, ");

    //EnterCriticalSection (&cs);

    WriteProcessMemory(GetCurrentProcess(), (void*)adr_DirectSoundCreate,
        (void*)old_directsoundcreate, 6, &written);

    //LeaveCriticalSection (&cs);

    //memset(buffers, 0, sizeof (buffer) * 1000);
    //i = 0;

    log ("flushing buffers");

    HRESULT theresult = ((HRESULT (__stdcall*)(DWORD, DWORD, DWORD))adr_DirectSoundCreate)
        (lpcGuidDevice,
        ppDS8,
        pUnkOuter
    );

    if (!dshookinstalled)
    {
        adr_DuplicateSoundBuffer = *(DWORD*)(**(DWORD**)ppDS8 + 0x14);
        adr_CreateSoundBuffer = *(DWORD*)(**(DWORD**)ppDS8 + 0xC);

        log ("IDirectSound::DuplicateSoundBuffer found at %X", adr_DuplicateSoundBuffer);
        log ("IDirectSound::CreateSoundBuffer found at %X", adr_CreateSoundBuffer);

        InterceptFunctionsIDS ();
        dshookinstalled = 1;
    }
    //EnterCriticalSection (&cs);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_DirectSoundCreate,
        (void*)jump_directsoundcreate, 6, &written);

    //LeaveCriticalSection (&cs);

    //StopThreads (0);

    return theresult;
}

HRESULT __stdcall Intercept_Play(
    DWORD ds_obj_id,
    DWORD dwReserved1,
    DWORD dwPriority,
    DWORD dwFlags)
{
#ifdef _DEBUG
    log ("IDirectSoundBuffer::Play called, buffer %X, flags %X", ds_obj_id, dwFlags);
#endif

    DWORD ds_status;
    HRESULT returnvalue;
    LPDIRECTSOUNDBUFFER ds_id = (LPDIRECTSOUNDBUFFER)ds_obj_id;
    int n = getIforThisBuf (ds_id);
    
    //log ("Found this buffer at %d", n);
    
    int s = -1;
    LPDIRECTSOUNDBUFFER bufToPlay;
    if ( (buffers[n].requiredDuplicates) && (n != -1) )
        for (int j = 0; j < BUFCOUNT; j++)
        {
            buffers[n].bufs[j]->GetStatus (&ds_status);
            //log ("Additional buffer %d, status %d", buffers[n].bufs[j], ds_status);
            if (ds_status & DSBSTATUS_PLAYING)
                continue;
            else
            {
                s = j;
                break;
            }
        }
    
    //log ("S = %d", s);

    if (!buffers[n].requiredDuplicates)
        bufToPlay = buffers[n].mainBuf;
    else if (dwFlags & DSBPLAY_LOOPING)
        bufToPlay = buffers[n].mainBuf;
    else if (buffers[n].bufs[s] == 0)
        ExitProcess (0);
    else if (s == -1)
        bufToPlay = buffers[n].mainBuf;
    else
        bufToPlay = buffers[n].bufs[s];

    long currentTime = timeGetTime ();

    if ( (currentTime - buffers[n].startTime) > PLAY_DELAY
        || buffers[n].stopped)         //if enough time had passed OR buffer is marked as stopped
    {
        buffers[n].startTime = currentTime;

        if (buffers[n].stopped)
        {
            bufToPlay = buffers[n].mainBuf;
            buffers[n].stopped = 0;
#ifdef _DEBUG
            log ("Buffer is marked as stopped, therefore no duplicates used");
#endif
        }

#ifdef _DEBUG
        log ("Calling IDirectSoundBuffer::Play for buffer %X", bufToPlay);
#endif

        WriteProcessMemory(GetCurrentProcess(), (void*)adr_Play,
        (void*)old_play, 6, &written);

        returnvalue = ((HRESULT (__stdcall*)(DWORD, DWORD, DWORD, DWORD))adr_Play)
        ((DWORD)bufToPlay,
        dwReserved1,
        dwPriority,
        dwFlags);

        //log ("IDirectSoundBuffer::Play returned %X", returnvalue);

        //Снова заменяем  6 байт функции на команду перехода на нашу функцию
        //WriteProcessMemory(GetCurrentProcess(), (char*)adr_Play,
        //(void*)&jump_play, 6, &written);
    }
    else
    {
        returnvalue = 0;
    }

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_Play,
        (void*)jump_play, 6, &written);

    //log ("IDirectSoundBuffer::Play finished");

    //LeaveCriticalSection (&cs);

    //StopThreads(0);

    return returnvalue;
}

HRESULT __stdcall Intercept_Stop(
    DWORD ds_obj_id)
{
#ifdef _DEBUG
    log ("IDirectSoundBuffer::Stop called, buffer %X", ds_obj_id);
#endif
    
    LPDIRECTSOUNDBUFFER ds_id = (LPDIRECTSOUNDBUFFER)ds_obj_id;
    int n = getIforThisBuf (ds_id);
    
    //log ("Found this buffer at %d", n);
    
    buffers[n].stopped = 1;

#ifdef _DEBUG
    log ("Buffer %X has been marked as stopped", ds_obj_id);
#endif
    /*DWORD ds_status;
    HRESULT returnvalue;
    LPDIRECTSOUNDBUFFER ds_id = (LPDIRECTSOUNDBUFFER)ds_obj_id;
    int n = getIforThisBuf (ds_id);
    
    log ("Found this buffer at %d\x0D\x0A", n);
    
    int s = -1;
    LPDIRECTSOUNDBUFFER bufToPlay;
    if ( (buffers[n].requiredDuplicates) && (n != -1) )
        for (int j = 0; j < BUFCOUNT; j++)
        {
            buffers[n].bufs[j]->GetStatus (&ds_status);
            log ("Additional buffer %d, status %d\x0D\x0A", buffers[n].bufs[j], ds_status);
            if (ds_status & DSBSTATUS_PLAYING)
                continue;
            else
            {
                s = j;
                break;
            }
        }
    
    log ("S = %d\x0D\x0A", s);

    if (!buffers[n].requiredDuplicates)
        bufToPlay = buffers[n].mainBuf;
    else if (dwFlags & DSBPLAY_LOOPING)
        bufToPlay = buffers[n].mainBuf;
    else if (buffers[n].bufs[s] == 0)
        exit (0);
    else if (s == -1)
        bufToPlay = buffers[n].mainBuf;
    else
        bufToPlay = buffers[n].bufs[s];

    unsigned long currentTime = timeGetTime ();

    if ( (currentTime - buffers[n].startTime) > PLAY_DELAY) //if enough time had passed
    {
        buffers[n].startTime = currentTime;

        log ("Calling IDirectSoundBuffer::Play for buffer %X\x0D\x0A", bufToPlay);
        */
        WriteProcessMemory(GetCurrentProcess(), (void*)adr_Stop,
        (void*)old_stop, 6, &written);
        
        HRESULT returnvalue = ((HRESULT (__stdcall*)(DWORD))adr_Stop)
        ((DWORD)ds_obj_id);

        //log ("IDirectSoundBuffer::Stop returned %X", returnvalue);

        //Снова заменяем  6 байт функции на команду перехода на нашу функцию
        //WriteProcessMemory(GetCurrentProcess(), (char*)adr_Play,
        //(void*)&jump_play, 6, &written);
    /*}
    else
    {
        returnvalue = 0;
    }*/

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_Stop,
        (void*)jump_stop, 6, &written);

    //log ("IDirectSoundBuffer::Stop finished");

    //LeaveCriticalSection (&cs);

    //StopThreads(0);

    return returnvalue;
}

HRESULT __stdcall Intercept_CreateSoundBuffer(
    DWORD ds_obj_id,
    DWORD pcDSBufferDesc,
    DWORD ppDSBuffer,
    DWORD pUnkOuter)
{
#ifdef _DEBUG
    log ("IDirectSound::CreateSoundBuffer called, ");
#endif

    //EnterCriticalSection (&cs);

    WriteProcessMemory(GetCurrentProcess(), (void*)adr_CreateSoundBuffer,
        (void*)old_createsoundbuffer, 6, &written);

    //LeaveCriticalSection (&cs);

#ifdef _DEBUG
    log ("flags are %X", (*(LPDSBUFFERDESC)pcDSBufferDesc).dwFlags);
#endif

    HRESULT returnvalue = ((HRESULT (__stdcall*)(DWORD, DWORD, DWORD, DWORD))adr_CreateSoundBuffer)
        (ds_obj_id,
        pcDSBufferDesc,
        ppDSBuffer,
        pUnkOuter);

#ifdef _DEBUG
    log ("Buffer %X created", *(int*)ppDSBuffer);
#endif

    curBuf = getIforThisBuf(*(LPDIRECTSOUNDBUFFER*)ppDSBuffer);
    if (curBuf != -1)
    {
        log ("WARNING: Pointer to this buffer already exists, overwriting it.");
        
        //MessageBox (0, "Achtung!", "OOPS", MB_ICONEXCLAMATION);
    }
    else
    {
        curBuf = buffer_index;
        buffer_index++;
    }

    if (!playhookinstalled)
    {
        adr_Play = *(DWORD*)(**(DWORD**)ppDSBuffer + 0x30);
        adr_GetStatus = *(DWORD*)(**(DWORD**)ppDSBuffer + 0x24);
        adr_Stop = *(DWORD*)(**(DWORD**)ppDSBuffer + 0x48);

        log ("IDirectSoundBuffer::Play found at %X", adr_Play);
        log ("IDirectSoundBuffer::GetStatus found at %X", adr_GetStatus);
        log ("IDirectSoundBuffer::Stop found at %X", adr_Stop);

        playhookinstalled = 1;
        InterceptFunctionsIDSB ();
    }

    HRESULT tempreturn;

    //LPDIRECTSOUND ds_id = (LPDIRECTSOUND)ds_obj_id;
    if (!( (*(LPDSBUFFERDESC)pcDSBufferDesc).dwFlags & DSBCAPS_PRIMARYBUFFER ))
    {
        for (int j = 0; j < (BUFCOUNT+1); j++)
        {
            tempreturn = ((HRESULT (__stdcall*)(DWORD, DWORD, DWORD))adr_DuplicateSoundBuffer)
                (ds_obj_id,
                (DWORD)*(DWORD*)ppDSBuffer,
                (DWORD)&(buffers[curBuf].bufs[j])
                );
#ifdef _DEBUG
            log ("Created duplicated buffer %X with return value %X", buffers[curBuf].bufs[j], tempreturn);
#endif
        }
        buffers[curBuf].requiredDuplicates = 1;
    }
    else
    {
        buffers[curBuf].requiredDuplicates = 0;
#ifdef _DEBUG
        log ("This is primary buffer, therefore no duplicates");
#endif
    }
    buffers[curBuf].mainBuf = (LPDIRECTSOUNDBUFFER)(*(DWORD*)ppDSBuffer);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_CreateSoundBuffer,
        (void*)jump_createsoundbuffer, 6, &written);

    return returnvalue;
}
