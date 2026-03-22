#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "data_packet.h"

int main(int argc, char* argv[]){
    if(argc != 4){
        printf("Usage : %s <peer_ip> <peer_port> <msg>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* peer_ip = argv[1];
    int peer_port = atoi(argv[2]);
    const char* msg = argv[3];

    struct sockaddr_in peer_addr = {.sin_family = AF_INET, .sin_port = htons(peer_port)};
    
    if(inet_pton(AF_INET, peer_ip, &(peer_addr.sin_addr)) <= 0){
        perror("Something wrong with IP address..");
        return EXIT_FAILURE;
    }

    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket < 0){
        perror("Can't create the socket!");
        return EXIT_FAILURE;
    }

    // if(sendto(udp_socket, msg, strlen(msg) + 1, 0, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) < 0){
        // perror("Failure in sending the message..!!");
        // close(udp_socket);
        // return EXIT_FAILURE;
    // }

    packet pkt = {0};
    pkt.start_bit = 0xAA55;
    pkt.sender_id = 1;pkt.auth_token = 0x1234;
    pkt.index_num = 9;
    pkt.auth_token = 0x1234;
 
    strncpy((char*)pkt.payload, msg, sizeof(pkt.payload) - 1);
    pkt.payload_len = strlen(msg);

    encrypt_decrypt_payload(pkt.payload, pkt.payload_len, 0xCE);

    pkt.checksum = calculate_packet_crc(&pkt);

    sendto(udp_socket, &pkt, sizeof(packet), 0, (struct sockaddr*)&peer_addr, sizeof(peer_addr));
    printf("Sent the encrypted request to the server...\n");

    packet response_pkt;
    socklen_t addr_len = sizeof(peer_addr);
    recvfrom(udp_socket, &response_pkt, sizeof(packet), 0, (struct sockaddr*)&peer_addr, &addr_len);

    printf("Received server response : %s\n", response_pkt.payload);

    printf("Sent \"%s\" to %s:%d\n", msg, peer_ip, peer_port);
    close(udp_socket);
    return EXIT_SUCCESS;
}