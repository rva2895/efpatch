#pragma once

void __stdcall editor_enter();
void __stdcall editor_exit();

void editor_autosave();

void setAutosaveHooks(int version, int interval);
