#!/bin/bash

function fail()
{
    echo "fail: $@"
    exit 1
}

rm -f disk.img || fail "can't remove previous disk image"

dd of=disk.img bs=512 seek=1048576 count=0 || fail "can't create new disk image"

echo -ne '\x1b[1m\x1b[31m'
echo '========== BOOTLOADER SUMMARY =========='
ls -l boot/boot.bin
echo -ne '\x1b[0m'

dd if=boot/boot.bin bs=440 count=1 of=disk.img conv=notrunc || fail "can't copy bootloader to disk image"

echo -e '2048,32768\n34816,' | sfdisk --force -u S disk.img || fail "can't create partitions"

# the magic number 2048 is offset of partition 1
dd if=boot/boot.elf of=disk.img conv=notrunc bs=512 seek=2048 || fail "can't copy bootloader"

# the magic number 34816 is offset of partition 2
dd if=kern/vmaim.elf of=disk.img conv=notrunc bs=512 seek=34816 || fail "can't copy kernel"

echo '========================================'
