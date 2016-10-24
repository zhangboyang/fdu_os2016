#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/boot.h>
#include <arch-boot.h>

/*
 * this file is added by ZBY
 */

#define MAX_MEMORY_REGIONS 20

struct memmap {
    union {
        uint64_t base;
        struct { uint32_t base_low, base_high; };
    };
    union {
        uint64_t size;
        struct { uint32_t size_low, size_high; };
    };
    uint32_t type;
    uint32_t unknown;
}; // 24 bytes

static void detectmemory()
{
    struct memmap map[MAX_MEMORY_REGIONS];
    
    bmemset((void *) 0x1000, 0xCD, MAX_MEMORY_REGIONS * sizeof(struct memmap));
    int cnt = detectmemory_realmode(0x1000);
    
    if (!cnt) {
        bpanic("can't detect memory in real mode.");
    }
    bmemcpy(map, (void *) 0x1000, MAX_MEMORY_REGIONS * sizeof(struct memmap));
    
    int i;
    for (i = 0; i < cnt; i++) {
        bprintf("base %x %x size %x %x end %x %x type %d\n", map[i].base_high, map[i].base_low, map[i].size_high, map[i].size_low, (uint32_t) ((map[i].base + map[i].size) >> 32), (uint32_t) (map[i].base + map[i].size), map[i].type);
    }
}

