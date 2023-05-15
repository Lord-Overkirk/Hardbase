target remote :1337
# set remote memory-read-packet-size fixed
# set remote memory-read-packet-size 0x90000
# show remote memory-read-packet-size

b *0x40620aec
c

# Memory regions
# dump binary memory res1.bin 0x04800000 0x04804000
# dump binary memory res2.bin 0x04000000 0x04020000
dump binary memory res4.bin 0x40000000 0x48000000