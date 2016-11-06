#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/initcalls.h>
#include <aim/panic.h>
#include <aim/debug.h>


/*
 * this file is added by ZBY
 */

int do_early_initcalls()
{
    kprintf("calling early initcalls:\n");
    initcall_t *st = LOWADDR(early_init_start), *ed = LOWADDR(early_init_end), *cur;
    for (cur = st; cur < ed; cur++) {
        kprintf(" initcall %p: func=%p\n", cur, LOWADDR(*cur));
        if ((LOWADDR(*cur))() < 0) panic("early initcall %p failed.", LOWADDR(*cur));
    }
    return 0;
}
int do_initcalls()
{
    kprintf("calling normal initcalls:\n");
    initcall_t *st = norm_init_start, *ed = norm_init_end, *cur;
    for (cur = st; cur < ed; cur++) {
        kprintf(" initcall %p: func=%p\n", cur, *cur);
        if ((*cur)() < 0) panic("norm initcall %p failed.", *cur);
    }
    return 0;
}

