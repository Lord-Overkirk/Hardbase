#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>
#include<fcntl.h>
#include<errno.h>

#include "gdbserver.h"
#include "../tools/process.h"
#include "../tools/modem_ioctl.h"


char reply_buff[0x20000];

void process_query_packet(char* payload) {
    const char *name;
    char *args;
    
    args = strchr(payload, ':');
    if (args) {
        *args++ = '\0';
    }
    name = payload;
    if (!strcmp(name, "Supported")) {
        // write_packet("PacketSize=8000;qXfer:features:read+;qXfer:auxv:read+;qXfer:exec-file:read+;multiprocess+");
        write_packet("PacketSize=8000;qXfer:features:read+");
    }
    else if (!strcmp(name, "Xfer")) {
        write_packet(FEATURE_STR);
    }
    else if (!strcmp(name, "TStatus")) {
        write_packet("");
    }
    else if (!strcmp(name, "fThreadInfo")) {
        write_packet("m1");
    }
    else if (!strcmp(name, "sThreadInfo")) {
        write_packet("1");
    }
    else if (!strcmp(name, "Attached")) {
        write_packet("0");
    }
    else if (!strcmp(name, "C")) {
        write_packet("0");
    }
}

void process_v(char* payload) {
    const char *name;
    char *args;
    
    args = strchr(payload, ';');
    if (args) {
        *args++ = '\0';
    }
    name = payload;
    if (!strcmp(name, "MustReplyEmpty")) {
        write_packet("");
    }
}

void read_command() {
    char* buff = get_in();
    int buff_len = get_in_end();
    char* content = memchr(buff, '#', buff_len);
    int content_end = content - buff;
    assert(buff[0] == '$');
    char request = buff[1];
    char *payload = (char *)&buff[2];
    buff[content_end] = '\0';

    char checksum = 0;
    for (int i = 1; i < content_end; i++) {
        checksum += buff[i];
    }
    assert(checksum == (hex(buff[content_end + 1]) << 4 | hex(buff[content_end + 2])));
    printf("Command: %c with payload %s\n", request, payload);
    switch (request) {
    case 'q':
        process_query_packet(payload);
        break;
    case 'v':
        process_v(payload);
        break;
    case 'H':
        write_packet("OK");
        break;
    case 'g':
        printf("writing g\n");
        write_packet("xxxxxxxx00000001xxxxxxxx00000002"\
                      "xxxxxxxx00000003xxxxxxxx00000004"\
                      "xxxxxxxx00000005xxxxxxxx00000006"\
                      "xxxxxxxx00000007xxxxxxxx00000008");
        break;
    case 'm': {
        size_t start_addr, len;
        sscanf(payload, "%zx,%zx", &start_addr, &len);

        memset(&reply_buff, 'a', len);
        reply_buff[len] = '\0';
        write_packet(reply_buff);
        break;
    }
    case 'p': {
        int i = strtol(payload, NULL, 16);
        if (i > 15) {
            write_packet("xxxxxxxx");
        }
        break;
    }
    case '?':
        write_packet("S00");
        break;
    default:
        break;
    }
    pktbuf_in_erase(content_end + 3);
}

void read_ipc(int fd) {
    u_int32_t buffsize = 65536;
    char buff[buffsize];
    memset(buff, 0, buffsize);

    read(fd, buff, buffsize);
    struct sipc_fmt_hdr* hdr = (struct sipc_fmt_hdr*) buff;

    printf("Buff: %d\n", hdr->len);
    printf("Buff: %x\n", buff);
    printf("Buff: %s\n", buff);

    for (int i = 0; i < buffsize; i++) {
        if (buff[i] != 0)
            printf("Buffi: %d: %c %x\n", i, buff[i], buff[i]);
    }
}

void get_request() {
    for (;;) {
        printf("Reading...\n");
        read_packet();
        read_command();
        write_flush();
    
        int boot_fd = open("/dev/umts_boot0", O_RDWR);
        int ipc_fd = open("/dev/umts_ipc0", O_RDWR);

        if (ipc_fd == -1) {
            printf("Error Number % d\n", errno);
            perror("boot0");
            exit(-1);
        }

        ioctl(ipc_fd, IOCTL_MODEM_OFF, 0);
        int res = ioctl(ipc_fd, IOCTL_MODEM_STATUS, 0);
        switch (res) {
        case STATE_OFFLINE:
            printf("Modem offline\n");
            break;
        case STATE_BOOTING:
            printf("Modem booting\n");
            break;
        case STATE_ONLINE:
            printf("Modem online\n");
            break;
        default:
            break;
        }

        read_ipc(ipc_fd);

        close(ipc_fd);
    }
}

int main(int argc, char const *argv[]) {

    if (argc < 2) {
        printf("Usage : gdbserver :1234\n");
        exit(-1);
    }

    const char* port_str = argv[1];

    init_socket(port_str);
    get_request();
    return 0;
}
