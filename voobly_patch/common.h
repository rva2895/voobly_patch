#pragma once

void* getCurrentPlayer();

extern void (__thiscall* player_clearSelection) (void*);

void writeDword(DWORD, DWORD);
void writeWord(DWORD, WORD);
void writeByte(DWORD, BYTE);
void setHook(void*, void*);

int __stdcall isRec();
