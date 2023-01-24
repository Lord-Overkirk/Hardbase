import argparse
import os
import sys
import socket
from ATDebugger import ATDebugger

INTERRUPT_CHAR = '\x03'
ARCH_STR = 'l<target version=\"1.0\"><architecture>arm</architecture></target>'


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
        except ValueError:
            query_cmd = packet_str

        match query_cmd:
            case 'Supported':
                self.write_packet('PacketSize=8000;qXfer:features:read+')
            case 'Xfer':
                self.write_packet(ARCH_STR)
            case 'TStatus':
                self.write_packet('')
            case 'fThreadInfo':
                self.write_packet('m1')
            case 'sThreadInfo':
                self.write_packet('1')
            case 'Attached':
                self.write_packet('0')
            case 'C':
                self.write_packet('0')
            case _:
                print("Should handle query case {0}".format(query_cmd))

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

        match cmd_type:
            case 'q':
                self.handle_query_packet(payload_raw)
            case 'v':
                if payload_raw == "MustReplyEmpty":
                    self.write_packet("")
                    pass
            case 'H':
                self.write_packet("OK")
            case '?':
                self.write_packet('S00')
            case 'g':
                # Registers
                registers = self.at.get_registers()
                rs = registers[:-8]
                csrp = registers[-8:]

                fake_regs = "xxxxxxxx00000001xxxxxxxx00000002"\
                            "xxxxxxxx00000003xxxxxxxx00000004"\
                            "xxxxxxxx00000005xxxxxxxx00000006"\
                            "xxxxxxxx00000007xxxxxxxx00000008"
                print(rs, csrp)
                print(fake_regs)
                self.write_packet(rs)
            case 'm':
                addr, size = payload_raw.split(',')
                addr = int(addr, 16)
                size = int(size, 16)

                fake_mem = 0xdeadbeef
                self.write_packet(str(fake_mem))
            case 'p':
                print("hier")
                registers = self.at.get_registers()
                rs = registers[:-8]
                csrp = registers[-8:]
                if (int(payload_raw) > 15):
                    self.write_packet(csrp)
            case _:
                print("Should handle case {0}".format(cmd_type))

    def receive(self):
        raw_data = self.sock.recv(2048)
        # print(raw_data)
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
        print("Port must be a num")
        sys.exit(os.EX_DATAERR)

    return args.port


def main():
    port_num = parse_args()
    server = GdbServer(port_num)


if __name__ == "__main__":
    main()
