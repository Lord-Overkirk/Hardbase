import serial, time
import DebugCommand

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
        print(text)

at = ATDebugger()
at.ser.flushInput()
at.ser.flushOutput()

a = DebugCommand.DebugCommand(DebugCommand.REGISTERS)
b = a.send()
# print(b)
at.write_command(b, True)
at.read_command()

at.ser.close()

# https://ttotem.com/wp-content/uploads/wpforo/attachments/113/88-DIAGNOTICO-POR-COMANDOS.pdf