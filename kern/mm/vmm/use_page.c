/*
    ZBY:
       simplely use the page allocator
       alloc page for any size object
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


static void *use_page_vmalloc__malloc(THIS, size_t size)
{
    DECLARE_THIS(use_page_vmalloc);
    addr_t pa = VF(M(palloc), malloc, size);
    if (pa == -1) return NULL;
    return PTRCAST(pa + KOFFSET);
}

static void *use_page_vmalloc__free(THIS, void *ptr)
{
    addr_t pa = (ULCAST(ptr) - KOFFSET);
    VF(M(palloc), free, pa);
}

static size_t use_page_vmalloc__size(THIS, void *obj)
{
    panic("use_page_vmalloc::size() is called");
}

void use_page_vmalloc__ctor(struct use_page_vmalloc *this, struct virt_pmalloc *palloc)
{
    INST_VTBL_SINGLETON(this, {
        .malloc = use_page_vmalloc__malloc,
        .free = use_page_vmalloc__free,
        .size = use_page_vmalloc__size,
    });
    M(palloc) = palloc;
}

