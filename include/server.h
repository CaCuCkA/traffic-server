#ifndef TRAFFIC_SERVER_SERVER_H
#define TRAFFIC_SERVER_SERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


void            create_socket(int* t_socket);
void            bind_socket(int* t_socket, struct sockaddr t_address);
void            listen_socket(int* t_socket, uint32_t t_amount_of_users);
void            accept_sockets(int* t_socket, int* listening_socket, struct sockaddr socket_address,
                               ssize_t* address_length);
struct sockaddr make_address(char* t_address, uint16_t t_port);

#endif //TRAFFIC_SERVER_SERVER_H
