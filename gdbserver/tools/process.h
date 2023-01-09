#define PACKET_BUF_SIZE 0x8000
#define POLL_BUFF_SIZE 0x4096

#define WR_IOCTL _IOW('a', 'a', int32_t*)
#define RD_IOCTL _IOR('a', 'b', int32_t*)


static const char INTERRUPT_CHAR = '\x03';

int init_socket(const char*);
void read_packet(void);
char* get_in(void);
int get_in_end(void);
void write_packet(char*);
void pktbuf_in_erase(ssize_t);
void write_flush(void);