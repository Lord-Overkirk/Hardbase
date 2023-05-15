import gdb
import json

MEMORY_MAP = [
    (0x04000000, 0x04020000),
    (0x04800000, 0x04804000),
    (0x40000000, 0x48000000),
    # (0x46000000, 0x48000000),
]

def store_registers(regs):
    with open("registers.json", "w") as json_file:
        json.dump(regs, json_file)

def parse_registers(regs):
    register_dict = {}
    reg_list = regs.splitlines()
    for reg_line in reg_list:
        (register, value) = reg_line.split(' ', 1)
        value = value.strip()
        (hex_value, _) = value.split(' ', 1)
        register_dict.update({register: hex_value})
    return register_dict


def main():
    gdb.execute("target remote :1337")
    registers = gdb.execute("i r", to_string=True)
    reg_dict = parse_registers(registers)
    store_registers(reg_dict)
    gdb.execute("b *0x40620aec")
    gdb.execute("c")
    for mem_region in MEMORY_MAP:
        start = mem_region[0]
        end = mem_region[1]
        start_str = str(hex(start))
        end_str = str(hex(end))

        gdb.execute(f"dump binary memory {start_str}-{end_str}.bin {start} {end}")


if __name__ == "__main__":
    main()