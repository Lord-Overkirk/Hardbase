import serial


class ATDebugger:
    def __init__(self):
        try:
            self.ser = serial.Serial(port="/dev/ttyACM0",
                                     baudrate=115200,
                                     bytesize=8,
                                     parity=serial.PARITY_NONE,
                                     stopbits=1,
                                     timeout=None,
                                     write_timeout=None,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
        except serial.SerialException:
            print("Error: No modem running on port /dev/ttyACM0")
            exit()

    def write_command(self, command, echo=False):
        if echo:
            print("> {0}".format(command))
        for letter in command:
            self.ser.write(letter.encode())
            self.ser.read(1)

        self.ser.write(('\r\n').encode())
        self.ser.read(2)

    def read_command(self):
        if self.ser.inWaiting() > 0:
            raw_data = self.ser.read(self.ser.inWaiting())
        text = raw_data.decode()
        print(text)

# at = ATDebugger()
# at.write_command("AT", True)
# at.read_command()

# at.ser.close()
