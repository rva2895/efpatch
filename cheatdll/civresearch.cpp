#include "stdafx.h"
#include "civresearch.h"

uint8_t* tableR;

#define RESEARCH_COUNT_FOR_CIVRESEARCH 0x600

const ID_ASSIGNR idAssignR[] =
{
    //  GE,    GN,    RA,    RN,    TF,    WK,    RP,    CF,    ZC,    GO,    FO,    RS,    IR,    NR,    BS
    { 0x83,  0xB6, 0x11E,  0xC8,  0xA5, 0x11F,  0x74,  0x79, 0x269,   746,   817,   877,  1005,  1055,  1105}, //AVAIL-ACRUC
    {0x1A0, 0x1A1, 0x1A3, 0x1A2,  0x35, 0x1A4, 0x219, 0x1FC, 0x290,   783,   854,   914,  1044,  1094,  1144}, //ut-boat-laser-3
    {0x1A5, 0x1A6, 0x1A8, 0x1A7,  0x33, 0x1A9, 0x1FD, 0x1FE, 0x293,   786,   857,   917,  1047,  1097,  1147}, //ut-boat-artillery-2
    {0x1AA, 0x1AB, 0x1AD, 0x1AC,  0x32, 0x1AE, 0x1FF, 0x200, 0x291,   784,   855,   915,  1045,  1095,  1145}, //ut-boat-cutlaser-2
    {0x1AF, 0x1B0, 0x1B2, 0x1B1,  0x31, 0x1B3, 0x201, 0x202, 0x292,   785,   856,   916,  1046,  1096,  1146}, //ut-boat-missile-2
    {0x1B9, 0x1BA, 0x1BC, 0x1BB,  0x34, 0x1BD, 0x203, 0x204, 0x28F,   782,   853,   913,  1043,  1093,  1143}, //ut-boat-laser-2
    {0x16E, 0x16F, 0x171, 0x170,  0x6C, 0x172, 0x205, 0x206, 0x28B,   778,   849,   909,  1039,  1089,  1139}, //ut-bomber-3
    {0x173, 0x174, 0x176, 0x175,  0x6E, 0x177, 0x207, 0x208, 0x289,   776,   847,   907,  1037,  1087,  1137}, //ut-fighter-3
    {0x178, 0x179, 0x17B, 0x17A,  0x6B, 0x17C, 0x209, 0x20A, 0x28A,   777,   848,   908,  1038,  1088,  1138}, //ut-bomber-2
    {0x17D, 0x17E, 0x180, 0x17F,  0x6D, 0x181, 0x20B, 0x20C, 0x288,   775,   846,   906,  1036,  1086,  1136}, //ut-fighter-2
    {0x14B, 0x14C, 0x14E, 0x14D,  0x39, 0x14F, 0x21A, 0x21B, 0x28D,   780,   851,   911,  1041,  1091,  1141}, //ut-artillery-2
    {0x155, 0x156, 0x158, 0x157,  0x38, 0x159, 0x21C, 0x21D, 0x28E,   781,   852,   912,  1042,  1092,  1142}, //ut-missile-launcher-2
    {0x15A, 0x15B, 0x15D, 0x15C,  0x3B, 0x15E, 0x21E, 0x21F, 0x28C,   779,   850,   910,  1040,  1090,  1140}, //ut-ram-2
    { 0xDE,  0xDF,  0xE1,  0xE0,  0x5D,  0xE2, 0x220, 0x221, 0x2A4,   791,   862,   922,   991,   994,   997}, //ut-jedi-2
    { 0xE3,  0xE4,  0xE5,  0xE6,  0x5E,  0xE7, 0x222, 0x223, 0x2A5,   792,   863,   923,   992,   995,   998}, //ut-jedi-3
    {0x12D, 0x12E, 0x130, 0x12F,  0x2A, 0x131, 0x226, 0x227, 0x285,   772,   843,   903,  1033,  1083,  1133}, //ut-single-mech-2
    {0x132, 0x133, 0x135, 0x134,  0x28, 0x136, 0x228, 0x229, 0x286,   773,   844,   904,  1034,  1084,  1134}, //ut-dual-mech-2
    {0x137, 0x138, 0x13A, 0x139,  0x63, 0x13B, 0x22A, 0x22B, 0x287,   774,   845,   905,  1035,  1085,  1135}, //ut-multi-mech-2
    { 0xEC,  0xED,  0xEF,  0xEE,  0x56,  0xF0, 0x22C, 0x22D, 0x284,   771,   842,   902,  1031,  1081,  1131}, //ut-mounted-3
    { 0xF1,  0xF2,  0xF4,  0xF3,  0x37,  0xF5, 0x22E, 0x22F, 0x283,   770,   841,   901,  1030,  1080,  1130}, //ut-mounted-2
    { 0xF6,  0xF7,  0xF9,  0xF8,  0x2D,  0xFA, 0x230, 0x231, 0x282,   769,   840,   900,  1032,  1082,  1132}, //ut-missile-troop-2
    { 0xFB,  0xFC,  0xFE,  0xFD,  0x2C,  0xFF, 0x232, 0x233, 0x280,   767,   838,   898,  1028,  1078,  1128}, //ut-laser-3
    {0x100, 0x101, 0x103, 0x102,  0x5A, 0x104, 0x234, 0x235, 0x281,   768,   839,   899,  1029,  1079,  1129}, //ut-laser-4
    {0x105, 0x106, 0x108, 0x107,  0x2B, 0x109, 0x236, 0x237, 0x27F,   766,   837,   897,  1027,  1077,  1127}, //ut-laser-2
    {0x1D8, 0x1DA, 0x1DC, 0x1DB,  0x71, 0x1DD, 0x215, 0x216, 0x2A2,   794,   866,   925,   979,   984,   989}, //uniq
    {  704,   705,   706,   707,   708,   709,   710,   711,   712,   789,   860,   920,  1048,  1098,  1148}  //ut-boat-grenade-2
};

void buildTableR()
{
    tableR = (uint8_t*)malloc(RESEARCH_COUNT_FOR_CIVRESEARCH);
    memset(tableR, 0xFF, RESEARCH_COUNT_FOR_CIVRESEARCH);

    for (int i = 0; i < (sizeof(idAssignR) / sizeof(idAssignR[0])); i++)
        for (int j = 0; j < CIV_COUNT; j++)
            tableR[idAssignR[i].IDs[j]] = (uint8_t)i;
}

__declspec(naked) void researchCivAssign() //0040FD10
{
    __asm
    {
        movsx   eax, dx
        cmp     eax, RESEARCH_COUNT_FOR_CIVRESEARCH
        ja      locret
        mov     edx, tableR
        mov     dl, byte ptr [edx + eax]
        cmp     dl, 0FFh
        jz      locret
        push    ebx
        mov     ebx, offset idAssignR
        movsx   edx, dl
        lea     edx, [edx + edx * 4]  //mul edx by sizeof(short)*CIV_COUNT = 30
        lea     edx, [edx + edx * 2]
        shl     edx, 1
        dec     ecx
        lea     edx, [edx + ecx * 2]
        mov     ax, word ptr [ebx + edx]
        pop     ebx
locret:
        ret
    }
}

void __cdecl initCivResearchTable_atexit()
{
    free(tableR);
}

void initCivResearchTable()
{
    buildTableR();
    setHook((void*)0x0040FD10, researchCivAssign);

    efpatch_atexit(initCivResearchTable_atexit);
}
