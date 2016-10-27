/*
    ZBY:
       the 'slub' allocator
       alloc page for any size object
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/vmm.h>
#include <aim/debug.h>
#include <aim/pmm.h>
#include <util.h>

DECLARE_BASE_VCLASS(fixed_alloc, struct {
    
}, struct {
});

static void *slub__malloc(THIS, size_t size)
{
    DECLARE_THIS(use_page_vmalloc);
    addr_t pa = VF(M(palloc), malloc, size);
    if (pa == -1) return NULL;
    return PTRCAST(pa + KOFFSET);
}

static void slub__free(THIS, void *ptr)
{
    DECLARE_THIS(use_page_vmalloc);
    addr_t pa = (ULCAST(ptr) - KOFFSET);
    VF(M(palloc), free, pa);
}

static size_t slub__size(THIS, void *obj)
{
    panic("use_page_vmalloc::size() is called");
}

void slub__ctor(struct use_page_vmalloc *this, struct virt_pmalloc *palloc)
{
    INST_VTBL_SINGLETON(this, {
        .malloc = slub__malloc,
        .free = slub__free,
        .size = slub__size,
    });
    M(palloc) = palloc;
}
