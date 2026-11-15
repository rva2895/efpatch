#include "stdafx.h"

// .rdata vftable of RGE_Animated_Object (lst :987149)
static RGE_Animated_ObjectVtbl* const rge_animated_object_vftable =
    (RGE_Animated_ObjectVtbl*)0x006566A0;

// Replacement for RGE_Animated_Object::RGE_Animated_Object (runtime ctor)
// Original: @ 0x00422980 (idb_functions.h:538). Range [0x422980,
// 0x422A3D) (lst :62193/:62283, retn 18h = 6 stack args), returns this.
// ABI: __thiscall - this_ in ECX. Callers: RGE_Master_Animated_Object__
// make_new_obj (owned, hook-routed), RGE_Doppleganger_Object__RGE_
// Doppleganger_Object+3E (lst :62194), RGE_Object_List__load paths.
// Decode (asm 0x422980-0x422A3C): base Static runtime ctor with
// do_setup = 0 (hooked -> routes), the Animated vfptr store, then the
// do_setup body: the RUNTIME Static setup (tobj, obj_owner, x, y, z);
// type @+0x5E = 0x14 (the runtime object's tier mirror - idb field
// `type`, asm 0x4229F1); speed @+0xB4 = 0 (the Animated extension,
// asm 0x4229F5); set_sleep_flag(0); and the sprite-list bootstrap:
// if sprite_list @+0x24 == 0 -> sprite_list = vfptr->create_sprite_list
// (RGE_Static_ObjectVtbl slot +0x2A8 = index 170, idb_types.h:4105),
// sprite = 0, then master->vfptr->get_sprite(master, this) (the MASTER
// vtbl slot +4) and vfptr->new_sprite(this, sprite) (slot +0xB0).
// SEH fold: unwind chunk 0x640F70 = mov ecx, [ebp-10h] / jmp
// RGE_Static_Object__destructor (0x54B2E0 - HOOKED -> routes into
// RGE_Static_Object__destructor_new per the hook-routing ruling).
RGE_Animated_Object* __fastcall RGE_Animated_Object__RGE_Animated_Object_new(
    RGE_Animated_Object* this_, DWORD dummy,
    RGE_Master_Animated_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Animated_Object* result = this_;
    __try
    {
        RGE_Static_Object__RGE_Static_Object(
            (RGE_Static_Object*)this_, (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_animated_object_vftable;
        if (do_setup != 0)
        {
            RGE_Static_Object__setup((RGE_Static_Object*)this_, (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z);
            this_->type = 0x14;                                   // asm 0x4229F1 — runtime tier mirror
            this_->speed = 0.0f;                                  // asm 0x4229F5 — the Animated extension @0xB4
            RGE_Static_Object__set_sleep_flag((RGE_Static_Object*)this_, 0);          // idb_functions.h:3891
            if (this_->sprite_list == 0)                          // @+0x24 (asm 0x422A00)
            {
                this_->sprite_list = this_->vfptr->create_sprite_list(this_);   // slot +0x2A8
                this_->sprite = 0;                                // asm 0x422A15
                RGE_Master_Animated_Object* master = this_->master_obj;
                this_->sprite = master->vfptr->get_sprite(master, (RGE_Static_Object*)this_);   // master vtbl slot +4
                this_->vfptr->new_sprite(this_, this_->sprite);   // slot +0xB0
            }
        }
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Animated_Object::vector_deleting_destructor
// Original: @ 0x0046FE70 (idb_functions.h:1481). Range [0x46FE70,
// 0x46FE8B) (lst :189152/:189171, retn 4), void* result. ABI:
// __thiscall. Virtual: vftable DATA XREF (lst :189153) - also the
// Doppleganger runtime vftable's slot (lst :189154). The runtime
// Animated class has NO destructor body of its own: the vdel calls
// j_RGE_Static_Object__destructor (the thunk -> 0x54B2E0 -> HOOKED ->
// routes into RGE_Static_Object__destructor_new per the hook-routing
// ruling), then flags & 1 -> operator_delete, return this.
void* __fastcall RGE_Animated_Object__vector_deleting_destructor_new(
    RGE_Animated_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Static_Object__destructor((RGE_Static_Object*)this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

void install_RGE_Animated_Object()
{
    setHook((void*)0x00422980, RGE_Animated_Object__RGE_Animated_Object_new);
    setHook((void*)0x0046FE70, RGE_Animated_Object__vector_deleting_destructor_new);
}
