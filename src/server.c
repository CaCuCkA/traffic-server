#include <stdio.h>
#include <stdlib.h>

#include "../include/server.h"
#include "../include/exception.h"

void
create_socket(int* t_socket) {
    *t_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (*t_socket == EC_CANT_CREATE_SOCKET) {
        fprintf(stderr, "Failed to create socket\n");
        close(*t_socket);
        exit(EC_CANT_CREATE_SOCKET);
    }
}

void
bind_socket(int* t_socket, struct sockaddr t_address) {
    int error = bind(*t_socket, &t_address, sizeof(t_address));
    if (error == EC_CANT_BIND_SOCKET) {
        fprintf(stderr, "Failed to bind socket\n");
        close(*t_socket);
        exit(EC_CANT_CREATE_SOCKET);
    }
}

void
listen_socket(int* t_socket, uint32_t t_amount_of_users) {
    int error = listen(*t_socket, t_amount_of_users);
    if (error == EC_CANT_LISTEN_SOCKET) {
        fprintf(stderr, "Failed to listen socket\n");
        close(*t_socket);
        exit(EC_CANT_CREATE_SOCKET);
    }
}

void
accept_sockets(int* t_socket, int* listening_socket, struct sockaddr socket_address,
                               ssize_t* address_length) {
    *t_socket = accept(*listening_socket, &socket_address, address_length);
}