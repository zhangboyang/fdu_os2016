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
