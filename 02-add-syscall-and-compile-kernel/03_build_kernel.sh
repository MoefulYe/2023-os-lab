#! /bin/sh
set -e

cd work/kernel
cd $(ls -d * | grep linux)
#清理上次构建的结果
make clean
#生成默认构建配置
make defconfig
#修改默认主机名
sed -i "s/.*CONFIG_DEFAULT_HOSTNAME.*/CONFIG_DEFAULT_HOSTNAME=\"my-minimal-linux\"/" .config
#构建内核镜像
make -j8
cd ../../..
