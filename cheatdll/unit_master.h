#pragma once

struct UNIT_MASTER_EXTRA
{

};

void setUnitMasterHooks();

UNIT_MASTER_EXTRA* __fastcall get_unit_master_extra(RGE_Master_Static_Object* master);
void __fastcall add_unit_master_extra(RGE_Master_Static_Object* master, UNIT_MASTER_EXTRA* ex);
