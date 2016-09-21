#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <elf.h>
#include <aim/boot.h>
#include <arch-boot.h>

/*
 * this file is added by ZBY
 */



void bputs(const char *str)
{
    while (*str) bputc(*str++);
    bputc('\n');
}





// naive-stdio
// see: https://github.com/zhangboyang/naive-stdio

static int write_char(char c)
{
    bputc(c);
    return 1;
}

static int write_string(char *s)
{
    int cnt = 0;
    while (*s) cnt += write_char(*s++);
    return cnt;
}

static int write_int(int d, unsigned int r)
{
    const char *dict = "0123456789abcdef";
    unsigned char buf[20] = {};
    int p = 0, ret;
    unsigned int u;
    if (d < 0) { write_char('-'); d = -d; }
    u = d;
    do {
        buf[p++] = u % r;
        u /= r;
    } while (u > 0);
    ret = p;
    if (r == 16) p = 8;
    while (p > 0) write_char(dict[buf[--p]]);
    return ret;
}


int bprintf(const char *fmt, ...)
{
    va_list ap;
    long long lld;
    int d;
    char c, *s;
    int cnt = 0;
    
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            switch (*++fmt) {
                case 's':
                    s = va_arg(ap, char *);
                    cnt += write_string(s);
                    break;
                case 'd':
                    d = va_arg(ap, int);
                    cnt += write_int(d, 10);
                    break;
                case 'x':
                    d = va_arg(ap, int);
                    cnt += write_int(d, 16);
                    break;
                case 'c':
                    c = va_arg(ap, int);
                    cnt += write_char(c);
                    break;
                case '%':
                    cnt += write_char('%');
                    break;
            }
        } else {
            cnt += write_char(*fmt);
        }
        fmt++;
    }
    va_end(ap);
    return cnt;
}
