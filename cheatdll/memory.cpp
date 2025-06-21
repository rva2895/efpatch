#include "stdafx.h"
#include "memory.h"

#ifdef EFPATCH_MEMORY_DEBUG
#include <intrin.h>
#include <algorithm>
#endif

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

#ifdef EFPATCH_MEMORY_DEBUG

#define PAGE_SIZE 0x1000

void* __cdecl calloc_protect(size_t size)
{
    DWORD old;
    size_t page_cnt = size / PAGE_SIZE + 1;
    void* p = VirtualAlloc(NULL, (page_cnt + 1) * PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (!p)
        __debugbreak();

    VirtualProtect((char*)p + page_cnt * PAGE_SIZE, PAGE_SIZE, PAGE_NOACCESS, &old);

    *(void**)((char*)p + PAGE_SIZE - (size % PAGE_SIZE) - 4) = p;

    return (char*)p + PAGE_SIZE - (size % PAGE_SIZE);
}


struct allocation
{
    void* at;
    void* p;
    size_t size;

    allocation(void* at_, void* p_, size_t size_)
    {
        at = at_;
        p = p_;
        size = size_;
    }
};

std::map<void*, allocation> allocations;
std::map<void*, size_t> alloc_size;

#endif

void* __cdecl malloc_new(size_t size)
{
    void* p = malloc(size);
#ifdef EFPATCH_MEMORY_DEBUG
    allocations.emplace(std::pair(p, allocation(_ReturnAddress(), p, size)));
#endif
    return p;
}

void* __cdecl calloc_new(size_t number, size_t size)
{
    void* p = calloc(number, size);
#ifdef EFPATCH_MEMORY_DEBUG
    allocations.emplace(std::pair(p, allocation(_ReturnAddress(), p, number * size)));
#endif
    return p;
}

void __cdecl free_new(void* lpMem)
{
#ifdef EFPATCH_MEMORY_DEBUG
    if (lpMem)
    {
        auto it = allocations.find(lpMem);
        if (it != allocations.end())
            allocations.erase(it);
        else
            __debugbreak();
    }
#endif
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

    writeNops(0x0059416C, 9);   //enemyUnits destructor

    log("New allocator uninstalled");

#ifdef EFPATCH_MEMORY_DEBUG
    size_t total = 0;

    for (auto it = allocations.begin(); it != allocations.end(); ++it)
    {
        log("Memory leak: at 0x%08X, size %u", it->second.at, it->second.size);
        total += it->second.size;
        auto it2 = alloc_size.find(it->second.at);
        if (it2 == alloc_size.end())
            alloc_size.insert(std::pair(it->second.at, it->second.size));
        else
            it2->second += it->second.size;
    }

    log("Memory leak total: %u%s", total > 0x4000 ? total / 1024 : total, total > 0x4000 ? " K" : "");

    for (auto it = alloc_size.begin(); it != alloc_size.end(); ++it)
        log("Address 0x%08X, total %u%s", it->first, it->second > 0x4000 ? it->second / 1024 : it->second, it->second > 0x4000 ? " K" : "");
#endif
}
#pragma optimize( "", on )
