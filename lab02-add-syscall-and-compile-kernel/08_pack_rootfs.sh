#! /bin/sh

cd work
rm -f rootfs.img.gz
cd rootfs
find . | cpio -H newc -o > ../rootfs.img
cd ../..
