#pragma once

struct UNIT_MASTER_EXTRA
{

};

void setUnitMasterHooks();

UNIT_MASTER_EXTRA* __fastcall get_unit_master_extra(prop_object* master);
void __fastcall add_unit_master_extra(prop_object* master, UNIT_MASTER_EXTRA* ex);
