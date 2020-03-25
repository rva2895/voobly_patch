#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

void* (__thiscall* global_getCurrentPlayer) (void* globalPtr) =
	(void* (__thiscall*) (void*))0x00428750;

void(__thiscall* player_clearSelection) (void* player) =
	(void(__thiscall*) (void*))0x004C3050;

int(__thiscall* player_scrollView)(void *player, float x, float y, int unk) =
	(int(__thiscall*) (void*, float, float, int))0x004C2010;

void* getCurrentPlayer()
{
	return global_getCurrentPlayer(*(void**)0x006A3684);
}

void writeDword(DWORD a, DWORD d)
{
	g_pVoobly->Write(a, d);
}

void writeWord(DWORD a, WORD d)
{
	g_pVoobly->Write(a, &d, sizeof(WORD));
}

void writeByte(DWORD a, BYTE d)
{
	g_pVoobly->Write(a, &d, sizeof(BYTE));
}

void setHook(void* a, void* d)
{
	g_pVoobly->WriteJump((DWORD)a, d);
}
