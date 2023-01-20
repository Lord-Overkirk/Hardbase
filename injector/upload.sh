#! /usr/bin/env bash

MY_CONTAINER_ID="b9cd6463e7cf"
MY_CONTAINER_NAME="priceless_swartz"

modem_dir='./../modem_files'
build_dir='./src/build'

# Personal automation of different machines.
STR="$(uname -a)"
MAC='Mac'
LINUX='Linux'
if [[ "$STR" == *"$MAC"* ]]; then
    adb='/Users/lordoverkirk/platform-tools/adb'
elif [[ "$STR" == *"$LINUX"* ]]; then
    adb='/home/maurits/platform-tools/adb'
fi

while getopts 'hm:' OPTION; do
  case "$OPTION" in
    h)
        echo "Usage: ./upload.sh -m <output_modem.bin>"
        exit 1
        ;;
    m)
        modem_file="$OPTARG"
        ;;
    ?)
        echo "Usage: ./upload.sh -m <output_modem.bin>"
        exit 1
        ;;
  esac
done

if [ -z "$modem_file" ]; then
    echo "Usage: ./upload.sh -m <output_modem.bin>"
    EXIT_STR=$'Patch name not present.\nExiting...'
    echo "$EXIT_STR"
    exit -1
else
    echo $modem_file
fi

make

python2 patch_modem.py "${build_dir}/debugger.bin" "${modem_dir}/modem.bin" $modem_file
echo $adb
$adb push $modem_file /data/local/tmp

# Start the modified baseband firmware.
$adb shell -t "su -c cbd -d -tss310 -bm -mm -P ../../data/local/tmp/$modem_file"
