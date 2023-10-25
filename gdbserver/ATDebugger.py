import serial
import DebugCommand
import socket
import sys
import logging
import datetime
import time

log = logging.getLogger('gdbserver')

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
                                     timeout=0.8, # with set remote memory-read-packet-size 0x6000 in gdb
                                     write_timeout=None,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
        except serial.SerialException:
            log.error(f"No modem running on port {TTY}")
            exit()
        # self.ser.cancel_read()
        # self.ser.cancel_write()
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()


    def get_registers(self):
        """Read the register contents through AT commands."""
        cmd = DebugCommand.DebugCommand(DebugCommand.REGISTERS, DebugCommand.READ)
        cmd_str = cmd.build()
        raw_response = self.send(cmd_str, True)
        log.info(raw_response)
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
        if r[-4:] != b'OK\r\n':
            raise WriteFailedException


    def read_memory(self, addr, size):
        """Read memory of specified size via AT commands."""
        cmd = DebugCommand.DebugCommand(DebugCommand.MEMORY, DebugCommand.READ)
        cmd.memory_start = addr
        cmd.memory_end = addr + size
        cmd_str = cmd.build()
        s = datetime.datetime.now()
        raw_response = self.send(cmd_str)
        e = datetime.datetime.now()
        print(e-s, self.ser.in_waiting, len(raw_response))
        while (raw_response[-4:] != b'OK\r\n' or raw_response[34:38] != b'\r\n\r\n'):
            time.sleep(1.0)
            print('resend', self.ser.in_waiting)
            self.ser.read_all()
            self.ser.flush()
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            # self.ser.timeout = 2
            raw_response = self.send(cmd_str)
            print(raw_response[0:50], raw_response[-4:], self.ser.in_waiting)

        self.ser.timeout = 0.4

        (_, response) = raw_response.split(b'\r\n\r\n', 1)
        payload_parts = response.split(b'\r\nOK\r\n')
        print(len(response), len(payload_parts))
        payload = b''.join(payload_parts[:-1])
        return(str(payload.hex()))

    def insert_breakpoint(self, addr, kind):
        addr = int(addr, base=16)
        old_instr = self.read_memory(addr, 2)
        breakpoint_instr = '01be'       # bkpt 0x1
        log.debug("Inserting bkpt at:" + hex(addr))
        self.write_memory(addr, breakpoint_instr)

    def send(self, cmd, reg=False):
        log.debug(cmd)
        while True:
            self.ser.write(cmd.encode())
            data = self.ser.read_until(expected=b'OK\r\n')
            if data:
                if reg:
                    return data.decode()
                while self.ser.in_waiting != 0:
                    data += self.ser.read_until(expected=b'OK\r\n')
                return data
