import argparse
import os
import sys
import socket
import logging
import time

from ATDebugger import ATDebugger
import xml.etree.ElementTree as XML

log = logging.getLogger('gdbserver')
INTERRUPT_CHAR = '\x03'
# ARCH_STR = 'l<target version=\"1.0\"><architecture>arm</architecture></target>'
with open('target.xml', 'r') as file:
    ARCH_STR = file.read().replace('\n', '').replace('    ', '')
ARCH_STR = 'l' + ARCH_STR


class GdbServer:
    def __init__(self, port):
        if sys.platform != 'darwin':
            s = socket.socket(socket.AF_INET,
                              socket.SOCK_STREAM | socket.SOCK_CLOEXEC,
                              socket.IPPROTO_TCP)
        else:
            s = socket.socket(socket.AF_INET)

        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(('localhost', int(port)))
        s.listen(1)
        (self.sock, _) = s.accept()
        self.at = ATDebugger()
        while True:
            self.receive()

    def skip_start(self, cmd):
        for i, byte in enumerate(cmd):
            if chr(byte) == '$' or byte == INTERRUPT_CHAR:
                return cmd[i+1:]

    def checksum_ok(self, packet, checksum):
        checksum = int(checksum, 16)
        calc_checksum = sum(packet.encode()) % 256

        if checksum != calc_checksum:
            raise RuntimeError("Wrong checksum!")
    def handle_query_packet(self, packet):
        packet_str = packet
        try:
            (query_cmd, payload) = packet_str.split(':', 1)
            log.info(f"Handling query {query_cmd} with payload: {payload}")
        except ValueError:
            query_cmd = packet_str
        match query_cmd:
            case 'Supported':
                self.write_packet('PacketSize=200000;qXfer:features:read+')
            case 'Xfer':
                self.write_packet(ARCH_STR)
            case 'TStatus':
                self.write_packet('')
            case 'fThreadInfo':
                self.write_packet('m1')
            case 'sThreadInfo':
                self.write_packet('l')
            case 'Attached':
                # Existing proc
                self.write_packet('1')
            case 'C':
                self.write_packet('0')
            case _:
                log.warning(f"unhandled query case {query_cmd}")

    def write_packet(self, payload):
        payload = payload.encode()
        checksum = sum(payload) % 256

        self.sock.sendall(b'$')
        self.sock.sendall(payload)
        self.sock.sendall(b'#')
        self.sock.sendall('{:02x}'.format(checksum).encode())

    def read_command(self, cmd):
        cmd_type = cmd[0]
        payload_raw = cmd[1:]
        log.info(f"Handling {cmd_type} with payload: {payload_raw}")
        match cmd_type:
            case 'D':
                self.write_packet('OK')
                sys.exit(os.EX_OK)
            case 'c':
                self.write_packet('S05')
                # registers = self.at.get_registers()
            case 's':
                self.write_packet('OK')
                registers = self.at.get_registers()
            case 'q':
                self.handle_query_packet(payload_raw)
            case 'v':
                if payload_raw == "MustReplyEmpty":
                    self.write_packet('')
                if payload_raw == "Cont?":
                    self.write_packet('')
            case 'H':
                self.write_packet("OK")
            case '?':
                self.write_packet('S00')
            case 'g':
                log.debug(f"case {cmd_type} {payload_raw}")
                # Registers
                registers = self.at.get_registers()
                rs = registers[:-8]
                csrp = registers[-8:]
                self.write_packet(rs)
            case 'm':
                addr, size = payload_raw.split(',')
                addr = int(addr, 16)
                size = int(size, 16)
                log.debug(f"case {cmd_type} {hex(addr)} {hex(size)}")
                # print("mem", hex(addr), size)
                raw_bytes = self.at.read_memory(addr, size)
                self.write_packet(raw_bytes)
            case 'p':
                registers = self.at.get_registers()
                rs = registers[:-8]

                # cprs = "".join(reversed([cprs[i:i+2] for i in range(0, len(cprs), 2)]))
                if (int(payload_raw, 16) == 25):
                    cpsr = registers[-8:]
                    self.write_packet(cpsr)
            case 'P':
                print("TODO: setting registers")
            case 'z' | 'Z':
                b_type, addr, kind = payload_raw.split(',')
                self.at.insert_breakpoint(addr, kind)
                self.write_packet("OK")
                
                # Used for reaching the bkpt, could be done better through singalling from the custom
                # prefetch abort handler. But since we can't really 'halt' the bp, successive memory dumps
                # are also influenced by the still running bp.
                time.sleep(1)
            case _:
                print("Should handle case {0}".format(cmd_type), payload_raw)

    def receive(self):
        raw_data = self.sock.recv(2048)
        if raw_data == b'':
            raise RuntimeError("Connection broken")
        skipped = self.skip_start(raw_data)
        if skipped is not None:
            packet_str = skipped.decode()
            (payload, checksum) = packet_str.split('#', 1)
            self.checksum_ok(payload, checksum)
            # Ack
            self.sock.sendall('+'.encode())
            self.read_command(payload)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.prog = 'gdbserver'
    parser.add_argument("-p", "--port", help='Port number', required=True)
    args = parser.parse_args()

    if not args.port.isnumeric():
        log.error("Port must be a num")
        sys.exit(os.EX_DATAERR)

    return args.port


def main():
    log.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(levelname)s %(asctime)s - (%(filename)s:%(lineno)s) %(message)s', "[%H:%M:%S:%m]")
    ch.setFormatter(formatter)
    log.addHandler(ch)
    port_num = parse_args()
    server = GdbServer(port_num)
    log.info(f"Stared gdbserver on port {port_num}")


if __name__ == "__main__":
    main()
