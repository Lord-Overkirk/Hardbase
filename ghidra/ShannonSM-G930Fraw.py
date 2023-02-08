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
    'debugger_payload': b'\xb5\x89\xb0\x48\xf6\x49\x23\xc4\xf2\xe8\x03',
    'print_regs': b'\xb5\x8f\xb0\x02\x46'
}

arm_functions = {
    "prefetch_abort": b'\x04\xe0\x4e\xe2\x00\x40\x2d\xe9'
}

def rename_functions(functions, TMode):
    mgr = currentProgram.getFunctionManager()

    tmode_reg = currentProgram.getProgramContext().getRegister("TMode")
    if tmode_reg is not None:
        if TMode:
            arm_mode = RegisterValue(tmode_reg, b'0x1')
        else:
            arm_mode = RegisterValue(tmode_reg, b'0x0')
    addr_set = AddressSet()

    for func_name in functions:
        entry_addr = find(toAddr(0), functions.get(func_name))
        if entry_addr is not None:
            print(entry_addr)
        else:
            raise

        if TMode:
            created = createFunction(entry_addr.previous(), func_name)
        else:
            created = createFunction(entry_addr, func_name)

        addr_set.addRange(entry_addr, entry_addr)

    # Make sure we disassemble in Thumb or Arm mode.
    cmd = ArmDisassembleCommand(addr_set, None, TMode)
    cmd.applyTo(currentProgram, monitor)
    for f in addr_set:
        disassemble(f.getMinAddress())

def main():
    rename_functions(arm_functions, False)
    rename_functions(thumb_functions, True)

if __name__ == "__main__":
    main()