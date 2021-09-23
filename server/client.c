#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "clientserver.h"

bool msgs_too_long(int argc, char *argv[]) {
    int cumulative_len = 0;
    for (int i = 1; i < argc; i++) {
        cumulative_len += strlen(argv[i]);
        if (i < argc - 1) {
            cumulative_len++; // +1 for space
        }
        if (cumulative_len >= BUFLEN) {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (msgs_too_long(argc, argv)) {
        fprintf(stderr,
                "Error: At most %d characters can be sent, including '\\0'!\n",
                BUFLEN);
        return EXIT_FAILURE;
    }

    int client_socket, bytes_recvd, ip_conversion, retval = EXIT_SUCCESS;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];
    char *addr_str = "96.242.21.198";

    // Create a reliable, stream socket using TCP.
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    memset(&server_addr, 0, addrlen); // Zero out structure

    // Convert character string into a network address.
    ip_conversion = inet_pton(AF_INET, addr_str, &server_addr.sin_addr);
    if (ip_conversion == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", addr_str);
        retval = EXIT_FAILURE;
        goto EXIT;
    } else if (ip_conversion < 0) {
        fprintf(stderr, "Error: Failed to convert IP address. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    server_addr.sin_family = AF_INET;   // Internet address family
    server_addr.sin_port = htons(PORT); // Server port, 16 bits.
    // server_addr.sin_addr is already set from inet_pton() above.

    memset(buf, 0, BUFLEN);
    for (int i = 1; i < argc; i++) {
        if (strlen(buf) + strlen(argv[i]) + 1 >= BUFLEN) {
            break;
        }
        strncat(buf, argv[i], BUFLEN - 1);
        if (i != argc - 1) {
            strncat(buf, " ", BUFLEN - 1);
        }
    }

    // Establish the connection to the echo server.
    if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    printf("Sending message to server at [%s:%d].\n",
           inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    if (send(client_socket, buf, strlen(buf), 0) < 0) {
        fprintf(stderr, "Error: Failed to send message to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if ((bytes_recvd = recv(client_socket, buf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    buf[bytes_recvd] = '\0';
    printf("Received response from server: %s\n", buf);

EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}
