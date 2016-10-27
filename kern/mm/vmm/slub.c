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


static struct slub_header *alloc_slub(THIS, int level)
{
    DECLARE_THIS(slub_vmalloc);
    addr_t pa = VF(M(palloc), aligned_malloc, SLUB_BLOCK_SIZE, SLUB_BLOCK_SIZE);
    
    if (slub) return NULL;
    return NULL;
}

static void *slub_vmalloc__malloc(THIS, size_t size)
{
//    DECLARE_THIS(slub_vmalloc);
    return NULL;
}

static void slub_vmalloc__free(THIS, void *ptr)
{

}

static size_t slub_vmalloc__size(THIS, void *obj)
{
    panic("size() is not supported");
}


void slub_vmalloc__ctor(struct slub_vmalloc *this, struct virt_pmalloc *palloc)
{   
    INST_VTBL_SINGLETON(this, {
        .malloc = slub_vmalloc__malloc,
        .free = slub_vmalloc__free,
        .size = slub_vmalloc__size,
    });
    M(palloc) = palloc;
    for (size_t i = MIN_SLUB_LEVEL; i <= MAX_SLUB_LEVEL; i++) {
        INIT_LIST_HEAD(M(slub[SLUB_LEVEL_INDEX(i)]));
    }
}




