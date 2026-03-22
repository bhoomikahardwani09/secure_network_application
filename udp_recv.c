#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "data_packet.h"

#define SIZE 1024
int check(int exp, const char* msg);

int main(int argc, char **argv){
    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }  

    int port = atoi(argv[1]);
    int udp_rx_socket;
    struct sockaddr_in peer_addr;
    struct sockaddr_in my_addr = {.sin_family = AF_INET, 
                                  .sin_addr.s_addr = INADDR_ANY, 
                                  .sin_port = htons(port)};  

    // char buffer[SIZE]; //holds the packet which we'll be receiving

    if((udp_rx_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0){
        perror("Failed to create socket..!!");
        return EXIT_FAILURE;
    }

    //bind the socket to the address
    int res = bind(udp_rx_socket, (struct sockaddr *)&my_addr, sizeof(my_addr));
    check(res, "Couldn't bind the socket to the address!");

    packet rx_pkt;
    socklen_t addr_len = sizeof(peer_addr);
    int bytes_received = recvfrom(udp_rx_socket, &rx_pkt, sizeof(packet), 0, (struct sockaddr*)&peer_addr, &addr_len);

    check(bytes_received, "Failure in receiving :(");
    
    printf("Received a packet from %s:%d -- Message = \n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));

    if(bytes_received != sizeof(packet)) printf("Error : Received partial packet!!\n");

    else{
        uint32_t calculated_crc = calculate_packet_crc(&rx_pkt);
        printf("\n--- Packet Received ---\n");
        printf("From: %s:%d\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
        printf("Start bit: 0x%X\n", rx_pkt.start_bit);
        printf("Index num: %u\n", rx_pkt.index_num);
        printf("Payload: %s\n", rx_pkt.payload);

        if(calculated_crc == rx_pkt.checksum){
            printf("SUCCESS : Integrity Check Passed!\n");

            //1.check authentication
            if(rx_pkt.auth_token != 0x1234) printf("FAILED : Unauthorised sender!\n");

            else{
                //2. decrypt the payload by using the key
                encrypt_decrypt_payload(rx_pkt.payload, rx_pkt.payload_len, 0xCE);
                printf("Decrypted Payload: %s\n", rx_pkt.payload);

                //3.send response back to client
                packet ack_pkt = {0};
                ack_pkt.start_bit = 0xAA55;
                ack_pkt.sender_id = 3;
                const char* ack_msg = "ACK : Frame received";
                strncpy((char*)ack_pkt.payload, ack_msg, sizeof(ack_pkt.payload));
                ack_pkt.payload_len = strlen(ack_msg);
                ack_pkt.checksum = calculate_packet_crc(&ack_pkt);

                sendto(udp_rx_socket, &ack_pkt, sizeof(packet), 0, (struct sockaddr*)&peer_addr, addr_len);
            }
        }

        else printf("FAILED : Data Corruption Detected!\n");
    }
    close(udp_rx_socket);
    return EXIT_FAILURE;
}
#define SOCKETERROR (-1)
int check(int exp, const char* msg){
    if(exp == SOCKETERROR){
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}