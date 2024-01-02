#! /bin/sh

set -e

cd work/busybox
cd $(ls -d * | grep busybox)
make clean
make defconfig
# 生成经他静态链接的busybox, 否则在最小环境中无法运行
sed -i "s/.*CONFIG_STATIC.*/CONFIG_STATIC=y/" .config
make busybox -j8
make install -j8
cd ../../..
