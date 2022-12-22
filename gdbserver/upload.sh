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

while getopts 'h' OPTION; do
  case "$OPTION" in
    h)
      echo "Usage: ./upload.sh [options]"
      echo "Options:"
      echo "    build           Build the gdbserver binary and adb push to smartphone."
      echo "    clean           Clean the project"
      exit 0
      ;;
    ?)
      echo "Usage: ./upload.sh <output_modem.bin>"
      exit 1
      ;;
  esac
done

BUILD='build'
CLEAN='clean'
if [[ "$1" == *"$BUILD"* ]]; then
    # Build the executable
    ~/Android/Sdk/ndk/25.1.8937393/ndk-build NDK_APPLICATION_MK=./server/Application.mk
    $adb push libs/armeabi-v7a/gdbserver /data/local/tmp

    $adb shell -t "cd /data/local/tmp; sh"
elif [[ "$1" == *"$CLEAN"* ]]; then
    ~/Android/Sdk/ndk/25.1.8937393/ndk-build clean NDK_APPLICATION_MK=./server/Application.mk
fi
