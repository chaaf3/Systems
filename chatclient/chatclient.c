/*******************************************************************************
 * Name        : chatclient.c
 * Author      : Ming Lin & Connor Haaf
 * Date        : July 4, 2021
 * Description : chatclient
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "util.h"

volatile sig_atomic_t running = true;

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin() {
    int output = 0;
    if ((output = get_string(outbuf, MAX_MSG_LEN + 1)) == TOO_LONG) {
        fprintf(stderr, "Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
    } else if (send(client_socket, outbuf, strlen(outbuf), 0) < 0) {
        fprintf(stderr,
                "Warning: Failed to broadcast message. %s.\n", strerror(errno));
    } else {
        if (strcmp(outbuf, "bye") == 0) {
            printf("Goodbye.\n");
            return -1;
        }
    }
    return 0;
}

int handle_client_socket() {
    int bytes_recvd = 0;
    bytes_recvd = recv(client_socket, inbuf, MAX_MSG_LEN + 1, 0);
    if (bytes_recvd == -1 && errno != EINTR) {
        fprintf(stderr,
                "Warning: Failed to receive incoming message. %s.\n", strerror(errno));
    } else if (bytes_recvd == 0) {
        // The server disconnected.
        fprintf(stderr, "\nConnection to server has been lost.\n");
        return -1;
    } else {
        // Process the incoming data. If "bye", the client disconnected.
        // Otherwise, broadcast the message to all the other users.
        inbuf[bytes_recvd] = '\0';
        if (strcmp(inbuf, "bye") == 0) {
            fprintf(stderr, "\nServer initiated shutdown. \n");
            return -1;
        } else {
            printf("\n%s\n", inbuf);
            return 0;
        }
    }
    return 0;
}

void printUsage() {
    fprintf(stderr, "Usage: ./chatclient <server IP> <port>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printUsage();
        return EXIT_FAILURE;
    }
    int ip_conversion = 0, bytes_recvd = 0, retval = EXIT_SUCCESS;

    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    ip_conversion = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    if (ip_conversion == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", argv[1]);
        retval = EXIT_FAILURE;
        goto EXIT;
    } else if (ip_conversion < 0) {
        fprintf(stderr, "Error: Failed to convert IP address. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    int port = 0;
    if (!parse_int(argv[2], &port, "port number")) {
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    bool hasBadUser = false, hasNoUser = false;
    int bytes_read_arg = 0;
    printf("Enter your username: ");
    fflush(stdout);

    while (!hasBadUser && !hasNoUser) {
        if ((bytes_read_arg = read(STDIN_FILENO, username, 4096)) < 0) {
            if (errno != EINTR) {
                fprintf(stderr, "Error: Failed to read from stdin. %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
        if (bytes_read_arg == 1) {
            printf("Enter your username: ");
            fflush(stdout);
            hasBadUser = false, hasNoUser = false;
            memset(username, 0, MAX_NAME_LEN + 1);
            continue;
        }

        if (bytes_read_arg > MAX_NAME_LEN + 1) {
            fprintf(stderr, "Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
            printf("Enter your username: ");
            fflush(stdout);
            hasBadUser = false, hasNoUser = false;
            memset(username, 0, MAX_NAME_LEN + 1);
            continue;
        } else {
            hasBadUser = true, hasNoUser = true;
        }
    }

    username[strlen(username) - 1] = '\0';

    printf("Hello, %s. Let's try to connect to the server.\n", username);

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    memset(&server_addr, 0, addrlen);          // Zero out structure
    server_addr.sin_family = AF_INET;          // Internet address family
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Internet address, 32 bits
    server_addr.sin_port = htons(port);        // Server port, 16 bits

    memset(inbuf, 0, BUFLEN);
    memset(outbuf, 0, BUFLEN);

    // Establish the connection to the echo server.
    if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    // printf("hello 149\n");

    if ((bytes_recvd = recv(client_socket, inbuf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if (bytes_recvd == 0) {
        fprintf(stderr, "All connections are busy. Try again later.\n");
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    inbuf[bytes_recvd] = '\0';
    printf("\n%s\n", inbuf);

    if (send(client_socket, username, strlen(username), 0) < 0) {
        fprintf(stderr, "Error: Failed to send username to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    fd_set sockset;
    // int count = 0;
    printf("\n[%s]:", username);
    fflush(stdout);
    while (running) {
        FD_ZERO(&sockset);
        FD_SET(client_socket, &sockset);
        FD_SET(STDIN_FILENO, &sockset);
        int handleSTDIN = 0;
        int handleClient = 0;

        if (select(client_socket + 1, &sockset, NULL, NULL, NULL) < 0 && errno != EINTR) {
            fprintf(stderr, "Error: select() failed. %s.\n", strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT;
        }

        if (client_socket > -1 &&
            FD_ISSET(STDIN_FILENO, &sockset)) {
            if ((handleSTDIN = handle_stdin()) == -1) {
                retval = EXIT_SUCCESS;
                goto EXIT;
            }
        }
        if (running &&
            FD_ISSET(client_socket, &sockset)) {
            if ((handleClient = handle_client_socket()) == -1) {
                retval = EXIT_FAILURE;
                goto EXIT;
            }
        }

        memset(inbuf, 0, BUFLEN);
        memset(outbuf, 0, BUFLEN);
        printf("[%s]:", username);
        fflush(stdout);
    }

EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}