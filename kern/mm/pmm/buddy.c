/*
    ZBY:
    this is the buddy physical page allocator
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/pmm.h>
#include <aim/vmm.h>


#define get_buddy(index, order) ((index) ^ (1 << (order)))
#define get_low_index(index, order) ((index) & ~(1 << (order)))
#define get_high_index(index, order) ((index) | (1 << (order)))


static void buddy_pmalloc__print(THIS)
{
    DECLARE_THIS(buddy_pmalloc);
    kprintf("buddy allocator status for %p:\n", this);
    for (short order = 0; order <= M(max_order); order++) {
        kprintf(" order %d:\n", order);
        struct buddy_page *pp;
        for_each_entry(pp, &M(list[order]), node) {
            kprintf("  index %08x addr %016llx\n", pp - M(pages), M(base) + M(page_size) * (pp - M(pages)));
        }
    }
}

static addr_t buddy_pmalloc__malloc(THIS, lsize_t size)
{
    DECLARE_THIS(buddy_pmalloc);
    
    // map size to page-count
    size_t cnt = DIV_ROUND_UP(size, M(page_size));
    
    // calc corrsponding order
    short target_order;
    for (target_order = 0; (1 << target_order) < cnt; target_order++);
    
    // find a block with avaliable nodes
    short order;
    for (order = target_order; order <= M(max_order); order++) {
        if (!list_empty(&M(list[order]))) {
            break;
        }
    }
    if (order > M(max_order)) {
        // there are no more memory
        return -1;
    }
    
    // find the block
    struct buddy_page *pp = list_first_entry(&M(list[order]), struct buddy_page, node);
    size_t index = pp - M(pages);
    pp->in_use = 1;
    pp->order = target_order;
    kprintf(" pp=%p cnt=%d order=%d\n", pp, cnt, order);

    // split blocks
    list_del(&pp->node);
    while (order > target_order) {
        order--;
        size_t high_index = get_high_index(index, order);
        kprintf(" idx=%x hidx=%x\n", index, high_index);
        if (high_index < M(page_count)) { // if buddy index is valid
            // insert the high node to the linked list
            struct buddy_page *buddy_pp = &M(pages)[high_index];
            buddy_pp->in_use = 0;
            buddy_pp->order = order;
            list_add(&buddy_pp->node, &M(list[order]));
        }
    }
    
    return M(base) + M(page_size) * index;
}


static void buddy_pmalloc__free(THIS, addr_t ptr)
{
    //if ((ULCAST(ptr) & (0x10000000 - 1)) == 0) kprintf("ptr=%llx\n", ptr);
    
    DECLARE_THIS(buddy_pmalloc);
    assert(ROUNDDOWN(ptr, M(page_size)) == ptr);
    
    // calc index in pages[]
    size_t index = (ptr - M(base)) / M(page_size);
    struct buddy_page *pp = &M(pages)[index]; // pp: pointer to page
    assert((index & ((1 << pp->order) - 1)) == 0);
    assert(index < M(page_count));
    
    // try to merge with buddy
    short order;
    for (order = pp->order; order < M(max_order); order++) {
        // check if we can merge
        size_t buddy_index = get_buddy(index, order);
        if (buddy_index >= M(page_count)) break; // if the buddy index is invalid (out of range), exit loop
        struct buddy_page *buddy_pp = &M(pages)[buddy_index];
        //kprintf("free: in_use=%d order=%d pporder=%d\n", buddy_pp->in_use, buddy_pp->order, order);
        if (buddy_pp->in_use) break; // if the buddy is currently in use, can't merge
        if (buddy_pp->order != order) break; // the buddy is empty, and not in the same order, can't merge
        
        // do merge
        //kprintf("free: merge %d with %d\n", index, buddy_index);
        list_del(&buddy_pp->node); // remove buddy from linked-list
        index = get_low_index(index, order); // set pp to left node
        pp = &M(pages)[index];
    }
    
    // finally insert merged node to linked-list
    pp->in_use = 0;
    pp->order = order;
    list_add(&pp->node, &M(list[order]));
}

void buddy_pmalloc__ctor(struct buddy_pmalloc *this, struct virt_vmalloc *valloc, addr_t base, size_t page_size, size_t page_count)
{
    // this function will initialize the buddy physical page allocator
    // after initializtion, all pages is in-use
    // caller should free the pages that can be allocated

    // install vtbl
    INST_VTBL_SINGLETON(this, {
        .malloc = buddy_pmalloc__malloc,
        .free = buddy_pmalloc__free,
        .print = buddy_pmalloc__print,
    });
    
    // init variables
    M(base) = base;
    M(page_size) = page_size;
    M(page_count) = page_count;
    for (M(max_order) = 0; (1 << (M(max_order) + 1)) <= M(page_count); M(max_order)++);
    kprintf("buddy: base %08llx pagesz %08x pagecnt %08x maxorder %08x\n", M(base), M(page_size), M(page_count), M(max_order));
    
    // alloc memory for internal data-structures
    // note that we only alloc memory at construction
    // and we will never release these memory
    M(pages) = VF(valloc, malloc, sizeof(struct buddy_page) * page_count);
    if (!M(pages)) {
        panic("can't alloc memory for buddy physical memory allocator!");
    }
    
    memset(M(pages), 0, sizeof(struct buddy_page) * page_count);
    
    // mark all pages in-use
    for (size_t i = 0; i < page_count; i++) {
        M(pages)[i].order = 0;
        M(pages)[i].in_use = 1;
    }
    
    // init all linked-list to null, i.e. no allocatable node
    for (short order = 0; order <= M(max_order); order++) {
        INIT_LIST_HEAD(&M(list[order]));
    }
}



void pmalloc_bootstrip(struct bootstrap_vmalloc *valloc)
{
    // init pmm zones
    arch_init_pmm_zone();

    // new buddy allocator for each zone
    for (int i = 0; i < MAX_MEMORY_ZONE; i++) {
        struct zone *z = &pmm_zone[i];

        struct buddy_pmalloc *bpa = VF(valloc, malloc, sizeof(struct buddy_pmalloc));
        if (!bpa) panic("can't alloc memory for zone %d", i);
        
        buddy_pmalloc__ctor(bpa, BC(valloc), z->base, z->page_size, z->size / z->page_size);
        z->allocator = BC(bpa);
    }
    
    // free allocable memory regions
    arch_init_free_pmm_zone();
    
    /*VF(pmm_zone[ZONE_DMA].allocator, free, 0x1000 * 0);
    VF(pmm_zone[ZONE_DMA].allocator, free, 0x1000 * 1);
    VF(pmm_zone[ZONE_DMA].allocator, free, 0x1000 * 2);
    VF(pmm_zone[ZONE_DMA].allocator, free, 0x1000 * 3);
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    VF(pmm_zone[ZONE_DMA].allocator, free, 0x1000 * 2);
    VF(pmm_zone[ZONE_DMA].allocator, free, 0x1000 * 3);
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 2));*/

    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    kprintf("alloc = %016llx\n", VF(pmm_zone[ZONE_DMA].allocator, malloc, 0x1000 * 1));
    
    panic("hello!");
}
