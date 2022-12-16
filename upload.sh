#! /usr/bin/env bash

MY_CONTAINER_ID="b9cd6463e7cf"
MY_CONTAINER_NAME="priceless_swartz"

modem_dir='./modem_files'
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

if [ -z "$1" ]; then
    EXIT_STR=$'Patch name not present.\nExiting...'
    echo "$EXIT_STR"
    exit -1
else
    echo $1
fi

# docker start $MY_CONTAINER_ID

# docker exec -it $MY_CONTAINER_NAME make -C ../workspaces/FirmWire/modkit/shannon/

# Copy modkit to local
# docker cp $MY_CONTAINER_NAME:../workspaces/FirmWire/modkit/shannon/build/adjuster.bin $modem_dir

# We dont need the docker container at this point.
# docker stop $MY_CONTAINER_ID
make


python2 patch_modem.py "${build_dir}/adjuster.bin" "${modem_dir}/modem.bin" $1
echo $adb
$adb push $1 /data/local/tmp
$adb shell -t "su"
