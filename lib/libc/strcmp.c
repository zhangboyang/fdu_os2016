/*
 * this file is added by ZBY
 */

#include <libc/string.h>
#include <util.h>

int strcmp(const char *s1, const char *s2)
{
    size_t l1 = strlen(s1), l2 = strlen(s2);
    size_t len = min(l1, l2) + 1;
    return memcmp(s1, s2, len);
}
