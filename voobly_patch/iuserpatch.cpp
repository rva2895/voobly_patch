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

bool CUserPatch::Init(struct UserPatchConfig_t &config)
{
	// Write 2.1 exe version string	
	g_pVoobly->Write(0x689BA4, "322E31");

	setFileNameHooks();
	if (strstr(config.VooblyModDirPath, "Data Patch"))
	{
		g_pVoobly->Log("Data patch is ON");
		dataPatch = true;
		setFlareHooks();
		setTerrainGenHooks();
		setSaveGameVerHooks(true);
	}
	else
	{
		setSaveGameVerHooks(false);
		g_pVoobly->Log("Data patch is OFF");
	}

	setVotePanelHooks();
	setGameSpeedHooks();
	setTimelineHooks();
	setScrollHooks();
	setTechUpColorHooks();

	// Apply patches from bin2cpp tool
	bool bSuccess = true;//ApplyPatchList();

	// Write DLL version to Voobly log			
	g_pVoobly->Log(USERPATCH_VERSION);

	return bSuccess;
}

bool CUserPatch::OnChatMessage(const char *text)
{
	if (!strcmp(text, "/upversion"))
	{
		char* str = dataPatch ? "ON" : "OFF";
		g_pVoobly->ChatMessage("UserPatch", "%s, Data patch: %s", USERPATCH_VERSION, str);
		return true;
	}

	return false;
}
