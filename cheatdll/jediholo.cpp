#include "stdafx.h"
#include "jediholo.h"

struct jedi_holo_ids
{
    short id1;
    short id2;
};

int jediHoloN = 0;
jedi_holo_ids* jediHoloIDs = NULL;

bool jedi_holo_new_hooks_installed = false;

int __fastcall get_holo_id(int id)
{
    for (int i = 0; i < jediHoloN; i++)
        if (jediHoloIDs[i].id1 == id)
            return jediHoloIDs[i].id2;
    return id;
}

int __fastcall get_non_holo_id(int id)
{
    for (int i = 0; i < jediHoloN; i++)
        if (jediHoloIDs[i].id2 == id)
            return jediHoloIDs[i].id1;
    return id;
}

void setJediHoloHooks(const char* prefix, const char* filename)
{
    log("Loading jedi-holo unit list");
    char full_filename[0x100];
    snprintf(full_filename, _countof(full_filename), "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        short id1, id2;
        jediHoloN = 0;
        if (jediHoloIDs)
        {
            free(jediHoloIDs);
            jediHoloIDs = NULL;
        }

        while (fscanf(f, "%hd-%hd", &id1, &id2) > 0)
        {
            jediHoloN++;
            jediHoloIDs = (jedi_holo_ids*)realloc(jediHoloIDs, jediHoloN * sizeof(jedi_holo_ids));
            jediHoloIDs[jediHoloN - 1].id1 = id1;
            jediHoloIDs[jediHoloN - 1].id2 = id2;
        }

        fclose(f);
        if (!jedi_holo_new_hooks_installed)
        {
            setHook((void*)0x005E3EE0, get_holo_id);
            setHook((void*)0x005E3F10, get_non_holo_id);

            jedi_holo_new_hooks_installed = true;
        }
    }
    else
        log("Warning: %s not found, using default settings", full_filename);
}
