import struct
from dataclasses import dataclass
from ctypes import c_uint32

MEMORY = "MEM".encode()
REGISTERS = "REG".encode()
READ = "r".encode()
WRITE = "w".encode()
SEPARATOR = "|".encode()

@dataclass
class DebugCommand:
    command_type: str
    op: str
    memory_start: c_uint32
    memory_end: str

    def __init__(self, command_type, op, mem_start=0, mem_end=0):
        self.memory_start = mem_start
        self.op = op
        self.memory_end = mem_end
        self.command_type = command_type

    def send(self):
        # Unfortunately we need to pass ascii chars, otherwise the baseband does throw away the non ascii bytes.
        send_format = "3sccc8sc8s"
        bytes_to_send = struct.pack(send_format,
                                    self.command_type,
                                    SEPARATOR,
                                    self.op,
                                    SEPARATOR,
                                    '{:08x}'.format(self.memory_start).encode(),
                                    SEPARATOR,
                                    '{:08x}'.format(self.memory_end).encode())
        return bytes_to_send
