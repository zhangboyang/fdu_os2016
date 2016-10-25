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
    kprintf(MSG_SIGN "malloc(%u) = ", (unsigned) size);
    DECLARE_THIS(bootstrap_vmalloc);
    M(cur) = PTRCAST(ROUNDUP(ULCAST(M(cur)), PAGE_SIZE)); // align to PAGE_SIZE
    void *ret = M(cur);
    if (M(cur) + size > M(limit)) {
        kprintf("NULL\n");
        return NULL;
    }
    M(cur) += size;
    memset(ret, 0xCD, size); // fill memory with junk, to avoid programming mistakes
    kprintf("%p\n", ret);
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

void bootstrap_vmalloc__ctor(struct bootstrap_vmalloc *this, void *base, size_t max_size)
{
    INST_VTBL_SINGLETON(this, {
        .malloc = bootstrap_vmalloc__malloc,
        .free = bootstrap_vmalloc__free,
        .area = bootstrap_vmalloc__area,
    });
    
    M(base) = M(cur) = base;
    M(limit) = base + 4096;
}




void vmalloc_bootstrap()
{
    // this function will bootstrip the virtual memory allocation system
    
    // alloc a bootstrip allocator on stack
    // this allocator will alloc memory at the end of kernel
    // no free(), and allocated memory will never be freed
    struct bootstrap_vmalloc vmalloc_tmp;
    bootstrap_vmalloc__ctor(&vmalloc_tmp, KERN_END_HIGH, size_after_kernel);
    
    // do some tests
    VF(&vmalloc_tmp, malloc, 12);
    VF(&vmalloc_tmp, malloc, 120);
    VF(&vmalloc_tmp, malloc, 4096 * 0x10);
    VF(&vmalloc_tmp, malloc, 12);
    
    // let the physical memory allocator bootstrip
    pmalloc_bootstrip(&vmalloc_tmp);
    
}
