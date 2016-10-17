target remote :1234
symbol kern/vmaim.elf
layout asm
b *0x00007c00
b *0x01000000
c

