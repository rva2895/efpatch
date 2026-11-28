#pragma once
// ============================================================================
// RGE_Static_Object.h - the recovered class (the conversion commit, 2026-11-28)
// ============================================================================
// THE RECOVERY: the class behind the binary's runtime Static tier
// (sizeof 180, vftable .rdata:00662C8C, 219 slots - the DD-list walk
// lst :994786-:994952, cross-validated against RGE_Static_ObjectVtbl
// (idb_types.h:3958-4179) 219 = 219, index-for-index).
//
// THE LAYOUT CONTRACT (the mixed phase, sec. 3/sec. 3b of DECOMP_CPP_MIGRATION.md):
// - the member order = the IDB order (idb_types.h:1447-1501); the
//   compiler's implicit vfptr occupies offset 0 exactly where the IDB's
//   vfptr field was; sizeof == 180; no RTTI (/GR-).
// - the vtable slot order = the DD-list order; the slot contents =
//   the STATIC tier's implementations: ~99 real procs (our hooks) +
//   120 shared empty defaults (the nullsub family + `function`
//   @0x0060F920 - the AI-log empty sink), ICF-folded by shape.
// - the unit_extra paddings stay (the casts.cpp UNIT_EXTRA scheme is
//   live across them).
// - the base dtor is NON-VIRTUAL: the binary has no compiler-generated
//   dtor slots; the delete protocol = the slot-0 hand-rolled thunk,
//   recovered as the ordinary virtual vector_deleting_destructor
//   (the probe verdict sec. 8: approach A - the plain virtual dtor - is
//   disqualified on the measured free routing; the = delete tripwire
//   is measured inert; the private-undefined form fires C2248).
//
// THE INCLUDE POSITION (the name swap, sec. 3b - THE INVERTED ORDER,
// owner-approved 2026-11-28): the by-value member ei (LOS_ExploreInfo,
// 36 B @+0x90) requires the idb definitions FIRST, so stdafx.h orders:
//     #define RGE_Static_Object RGE_Static_Object_idb   // retire the struct
//     #include "src\idb\idb_types.h"                    // regen output renamed
//     #undef RGE_Static_Object
//     #include "src\engine\object\RGE_Static_Object.h"  // this header
//     #include "src\idb\idb_functions.h"                // prototypes see the class
// The idb struct survives as RGE_Static_Object_idb (alive in the decode
// references, dead in the build); token-safe: RGE_Static_ObjectVtbl /
// RGE_Static_Object__method / RGE_Static_Object_RGE_Static_Object are
// distinct preprocessor tokens. This header needs NO forward declarations
// - every type it names resolves from idb_types.h (included above it).
// The static_asserts below are the layout gate compiled into every TU.
// ============================================================================

#include <stddef.h>

class RGE_Static_Object
{
public:
    // ---- the constructors (the placement-new trampoline targets) ----
    RGE_Static_Object(RGE_Master_Static_Object* tobj, TRIBE_Player* obj_owner,
                      float x, float y, float z, int do_setup);          // 0x0054B2E0 (:3774)
    RGE_Static_Object(int infile, RGE_Game_World* gworld, int do_setup); // 0x0054B600 (:3787) - the stream ctor overload

    // ---- the tripwires (the hard gates) ----
    RGE_Static_Object(const RGE_Static_Object&) = delete;                // nothing copies these objects
    RGE_Static_Object& operator=(const RGE_Static_Object&) = delete;  // (suppresses the implicit moves too)
private:
    static void operator delete(void*);                                // the tripwire: any delete-expression = C2248
    static void operator delete[](void*);                              //   (the probe-verified form; "= delete" measured inert)
public:
    // slot 0 [0x0054B5E0]  impl: OUR HOOK
    virtual void * vector_deleting_destructor(unsigned int);
    // ---- the dtor body: non-virtual (no vtable slot); invoked by the thunk and by
    // ---- the derived chains via the qualified pseudo-dtor ----
    ~RGE_Static_Object();
    // slot 1 [0x0054B370]  impl: OUR HOOK
    virtual int play_unload_sound();
    // slot 2 [0x0054B3E0]  impl: OUR HOOK
    virtual int gbg_get_civ_override();
    // slot 3 [0x0054B480]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isSpaceport();
    // slot 4 [0x0054B460]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isGEAssaultMech();
    // slot 5 [0x0054B400]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isAircraft();
    // slot 6 [0x0054B450]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isAirCruiser();
    // slot 7 [0x0054B4A0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isResourceBuilding();
    // slot 8 [0x0054B4E0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isMilitaryUnit();
    // slot 9 [0x0054B550]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isCivilianUnit();
    // slot 10 [0x0054B5B0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isPredatorUnit();
    // slot 11 [0x0054B5C0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isStaticPredatorUnit();
    // slot 12 [0x0054B170]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isPadawan();
    // slot 13 [0x0054B1D0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isMaster();
    // slot 14 [0x0054B280]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isAttackGroundButtonAvailable();
    // slot 15 [0x0054BC40]  impl: OUR HOOK
    virtual unsigned __int8 is_shielded();
    // slot 16 [0x0054ED30]  impl: OUR HOOK
    virtual void gbg_doShieldRecharge();
    // slot 17 [0x0054EE70]  impl: OUR HOOK
    virtual void gbg_doShieldDropoff();
    // slot 18 [0x0054BAB0]  impl: OUR HOOK
    virtual unsigned __int8 is_invisible(int);
    // slot 19 [0x0054BBC0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isDetector();
    // slot 20 [0x0054BA60]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isObjectSpriteDisplayed(int);
    // slot 21 [0x0054BDA0]  impl: OUR HOOK
    //   the asm frame label: "flag = byte ptr 4" (the IDA prototype: unsigned __int8) - the idb
    //   Vtbl member's (int) typing is the artifact; the binary consumption is the byte
    virtual unsigned __int8 is_powered(unsigned __int8);
    // slot 22 [0x0054BE40]  impl: OUR HOOK
    virtual unsigned __int8 do_i_care_about_power();
    // slot 23 [0x0054B950]  impl: OUR HOOK
    virtual unsigned __int8 gbg_unknown23();
    // slot 24 [0x0054B9E0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isBiological();
    // slot 25 [0x0054B870]  impl: OUR HOOK
    virtual unsigned __int8 gbg_isDamaged();
    // slot 26 [0x0054B8A0]  impl: OUR HOOK
    virtual unsigned __int8 gbg_skip_unit_ai_update();
    // slot 27 [0x0054B8B0]  impl: OUR HOOK
    virtual unsigned __int8 can_be_healed(int);
    // slot 28 [0x0054B900]  impl: OUR HOOK
    virtual unsigned __int8 can_be_repaired(int);
    // slot 29 [0x0054BE50]  impl: OUR HOOK
    virtual void recycle_in_to_game(RGE_Master_Static_Object *, RGE_Player *, float, float, float);
    // slot 30 [0x0054C0A0]  impl: OUR HOOK
    virtual void recycle_out_of_game();
    // slot 31 [0x0054C9E0]  impl: OUR HOOK
    virtual void draw(TDrawArea *, __int16, __int16, RGE_Color_Table *);
    // slot 32 [0x0054CBD0]  impl: OUR HOOK
    virtual void shadow_draw(TDrawArea *, __int16, __int16, unsigned __int8);
    // slot 33 [0x0054CC70]  impl: OUR HOOK
    virtual void normal_draw(TDrawArea *, __int16, __int16);
    // slot 34 [0x0054CD50]  impl: OUR HOOK
    virtual void draw_front_frame(TDrawArea *, __int16, __int16);
    // slot 35 [0x0054CDD0]  impl: OUR HOOK
    virtual void draw_back_frame(TDrawArea *, __int16, __int16);
    // slot 36 [0x0054EB70]  impl: OUR HOOK
    virtual void draw_frame(TDrawArea *, __int16, __int16);
    // slot 37 [0x0054EF00]  impl: OUR HOOK
    virtual unsigned __int8 update();
    // slot 38 [0x0054F1F0]  impl: OUR HOOK
    virtual void check_damage_sprites();
    // slot 39 [0x0054F4B0]  impl: OUR HOOK
    virtual void rehook();
    // slot 40 [0x0054F550]  impl: OUR HOOK
    virtual void save(int);
    // slot 41 [0x0054F770]  impl: OUR HOOK
    virtual void teleport_off_map();
    // slot 42 [0x0054F820]  impl: OUR HOOK
    virtual double teleport(float, float, float);
    // slot 43 [0x0054FD20]  impl: OUR HOOK
    virtual double handle_teleport_z_correction(float);
    // slot 44 [0x0054FDA0]  impl: OUR HOOK
    virtual void new_sprite(RGE_Sprite *);
    // slot 45 [0x0054FED0]  impl: OUR HOOK
    virtual void add_overlay_sprite(RGE_Sprite *, unsigned __int8);
    // slot 46 [0x0054FF00]  impl: OUR HOOK
    virtual void remove_overlay_sprite(RGE_Sprite *);
    // slot 47 [0x0054FF20]  impl: OUR HOOK
    virtual void change_ownership(RGE_Player *);
    // slot 48 [0x005501A0]  impl: OUR HOOK
    virtual void modify(float, unsigned __int8);
    // slot 49 [0x00550290]  impl: OUR HOOK
    virtual void modify_delta(float, unsigned __int8);
    // slot 50 [0x00550320]  impl: OUR HOOK
    virtual void modify_percent(float, unsigned __int8);
    // slot 51 [0x005503B0]  impl: OUR HOOK
    virtual void transform(RGE_Master_Static_Object *);
    // slot 52 [0x005505B0]  impl: OUR HOOK
    virtual void copy_obj(RGE_Master_Static_Object *);
    // slot 53 [0x00550970]  impl: OUR HOOK
    //   the asm frame label: "new_object_state = byte ptr 4" (the IDA prototype: char) - the idb
    //   Vtbl member's u8 typing is the artifact; the shipped decode used char
    virtual void set_object_state(char);
    // slot 54 [0x00550730]  impl: OUR HOOK
    virtual void remove_visible_resource();
    // slot 55 [0x00550880]  impl: OUR HOOK
    virtual void create_doppleganger_when_dying();
    // slot 56 [0x005506B0]  impl: OUR HOOK
    virtual void destroy_obj();
    // slot 57 [0x00550700]  impl: OUR HOOK
    virtual void die_die_die();
    // slot 58 [0x005517F0]  impl: OUR HOOK
    virtual void damage(int, RGE_Armor_Weapon_Info *, float, RGE_Player *, RGE_Static_Object *, int);
    // slot 59 [0x005518F0]  impl: OUR HOOK
    virtual double calculateDamage(int, RGE_Armor_Weapon_Info *, float, RGE_Player *, RGE_Static_Object *);
    // slot 60 [0x00551930]  impl: OUR HOOK
    virtual void rotate(int);
    // slot 61 [0x00408A30]  impl: the forwarder target: nullsub_27 (retn)
    virtual unsigned __int8 can_attack();
    // slot 62 [0x00551990]  impl: OUR HOOK
    virtual void set_attribute(__int16, float);
    // slot 63 [0x005519B0]  impl: OUR HOOK
    virtual void set_attribute_amount(float, unsigned __int8, unsigned __int8);
    // slot 64 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int heal(int, int);
    // slot 65 [0x00551A00]  impl: OUR HOOK
    //   the body = the proc IDA labeled "RGE_Static_Object__heal" @0x551A00 (our hook; the heal/heal2 label alias - the slot-64 heal = the nullsub_23 forwarder)
    virtual unsigned __int8 heal2(float);
    // slot 66 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int canRepair();
    // slot 67 [0x00408FC0]  impl: the forwarder target: nullsub_1 (retn 8)
    virtual void notify_of_relation(int, unsigned __int8);
    // slot 68 [0x00422A40]  impl: OUR HOOK
    virtual void do_command(RGE_Static_Object *, float, float, float);
    // slot 69 [0x00422A50]  impl: OUR HOOK
    virtual int actionTypeIfOrdered(RGE_Static_Object *, float, float, float, float *);
    // slot 70 [0x00422A60]  impl: OUR HOOK
    virtual unsigned __int8 move_to(RGE_Static_Object *, float, float, float);
    // slot 71 [0x00422A70]  impl: the forwarder target: nullsub_16 (retn 14h)
    virtual void work(RGE_Static_Object *, float, float, float);
    // slot 72 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void stop();
    // slot 73 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void set_attack(RGE_Static_Object *);
    // slot 74 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void play_command_sound();
    // slot 75 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void play_move_sound();
    // slot 76 [0x0058A170]  impl: the forwarder target: nullsub_3 (retn 4)
    virtual void new_angle(float);
    // slot 77 [0x0054F440]  impl: OUR HOOK
    virtual RGE_Static_Object * spawn_death_obj();
    // slot 78 [0x00422A80]  impl: OUR HOOK
    virtual RGE_Master_Static_Object * get_command_master(RGE_Static_Object *, float, float, float);
    // slot 79 [0x00551A60]  impl: OUR HOOK
    virtual void set_being_worked_on(RGE_Action_Object *, __int16, unsigned __int8);
    // slot 80 [0x00551A80]  impl: OUR HOOK
    virtual void release_being_worked_on(RGE_Static_Object *);
    // slot 81 [0x00405D30]  impl: the forwarder target: nullsub_36 (retn)
    virtual unsigned __int8 is_moving();
    // slot 82 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual RGE_Static_Object * get_target_obj();
    // slot 83 [0x005513C0]  impl: OUR HOOK
    virtual unsigned __int8 more_room(RGE_Static_Object *);
    // slot 84 [0x00551410]  impl: OUR HOOK
    virtual void enter_obj(RGE_Static_Object *, unsigned __int8);
    // slot 85 [0x00551500]  impl: OUR HOOK
    virtual void exit_obj();
    // slot 86 [0x00551520]  impl: OUR HOOK
    virtual void add_obj(RGE_Static_Object *);
    // slot 87 [0x00551560]  impl: OUR HOOK
    virtual void remove_obj(RGE_Static_Object *);
    // slot 88 [0x005515B0]  impl: OUR HOOK
    virtual int explore_terrain(RGE_Player *, unsigned __int8, int, int);
    // slot 89 [0x005516E0]  impl: OUR HOOK
    virtual void unexplore_terrain(RGE_Player *, unsigned __int8, int, int);
    // slot 90 [0x00551580]  impl: OUR HOOK
    virtual LOSTBL * get_los_table();
    // slot 91 [0x00408A50]  impl: OUR HOOK
    virtual unsigned __int8 get_visibility(RGE_Player *);
    // slot 92 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual int inAttackRange(RGE_Static_Object *);
    // slot 93 [0x005524F0]  impl: OUR HOOK
    virtual unsigned __int8 underAttack();
    // slot 94 [0x00552500]  impl: OUR HOOK
    virtual void setUnderAttack(unsigned __int8);
    // slot 95 [0x00408A90]  impl: OUR HOOK
    virtual double calc_attack_modifier(RGE_Static_Object *);
    // slot 96 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double getSpeed();
    // slot 97 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double getAngle();
    // slot 98 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double maximumSpeed();
    // slot 99 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double rateOfFire();
    // slot 100 [0x00422AF0]  impl: the forwarder target: nullsub_29 (retn 4)
    virtual double damageCapability(RGE_Static_Object *);
    // slot 101 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double weaponRange();
    // slot 102 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double minimumWeaponRange();
    // slot 103 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int passableTile(float, float, int *, int);
    // slot 104 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int passableTile2(float, float);
    // slot 105 [0x00408AA0]  impl: the forwarder target: nullsub_30 (retn)
    virtual int currentTargetID();
    // slot 106 [0x00408AA0]  impl: the forwarder target: nullsub_30 (retn)
    virtual int currentTargetID2();
    // slot 107 [0x00408AB0]  impl: the forwarder target: nullsub_31 (retn)
    virtual double currentTargetX();
    // slot 108 [0x00408AB0]  impl: the forwarder target: nullsub_31 (retn)
    virtual double currentTargetY();
    // slot 109 [0x00408AB0]  impl: the forwarder target: nullsub_31 (retn)
    virtual double currentTargetZ();
    // slot 110 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setWaitingToMove(unsigned __int8);
    // slot 111 [0x00408A30]  impl: the forwarder target: nullsub_27 (retn)
    virtual unsigned __int8 waitingToMove();
    // slot 112 [0x00408A30]  impl: the forwarder target: nullsub_27 (retn)
    virtual unsigned __int8 actionState();
    // slot 113 [0x00408AC0]  impl: the forwarder target: nullsub_12 (retn 4)
    virtual unsigned __int8 keepGatheringWhenObjectIsOut(int);
    // slot 114 [0x00408AD0]  impl: the forwarder target: nullsub_5 (retn 4)
    virtual unsigned __int8 produceWhenKilledBy(int);
    // slot 115 [0x00408AE0]  impl: the forwarder target: nullsub_10 (retn)
    virtual unsigned __int8 useSameZoneDropsite();
    // slot 116 [0x00552E10]  impl: OUR HOOK
    virtual double searchRadius();
    // slot 117 [0x00408A30]  impl: the forwarder target: nullsub_27 (retn)
    virtual unsigned __int8 is_idle();
    // slot 118 [0x00552510]  impl: OUR HOOK
    virtual void __cdecl logDebug(char *, ...);
    // slot 119 [0x0060F920]  impl: the forwarder target: function ()
    //   the DD target = the `function` proc @0x0060F920 (the AI-log empty sink; the same cdecl form)
    virtual void __cdecl debug(char *, ...);
    // slot 120 [0x00408AF0]  impl: the forwarder target: nullsub_13 ()
    //   the DD target = nullsub_13 @0x00408AF0 (the COLLAPSED 3-byte retn-0; the address recovered from the collapsed-form line)
    virtual void notify(int, int, int, int, int, int);
    // slot 121 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int attack(float, float, float, int);
    // slot 122 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int attack2(int, int);
    // slot 123 [0x00408B30]  impl: the forwarder target: nullsub_15 (retn 18h)
    virtual int moveTo(float, float, float, float, int, int);
    // slot 124 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual int moveTo2(int, float, int);
    // slot 125 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int moveTo3(int, int);
    // slot 126 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int moveAwayFrom(int, int);
    // slot 127 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int hunt(int, int);
    // slot 128 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int gather(int, int);
    // slot 129 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int convert(int, int);
    // slot 130 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int repair(int, int);
    // slot 131 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int build(int, int);
    // slot 132 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int trade(int, int);
    // slot 133 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual int explore(int, int, int);
    // slot 134 [0x00408B00]  impl: the forwarder target: nullsub_23 (retn 8)
    virtual int enter(int, int);
    // slot 135 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int unload(int, float, float, float);
    // slot 136 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int transport(float, float, float, int);
    // slot 137 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int stopAction();
    // slot 138 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int pause();
    // slot 139 [0x00408B30]  impl: the forwarder target: nullsub_15 (retn 18h)
    virtual int canPath(int, float, float *, int, int, bool);
    // slot 140 [0x00422A90]  impl: the forwarder target: nullsub_37 (retn 24h)
    virtual int canPath2(XYZPoint, float, int, float *, int, int, bool);
    // slot 141 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual int canQuickPath(int, float, float *);
    // slot 142 [0x00408B30]  impl: the forwarder target: nullsub_15 (retn 18h)
    virtual int canPathWithObstructions(int, float, float *, int, int, ManagedArray_int_ *);
    // slot 143 [0x00422AA0]  impl: the forwarder target: nullsub_38 (retn 38h)
    virtual int canPathWithAdditionalPassability(XYZPoint, float, int, float *, int, int, int, int, int, int, int, BPath *);
    // slot 144 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int findFirstTerrainAlongExceptionPath(int, float *, float *, BPath *);
    // slot 145 [0x00408B30]  impl: the forwarder target: nullsub_15 (retn 18h)
    virtual int canLinePath(int, int, int, int, float, int);
    // slot 146 [0x00408B40]  impl: the forwarder target: nullsub_22 (retn 14h)
    virtual int canLinePath2(const XYPoint *, const XYPoint *, float, XYPoint *, int);
    // slot 147 [0x00408B30]  impl: the forwarder target: nullsub_15 (retn 18h)
    virtual int firstTileAlongLine(const XYPoint *, const XYPoint *, XYPoint *, int, int, int);
    // slot 148 [0x00408A30]  impl: the forwarder target: nullsub_27 (retn)
    virtual unsigned __int8 have_action();
    // slot 149 [0x00408AA0]  impl: the forwarder target: nullsub_30 (retn)
    virtual int gameActionType();
    // slot 150 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual BVector * userDefinedWaypoint(int);
    // slot 151 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual int addUserDefinedWaypoint(float, float, float);
    // slot 152 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void zeroUserDefinedWaypoints();
    // slot 153 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void removeAllUserDefinedWaypoints();
    // slot 154 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int numberUserDefinedWaypoints();
    // slot 155 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual BPath * getPatrolInformation(int *);
    // slot 156 [0x00422AB0]  impl: the forwarder target: nullsub_39 (retn 0Ch)
    virtual double distanceToHighestLevelPath(BVector);
    // slot 157 [0x00488590]  impl: the forwarder target: nullsub_40 (retn)
    virtual double distanceToHighestLevelPath2();
    // slot 158 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual BPath * findAvoidancePath(XYZPoint *, float, int);
    // slot 159 [0x0057AC00]  impl: the forwarder target: nullsub_34 (retn)
    virtual int get_action_checksum();
    // slot 160 [0x0057AC00]  impl: the forwarder target: nullsub_34 (retn)
    virtual int get_waypoint_checksum();
    // slot 161 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setPatrolMode(unsigned __int8);
    // slot 162 [0x00552EA0]  impl: OUR HOOK
    virtual int garrisoned_count();
    // slot 163 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int lock_down();
    // slot 164 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int unlock_down();
    // slot 165 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int get_locked_down_count();
    // slot 166 [0x00422AD0]  impl: OUR HOOK
    //   the DD target = the shared zero-return proc @0x00422AD0 (fld 0.0 / retn 10h; ICF-folded with slot 194)
    virtual double getTradeAmount(float, float, float, float);
    // slot 167 [0x0054C510]  impl: OUR HOOK
    //   the DD target = the setup_read proc @0x0054C510 (the IDB member name displaced - the audit correction)
    virtual int setup_read(int, RGE_Game_World *);
    // slot 168 [0x0054C2A0]  impl: OUR HOOK
    //   the DD target = the setup proc @0x0054C2A0 (the IDB member name "setup2" displaced)
    virtual int setup(RGE_Master_Static_Object *, RGE_Player *, float, float, float);
    // slot 169 [0x0054C850]  impl: OUR HOOK
    virtual RGE_New_Object_List * create_object_list();
    // slot 170 [0x0054C8B0]  impl: OUR HOOK
    virtual RGE_Active_Sprite_List * create_sprite_list();
    // slot 171 [0x00552C40]  impl: OUR HOOK
    virtual BVector * position(BVector *);
    // slot 172 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual BPathData * getPathData();
    // slot 173 [0x00408AA0]  impl: the forwarder target: nullsub_30 (retn)
    virtual int actionType();
    // slot 174 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual int hasActionOfType(int);
    // slot 175 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual int hasSubActionOfType(int);
    // slot 176 [0x00408B30]  impl: the forwarder target: nullsub_15 (retn 18h)
    virtual int addMove_old(int, float, int, int, int);
    // slot 177 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int addCurrentPath(BPath *, int, float, int);
    // slot 178 [0x00408B10]  impl: the forwarder target: nullsub_32 (retn 10h)
    virtual int addFuturePath(BPath *, int, float, int);
    // slot 179 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void stopAllMovement();
    // slot 180 [0x00552C60]  impl: OUR HOOK
    virtual void setOrientation(BVector *, BVector *);
    // slot 181 [0x00422B30]  impl: OUR HOOK
    //   the DD target = the shared BVector-zeroing proc @0x00422B30 (ICF-folded with slots 183 and 207)
    virtual BVector * velocity(BVector *);
    // slot 182 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setVelocity(BVector *);
    // slot 183 [0x00422B30]  impl: OUR HOOK
    //   the DD target = the shared BVector-zeroing proc @0x00422B30 (ICF-folded with slots 181 and 207)
    virtual BVector * acceleration(BVector *);
    // slot 184 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setAcceleration(BVector *);
    // slot 185 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double forwardVelocity();
    // slot 186 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setForwardVelocity(float);
    // slot 187 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double forwardAcceleration();
    // slot 188 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setForwardAcceleration(float);
    // slot 189 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void moveForward(float);
    // slot 190 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setPosition(BVector *);
    // slot 191 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void yaw(float);
    // slot 192 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int isMoving();
    // slot 193 [0x004BF920]  impl: the forwarder target: nullsub_7 (retn)
    virtual void stopMoving();
    // slot 194 [0x00422AD0]  impl: OUR HOOK
    //   the DD target = the shared zero-return proc @0x00422AD0 (ICF-folded with slot 166)
    virtual double calculateFutureVelocity(float, float, float, BVector *);
    // slot 195 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual unsigned int lastMoveTime();
    // slot 196 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void pauseMovement(float);
    // slot 197 [0x00422AE0]  impl: the forwarder target: nullsub_41 (retn)
    virtual int movementPriority();
    // slot 198 [0x00408AA0]  impl: the forwarder target: nullsub_30 (retn)
    virtual int intelligentMovementUpdate();
    // slot 199 [0x00422AF0]  impl: the forwarder target: nullsub_29 (retn 4)
    virtual double calculateMaximumSpeed(int);
    // slot 200 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual int calculateForward(BVector *, BVector *, BVector *);
    // slot 201 [0x00422AD0]  impl: OUR HOOK
    virtual double calculateYawAngle(BVector *, BVector *, float, int *);
    // slot 202 [0x00422B00]  impl: the forwarder target: nullsub_42 (retn 0Ch)
    virtual int calculateReachedPoint(BVector *, BVector *, BVector *);
    // slot 203 [0x00422AC0]  impl: the forwarder target: nullsub_28 (retn)
    virtual double calculateTurnRadius();
    // slot 204 [0x00408B20]  impl: the forwarder target: nullsub_33 (retn 0Ch)
    virtual int checkAcceleration(BVector *, int, int);
    // slot 205 [0x00422B10]  impl: OUR HOOK
    virtual BVector * straightProjectedPosition(BVector *, BVector *, float);
    // slot 206 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int onPatrol();
    // slot 207 [0x00422B30]  impl: OUR HOOK
    //   the DD target = the orientationForward proc @0x00422B30 (OUR HOOK; the ICF fold also serves slots 181/183)
    virtual BVector * orientationForward(BVector *);
    // slot 208 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int setRunSprite();
    // slot 209 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int setMoveSprite();
    // slot 210 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int setFidgetSprite();
    // slot 211 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int getGateLocked();
    // slot 212 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void setGateLocked(int);
    // slot 213 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual int isGateOpenable(int);
    // slot 214 [0x00408B60]  impl: the forwarder target: nullsub_46 (retn 4)
    virtual int getLinkedChild(int);
    // slot 215 [0x00408A80]  impl: the forwarder target: nullsub_25 (retn 4)
    virtual int get_next_selected_group(int);
    // slot 216 [0x00422B50]  impl: the forwarder target: nullsub_35 (retn)
    virtual __int16 get_Selected_Groups();
    // slot 217 [0x00408B50]  impl: the forwarder target: nullsub_14 (retn 4)
    virtual void set_Selected_Groups(__int16);
    // slot 218 [0x00408A40]  impl: the forwarder target: nullsub_24 (retn)
    virtual int get_num_Selected_groups();

    // ---- the non-virtual members (the direct-call methods: NOT slots in
    // any Static-tier vftable - the binary evidence; the 219-virtual count
    // is the gate) ----
    UnitAIModule* unitAI();
    void removeAllFromPathingGroup();
    unsigned __int8 is_dying();
    double distance_to_object(RGE_Static_Object* obj);
    RGE_Static_Object* get_object_pointer(int obj_id);
    void set_location(float x, float y, float z);
    __int16 get_animation_frame();
    void set_animation_frame(__int16 frame);
    RGE_Sprite* get_sprite_pointer(__int16 sprite_id);
    int inLockedFormation();
    void set_facet(char new_facet);
    void capture_frame(TDrawArea* render_area, __int16 x, __int16 y);
    void remove_shadows();
    int get_frame(__int16* min_x, __int16* min_y, __int16* max_x, __int16* max_y);
    void give_attribute_to_owner();
    void take_attribute_from_owner();
    void change_unique_id();
    void set_sleep_flag(char flag_in);
    int isFlagSet(int n);
    unsigned __int8 lookupZone(int x, int y);
    unsigned __int8 currentZone();
    int withinRangeOfZone(unsigned __int8 zone, float range, XYPoint* goal);
    int findClosestPointInTerrainType(XYPoint p, XYPoint* rVal, int type1, int type2, int range);
    void addToPathingGroup(int uID);
    int inRange(RGE_Static_Object* obj, float range);
    double distance_to_position(float x, float y, float z);
    double distance_to_object2(RGE_Static_Object* obj, float x, float y);
    unsigned __int8 hit_test(__int16 draw_x, __int16 draw_y, __int16 mouse_x, __int16 mouse_y, __int16 check_radius);
    unsigned __int8 drop_held_objects(int dropZone);
    unsigned __int8 box_hit_test(__int16 draw_x, __int16 draw_y, __int16 start_x, __int16 start_y, __int16 end_x, __int16 end_y);
    char get_3d_sound_info(int *pan, int *volume);
    void draw_frame_3d_square_back(TDrawArea *render_area, __int16 x, __int16 y);
    unsigned __int8 ** fill_pathable_map(int x, int y, int width, int height, int offset_x, int offset_y, RGE_Map_Analysis *map_analysis, unsigned __int8 **pathable, unsigned __int8 first_time);
    unsigned __int8 find_drop_location(int dropZone, float* x, float* y, RGE_Master_Static_Object* m_obj, RGE_Static_Object* obj, float drop_x, float drop_y, float radius_x, float radius_y);
    void draw_frame_3d_square_front(TDrawArea* render_area, __int16 x, __int16 y);
    void draw_frame_3d_cube_back(TDrawArea* render_area, __int16 x, __int16 y);
    void draw_frame_3d_cube_front(TDrawArea* render_area, __int16 x, __int16 y);
    void capture_frame_3d_square(TDrawArea* render_area, __int16 x, __int16 y);
    RGE_Static_Object* find_closest_resource(RGE_Static_Object *target_obj, int range);

    // ---- THE MIXED-PHASE BRIDGE (owner-approved 2026-11-28): the idb vtbl
    // structs compile under the RGE_Static_Object_idb rename, so their
    // RGE_Static_Object positions take the renamed struct; this conversion
    // lets the UNCONVERTED code keep passing this class at those positions
    // without per-site casts. Layout-identical (the class mirrors the struct
    // byte-for-byte - the layout gate below), so the conversion is a no-op
    // reinterpretation. The reverse direction (an _idb-typed RESULT into a
    // class-typed slot) stays an explicit cast at the site.
    operator RGE_Static_Object_idb*(void) { return reinterpret_cast<RGE_Static_Object_idb*>(this); }

    // ---- the members (the IDB declaration order; ALL PUBLIC per the
    // mixed phase) ----
    int gbg_civ_override;                              // +0x04
    float shield_recharge_accumulator;                 // +0x08 (the shield charge-tick bank)
    float shield_hit_timer;                            // +0x0C (the shield-hit flash timer)
    int id;                                            // +0x10
    RGE_Master_Static_Object* master_obj;              // +0x14
    TRIBE_Player* owner;                               // +0x18
    RGE_Sprite* sprite;                                // +0x1C
    RGE_Sprite* old_sprite;                            // +0x20
    RGE_Active_Sprite_List* sprite_list;               // +0x24
    RGE_Tile* tile;                                    // +0x28
    RGE_Static_Object* inside_obj;                     // +0x2C
    RGE_New_Object_List* objects;                      // +0x30
    __int16 screen_x_offset;                           // +0x34
    __int16 screen_y_offset;                           // +0x36
    __int16 shadow_x_offset;                           // +0x38
    __int16 shadow_y_offset;                           // +0x3A
    float hp;                                          // +0x3C
    float sp;                                          // +0x40
    unsigned __int8 curr_damage_percent;               // +0x44
    unsigned __int8 facet;                             // +0x45
    unsigned __int8 selected;                          // +0x46
    float world_x;                                     // +0x48
    float world_y;                                     // +0x4C
    float world_z;                                     // +0x50
    float attribute_amount_held;                       // +0x54
    unsigned __int8 object_state;                      // +0x58
    unsigned __int8 sleep_flag;                        // +0x59
    unsigned __int8 dopple_flag;                       // +0x5A
    unsigned __int8 goto_sleep_flag;                   // +0x5B
    __int16 attribute_type_held;                       // +0x5C
    unsigned __int8 type;                              // +0x5E (the tier mirror: 10/15/20/25/30/40/50/60/70/80)
    unsigned __int8 worker_num;                        // +0x5F
    ObsRecord* CurrentObstructionData;                 // +0x60
    ObsRecord* ObstructionDataList;                    // +0x64
    RGE_Static_Object** player_object_node;            // +0x68
    int* mPathingGroup;                                // +0x6C
    int mPathingGroupSize;                             // +0x70
    int mMaximumPathingGroupSize;                      // +0x74
    unsigned __int8 damaged_lately_timer;              // +0x78 (the UNIT_EXTRA scheme lives across
    unsigned __int8 unit_extra_padding_1;              // +0x79  0x79-0x7B and +0x87 - NOT reclaimed)
    unsigned __int8 unit_extra_padding_2;              // +0x7A
    unsigned __int8 unit_extra_padding_3;              // +0x7B
    UnitAIModule* unitAIValue;                         // +0x7C
    int zoneMapIndex;                                  // +0x80
    unsigned __int8 inObstructionMapValue;             // +0x84
    unsigned __int8 lastInObstructionMapValue;         // +0x85
    unsigned __int8 underAttackValue;                  // +0x86
    unsigned __int8 unit_extra_padding_4;              // +0x87
    int mGroupID;                                      // +0x88
    unsigned __int8 mROOAlreadyCalled;                 // +0x8C (the mROO gate - the ctor store, the desync class)
    LOS_ExploreInfo ei;                                // +0x90 (36 B by value; theObj repurposed by the LOS mod)
};                                                     // sizeof == 180 (0xB4)

// ---- the layout gate (compiled into every TU; the FULL CENSUS SET - every
// member walked against idb_types.h:1447-1501 under the brace rule, the
// anchors re-verified 2026-11-28; the implicit vfptr occupies +0x00) ----
static_assert(sizeof(RGE_Static_Object) == 180, "RGE_Static_Object: the size drifted from the binary (0xB4)");
static_assert(offsetof(RGE_Static_Object, gbg_civ_override)          == 0x04, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, shield_recharge_accumulator) == 0x08, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, shield_hit_timer)          == 0x0C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, id)                        == 0x10, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, master_obj)                == 0x14, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, owner)                     == 0x18, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, sprite)                    == 0x1C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, old_sprite)                == 0x20, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, sprite_list)               == 0x24, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, tile)                      == 0x28, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, inside_obj)                == 0x2C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, objects)                   == 0x30, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, screen_x_offset)           == 0x34, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, screen_y_offset)           == 0x36, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, shadow_x_offset)           == 0x38, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, shadow_y_offset)           == 0x3A, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, hp)                        == 0x3C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, sp)                        == 0x40, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, curr_damage_percent)       == 0x44, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, facet)                     == 0x45, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, selected)                  == 0x46, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, world_x)                   == 0x48, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, world_y)                   == 0x4C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, world_z)                   == 0x50, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, attribute_amount_held)     == 0x54, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, object_state)              == 0x58, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, sleep_flag)                == 0x59, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, dopple_flag)               == 0x5A, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, goto_sleep_flag)           == 0x5B, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, attribute_type_held)       == 0x5C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, type)                      == 0x5E, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, worker_num)                == 0x5F, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, CurrentObstructionData)    == 0x60, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, ObstructionDataList)       == 0x64, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, player_object_node)        == 0x68, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, mPathingGroup)             == 0x6C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, mPathingGroupSize)         == 0x70, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, mMaximumPathingGroupSize)  == 0x74, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, damaged_lately_timer)      == 0x78, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, unit_extra_padding_1)      == 0x79, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, unit_extra_padding_2)      == 0x7A, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, unit_extra_padding_3)      == 0x7B, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, unitAIValue)               == 0x7C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, zoneMapIndex)              == 0x80, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, inObstructionMapValue)     == 0x84, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, lastInObstructionMapValue) == 0x85, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, underAttackValue)          == 0x86, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, unit_extra_padding_4)      == 0x87, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, mGroupID)                  == 0x88, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, mROOAlreadyCalled)         == 0x8C, "the layout drifted");
static_assert(offsetof(RGE_Static_Object, ei)                        == 0x90, "the layout drifted");

// ---- the delete-protocol note (the tripwire above is the hard gate;
// the probe-verified form; "= delete" measured INERT: MSVC ignores deleted
// deallocation functions in delete-expressions and falls through to the
// global CRT delete. The destruction protocol = the slot-0 thunk via
// operator_delete_internal, never a language delete.) ----
