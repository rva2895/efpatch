#include "stdafx.h"

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

#include "rundll.h"

#ifdef TARGET_VOOBLY

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
bool expanding_fronts = false;

void* voob_log = 0;

bool CUserPatch::Init(struct UserPatchConfig_t &config)
{
	// Write DLL version to Voobly log			
	g_pVoobly->Log(USERPATCH_VERSION);

	// Write 2.2 exe version string	
	g_pVoobly->Write(0x689BA4, "322E32");

	if (strstr(config.VooblyModDirPath, "Data Patch"))
	{
		g_pVoobly->Log("Data patch is ON");
		dataPatch = true;
		setTerrainGenHooks();
		setSaveGameVerHooks(true);
	}
	else if (strstr(config.VooblyModDirPath, "Expanding Fronts"))
	{
		expanding_fronts = true;
		setSaveGameVerHooks(false);
		g_pVoobly->Log("Running in EF mode");
	}
	else
	{
		setSaveGameVerHooks(false);
		g_pVoobly->Log("Data patch is OFF");
	}

	// Write 2.2 exe version string	
	/*g_pVoobly->Write(0x689BA4, "322E32");

	setFileNameHooks(false);
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

	//setTerrainGenHooks_v2();

	//UI bar update
	g_pVoobly->Write(0x005DDBA4, 100);
	g_pVoobly->Write(0x005DDB73, "9090");
	g_pVoobly->Write(0x005DDB7B, 100);

	//chat display time
	g_pVoobly->Write(0x004CCAD0, 15000);*/

	initialSetup();

	// Apply patches from bin2cpp tool
	bool bSuccess = true;//ApplyPatchList();

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
#endif