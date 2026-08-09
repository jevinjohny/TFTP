#include "tftp.h"
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
}