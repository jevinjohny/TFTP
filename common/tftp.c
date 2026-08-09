/* Common file for server & client */

#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename)
{
    // Implement file sending logic here
    FILE *fp = fopen(filename, "rb");

    if (!fp)
    {
        perror("fopen");
        return;
    }

    char data[512];
    uint16_t block_number = 1;
    while (1)
    {
        size_t n = fread(data, 1, sizeof(data), fp);

        char buffer[BUFFER_SIZE];

        uint16_t opcode = htons(DATA);

        memcpy(buffer, &opcode, sizeof(opcode));

        uint16_t block = htons(block_number);
        memcpy(buffer + sizeof(opcode), &block, sizeof(block));

        memcpy(buffer + sizeof(opcode) + sizeof(block), data, n);

        size_t packetlen = sizeof(opcode) + sizeof(block) + n;

        sendto(sockfd, buffer, packetlen, 0, (struct sockaddr *)&client_addr, client_len);

        char ack_buffer[4];
        int ack_len = recvfrom(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)&client_addr, &client_len);
        if (ack_len == 4)
        {
            // extract opcode
            uint16_t ack_opcode;
            memcpy(&ack_opcode, ack_buffer, sizeof(ack_opcode));
            ack_opcode = ntohs(ack_opcode);

            // extract blocknumber
            uint16_t ack_block;
            memcpy(&ack_block, ack_buffer + sizeof(ack_opcode), sizeof(ack_block));
            ack_block = ntohs(ack_block);

            if (ack_block == block_number && ack_opcode == ACK)
            {
                printf("Block %d is acknowleged\n", block_number);
            }
        }
        block_number++;
        if (n < sizeof(data))
            break;
    }
}

// void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename)
// {
//     // Implement file receiving logic here
// }