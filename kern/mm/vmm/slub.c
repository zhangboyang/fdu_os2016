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

#define CALC_MAXSOLTS(level) ((SLUB_BLOCK_SIZE - sizeof(struct slub_header)) >> (level))
#define SOLTS_OFFSET(level) (ALIGN_ABOVE(sizeof(struct slub_header), (1 << (level))))

static int slub_is_full(struct slub_header *slub)
{
    return list_empty(&slub->free_list);
}

static struct slub_header *alloc_slub(THIS, int level)
{
    DECLARE_THIS(slub_vmalloc);
    struct slub_header *slub = VF(M(pvbridge), aligned_malloc, SLUB_BLOCK_SIZE, SLUB_BLOCK_SIZE);
    if (!slub) return NULL;
    
    size_t max_solts = CALC_MAXSOLTS(level);
    slub->level = level;
    INIT_LIST_HEAD(&slub->free_list);

    struct list_head *solts = ((void *) slub) + SOLTS_OFFSET(level);
    for (size_t i = 0; i < max_solts; i++) {
        list_add(&solts[i], &slub->free_list);
    }
    
    return slub;
}

static void *malloc_from_slub(struct slub_header *slub)
{
    assert(!list_empty(&slub->free_list));
    struct list_head *solt = slub->free_list.next;
    list_del(solt);
    return solt;
}

static void *slub_vmalloc__malloc(THIS, size_t size)
{
    DECLARE_THIS(slub_vmalloc);
    size_t level = 0;
    while ((1ULL << level) < size) level++;
    
    if (list_empty(&M(slub_avail[level]))) {
        struct slub_header *new_slub = alloc_slub(this, level);
        if (!new_slub) return NULL;
        list_add(&new_slub->node, &M(slub_avail[level]));
    }
    
    struct slub_header *slub = list_first_entry(&M(slub_avail[level]), struct slub_header, node);
    void *ret = malloc_from_slub(slub);
    if (slub_is_full(slub)) {
        list_del(&slub->node);
        list_add(&slub->node, &M(slub_full[level]));
    }
    return ret;
}

static void slub_vmalloc__free(THIS, void *ptr)
{
    DECLARE_THIS(slub_vmalloc);
    struct list_head *solt = ptr;
    struct slub_header *slub = PTRCAST(ALIGN_BELOW(ULCAST(ptr), SLUB_BLOCK_SIZE));
    int move_flag = slub_is_full(slub);
    list_add(solt, &slub->free_list);
    if (move_flag) {
        list_del(&slub->node);
        list_add(&slub->node, &M(slub_avail[slub->level]));
    }
}

static size_t slub_vmalloc__size(THIS, void *obj)
{
    panic("size() is not supported");
}


void slub_vmalloc__ctor(struct slub_vmalloc *this, struct virt_pvbridge *pvbridge)
{   
    INST_VTBL_SINGLETON(this, {
        .malloc = slub_vmalloc__malloc,
        .free = slub_vmalloc__free,
        .size = slub_vmalloc__size,
    });
    M(pvbridge) = pvbridge;
    for (size_t i = MIN_SLUB_LEVEL; i <= MAX_SLUB_LEVEL; i++) {
        INIT_LIST_HEAD(M(slub_avail[SLUB_LEVEL_INDEX(i)]));
        INIT_LIST_HEAD(M(slub_full[SLUB_LEVEL_INDEX(i)]));
    }
}




