#!/usr/bin/env python
#
# Very simple serial terminal
#
# This file is part of pySerial. https://github.com/pyserial/pyserial
# (C)2002-2020 Chris Liechti <cliechti@gmx.net>
#
# SPDX-License-Identifier:    BSD-3-Clause

from __future__ import absolute_import

import codecs
import os
import queue
import sys
import threading

import serial
from serial.tools.list_ports import comports
from serial.tools import hexlify_codec

# pylint: disable=wrong-import-order,wrong-import-position

class Miniterm(object):
    """\
    Terminal application. Copy data from serial port to console and vice versa.
    Handle special keys from the console to show menu etc.
    """

    def __init__(self, serial_instance, echo=False, eol='crlf', filters=()):
        self.serial = serial_instance
        self.echo = echo
        self.raw = False
        self.input_encoding = 'UTF-8'
        self.output_encoding = 'UTF-8'
        self.eol = eol
        self.filters = filters
        self.alive = None
        self._reader_alive = None
        self.receiver_thread = None
        self.rx_decoder = None
        self.tx_decoder = None
        self.input_queue = queue.Queue()
        self.output_queue = queue.Queue()

    def _start_reader(self):
        """Start reader thread"""
        self._reader_alive = True
        # start serial->console thread
        self.receiver_thread = threading.Thread(target=self.reader, name='rx')
        self.receiver_thread.daemon = True
        self.receiver_thread.start()

    def _stop_reader(self):
        """Stop reader thread only, wait for clean exit of thread"""
        self._reader_alive = False
        if hasattr(self.serial, 'cancel_read'):
            self.serial.cancel_read()
        self.receiver_thread.join()

    def start(self):
        """start worker threads"""
        self.alive = True
        self._start_reader()
        # enter console->serial loop
        self.transmitter_thread = threading.Thread(target=self.writer, name='tx')
        self.transmitter_thread.daemon = True
        self.transmitter_thread.start()

    def stop(self):
        """set flag to stop worker threads"""
        self.alive = False

    def join(self, transmit_only=False):
        """wait for worker threads to terminate"""
        self.input_queue.join()
        self.transmitter_thread.join()
        if not transmit_only:
            self.output_queue.join()
            if hasattr(self.serial, 'cancel_read'):
                self.serial.cancel_read()
            self.receiver_thread.join()

    def close(self):
        self.serial.close()

    def reader(self):
        """loop and copy serial->console"""
        try:
            while self.alive and self._reader_alive:
                # read all that is there or wait for one byte
                # data = self.serial.read(self.serial.in_waiting or 1)
                data = self.serial.read_until(b'OK\r\n')
                # print("asd", data)
                if data:
                    text = data.decode()
                    # print(text)
                    self.output_queue.put(text)
                    # self.console.write(text)
        except serial.SerialException:
            self.alive = False
            self.console.cancel()
            raise       # XXX handle instead of re-raise?

    def writer(self):
        """\
        Loop and copy console->serial until self.exit_character character is
        found. When self.menu_character is found, interpret the next key
        locally.
        """
        try:
            while self.alive:
                try:
                    cmd = self.input_queue.get(timeout=1)
                    self.serial.write(cmd.encode())
                    self.input_queue.task_done()
                except queue.Empty:
                    self.alive = False
                    pass
                # self.serial.write("AT+DEBUG=REG|r\r\n".encode())
                # self.serial.write("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n".encode())
        except:
            self.alive = False
            raise

    def get_response(self):
        response = self.output_queue.get()
        print(response)
        self.output_queue.task_done()

    def get_memory(self, addr, size):
        cmd = DebugCommand.DebugCommand(DebugCommand.MEMORY, DebugCommand.READ)
        cmd.memory_start = addr
        cmd.memory_end = addr + size
        cmd_str = cmd.build()
        self.send(cmd_str)
        raw_response = self.get_response()
        hex_bytes = raw_response.splitlines()[2].split()
        return ''.join(hex_bytes)

ser = serial.Serial(port='/dev/ttyACM0',
                                     baudrate=115200,
                                     bytesize=8,
                                     parity=serial.PARITY_NONE,
                                     stopbits=1,
                                     timeout=None,
                                     write_timeout=None,
                                     xonxoff=False,
                                     rtscts=False,
                                     dsrdtr=False)
m = Miniterm(ser)
m.start()
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.get_response()
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.get_response()
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.get_response()
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.get_response()
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.input_queue.put("AT+DEBUG=REG|r\r\n")
m.input_queue.put("AT+DEBUG=MEM|r|0x40669586|0x4066958f\r\n")
m.join(False)
# m.stop()