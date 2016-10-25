/*
    ZBY:
    this is the buddy physical page allocator
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>
#include <aim/mmu.h>
#include <aim/pmm.h>
#include <aim/vmm.h>


static addr_t buddy_pmalloc__malloc(THIS, size_t size)
{
    return -1;    
}

static void buddy_pmalloc__free(THIS, addr_t ptr)
{

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
        M(pages)[i].in_use = 1;
    }
    
    // init all linked-list to null, i.e. no allocatable node
    for (int order = 0; order < M(max_order); order++) {
        INIT_LIST_HEAD(&M(pages)[i].list[order]);
    }
}

