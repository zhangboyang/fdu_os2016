#ifndef _ASM_H
#define _ASM_H

static inline void dmb()
{
    __asm__ __volatile__ ("dmb");
}
static inline void dsb()
{
    __asm__ __volatile__ ("dsb");
}
static inline void isb()
{
    __asm__ __volatile__ ("isb");
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

#endif
