#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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

    char buffer[SIZE]; //holds the packet which we'll be receiving

    if((udp_rx_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0){
        perror("Failed to create socket..!!");
        return EXIT_FAILURE;
    }

    //bind the socket to the address
    int res = bind(udp_rx_socket, (struct sockaddr *)&my_addr, sizeof(my_addr));
    check(res, "Couldn't bind the socket to the address!");

    socklen_t addr_len = sizeof(peer_addr);
    int bytes_received = recvfrom(udp_rx_socket, buffer, SIZE, 0, (struct sockaddr*)&peer_addr, &addr_len);

    check(bytes_received, "Failure in receiving :(");
    
    printf("Received a packet from %s:%d -- Message = %s\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), buffer);

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