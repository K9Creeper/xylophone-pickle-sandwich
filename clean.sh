#!/bin/bash
export BUILD_BIN_PATH=build/.bin
export BUILD_ISO_PATH=build/.iso

clear

rm -f $BUILD_BIN_PATH/*.o
rm -f $BUILD_BIN_PATH/*.bin
rm -f $BUILD_ISO_PATH/boot/*.bin
rm -f $BUILD_ISO_PATH/boot/grub/*.img
rm -f build/*.iso
rm -f build/os.img

if ! [ -e "debug-log.txt" ]; then
  > debug-log.txt
fi