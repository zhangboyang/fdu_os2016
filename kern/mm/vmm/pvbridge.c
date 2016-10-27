/*
    ZBY:
       the pmm and vmm bridge
       convert PA to VA
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/vmm.h>
#include <aim/debug.h>
#include <aim/pmm.h>
#include <util.h>

static void *koffset_pvbridge__malloc(THIS, lsize_t size)
{
    DECLARE_THIS(koffset_pvbridge);
    addr_t pa = VF(M(palloc), malloc, size);
    if (pa == -1) return NULL;
    return PTRCAST(pa + KOFFSET);
}

static void *koffset_pvbridge__aligned_malloc(THIS, lsize_t size, lsize_t align)
{
    DECLARE_THIS(koffset_pvbridge);
    addr_t pa = VF(M(palloc), aligned_malloc, size, align);
    if (pa == -1) return NULL;
    return PTRCAST(pa + KOFFSET);
}

static void koffset_pvbridge__free(THIS, void *ptr)
{
    DECLARE_THIS(koffset_pvbridge);
    if (ptr == NULL) return;
    addr_t pa = ULCAST(ptr) - KOFFSET;
    VF(M(palloc, free, pa));
}

void koffset_pvbridge__ctor(struct koffset_pvbridge *this, struct virt_pmalloc *palloc)
{
    INST_VTBL_SINGLETON(this, {
        .malloc = koffset_pvbridge__malloc,
        .aligned_malloc = koffset_pvbridge__aligned_malloc,
        .free = koffset_pvbridge__free,
    });
    M(palloc) = palloc;
}

