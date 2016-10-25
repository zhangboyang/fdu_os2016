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


static addr_t buddy_pmalloc__malloc(THIS, lsize_t size)
{
/*    DECLARE_THIS(buddy_pmalloc);
    size_t pcnt = DIV_ROUND_UP(size, M(page_size));*/
    return -1;    
}

#define get_buddy(index, order) ((index) ^ (1 << (order)))
#define get_low_index(index, order) ((index) & ~(1 << (order)))
static void buddy_pmalloc__free(THIS, addr_t ptr)
{
    DECLARE_THIS(buddy_pmalloc);
    assert(ROUNDDOWN(ptr, M(page_size)) == ptr);
    
    // calc index in pages[]
    size_t index = (ptr - M(base)) / M(page_size);
    struct buddy_page *pp = &M(pages)[index]; // pp: pointer to page
    assert((index & ((1 << pp->order) - 1)) == 0);
    
    // clear in_use flag
    pp->in_use = 0;
    
    // try to merge with buddy
    short order;
    for (order = pp->order; order < M(max_order); order++)
        // check if we can merge
        size_t buddy_index = get_buddy(index, pp->order);
        if (buddy_index >= M(page_count)) break; // if the buddy index is invalid (out of range), exit loop
        struct buddy_page *buddy_pp = &M(pages)[buddy_index];
        if (buddy_pp->in_use) break; // if the buddy is currently in use, can't merge
        if (buddy_pp->order != pp->order) break; // the buddy is not in the same order, can't merge
        
        // start merge
        list_del(&buddy_pp->node); // remove buddy from linked-list
        pp = &M(pages)[get_low_index(index)]; // set pp to left node
    }
    
    // finally insert merged node to linked-list
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
    for (int order = 0; order < M(max_order); order++) {
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
    
    panic("hello!");
}
