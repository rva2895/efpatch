#include "stdafx.h"

#include <regex>
#include "effects.h"
#include "advtriggereffect.h"

assign dataIdentifiers[] =
{
	{"Unknown000",                                     0x000,  T_INT32}, //ptr, //type 10+ (all units)
	{"Type",                                           0x004,  T_INT8},
	{"Unknown008",                                     0x008,  T_INT32}, //ptr
	{"Unknown00C",                                     0x00C,  T_INT32}, //ptr
	{"Unitline",                                       0x010,  T_INT16},
	{"MinTechLevel",                                   0x012,  T_INT8 },
	{"LanguageDLLName",                                0x014,  T_INT16},
	{"LanguageDLLCreation",                            0x016,  T_INT16},
	{"ID1",                                            0x018,  T_INT16},
	{"ID2",                                            0x01A,  T_INT16},
	{"ID3",                                            0x01C,  T_INT16},
	{"Class",                                          0x01E,  T_INT16},
	{"StandingGraphics",                               0x020,  T_PTR_G}, //ptr?standing g
	{"Unknown024",                                     0x024,  T_INT32}, //0
	{"DyingGraphics",                                  0x028,  T_PTR_G}, //ptr?die g
	{"Unknown02C",                                     0x02C,  T_INT32}, //0
	{"DeathMode",                                      0x030,  T_INT8 },
	{"HitPoints",                                      0x032,  T_INT16}, //edited, old=0x030
	{"LineOfSight",                                    0x034,  T_FLOAT},
	{"GarrisonCapacity",                               0x038,  T_INT8 },
	{"CollisionSizeX",                                 0x03C,  T_FLOAT}, //RENAMED
	{"CollisionSizeY", /*+0x40*/                       0x040,  T_FLOAT}, //RENAMED
	{"CollisionSizeZ",                                 0x044,  T_FLOAT}, //RENAMED
	{"Unknown048",                                     0x048,  T_INT32}, //ptr?
	{"Unknown04C",                                     0x04C,  T_INT32}, //ptr?
	{"Unknown050",                                     0x050,  T_INT32}, //0
	{"Unknown054",                                     0x054,  T_INT32}, //0
	{"DeadUnitID",                                     0x058,  T_INT16},
	{"PlacementMode",                                  0x05A,  T_INT8 },
	{"AirMode",                                        0x05B,  T_INT8 },
	{"IconID",                                         0x05C,  T_INT16},
	{"HideInEditor",                                   0x05E,  T_INT16},
	{"Unknown060",                                     0x060,  T_INT16}, //unknown 1
	{"Unknown062",                                     0x062,  T_INT8 }, //1 enabled???
	{"Unknown063",                                     0x063,  T_INT8 }, //disabled???
	{"PlacementSideTerrain1",                          0x064,  T_INT16},
	{"PlacementSideTerrain2",                          0x066,  T_INT16},
	{"PlacementTerrain1",                              0x068,  T_INT16},
	{"PlacementTerrain2",                              0x06A,  T_INT16},
	{"ClearanceSizeX",                                 0x06C,  T_FLOAT}, //check XY
	{"ClearanceSizeY",                                 0x070,  T_FLOAT}, //^
	{"HillMode",                                       0x074,  T_INT8 },
	{"FogVisibility",                                  0x075,  T_INT8 },
	{"TerrainRestriction",                             0x076,  T_INT16},
	{"FlyMode",                                        0x078,  T_INT8 }, //1
	{"ResourceStorageType1",                           0x07A,  T_INT16},
	{"ResourceStorageType2",                           0x07C,  T_INT16},
	{"ResourceStorageType3",                           0x07E,  T_INT16},
	{"Resource1Storage", /*+0x80*/                     0x080,  T_FLOAT},
	{"Resource2Storage",                               0x084,  T_FLOAT},
	{"Resource3Storage",                               0x088,  T_FLOAT},
	{"ResourceCapacity",                               0x08C,  T_INT16},
	{"ResourceDecay",                                  0x090,  T_FLOAT},
	{"Unknown 3A",                                     0x094,  T_FLOAT},
	{"Unknown098",                                     0x098,  T_INT8 },
	{"Unknown099",                                     0x099,  T_INT8 },
	{"Unknown09A",                                     0x09A,  T_INT8 },
	{"BlastDefenseLevel",                              0x09B,  T_INT8 },
	{"Unknown2InteractionModeMinimapMode",             0x09C,  T_INT8 }, //test
	{"InteractionMode",                                0x09D,  T_INT8 },
	{"MinimapMode",                                    0x09E,  T_INT8 },
	{"Command ID",                                     0x09F,  T_INT8 },
	{"MinimapColor",                                   0x0A0,  T_INT8 },
	{"AttackMode",                                     0x0A1,  T_INT8 },
	{"Unknown 2",                                      0x0A2,  T_INT8 },
	{"Unknown0A4",                                     0x0A4,  T_INT32},
	{"SelectionMask",                                  0x0A8,  T_INT8 },
	{"SelectionShapeType",                             0x0A9,  T_INT8 },
	{"SelectionShape",                                 0x0AA,  T_INT8 },
	//{"Unknown0AB",                                     0x0AB,  T_INT8 }, //padding byte
	{"UnitAttribute",                                  0x0AC,  T_INT8 },
	{"Civilization",                                   0x0AD,  T_INT8 },
	{"AttributeLeftover",                              0x0AE,  T_INT16},
	{"LanguageDLLHelp",                                0x0B0,  T_INT32},
	{"LanguageDLLHotKeyText",                          0x0B4,  T_INT32},
	{"LanguageDLLHotKey",                              0x0B8,  T_INT32},
	{"Unselectable",                                   0x0BC,  T_INT8 },
	{"EnableAutoGather",                               0x0BD,  T_INT8 },
	{"AutoGatherMode",                                 0x0BE,  T_INT8 },
	{"AutoGatherID",                                   0x0BF,  T_INT8 },
	{"SelectionEffect", /*+0xC0*/                      0x0C0,  T_INT8 },
	{"SelectionColor",                                 0x0C1,  T_INT8 },
	{"SelectionShapeSizeX",                            0x0C4,  T_FLOAT},
	{"SelectionShapeSizeY",                            0x0C8,  T_FLOAT},
	{"SelectionShapeSizeZ",                            0x0CC,  T_FLOAT}, //type 10 ends here
	{"Speed",                                          0x0D0,  T_FLOAT}, //the only type 20 var
	{"WalkingGraphics",                                0x0D4,  T_PTR_G}, //type 30+ only
	{"WalkingGraphics2",                               0x0D8,  T_INT32},
	{"RotationSpeed", /*new*/                          0x0DC,  T_FLOAT},
	{"Unknown0E0",                                     0x0E0,  T_INT8 },
	{"TrackingUnit",                                   0x0E2,  T_INT16},
	{"TrackingUnitUsed",                               0x0E4,  T_INT8 },
	{"TrackingUnitDensity", /*new*/                    0x0E8,  T_FLOAT},
	{"Unknown0EC",                                     0x0EC,  T_INT8 },
	{"RotationsInRadians1",                            0x0F0,  T_FLOAT},
	{"RotationsInRadians2",                            0x0F4,  T_FLOAT},
	{"RotationsInRadians3",                            0x0F8,  T_FLOAT},
	{"RotationsInRadians4",                            0x0FC,  T_FLOAT},
	{"RotationsInRadians5",                            0x100,  T_FLOAT}, //type 30 ends here
	{"Unknown104",                                     0x104,  T_INT32}, //type 40+ only
	{"ActionWhenDiscovered",                           0x108,  T_INT16},
	{"SearchRadius",                                   0x10C,  T_FLOAT},
	{"WorkRate1",            /*???new*/                0x110,  T_FLOAT}, //test
	{"WorkRate2",            /*???new*/                0x114,  T_FLOAT}, //test
	{"DropSite1",                                      0x118,  T_INT16},
	{"DropSite2",                                      0x11A,  T_INT16},
	{"TaskSwapID",                                     0x11C,  T_INT8 },
	{"AttackSound",                                    0x120,  T_INT32}, //ptr
	{"MoveSound",                                      0x124,  T_INT32}, //ptr
	{"AnimalMode",                                     0x128,  T_INT8 }, //type 40 ends here
	{"AttackGraphics",                                 0x12C,  T_PTR_G}, //type 50+ only
	{"DefaultArmor",                                   0x130,  T_INT16},
	{"Armors count",                                   0x132,  T_INT16}, //old = BlastDefenseLevel
	{"Armors ptr",                                     0x134,  T_INT32},  //armors
	{"Attacks count",                                  0x138,  T_INT16},
	{"Attacks ptr",                                    0x13C,  T_INT32},
	{"Unknown140", /*+0x140*/                          0x140,  T_INT16}, //-1
	{"Range",                                          0x144,  T_FLOAT},
	{"BlastRadius",                                    0x148,  T_FLOAT},
	{"BlastLevel",                                     0x14C,  T_INT8 },
	{"ReloadTime1",                                    0x150,  T_FLOAT},
	{"ProjectileUnitID",                               0x154,  T_INT16},
	{"Accuracy",                                       0x156,  T_INT16},
	{"TowerMode",                                      0x158,  T_INT8 },
	{"FrameDelay",                                     0x15A,  T_INT16},
	{"ProjectileGraphicDisplacementX",                 0x15C,  T_FLOAT},
	{"ProjectileGraphicDisplacementY",                 0x160,  T_FLOAT},
	{"ProjectileGraphicDisplacementZ",                 0x164,  T_FLOAT},
	{"MinRange",                                       0x168,  T_FLOAT},
	{"DisplayedMeleeArmor",                            0x16C,  T_INT16},
	{"DisplayedAttack",                                0x16E,  T_INT16},
	{"DisplayedRange",                                 0x170,  T_FLOAT},
	{"ReloadTime2",                                    0x174,  T_FLOAT},
	{"AccuracyErrorRadius",                            0x178,  T_FLOAT}, //type 50 ends here
	{"Cost1Type",                                      0x17C,  T_INT16}, //type 70+ only
	{"Cost1Amount", /*+0x180*/                         0x17E,  T_INT16},
	{"Cost1IsPaid", /*+0x180*/                         0x180,  T_INT16},
	{"Cost2Type",                                      0x182,  T_INT16},
	{"Cost2Amount",                                    0x184,  T_INT16},
	{"Cost2IsPaid",                                    0x186,  T_INT16},
	{"Cost3Type",                                      0x188,  T_INT16},
	{"Cost3Amount",                                    0x18A,  T_INT16},
	{"Cost3IsPaid",                                    0x18C,  T_INT16},
	{"TrainTime",                                      0x18E,  T_INT16},
	{"TrainLocation",                                  0x190,  T_INT16},
	{"TrainButton",                                    0x192,  T_INT8 },
	{"DisplayedPierceArmor",                           0x194,  T_INT16},
	{"Unknown198",                                     0x198,  T_INT32}, //Unknown26
	{"Unknown19C",                                     0x19C,  T_INT32}, //Unknown27
	{"CreatableType",                                  0x1A0,  T_INT8 },
	{"MinDuplMissiles",   /*new*/                      0x1A4,  T_FLOAT},
	{"MaxDuplMissiles",                                0x1A8,  T_INT8 },
	{"MissileSpawningAreaWidth",                       0x1AC,  T_FLOAT},
	{"MissileSpawningAreaLength",                      0x1B0,  T_FLOAT},
	{"MissileSpawningRandomness",                      0x1B4,  T_FLOAT},
	{"MissileDuplUnit",                                0x1B8,  T_INT16},
	{"SpecialGraphics",                                0x1BC,  T_INT32},
	{"SpecialAbility", /*+0x1C0*/                      0x1C0,  T_INT8 },
	{"HeroMode",                                       0x1C1,  T_INT8 }, //type 70 ends here
	{"GarrisonGraphics",                               0x1C4,  T_INT32}, //type 80+ only (sure?)
	{"Unknown1C8",                                     0x1C8,  T_INT32},
	{"Unknown1CC",                                     0x1CC,  T_INT32},
	{"Unknown1D0",                                     0x1D0,  T_INT32},
	{"AdjacentMode",                                   0x1D4,  T_INT8 },
	{"GraphicsAngle",                                  0x1D6,  T_INT16},
	{"DisappearsWhenBuilt",                            0x1D8,  T_INT8 },
	{"StackUnitID",                                    0x1DA,  T_INT16},
	{"FoundationTerrainID",                            0x1DC,  T_INT16},
	{"OldTerrainLikeID",                               0x1DE,  T_INT16},
	{"ResearchID",                                     0x1E0,  T_INT16}
	//TODO
};

int getArrayIndex(const char* txt, char* type)
{
	int i;
	for (i = 0; i < sizeof(dataIdentifiers) / sizeof(dataIdentifiers[0]); i++)
	{
		if (!strcmp(dataIdentifiers[i].str, txt))
		{
			*type = dataIdentifiers[i].type;
			return dataIdentifiers[i].offset;
		}
	}
	return -1;
}

void setUnitDataC(int index, void* unitData, char value)
{
	*(char*)((char*)unitData + index) = value;
}
void setUnitDataS(int index, void* unitData, short value)
{
	*(short*)((char*)unitData + index) = value;
}
void setUnitDataL(int index, void* unitData, long value)
{
	*(long*)((char*)unitData + index) = value;
}
void setUnitDataF(int index, void* unitData, float value)
{
	*(float*)((char*)unitData + index) = value;
}

void modifyUnitDataC(int index, void* unitData, char value)
{
	*(char*)((char*)unitData + index) += value;
}
void modifyUnitDataS(int index, void* unitData, short value)
{
	*(short*)((char*)unitData + index) += value;
}
void modifyUnitDataL(int index, void* unitData, long value)
{
	*(long*)((char*)unitData + index) += value;
}
void modifyUnitDataF(int index, void* unitData, float value)
{
	*(float*)((char*)unitData + index) += value;
}

void multiplyUnitDataC(int index, void* unitData, float value)
{
	*(char*)((char*)unitData + index) *= value;
}
void multiplyUnitDataS(int index, void* unitData, float value)
{
	*(short*)((char*)unitData + index) *= value;
}
void multiplyUnitDataL(int index, void* unitData, float value)
{
	*(long*)((char*)unitData + index) *= value;
}
void multiplyUnitDataF(int index, void* unitData, float value)
{
	*(float*)((char*)unitData + index) *= value;
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void* __stdcall getGraphicPtr(short id)
{
	__asm
	{
		mov     eax, 6A3684h
		mov     eax, [eax]
		mov     eax, [eax + 17B4h]
		mov     eax, [eax + 126Ch]
		//mov     eax, [eax+420h]

		mov     edx, [eax + 40h] //graphics count
		movsx   ecx, word ptr [esp + 4]
		cmp     ecx, edx
		//jge     badID
		mov     eax, [eax + 44h]
		mov     eax, [eax + ecx * 4]
		ret     4
badID:
		xor     eax, eax
		ret     4
	}
}

__declspec(naked) short* __stdcall getAttackArmorPtr()
{
	__asm
	{
		test    ecx, ecx                //ecx - count
		jz      _atkEnd                 //edx - ptr
_atkCont:                               //ax  - class
		cmp     ax, word ptr [edx]
		jz      _atkFound
		add     edx, 4
		dec     ecx
		jnz     _atkCont
_atkEnd:
		xor     eax, eax
		ret
_atkFound:
		lea     eax, [edx + 2]
		ret
	}
}

__declspec(naked) short* __fastcall getAttackPtr(void* propObj, int c)
{
	__asm
	{
		mov     eax, edx
		mov     edx, [ecx + 13Ch]           //attacks
		movsx   ecx, word ptr [ecx + 138h]  //count

		call    getAttackArmorPtr
		ret
	}
}

__declspec(naked) short* __fastcall getArmorPtr(void* propObj, int c)
{
	__asm
	{
		mov     eax, edx
		mov     edx, [ecx + 134h]           //attacks
		movsx   ecx, word ptr [ecx + 132h]  //count

		call    getAttackArmorPtr
		ret
	}
}
#pragma warning(pop)

bool isVarAllowed(void* unitData, int i)
{
	int type = *(char*)((int)unitData + 4);
	int max;
	switch (type)
	{
	case 10:
		max = 0x0CC;
		break;
	case 20:
		max = 0x0D0;
		break;
	case 30:
		max = 0x100;
		break;
	case 40:
		max = 0x128;
		break;
	case 50:
		max = 0x178;
		break;
	case 60:
		max = 0;
		break;
	case 70:
		max = 0x1C1;
		break;
	case 80:
		max = 0x1E0;
		break;
	default:
		max = 0;
		break;
	}
	return (i <= max);
}

/*void editUnitData(void* unitData, int i, int action)
{

}

bool __stdcall advTriggerEffectActual2(void* unitData, char* str)
{
	std::regex r("^(.+) (.+) ([0-9]+ )?([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)$",
		std::regex_constants::icase);

	std::smatch h;
	std::string s(str);
	int action;
	if (std::regex_match(s, h, r))
	{
		std::ssub_match h_sub = h[1];
		std::string command = h_sub.str();
		if (command == "SET")
			action = 0;
		else if (command == "ADD")
			action = 1;
		else if (command == "MUL")
			action = 2;
		else
			return false;

		h_sub = h[2];
		std::string variable = h_sub.str();
		char type;
		int i = getArrayIndex(variable.c_str(), &type);
		if (i == -1)
		{
			if (variable == "Attack")
			{
				//todo
			}
			else if (variable == "Armor")
			{
				//todo
			}
			else
				return false;
			h_sub = h[3];
			std::string cls_s = h_sub.str();
			int cls;
			sscanf(cls_s.c_str(), "%d", &cls);

		}
		else
			editUnitData(unitData, i, action);
		return true;
	}
	else
		return false;
}*/

void __stdcall advTriggerEffectActual(void* unitData, char* s)
{
	char type;
	char command[50];
	char variable[50];
	char amount[50];

	long int8Amount;
	long int16Amount;
	long int32Amount;
	float floatAmount;

	int index;
	//strcpy (s,(char*)x);
	sscanf(s, "%s %s %s", command, variable, amount);
	index = getArrayIndex(variable, &type);

	//if (index == 0x38)
	//	index = -1;

	if (index != -1)
	{
		if (!isVarAllowed(unitData, index))
		{
			log("Warning: cannot modify var %s for type %d", variable, *(char*)((int)unitData + 4));
			return;
		}
	}

#ifdef _DEBUG
	log("Adv trigger effect: cmd=%s var=%s amount=%s", command, variable, amount);
#endif

	if (index != -1)
	{
		_strupr(command);
		if (!strcmp(command, "SET"))
		{
			switch (type)
			{
			case T_INT8:
				sscanf(amount, "%d", &int8Amount);
				setUnitDataC(index, unitData, int8Amount);
				break;
			case T_INT16:
				sscanf(amount, "%d", &int16Amount);
				setUnitDataS(index, unitData, int16Amount);
				break;
			case T_INT32:
				sscanf(amount, "%d", &int32Amount);
				setUnitDataL(index, unitData, int32Amount);
				break;
			case T_FLOAT:
				sscanf(amount, "%f", &floatAmount);
				setUnitDataF(index, unitData, floatAmount);
				break;
			case T_PTR_G:
				sscanf(amount, "%d", &int16Amount);
				void* p = getGraphicPtr(int16Amount);
				if (p)
					setUnitDataL(index, unitData, (long)p);
				else
					log("Error: graphic ID (%d) >= n (%d)", index, int16Amount);
			}
		}
		else if (!strcmp(command, "ADD"))
		{
			switch (type)
			{
			case T_INT8:
				sscanf(amount, "%d", &int8Amount);
				modifyUnitDataC(index, unitData, int8Amount);
				break;
			case T_INT16:
				sscanf(amount, "%d", &int16Amount);
				modifyUnitDataS(index, unitData, int16Amount);
				break;
			case T_INT32:
				sscanf(amount, "%d", &int32Amount);
				modifyUnitDataL(index, unitData, int32Amount);
				break;
			case T_FLOAT:
				sscanf(amount, "%f", &floatAmount);
				modifyUnitDataF(index, unitData, floatAmount);
				break;
			case T_PTR_G:
				log("Error: command ADD is not allowed for pointers");
			}
		}
		else if (!strcmp(command, "MUL"))
		{
			switch (type)
			{
			case T_INT8:
			{
				sscanf(amount, "%f", &floatAmount);
				multiplyUnitDataC(index, unitData, floatAmount);
				break;
			}
			case T_INT16:
			{
				sscanf(amount, "%f", &floatAmount);
				multiplyUnitDataS(index, unitData, floatAmount);
				break;
			}
			case T_INT32:
			{
				sscanf(amount, "%f", &floatAmount);
				multiplyUnitDataL(index, unitData, floatAmount);
				break;
			}
			case T_FLOAT:
			{
				sscanf(amount, "%f", &floatAmount);
				multiplyUnitDataF(index, unitData, floatAmount);
				break;
			}
			}
		}
		else
			log("Error: unknown command: %s", command);
	}
	else
	{
		char amount2[50];
		sscanf(s, "%s %s %s %s", command, variable, amount, amount2);
		if (*(int*)((int)unitData + 4) < 50)
		{
			log("Warning: Cannot modify attack or armor for type %d", *(char*)((int)unitData + 4));
			return;
		}
		int c;
		int v;
		short* d;
		sscanf(amount, "%d", &c);
		sscanf(amount2, "%d", &v);
		if (!strcmp(variable, "Attack"))
			d = getAttackPtr(unitData, c);
		else if (!strcmp(variable, "Armor"))
			d = getArmorPtr(unitData, c);
		else
		{
			log("Error: unknown variable: %s", variable);
			return;
		}
		if (!strcmp(command, "SET"))
			*d = v;
		else if (!strcmp(command, "ADD"))
			*d += v;
		else if (!strcmp(command, "MUL"))
			*d *= v;
		else
			log("Error: unknown command: %s", command);
	}
}

void __declspec(naked) advTriggerEffect()
{
	__asm
	{
		mov     eax, [edi + 6Ch]
		push    eax			//string
		push    esi			//property object
		call    advTriggerEffectActual
		ret
	}
}
