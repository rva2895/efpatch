#include "stdafx.h"

#include "civunit.h"

unsigned char* tableU;

ID_ASSIGNU idAssignU[] =
{
	//   E,     G,     R,     N,     F,     W,     A,     C,     Z,     O
	{ 0xB4,  0xB7,  0xE8,  0xCC,  0x34,  0xEF, 0x287, 0x28C, 0x9F5,  4064},  //padawan
	{ 0x2C,  0x3D,  0x4A,  0x3E,  0x61,  0x4D, 0x621, 0x61F, 0x9F7,  4066},  //knight
	{ 0x73,  0x7D,  0x88,  0x86,  0x59,  0x8C, 0x283, 0x285, 0x9F9,  4068},  //master
	{0x378, 0x380, 0x37C, 0x37A,  0xB0, 0x37E, 0x5C5, 0x5C7, 0x6F7,  4024},  //adv frigate
	{0x323, 0x325, 0x329, 0x327,  0xB1, 0x32B, 0x5C9, 0x5CB, 0x6FD,  4020},  //adv cruiser
	{0x38C, 0x392, 0x394, 0x38E,  0xB6, 0x390, 0x5CD, 0x5CF, 0x6F9,  4022},  //hvy destroyer
	{0x3A0, 0x33D, 0x33F, 0x3A2,  0xB8, 0x343, 0x5D1, 0x5D3, 0x6FB,  4018},  //hvy aa destroyer
	{0x382, 0x388, 0x38A, 0x384,  0xBA, 0x386, 0x5D5, 0x5D7, 0x6F8,  4023},  //destroyer
	{0x364, 0x36C, 0x368, 0x366,  0xBB, 0x36A, 0x5D9, 0x5DB, 0x6F5,  4025},  //lt frigate
	{0x396, 0x39A, 0x39C, 0x398,  0xBC, 0x39E, 0x5DD, 0x5DF, 0x6FA,  4019},  //aa destroyer
	{0x36E, 0x376, 0x372, 0x370,  0xBD, 0x374, 0x5E1, 0x5E3, 0x6F6,  4026},  //frigate
	{0x32F, 0x332, 0x337, 0x334,  0xBE, 0x339, 0x5E5, 0x5E7, 0x6FC,  4021},  //cruiser
	{0x346, 0x347, 0x349, 0x348,  0xBF, 0x34A, 0x5E9, 0x5EB, 0x6F4,  4029},  //transport ship
	{0x2DD, 0x2DF, 0x2EC, 0x2E9,  0x6C, 0x2EE, 0x5ED, 0x5EE, 0x6E7,  4002},  //adv bomber
	{0x2F0, 0x2F2, 0x2F6, 0x2F4,  0x71, 0x2F8, 0x5EF, 0x5F0, 0x6E4,  4005},  //adv fighter
	{0x40C, 0x410, 0x416, 0x412, 0x40E, 0x414, 0x5F1, 0x5F2, 0x6E8,  4001},  //air transport
	{0x2FA, 0x2FC, 0x301, 0x2FE,  0x7F, 0x303, 0x5F3, 0x5F4, 0x6E5,  4003},  //bomber
	{0x271, 0x276, 0x27A, 0x278, 0x274, 0x27C, 0x26B, 0x26C, 0x6F3,  4008},  //air cruiser
	{0x305, 0x307, 0x30B, 0x309,  0x9E, 0x30D, 0x5F5, 0x5F6, 0x6E2,  4006},  //fighter
	{0x30F, 0x311, 0x315, 0x313,  0xA0, 0x317, 0x5F7, 0x5F8, 0x6E6,  4004},  //enh bomber
	{0x319, 0x31B, 0x31F, 0x31D,  0xA3, 0x321, 0x5F9, 0x5FA, 0x6E3,  4007},  //fast fighter
	{0x3B7, 0x3B8, 0x3BA, 0x3B9,  0xA9, 0x3BB, 0x5FB, 0x5FC, 0x6F1,  4015},  //undeployed cannon
	{0x3C1, 0x3C3, 0x3C7, 0x3C5,  0xAA, 0x3C9, 0x5FD, 0x5FF, 0x6F2,  4060},  //bounty hunter
	{0x3CB, 0x3CF, 0x3D3, 0x3D1,  0xAC, 0x3D5, 0x27E,  4085, 0x9F0,   640},  //elite UU
	{0x3D7, 0x3D8, 0x3DA, 0x3D9, 0x1D4, 0x3DB, 0x601, 0x602, 0x6EF,  4016},  //cannon
	{0x3E1,     2,     6,     4,  0xAE,     8, 0x281,  2277, 0x9F2,   642},  //UU
	{0x277, 0x279, 0x27D, 0x27B,  0xC0, 0x27F, 0x603, 0x605, 0x6EC,  4011},  //hvy artillery
	{0x28B, 0x28D, 0x2A3, 0x291,  0xC5, 0x2A7, 0x607, 0x609, 0x6EE,  4009},  //hvy aa mobile
	{0x2A9, 0x2AB, 0x2AF, 0x2AD,  0xC8, 0x2B1, 0x60B, 0x60D, 0x6EA,  4013},  //hvy pummel
	{0x2B3, 0x2B5, 0x2BA, 0x2B7,  0xCB, 0x2BC, 0x60F, 0x611, 0x6EB,  4012},  //artillery
	{0x2BE, 0x2C0, 0x2C4, 0x2C2,  0xCF, 0x2C7, 0x613, 0x615, 0x6ED,  4010},  //aa mobile
	{0x2C9, 0x2CB, 0x2CF, 0x2CD,  0xE7, 0x2D1, 0x617, 0x619, 0x6E9,  4014},  //pummel
	{0x1D5, 0x1D7, 0x1DB, 0x1D9,  0xF4, 0x1DF, 0x387, 0x38B, 0x6DC,  4032},  //strike mech
	{0x1E5, 0x1E9, 0x1EF, 0x1ED,  0xF6, 0x1F1, 0x38F, 0x393, 0x6DE,  2568},  //mech destroyer
	{0x1F4, 0x1F6, 0x202, 0x1FC,  0xF9, 0x205, 0x397, 0x39B, 0x6E0,  4035},  //assault mech
	{0x226, 0x228, 0x22C, 0x22A, 0x102, 0x22E, 0x39F, 0x3A4, 0x6DB,  4034},  //scout
	{0x230, 0x236, 0x23A, 0x238, 0x105, 0x23C, 0x3BD, 0x3BF, 0x6DD,  4033},  //hvy strike mech
	{0x23E, 0x240, 0x24C, 0x242, 0x107, 0x254, 0x3CD, 0x3D4, 0x6DF,  2569},  //hvy mech destroyer
	{0x25B, 0x26E, 0x272, 0x270, 0x115, 0x275, 0x3DD, 0x3DF, 0x6E1,  4036},  //hvy assault mech
	{0x3AB, 0x3AD, 0x3B1, 0x3AF,  0xA7, 0x3B3, 0x3E4, 0x3ED, 0x705,  4061},  //medic
	{0x3F1, 0x3F3, 0x3F5, 0x3F4, 0x3F2, 0x3F6, 0x3EF, 0x3F0, 0x706,  4062},  //power droid
	{0x149, 0x14B, 0x15A, 0x158, 0x11F, 0x160, 0x3F7, 0x40D, 0x6D6,  4054},  //adv mounted trooper
	{0x167, 0x169, 0x178, 0x172, 0x121, 0x17A, 0x413, 0x417, 0x6D4,  4056},  //mounted trooper
	{0x17D, 0x17F, 0x183, 0x181, 0x124, 0x185, 0x5C6, 0x5CA,  1696,  4048},  //grenade trooper
	{0x187, 0x189, 0x18D, 0x18B, 0x127, 0x18F, 0x5CE, 0x5D2, 0x6D5,  4058},  //hvy mounted trooper
	{0x191, 0x193, 0x197, 0x195, 0x12D, 0x199, 0x5D6, 0x5DA, 0x6D8,  4052},  //hvy aa trooper
	{0x19C, 0x1A2, 0x1A6, 0x1A4, 0x129, 0x1A8, 0x5DE, 0x5E2, 0x6D0,  4044},  //hvy trooper
	{0x1AA, 0x1AC, 0x1B0, 0x1AE, 0x12F, 0x1B4, 0x5E6, 0x5EA,  1698,  4046},  //repeater trooper
	{0x1B6, 0x1B8, 0x1BC, 0x1BA, 0x131, 0x1BE, 0x604, 0x608,  1697,  4050},  //aa trooper
	{0x133,  0xEE, 0x11A, 0x118, 0x135, 0x11E, 0x60C, 0x610, 0x6CE,  4040},  //trooper recruit
	{0x1C0, 0x1C4, 0x1CC, 0x1C6, 0x13B, 0x1CE, 0x614, 0x618, 0x6CF,  4042},  //trooper
	{0x9C8, 0x9C9, 0x9CA, 0x9CB, 0x9CC, 0x9CD, 0x9CE, 0x9CF, 0x9D0,  4031},  //interceptor
	{0x9DC, 0x9DD, 0x9DE, 0x9DF, 0x9E0, 0x9E1, 0x9E2, 0x9E3, 0x9E4,  4030},  //attacker
	{ 1803,  1805,  1806,  1807,  1808,  1809,  1810,  1811,  1812,  4027},  //monitor
	{ 1813,  1827,  1838,  1840,  1846,  1852,  1870,  1879,  1905,  4028},  //hvy monitor
	{ 1949,  1634,  1635,  1954,  1639,  1641,  1643,  1644,  1645,  4017}   //transport mech
};

void buildTableU()
{
	tableU = (unsigned char*)malloc(UNIT_COUNT);
	memset(tableU, 0xFF, UNIT_COUNT);

	for (int i = 0; i < (sizeof(idAssignU) / sizeof(idAssignU[0])); i++)
		for (int j = 0; j < CIV_COUNT; j++)
			tableU[idAssignU[i].IDs[j]] = i;
}

__declspec(naked) void unitCivAssign() //004DAD80
{
	__asm
	{
		mov     eax, ecx
		cmp     ecx, UNIT_COUNT
		ja      locret
		mov     ecx, tableU
		mov     cl, byte ptr [ecx + eax]
		cmp     cl, 0FFh
		jz      locret
		push    ebx
		mov     ebx, offset idAssignU
		movsx   ecx, cl
		lea     ecx, [ecx + ecx * 4]   //mul ecx by sizeof(short)*CIV_COUNT = 20
		shl     ecx, 2
		dec     edx
		lea     ecx, [ecx + edx * 2]
		mov     ax, word ptr [ebx + ecx]
		pop     ebx
locret:
		ret
	}
}

void initCivUnitTable()
{
	buildTableU();
	setHook((void*)0x004DAD80, unitCivAssign);
}
