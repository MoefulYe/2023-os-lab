#! /bin/sh

set -e 

. ./config.sh

DOWNLOAD_URL=$KERNEL_SOURCE_URL
ARCHIVE_NAME=$(basename $DOWNLOAD_URL)

cd work
rm -f $ARCHIVE_NAME
wget $DOWNLOAD_URL
rm -rf kernel
mkdir kernel
tar -xvf $ARCHIVE_NAME -C kernel
cd ..
