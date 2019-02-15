#include "techupcolor.h"
#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall sendTechChat2(int player, char* s, int unk)
{
	__asm
	{
		mov     eax, [esp + 0Ch]  //unk
		mov     ecx, [esp + 8]    //str
		mov     edx, [esp + 4]    //player
		push    0
		push    eax
		push    edx
		push    ecx
		mov     ecx, 006A35D8h
		mov     ecx, [ecx]
		push    0
		mov     eax, 0042D5E0h
		call    eax
		retn    0Ch
	}
}
#pragma warning(pop)

__declspec(naked) void onTechUpSend() //005E9990
{
	__asm
	{
		push    edi
		call    sendTechChat2
		mov     eax, 005E9995h
		jmp     eax
	}
}

void setTechUpColorHooks()
{
	g_pVoobly->WriteJump(0x005E9990, onTechUpSend);
}
