#include <stdio.h>
#include <stdlib.h>

#include "../include/exception.h"
#include "../include/daemon.h"

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "%s", "{ERROR}: Not enough arguments\n"
                              "Use this template: ./daemon [ip] [port]\n");
        exit(EC_NOT_ENOUGH_ARGS);
    }

    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);
    run(ip, port);

    return 0;
}