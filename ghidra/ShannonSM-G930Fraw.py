# Rename Shannon baseband lib functions for the galaxy s7.
# This flavour is assuming a raw loaded binary. No toc parsing is done etc.
# @category Shannon

import ghidra.app.script.GhidraScript
import ghidra.program.model.symbol.SourceType as SourceType
import ghidra.program.model.lang.RegisterValue as RegisterValue
import ghidra.program.model.address.AddressSet as AddressSet
import ghidra.app.cmd.disassemble.ArmDisassembleCommand as ArmDisassembleCommand
import ghidra.program.flatapi.FlatProgramAPI as FlatProgramAPI


thumb_functions = {
    'debugger_payload': 0x0262ee90,
    'print_regs': 0x0262ef38,
}

arm_functions = {
    "prefetch_abort": 0x0000283c,
}

# function_addrs = {}

def rename_functions(functions, TMode):
    tmode_reg = currentProgram.getProgramContext().getRegister("TMode")
    if tmode_reg is not None:
        if TMode:
            RegisterValue(tmode_reg, b'0x1')
        else:
            RegisterValue(tmode_reg, b'0x0')
    addr_set = AddressSet()

    for func_name in functions:
        entry_addr = toAddr(functions.get(func_name))
        createFunction(entry_addr, func_name)

        addr_set.addRange(entry_addr, entry_addr)

    # Make sure we disassemble in Thumb or Arm mode.
    cmd = ArmDisassembleCommand(addr_set, None, TMode)
    cmd.applyTo(currentProgram, monitor)
    for f in addr_set:
        disassemble(f.getMinAddress())

def custom_prefetch_abort():
    pass


def main():
    rename_functions(arm_functions, False)
    rename_functions(thumb_functions, True)

    # custom_prefetch_abort()

if __name__ == "__main__":
    main()