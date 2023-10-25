#!/usr/bin/env python3

import json
import pexpect
import time

MEMORY_MAP = [
    (0x04000000, 0x04020000),
    (0x04800000, 0x04804000),
    (0x40000000, 0x48000000),
]

def store_registers(regs):
    with open("registers.json", "w") as json_file:
        json.dump(regs, json_file)

def parse_registers(regs):
    first = regs[0]
    while first == '\r' or first == '\n':
        regs = regs.strip(first)
        first = regs[0]

    register_dict = {}
    reg_list = regs.split('\r\n')
    for reg_line in reg_list:
        (register, value) = reg_line.split(':', 1)
        value = value.strip()
        register_dict.update({register: value})
    return register_dict

def special_send(command):
    child = pexpect.spawn('python -m serial.tools.miniterm --raw')
    child.timeout = 2
    child.sendline('1')
    try:
        child.read(99999999)
    except pexpect.exceptions.TIMEOUT:
        child.sendline('\x1d') # ctrl+[
        child.expect(pexpect.EOF)
        child.close()

    child = pexpect.spawn('python -m serial.tools.miniterm')
    child.sendline('1')
    child.send(command)
    child.timeout = 2
    parts = b''
    try:
        while True:
            child.expect_exact(b'asjflskj')
            # child.read(9999999)
            parts += child.before
            print(parts[-20:], len(child.before))
            parts += b'\r\nOK\r'
    except pexpect.exceptions.TIMEOUT:
        parts += child.before
        child.sendline('\x1d') # ctrl+[
        child.expect(pexpect.EOF)
        child.close()

def send_command(command, echo=False):
    child = pexpect.spawn('python -m serial.tools.miniterm')
    child.sendline('1')
    if echo:
        print(f'Sending: {command.decode()}')
    child.send(command)
    child.expect_exact('\r\nOK\r')
    child.sendline('\x1d') # ctrl+[
    payload = child.before
    child.expect(pexpect.EOF)
    child.close()
    return payload

def strip_raw_dump(input_file):
    with open(input_file, 'rb') as f:
        current = f.read()
        stripped = current[44:-6]
        print("From file: ", current[0:44], len(current))
        return stripped

def get_raw_mem_range(start, end, is_chunk=False, special=False):
    start_str = '0x' + str(hex(start)[2:]).zfill(8)
    end_str = '0x' + str(hex(end)[2:]).zfill(8)
    command = f'AT+DEBUG=MEM|r|{start_str}|{end_str}|0\r\n'.encode()

    if special:
        special_send(command)
        return strip_raw_dump('temp_dump.txt')

    send_command(command)
    while resend_check():
        send_command(command)


    res = strip_raw_dump('temp_dump.txt')
    return res

def resend_check():
    with open('temp_dump.txt', 'rb') as f:
        to_parse = f.read()
        safety_check = to_parse[0:44]
        if safety_check[-4:] != b'\r\n\r\n':
            print('Resend needed')
            return True

def get_raw_mem():
    for mem_region in MEMORY_MAP:
        start = mem_region[0]
        end = mem_region[1]

        chunk_size = 0x100000
        if end - start >= chunk_size:
            raw_data = []
            for chunk in range(start, end, chunk_size):
                raw_bytes = get_raw_mem_range(chunk, chunk+chunk_size, is_chunk=True)
                if len(raw_bytes) != chunk_size:
                    print('Err: Read smaller than chunksize')
                    raw_bytes = get_raw_mem_range(chunk, chunk+chunk_size, is_chunk=True, special=True)
                    raw_data.append(raw_bytes)
                else:
                    raw_data.append(raw_bytes)
                    
            full_range_dump = b''.join(raw_data)
            output_file_name = f'{str(hex(start))}-{str(hex(end))}.bin'
            with open(output_file_name, 'wb') as output_file:
                output_file.write(full_range_dump)

        else:
            raw_bytes = get_raw_mem_range(start, end)
            output_file_name = f'{str(hex(start))}-{str(hex(end))}.bin'
            with open(output_file_name, 'wb') as output_file:
                output_file.write(raw_bytes)

def place_bkpt(bkpt):
    command = f'AT+DEBUG=MEM|w|{bkpt}|00000000|4|01be\r\n'.encode()
    send_command(command, echo=True)

def get_registers():
    command = f'AT+DEBUG=REG\r\n'.encode()
    registers = send_command(command)
    return (registers.decode().strip()).split('AT+DEBUG=REG')[1]

def main():
    # bkpt = "0x40cab808" # GSM_CC msgReveiveMbx
    # bkpt = "0x40cab7fc" # GSM_CC msgReveiveMbx
    # bkpt = "0x414c221c" # Background
    # bkpt = "0x4145394a" #LTE_RLC msgReceiveMbx
    bkpt = "0x40c842ec" #GSM_SM msgReceiveMbx
    place_bkpt(bkpt)
    registers = get_registers()
    # print(registers)
    reg_dict = parse_registers(registers.strip())
    print('continue...', end='', flush=True)
    while reg_dict.get('pc') != bkpt:
        print(reg_dict.get('pc'))
        time.sleep(0.4)
        registers = get_registers()
        print('.',end='', flush=True)
        reg_dict = parse_registers(registers)

    print(f'\n#### bkpt at {bkpt} triggered! ####\n')
    store_registers(reg_dict)
    get_raw_mem()

if __name__ == "__main__":
    main()