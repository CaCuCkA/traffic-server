#ifndef TRAFFIC_SERVER_SERVER_H
#define TRAFFIC_SERVER_SERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/un.h>


void        create_socket(int* t_socket, int domain, FILE* fp);
void        bind_socket(const int *t_socket, struct sockaddr_un* t_address, FILE* fp);
void        listen_socket(const int* t_socket, int t_amount_of_users, FILE* fp);
void        accept_sockets(int* t_socket, const int* listening_socket, struct sockaddr_un* socket_address,
                           socklen_t*  address_length, FILE* fp);
void        set_socket_option(int* t_socket, int level, int socket_option, const void* buffer,
                              size_t buffer_size, FILE* fp);
void        connect_socket(int* t_socket, struct sockaddr_un* socket_address, FILE* fp);
void        make_address(struct sockaddr_un* server_address, int family, const char* socket_path);
void        send_data_to_socket(const int* t_socket, char* buffer, ssize_t bytes_recv, FILE* fp);
ssize_t     read_data_from_socket(const int* t_socket, char* buffer, uint16_t bufferLength, FILE* fp);

#endif //TRAFFIC_SERVER_SERVER_H
