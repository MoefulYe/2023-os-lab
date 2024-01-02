#! /bin/sh

set -e

cd work
rm -rf rootfs
cd busybox
cd $(ls -d * | grep busybox)
cp -r _install ../../rootfs
cd ../../rootfs
rm -f linuxrc
cp ../../user/my_nice bin/my_nice
mkdir dev etc proc src sys tmp
cd etc
touch welcome.txt
echo >> welcome.txt
echo '  #####################################' >> welcome.txt
echo '  #                                   #' >> welcome.txt
echo '  #  Welcome  to  "My-Minimal-Linux"  #' >> welcome.txt
echo '  #                                   #' >> welcome.txt
echo '  #####################################' >> welcome.txt
echo >> welcome.txt
cd ..
touch init
echo '#! /bin/sh' >> init
echo 'dmesg -n 1' >> init
echo 'mount -t devtmpfs none /dev' >> init
echo 'mount -t proc none /proc' >> init
echo 'mount -t sysfs none /sys' >> init
echo 'cat /etc/welcome.txt' >> init
echo 'while true' >> init
echo 'do' >> init
echo '  setsid cttyhack /bin/sh' >> init
echo 'done' >> init
echo >> init
chmod +x init
cp ../../*.sh src
cd ../..
