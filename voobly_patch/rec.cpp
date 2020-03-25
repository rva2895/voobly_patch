#include "rec.h"
#include "iuserpatch.h"

#include <stdio.h>

extern IVoobly *g_pVoobly;

__declspec(naked) int __stdcall isRec()
{
	__asm
	{
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     ecx, [ecx + 17B4h]
		test    ecx, ecx
		jz      _no_rec
		mov     ecx, [ecx + 126Ch]
		test    ecx, ecx
		jz      _no_rec
		mov     ecx, [ecx + 0Ch]
		test    ecx, ecx
		jz      _no_rec
		mov     edx, [ecx + 1614h]
		mov     eax, [edx + 4]
		test    eax, eax
		setg    al
		movzx   eax, al
		ret
_no_rec:
		xor     eax, eax
		ret
	}
}

__declspec(naked) void onMenuInit() //0045E425
{
	__asm
	{
		cmp     dword ptr [esp + 18h], 1
		jz      _contMenu
		call    isRec
		mov     [esp + 18h], eax
_contMenu:
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     eax, 0045E42Bh
		jmp     eax
	}
}

char* chatStr = (char*)0x007A20C0;

void __stdcall recPrintChat(bool our)
{
	char s[512];
	if (!our)
	{
		strcpy(s, chatStr);
		sprintf(chatStr + 3, "< %s >", s + 3);
	}
}

__declspec(naked) void onRecChat() //0061FCD0
{
	__asm
	{
		cmp     [edx + 538h], eax
		setz    al
		push    eax
		call    recPrintChat
		mov     eax, 0061FCD8h
		jmp     eax
	}
}

void setRecHooks()
{
	g_pVoobly->WriteJump(0x0045E425, onMenuInit);
	g_pVoobly->WriteJump(0x0061FCD0, onRecChat);
}

void recSwitch(int p)
{
	if (isRec())
	{
		player_clearSelection(getCurrentPlayer());
		takeControl(p);
	}
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall takeControl(int p)
{
	__asm
	{
		mov     eax, [esp + 4]
		push    esi
		push    edi
		push    ebp
		push    ebx
		mov     ecx, 6A3684h
		mov     ecx, [ecx]
		push    eax
		mov     edx, 005E68B0h //takeControl
		call    edx
		pop     ebx
		pop     ebp
		pop     edi
		pop     esi
		ret     4
	}
}
#pragma warning(pop)
