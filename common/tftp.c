/* Common file for server & client */

#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, char *mode)
{
    // Implement file sending logic here
    int fd = open(filename, O_RDONLY);

    if (fd < 0)
    {
        perror("open");
        return;
    }

    int size;
    if (strcmp(mode, "default") == 0)
    {
        size = 512;
    }
    else if (strcmp(mode, "octet") == 0)
    {
        size = 1;
    }
    char data[size];

    uint16_t block_number = 1;
    while (1)
    {
        ssize_t n = read(fd, data, sizeof(data));

        if (n < 0)
        {
            perror("read");
            close(fd);
            return;
        }

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
        if (n < (ssize_t)sizeof(data))
            break;

        block_number++;
    }
    close(fd);
}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, char *mode)
{
    // Implement file receiving logic here
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0)
    {
        perror("fopen");
        return;
    }
    uint16_t block_number = 1;
    char buffer[BUFFER_SIZE];

    while (1)
    {
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
        if (n < 0)
        {
            perror("recvfrom");
            return;
        }
        printf("received %d bytes\n", n);

        // extract opcode
        uint16_t data_opcode;
        memcpy(&data_opcode, buffer, sizeof(data_opcode));
        data_opcode = ntohs(data_opcode);

        // extract
        uint16_t block;
        memcpy(&block, buffer + sizeof(data_opcode), sizeof(block));
        block = ntohs(block);

        // verify
        if (data_opcode == DATA && block == block_number)
        {
            size_t datalen = n - (sizeof(data_opcode) + sizeof(block));

            int val = write(fd, buffer + sizeof(data_opcode) + sizeof(block), datalen);

            if (val < 0)
            {
                perror("write");
                close(fd);
                return;
            }

            // creating ack
            uint16_t ack_opcode = htons(ACK);
            char ack_buffer[4];
            memcpy(ack_buffer, &ack_opcode, sizeof(ack_opcode));
            uint16_t ack_block = htons(block_number);
            memcpy(ack_buffer + sizeof(ack_opcode), &ack_block, sizeof(ack_block));

            sendto(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)&client_addr, client_len);

            if (strcmp(mode, "default") == 0)
            {
                if (datalen < 512)
                    break;
            }
            else if (strcmp(mode, "octet") == 0)
            {
                if (datalen < 1)
                    break;
            }
            block_number++;
        }
    }
    close(fd);
}