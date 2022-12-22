# Hardbase

A **hard**ware in the loop injector for SHANNON **base**bands.

## Dependencies
* Install `adb`. Download Android [platform-tools](https://developer.android.com/studio/command-line/adb)
* Root access for the Samsung smartphone is required.
* The Android Native Development Kit ([NDK](https://developer.android.com/ndk)) is required for running C code on an Android device. This is required for the `gdbserver`.

## Project structure
The `injector` directory contains the files that are required for building code that is injected directly into the baseband firmware.

The `gdbserver` directory contains the files that implement the [gdb remote serial protocol](https://www.embecosm.com/appnotes/ean4/embecosm-howto-rsp-server-ean4-issue-2.html). The compiled binary should be run on the AP of the targeted smartphone. It can be targeted from a remote gdb instance.

## Usage
Both directory contain bash scripts that build and push the relevant code to the smartphone via `adb`.