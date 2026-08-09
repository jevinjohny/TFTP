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
    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        perror("fopen");
        return;
    }

    char data[512];

    int n = fread(data, 1, sizeof(data), fp);

    char buffer[BUFFER_SIZE];

    uint16_t opcode = htons(DATA);

    memcpy(buffer, &opcode, sizeof(opcode));

    uint16_t block_number = 1;
    uint16_t block = htons(block_number);
    memcpy(buffer + sizeof(opcode), &block, sizeof(block));

    memcpy(buffer + sizeof(opcode) + sizeof(block), data, n);

    size_t packetlen = sizeof(opcode) + sizeof(block) + n;

    sendto(sockfd, buffer, packetlen, 0, (struct sockaddr *)&client_addr, client_len);
}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename)
{
    // Implement file receiving logic here
}