/*
    ZBY:
    this is the simple allocator used in stage1
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/vmm.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/pmm.h>
#include <util.h>


#define MSG_SIGN " bsvma: " // BootStrap Virtual Memory Allocator
static void *bootstrap_vmalloc__malloc(THIS, size_t size)
{
    //kprintf(MSG_SIGN "malloc(%u) = ", (unsigned) size);
    DECLARE_THIS(bootstrap_vmalloc);
    M(cur) = PTRCAST(ROUNDUP(ULCAST(M(cur)), PAGE_SIZE)); // align to PAGE_SIZE
    void *ret = M(cur);
    if (M(cur) + size > M(limit)) {
        //kprintf("NULL\n");
        return NULL;
    }
    M(cur) += size;
    //kprintf("[ptr=%p]", ret);
    memset(ret, 0xCD, size); // fill memory with junk, to avoid programming mistakes
    //kprintf("%p\n", ret);
    return ret;
}
static void bootstrap_vmalloc__free(THIS, void *ptr)
{
    // let the memory leak!
}
static size_t bootstrap_vmalloc__area(THIS)
{
    DECLARE_THIS(bootstrap_vmalloc);
    return M(cur) - M(base);
}

static size_t bootstrap_vmalloc__size(THIS, void *obj)
{
    panic("bootstrip_vmalloc::size() is called");
}

void bootstrap_vmalloc__ctor(struct bootstrap_vmalloc *this, void *base, size_t max_size)
{
    INST_VTBL_SINGLETON(this, {
        .malloc = bootstrap_vmalloc__malloc,
        .free = bootstrap_vmalloc__free,
        .size = bootstrap_vmalloc__size,
        .area = bootstrap_vmalloc__area,
    });
    
    M(base) = M(cur) = base;
    M(limit) = base + max_size;
}


void pmm_selftest()
{
#define PMM_SELFTEST__MAX_PAGES_PER_ALLOC 10
#define PMM_SELFTEST__P_FREE 20
    kprintf("before pmm self-test:\n");
    VF(pmm_zone[ZONE_NORMAL].allocator, print);
    kprintf("running pmm self-test ...\n");
    addr_t page;
    int magic = 0x38276abd;
    unsigned r = 123456;
    unsigned i, sz;
    unsigned long long tot = 0;
    while (1) {
        r = (1103515245 * r + 12345) & 0x7fffffff; // next rand
        for (sz = r % PMM_SELFTEST__MAX_PAGES_PER_ALLOC + 1; sz > 0; sz--) {
            page = VF(pmm_zone[ZONE_NORMAL].allocator, malloc, 0x1000LL * sz);
            if (page != -1) break;
        }
        if (sz == 0) break;
        
        r = (1103515245 * r + 12345) & 0x7fffffff; // next rand
        //kprintf("got page sz=%d paddr=%016llx vaddr=%08x\n", sz, page, (unsigned) (page + KOFFSET)  );
        
        for (i = 0; i < sz; i++) {
            int *x = (void *) (long)(page + i * 0x1000LL + KOFFSET);
            if (*x == magic) {
                panic("ERROR! double alloc is found!");
                while (1);
            }
            *x = magic;
        }
        memset((void *) (long)(page + KOFFSET), 'A', 0x1000LL * sz);
        if (r % PMM_SELFTEST__P_FREE > 0) {
            VF(pmm_zone[ZONE_NORMAL].allocator, free, page);
        } else {
            tot += 0x1000LL * sz;
        }
    }
    kprintf("PLEASE VERIFY:\n");
    VF(pmm_zone[ZONE_NORMAL].allocator, print);
    kprintf("total memory is allocated %lld KB (due to internal fragment, holes in memory, the value may smaller than real value)\n", (tot >> 10));
    panic("pmm test OK!");
}

void vmm_selftest()
{
    unsigned r = 162;
    unsigned magic = 0x1223acdb;
    lsize_t tot = 0;
    kprintf("vmm selftest ...\n");
    while (1) {
        r = (1103515245 * r + 12345) & 0x7fffffff; // next rand
        size_t sz = r % 100 * 0x10 + 4;
        unsigned *p = VF(g_vmalloc, malloc, sz);
        if (!p) p = VF(g_vmalloc, malloc, (sz = 4));
        if (!p) break;
        //kprintf("p=%p sz=%d\n", p, sz);
        //kprintf("*%p=%08x\n", p, *p);
        if (*p == magic) {
            panic("double alloc!");
        }
        memset(p, 0x18, sz);
        r = (1103515245 * r + 12345) & 0x7fffffff; // next rand
        if (r % 10 < 8) {
            VF(g_vmalloc, free, p);
            //kprintf("free(%p)\n", p);
        } else {
            kprintf("*%p=%08x\n", p, *p);
            *p = magic;
            tot += sz;
            //kprintf("*%p=%08x\n", p, *p);
        }
    }
    kprintf("total: %lld KB\n", (tot >> 10));
    panic("vmm test OK!");
}

void vmm_bootstrap()
{
    // this function will bootstrip the virtual memory allocation system
    
    // alloc a bootstrip allocator on stack
    // this allocator will alloc memory at the end of kernel
    // no free(), and allocated memory will never be freed
    struct bootstrap_vmalloc vmalloc_tmp;
    bootstrap_vmalloc__ctor(&vmalloc_tmp, KERN_END_HIGH, size_after_kernel);
    
    // init pmm zones
    arch_init_pmm_zone();

    // new buddy allocator for each zone
    for (int i = 0; i < MAX_MEMORY_ZONE; i++) {
        struct zone *z = &pmm_zone[i];

        struct buddy_pmalloc *bpa = VF(&vmalloc_tmp, malloc, sizeof(struct buddy_pmalloc));
        if (!bpa) panic("can't alloc memory for zone %d", i);
        
        buddy_pmalloc__ctor(bpa, BC(&vmalloc_tmp), z->base, z->page_size, z->size / z->page_size);
        z->allocator = BC(bpa);
    }
    
    // free allocable memory regions, the life of @vmalloc_tmp has ended!
    addr_t kstart = ROUNDDOWN(ULCAST(KERN_START_LOW), PAGE_SIZE);
    addr_t kend = ROUNDUP(ULCAST(KERN_END_LOW), PAGE_SIZE);
    addr_t real_kend = kend + VF(&vmalloc_tmp, area);
    real_kend = ROUNDUP(real_kend, PAGE_SIZE);
    arch_init_free_pmm_zone(kstart, real_kend);

    
    //pmm_selftest();
    
    
    // set global pvbridge
    static struct koffset_pvbridge pvbridge;
    koffset_pvbridge__ctor(&pvbridge, pmm_zone[ZONE_NORMAL].allocator);
    g_pvbridge = BC(&pvbridge);
    
    
    // set global vmm allocator
    static struct slub_vmalloc vmalloc;
    slub_vmalloc__ctor(&vmalloc, g_pvbridge);
    g_vmalloc = BC(&vmalloc);
    
    
    // install adapter for AIM interface
    install_pmm_adapter();
    install_vmm_adapter();
    
    vmm_selftest();
    
    // print ZONE_NORMAL summary
    VF(pmm_zone[ZONE_NORMAL].allocator, print);
}
