#pragma once

int terrain1();
int terrain2();

void setExtraTerrainHooks();
void setExtraTerrainHooks_CC();

void setTerrainLoadHooks(int);
void loadTerrainTxt(const char* prefix, const char* filename);

void setTerrainBlendHooks();
