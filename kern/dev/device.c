#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/initcalls.h>
#include <aim/panic.h>
#include <aim/device.h>
#include <libc/string.h>

/*
 * this file is added by ZBY
 */

void initdev(struct device *dev, int class, const char *devname, dev_t devno, struct driver *drv)
{
    dev->class = class;
    dev->devno = devno;
    strncpy(dev->name, devname, sizeof(dev->name)); dev->name[sizeof(dev->name) - 1] = 0;
    size_t sz;
    switch (class) {
        case DEVCLASS_NON: sz = sizeof(struct driver); break;
        case DEVCLASS_CHR: sz = sizeof(struct chr_driver); break;
        case DEVCLASS_BLK: sz = sizeof(struct blk_driver); break;
        case DEVCLASS_NET: sz = sizeof(struct net_driver); break;
        case DEVCLASS_BUS: sz = sizeof(struct bus_driver); break;
        default: panic("unknown dev class %d", class);
    }
    memmove(&dev->driver, drv, sz);
}
