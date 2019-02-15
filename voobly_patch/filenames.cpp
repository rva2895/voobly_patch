#include <time.h>
#include "filenames.h"
#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

char recCC [] = "recorded game - %Y-%m-%d_%H-%M-%S.mg1";
char timeOnly [] = "%Y-%m-%d_%H-%M-%S";

void __stdcall printTime(char* s, const char* fmt)
{
	time_t rawtime;
	tm* tm_time;
	rawtime = time(0);
	tm_time = localtime(&rawtime);
	strftime(s, 256, fmt, tm_time);
}

__declspec(naked) void onRecFileName() //005ED479
{
	__asm
	{
		mov     ecx, offset recCC
		push    ecx
		lea     ecx, [esp+10h]
		push    ecx
		call    printTime
		mov     edx, [esi+24h]
		push    005ED53Eh
		ret
	}
}

__declspec(naked) void onMPSaveFileName() //00433845
{
	__asm
	{
		mov     eax, offset timeOnly
		push    eax
		push    ecx
		call    printTime
		add     esp, 8
		push    00433853h
		ret
	}
}

void setFileNameHooks() //00433877 - mp save filename
{
	g_pVoobly->WriteJump(0x005ED479, onRecFileName);
	g_pVoobly->WriteJump(0x00433845, onMPSaveFileName);
}
