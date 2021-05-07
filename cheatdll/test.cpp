#include "stdafx.h"

#include "test.h"
#include "log.h"
#include "advcheat.h"
#include "rundll.h"

#include <process.h>
#include <MMSystem.h>
#include <vector>
#include <string>
#include <map>

/*__declspec(naked) int someText()
{
	__asm
	{
		push    eax
		mov     ecx, [eax + 38h]
		push    ecx
		call    someTextOut
		pop     eax

		mov     edx, [esi + 100h]
		push    5E02BEh
		ret
	}
}*/

#ifdef _DEBUG

int effectsFired = 0;
int triggerBegun = 0;
char triggerText[256];

void __cdecl effectLog(int n, char* ef)
{
	if (triggerBegun)
	{
		log("%s", triggerText);
		triggerBegun = 0;
	}
	log("---> Effect %d (%d)", n, *(int*)(ef + 4));
}

__declspec(naked) int effectLogHook() //005F4A77
{
	__asm
	{
		mov     eax, [edx]
		mov     ecx, [ecx + edi * 4]
		push    eax
		push    ecx
		push    edi
		call    effectLog
		pop     edi
		pop     ecx
		push    005F4A7Dh
		ret
	}
}

void __cdecl triggerLog(int n, char* tr)
{
	triggerBegun = 1;

	char szName[] = "Untitled";
	//*szName = 0;
	char* name;
	if (*(char**)(tr + 0x14))
		name = *(char**)(tr + 0x14);
	else
		name = szName;

	if (*tr == 1)
		sprintf(triggerText, "Trigger %d (name: %s; %d conditions, %d effects", n,
			name, *(int*)(tr + 0x20), *(int*)(tr + 0x30));
}

__declspec(naked) int triggerLogHook() //005F54D2
{
	__asm
	{
		mov     ecx, [esi + 10h]
		push    ebx
		mov     ecx, [ecx + edi * 4]
		push    ecx
		push    edi
		call    triggerLog
		pop     edi
		pop     ecx
		push    005F54D9h
		ret
	}
}
#endif

int retSave;
int data;

const char title[] = "DAT";
const char text[] = "read";

__declspec(naked) int onReadDat()
{
	__asm
	{
		mov     ecx, data
		mov     ecx, [ecx]
		cmp     cx, 9852
		jnz     _end
		//int     3
		//push    eax
		//push    0
		//push    offset title
		//push    offset text
		//push    0
		//call    ds:[MessageBoxA]
		//pop     eax
_end:
		mov     ecx, retSave
		jmp     ecx
	}
}

__declspec(naked) int readDatHook() //004D5550
{
	__asm
	{
		mov     eax, [esp]
		mov     retSave, eax
		mov     data, edx
		mov     eax, onReadDat
		mov     [esp], eax
		push    ebp
		mov     ebp, esp
		sub     esp, 0Ch
		push    004D5556h
		ret
	}
}

FILE* rms_f = NULL;

__declspec(naked) void rms_fptr() //004E1801
{
	__asm
	{
		mov     rms_f, eax
		mov     eax, [ebp + 45E28h]
		mov     edi, 004E1807h
		jmp     edi
	}
}

__declspec(naked) void rms_fptr_close() //004E1951
{
	__asm
	{
		xor     eax, eax
		mov     rms_f, eax
		mov     eax, [ebp + 3F6FCh]
		mov     ecx, 004E1957h
		jmp     ecx
	}
}

__int32 (__cdecl* ftell2)(FILE *File) =
	(__int32 (__cdecl*) (FILE*))0x006361B6;

int (__cdecl* fseek2)(FILE *File, __int32 Offset, int Origin) =
	(int (__cdecl*) (FILE*, __int32, int))0x00635E6D;

size_t (__cdecl* fread2)(void *DstBuf, size_t ElementSize, size_t Count, FILE *File) =
	(size_t (__cdecl*) (void*, size_t, size_t, FILE*))0x006360CE;

int (__cdecl* fscanf2)(FILE *File, const char *Format, ...) =
	(int (__cdecl*) (FILE*, const char*, ...))0x0063481F;

char* (__cdecl* fgets2)(char *Buf, int MaxCount, FILE *File) =
	(char* (__cdecl*) (char*, int, FILE*))0x0063303A;

extern std::string rms_error_1;
extern std::string rms_error_2;

extern bool isEditor;

extern bool editorstatus_isValid;
bool rms_first_error = false;

char* log_int_s = NULL;
__declspec(noinline) void __cdecl log_int(int unk1, char* fmt, ...)
{
	UNREFERENCED_PARAMETER(unk1);
	va_list ap;
	__int32 pos;
	void* d;
	int lines;
	if (!log_int_s)
		log_int_s = (char*)malloc(0x200);
	
	if (!(((unsigned long)fmt >= 0x00689000) && ((unsigned long)fmt < 0x7A3A2C)))
		return;

	if ((strlen(fmt) < 8) || (strlen(fmt) > 0x180))
		return;

	if (!strncmp("Act_Art", fmt, 7))
		return;

	//
	char b[0x100];
	bool rms_flag = false;

	if (!strcmp(fmt, "------Opening script (%s)."))
	{
		//rms_begin = true;
		rms_error_1 = "RMS: No errors";
		rms_error_2 = "";
		if (isEditor)
			update_editor_bk();
		rms_first_error = true;
	}

	if (rms_f && (rms_f != (FILE*)-1))
	{
		pos = ftell2(rms_f);
		if (pos > 0)
		{
			d = malloc(pos);
			fseek2(rms_f, 0, SEEK_SET);
			lines = 0;
			do
			{
				fgets2(log_int_s, 0x200, rms_f);
				lines++;
			} while (ftell2(rms_f) < pos);

			free(d);
			fseek2(rms_f, pos, SEEK_SET);
			log("** RMS, line %d:\n\n%s", lines, log_int_s);
			if (rms_first_error)
			{
				sprintf(b, "RMS: line %d: %s", lines, log_int_s);
				rms_error_1 = b;
				rms_flag = true;
				rms_first_error = false;
			}
		}
	}
	//

	va_start(ap, fmt);
	vsprintf(log_int_s, fmt, ap);
	log_internal("%s", log_int_s);
	if (rms_flag)
	{
		sprintf(b, "%s", log_int_s);
		rms_error_2 = b;
		rms_flag = false;
	}
	va_end(ap);
}

void* log_int_retval;

__declspec(naked) void log_int_end_wr()
{
	__asm
	{
		mov     eax, log_int_retval
		sub     esp, 4
		jmp     eax
	}
}

__declspec(naked) void log_int_wr()
{
	__asm
	{
		mov     eax, [esp + 8]
		cmp     eax, 00689000h
		jb      _fix_log_ret
		cmp     eax, 007A3A2Ch
		jnb     _fix_log_ret
		jmp     log_int
_fix_log_ret:
		pop     eax
		mov     log_int_retval, eax
		mov     eax, log_int_end_wr
		mov     [esp], eax
		jmp     log_int
		ret
	}
}

__declspec(naked) void nullsub_26()
{
	__asm
	{
		ret
	}
}

int check_file(char* file, int player)
{
	char s[0x100];
	sprintf(s, "Checking %s...", file);
	sendChat(s, player);
	FILE* f = fopen(file, "rb");
	if (!f)
	{
		sprintf(s, "Failed to open %s!", file);
		sendChat(s, player);
		return 0;
	}
	char b;
	char sum = 0;
	while (fread(&b, 1, 1, f) > 0)
		sum += b;
	sprintf(s, "File %s: %d", file, sum);
	sendChat(s, player);
	return sum;
}

void thread_proc(void* p)
{
	int player = (int)p;
	int sum = 0;
	sum += check_file("battlegrounds_x1.exe", player);
	sum += check_file("efpatch.dll", player);
	sum += check_file("language.dll", player);
	sum += check_file("language_x1.dll", player);
	sum += check_file("language_x2.dll", player);
	sum += check_file("data\\genie_x2.dat", player);
	sum += check_file("data\\gamedata.drs", player);
	sum += check_file("data\\gamedata_x1.drs", player);
	sum += check_file("data\\gamedata_x2.drs", player);
	sum += check_file("data\\expl.txt", player);
	sum += check_file("data\\ground-to-air.txt", player);
	sum += check_file("data\\jedi-holo.txt", player);
	sum += check_file("data\\master.txt", player);
	sum += check_file("data\\padawan.txt", player);
	sum += check_file("data\\resgen.txt", player);
	sum += check_file("data\\unconv.txt", player);
	sum += check_file("data\\terrain.txt", player);
	char s[0x100];
	sprintf(s, "Integrity check complete, checksum = %d", sum);
	sendChat(s, player);
}

extern int memory_temp;

int __stdcall onChat_2(int player, char* targets, char* s)
{
	UNREFERENCED_PARAMETER(targets);
	if (!strcmp(s, "/test"))
	{
		sendChat("Started integrity check", player);
		_beginthread(thread_proc, 0, (void*)player);
		return 1;
	}
	else if (!strcmp(s, "/version"))
	{
		sendChat("Compile time: " __DATE__ ", " __TIME__, player);
		return 1;
	}
	else if (!strcmp(s, "/memory"))
	{
		sendChat("Memory: set to 0x100000", player);
		memory_temp = 0x100000;
		return 1;
	}
	else
		return 0;
}

__declspec(naked) void onChat() //00438140
{
	__asm
	{
		push    ecx
		mov     eax, [esp + 10h]
		push    eax
		mov     eax, [esp + 10h]
		push    eax
		mov     eax, [esp + 10h]
		push    eax
		call    onChat_2
		pop     ecx
		test    eax, eax
		jnz     __skip_chat
		sub     esp, 118h
		mov     eax, 00438146h
		jmp     eax
__skip_chat:
		xor     eax, eax
		ret     0Ch
	}
}

void* malloc_locret;
void* malloc_pos;
int malloc_size;

void __stdcall malloc_log(void* ptr)
{
	log("malloc: allocated %d at %p to %p", malloc_size, malloc_pos, ptr);
}

__declspec(naked) void malloc_hook_end()
{
	__asm
	{
		push    eax
		push    eax
		call    malloc_log
		pop     eax
		mov     ecx, malloc_locret
		jmp     ecx
	}
}

__declspec(naked) void malloc_hook() //0063329E
{
	__asm
	{
		mov     eax, [esp + 4]
		mov     malloc_size, eax
		mov     eax, [esp]
		mov     malloc_locret, eax
		mov     eax, [esp + 0Ch]
		mov     malloc_pos, eax
		mov     eax, malloc_hook_end
		mov     [esp], eax
		cmp     dword ptr [esp + 4], 0FFFFFFE0h
		mov     eax, 006332A3h
		jmp     eax
	}
}

__declspec(naked) void malloc2_hook() //00632D33
{
	__asm
	{
		mov     eax, [esp + 4]
		imul    eax, [esp + 8]
		mov     malloc_size, eax
		mov     eax, [esp]
		mov     malloc_locret, eax
		mov     malloc_pos, eax
		mov     eax, malloc_hook_end
		mov     [esp], eax

		push    ebp
		mov     ebp, esp
		push    esi
		mov     esi, [ebp + 8]
		mov     eax, 00632D3Ah
		jmp     eax
	}
}

void __stdcall rms_token_log(char char5, char char4, char char3, char char2, int id, char char1, char* token, void* caller)
{
	log("RMS: (%d, %d, %d, %d, %d) - @0x%p: %d = %s", (int)char1, (int)char2, (int)char3, (int)char4, (int)char5, caller, id, token);
}

__declspec(naked) void onRmsToken() //004E1D40
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		push    ecx
		mov     eax, [ebp + 4] //caller
		mov     ecx, [ebp + 8] //token
		push    eax
		push    ecx
		mov     eax, [ebp + 0Ch] //char1
		mov     ecx, [ebp + 10h] //id
		push    eax
		push    ecx
		mov     eax, [ebp + 14h] //char2
		mov     eax, [ebp + 18h] //char3
		push    eax
		push    ecx
		mov     eax, [ebp + 1Ch] //char4
		mov     eax, [ebp + 20h] //char5
		push    eax
		push    ecx
		call    rms_token_log
		pop     ecx
		leave

		push    ebx
		push    ebp
		push    esi
		push    edi
		mov     edi, [esp + 14h]
		mov     eax, 004E1D48h
		jmp     eax
	}
}

void fix_function_call(DWORD addr, DWORD f)
{
	writeDword(addr, f - (addr + 4));
}

std::vector<void*> addresses;

std::vector<void*> adr_test;

void __stdcall add_addr(void* a)
{
	for (auto it = addresses.begin(); it != addresses.end(); ++it)
		if (*it == a)
			return;
	addresses.push_back(a);
}

extern bool time_collect;
extern bool time_stage_find;

int left;
int right;

volatile bool answer_flag = false;
volatile bool answer = false;

void msg_box(const char* s, int formal)
{
	UNREFERENCED_PARAMETER(formal);
	MessageBox(0, s, "Message", 0);
}

bool check(int l, int r)
{
	left = l;
	right = r;
	answer_flag = false;
	char s[0x100];
	sprintf(s, "Checking %d - %d, condition met? [/yes] [/no]", l, r);
	msg_box(s, 0);
	while (!answer_flag)
		;
	return answer;
}

// A iterative binary search function. It returns 
// location of x in given array arr[l..r] if present, 
// otherwise -1 
int search_function(std::vector<void*> arr)
{
	int l = 0;
	int r = arr.size() - 1;
	while (l <= r)
	{
		int m = l + (r - l) / 2;

		// Check if x is present at mid 
		
		if (check(m, m))
			return m;

		// If x greater, ignore left half 
		if (check(m, r))
			l = m + 1;

		// If x is smaller, ignore right half 
		else
			r = m - 1;
	}

	// if we reach here, then element was 
	// not present 
	return -1;
}

void time_find(void*)
{
	int p = search_function(addresses);
	char s[0x100];
	if (p != -1)
	{
		sprintf(s, "Found at 0x%X", addresses[p]);
		msg_box(s, 0);
	}
	else
		msg_box("Not found!", 0);
}

bool thread_begun = false;

DWORD adr_time;
BYTE jump_time[6];
BYTE old_time[6];

DWORD adr_text;
BYTE jump_text[6];
BYTE old_text[6];

DWORD last_call = 0;

DWORD __stdcall Intercept_timeGetTime()
{
	DWORD written;
	WriteProcessMemory(GetCurrentProcess(), (void*)adr_time,
		(void*)old_time, 6, &written);

	DWORD retval = timeGetTime();

	WriteProcessMemory(GetCurrentProcess(), (char*)adr_time,
		(void*)jump_time, 6, &written);

	//void* retaddr = _ReturnAddress();
	if (time_collect)
		add_addr(_ReturnAddress());

	/*if (!time_stage_find && ((retval - last_call) > 10000))
	{
		last_call = retval;
		char s[50];
		sprintf(s, "Found %d addresses", addresses.size());
		//sendChat(s, 0);
		MessageBox(0, s, "timeGetTime", 0);
	}*/

	if (time_stage_find && !thread_begun)
	{
		thread_begun = true;
		_beginthread(time_find, 0, 0);
	}

	if (time_stage_find)
		for (int i = left; i <= right; i++)
			if (addresses[i] == _ReturnAddress())
				retval *= 10;
	//retval *= 10;
	//li->QuadPart *= 10;

	return retval;
}

void interceptTime()
{
	DWORD written;
	HINSTANCE hinst = GetModuleHandle("win32.dll");
	adr_time = (DWORD)GetProcAddress(hinst, "timeGetTime");

	log("timeGetTime found at %X", adr_time);

	jump_time[0] = 0x68;
	*(DWORD*)(jump_time + 1) = (DWORD)&Intercept_timeGetTime;
	jump_time[5] = 0xC3;

	ReadProcessMemory(GetCurrentProcess(), (void*)adr_time,
		(void*)old_time, 6, &written);

	WriteProcessMemory(GetCurrentProcess(), (char*)adr_time,
		(void*)jump_time, 6, &written);
}

int __stdcall Intercept_DrawTextA(HDC hdc, LPCSTR s, int c, LPRECT r, UINT format)
{
	//DrawTextA()
	DWORD written;
	WriteProcessMemory(GetCurrentProcess(), (void*)adr_text,
		(void*)old_text, 6, &written);

	if (c == -1)
		c = strlen(s);

	int x = r->left;
	int y = r->top;
	if (format & DT_BOTTOM)
		y = r->bottom - 15;

	//if (strstr(s, "for the"))
	//	__debugbreak();

	int retval;
	if (format & DT_RIGHT)
		retval = DrawTextA(hdc, s, c, r, format);
	else
		retval = TextOutA(hdc, x, y, s, c);

	WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
		(void*)jump_text, 6, &written);

	return retval;
}

void interceptTextOut()
{
	DWORD written;
	HINSTANCE hinst = GetModuleHandle("user32.dll");
	adr_text = (DWORD)GetProcAddress(hinst, "DrawTextA");

	log("DrawTextA found at %X", adr_text);

	jump_text[0] = 0x68;
	*(DWORD*)(jump_text + 1) = (DWORD)&Intercept_DrawTextA;
	jump_text[5] = 0xC3;

	ReadProcessMemory(GetCurrentProcess(), (void*)adr_text,
		(void*)old_text, 6, &written);

	WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
		(void*)jump_text, 6, &written);
}

void thread_scroll(void*)
{
	float x = 10;
	float y = 10;
	float step = 0.02f;
	DWORD d = 16;
	Sleep(15000);
	DWORD prev_time = timeGetTime();
	while (true)
	{
		void* player = getCurrentPlayer();
		if (player)
		{
			player_scrollView(player, x, y, 0);
			x += step;
			y += step;
			if (x > 100)
				x = 10;
			if (y > 100)
				y = 17;
			while ((timeGetTime() - prev_time) < d)
				;
			prev_time = timeGetTime();
		}
	}
}

long __stdcall my_ftol(float x, float y)
{
	return MAKELPARAM(x, y);
}

__declspec(naked) void onScroll() //0060BCFF
{
	__asm
	{
		push    ebx
		push    ebp
		mov     eax, [edi + 178h]
		mov     ecx, [edi + 17Ch]
		push    ecx
		push    eax
		call    my_ftol
		mov     [esi + 12Eh], ax
		shr     eax, 16
		mov     ecx, 0060BD1Eh
		jmp     ecx
	}
}

__declspec(naked) void onScroll2() //0060BE1C
{
	__asm
	{
		movsx   ebx, word ptr [esi + 130h]
		//inc     ebx
		//inc     edx
		//add ebx, 10
		//add edx, 10
		push    0060BE23h
		ret
	}
}

void __stdcall add_drs(char* name, int id)
{
	FILE* f = fopen("drs.txt", "at+");
	fprintf(f, "%d,%s\n", id, name);
	fclose(f);
}

__declspec(naked) void loadDRSHookT() //00542870
{
	__asm
	{
		push    ecx
		mov     eax, [esp + 0Ch]
		push    eax
		mov     eax, [esp + 0Ch]
		push    eax
		call    add_drs
		pop     ecx
		sub     esp, 104h
		mov     eax, 00542876h
		jmp     eax
	}
}

float old_val;
float new_val;

float glitched_res = 0;

void __stdcall check_res()
{
	if (old_val < new_val)
		//__debugbreak();
	{
		glitched_res += (new_val - old_val);
	}
}

__declspec(naked) void repair_test() //005D0D59
{
	__asm
	{
		fmul    dword ptr [esp+10h]
		fsubr   dword ptr [ecx]
		
		push    eax
		push    ecx
		push    edx
		mov     eax, [ecx]
		mov     old_val, eax

		fstp    dword ptr [ecx]
		
		mov     eax, [ecx]
		mov     new_val, eax

		call    check_res

		pop     edx
		pop     ecx
		pop     eax

		push    005D0D61h
		ret
	}
}

/*struct function_data
{
	BYTE original_data[11];
};

//std::map<std::pair<DWORD, function_data>> functions;
std::map<DWORD, function_data> functions;

DWORD __stdcall record_function_call(DWORD caller, DWORD function)
{

}

DWORD __stdcall record_function_return()
{

}

__declspec(naked) void on_function_return()
{
	__asm
	{

	}
}

__declspec(naked) void on_function_call()
{
	__asm
	{
		push    ecx
		push    edx
		mov     eax, [esp + 0Ch] //function
		mov     ecx, [esp + 10h] //caller
		push    eax
		push    ecx
		call    record_function_call
		push    eax
		push    edi
		push    esi
		push    0C8763E65h
		sub     esp, 100h
		mov     [esp], on_function_return
		lea     esi, [esp + 120h]
		lea     edi, [esp + 4]
		mov     ecx, 60
		movsd
		mov     eax, [esp + 118h]
		jmp     eax
	}
}

void __stdcall add_function(DWORD addr)
{
	function_data fd;
	DWORD r;
	ReadProcessMemory(GetCurrentProcess(), (void*)addr, fd.original_data, 11, &r);
	writeByte(addr, 0xE8); //call
	writeDword(addr + 1, 0); //$+0
	writeByte(addr + 5, 0x68); //push
	writeDword(addr + 6, (DWORD)&on_function_call); //address
	writeByte(addr + 10, 0xC3); //ret
}*/

//int control_source;
//int control_target = 1;
//bool control_initiated = false;

//for takecontrol
/*__declspec(naked) void onSync() //00432DFF
{
	__asm
	{
		mov      al, control_initiated
		test     al, al
		jz       _no_control
		push     ecx
		push     control_source
		call     takeControl
		pop      ecx
		mov      eax, 004496A0h
		call     eax
		mov      eax, control_target
		push     eax
		call     takeControl
		mov      eax, 00432E04h
		jmp      eax
_no_control:
		mov      eax, 004496A0h
		call     eax
		mov      eax, 00432E04h
		jmp      eax
	}
}*/

#pragma optimize( "s", on )
void setTestHook()
{
	//setHook((void*)0x005D0D59, repair_test);
	//setHook((void*)0x00432DFF, onSync);

	//setHook((void*)0x00542870, loadDRSHookT);
	//timeGetTime
	//interceptTime();
	//interceptTextOut();

	//setHook((void*)0x0060BCFF, onScroll);
	//setHook((void*)0x0060BE1C, onScroll2);
	//writeDword(0x0060BDD1, (DWORD)my_ftol_wr - 0x0060BDD5);

	//_beginthread(thread_scroll, 0, 0);
	//

	//RMS token count
	//setHook((void*)0x004E1D40, onRmsToken);
	//
	//setHook((void*)0x0063329E, malloc_hook);
	//setHook((void*)0x00632D33, malloc2_hook);
	//str = (char*) malloc (2000);

	//setHook ((void*)0x5E02B8, &someText);

#ifdef _DEBUG
	setHook((void*)0x005F54D2, triggerLogHook);
	setHook((void*)0x005F4A77, effectLogHook);
#endif

	//fix internal log
	writeDword(0x0040F1A6, (DWORD)nullsub_26);
	writeDword(0x0040F842, (DWORD)nullsub_26);
	writeDword(0x0040F885, (DWORD)nullsub_26);
	writeDword(0x0040F75B, (DWORD)nullsub_26);
	writeDword(0x0040EFBB, (DWORD)nullsub_26);
	writeDword(0x0057796E, (DWORD)nullsub_26);
	writeDword(0x00577A63, (DWORD)nullsub_26);
	writeDword(0x00577A89, (DWORD)nullsub_26);
	writeDword(0x00577AAF, (DWORD)nullsub_26);

	fix_function_call(0x0059B18B, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B1C6, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B1EA, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B4BE, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B524, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B56D, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B5AC, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B5D0, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B63D, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B675, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B73C, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B827, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B870, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B8AF, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B8C7, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B947, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059B9D0, (DWORD)nullsub_26); //Entering task
	fix_function_call(0x0059BACB, (DWORD)nullsub_26); //Entering task
	//
	writeByte(0x004428F6, 0x90);
	writeDword(0x004428F7, 0x90909090);
	writeByte(0x0061B129, 0x90);
	writeDword(0x0061B12A, 0x90909090);
	writeByte(0x005B944D, 0x90);
	writeDword(0x005B944E, 0x90909090);
	writeByte(0x005A05DD, 0x90);
	writeDword(0x005A05DE, 0x90909090);
	writeByte(0x0050D0F3, 0x90);
	writeDword(0x0050D0F4, 0x90909090);
	writeDword(0x00410D38, 0x90909090);
	writeByte(0x00410D3C, 0x90);
	writeByte(0x0042520B, 0x90);
	writeDword(0x0042520C, 0x90909090);
	writeDword(0x004776BE, 0x90909090);
	writeByte(0x004776C2, 0x90);
	setHook((void*)0x0060F920, log_int_wr);

	setHook((void*)0x004E1801, rms_fptr);
	setHook((void*)0x004E1951, rms_fptr_close);

	setHook((void*)0x00438140, onChat);

	//
#ifdef _DEBUG
	setHook((void*)0x004D5550, readDatHook);
#endif
}
#pragma optimize( "", on )
