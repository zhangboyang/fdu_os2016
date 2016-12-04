#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <bcm2836.h>

void mach_init()
{
    bcm2836_init();
}
