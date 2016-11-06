/*
 * this file is added by ZBY
 */

#include <libc/string.h>

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *pdest = dest;
    const unsigned char *psrc = src;
    if (pdest < psrc) {
        while (n--) *pdest++ = *psrc++;
    } else if (pdest > psrc) {
        pdest += n; psrc += n;
        while (n--) *--pdest = *--psrc;
    }
    return dest;
}
