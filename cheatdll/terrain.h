#pragma once

int terrain1();
int terrain2();

void setExtraTerrainHooks();
void setExtraTerrainHooks_CC();

void setTerrainLoadHooks(int);
void loadTerrainTxt(const wchar_t* prefix, const wchar_t* filename);

void setTerrainBlendHooks();
