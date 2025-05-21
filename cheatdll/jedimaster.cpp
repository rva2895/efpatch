#include "stdafx.h"
#include "jedimaster.h"

__int16* masters_list = NULL;
int masters_n = 0;
bool masters_installed = false;

__int16* padawans_list = NULL;
int padawans_n = 0;
bool padawans_installed = false;

bool __fastcall RGE_Static_Object__gbg_isMaster_new(RGE_Static_Object* obj)
{
    if (obj->master_obj->object_group == 50)
        return is_id_in_list(obj->master_obj->copy_id, masters_list, masters_n);
    else
        return false;
}

bool __fastcall RGE_Static_Object__gbg_isPadawan_new(RGE_Static_Object* obj)
{
    if (obj->master_obj->object_group == 50)
        return is_id_in_list(obj->master_obj->copy_id, padawans_list, padawans_n);
    else
        return false;
}

void __cdecl masters_hooks()
{
    setHook((void*)0x0054B1D0, RGE_Static_Object__gbg_isMaster_new);
}

void __cdecl padawans_hooks()
{
    setHook((void*)0x0054B170, RGE_Static_Object__gbg_isPadawan_new);
}

void setJediMasterHooks(const char* prefix, const char* filename_master, const char* filename_padawan)
{
    load_ids_from_txt(
        prefix,
        filename_master,
        &masters_list,
        &masters_n,
        &masters_installed,
        masters_hooks,
        "Loading jedi master unit list"
    );

    load_ids_from_txt(
        prefix,
        filename_padawan,
        &padawans_list,
        &padawans_n,
        &padawans_installed,
        padawans_hooks,
        "Loading jedi padawan unit list"
    );
}
