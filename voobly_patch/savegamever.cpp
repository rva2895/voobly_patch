#include "iuserpatch.h"
#include <stdio.h>

extern IVoobly *g_pVoobly;

__declspec(naked) void __stdcall errorScreen
(char* s, int unk0, int x, int y, int unk1, void* ptr)
{
	__asm
	{
		mov     ecx, [esp + 18h]
		mov     eax, [esp]          //old ret addr
		mov     [esp + 18h], eax    //save in unused var
		add     esp, 4
		mov     eax, 4BB250h
		call    eax
		pop     eax                 //restore old ret addr
		jmp     eax
	}
}

char ver[4];

void __stdcall displayVersionError(void* ptr)
{
	char buf[0x100];
	ver[3] = 0;
	if (!strcmp(ver, "9.4"))
		strcpy(ver, "1.1");
	sprintf(buf, "This file was recorded with a different version of the game\n"
		"Current version: " EXE_PATCH_VERSION ", "
		"File version: %s",
		ver);

	errorScreen(buf, 0, 450, 100, 1, ptr);
}

__declspec(naked) void onResult() //0050AD06
{
	__asm
	{
		test    eax, eax
		jnz     _no_error
		push    ebx
		call    displayVersionError
_no_error:
		mov     ebx, 0050AE72h
		jmp     ebx
	}
}

__declspec(naked) void onGetSaveGameVersion() //0061D9A5
{
	__asm
	{
		mov     esi, 0069E564h
		lea     eax, [esp + 1Ch]
		mov     edx, [eax]
		mov     ecx, offset ver
		mov     [ecx], edx
		sub     eax, 4
		mov     edx, 0061D9AEh
		jmp     edx
	}
}

void setSaveGameVerHooks(bool datap)
{
	if (datap)
		g_pVoobly->Write(0x0069E568, EXE_PATCH_VERSION, strlen(EXE_PATCH_VERSION)+1);
	g_pVoobly->WriteJump(0x0050AD06, onResult);
	g_pVoobly->WriteJump(0x0061D9A5, onGetSaveGameVersion);
}
