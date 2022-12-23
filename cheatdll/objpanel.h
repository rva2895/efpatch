#pragma once

void setObjectPanelHooks(int version);

void __cdecl objPanel(RGE_Static_Object* unit);
void __stdcall objPanel_invalidate();
