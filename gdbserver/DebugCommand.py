import struct
from dataclasses import dataclass
from ctypes import c_uint32

DEBUG_PREFIX = 'AT+DEBUG='
MEMORY = 'MEM'
REGISTERS = 'REG'
READ = 'r'
WRITE = 'w'
SEPARATOR = '|'
CRLF = '\r\n'

@dataclass
class DebugCommand:
    command_type: str
    op: str
    memory_start: c_uint32
    memory_end: c_uint32
    payload_size: c_uint32
    payload: str

    def __init__(self, command_type, op, mem_start=0, mem_end=0, payload=None):
        self.command_type = command_type
        self.op = op
        self.memory_start = mem_start
        self.memory_end = mem_end
        # if self.op == READ:
        # else:
        self.payload = payload
        if self.payload is not None:
            self.payload_size = int(len(payload)/2)
        else:
            self.payload_size = 0

    def build(self):
        """Build the AT command."""
        # Unfortunately we need to pass ascii chars, otherwise the baseband does throw away the non ascii bytes.
        # send_format = "3sccc8sc8s"
        # bytes_to_send = struct.pack(send_format,
        #                             self.command_type,
        #                             SEPARATOR,
        #                             self.op,
        #                             SEPARATOR,
        #                             '{:08x}'.format(self.memory_start).encode(),
        #                             SEPARATOR,
        #                             '{:08x}'.format(self.memory_end).encode())
        string_to_send = DEBUG_PREFIX + self.command_type + SEPARATOR + \
                         self.op + SEPARATOR + \
                         '{:08x}'.format(self.memory_start) + SEPARATOR + \
                         '{:08x}'.format(self.memory_end) + SEPARATOR
        # if self.payload is not None:
        string_to_send += str(self.payload_size)
        if self.payload_size != 0:
            string_to_send += SEPARATOR + self.payload
        string_to_send += CRLF
        return string_to_send
    
    def build_mem_write(self):
        pass