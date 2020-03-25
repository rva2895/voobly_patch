#include "windows.h"

#include "iuserpatch.h"

#include "flare.h"
#include "filenames.h"
#include "terrain_gen.h"
#include "votepanel.h"
#include "timeline.h"
#include "scroll.h"
#include "gamespeed.h"
#include "savegamever.h"
#include "techupcolor.h"
#include "wndproc.h"
#include "rec.h"
#include "recbrowse.h"
#include "elevation.h"
#include "network.h"
#include "hotkeyjump.h"
#include "recordrestore.h"
#include "mouseoverride.h"
#include "hotkey.h"
#include "minimap.h"
#include "notify.h"

#include "mg1search.h"

class CUserPatch : public IUserPatch
{
public:
	virtual bool Init(struct UserPatchConfig_t &config);
	virtual bool OnChatMessage(const char *text);
};

IVoobly *g_pVoobly = NULL;
CUserPatch g_UserPatch;

#ifndef INTEGRATED_USERPATCH
extern "C" __declspec(dllexport) 
#endif
void GetUPInterface(IUserPatch **ppUserPatch, IVoobly *pVoobly)
{
	g_pVoobly = pVoobly;
	*ppUserPatch = &g_UserPatch;
}

bool dataPatch = false;

//

/*int __stdcall Intercept_DrawTextA(HDC hdc, LPCSTR s, int c, LPRECT r, UINT format)
{
	//DrawTextA()
	//DWORD written;
	//WriteProcessMemory(GetCurrentProcess(), (void*)adr_text,
	//	(void*)old_text, 6, &written);

	if (c == -1)
		c = strlen(s);

	int x = r->left;
	int y = r->top;
	if (format & DT_BOTTOM)
		y = r->bottom - 15;

	//if (strstr(s, "should probably"))
	//	__debugbreak();

	int retval;
	if (format & DT_RIGHT)
		retval = DrawTextA(hdc, s, c, r, format);
	else
	{
		char s2[0x200];
		strcpy(s2+1, s);
		s2[0] = '$';
		retval = TextOutA(hdc, x, y, s2, c);
	}

	//WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
	//	(void*)jump_text, 6, &written);

	return retval;
}

//

void __stdcall write_call(DWORD at, DWORD new_addr)
{
	g_pVoobly->Write(at, "E8"); //call
	g_pVoobly->Write(at + 1, new_addr - (at + 5));
}

__declspec(naked) void sub_00438140_fix()
{
	__asm
	{
		sub     esp, 118h
		mov     eax, 00438146h
		jmp     eax
	}
}*/

bool keydown_hotkeys = false;
bool dark_grey = false;

bool CUserPatch::Init(struct UserPatchConfig_t &config)
{
	// fetch settings
	HKEY hKeyCU;
	HKEY hKey;
	unsigned long disposition;
	unsigned long type, size = 0x100;

	char str[0x100];

	long regResult = RegOpenCurrentUser(KEY_QUERY_VALUE, &hKeyCU);
	regResult = RegCreateKeyEx(
		hKeyCU,
		REGPATH,
		0,
		0,
		0,
		KEY_QUERY_VALUE,
		0,
		&hKey,
		&disposition);
	if (regResult == 0)
	{
		if (regResult = RegQueryValueEx(
			hKey,
			"Keydown Object Hotkeys",
			0,
			&type,
			(BYTE*)str,
			&size))
			keydown_hotkeys = false;
		else
		{
			strupr(str);
			if (strstr(str, "TRUE"))
				keydown_hotkeys = true;
			else if (strstr(str, "FALSE"))
				keydown_hotkeys = false;
			else
				keydown_hotkeys = false;
		}
		if (regResult = RegQueryValueEx(
			hKey,
			"Dark Minimap Grey",
			0,
			&type,
			(BYTE*)str,
			&size))
			dark_grey = false;
		else
		{
			strupr(str);
			if (strstr(str, "TRUE"))
				dark_grey = true;
			else if (strstr(str, "FALSE"))
				dark_grey = false;
			else
				dark_grey = false;
		}
		RegCloseKey(hKey);
		RegCloseKey(hKeyCU);
	}
	else
		g_pVoobly->Log("Failed to query settings! Cannot open key.");

	g_pVoobly->Log("Settings: Keydown Object Hotkeys = %s, Dark Minimap Grey = %s",
		keydown_hotkeys ? "true" : "false", dark_grey ? "true" : "false");

	// Write 2.2 exe version string	
	g_pVoobly->Write(0x689BA4, "322E32");

	setFileNameHooks();
	if (strstr(config.VooblyModDirPath, "Data Patch"))
	{
		g_pVoobly->Log("Data patch is ON");
		dataPatch = true;
		setTerrainGenHooks();
		setSaveGameVerHooks(true);
	}
	else
	{
		setSaveGameVerHooks(false);
		g_pVoobly->Log("Data patch is OFF");
	}

	//
	setFlareHooks();
	//

	setVotePanelHooks();
	setGameSpeedHooks();
	setTimelineHooks();
	setScrollHooks();
	setTechUpColorHooks();
	setWndProcHooks();
	setRecHooks();
	setElevationHooks();
	setRecBrowseHooks();
	setNetworkHooks();
	setHotkeyJumpHooks();
	setRecordRestoreHooks();
	setMouseOverrideHooks();
	setNotifyHooks();
	if (keydown_hotkeys)
		setHotkeyHooks();
	if (dark_grey)
		setMinimapHooks();

	//setTerrainGenHooks_v2();

	//UI bar update
	g_pVoobly->Write(0x005DDBA4, 100);
	g_pVoobly->Write(0x005DDB73, "9090");
	g_pVoobly->Write(0x005DDB7B, 100);

	//chat display time
	g_pVoobly->Write(0x004CCAD0, 15000);

	//MP mouse lag
	g_pVoobly->Write(0x0049F686, "0C");
	g_pVoobly->Write(0x0049F906, "9090");
	g_pVoobly->Write(0x00429541, "EB");

	//wndproc loop delay
	writeDword(0x00425EA6, 0x0674023C);
	writeDword(0x00425EAA, 0x013C042C);
	writeByte(0x00425EAE, 0x77);

	// Apply patches from bin2cpp tool
	bool bSuccess = true;//ApplyPatchList();

	// Write DLL version to Voobly log			
	g_pVoobly->Log(USERPATCH_VERSION);

	//draw text fix
	//g_pVoobly->Write(0x00654304, (DWORD)&Intercept_DrawTextA);

	//g_pVoobly->Write(0x00438140, "81EC18010000");
	/*write_call(0x00438073, (DWORD)&sub_00438140_fix);
	write_call(0x0043808E, (DWORD)&sub_00438140_fix);
	write_call(0x004380AE, (DWORD)&sub_00438140_fix);
	write_call(0x00438104, (DWORD)&sub_00438140_fix);
	write_call(0x00438114, (DWORD)&sub_00438140_fix);
	write_call(0x00438133, (DWORD)&sub_00438140_fix);
	write_call(0x004407A5, (DWORD)&sub_00438140_fix);*/

	//g_pVoobly->Write(0x00438000, "C20400");
	//g_pVoobly->Write(0x004380C0, "C20400");

	//g_pVoobly->Write(0x004CD3B0, "C3");

	//setMg1SearchHooks();

	return bSuccess;
}

bool CUserPatch::OnChatMessage(const char *text)
{
	if (!strcmp(text, "/upversion"))
	{
		char* s_dataPatch = dataPatch ? "ON" : "OFF";
		char* s_keydown_hotkeys = keydown_hotkeys ? "ON" : "OFF";
		char* s_dark_grey = dark_grey ? "ON" : "OFF";
		g_pVoobly->ChatMessage("UserPatch", "%s, Data patch: %s",
			USERPATCH_VERSION, s_dataPatch);
		g_pVoobly->ChatMessage("UserPatch", "Keydown hotkeys: %s, Dark Minimap Grey: %s",
			s_keydown_hotkeys, s_dark_grey);
		return true;
	}

	return false;
}
