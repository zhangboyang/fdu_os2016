#ifndef _ASM_H
#define _ASM_H

static inline uint32_t inl(uint32_t addr)
{
    uint32_t ret;
    __asm__ __volatile__ ("ldr %0,[%1]":"=r"(ret):"r"(addr));
	return 0;
}

#endif
