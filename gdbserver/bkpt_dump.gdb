target remote :1337
# set remote memory-read-packet-size fixed
# set remote memory-read-packet-size 0x90000
# show remote memory-read-packet-size

b *0x40620aec
c

dump binary memory res3.bin 0x40000000 0x41000000
                                    #   0x4303ad00
