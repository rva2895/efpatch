#include "stdafx.h"

//#include <regex>
#include "effects.h"
#include "advtriggereffect.h"

const assign dataIdentifiers[] =
{
    //type 10
    {"Unknown000",                                     0x000,  T_INT32}, //ptr
    {"Type",                                           0x004,  T_INT8 }, //saved
    {"Unknown008",                                     0x008,  T_INT32}, //ptr
    {"Unknown00C",                                     0x00C,  T_INT32}, //ptr
    {"Unitline",                                       0x010,  T_INT16},
    {"MinTechLevel",                                   0x012,  T_INT8 },
    {"LanguageDLLName",                                0x014,  T_INT16},
    {"LanguageDLLCreation",                            0x016,  T_INT16},
    {"ID1",                                            0x018,  T_INT16}, //saved
    {"ID2",                                            0x01A,  T_INT16}, //saved
    {"ID3",                                            0x01C,  T_INT16}, //saved
    {"Class",                                          0x01E,  T_INT16}, //saved
    {"StandingGraphics",                               0x020,  T_PTR_G}, //ptr?standing g
    {"Unknown024",                                     0x024,  T_INT32}, //0
    {"DyingGraphics",                                  0x028,  T_PTR_G}, //ptr?die g
    {"Unknown02C",                                     0x02C,  T_INT32}, //0
    {"DeathMode",                                      0x030,  T_INT8 }, //OLD
    {"UndeadMode",                                     0x030,  T_INT8 }, //NEW
    {"HitPoints",                                      0x032,  T_INT16}, //saved
    {"LineOfSight",                                    0x034,  T_FLOAT}, //saved
    {"GarrisonCapacity",                               0x038,  T_INT8 }, //saved
    {"CollisionSizeX",                                 0x03C,  T_FLOAT}, //saved
    {"CollisionSizeY", /*+0x40*/                       0x040,  T_FLOAT}, //saved
    {"CollisionSizeZ",                                 0x044,  T_FLOAT}, //not saved
    {"Unknown048",                                     0x048,  T_INT32}, //ptr?
    {"Unknown04C",                                     0x04C,  T_INT32}, //ptr?
    {"Unknown050",                                     0x050,  T_INT32}, //0
    {"Unknown054",                                     0x054,  T_INT32}, //0
    {"DeadUnitID",                                     0x058,  T_INT16},
    {"PlacementMode",                                  0x05A,  T_INT8 }, //sort number
    {"AirMode",                                        0x05B,  T_INT8 }, //OLD
    {"CanBeBuiltOn",                                   0x05B,  T_INT8 }, //NEW
    {"IconID",                                         0x05C,  T_INT16},
    {"HideInEditor",                                   0x05E,  T_INT8 }, //saved
    {"Unknown060",                                     0x060,  T_INT16}, //unknown 1
    {"Available",                                      0x062,  T_INT8 }, //saved
    {"Disabled",                                       0x063,  T_INT8 }, //saved
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
    {"Resource1Storage", /*+0x80*/                     0x080,  T_FLOAT}, //saved
    {"Resource2Storage",                               0x084,  T_FLOAT},
    {"Resource3Storage",                               0x088,  T_FLOAT},
    {"ResourceCapacity",                               0x08C,  T_INT16}, //saved
    {"ResourceDecay",                                  0x090,  T_FLOAT},
    {"Unknown 3A",                                     0x094,  T_FLOAT},
    {"Resource1StoreMode",                             0x098,  T_INT8 },
    {"Resource2StoreMode",                             0x099,  T_INT8 },
    {"Resource3StoreMode",                             0x09A,  T_INT8 },
    {"BlastDefenseLevel",                              0x09B,  T_INT8 },
    {"CombatLevel",                                    0x09C,  T_INT8 },
    {"InteractionMode",                                0x09D,  T_INT8 },
    {"MinimapMode",                                    0x09E,  T_INT8 },
    {"InterfaceKind",                                  0x09F,  T_INT8 },
    {"MinimapColor",                                   0x0A0,  T_INT8 },
    {"AttackMode",                                     0x0A1,  T_INT8 },
    {"Unknown0A2",                                     0x0A2,  T_INT8 },
    {"Unknown0A3",                                     0x0A3,  T_INT8 },
    {"Unknown0A4",                                     0x0A4,  T_INT32}, //ptr
    {"SelectionMask",                                  0x0A8,  T_INT8 }, //OLD
    {"OcclusionMode",                                  0x0A8,  T_INT8 }, //NEW
    {"SelectionShapeType",                             0x0A9,  T_INT8 }, //OLD
    {"ObstructionType",                                0x0A9,  T_INT8 }, //NEW
    {"SelectionShape",                                 0x0AA,  T_INT8 }, //OLD
    {"BlockageClass",                                  0x0AA,  T_INT8 }, //NEW
    {"UnitAttribute",                                  0x0AC,  T_INT8 },
    {"Civilization",                                   0x0AD,  T_INT8 },
    {"AttributeLeftover",                              0x0AE,  T_INT16},
    {"LanguageDLLHelp",                                0x0B0,  T_INT32},
    {"LanguageDLLHotKeyText",                          0x0B4,  T_INT32},
    {"LanguageDLLHotKey",                              0x0B8,  T_INT32}, //saved
    {"Unselectable",                                   0x0BC,  T_INT8 }, //OLD
    {"Recyclable",                                     0x0BC,  T_INT8 }, //NEW
    {"EnableAutoGather",                               0x0BD,  T_INT8 }, //OLD
    {"CanBeGathered",                                  0x0BD,  T_INT8 }, //NEW
    {"AutoGatherMode",                                 0x0BE,  T_INT8 },
    {"AutoGatherID",                                   0x0BF,  T_INT8 },
    {"SelectionEffect", /*+0xC0*/                      0x0C0,  T_INT8 },
    {"SelectionColor",                                 0x0C1,  T_INT8 },
    {"SelectionShapeSizeX",                            0x0C4,  T_FLOAT},
    {"SelectionShapeSizeY",                            0x0C8,  T_FLOAT},
    {"SelectionShapeSizeZ",                            0x0CC,  T_FLOAT}, //type 10 ends here
    //type 20
    {"Speed",                                          0x0D0,  T_FLOAT}, //saved
    //type 30
    {"WalkingGraphics",                                0x0D4,  T_PTR_G}, //not saved
    {"WalkingGraphics2",                               0x0D8,  T_INT32}, //not saved
    {"RotationSpeed",                                  0x0DC,  T_FLOAT}, //saved
    {"SizeClass",                                      0x0E0,  T_INT8 }, //not saved
    {"TrackingUnit",                                   0x0E2,  T_INT16}, //not saved
    {"TrackingUnitUsed",                               0x0E4,  T_INT8 }, //not saved
    {"TrackingUnitDensity",                            0x0E8,  T_FLOAT}, //not saved
    {"MoveAlgorithm",                                  0x0EC,  T_INT8 }, //not saved
    {"RotationsInRadians1",                            0x0F0,  T_FLOAT}, //not saved
    {"RotationsInRadians2",                            0x0F4,  T_FLOAT}, //not saved
    {"RotationsInRadians3",                            0x0F8,  T_FLOAT}, //not saved
    {"RotationsInRadians4",                            0x0FC,  T_FLOAT}, //not saved
    {"RotationsInRadians5",                            0x100,  T_FLOAT}, //not saved
    //type 40
    {"DefaultTask",                                    0x104,  T_INT16}, //not saved
    {"Unknown106",                                     0x106,  T_INT16}, //not saved
    {"ActionWhenDiscovered",                           0x108,  T_INT16}, //not saved
    {"SearchRadius",                                   0x10C,  T_FLOAT}, //saved
    {"WorkRate1",                                      0x110,  T_FLOAT}, //saved
    {"WorkRate2",                                      0x114,  T_FLOAT}, //not saved
    {"DropSite1",                                      0x118,  T_INT16}, //not saved
    {"DropSite2",                                      0x11A,  T_INT16}, //not saved
    {"TaskSwapID",                                     0x11C,  T_INT8 }, //not saved
    {"AttackSound",                                    0x120,  T_INT32}, //ptr, not saved
    {"MoveSound",                                      0x124,  T_INT32}, //ptr, not saved
    {"AnimalMode",                                     0x128,  T_INT8 }, //not saved
    //type 50
    {"AttackGraphics",                                 0x12C,  T_PTR_G}, //not saved
    {"DefaultArmor",                                   0x130,  T_INT16}, //saved
    {"Armors count",                                   0x132,  T_INT16}, //saved
    {"Armors ptr",                                     0x134,  T_INT32}, //saved
    {"Attacks count",                                  0x138,  T_INT16}, //saved
    {"Attacks ptr",                                    0x13C,  T_INT32}, //saved
    {"TerrainDefenseBonus", /*+0x140*/                 0x140,  T_INT16}, //saved
    {"Range",                                          0x144,  T_FLOAT}, //saved twice (why?)
    {"BlastRadius",                                    0x148,  T_FLOAT}, //saved
    {"BlastLevel",                                     0x14C,  T_INT8 }, //not saved
    {"ReloadTime1",                                    0x150,  T_FLOAT}, //saved
    {"ProjectileUnitID",                               0x154,  T_INT16}, //saved
    {"Accuracy",                                       0x156,  T_INT16}, //saved
    {"TowerMode",                                      0x158,  T_INT8 }, //not saved
    {"FrameDelay",                                     0x15A,  T_INT16}, //not saved
    {"ProjectileGraphicDisplacementX",                 0x15C,  T_FLOAT}, //not saved
    {"ProjectileGraphicDisplacementY",                 0x160,  T_FLOAT}, //not saved
    {"ProjectileGraphicDisplacementZ",                 0x164,  T_FLOAT}, //not saved
    {"MinRange",                                       0x168,  T_FLOAT}, //saved
    {"DisplayedMeleeArmor",                            0x16C,  T_INT16}, //not saved
    {"DisplayedAttack",                                0x16E,  T_INT16}, //not saved
    {"DisplayedRange",                                 0x170,  T_FLOAT}, //not saved
    {"ReloadTime2",                                    0x174,  T_FLOAT}, //not saved
    {"AccuracyErrorRadius",                            0x178,  T_FLOAT}, //not saved
    //type 60
    {"ProjectileType",                                 0x17C,  T_INT8 }, //not saved
    {"SmartMode",                                      0x17D,  T_INT8 }, //saved
    {"HitMode",                                        0x17E,  T_INT8 }, //not saved
    {"VanishMode",                                     0x17F,  T_INT8 }, //not saved
    {"AreaEffects",                                    0x180,  T_INT8 }, //not saved
    {"ProjectileArc",                                  0x184,  T_FLOAT}, //not saved
    //type 70
    {"Cost1Type",                                      0x17C,  T_INT16}, //saved
    {"Cost1Amount", /*+0x180*/                         0x17E,  T_INT16}, //saved
    {"Cost1IsPaid", /*+0x180*/                         0x180,  T_INT16}, //saved
    {"Cost2Type",                                      0x182,  T_INT16}, //saved
    {"Cost2Amount",                                    0x184,  T_INT16}, //saved
    {"Cost2IsPaid",                                    0x186,  T_INT16}, //saved
    {"Cost3Type",                                      0x188,  T_INT16}, //saved
    {"Cost3Amount",                                    0x18A,  T_INT16}, //saved
    {"Cost3IsPaid",                                    0x18C,  T_INT16}, //saved
    {"TrainTime",                                      0x18E,  T_INT16}, //saved
    {"TrainLocation",                                  0x190,  T_INT16}, //not saved
    {"TrainButton",                                    0x192,  T_INT8 }, //not saved
    {"DisplayedPierceArmor",                           0x194,  T_INT16}, //not saved
    {"Unknown198",                                     0x198,  T_INT32}, //Unknown26, not saved
    {"Unknown19C",                                     0x19C,  T_INT32}, //Unknown27, not saved
    {"CreatableType",                                  0x1A0,  T_INT8 }, //not saved
    {"MinDuplMissiles",                                0x1A4,  T_FLOAT}, //saved
    {"MaxDuplMissiles",                                0x1A8,  T_INT8 }, //saved
    {"MissileSpawningAreaWidth",                       0x1AC,  T_FLOAT}, //not saved
    {"MissileSpawningAreaLength",                      0x1B0,  T_FLOAT}, //not saved
    {"MissileSpawningRandomness",                      0x1B4,  T_FLOAT}, //not saved
    {"MissileDuplUnit",                                0x1B8,  T_INT32}, //not saved
    {"SpecialGraphics",                                0x1BC,  T_INT32}, //not saved
    {"SpecialAbility", /*+0x1C0*/                      0x1C0,  T_INT8 }, //not saved
    {"HeroMode",                                       0x1C1,  T_INT8 }, //not saved
    //type 80
    {"GarrisonGraphics",                               0x1C4,  T_INT32}, //not saved
    {"Unknown1C8",                                     0x1C8,  T_INT32}, //not saved
    {"Unknown1CC",                                     0x1CC,  T_INT32}, //not saved
    {"Unknown1D0",                                     0x1D0,  T_INT32}, //not saved
    {"AdjacentMode",                                   0x1D4,  T_INT8 }, //not saved
    {"GraphicsAngle",                                  0x1D6,  T_INT16}, //saved
    {"DisappearsWhenBuilt",                            0x1D8,  T_INT8 }, //not saved
    {"StackUnitID",                                    0x1DA,  T_INT16}, //not saved
    {"FoundationTerrainID",                            0x1DC,  T_INT16}, //not saved
    {"OldTerrainLikeID",                               0x1DE,  T_INT16}, //not saved
    {"ResearchID",                                     0x1E0,  T_INT16}, //not saved
    {"Unknown1E4",                                     0x1E4,  T_INT32}, //not saved
    {"Unknown1E8",                                     0x1E8,  T_INT32}, //not saved
    {"Unknown1EC",                                     0x1EC,  T_INT32}, //not saved
    {"Unknown1F0",                                     0x1F0,  T_INT32}, //not saved
    {"Unknown1F4",                                     0x1F4,  T_INT32}, //not saved
    {"Unknown1F8",                                     0x1F8,  T_INT32}, //not saved
    {"Unknown1FC",                                     0x1FC,  T_INT32}, //not saved
    {"Unknown200",                                     0x200,  T_INT32}, //not saved
    {"Unknown204",                                     0x204,  T_INT32}, //not saved
    {"Unknown208",                                     0x208,  T_INT32}, //not saved
    {"Unknown20C",                                     0x20C,  T_INT32}, //not saved
    {"Unknown210",                                     0x210,  T_INT32}, //not saved
    {"Unknown214",                                     0x214,  T_INT32}, //not saved
    {"GarrisonHealRate",                               0x218,  T_FLOAT}, //not saved
    {"Unknown21C",                                     0x21C,  T_INT32}, //not saved
    {"Unknown220",                                     0x220,  T_INT32}, //not saved
    {"Unknown224",                                     0x224,  T_INT32}, //not saved
    {"Unknown228",                                     0x228,  T_INT32}  //not saved
    //TODO
};

uint32_t getArrayIndex(const char* txt, master_data_types* type)
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
    return UINT32_MAX;
}

void setUnitDataC(uint32_t index, void* unitData, char value)
{
    *(char*)((char*)unitData + index) = value;
}
void setUnitDataS(uint32_t index, void* unitData, short value)
{
    *(short*)((char*)unitData + index) = value;
}
void setUnitDataL(uint32_t index, void* unitData, long value)
{
    *(long*)((char*)unitData + index) = value;
}
void setUnitDataF(uint32_t index, void* unitData, float value)
{
    *(float*)((char*)unitData + index) = value;
}

void modifyUnitDataC(uint32_t index, void* unitData, char value)
{
    *(char*)((char*)unitData + index) += value;
}
void modifyUnitDataS(uint32_t index, void* unitData, short value)
{
    *(short*)((char*)unitData + index) += value;
}
void modifyUnitDataL(uint32_t index, void* unitData, long value)
{
    *(long*)((char*)unitData + index) += value;
}
void modifyUnitDataF(uint32_t index, void* unitData, float value)
{
    *(float*)((char*)unitData + index) += value;
}

void multiplyUnitDataC(uint32_t index, void* unitData, float value)
{
    *(char*)((char*)unitData + index) *= value;
}
void multiplyUnitDataS(uint32_t index, void* unitData, float value)
{
    *(short*)((char*)unitData + index) *= value;
}
void multiplyUnitDataL(uint32_t index, void* unitData, float value)
{
    *(long*)((char*)unitData + index) *= value;
}
void multiplyUnitDataF(uint32_t index, void* unitData, float value)
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
#pragma warning(pop)

void modify_class_val(short* count_ptr, short** val_ptr, short c, short v, master_data_operations op)
{
    short* class_val_ptr = NULL;
    for (int i = 0; i < *count_ptr; i++)
        if ((*val_ptr)[i * 2] == c)
            class_val_ptr = *val_ptr + i * 2 + 1;
    if (!class_val_ptr)
    {
        short* new_ptr = (short*) calloc_internal(4, (*count_ptr + 1));
        memcpy(new_ptr, *val_ptr, 4 * *count_ptr);
        new_ptr[*count_ptr * 2] = c;
        new_ptr[*count_ptr * 2 + 1] = 0;
        class_val_ptr = new_ptr + *count_ptr * 2 + 1;
        free_internal(*val_ptr);
        *val_ptr = new_ptr;
        (*count_ptr)++;
    }
    switch (op)
    {
    case OP_SET:
        *class_val_ptr = v;
        break;
    case OP_ADD:
        *class_val_ptr += v;
        break;
    case OP_MUL:
        *class_val_ptr *= v;
        break;
    default:
        break;
    }
}

void modify_attack(prop_object* p, master_data_operations op, short c, short v)
{
    modify_class_val(&p->attacks_count, (short**)&p->attacks_ptr, c, v, op);
}

void modify_armor(prop_object* p, master_data_operations op, short c, short v)
{
    modify_class_val(&p->armors_count, (short**)&p->armors_ptr, c, v, op);
}

bool isVarAllowed(prop_object* unitData, uint32_t i)
{
    uint32_t max;
    switch (unitData->type)
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
        max = 0x184;
        break;
    case 70:
        max = 0x1C1;
        break;
    case 80:
        max = 0x228;
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
            sscanf_s(cls_s.c_str(), "%d", &cls);

        }
        else
            editUnitData(unitData, i, action);
        return true;
    }
    else
        return false;
}*/

void __stdcall advTriggerEffectActual_sub(prop_object* unitData, const char* s)
{
    master_data_types type;
    char command[8];
    char variable[64];
    char amount[32];

    long int8Amount;
    long int16Amount;
    long int32Amount;
    float floatAmount;

    uint32_t index;
    sscanf_s(s, "%s %s %s",
        command, (unsigned)_countof(command), variable, (unsigned)_countof(variable), amount, (unsigned)_countof(amount));
    index = getArrayIndex(variable, &type);

    if (index != UINT32_MAX)
    {
        if (isVarAllowed(unitData, index))
        {
            _strupr(command);
            if (!strcmp(command, "SET"))
            {
                switch (type)
                {
                case T_INT8:
                    if (sscanf_s(amount, "%ld", &int8Amount) > 0)
                        setUnitDataC(index, unitData, int8Amount);
                    break;
                case T_INT16:
                    if (sscanf_s(amount, "%ld", &int16Amount) > 0)
                        setUnitDataS(index, unitData, int16Amount);
                    break;
                case T_INT32:
                    if (sscanf_s(amount, "%ld", &int32Amount) > 0)
                        setUnitDataL(index, unitData, int32Amount);
                    break;
                case T_FLOAT:
                    if (sscanf_s(amount, "%f", &floatAmount) > 0)
                        setUnitDataF(index, unitData, floatAmount);
                    break;
                case T_PTR_G:
                    if (sscanf_s(amount, "%ld", &int16Amount) > 0)
                    {
                        void* p = getGraphicPtr(int16Amount);
                        if (p)
                            setUnitDataL(index, unitData, (long)p);
                        else
                            log("Error: graphic ID (%d) >= n (%d)", index, int16Amount);
                    }
                    break;
                }
            }
            else if (!strcmp(command, "ADD"))
            {
                switch (type)
                {
                case T_INT8:
                    if (sscanf_s(amount, "%ld", &int8Amount) > 0)
                        modifyUnitDataC(index, unitData, int8Amount);
                    break;
                case T_INT16:
                    if (sscanf_s(amount, "%ld", &int16Amount) > 0)
                        modifyUnitDataS(index, unitData, int16Amount);
                    break;
                case T_INT32:
                    if (sscanf_s(amount, "%ld", &int32Amount) > 0)
                        modifyUnitDataL(index, unitData, int32Amount);
                    break;
                case T_FLOAT:
                    if (sscanf_s(amount, "%f", &floatAmount) > 0)
                        modifyUnitDataF(index, unitData, floatAmount);
                    break;
                case T_PTR_G:
                    log("Error: command ADD is not allowed for pointers");
                    break;
                }
            }
            else if (!strcmp(command, "MUL"))
            {
                switch (type)
                {
                case T_INT8:
                    if (sscanf_s(amount, "%f", &floatAmount) > 0)
                        multiplyUnitDataC(index, unitData, floatAmount);
                    break;
                case T_INT16:
                    if (sscanf_s(amount, "%f", &floatAmount) > 0)
                        multiplyUnitDataS(index, unitData, floatAmount);
                    break;
                case T_INT32:
                    if (sscanf_s(amount, "%f", &floatAmount) > 0)
                        multiplyUnitDataL(index, unitData, floatAmount);
                    break;
                case T_FLOAT:
                    if (sscanf_s(amount, "%f", &floatAmount) > 0)
                        multiplyUnitDataF(index, unitData, floatAmount);
                    break;
                case T_PTR_G:
                    log("Error: command MUL is not allowed for pointers");
                    break;
                }
            }
            else
                log("Error: unknown command: %s", command);
        }
        else
        {
            log("Warning: cannot modify var %s for type %d", variable, (int)unitData->type);
            return;
        }
    }
    else
    {
        char amount2[32];
        sscanf_s(s, "%s %s %s %s",
            command, (unsigned)_countof(command), variable, (unsigned)_countof(variable), amount, (unsigned)_countof(amount), amount2, (unsigned)_countof(amount2));

        if (unitData->type >= 50)
        {
            master_data_operations op;
            if (!strcmp(command, "SET"))
                op = OP_SET;
            else if (!strcmp(command, "ADD"))
                op = OP_ADD;
            else if (!strcmp(command, "MUL"))
                op = OP_MUL;
            else
            {
                log("Error: unknown command: %s", command);
                return;
            }
            int c;
            int v;
            if (sscanf_s(amount, "%d", &c) > 0 && sscanf_s(amount2, "%d", &v) > 0)
            {
                if (!strcmp(variable, "Attack"))
                    modify_attack(unitData, op, c, v);
                else if (!strcmp(variable, "Armor"))
                    modify_armor(unitData, op, c, v);
                else
                {
                    log("Error: unknown variable: %s", variable);
                    return;
                }
            }
        }
    }
}

void __stdcall advTriggerEffectActual(prop_object* unitData, const char* str)
{
    char* s_heap = NULL;
    char* s_stack = NULL;
    size_t s_len = strlen(str);
    char* s;
    if (s_len < 0x400)
    {
        s_stack = (char*)alloca(s_len + 1);
        s = s_stack;
    }
    else
    {
        s_heap = (char*)malloc(s_len + 1);
        s = s_heap;
    }
    strcpy_safe(s, s_len + 1, str);
    char* pch = strtok(s, "\r\n");
    char* com_strs[64];
    int str_count = 0;
    while (pch && str_count < 64)
    {
        com_strs[str_count++] = pch;
        pch = strtok(NULL, "\r\n");
    }
    for (int i = 0; i < str_count; i++)
        advTriggerEffectActual_sub(unitData, com_strs[i]);

    free(s_heap);
}

void __declspec(naked) advTriggerEffect()
{
    __asm
    {
        mov     eax, [edi + 6Ch]
        push    eax            //string
        push    esi            //property object
        call    advTriggerEffectActual
        ret
    }
}
