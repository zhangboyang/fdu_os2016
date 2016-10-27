/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _AIM_VMM_H
#define _AIM_VMM_H

#include <aim/gfp.h>
#include <libc/string.h>
#include <list.h>

/*
 * Two kinds of memory object allocators (may) exist inside a running kernel.
 * - One or two simple allocators: used in kmalloc()-like routines.
 *   These access interfaces do not allow multiple algorithms, nor multiple
 *   instances inside a running kernel. The only case with two allocators
 *   is the bootstrap of memory management subsystem, where we have to prepare
 *   one temporary allocator to start the page allocator. The temporary
 *   instance is destroyed in the process and is never visible to other
 *   subsystems.
 * - Zero to many cached allocators: These allocators allocate initialized
 *   objects, and preserve their states across multiple allocations. They are
 *   here to prevent the need of frequent initialization of complex objects.
 *   SLAB algorithm is popular for this use.
 */

#ifndef __ASSEMBLER__

/* currently ignored */

struct simple_allocator {
	void *(*alloc)(size_t size, gfp_t flags);
	void (*free)(void *obj);
	size_t (*size)(void *obj);
};

int simple_allocator_init(void);
void set_simple_allocator(struct simple_allocator *allocator);
void get_simple_allocator(struct simple_allocator *allocator);
/* The above get* and set* functions COPIES structs */

struct allocator_cache {
	//lock_t lock;
	void *head; /* recognized by the allocator */
	size_t size;
	size_t align;
	gfp_t flags;
	void (*create_obj)(void *obj);
	void (*destroy_obj)(void *obj);
};

struct caching_allocator {
	int (*create)(struct allocator_cache *cache);
	int (*destroy)(struct allocator_cache *cache);
	void *(*alloc)(struct allocator_cache *cache);
	int (*free)(struct allocator_cache *cache, void *obj);
	void (*trim)(struct allocator_cache *cache);
};

void set_caching_allocator(struct caching_allocator *allocator);

void *kmalloc(size_t size, gfp_t flags);
static inline void *kcalloc(size_t nmemb, size_t size, gfp_t flags)
{
	void *result = kmalloc(nmemb * size, flags);
	memset(result, 0, nmemb * size);
	return result;
}
void kfree(void *obj);
size_t ksize(void *obj);

int cache_create(struct allocator_cache *cache);
int cache_destroy(struct allocator_cache *cache);
void *cache_alloc(struct allocator_cache *cache);
int cache_free(struct allocator_cache *cache, void *obj);
void cache_trim(struct allocator_cache *cache);


////////////////////////////// by ZBY ///////////////////////////////////////
#include <zby.h>

DECLARE_BASE_VCLASS(virt_vmalloc, struct {
    void *(*malloc)(THIS, size_t size);
    void (*free)(THIS, void *ptr);
    size_t (*size)(THIS, void *obj);
}, struct {
    // no data here
});

extern struct virt_vmalloc *g_vmalloc; // the global allocator

DECLARE_DERIVED_VCLASS(bootstrap_vmalloc, virt_vmalloc, struct {
    size_t (*area)(THIS);
}, struct {
    void *base, *limit;
    void *cur;
});


///////////////////////////////////////////////////////////// pmm to vmm bridge
DECLARE_BASE_VCLASS(virt_pvbridge, struct {
    void *(*malloc)(THIS, lsize_t size);
    void *(*aligned_malloc)(THIS, lsize_t size, lsize_t align);
    void (*free)(THIS, void *ptr);
}, struct {
    struct virt_pmalloc *palloc;
});
extern struct virt_pvbridge *g_pvbridge;

// the bridge just add KOFFSET
DECLARE_DERIVED_VCLASS(koffset_pvbridge, virt_pvbridge, struct {
}, struct {
});


///////////////////////////////////////////////////////////// real vmalloc
DECLARE_DERIVED_VCLASS(use_page_vmalloc, virt_vmalloc, struct {
}, struct {
    struct virt_pvbridge *pvbridge;
});


struct slub_header {
    struct list_head node;
    struct list_head free_list;
    int level;
};

DECLARE_DERIVED_VCLASS(slub_vmalloc, virt_vmalloc, struct {
}, struct {
    struct virt_pvbridge *pvbridge;
#define SLUB_BLOCK_LEVEL 14
#define SLUB_BLOCK_SIZE (1LL << SLUB_BLOCK_LEVEL)

#define MIN_SLUB_LEVEL 4
#define MAX_SLUB_LEVEL 20
#define SLUB_LEVEL_INDEX(x) ((x) - MIN_SLUB_LEVEL)
#define SLUB_TOTAL_LEVELS (MAX_SLUB_LEVEL - MIN_SLUB_LEVEL + 1)
    struct list_head slub_avail[SLUB_TOTAL_LEVELS];
    struct list_head slub_full[SLUB_TOTAL_LEVELS];
});

_Static_assert(sizeof(struct list_head) > (1 << MIN_SLUB_LEVEL), "list head is too big");

///////////////////////////////////////////////////////////////// functions

extern void use_page_vmalloc__ctor(struct use_page_vmalloc *this, struct virt_pvbridge *pvbridge);
extern void bootstrap_vmalloc__ctor(struct bootstrap_vmalloc *this, void *base, size_t max_size);
void koffset_pvbridge__ctor(struct koffset_pvbridge *this, struct virt_pmalloc *palloc);

extern void install_vmm_adapter();

extern void vmm_bootstrap();


#endif /* !__ASSEMBLER__ */

#endif /* _AIM_VMM_H */

