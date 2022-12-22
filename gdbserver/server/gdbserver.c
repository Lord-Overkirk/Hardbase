#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../tools/process.h"

void get_request() {
    for (;;) {
        sleep(1);
        printf("Reading...\n");
        read_packet();
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
