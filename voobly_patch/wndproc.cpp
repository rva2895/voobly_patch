#include "iuserpatch.h"
#include "wndproc.h"
#include "rec.h"

extern IVoobly *g_pVoobly;

HWND hWnd_global;

extern bool rec_cache_invalid;

__declspec(naked) void __stdcall update_window(void* wnd)
{
	__asm
	{
		mov     ecx, [esp + 4]
		mov     eax, [ecx + 18h]
		mov     edx, [ecx + 14h]
		//push    eax
		//push    edx
		//mov     eax, [ecx]
		//call    dword ptr[eax + 5Ch]

		mov     ecx, [esp + 4]
		push    1
		mov     eax, [ecx]
		call    dword ptr[eax + 2Ch]
		ret     4
	}
}

int (CALLBACK* WndProc_exe) (HWND, UINT, WPARAM, LPARAM) =
	(int (CALLBACK*) (HWND, UINT, WPARAM, LPARAM)) 0x00426530;

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	hWnd_global = hWnd;

	if (msg == WM_SYSKEYDOWN)
	{
		
		if ((LOWORD(wParam) >= '1') && (LOWORD(wParam) <= '9')) //rec switch player
		{
			//if (short x = GetKeyState(VK_SHIFT))
			//{
				recSwitch(LOWORD(wParam) - 0x30);
			//}
		}
	}

	if (msg == WM_APP + 1000) //updatebk
	{
		if (!rec_cache_invalid)
			update_window((void*)wParam);
	}
	
	return WndProc_exe(hWnd, msg, wParam, lParam);
}

void setWndProcHooks()
{
	g_pVoobly->Write(0x00426509, (DWORD)&WndProc_dll);
}
