import struct
from dataclasses import dataclass
from ctypes import c_uint32

MEMORY = "MEM".encode()
REGISTERS = "REG".encode()

@dataclass
class DebugCommand:
    command_type: str
    memory_start: c_uint32
    memory_end: c_uint32

    def __init__(self, command_type, mem_start=0, mem_end=0):
        self.memory_start = mem_start
        self.memory_end = mem_end
        self.command_type = command_type

    def send(self):
        # Unfortunately we need to pass ascii chars, otherwise the baseband does throw away the non ascii bytes.
        send_format = "3sII"

        bytes_to_send = struct.pack(send_format, self.command_type, self.memory_start, self.memory_end)
        return bytes_to_send
