#pragma once
// ============================================================================
// RGE_Doppleganger_Object.h - the converted runtime Doppleganger tier (the
// ladder conversion, 2026-11-28; derives the converted RGE_Animated_Object).
// ============================================================================
// THE RECOVERY: the class behind the binary's runtime Doppleganger tier
// (sizeof 220 = 0xDC = the Animated 184 + the 36-byte doppleganger
// extension @0xB8..0xDB; the vftable .rdata:0065A2C0 = 223 entries
// (0x65A2C0..0x65A63C) = the Animated's 220 + THREE new appended virtuals).
//
// THE VTABLE STRUCTURE (the DD-list walk vs the Animated table):
// - the 4 overrides: slot 37 update, 39 rehook, 40 save, 167 setup_read.
// - slot 29 (recycle_in_to_game) is NOT overridden: it stays the ANIMATED
//   proc inherited. The Dopple's own recycle_in_to_game (the 7-arg form
//   tobj/obj_owner/x/y/z/obj_to_dopple) HIDES the Animated's 5-arg slot-29
//   virtual (two extra parameters) and APPENDS as slot 220 - the same
//   hide-and-append class as the Animated setup (15: the different
//   parameter list hides the base virtual and appends).
// - slot 221: validate - the brand-new virtual (no base counterpart).
// - slot 222: setup - the Dopple's setup (the RGE_Master_Doppleganger*
//   tobj) hides the Animated's setup (the RGE_Master_Animated* tobj,
//   the slot 219) and appends.
// - slot 0: the ANIMATED vdel inherited (the Dopple has no vdel export).
//
// THE EXTENSION FIELDS (the idb_types.h dopple block; 184..220): the
// doppleganger disguise state (the doppled_object source binding, the
// draw-color owner, the disguised master/player ids, the MAP_SIZE-gated
// MapAddress tile pointer + LastMapValue/CantSeeBits visibility state,
// the map draw/color trio, the Destroy_Next_Turn defer flag).
//
// THE LAYOUT CONTRACT: sizeof == 220; no RTTI (/GR-). The static_asserts
// below are the derived-layout gate compiled into every TU naming the class.
// ============================================================================

class RGE_Doppleganger_Object : public RGE_Animated_Object
{
public:
    // ---- the extension fields (the idb block; 184..220) ----
    RGE_Static_Object* doppled_object;              // +0xB8 = 184 (the disguise source binding)
    RGE_Player* draw_color_owner;                   // +0xBC = 188
    RGE_Master_Static_Object* doppled_master_obj;   // +0xC0 = 192
    int doppled_player;                             // +0xC4 = 196
    unsigned int* MapAddress;                       // +0xC8 = 200 (the MAP_SIZE-gated tile pointer)
    unsigned int LastMapValue;                      // +0xCC = 204
    unsigned int CantSeeBits;                       // +0xD0 = 208
    unsigned __int8 map_draw_level;                 // +0xD4 = 212
    unsigned __int8 map_color;                      // +0xD5 = 213
    unsigned __int8 Multi_Unified_Points;           // +0xD6 = 214
    unsigned __int8 Destroy_Next_Turn;              // +0xD7 = 215
    int doppledObjectID;                            // +0xD8 = 216

    // ---- the constructors (the placement-new trampoline targets) ----
    RGE_Doppleganger_Object(RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
                            float x, float y, float z, int do_setup, RGE_Static_Object* obj_to_dopple);   // 0x0046FBF0 (:1480)
    RGE_Doppleganger_Object(int infile, RGE_Game_World* gworld, int do_setup);   // 0x0046FE90 (:1482)

    // ---- the slot overrides (the signatures inherit the base declarations) ----
    virtual unsigned __int8 update() override;                               // slot 37
    virtual void rehook() override;                                          // slot 39
    virtual void save(int outfile) override;                                 // slot 40
    virtual int setup_read(int infile, RGE_Game_World* gworld) override;     // slot 167

    // ---- the NEW appended virtuals (the slots 220-222; the hide-and-append
    // ---- trio - see the header comment; deliberately NO override keyword)
    virtual void recycle_in_to_game(RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
                                    float x, float y, float z, RGE_Static_Object* obj_to_dopple);   // slot 220 (hides the Animated's 5-arg slot 29)
    virtual void validate();                                                 // slot 221 (the brand-new virtual)
    virtual int setup(RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
                      float x, float y, float z);                            // slot 222 (hides the Animated's slot-219 setup)
};

// ---- the derived-layout gate ----
static_assert(sizeof(RGE_Doppleganger_Object) == 220, "RGE_Doppleganger_Object: the size drifted from the binary (0xDC)");
static_assert(offsetof(RGE_Doppleganger_Object, doppled_object) == 0xB8, "RGE_Doppleganger_Object: the dopple extension drifted (the binary: +0xB8 = 184)");
static_assert(offsetof(RGE_Doppleganger_Object, doppledObjectID) == 0xD8, "RGE_Doppleganger_Object: doppledObjectID drifted (the binary: +0xD8 = 216)");
