#!/bin/sh

set -e    # Exit on error

touch Code/src/firmware/main.cpp # touch file to always print linker memory usage

git submodule update --init
cd Code/lib/Pico-DMX
git submodule update --init
cd ../pico-sdk/
export PICO_SDK_PATH=$PWD
git submodule update --init 
cd ../ArtNet
git submodule update --init
cd ../Ethernet
git submodule update --init
cd ../..

mkdir -p build
cp ./lib/pico-sdk/external/pico_sdk_import.cmake .
cd build
cmake .. -DPICO_BOARD=pico_w
make -j $(($(nproc) * 2))

while getopts 'l' flag; do
  case "${flag}" in (l) 
      echo "Loading firmware <$(pwd)/src/firmware/firmware.bin> to RP2040"
      sudo picotool load $(pwd)/src/firmware/firmware.bin -f
      ;;
  esac
done