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


#include "process.h"

int socket_fd;

struct packet_buf {
    uint8_t buf[PACKET_BUF_SIZE];
    int end;
} in, out;

void packet_buf_fill(struct packet_buf* pkt, uint8_t* buffer, size_t len) {
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
    uint8_t buffer[POLL_BUFF_SIZE];

    printf("Before poll\n");
    poll_incoming();
    printf("After poll\n");

    size_t n_read = read(socket_fd, buffer, sizeof (buffer));
    if (n_read < 0) {
        printf("Connection closed");
        exit(0);
    }
    printf("THe buff %s", buffer);
    packet_buf_fill(&in, buffer, n_read);
}

void read_packet() {
    printf("Before read\n");
    read_data_once();
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

    printf("Port num %d\n", htons(port));

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


    printf("Socket created\n");
    socket_fd = accept(sock_fd, NULL, NULL);
    printf("Socket2 created\n");
    if (socket_fd < 0) {
        perror("Error: accept()");
        exit(-1);
    }

    opts = setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &level, sizeof(level));
    opts = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &level, sizeof(level));
    close(sock_fd);

    return 0;
}