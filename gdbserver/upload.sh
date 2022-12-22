#! /usr/bin/env bash

export NDK_PROJECT_PATH=.

# Personal automation of different machines.
STR="$(uname -a)"
MAC='Mac'
LINUX='Linux'
if [[ "$STR" == *"$MAC"* ]]; then
    adb='/Users/lordoverkirk/platform-tools/adb'
elif [[ "$STR" == *"$LINUX"* ]]; then
    adb='/home/maurits/platform-tools/adb'
fi

while getopts 'hbc' OPTION; do
  case "$OPTION" in
    h)
        echo "Usage: ./upload.sh [options]"
        echo "Options:"
        echo "    b           Build the gdbserver binary and adb push to smartphone."
        echo "    c           Clean the project"
        exit 0
        ;;
    b)
        opt='build'
        ;;
    c)
        opt='clean'
        ;;
    ?)
      echo "Usage: ./upload.sh <output_modem.bin>"
      exit 1
      ;;
  esac
done

if [ -z "$opt" ]; then
    echo "For usage try: ./upload.sh -h"
    exit -1
fi

BUILD='build'
CLEAN='clean'
if [[ "$opt" == *"$BUILD"* ]]; then
    # Build the executable
    ~/Android/Sdk/ndk/25.1.8937393/ndk-build NDK_APPLICATION_MK=./server/Application.mk
    $adb push libs/armeabi-v7a/gdbserver /data/local/tmp

    $adb shell -t "cd /data/local/tmp; sh"
elif [[ "$opt" == *"$CLEAN"* ]]; then
    ~/Android/Sdk/ndk/25.1.8937393/ndk-build clean NDK_APPLICATION_MK=./server/Application.mk
fi
