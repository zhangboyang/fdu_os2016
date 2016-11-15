#!/bin/bash

function fail()
{
    echo "fail: $@"
    exit 1
}
function int2bin()
{
    echo -e "#include<stdio.h>\nmain(){int x=$@;fwrite(&x,4,1,stdout);}" | tcc -run -
    true
}

echo -ne '\x1b[1m\x1b[31m'
echo '========== BOOTLOADER SUMMARY =========='
ls -l boot/boot.bin
echo -ne '\x1b[0m'

cp boot/boot.bin kernel7.img

ELF_INFO_START=0x10000
(int2bin `stat --printf="%s" kern/vmaim.elf` && int2bin 0x`crc32 kern/vmaim.elf`) | dd of=kernel7.img bs=512 seek=$[$ELF_INFO_START/512]

ELF_START=0x20000
dd if=kern/vmaim.elf of=kernel7.img bs=512 seek=$[$ELF_START/512]
