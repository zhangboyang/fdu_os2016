#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <platform.h>

void mach_early_init()
{
    mailbox_init();
    
    int r;
    #define VALBUFSIZE (16 * 4)
    #define BUFSIZE (VALBUFSIZE + 3 * sizeof(uint32_t))
    uint32_t buf[BUFSIZE / 4];
    struct property_tag *tag = (void *) &buf;
    *tag = (struct property_tag) {
        .id = MAILBOX_PROP_ARMMEMORY,
        .bufsize = VALBUFSIZE,
        .size = 0,
        .type = PROPERTY_TAG_REQUEST,
    };
    r = ask_property(tag, BUFSIZE, BUFSIZE);
    kprintf("r=%d memory=0x%x\n", r, tag->val32[0]);
}
