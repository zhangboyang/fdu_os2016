#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/boot.h>
#include <arch-boot.h>

/*
 * this file is added by ZBY
 */


struct bootloader_elf_info {
    uint32_t elf_len;
    uint32_t elf_crc32;
};

#define elf_info ((struct bootloader_elf_info *)(ELF_INFO_START))
#define elf_data ((uint8_t *)(ELF_START))

void kernreader_init(void)
{
    // check ELF crc32
    bprintf("ELF size %x, start %p, end %p\n", elf_info->elf_len, elf_data, elf_data + elf_info->elf_len);
    if (elf_info->elf_len == 0) bpanic("ELF has no length!");
    bprintf("checking ELF crc32 checksum ... ");
    uint32_t cur_crc32 = crc32(0, elf_data, elf_info->elf_len);
    if (cur_crc32 != elf_info->elf_crc32) {
        bpanic("BAD, current %x, expected %x.", cur_crc32, elf_info->elf_crc32);
    }
    bprintf("%x, OK\n", cur_crc32);
}

void kernreader_readfile(void *buf, size_t size, size_t offset)
{
    if ((void *) elf_data <= buf && buf < (void *)(elf_data + elf_info->elf_len)) { // not a robust detection
        bpanic("overlap detected! buf=%p size=%x offset=%x", buf, size, offset);
    }
    bmemcpy(buf, elf_data + offset, size);
    bprintf("[%d bytes copied from ELF]\n", size);
}

