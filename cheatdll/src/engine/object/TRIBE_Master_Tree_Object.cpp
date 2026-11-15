#include "stdafx.h"
#include "TRIBE_Master_Tree_Object.h"

// .rdata vftable of TRIBE_Master_Tree_Object
// (pseudocode :392759 - "6674A4: ... TRIBE_Master_Tree_Object::vftable")
static TRIBE_Master_Tree_ObjectVtbl* const tribe_master_tree_object_vftable =
    (TRIBE_Master_Tree_ObjectVtbl*)0x006674A4;

// Replacement for TRIBE_Master_Tree_Object::scalar_deleting_destructor
// Original: @ 0x005C9780 (idb_functions.h:5140). Range [0x5C9780, 0x5C979B)
// (lst :766699/:766717, retn 4), void* result. ABI: __thiscall - this_ in
// ECX. Virtual: vftable DATA XREF (lst :766700). NO Tree destructor export
// exists - destruction is entirely the base Static destructor's (the asm
// calls it through the j_ thunk at 0x5C97A0; our call goes through the
// idb constant 0x0048D200, idb_functions.h:1914 - hooked, routes into
// RGE_Master_Static_Object__destructor_new per the HOOK-ROUTING ruling).
// flags & 1 -> operator_delete (operator_delete_internal = the binary
// operator_delete target 0x00632B42, common.cpp per the Static
// scalar_deleting precedent). Returns this.
void* __fastcall TRIBE_Master_Tree_Object__scalar_deleting_destructor_new(
    TRIBE_Master_Tree_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for TRIBE_Master_Tree_Object::TRIBE_Master_Tree_Object2
// Original: @ 0x005C97B0 (idb_functions.h:5141). Range [0x5C97B0,
// 0x5C981E) (lst :766728/:766785, retn 10h = 4 stack args), returns this.
// ABI: __thiscall - this_ in ECX. Caller: TRIBE_Master_Player__load_
// master_object+139 (lst :766729). SEH fold per the Animated ctor
// precedent (unwind chunk 0x646780: mov ecx, [ebp-10h] / jmp
// RGE_Master_Static_Object__destructor - the IMMEDIATE base level;
// handler = __CxxFrameHandler dispatch, unwind-only). Base Static ctor2
// with do_setup = 0, the Tree vfptr store, then the do_setup body =
// Static setup2 (Tree has NO setup overrides - the asm calls the base
// setups directly) + master_type = 15 (asm 0x5C9806 byte store @+4).
TRIBE_Master_Tree_Object* __fastcall TRIBE_Master_Tree_Object__TRIBE_Master_Tree_Object2_new(
    TRIBE_Master_Tree_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    TRIBE_Master_Tree_Object* result = this_;
    __try
    {
        RGE_Master_Static_Object__RGE_Master_Static_Object2((RGE_Master_Static_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = tribe_master_tree_object_vftable;
        if (do_setup)
        {
            RGE_Master_Static_Object__setup2((RGE_Master_Static_Object*)this_, infile, sprites, sounds);
            this_->master_type = RGE_MASTER_TYPE_TREE;
        }
        result = this_;
    }
    __except (RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Tree_Object::TRIBE_Master_Tree_Object3
// Original: @ 0x005C9830 (idb_functions.h:5142). Range [0x5C9830,
// 0x5C98A8) (lst :766794/:766856, retn 14h = 5 stack args), returns this.
// ABI: __thiscall. Caller: TRIBE_Master_Player__load_object+148
// (lst :766795). Same SEH fold (chunk 0x6467A0, same Static-destructor
// action). Base Static ctor3 with do_setup = 0; the do_setup body =
// Static setup3 + master_type = 15 (asm 0x5C9890).
TRIBE_Master_Tree_Object* __fastcall TRIBE_Master_Tree_Object__TRIBE_Master_Tree_Object3_new(
    TRIBE_Master_Tree_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    TRIBE_Master_Tree_Object* result = this_;
    __try
    {
        RGE_Master_Static_Object__RGE_Master_Static_Object3((RGE_Master_Static_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = tribe_master_tree_object_vftable;
        if (do_setup)
        {
            RGE_Master_Static_Object__setup3((RGE_Master_Static_Object*)this_, infile, sprites, sounds, in_id);
            this_->master_type = RGE_MASTER_TYPE_TREE;
        }
        result = this_;
    }
    __except (RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Tree_Object::TRIBE_Master_Tree_Object4
// Original: @ 0x005C98B0 (idb_functions.h:5143). Range [0x5C98B0,
// 0x5C9928) (lst :766865/:766927, retn 14h = 5 stack args), returns this.
// ABI: __thiscall. Caller: TRIBE_Player__load_master_object+185
// (lst :766866). Same SEH fold (chunk 0x6467C0). Base Static ctor4 with
// do_setup = 0; the do_setup body = Static setup4 + master_type = 15
// (asm 0x5C9910).
TRIBE_Master_Tree_Object* __fastcall TRIBE_Master_Tree_Object__TRIBE_Master_Tree_Object4_new(
    TRIBE_Master_Tree_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites,
    RGE_Sound** sounds, int do_setup)
{
    TRIBE_Master_Tree_Object* result = this_;
    __try
    {
        RGE_Master_Static_Object__RGE_Master_Static_Object4((RGE_Master_Static_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = tribe_master_tree_object_vftable;
        if (do_setup)
        {
            RGE_Master_Static_Object__setup4((RGE_Master_Static_Object*)this_, infile, list, sprites, sounds);
            this_->master_type = RGE_MASTER_TYPE_TREE;
        }
        result = this_;
    }
    __except (RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Tree_Object::make_new_obj
// Original: @ 0x005C9930 (idb_functions.h:5144). Range [0x5C9930,
// 0x5C99A7) (lst :766936/:766992, retn 10h = 4 stack args), RGE_Static_
// Object* result. ABI: __thiscall. Virtual: vftable DATA XREF
// (lst :766937). NO RECYCLE PATH (Tree runtime objects never recycle).
// operator_new(0xB4) - sizeof(TRIBE_Tree_Object) = 180 = 0xB4 verified:
// the runtime struct (idb_types.h:21889-21940) is the RGE_Static_Object
// layout (:1438-1488, mechanically verified 180). The runtime
// TRIBE_Tree_Object ctor1 (idb_functions.h:4084, 0x00564DB0, unhooked -
// the asm's direct call) binds tobj = this Tree master and do_setup = 1.
// SEH fold per the make_new_obj precedent (chunk 0x6467E0: the
// operator_delete filter on the allocation).
RGE_Static_Object* __fastcall TRIBE_Master_Tree_Object__make_new_obj_new(
    TRIBE_Master_Tree_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    TRIBE_Tree_Object* new_obj = 0;
    __try
    {
        new_obj = (TRIBE_Tree_Object*)operator_new_internal(sizeof(TRIBE_Tree_Object));
        if (new_obj)
            result = (RGE_Static_Object*)TRIBE_Tree_Object__TRIBE_Tree_Object(new_obj, this_, owner, x, y, z, 1);
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Tree_Object::make_new_master
// Original: @ 0x005C99B0 (idb_functions.h:5145). Range [0x5C99B0,
// 0x5C9A19) (lst :767001/:767049, no stack args), RGE_Master_Static_
// Object* result. ABI: __thiscall. Virtual: vftable DATA XREF
// (lst :767002). Master-template factory: operator_new(0xD0) -
// sizeof(TRIBE_Master_Tree_Object) = 208 = 0xD0 verified: the master
// struct (idb_types.h:22170-22250) is the Static master mirror ending
// at outline_radius_z, no extension (the identical walk verified at 208
// in the Static make_new_master round). Then the base Static COPY-CTOR
// with do_setup = 0, the Tree vfptr store, the base Static setup
// (hooked -> routes; it copies the template from this and sets
// master_type = 10), and the Tree master_type = 15 store AFTER the
// setup (asm order 0x5C99FE/0x5C9A03 - later store wins; NOTE this
// differs from the Combat(70)/Building factories, whose derived setups
// store master_type themselves - Tree has no setup of its own).
// SEH fold per the make_new_master precedent (chunk 0x646800: the
// operator_delete filter).
RGE_Master_Static_Object* __fastcall TRIBE_Master_Tree_Object__make_new_master_new(
    TRIBE_Master_Tree_Object* this_)
{
    TRIBE_Master_Tree_Object* new_master =
        (TRIBE_Master_Tree_Object*)operator_new_internal(sizeof(TRIBE_Master_Tree_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Static_Object__RGE_Master_Static_Object(
                (RGE_Master_Static_Object*)new_master, (RGE_Master_Static_Object*)this_, 0);
            new_master->vfptr = tribe_master_tree_object_vftable;
            RGE_Master_Static_Object__setup((RGE_Master_Static_Object*)new_master, (RGE_Master_Static_Object*)this_);
            new_master->master_type = RGE_MASTER_TYPE_TREE;
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

void install_TRIBE_Master_Tree_Object()
{
    setHook((void*)0x005C9780, TRIBE_Master_Tree_Object__scalar_deleting_destructor_new);
    setHook((void*)0x005C97B0, TRIBE_Master_Tree_Object__TRIBE_Master_Tree_Object2_new);
    setHook((void*)0x005C9830, TRIBE_Master_Tree_Object__TRIBE_Master_Tree_Object3_new);
    setHook((void*)0x005C98B0, TRIBE_Master_Tree_Object__TRIBE_Master_Tree_Object4_new);
    setHook((void*)0x005C9930, TRIBE_Master_Tree_Object__make_new_obj_new);
    setHook((void*)0x005C99B0, TRIBE_Master_Tree_Object__make_new_master_new);
}
