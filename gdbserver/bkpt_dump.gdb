target remote :1337

b *0x40620aec
c

dump binary memory res3.bin 0x40000000 0x41000000
                                      0x4303ad00
