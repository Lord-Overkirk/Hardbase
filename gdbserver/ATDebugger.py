import queue
import threading
import serial, time
import DebugCommand
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
        self.input_queue = queue.Queue()
        self.output_queue = queue.Queue()
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
            print("Error: No modem running on port" + TTY)
            exit()
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        self.start()

    def start_reader(self):
        self.reader_alive = True
        self.reader_thread = threading.Thread(target=self.reader, name='read')
        self.reader_thread.daemon = True

    def start_writer(self):
        self.writer_alive = True
        self.writer_thread = threading.Thread(target=self.writer, name='write')
        self.writer_thread.daemon = True

    def stop_reader(self):
        self.output_queue.join()
        self.reader_alive = False
        if hasattr(self.ser, 'cancel_read'):
            self.ser.cancel_read()
        print(self.output_queue.empty(), self.reader_alive, self.writer_alive)
        self.reader_thread.join()

    def stop_writer(self):
        # print("send", self.input_queue.)
        # while not self.input_queue.empty():
        #     print("wait")
        #     time.sleep(0.1)
        #     pass
        self.input_queue.join()
        self.writer_alive = False
        self.writer_thread.join()

    def start(self):
        print("Start")
        self.start_reader()
        self.start_writer()
        self.writer_thread.start()
        self.reader_thread.start()

    def stop(self):
        self.stop_writer()
        print("writer closed")
        self.stop_reader()
        print("Stop")
        self.ser.close()


    def reader(self):
        try:
            while self.reader_alive and self.writer_alive:
                print("hier1", self.ser.in_waiting)
                # if self.ser.in_waiting > 0:
                data = self.ser.read_until(b'OK\r\n')
                print("hier2")
                # data = self.ser.read(self.ser.in_waiting or 1)
                if data:
                    print("hier3")
                    self.output_queue.put(data)
                print("hier4")
        except:
            print("oops")


    def get_response(self):
        print(self.output_queue.empty())
        resp = self.output_queue.get()
        print(resp.decode())
        self.output_queue.task_done()

    def writer(self):
        while self.writer_alive:
            # if not self.input_queue.empty():
            try:
                command = self.input_queue.get(timeout=1)
                    # print("a", command)
                self.ser.write(command.encode())
                self.input_queue.task_done()
            except:
                pass


    def send(self, command):
        self.input_queue.put(command)
        # with self.send_cond:
        #     while self.input_buffer != "":
        #         self.send_cond.wait()
        
        # has_send = False
        # while not has_send:
        #     self.input_lock.acquire()
        #     print("send lock aq")
        #     if self.input_buffer == "":
        #         print("cmd send")
        #         has_send = True
        #         self.input_buffer = command
        #         self.input_lock.release()
        #         return has_send
        #     self.input_lock.release()
        #     print("send lock release")

    # def write_command(self, command, echo=False):
    #     self.ser.write("AT\r\n".encode())
    #     while self.ser.in_waiting:
    #         data = self.ser.read(self.ser.in_waiting or 1)
    #         print("a", data.decode())
    #     # print(command)
    #     # start_command = "AT+DEBUG="
    #     # if echo:
    #     #     print("> {0}".format(start_command))
    #     # for letter in start_command:
    #     #     self.ser.write(letter.encode())
    #     #     self.ser.read(1)

    #     # for byte in command:
    #     #     self.ser.write(byte.to_bytes(1, 'big'))
    #     #     self.ser.read(1)

    #     # self.ser.write(('\r\n').encode())
    #     # self.ser.read(2)

    # def read_command(self):
    #     raw_data = b''
    #     # while raw_data[-2:] != b'\r\n':
    #     while self.ser.inWaiting() > 0:
    #         raw_data += self.ser.read(self.ser.inWaiting())
    #         print(raw_data)
    #     text = raw_data.decode()
    #     # print(text)
    #     return text

    # def get_registers(self):
    #     cmd = DebugCommand.DebugCommand(DebugCommand.REGISTERS, DebugCommand.READ)
    #     b = cmd.send()
    #     self.write_command(b, False)
    #     raw_response = self.read_command()
    #     raw_regs = raw_response.strip().split('\r\n')[:-2]
    #     register_values = [reg.split(' ')[1].removeprefix('0x') for reg in raw_regs]
    #     r = [socket.htonl(int(reg, 16)) for reg in register_values]
    #     regs = ['{:08x}'.format(reg) for reg in r]
    #     return ''.join(regs)
    
    # def get_memory(self, addr, size):
    #     cmd = DebugCommand.DebugCommand(DebugCommand.MEMORY, DebugCommand.READ, addr, addr+size)
    #     b = cmd.send()
    #     self.write_command(b, False)
    #     raw_response = self.read_command()
    #     print("memread", raw_response)

    # # def set_register(self, regnum):
    # #     cmd = DebugCommand.DebugCommand(DebugCommand.REGISTERS)


while True:
    at = ATDebugger()
    # s = at.send("AT\r\n")
    s = at.send("AT+DEBUG=REG|r\r\n")
    at.get_response()
    s = at.send("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
    at.get_response()
    s = at.send("AT+CHNSELCT\r\n")
    at.get_response()
    # time.sleep(0.2)
    # time.sleep(1)


    at.stop()

# at.ser.flushInput()
# at.ser.flushOutput()

# a = DebugCommand.DebugCommand(DebugCommand.REGISTERS, DebugCommand.READ)
# b = a.send()
# # print(b)
# at.write_command(b, True)
# at.read_command()

# at.ser.close()

# https://ttotem.com/wp-content/uploads/wpforo/attachments/113/88-DIAGNOTICO-POR-COMANDOS.pdf