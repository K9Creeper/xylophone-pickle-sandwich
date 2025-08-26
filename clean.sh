#!/bin/bash
export BUILD_BIN_PATH=build/.bin
export BUILD_PATH=build

rm -f $BUILD_BIN_PATH/*.o
rm -f $BUILD_BIN_PATH/*.bin
rm -f $BUILD_PATH/boot/*.bin
rm -f $BUILD_PATH/boot/grub/*.img
rm -f $BUILD_PATH/*.iso
rm -f $BUILD_PATH/*.img
rm -f $BUILD_PATH/*.bin

if ! [ -e "debug-log.txt" ]; then
  > debug-log.txt
fi