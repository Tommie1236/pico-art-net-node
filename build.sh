#!/bin/sh

set -e    # Exit on error

touch Code/src/firmware/main.cpp # touch file to always print linker memory usage

git submodule update --init --recursive
cd Code/lib/pico-sdk/
export PICO_SDK_PATH=$PWD
cd ../..

mkdir -p build
cp ./lib/pico-sdk/external/pico_sdk_import.cmake .
cd build
cmake .. -DPICO_BOARD=pico
make -j $(($(nproc) * 2))

while getopts 'l' flag; do
  case "${flag}" in (l) 
      echo "Loading firmware <$(pwd)/src/firmware/firmware.bin> to RP2040"
      sudo picotool load $(pwd)/src/firmware/firmware.bin -f
      ;;
  esac
done