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
    int error = bind(*t_socket, (const struct sockaddr *) t_address, sizeof(struct sockaddr_un));
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
read_data_from_socket(const int *t_socket, void* data, uint16_t bufferLength, FILE* fp) {
    ssize_t bytes_recv = recv(*t_socket, data, bufferLength, 0);
    if (bytes_recv == EC_CANT_READ_DATA) {
        raise_error(fp, t_socket, "Failed to read data from socket\n");
    }
    return bytes_recv;
}

void
send_data_to_socket(const int *t_socket, void* data, size_t bytes_recv, FILE* fp) {
    size_t send_bytes = send(*t_socket, data, bytes_recv, 0);
    if (send_bytes == EC_CANT_SEND_DATA) {
        raise_error(fp, t_socket, "Failed to send data from socket\n");
    } else if (send_bytes < bytes_recv) {
        send_data_to_socket(t_socket, data + send_bytes, bytes_recv - send_bytes, fp);
    }
}

void
connect_socket(int* t_socket, struct sockaddr_un* socket_address, FILE* fp) {
    int error = connect(*t_socket, (const struct sockaddr *) socket_address, sizeof(*socket_address));
    if (error == EC_CANT_CONNECT_SOCKET) {
        raise_error(fp, t_socket, "Failed to connect socket\n");
    }
}

void
set_socket_option(int* t_socket, int level, int socket_option, int buffer, size_t buffer_size, FILE* fp) {
    int error = setsockopt(*t_socket, level, socket_option, &buffer, buffer_size);
    if (error == EC_CANT_SET_SOCKET_OPTION) {
        raise_error(fp, t_socket, "Failed to set socket option\n");
    }
}

void
make_address(struct sockaddr_un* server_address, int family, const char* socket_path) {
    memset(server_address, 0, sizeof(*server_address));
    server_address->sun_family = family;
    strncpy(server_address->sun_path, socket_path, sizeof(server_address->sun_path) - 1);
#ifdef DAEMON
    server_address->sun_path[sizeof(server_address->sun_path) - 1] = '\0';
    unlink(socket_path);
#endif
}

