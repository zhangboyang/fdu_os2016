/*
 * this file is added by ZBY
 */

#include <libc/string.h>
#include <util.h>

size_t strlen(const char *s)
{
    size_t r = 0;
    while (*s++) r++;
    return r;
}
