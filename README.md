# Hardbase

A **hard**ware in the loop injector for SHANNON **base**bands.

## Dependencies
* Install `adb`. Download Android [platform-tools](https://developer.android.com/studio/command-line/adb)
* Root access for the Samsung smartphone is required.
* python >= 3.10 for the gdbserver.
* [pySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) for AT serial communication.

## Getting started
Use the [el3_patcher](https://github.com/synacktiv/shannon-dbg) to enable the pushing of custom modem files on the UE. Run `./patch_el3` on the UE.

## Project structure
The `injector` directory contains the files that are required for building code that is injected directly into the baseband firmware. Current debugging capabilities are register reads, memory reads and breakpoint placements through AT commands.

The `gdbserver` directory contains the files that implement the [gdb remote serial protocol](https://www.embecosm.com/appnotes/ean4/embecosm-howto-rsp-server-ean4-issue-2.html). Running `gdbserver.py` can be used to serve as a wrapper for the various (custom) AT commands. It can be targeted from a remote gdb instance.

`modem_files` contains baseband firmware.

`ue_mem_dumps` contains example UE memory dumps for the three targeted ShannonOS tasks. Additionally, a script is present that places a breakpoint and dumps the memory once the breakpoint is hit.

## Usage
Both directory contain bash scripts that build and push the relevant code to the smartphone via `adb`. Sending raw AT commands is recommended for manual exploration. If one wants to dump the UE memory at a breakpoint, consult the `ue_mem_dumps` folder.

### AT commands
The custom AT commands are structured as follows:
#### Register reads
`AT+DEBUG=REG`: Shows the registers at the placed breakpoint, if no breakpoint was set, it points to some default values needed for the gdbserver.
#### Memory reads
`AT+DEBUG=MEM|r|Start address|End address|0`\
For example:\
`AT+DEBUG=MEM|r|40000000|41000000|0`: Reads memory at the given range. Result is returned in bytes.
#### Memory writes/breakpoint placement
`AT+DEBUG=MEM|w|Start address|00000000|payload_lenght|payload`\
For example:\
`AT+DEBUG=MEM|w|0x40687d00|00000000|4|01be`: Injects a hexadecimal payload at a given address. The length is the amount of hex digits. In this example, the 0x01be bytes result in an ARM bkpt instruction. This is how memory writes serve as a breakpoint placement. The gdbserver performs this task automatically.