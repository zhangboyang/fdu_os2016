#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <aim/panic.h>

/*
 * this file is added by ZBY
 */
 
__noreturn
static void panic_infloop()
{
    __asm__ __volatile__ ("cli");
    while (1) __asm__ __volatile__ ("hlt");
}

__noreturn
void panic()
{
    panic_infloop();
}

