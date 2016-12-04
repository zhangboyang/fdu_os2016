#ifndef _ASM_H
#define _ASM_H

#define barrier() do { __asm__ __volatile__ ("": : :"memory"); } while (0)

static inline void dmb()
{
    __asm__ __volatile__ ("dmb":::"memory");
}
static inline void dsb()
{
    __asm__ __volatile__ ("dsb":::"memory");
}
static inline void isb()
{
    __asm__ __volatile__ ("isb":::"memory");
}


static inline void outl(uint32_t addr, uint32_t data)
{
    __asm__ __volatile__ ("str %0, [%1]"::"r"(data), "r"(addr):"memory");
}

static inline uint32_t inl(uint32_t addr)
{
    uint32_t ret;
    __asm__ __volatile__ ("ldr %0, [%1]":"=r"(ret):"r"(addr):"memory");
    dmb();
	return ret;
}

static inline void nop()
{
    __asm__ __volatile__ ("nop");
}




//////////////// spinlock ////////////////////
typedef struct {
    int locked;
} lock_t;

/* By initializing a lock, caller assumes no code is holding it. */
static inline void spinlock_init(lock_t *lock)
{
    lock->locked = 0;
}
static inline void spin_lock(lock_t *lock)
{
    uint32_t result = 1;
    while (result) {
        __asm__ __volatile__ (
            "ldrex r0, [%1]\n\t"
            "cmp r0, #0\n\t"
            "mov r0, #1\n\t"
            "strexeq r0, %0, [%1]\n\t"
        : "=r"(result)
        : "r"(&lock->locked)
        : "r0"
        );
    }
    dmb();
}
/* spin_unlock may contain instructions to send event */
static inline void spin_unlock(lock_t *lock)
{
    dmb();
    lock->locked = 0;
    dsb();
}
static inline bool spin_is_locked(lock_t *lock)
{
    return lock->locked == 1;
}

#define local_irq_save(flags) \
    do { \
    } while (0)

#define local_irq_restore(flags) \
    do { \
    } while (0)

#define spin_lock_irq_save(lock, flags) \
	do { \
		local_irq_save(flags); \
		spin_lock(lock); \
	} while (0)
#define spin_unlock_irq_restore(lock, flags) \
	do { \
		spin_unlock(lock); \
		local_irq_restore(flags); \
	} while (0)



#endif
