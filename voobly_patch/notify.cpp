#include "iuserpatch.h"
#include "notify.h"

__declspec(naked) void notify4() //00435825
{
	__asm
	{
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     eax, [ecx + 1DCh]
		cmp     eax, 5
		jnz     short loc_7C91CC
		mov     edx, ebx
		mov     ecx, 006A35E0h
		mov     ecx, [ecx]
		push    edx
		mov     eax, 0043B5A0h
		call    eax
		test    eax, eax
		jz      short loc_7C91CC
		sub     esp, 110h
		mov     [esp], eax
		push    0BC4h
		mov     eax, 004B76F0h
		call    eax
		mov     ecx, [esp]
		lea     edx, [esp + 10h]
		push    ecx
		push    eax
		push    edx
		mov     eax, 006330C4h
		call    eax
		add     esp, 0Ch
		lea     eax, [esp + 10h]
		push    1
		push    eax
		mov     eax, 004F9AF0h
		call    eax
		add     esp, 110h

loc_7C91CC:
		mov     [esi + ebx * 4 + 13C0h], edi
		mov     eax, 0043582Ch
		jmp     eax
	}
}

void setNotifyHooks()
{
	setHook((void*)0x00435825, notify4);
}
