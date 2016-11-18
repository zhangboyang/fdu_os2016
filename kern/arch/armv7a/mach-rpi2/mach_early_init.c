#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <platform.h>

void mach_early_init()
{
    mailbox_init();
}
