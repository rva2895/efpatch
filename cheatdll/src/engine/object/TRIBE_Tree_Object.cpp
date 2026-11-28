#include "stdafx.h"

// ============================================================================
// TRIBE_Tree_Object.cpp - the converted runtime Tree tier (the TREE
// conversion step, 2026-11-28). The DERIVED-CLASS PROOF: the first derived
// conversion (the base = the converted RGE_Static_Object). The 2 exports:
// the ctor @0x00564DB0 + the set_object_state forwarder @0x00564E40.
// ============================================================================

// ----------------------------------------------------------------------------
// TRIBE_Tree_Object::TRIBE_Tree_Object - the runtime ctor. Original:
// @ 0x00564DB0 (idb_functions.h:4084). Range [0x564DB0, 0x564E30)
// (retn 18h = 6 stack args), returns this. ABI: __thiscall.
// Decode (asm 0x564DB0-0x564E30): the base Static ctor with do_setup = 0
// ALWAYS (hooked -> routes), the Tree vftable store (asm 0x564E00), then
// IF do_setup the base setup (asm 0x564E17 - the direct call to the base
// implementation). THE C++ CHAIN: the mem-init passes 0; the compiler
// stores the Tree vftable before the derived body (the standard
// construction order = the original's order); the body calls the base
// setup through the QUALIFIED member form (the direct implementation -
// never the unqualified virtual dispatch). The original ctor's SEH fold
// (chunk 0x644CC0 -> the Static destructor) is subsumed by the
// compiler-generated ctor-unwind of the C++ chain (the throwing base ctor
// destroys the base subobject - the same destructor, the same game-heap
// pairing through the base tripwire bodies).
// ----------------------------------------------------------------------------
TRIBE_Tree_Object::TRIBE_Tree_Object(TRIBE_Master_Tree_Object* tobj, TRIBE_Player* obj_owner,
                                     float x, float y, float z, int do_setup)
    : RGE_Static_Object((RGE_Master_Static_Object*)tobj, obj_owner, x, y, z, 0)   // asm 0x564DB5-0x564DFF (do_setup = 0 ALWAYS)
{
    if (do_setup != 0)
        RGE_Static_Object::setup((RGE_Master_Static_Object*)tobj, (RGE_Player*)obj_owner, x, y, z);   // asm 0x564E17 (the direct call to the base implementation; the family cast)
}

// ----------------------------------------------------------------------------
// TRIBE_Tree_Object::set_object_state - the slot-53 override. Original:
// @ 0x00564E40 (idb_functions.h:4085). Range [0x564E40, 0x564E4A)
// (retn 4), void. ABI: __thiscall. THE PURE FORWARDER: the original proc
// tail-calls RGE_Static_Object::set_object_state (0x550970 - the base
// implementation; routes through our hook when the game dispatches the
// entry). The qualified base call reproduces it exactly. (The Tree export
// rendered the parameter unsigned __int8 and the base constant types it
// char - the bit-identical byte pass-through, asm 0x564E34-0x564E45 moves
// the raw dword arg; the base member carries the corrected char form.)
// ----------------------------------------------------------------------------
void TRIBE_Tree_Object::set_object_state(char new_state)
{
    RGE_Static_Object::set_object_state(new_state);   // asm 0x564E45: jmp/call RGE_Static_Object__set_object_state (the base implementation)
}

// ---- the placement-new trampoline (the ::new global form - no
// class-deallocation lookup in the throwing-ctor cleanup; see the
// migration doc 15.3) ----
static TRIBE_Tree_Object* __fastcall TRIBE_Tree_Object_ctor_trampoline(
    TRIBE_Tree_Object* this_, DWORD dummy, TRIBE_Master_Tree_Object* tobj, TRIBE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{ return ::new (this_) TRIBE_Tree_Object(tobj, obj_owner, x, y, z, do_setup); }

// ---- the qualified-call trampoline (the recursion trap: the unqualified
// form on a derived instance would re-enter this trampoline) ----
static void __fastcall TRIBE_Tree_Object_set_object_state_trampoline(
    TRIBE_Tree_Object* this_, DWORD dummy, char new_state)
{ this_->TRIBE_Tree_Object::set_object_state(new_state); }

// ============ THE INSTALLER (the same 2 addresses; the trampoline targets) ============
void install_TRIBE_Tree_Object()
{
    setHook((void*)0x00564DB0, TRIBE_Tree_Object_ctor_trampoline);
    setHook((void*)0x00564E40, TRIBE_Tree_Object_set_object_state_trampoline);
}
