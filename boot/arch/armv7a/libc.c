#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>

// tiny libc

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--) *p++ = c;
    return s;
}
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *pdest = dest;
    const unsigned char *psrc = src;
    while (n--) *pdest++ = *psrc++;
    return dest;
}
