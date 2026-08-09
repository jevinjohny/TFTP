#include "../common/tftp.h"
#include "tftp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main()
{
    char command[256];
    tftp_client_t client;
    memset(&client, 0, sizeof(client)); // Initialize client structure

    // Main loop for command-line interface
    while (1)
    {
        printf("tftp> ");
        fgets(command, sizeof(command), stdin);

        // Remove newline character
        command[strcspn(command, "\n")] = 0;

        // Process the command
        process_command(&client, command);
    }

    return 0;
}

// Function to process commands
void process_command(tftp_client_t *client, char *command)
{
    char *cmd = strtok(command, " ");
    if (strcmp(cmd, "connect") == 0)
    {
        char *ip = strtok(NULL, " ");
        char *port_str = strtok(NULL, " ");
        int port = atoi(port_str);
        connect_to_server(client, ip, port);
    }
    else if (strcmp(cmd, "get") == 0)
    {
        char *filename = strtok(NULL, " ");

        get_file(client, filename);
    }
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port)
{
    // TODO: ERROR CHECKING

    // Create UDP socket
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Set socket timeout option
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    setsockopt(client->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Set up server address
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &client->server_addr.sin_addr);
    client->server_len = sizeof(client->server_addr);
}

void put_file(tftp_client_t *client, char *filename)
{
    // Send WRQ request and send file
}

void get_file(tftp_client_t *client, char *filename)
{
    // Send RRQ and recive file
    send_request(client->sockfd, client->server_addr, filename, RRQ);
    receive_request(client->sockfd, client->server_addr, filename, RRQ);
}

void disconnect(tftp_client_t *client)
{
    // close fd
}

void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    char buffer[BUFFER_SIZE];

    // convert the opcode to network byte order
    uint16_t net_opcode = htons(opcode);

    memcpy(buffer, &net_opcode, sizeof(net_opcode));
    strcpy(buffer + sizeof(net_opcode), filename);
    strcpy(buffer + sizeof(net_opcode) + strlen(filename) + 1, "octet");

    size_t packet_len = sizeof(net_opcode) + strlen(filename) + 1 + strlen("octet") + 1;

    sendto(sockfd, buffer, packet_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void receive_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    char buffer[BUFFER_SIZE];
    socklen_t server_len = sizeof(server_addr);

    uint16_t block_number = 1;

    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("fopen");
        return;
    }

    while (1)
    {

        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &server_len);
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

            fwrite(buffer + sizeof(data_opcode) + sizeof(block), 1, datalen, fp);

            
            // creating ack
            uint16_t ack_opcode = htons(ACK);
            char ack_buffer[4];
            memcpy(ack_buffer, &ack_opcode, sizeof(ack_opcode));
            uint16_t ack_block = htons(block_number);
            memcpy(ack_buffer + sizeof(ack_opcode), &ack_block, sizeof(ack_block));
            
            sendto(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)&server_addr, server_len);
            
            block_number++;
            
            if (datalen < 512)
            {
                break;
            }
        }
    }
    fclose(fp);
}