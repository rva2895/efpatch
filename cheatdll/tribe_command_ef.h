#pragma once

void TRIBE_Command__do_command_ef();
void setTribeCommandEFHooks();
void make_cheat_by_id(RGE_Player* player, int ef_cheat_id);
void make_cheat_by_id_with_unit_list(RGE_Player* player, int ef_cheat_id, RGE_Static_Object** units, int n);
void make_cheat_by_id_with_position(RGE_Player* player, int ef_cheat_id, float x, float y);

//cheats

//fixed old cheats
#define EF_CHEAT_SIMONSAYS 0x30
#define EF_CHEAT_SCARYNEIGHBOR 0x31
#define EF_CHEAT_THATS_NO_MOON 0x32
#define EF_CHEAT_IMPERIAL_ENTANGLEMENTS 0x33
#define EF_CHEAT_TANTIVE_IV 0x34
#define EF_CHEAT_GALACTIC_UPHEAVAL 0x35

//new cheats
#define EF_CHEAT_FORCEHEAL 0x36
#define EF_CHEAT_FORCEPROTECT 0x37
#define EF_CHEAT_ULTIMATE_POWER_IN_THE_UNIVERSE 0x38
#define EF_CHEAT_HELP_ME_OBIWAN 0x39
#define EF_CHEAT_NOW_THIS_IS_PODRACING 0x3A
#define EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME 0x3B
#define EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG 0x3C
#define EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE 0x3D
#define EF_CHEAT_UNLIMITED_POWER 0x3E
#define EF_CHEAT_KOELSCH 0x3F
#define EF_CHEAT_JOIN_US_OR_DIE 0x40
#define EF_CHEAT_DEPLOY_THE_GARRISON 0x41
#define EF_CHEAT_THERES_ALWAYS_A_BIGGER_FISH 0x42
