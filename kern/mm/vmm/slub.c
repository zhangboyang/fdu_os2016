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
    assert(level >= MIN_SLUB_LEVEL);
    assert(level <= MAX_SLUB_LEVEL);
    DECLARE_THIS(slub_vmalloc);
    struct slub_header *slub = VF(M(pvbridge), aligned_malloc, SLUB_BLOCK_SIZE, SLUB_BLOCK_SIZE);
    if (!slub) return NULL;
    
    size_t max_solts = CALC_MAXSOLTS(level);
    slub->level_idx = SLUB_LEVEL_INDEX(level);
    INIT_LIST_HEAD(&slub->free_list);

    void *solts_base = ((void *) slub) + SOLTS_OFFSET(level);
    size_t solt_size = (1 << level);
    for (size_t i = 0; i < max_solts; i++) {
        struct list_head *solt = solts_base + solt_size * i;
        list_add(solt, &slub->free_list);
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
    int level = MIN_SLUB_LEVEL;
    while ((1ULL << level) < size) level++;
    if (level > MAX_SLUB_LEVEL) {
        // size is too big, pass it to pvbridge directly
        // we need to distinguish two type of pointers
        // so we add alignment requirements
        return VF(M(pvbridge), aligned_malloc, size, SLUB_BLOCK_SIZE);
    }
    
    int level_idx = SLUB_LEVEL_INDEX(level);
    if (list_empty(&M(slub_avail[level_idx]))) {
        struct slub_header *new_slub = alloc_slub(this, level);
        if (!new_slub) return NULL;
        list_add(&new_slub->node, &M(slub_avail[level_idx]));
    }
    
    struct slub_header *slub = list_first_entry(&M(slub_avail[level_idx]), struct slub_header, node);
    void *ret = malloc_from_slub(slub);
    if (slub_is_full(slub)) {
        list_del(&slub->node);
        list_add(&slub->node, &M(slub_full[level_idx]));
    }
    return ret;
}

static void slub_vmalloc__free(THIS, void *ptr)
{
    DECLARE_THIS(slub_vmalloc);
    if (IS_ALIGNED(ULCAST(ptr), SLUB_BLOCK_SIZE)) {
        // our pointer must be block-size aligned
        // if not aligned, pass it to pvbridge
        VF(M(pvbridge), free, ptr);
        return;
    }
    struct list_head *solt = ptr;
    struct slub_header *slub = PTRCAST(ALIGN_BELOW(ULCAST(ptr), SLUB_BLOCK_SIZE));
    int move_flag = slub_is_full(slub);
    list_add(solt, &slub->free_list);
    if (move_flag) {
        list_del(&slub->node);
        list_add(&slub->node, &M(slub_avail[slub->level_idx]));
    }
}

static size_t slub_vmalloc__size(THIS, void *obj)
{
    panic("size() is not supported");
}

static void slub_vmalloc__print(THIS)
{
    DECLARE_THIS(slub_vmalloc);
    for (int level = MIN_SLUB_LEVEL; level <= MAX_SLUB_LEVEL; level++) {
        int level_idx = SLUB_LEVEL_INDEX(level);
        kprintf("level %d:\n", level);
        struct slub_header *phdr;
        kprintf("  avail: ");
        int avail_cnt = 0;
        for_each_entry(phdr, &M(slub_avail[level_idx]), node) {
            //kprintf("%p ", phdr);
            avail_cnt = 0;
        }
        kprintf("count=%d\n", avail_cnt);
        kprintf("  full: ");
        int full_cnt = 0;
        for_each_entry(phdr, &M(slub_full[level_idx]), node) {
            //kprintf("%p ", phdr);
            full_cnt++;
        }
        kprintf("count=%d\n", full_cnt);
    }
}

void slub_vmalloc__ctor(struct slub_vmalloc *this, struct virt_pvbridge *pvbridge)
{   
    INST_VTBL_SINGLETON(this, {
        .malloc = slub_vmalloc__malloc,
        .free = slub_vmalloc__free,
        .size = slub_vmalloc__size,
        .print = slub_vmalloc__print,
    });
    M(pvbridge) = pvbridge;
    for (int i = MIN_SLUB_LEVEL; i <= MAX_SLUB_LEVEL; i++) {
        INIT_LIST_HEAD(&M(slub_avail[SLUB_LEVEL_INDEX(i)]));
        INIT_LIST_HEAD(&M(slub_full[SLUB_LEVEL_INDEX(i)]));
    }
}




