#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <assert.h>

#include "process.h"

int socket_fd;

struct packet_buf {
    char buf[PACKET_BUF_SIZE];
    int end;
} in, out;

char* get_in() {
    return in.buf;
}

int get_in_end() {
    return in.end;
}

void packet_buf_fill(struct packet_buf* pkt, char* buffer, size_t len) {
    if (pkt->end + len >= sizeof(pkt->buf)) {
        printf("Packet buffer overflow\n");
        exit(-2);
    }

    memcpy(pkt->buf + pkt->end, buffer, len);
    pkt->end += len;
}

void packet_buf_clear(struct packet_buf* pkt) {
    memset(pkt, 0, sizeof (struct packet_buf));
    pkt->end = 0;
}

void pktbuf_erase_head(struct packet_buf *pkt, ssize_t end) {
    memmove(pkt->buf, pkt->buf + end, pkt->end - end);
    pkt->end -= end;
}

void pktbuf_in_erase(ssize_t end) {
    pktbuf_erase_head(&in, end);
}

int poll_socket(short events) {
    struct pollfd pf;
    pf.fd = socket_fd;
    pf.events = events;
    
    int pol_res = poll(&pf, 1, -1);
    if (pol_res < 0) {
        perror("Error: poll()");
        exit(-1);
    }

    return pol_res > 0;
}

int poll_incoming() {
    return poll_socket(POLLIN);
}

int poll_outgoing() {
    return poll_socket(POLLOUT);
}

void read_data_once() {
    char buffer[POLL_BUFF_SIZE];

    poll_incoming();

    size_t n_read = read(socket_fd, buffer, sizeof (buffer));
    if (n_read < 0) {
        printf("Connection closed");
        exit(0);
    }
    packet_buf_fill(&in, buffer, n_read);
}

void write_data_raw(char* data, size_t len) {
    packet_buf_fill(&out, data, len);
}

void write_flush() {
    size_t write_i = 0;

    while (write_i < (uint32_t) out.end) {
        size_t bytes_written;
        poll_outgoing();
        bytes_written = write(socket_fd, out.buf + write_i, out.end - write_i);

        if (bytes_written < 0) {
            printf("Error writing bytes: write_flush()\n");
            exit(-2);
        }
        write_i += bytes_written;
    }
    packet_buf_clear(&out);
}

void write_hex(unsigned long hex) {
    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf) - 1, "%02lx", hex);
    write_data_raw(buf, len);
}

void write_packet_bytes(char* payload, size_t n) {
    char checksum;
    size_t i;

    write_data_raw("$", 1);
    for (i = 0, checksum = 0; i < n; i++) {
        checksum += payload[i];
    }

    write_data_raw(payload, n);
    write_data_raw("#", 1);
    write_hex(checksum);
}

void write_packet(char* payload) {
    write_packet_bytes(payload, strlen(payload));
}

int skip_start() {
    ssize_t end = -1;
    for (size_t i = 0; i < (uint32_t) in.end; i++) {
        if (in.buf[i] == '$' || in.buf[i] == INTERRUPT_CHAR) {
            end = i;
            break;
        }
    }

    if (end < 0) {
        packet_buf_clear(&in);
        return 0;
    }

    pktbuf_erase_head(&in, end);
    assert(1 <= in.end);
    assert('$' == in.buf[0] || INTERRUPT_CHAR == in.buf[0]);
    return 1;
}

void read_packet() {
    while (!skip_start()) {
        read_data_once();
    }
    // Ack packet
    write_data_raw("+", 1);
    write_flush();
}

int init_socket(const char* port_str) {
    struct sockaddr_in sock_addr;
    uint level = 1;
    int port = -1;
    char* garb;

    port = strtoul(port_str + 1, &garb, 10);
    if (port == 0) {
        printf("Port num parse error. Defaulting to :1337\n");
        port = 1337;
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sock_fd < 0) {
        perror("Error: socket()");
        exit(-1);
    }

    int opts = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &level, sizeof(level));
    if (opts < 0) {
        perror("setsockopt() failed");
        exit(-1);
    }

    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    memset(&sock_addr.sin_zero, 0, 8);

    int bind_val = bind(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (bind_val < 0) {
        perror("Error: bind()");
        exit(-1);
    }

    int listen_val =  listen(sock_fd, 1);
    if (listen_val < 0) {
        perror("Error: listen()");
        exit(-1);
    }

    socket_fd = accept(sock_fd, NULL, NULL);
    if (socket_fd < 0) {
        perror("Error: accept()");
        exit(-1);
    }

    opts = setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &level, sizeof(level));
    opts = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &level, sizeof(level));
    close(sock_fd);

    return 0;
}