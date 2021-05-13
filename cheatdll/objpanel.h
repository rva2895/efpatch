#pragma once

void setObjectPanelHooks();

//int (__thiscall* objPanelDrawItem)
//    (void*, int level, int slpFrame, int displayType, int v1, int v2, int v3, int v4, int langDLLDesc);

void __cdecl objPanel(UNIT* unit);
void __stdcall objPanel_invalidate();
