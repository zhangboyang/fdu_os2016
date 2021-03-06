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
#include <aim/pmm.h>
#include <util.h>


static void *use_page_vmalloc__malloc(THIS, size_t size)
{
    DECLARE_THIS(use_page_vmalloc);
    return VF(M(pvbridge), malloc, size);
}

static void use_page_vmalloc__free(THIS, void *ptr)
{
    DECLARE_THIS(use_page_vmalloc);
    VF(M(pvbridge), free, ptr);
}

static size_t use_page_vmalloc__size(THIS, void *obj)
{
    panic("use_page_vmalloc::size() is called");
}

void use_page_vmalloc__ctor(struct use_page_vmalloc *this, struct virt_pvbridge *pvbridge)
{
    INST_VTBL_SINGLETON(this, {
        .malloc = use_page_vmalloc__malloc,
        .free = use_page_vmalloc__free,
        .size = use_page_vmalloc__size,
    });
    M(pvbridge) = pvbridge;
}

