#include <stdio.h>
#include <stdlib.h>

#include "server.h"
#include "exception.h"

static void
raise_error(FILE* fp, const int *t_socket, char *error_msg) {
    fprintf(fp == NULL ? stderr : fp, "%s", error_msg);
    close(*t_socket);
    exit(-1);
}

void
create_socket(int *t_socket, int domain, FILE* fp) {
    *t_socket = socket(domain, SOCK_STREAM, 0);
    if (*t_socket == EC_CANT_CREATE_SOCKET) {
        raise_error(fp, t_socket, "Failed to create socket\n");
    }
}

void
bind_socket(const int *t_socket, struct sockaddr_un *t_address, FILE* fp) {
    int error = bind(*t_socket, (const struct sockaddr *) t_address, sizeof(*t_address));
    if (error == EC_CANT_BIND_SOCKET) {
        raise_error(fp, t_socket, "Failed to bind socket\n");
    }
}

void
listen_socket(const int *t_socket, int t_amount_of_users, FILE* fp) {
    int error = listen(*t_socket, t_amount_of_users);
    if (error == EC_CANT_LISTEN_SOCKET) {
        raise_error(fp, t_socket, "Failed to listen socket\n");
    }
}

void
accept_sockets(int *t_socket, const int *listening_socket, struct sockaddr_un *socket_address,
               socklen_t*  address_length, FILE* fp) {
    *t_socket = accept(*listening_socket, (struct sockaddr *) socket_address, address_length);
    if (*t_socket == EC_CANT_ACCEPT_SOCKET) {
        raise_error(fp, t_socket, "Failed to accept socket\n");
    }
}

ssize_t
read_data_from_socket(const int *t_socket, char *buffer, uint16_t bufferLength, FILE* fp) {
    ssize_t bytes_recv = recv(*t_socket, buffer, bufferLength, 0);
    if (bytes_recv == EC_CANT_READ_DATA) {
        raise_error(fp, t_socket, "Failed to read data from socket\n");
    }
    return bytes_recv;
}

void
send_data_to_socket(const int *t_socket, char *buffer, ssize_t bytes_recv, FILE* fp) {
    ssize_t send_bytes = send(*t_socket, buffer, bytes_recv, 0);
    if (send_bytes == EC_CANT_SEND_DATA) {
        raise_error(fp, t_socket, "Failed to send data from socket\n");
    } else if (send_bytes < bytes_recv) {
        send_data_to_socket(t_socket, buffer + send_bytes, bytes_recv - send_bytes, fp);
    }
}

void
make_address(struct sockaddr_un *server_address, int family, char *socket_path) {
    memset(server_address, 0, sizeof(struct sockaddr_un));
    server_address->sun_family = family;
    strncpy(server_address->sun_path, socket_path, sizeof(server_address->sun_path) - 1);
    unlink(socket_path);
}
