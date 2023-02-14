# Rename Shannon baseband lib functions for the galaxy s7
# @category Shannon

import ghidra.app.script.GhidraScript
import ghidra.program.model.symbol.SourceType as SourceType
import ghidra.program.model.lang.RegisterValue as RegisterValue
import ghidra.program.model.address.AddressSet as AddressSet
import ghidra.app.cmd.disassemble.ArmDisassembleCommand as ArmDisassembleCommand

thumb_functions = {
    "printlen": 0x40e895e9,
    "crlf": 0x40e88a49,
    "strlen": 0x40f146a5,
    "sprintf": 0x40f0c891,
    "strtok": 0x40f7c639,
    "strtol": 0x40a9794d,
    "atoi": 0x40f0153d,
    "debug_payload": 0x47C00001,
}

arm_functions = {
    "prefetch_abort": 0x400100bc,
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

    for name in functions:
        func_entry = toAddr(functions.get(name))
        if TMode:
            created = createFunction(func_entry.previous(), name)
        else:
            created = createFunction(func_entry, name)

        addr_set.addRange(func_entry, func_entry)

    # Make sure we disassemble in Thumb or Arm mode.
    cmd = ArmDisassembleCommand(addr_set, None, TMode)
    cmd.applyTo(currentProgram, monitor)
    for f in addr_set:
        disassemble(f.getMinAddress())

def main():
    rename_functions(thumb_functions, True)
    rename_functions(arm_functions, False)

if __name__ == "__main__":
    main()