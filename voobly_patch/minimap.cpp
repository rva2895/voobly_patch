#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

#define DARK_GREY_COLOR 11

__declspec(naked) void grey_load_dat_unit() //0048C45F
{
	__asm
	{
		mov     dx, [edi + 18h]
		cmp     dx, 217
		jz      carbon_rock
		cmp     dx, 1117
		jz      carbon_rock
		cmp     dx, 1160
		jz      carbon_rock
		cmp     dx, 1341
		jnz     not_carbon_rock
carbon_rock:
		mov     byte ptr [edi + 9Eh], 0
not_carbon_rock:
		push    1
		lea     edx, [edi + 9Fh]
		mov     eax, 0048C467h
		jmp     eax
	}
}

__declspec(naked) void grey_load_dat_color() //0042E072
{
	__asm
	{
		mov     edx, [esi + 4]
		cmp     edx, 6
		jnz     not_grey
		mov     dword ptr [esi + 20h], DARK_GREY_COLOR
not_grey:
		push    4
		lea     edx, [esi + 24h]
		mov     eax, 0042E077h
		jmp     eax
	}
}

void setMinimapHooks()
{
	setHook((void*)0x0048C45F, grey_load_dat_unit);
	setHook((void*)0x0042E072, grey_load_dat_color);
}
