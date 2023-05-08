#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <netinet/ip.h>
#include <linux/if_ether.h>

#include "daemon.h"
#include "server.h"
#include "rbtree.h"
#include "exception.h"


static FILE*       fp = NULL;
static char*       err_msg = NULL;
static char        err_buffer[PCAP_ERRBUF_SIZE];
static int*        client_socket;
static pcap_t*     handle = NULL;
static rb_node_t**  root = NULL;


static void
logging(char* log_msg) {
    fprintf(fp, "%s", log_msg);
    fflush(fp);
}

static void
packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet) {
    struct iphdr *iph = (struct iphdr *) (packet + sizeof(struct ethhdr));
    struct in_addr src_ip;
    memcpy(&src_ip, &iph->saddr, sizeof(src_ip));

    rb_node_t *node = (rb_node_t *) malloc(sizeof(rb_node_t));
    if (node == NULL) {
        logging("Error while allocating memory: %s\n");
        return;
    }
    node->ip = src_ip;
    node->count = 1;
    node->color = RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    rb_node_t *existing_node = (rb_node_t *) find_node(*root, src_ip);
    if (existing_node != NULL) {
        existing_node->count++;
        free(node);
    } else {
        insert(root, node);
    }
}

static void
set_daemon_mode() {
    pid_t process_id = 0;
    pid_t sid = 0;

    process_id = fork();
    if (process_id < 0) {
        fprintf(stderr, "Failed to fork processes");
        exit(EC_CANT_FORK_PROCESS);
    }

    if (process_id > 0) {
        fprintf(stdout, "process_id of child process %d \n", process_id);
        exit(EC_OK);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        fprintf(stderr, "Failed to set sid");
        exit(EC_CANT_SET_SID);
    }

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

static void
start_sniffing() {
    logging("\nStart start_sniffing() logs:\n");

    struct bpf_program bpf;
    bpf_u_int32 mask;
    bpf_u_int32 net = 0;

    if (*root == NULL) {
        struct in_addr null_ip;
        memset(&null_ip, 0, sizeof(null_ip));
        *root = create_node(null_ip, 0);
    }

    if (pcap_compile(handle, &bpf, "ip", 0, net) == -1) {
        snprintf(err_msg, strlen(err_msg), "Couldn't parse filter: %s\n", pcap_geterr(handle));
        logging(err_msg);
        return;
    }
    if (pcap_setfilter(handle, &bpf) == -1) {
        snprintf(err_msg, strlen(err_msg), "Couldn't install filter: %s\n", pcap_geterr(handle));
        logging(err_msg);
        return;
    }

    if (pcap_loop(handle, -1, packet_handler, NULL) == -1) {
        snprintf(err_msg, strlen(err_msg), "Error while sniffing packets: %s\n", pcap_geterr(handle));
        logging(err_msg);
        return;
    }
}

static void
stop_sniffing() {
    pcap_breakloop(handle);
    pcap_close(handle);
}


static void
select_iface(const char* iface_name) {
    logging("Start select_iface() logs:\n");

    pcap_if_t* interfaces;
    char buffer[BUFFER_SIZE];
    int iface_found = 0;

    if (pcap_findalldevs(&interfaces, err_buffer) == -1) {
        snprintf(buffer, BUFFER_SIZE, "Error finding available interfaces: %s\n", err_buffer);
        logging(buffer);
        send_data_to_socket(client_socket, buffer, strlen(buffer), fp);
        return;
    }

    for (pcap_if_t* iface = interfaces; iface != NULL; iface = iface->next) {
        if (strcmp(iface_name, iface->name) == 0) {
            handle = pcap_open_live(iface->name, BUFSIZ, 1, 1000, err_buffer);
            if (handle == NULL) {
                snprintf(buffer, BUFFER_SIZE, "Error opening interface %s: %s\n", iface->name, err_buffer);
                logging(buffer);
                send_data_to_socket(client_socket, buffer, strlen(buffer), fp);
            } else {
                iface_found = 1;
                break;
            }
        }
    }

    pcap_freealldevs(interfaces);

    if (!iface_found) {
        snprintf(buffer, BUFFER_SIZE, "Error: interface %s not found\n", iface_name);
        logging(buffer);
        send_data_to_socket(client_socket, buffer, strlen(buffer), fp);
    }
}

static void
print_ip_packet_count(const char* ip_str) {
    struct in_addr ip;
    int count = 0;
    if (inet_aton(ip_str, &ip) == 0) {
        logging("Invalid IP address\n");
        return;
    }
    rb_node_t* node = find_node(*root, ip);
    if (node == NULL) {
        logging("IP address not found in RB tree\n");
        return;
    }
    char buffer[BUFFER_SIZE];
    count = node->count;
    snprintf(buffer, BUFFER_SIZE, "IP address %s has %d packets\n", inet_ntoa(node->ip), count);
    send_data_to_socket(client_socket, buffer, strlen(buffer), fp);
    logging(buffer);
}


static void
help() {
    char* commands = "Available commands:\n"
                     " - start - start sniffing on the specified interface\n"
                     " - stop - stop sniffing\n"
                     " - stats [iface] - show statistics for the specified interface\n"
                     " - show [ip] count - show the number of packets received from the specified IP\n"
                     " - help - show this help message\n"
                     " - exit - exit the program\n";

    send_data_to_socket(client_socket, commands, strlen(commands), fp);
}

void
set_connection_options(int* server_socket, char* socket_path) {
    create_socket(server_socket, AF_UNIX, fp);
    struct sockaddr_un server_addr = {0};
    make_address(&server_addr, AF_UNIX, socket_path);
    bind_socket(server_socket, &server_addr, fp);
    listen_socket(server_socket, SOMAXCONN, fp);
}

static void
command_dispatcher(int* t_socket, char* buffer) {
    char* command = strtok(buffer, " ");

    if (!strcmp(command, "start")) {
        start_sniffing();
    } else if (!strcmp(command, "stop")) {
        stop_sniffing();
    } else if (!strcmp(command, "show")) {
        print_ip_packet_count(strtok(NULL, " "));
    } else if (!strcmp(command, "select")) {
        select_iface(strtok(NULL, " "));
    } else if (!strcmp(command, "stat")) {
//        show_statistic();
    } else {
        help();
    }
}

void
run(char* socket_path) {
    int* server_socket;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_recv = 0;

    set_connection_options(server_socket, socket_path);



//    while (1) {
//        accept_sockets(client_socket, server_socket, NULL, NULL, fp);
//        memset(buffer, 0, BUFFER_SIZE);
//        read_data_from_socket(client_socket, buffer, BUFFER_SIZE, fp);
//        command_dispatcher(client_socket, buffer);
//    }
}