#include "../common/tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet);

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    tftp_packet packet;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Set socket timeout option
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
    {
        perror("setsockopt");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        perror("bind");
        exit(1);
    }

    printf("TFTP Server listening on port %d...\n", PORT);

    // Main loop to handle incoming requests
    while (1)
    {
        socklen_t client_len = sizeof(client_addr);
        int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n < 0)
        {
            // perror("recvfrom");
            continue;
        }
        printf("Received packet: %d bytes\n", n);

        handle_client(sockfd, client_addr, client_len, &packet);
    }

    close(sockfd);
    return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet)
{
    uint16_t opcode = ntohs(packet->opcode);

    if (opcode == RRQ)
    {
        // Extract the TFTP operation (read or write) from the received packet
        char *filename = (char *)packet + sizeof(opcode);
        char *mode = (char *)packet + sizeof(opcode) + strlen(filename) + 1;

        printf("RRQ : filename : %s, mode : %s\n", filename, mode);

        send_file(sockfd, client_addr, client_len, filename);
    }
    else if (opcode == WRQ)
    {
        // Extract the TFTP operation (read or write) from the received packet
        char *filename = (char *)packet + sizeof(opcode);
        char *mode = (char *)packet + sizeof(opcode) + strlen(filename) + 1;

        printf("WRQ : filename : %s, mode : %s\n", filename, mode);

        // send ack
        uint16_t ack_opcode = htons(ACK);
        uint16_t ack_block = htons(0);
        char ack_buffer[4];
        memcpy(ack_buffer, &ack_opcode, sizeof(ack_opcode));
        memcpy(ack_buffer + sizeof(ack_opcode), &ack_block, sizeof(ack_block));
        sendto(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)&client_addr, client_len);
        receive_file(sockfd, client_addr, client_len, filename);
    }
    else
    {
        printf("Unknown\n");
    }

    // and call send_file or receive_file accordingly
}
