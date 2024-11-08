#include "stdafx.h"
#include "memory"

uint64_t save_8_bytes(void* addr)
{
    uint64_t d;
    ReadProcessMemory(GetCurrentProcess(), addr, &d, sizeof(d), NULL);
    return d;
}

void set_hook_and_save_original(void* addr, void* new_addr, uint64_t& old_data)
{
    old_data = save_8_bytes(addr);
    setHook(addr, new_addr);
}

void restore_original(void* addr, uint64_t& old_data)
{
    writeData((DWORD)addr, &old_data, sizeof(old_data));
}

uint64_t old_data_malloc;
uint64_t old_data_operator_new;
uint64_t old_data_calloc;
uint64_t old_data_free;
uint64_t old_data_delete;

void* __cdecl malloc_new(size_t size)
{
    return malloc(size);
}

void* __cdecl calloc_new(size_t number, size_t size)
{
    void* d = malloc_new(number * size);
    memset(d, 0, number * size);
    return d;
}

void __cdecl free_new(void* lpMem)
{
    free(lpMem);
}

#pragma optimize( "s", on )
void new_allocator_install()
{
    set_hook_and_save_original((void*)0x0063328C, malloc_new, old_data_malloc);
    set_hook_and_save_original((void*)0x00632B9D, malloc_new, old_data_operator_new);
    set_hook_and_save_original((void*)0x00632D33, calloc_new, old_data_calloc);
    set_hook_and_save_original((void*)0x00632CCA, free_new, old_data_free);
    set_hook_and_save_original((void*)0x00632B42, free_new, old_data_delete);

    log("New allocator installed");
}

void new_allocator_uninstall()
{
    restore_original((void*)0x0063328C, old_data_malloc);
    restore_original((void*)0x00632B9D, old_data_operator_new);
    restore_original((void*)0x00632D33, old_data_calloc);
    restore_original((void*)0x00632CCA, old_data_free);
    restore_original((void*)0x00632B42, old_data_delete);

    log("New allocator uninstalled");
}
#pragma optimize( "", on )
