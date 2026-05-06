#!/bin/sh

set -e    # Exit on error

touch Code/src/firmware/main.c # touch file to always print linker memory usage

git submodule update --init --remote 
cd Code/lib/pico-sdk/
git submodule update --init --remote lib/tinyusb 
export PICO_SDK_PATH=$PWD
cd ../..

mkdir -p build
cp ./lib/pico-sdk/external/pico_sdk_import.cmake .
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. -DPICO_BOARD=pico -DPICOTOOL_FORCE_FETCH_FROM_GIT=1
make -j $(($(nproc) * 2))

while getopts 'l' flag; do
  case "${flag}" in (l) 
      echo "Loading firmware <$(pwd)/src/firmware/firmware.bin> to RP2040"
      sudo picotool load $(pwd)/src/firmware/firmware.bin -f 
      ;;
  esac
done
