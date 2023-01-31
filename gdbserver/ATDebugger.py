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
        self.serial_thread = None
        # Thread safe queues for receiving and transmitting.
        self.input_queue = queue.Queue(3)
        self.output_queue = queue.Queue(3)
        try:
            self.ser = serial.Serial(port=TTY,
                                     baudrate=115200,
                                     bytesize=8,
                                     parity=serial.PARITY_NONE,
                                     stopbits=1,
                                     timeout=1,
                                     write_timeout=1,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
        except serial.SerialException:
            print("Error: No modem running on port" + TTY)
            exit()
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        self.start()

    # def start_reader(self):
    #     self.reader_alive = True
    #     self.reader_thread = threading.Thread(target=self.reader, name='read')
    #     self.reader_thread.daemon = True

    # def start_writer(self):
    #     self.writer_alive = True
    #     self.writer_thread = threading.Thread(target=self.writer, name='write')
    #     self.writer_thread.daemon = True

    # def stop_reader(self):
    #     self.output_queue.join()
    #     self.reader_alive = False
    #     if hasattr(self.ser, 'cancel_read'):
    #         self.ser.cancel_read()
    #     self.reader_thread.join()

    # def stop_writer(self):
    #     self.input_queue.join()
    #     self.writer_alive = False
    #     self.writer_thread.join()

    def stop_serial(self):
        self.input_queue.join()
        self.output_queue.join()
        self.writer_alive = False
        # if hasattr(self.ser, 'cancel_read'):
        self.ser.cancel_read()
        self.ser.cancel_write()
        self.serial_thread.join()

    def start(self):
        # self.start_reader()
        # self.start_writer()
        # self.writer_thread.start()
        # self.reader_thread.start()
        self.writer_alive = True
        self.serial_thread = threading.Thread(target=self.write, daemon=True, name='serial')
        self.serial_thread.start()

    def stop(self):
        self.stop_serial()
        self.ser.close()
        # self.stop_writer()
        # self.stop_reader()


    # def reader(self):
    #     try:
    #         while self.reader_alive and self.writer_alive:
    #             data = self.ser.read_until(b'OK\r\n')
    #             if data:
    #                 self.output_queue.put(data)
    #     except serial.SerialException:
    #         self.reader_alive = False
    #         raise

    # def get_response(self):
    #     """Read AT response from the output queue and
    #     signal that an item from the queue is read."""
    #     resp = self.output_queue.get()
    #     self.output_queue.task_done()
    #     return resp.decode()

    # def writer(self):
    #     """Writer thread that reads from the input queue.
    #     If an item is present in the queue, it is send over the
    #     serial interface."""
    #     while self.writer_alive:
    #         try:
    #             command = self.input_queue.get(timeout=1)
    #             self.ser.write(command.encode())
    #             self.input_queue.task_done()
    #         except queue.Empty:
    #             # Only happens on stop.
    #             pass

    # def send(self, command):
    #     self.input_queue.put(command)

    # def get_registers(self):
    #     cmd = DebugCommand.DebugCommand(DebugCommand.REGISTERS, DebugCommand.READ)
    #     cmd_str = cmd.build()
    #     self.send(cmd_str)
    #     raw_response = self.get_response()
    #     reg_list = raw_response.splitlines()[2:-2]
    #     register_values = [reg.split(' ')[1].removeprefix('0x') for reg in reg_list]
    #     # Convert endianess
    #     r = [socket.htonl(int(reg, 16)) for reg in register_values]
    #     regs = ['{:08x}'.format(reg) for reg in r]
    #     return ''.join(regs)

    # def get_memory(self, addr, size):
    #     time.sleep(1)
    #     cmd = DebugCommand.DebugCommand(DebugCommand.MEMORY, DebugCommand.READ)
    #     cmd.memory_start = addr
    #     cmd.memory_end = addr + size
    #     cmd_str = cmd.build()
    #     self.send(cmd_str)
    #     raw_response = self.get_response()
    #     hex_bytes = raw_response.splitlines()[2].split()
    #     return ''.join(hex_bytes)

    def write(self):
        try:
            while self.writer_alive:
                cmd = self.input_queue.get(timeout=1)
                self.ser.write(cmd.encode())
                self.input_queue.task_done()
                data = self.ser.read_until(b'\r\nOK')
                if data:
                    self.output_queue.put(data)
                print(self.writer_alive)
        except:
            # print("error")
            pass

    def handler(self, cmd):
        self.input_queue.put(cmd)
        resp = self.output_queue.get()
        self.output_queue.task_done()
        print(resp.decode())

at = ATDebugger()
at.handler("AT+DEBUG=REG|r\r\n")
at.handler("AT+DEBUG=REG|r\r\n")
at.handler("AT+DEBUG=REG|r\r\n")
at.handler("AT+DEBUG=REG|r\r\n")
at.handler("AT+DEBUG=REG|r\r\n")
at.handler("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
at.handler("AT+DEBUG=REG|r\r\n")
at.handler("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
at.stop()

# a = at.get_memory(0x47c001ae, 4)
# # print(a)
# a = at.get_memory(0x47c001ae, 4)
# # print(a)
# a = at.get_memory(0x47c001ae, 4)
# # print(a)
# a = at.get_memory(0x47c001ae, 4)
# # print(a)
# #     # s = at.send("AT+DEBUG=REG|r\r\n")
# #     # at.get_response()
# #     # s = at.send("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
# #     # at.get_response()
# #     # s = at.send("AT+CHNSELCT\r\n")
# #     # at.get_response()

# https://ttotem.com/wp-content/uploads/wpforo/attachments/113/88-DIAGNOTICO-POR-COMANDOS.pdf