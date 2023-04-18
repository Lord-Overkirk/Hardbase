import serial
import DebugCommand
import socket
import sys

class WriteFailedException(Exception):
    "Memory write failed"
    pass

class ATDebugger:
    def __init__(self):
        if sys.platform != 'darwin':
            TTY = '/dev/ttyACM0'
        else:
            TTY = '/dev/tty.usbmodem11202'
        try:
            self.ser = serial.Serial(port=TTY,
                                     baudrate=9600,
                                     bytesize=8,
                                     parity=serial.PARITY_NONE,
                                     stopbits=1,
                                     timeout=0.6, # with set remote memory-read-packet-size 0x6000 in gdb
                                     write_timeout=None,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
        except serial.SerialException:
            print(f"Error: No modem running on port {TTY}")
            exit()
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()


    def get_registers(self):
        """Read the register contents through AT commands."""
        cmd = DebugCommand.DebugCommand(DebugCommand.REGISTERS, DebugCommand.READ)
        cmd_str = cmd.build()
        raw_response = self.send(cmd_str)
        print(raw_response)
        reg_list = raw_response.splitlines()[2:-2]
        register_values = [reg.split(' ')[1].removeprefix('0x') for reg in reg_list]
        # Convert endianess
        r = [socket.htonl(int(reg, 16)) for reg in register_values]
        # r = [int(reg,16) for reg in register_values]
        regs = ['{:08x}'.format(reg) for reg in r]
        return ''.join(regs)

    def write_memory(self, addr, payload):
        cmd = DebugCommand.DebugCommand(DebugCommand.MEMORY, DebugCommand.WRITE, addr, payload=payload)
        cmd_str = cmd.build()
        print(cmd_str)
        r = self.send(cmd_str)
        print(r)
        if r[-4:] != 'OK\r\n':
            raise WriteFailedException


    def read_memory(self, addr, size):
        """Read memory of specified size via AT commands."""
        cmd = DebugCommand.DebugCommand(DebugCommand.MEMORY, DebugCommand.READ)
        cmd.memory_start = addr
        cmd.memory_end = addr + size
        cmd_str = cmd.build()
        raw_response = self.send(cmd_str)
        hex_bytes = raw_response.splitlines()[2].split()
        return ''.join(hex_bytes)

    def insert_breakpoint(self, addr, size):
        size = int(size, base=10)
        addr = int(addr, base=16)
        if size == 2:
            # ARM Thumb
            old_instr = self.read_memory(addr, 2)
            breakpoint_instr = '01be'       # bkpt 0x1
            print("Inserting bkpt at:" + hex(addr))
            print("Before: " + self.read_memory(addr, 2))
            self.write_memory(addr, breakpoint_instr)
        elif size == 4:
            # ARM
            pass

    def send(self, cmd):
        while True:
            self.ser.write(cmd.encode())
            data = self.ser.read_until(b'OK\r\n')
            if data:
                print(len(data), data[-5:-1])
                return data.decode()

# https://ttotem.com/wp-content/uploads/wpforo/attachments/113/88-DIAGNOTICO-POR-COMANDOS.pdf
# AT+DEBUG=MEM|r|00000000|00004000|0
# AT+DEBUG=MEM|r|00000000|00004001|0
# AT+DEBUG=MEM|r|04000000|04000010|0
# AT+DEBUG=MEM|r|80000000|800000df|0
# AT+DEBUG=MEM|r|80000000|e0000000|0
# AT+DEBUG=MEM|r|ffffff00|ffffffdf|0
# AT+DEBUG=MEM|r|ef000000|ef0000df|0
# AT+DEBUG=MEM|r|40000000|41000000|0
# AT+DEBUG=MEM|r|fffffffc|ffffffff|0
# AT+DEBUG=MEM|r|40080000|40081000|0


# AT+DEBUG=MEM|w|0x47c00076|00000000|4|01be
# AT+DEBUG=MEM|w|0x4061ba58|00000000|4|01be
# AT+DEBUG=MEM|w|0x4159a3e8|00000000|4|01be
# AT+DEBUG=MEM|w|0x4061cf20|00000000|4|01be
# AT+DEBUG=MEM|w|0x4061c7c0|00000000|4|01be

# AT+DEBUG=MEM|w|0x40687d00|00000000|4|01be
# AT+DEBUG=MEM|w|0x40f40b56|00000000|4|01be
# AT+DEBUG=MEM|w|0x40f40b57|00000000|4|01be

# AT+DEBUG=MEM|w|4060b1f4|00000000|4|01be  AT+DEBUG=MEM|w|4060b210|00000000|4|01be
# AT+DEBUG=MEM|w|4060b1ee|00000000|4|01be
# AT+DEBUG=MEM|w|40620b40|00000000|4|01be
# AT+DEBUG=MEM|w|40f2a41a|00000000|4|01be
# AT+DEBUG=MEM|r|4060b1ee|4060b1ef
# AT+DEBUG=MEM|r|4060b1f2|4060b1fa
# AT+DEBUG=MEM|w|40f0f86a|00000000|4|01be

# AT+DEBUG=MEM|w|0x4060b1ee|00000000|4|01be
# AT+DEBUG=MEM|r|0x4161b1b0|0x4161B2B8
# AT+DEBUG=MEM|r|0x4161b238|0x4161B340
# AT+DEBUG=MEM|r|0x4161a3c8|0x4161a4c8
# AT+DEBUG=MEM|r|0x4161b868|0x4161b898
# AT+DEBUG=MEM|w|0x4159a3e4|00000000|16|800008f1700020e1
# AT+DEBUG=MEM|w|0x4159a3e4|00000000|4|01be
# AT+DEBUG=MEM|w|0x4159a3e4|00000000|8|700020e1

# AT+DEBUG=MEM|w|0x41599b2c|00000000|4|01be
# AT+DEBUG=MEM|w|0x415988b8|00000000|4|01be
# AT+DEBUG=MEM|w|0x415988ba|00000000|4|01be
# AT+DEBUG=MEM|w|0x40f0dba4|00000000|4|01be

# AT+DEBUG=MEM|w|40f0cd6e|00000000|4|01be

# AT+DEBUG=MEM|w|0x41599ae4 |00000000|4|01be


# AT+DEBUG=MEM|w|0x41598d14|00000000|8|700020e1
# AT+DEBUG=MEM|w|0x4159a0dc|00000000|4|01be

# AT+DEBUG=MEM|w|0x40aad8dc|00000000|4|01be
