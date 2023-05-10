#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <linux/if_ether.h>
#include <pthread.h>
#include <stdbool.h>

#include "utils.h"
#include "daemon.h"
#include "server.h"
#include "rbtree.h"

static int          client_socket;
static bool         is_sniffing = false;
static FILE*        fp = NULL;
static pcap_t*      handle = NULL;
static rb_node_t**  root;

static void
packet_handler(u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {
    struct iphdr *iph = (struct iphdr *) (packet + sizeof(struct ethhdr));
    struct in_addr src_ip;
    memcpy(&src_ip, &iph->saddr, sizeof(src_ip));

    rb_node_t* existing_node = find_node(*root, src_ip);
    if (existing_node != NULL) {
        existing_node->count++;
    } else {
        logging(fp, "Create new packet from: %s\n", inet_ntoa(src_ip));
        rb_node_t* node = create_node(src_ip, 1);
        insert(root, node);
    }
}

static void*
sniff_thread_func(void* args) {
    int packets_number = -1;
    is_sniffing = true;
    pcap_loop(handle, packets_number, packet_handler, NULL);
    is_sniffing = false;
    return NULL;
}

static void
start_sniffing() {
    logging(fp, "\nStart start_sniffing() logs:\n");

    char filter_exp[] = "ip";
    struct bpf_program bpf;
    bpf_u_int32 netmask = 0;

    if (pcap_compile(handle, &bpf, filter_exp, 0, netmask) == -1) {
        logging(fp, "pcap compile does not work!\n");
        return;
    }
    if (pcap_setfilter(handle, &bpf) == -1) {
        logging(fp, "Couldn't install filter '%s': %s\n", filter_exp, pcap_geterr(handle));
        return;
    }

    pthread_t sniffer_thread;
    if (pthread_create(&sniffer_thread, NULL, sniff_thread_func, NULL) != 0) {
        logging(fp, "Failed to create sniff thread\n");
        return;
    }

    pthread_detach(sniffer_thread);
}

static void
stop_sniffing() {
    logging(fp, "stop sniffing\n");
    pcap_breakloop(handle);
    pcap_close(handle);
}

static void
select_iface(const char* iface_name) {
    logging(fp, "Start select_iface() logs:\n");

    pcap_if_t* interfaces;
    char err_buffer[PCAP_ERRBUF_SIZE];
    char buffer[BUFFER_SIZE];
    int iface_found = 0;

    if (pcap_findalldevs(&interfaces, err_buffer) == -1) {
        logging(fp, "Error finding available interfaces: %s\n", err_buffer);
        send_data_to_socket(&client_socket, buffer, strlen(buffer), fp);
        return;
    }

    for (pcap_if_t* iface = interfaces; iface != NULL; iface = iface->next) {
        if ((strncmp(iface->name, "eth", 3) == 0 || strncmp(iface->name, "wlan", 4) == 0)
            && strstr(iface->name, iface_name) != NULL) {
            handle = pcap_open_live(iface->name, BUFSIZ, 1, 1000, err_buffer);
            if (handle == NULL) {
                logging(fp, "Error opening interface %s: %s\n", iface->name, err_buffer);
            } else {
                iface_found = 1;
                break;
            }
        }
    }

    pcap_freealldevs(interfaces);

    if (!iface_found) {
        logging(fp, "Error: interface %s not found\n", iface_name);
    }
}

static void
print_ip_packet_count(const char* ip_str) {
    struct in_addr ip;
    unsigned int count = 0;

    if (inet_aton(ip_str, &ip) == 0) {
        fprintf(fp, "Invalid IP address\n");
        return;
    }

    rb_node_t* node = find_node(*root, ip);
    if (node == NULL) {
        fprintf(fp, "IP address not found in RB tree\n");
        return;
    }

    count = node->count;
    send_data_to_socket(&client_socket, &count, sizeof(int), fp);
}

static void
show_statistic() {
    char str[BUFFER_SIZE];
    struct pcap_stat stats;

    if (pcap_stats(handle, &stats) == -1) {
        logging(fp, "Error getting packet capture statistics\n");
        return;
    }

    snprintf(str, 512, "Packet capture statistics:\n"
                       "  Packets received: %u\n"
                       "  Packets dropped (by driver): %u\n"
                       "  Packets dropped (because there was no room in the operating system's buffer): %u\n"
                       "  Packets dropped (by filter): %u\n",
         stats.ps_recv, stats.ps_drop, stats.ps_ifdrop, stats.ps_drop);

    send_data_to_socket(&client_socket, str, 512, fp);
}

static void
command_dispatcher(char* buffer) {
    char* command = strtok(buffer, " ");

    if (!strcmp(command, "start")) {
        select_iface("eth0");
        start_sniffing();
    } else if (!strcmp(command, "stop")) {
        stop_sniffing();
    } else if (!strcmp(command, "show")) {
        print_ip_packet_count(strtok(NULL, " "));
    } else if (!strcmp(command, "select")) {
        select_iface(strtok(NULL, " "));
    } else if (!strcmp(command, "stat")) {
        show_statistic();
    }
}

void
run(char* socket_path) {
    int server_socket;
    struct sockaddr_un server_address;
    socklen_t socket_length = sizeof(server_address);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_recv;

    fp = open_file();

//    set_daemon_mode();

    root = rebuild_rbtree();

    set_connection_options(&server_socket, socket_path, fp);

    accept_sockets(&client_socket, &server_socket, &server_address, &socket_length, fp);

    while (true) {
        bytes_recv = read_data_from_socket(&client_socket, buffer, BUFFER_SIZE, fp);
        buffer[bytes_recv] = '\0';

        if (!strcmp(buffer, "exit")) {
            break;
        }

        command_dispatcher(buffer);

        memset(buffer, 0, bytes_recv);
    }

    if (is_sniffing) {
        logging(fp, "Works!\n");
        stop_sniffing();
    }

    end_processes(fp, root, &server_socket, &client_socket);
}