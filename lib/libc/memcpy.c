/*
 * this file is added by ZBY
 */

#include <libc/string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    while (n--) {
        if (*s1 != *s2) return *s1 < *s2 ? -1 : 1;
        s1++, s2++;
    }
    return 0;
}
