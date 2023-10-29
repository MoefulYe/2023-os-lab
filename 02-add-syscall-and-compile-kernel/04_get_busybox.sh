#! /bin/sh

set -e

. ./config.sh

DOWNLOAD_URL=$BUSYBOX_SOURCE_URL
ARCHIVE_NAME=$(basename $DOWNLOAD_URL)

cd work
rm -f $ARCHIVE_NAME
wget $DOWNLOAD_URL
rm -rf busybox
mkdir busybox
tar -xvf $ARCHIVE_NAME -C busybox
cd ..
