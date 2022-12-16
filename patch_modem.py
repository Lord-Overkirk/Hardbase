#!/usr/bin/env python2
# TODO: use python3
# coding: utf8
from struct import unpack, pack
import zlib
import sys

def u32(data):
    return unpack("<I", data[:4])[0]

def p32(data):
    return pack("<I", data)


class TOC_entry:
    @staticmethod
    def parse_entry(entry):
        name = entry[:12].replace("\x00", "")
        b_off = u32(entry[12:16])
        m_off = u32(entry[16:20])
        size = u32(entry[20:24])
        crc = u32(entry[24:28])
        etype = u32(entry[28:32])

        if name == "":
            return None

        seg_data = data[b_off:b_off+size]

        return TOC_entry(name, b_off, m_off, crc, etype, size, seg_data)

    def __init__(self, name, b_off, m_off, crc, etype, size, seg_data):
        self.name = name
        self.b_off = b_off
        self.m_off = m_off
        self.size = size
        self.crc = crc
        self.etype = etype
        self.seg_data = seg_data

    def is_loadable(self):
        if self.name in ["BOOT","MAIN","INJECT"]:
            return True
        return False

    def compute_crc(self):
        if self.name not in ["BOOT","MAIN","INJECT"]:
            self.crc = 0
        else:
            self.crc = zlib.crc32(self.seg_data) & 0xFFFFFFFF

    def check_crc(self):
        if self.name not in ["BOOT","MAIN","INJECT"]:
            return True
        ccrc = zlib.crc32(self.seg_data) & 0xFFFFFFFF
        if ccrc == self.crc:
            return True
        else:
            return False

    def pack_toc_entry(self):
        out=""
        out+=self.name + "\x00" * (12-len(self.name))
        out+=p32(self.b_off)
        out+=p32(self.m_off)
        out+=p32(self.size)
        out+=p32(self.crc)
        out+=p32(self.etype)
        return out

class TOC:

    TOC_HEADER_SIZE = 0x200
    TOC_ENTRIES_OFFSET = 0x20

    def __init__(self,data):
        self.header = data[:self.TOC_ENTRIES_OFFSET]
        self.entries = []
        entries_data = data[self.TOC_ENTRIES_OFFSET:self.TOC_HEADER_SIZE]

        while len(entries_data):
            entry=entries_data[:0x20]
            entries_data = entries_data[0x20:]
            seg_entry = TOC_entry.parse_entry(entry)
            if seg_entry is None:
                break
            seg_entry.check_crc()
            self.entries.append(seg_entry)

    def get_seg_by_name(self,name):
        for seg in self.entries:
            if seg.name == name:
                return seg
        return None

    def add_segment_after_main(self, name, data, addr, etype):
        main = self.get_seg_by_name("MAIN")
        offset_in_file = main.b_off + main.size
        # align 0x20
        if offset_in_file % 0x20:
            offset_in_file += (offset_in_file % 0x20)
        seg_entry = TOC_entry(name, offset_in_file, addr, 0, etype, len(data), data)
        seg_entry.compute_crc()
        main_pos = self.entries.index(main)
        self.entries.insert(main_pos + 1, seg_entry)

    def pack_header(self):
        self.header = self.header[:0x1C] + p32(len(self.entries))
        return self.header

    def pack_entries(self):
        out=""
        out+=self.pack_header()
        for seg in self.entries:
            out+=seg.pack_toc_entry()
        # pad
        out += "\x00"  * (self.TOC_HEADER_SIZE - len(out))
        return out

    def pack_all(self):
        out=""
        out+=self.pack_entries()
        for seg in self.entries:
            if seg.is_loadable:
                # seek
                out+="\x00" * (seg.b_off - len(out))
                out+=seg.seg_data
        return out



if __name__ == "__main__":
    print(sys.argv[0], sys.argv[1], sys.argv[2], sys.argv[3])
    data = open(sys.argv[2],"rb").read()
    toc = TOC(data)
    main_seg = toc.get_seg_by_name("MAIN")

    data_to_inject = open(sys.argv[1],"rb").read()
    inject_addr = 0x47C00000
    name = "INJECT"

    # patch handler
    #main_seg.seg_data = main_seg.seg_data[:0x90] + p32(inject_addr + 0xc) + main_seg.seg_data[0x94:]  # prefetch
    #main_seg.seg_data = main_seg.seg_data[:0x94] + p32(inject_addr + 0x10) + main_seg.seg_data[0x98:] # data abort
    #main_seg.seg_data = main_seg.seg_data[:0x9C] + p32(inject_addr + 0x18) + main_seg.seg_data[0xA0:] # irq
    off = main_seg.seg_data.find('+LEDTEST\0')
    print(hex(off))
    main_seg.seg_data = main_seg.seg_data[:off] + '+DOOM\0\0\0\0' + main_seg.seg_data[off+9:]
    # print(main_seg.seg_data[:off])

    # find pointer to ledtest function handler
    pointer_off = main_seg.seg_data.find(p32(main_seg.m_off + off)) + 12
    print(hex(pointer_off), hex(pointer_off+main_seg.m_off))
    main_seg.seg_data = main_seg.seg_data[:pointer_off] + p32(inject_addr | 1) + main_seg.seg_data[pointer_off+4:]


    ptr = 0x41617a24
    main_seg.seg_data = main_seg.seg_data[ :ptr - main_seg.m_off + 1] + b'\x03' +  main_seg.seg_data[ ptr - main_seg.m_off + 2: ] 
    print(len(main_seg.seg_data))

    
    toc.add_segment_after_main(name, data_to_inject, inject_addr, 2)

    patched_data = toc.pack_all()

    # patched_data[2336020:2336028] = "asdf"
    new_version = "DOOMHACK"
    patched_data = patched_data[:2336020] + new_version + patched_data[2336020+len(new_version):]
    print(patched_data[2336020:2336028], 0x40247d94)


    open(sys.argv[3], "w").write(patched_data)



'''
Commands to inject:

setprop ctl.stop cpboot-daemon
cbd -d -tss310 -bm -mm -P ../../data/local/tmp/modem.bin

'''
