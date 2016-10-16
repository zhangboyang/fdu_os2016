#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>

/*
 * this file is added by ZBY
 */

void master_init()
{
    bprintf("hello world! we are now at high address!\n");
    
    panic("haha");
}

