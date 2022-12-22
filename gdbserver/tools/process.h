#define PACKET_BUF_SIZE 0x8000
#define POLL_BUFF_SIZE 0x4096

int init_socket(const char*);
void read_packet(void);