#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/initcalls.h>
#include <aim/panic.h>


/*
 * this file is added by ZBY
 */

int do_early_initcalls()
{
    initcall_t *st = early_init_start, *ed = early_init_end, *cur;
    for (cur = st; cur < ed; cur++) {
        if ((*cur)() < 0) panic("early initcall %p failed.", *cur);
    }
}
int do_initcalls()
{
    initcall_t *st = norm_init_start, *ed = snorm_init_end, *cur;
    for (cur = st; cur < ed; cur++) {
        if ((*cur)() < 0) panic("norm initcall %p failed.", *cur);
    }
}
