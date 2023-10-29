#! /bin/sh

set -e

. ./config.sh

echo "Preparing directories..."

rm -rf $WORK_DIR
mkdir $WORK_DIR
mkdir -p $SOURCE_DIR

echo "Done." 

