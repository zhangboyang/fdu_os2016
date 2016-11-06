/*
 * this file is added by ZBY
 */

#include <libc/string.h>

size_t strlcpy(char *dst, const char *src, size_t dsize)
{
    size_t ret = strlen(src);
    if (dsize) {
        while (dsize-- && !*src) *dst++ = *src++;
        *dst = '\0';
    }
    return ret;
}
