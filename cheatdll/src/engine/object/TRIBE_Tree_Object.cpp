#include "stdafx.h"

// .rdata vftable of TRIBE_Tree_Object (lst :995582)
static TRIBE_Tree_ObjectVtbl* const rge_tribe_tree_object_vftable =
    (TRIBE_Tree_ObjectVtbl*)0x00663B64;

// Replacement for TRIBE_Tree_Object::TRIBE_Tree_Object (runtime ctor).
// Original: @ 0x00564DB0 (idb_functions.h:4084). Range [0x564DB0,
// 0x564E30) (lst :596653/:596720, retn 18h = 6 stack args), returns
// this. ABI: __thiscall - this_ in ECX. Caller: TRIBE_Master_Tree_
// Object__make_new_obj +4F (owned, hook-routed; lst :596654).
// Decode (asm 0x564DB0-0x564E30): base runtime Static ctor with
// do_setup = 0 (hooked -> routes), the Tree vfptr store, IF do_setup
// the runtime Static setup (idb constant :1910-region, asm 0x564E17 -
// the asm calls Static's setup directly). Tree = Static mirror, no
// extension fields (180 = 0xB4). SEH fold: chunk 0x644CC0 ->
// RGE_Static_Object__destructor (0x54B680 - HOOKED -> routes; the
// IMMEDIATE base level).
TRIBE_Tree_Object* __fastcall TRIBE_Tree_Object__TRIBE_Tree_Object_new(
    TRIBE_Tree_Object* this_, DWORD dummy,
    TRIBE_Master_Tree_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    TRIBE_Tree_Object* result = this_;
    __try
    {
        RGE_Static_Object__RGE_Static_Object(
            (RGE_Static_Object*)this_, (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_tribe_tree_object_vftable;    // asm 0x564E00
        if (do_setup != 0)
            RGE_Static_Object__setup((RGE_Static_Object*)this_,
                (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z);   // asm 0x564E17
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x644CC0
    {
    }
    return result;
}

// Replacement for TRIBE_Tree_Object::set_object_state. Original:
// @ 0x00564E40 (idb_functions.h:4085). Range [0x564E40, 0x564E4A)
// (lst :596729/:596738, retn 4), void. ABI: __thiscall. Virtual:
// vftable slot (lst :596730). Body: a PURE FORWARDER to RGE_Static_
// Object__set_object_state (:3847, 0x550970 - unhooked, executes the
// original; routes if ever hooked). No SEH. (The Tree export types the
// parameter unsigned __int8; the base constant types it char -
// bit-identical byte pass-through, asm 0x564E34-0x564E45 moves the raw
// dword arg.)
void __fastcall TRIBE_Tree_Object__set_object_state_new(
    TRIBE_Tree_Object* this_, DWORD dummy, unsigned __int8 new_object_state)
{
    RGE_Static_Object__set_object_state((RGE_Static_Object*)this_, new_object_state);
}

void install_TRIBE_Tree_Object()
{
    setHook((void*)0x00564DB0, TRIBE_Tree_Object__TRIBE_Tree_Object_new);
    setHook((void*)0x00564E40, TRIBE_Tree_Object__set_object_state_new);
}
