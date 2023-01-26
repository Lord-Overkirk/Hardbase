import serial, time
import DebugCommand
import socket

TTY = '/dev/ttyACM0'

class ATDebugger:
    def __init__(self):
        try:
            self.ser = serial.Serial(port=TTY,
                                     baudrate=115200,
                                     bytesize=8,
                                     parity=serial.PARITY_NONE,
                                     stopbits=1,
                                     timeout=0.2,
                                     write_timeout=0.1,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
        except serial.SerialException:
            print("Error: No modem running on port" + TTY)
            exit()

    def write_command(self, command, echo=False):
        start_command = "AT+DEBUG="
        if echo:
            print("> {0}".format(start_command))
        for letter in start_command:
            self.ser.write(letter.encode())
            self.ser.read(1)

        for byte in command:
            self.ser.write(byte.to_bytes(1, 'big'))
            self.ser.read(1)

        self.ser.write(('\r\n').encode())
        self.ser.read(2)

    def read_command(self):
        raw_data = b''
        # while raw_data[-2:] != b'\r\n':
        while self.ser.inWaiting() > 0:
            raw_data += self.ser.read(self.ser.inWaiting())
            # print(raw_data)
        text = raw_data.decode()
        # print(text)
        return text

    def get_registers(self):
        cmd = DebugCommand.DebugCommand(DebugCommand.REGISTERS)
        b = cmd.send()
        self.write_command(b, False)
        raw_response = self.read_command()
        raw_regs = raw_response.strip().split('\r\n')[:-2]
        register_values = [reg.split(' ')[1].removeprefix('0x') for reg in raw_regs]
        r = [socket.htonl(int(reg, 16)) for reg in register_values]
        regs = ['{:08x}'.format(reg) for reg in r]
        return ''.join(regs)


# at = ATDebugger()
# at.ser.flushInput()
# at.ser.flushOutput()

# a = DebugCommand.DebugCommand(DebugCommand.REGISTERS)
# b = a.send()
# # print(b)
# at.write_command(b, True)
# at.read_command()

# at.ser.close()

# https://ttotem.com/wp-content/uploads/wpforo/attachments/113/88-DIAGNOTICO-POR-COMANDOS.pdf