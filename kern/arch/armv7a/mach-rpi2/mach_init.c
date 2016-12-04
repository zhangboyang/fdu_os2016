#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <bcm2836.h>

void mach_init()
{
    bcm2836_init();
}
