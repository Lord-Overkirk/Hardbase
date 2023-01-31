import threading
import serial, time
import socket

TTY = '/dev/ttyACM0'

class ATDebugger:
    def __init__(self):
        self.reader_alive = None
        self.writer_alive = None
        self.reader_thread = None
        self.writer_thread = None
        self.response_buffer = ""
        self.input_buffer = ""
        # self.response_lock = threading.Lock()
        self.ready_lock = threading.Lock()
        self.send_cond = threading.Condition()
        self.recv_cond = threading.Condition()
        self.ready = False
        try:
            # self.ser = serial.Serial(port=TTY,
            #                          baudrate=115200,
            #                          bytesize=8,
            #                          parity=serial.PARITY_NONE,
            #                          stopbits=1,
            #                          timeout=0.2,
            #                          write_timeout=0.1,
            #                          xonxoff=False,
            #                          rtscts=False,
            #                          dsrdtr=False)
            self.ser = serial.Serial(port=TTY,
                                     baudrate=9600,
                                     bytesize=8,
                                     parity=serial.PARITY_NONE,
                                     stopbits=1,
                                     timeout=0.1,
                                     write_timeout=None,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
        except serial.SerialException:
            print("Error: No modem running on port" + TTY)
            exit()

    def send_msg(self, command):
        data = b''
        a = self.ser.write(command.encode())
        while data == b'':
            time.sleep(1)
            # print(a, command)
            data = self.ser.read_until(b'OK\r\n')
            # print(data)
            # while buff[-4:] != 'OK\r\n' and data != b'':
            #     data = self.ser.read(1)
            #     buff += data.decode()
            # print(buff)
            if data:
                # print(data)
                buff = data.decode()
                print(buff)
            # else:
            #     print("niks", data)

# while True:
at = ATDebugger()
# print("start")
at.send_msg("AT\r\n")
at.send_msg("AT+CHNSELCT\r\n")
at.send_msg("AT+DEBUG=REG|r\r\n")

# time.sleep(0.2)
