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
    uint32_t cur_crc32 = crc32(0, elf_data, elf_info->elf_len);
    if (cur_crc32 != elf_info->elf_crc32) {
        bpanic("crc32 mismatch, current %x, expected %x.", cur_crc32, elf_info->elf_crc32);
    }
}

void kernreader_readfile(void *buf, size_t size, size_t offset)
{
    if (buf < (void *) (elf_data + elf_info->elf_len)) {
        bpanic("phdr overlap detected, buf=%p elfend=%p size=%x offset=%x.", buf, elf_data + elf_info->elf_len, size, offset);
    }
    bmemcpy(buf, elf_data + offset, size);
}

