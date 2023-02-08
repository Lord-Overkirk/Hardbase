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
                                     timeout=1,
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
        r = self.send(cmd_str)
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
            self.write_memory(addr, breakpoint_instr)
            print(self.read_memory(addr, 2))
        elif size == 4:
            # ARM
            pass

    def send(self, cmd):
        while True:
            self.ser.write(cmd.encode())
            data = self.ser.read_until(b'OK\r\n')
            if data:
                return data.decode()

# https://ttotem.com/wp-content/uploads/wpforo/attachments/113/88-DIAGNOTICO-POR-COMANDOS.pdf

