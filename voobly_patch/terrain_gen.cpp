#include "terrain_gen.h"
#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

__declspec(naked) void onChangeTerrain2() //00557E45
{
	__asm
	{
		cmp     cl, 4             //swamp
		jnz     _terrain_change
		cmp     dl, 7             //farms
		jz      _terrain_change
		cmp     dl, 8
		jz      _terrain_change
		cmp     dl, 1Dh
		jz      _terrain_change
		cmp     dl, 1Eh
		jz      _terrain_change
		cmp     dl, 1Fh
		jz      _terrain_change
		cmp     dl, 30
		jz      _terrain_change
		push    00557E57h
		ret
_terrain_change:
		mov     [eax], dl
		mov     esi, [esp+10h]
		push    00557E4Bh
		ret
	}
}

void setTerrainGenHooks()
{
	g_pVoobly->Write(0x00557DD6, "35");   //gungan foundation: 53

	g_pVoobly->WriteJump(0x00557E45, onChangeTerrain2);
}
