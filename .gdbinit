target remote :1234

symbol kern/vmaim.elf
layout asm

#b *0x00007c00
#set arch i8086
#c

#b *0x01000000
set arch i386

