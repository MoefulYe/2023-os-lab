#! /bin/sh

qemu-system-x86_64 -m 128M -kernel work/kernel/linux-5.15.137/arch/x86/boot/bzImage -initrd work/rootfs.img -append "rdinit=/init" -vga std
