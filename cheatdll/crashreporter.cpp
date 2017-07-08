#include "stdafx.h"

#include <CrashRpt.h>
#include <DbgHelp.h>
#include <string>

crash_rpt::CrashRpt* g_crashRpt;

extern std::wstring logFileName;

crash_rpt::CrashProcessingCallbackResult CALLBACK crash_func(
	crash_rpt::CrashProcessingCallbackStage stage_,	//!< Current crash processing stage.
	crash_rpt::ExceptionInfo* exceptionInfo_,		//!< Information about exception being processed.
	LPVOID	userData_					//!< Pointer to user-defined data.
)
{
	crash_rpt::CrashProcessingCallbackResult r = crash_rpt::DoDefaultActions;
	log("Crashed. Sending error report.");
	flushLog();
	return r;
}

void initCrashReporter()
{
#ifndef _DEBUG
	crash_rpt::ApplicationInfo ai;
	ai.ApplicationInfoSize = sizeof(crash_rpt::ApplicationInfo);
	//ai.ApplicationGUID = "2106E204-7AC2-4444-829A-483CAF512259";
	ai.ApplicationGUID = "c2738b8f-cb34-4da0-9d77-be2c8ea8bae0";
	ai.Prefix = NULL;
	ai.AppName = L"Star Wars Galactic Battlegrounds: Expanding Fronts";
	ai.Company = L"Expanding Fronts Development Team";
	USHORT version[4] = { 1, 0, 0, 2 };
	ai.V[0] = version[0]; ai.V[1] = version[1]; ai.V[2] = version[2]; ai.V[3] = version[3];
	ai.Hotfix = 0;
	ai.PrivacyPolicyUrl = NULL;

	MINIDUMP_TYPE mt = (MINIDUMP_TYPE)(MiniDumpNormal
		| MiniDumpWithProcessThreadData
		| MiniDumpWithHandleData);


	crash_rpt::custom_data_collection::Settings s;
	s.SettingsSize = sizeof(crash_rpt::custom_data_collection::Settings);
	s.CollectionFunctionName = NULL;
	s.CollectionLibraryPath = NULL;
	s.UserData = NULL;
	s.UserDataSize = NULL;

	crash_rpt::HandlerSettings hs;
	hs.HandlerSettingsSize = sizeof(crash_rpt::HandlerSettings);
	hs.LeaveDumpFilesInTempFolder = false;
	hs.OpenProblemInBrowser = false;
	hs.UseWER = false;
	hs.SubmitterID = 0;
	hs.SendAdditionalDataWithoutApproval = true;
	hs.OverrideDefaultFullDumpType = true;
	hs.FullDumpType = mt;
	hs.LangFilePath = NULL;
	hs.SendRptPath = NULL;
	hs.DbgHelpPath = NULL;
	hs.CrashProcessingCallback = &crash_func;
	hs.CrashProcessingCallbackUserData = NULL;
	hs.CustomDataCollectionSettings = &s;

	g_crashRpt = new crash_rpt::CrashRpt(
		&ai,
		&hs,
		false
	);
#ifndef _CHEATDLL_NOLOG

	wchar_t fullPath[0x400];
	GetFullPathNameW(logFileName.c_str(), 0x200, fullPath, 0);
	g_crashRpt->AddFileToReport(fullPath, NULL);

	log("Crash reporter initialised");
#endif

#endif
}
