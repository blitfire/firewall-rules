#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256

void error(char *msg);

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf (stderr,"ERROR, no address provided\n");
        exit(1);
    } else if (argc < 3) {
        fprintf (stderr,"ERROR, no port provided\n");
        exit(1);
    } else if (argc < 4) {
        fprintf (stderr,"ERROR, no mode provided\n");
        exit(1);
    }

    char buffer[BUFFER_LENGTH];

    int socket_fd;
    long n;
    struct addrinfo address_info;
    struct addrinfo *result, *rp;
    int res;

    memset(&address_info, 0, sizeof(struct addrinfo));
    address_info.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    address_info.ai_socktype = SOCK_STREAM; // Datagram socket
    address_info.ai_flags = 0;
    address_info.ai_protocol = 0;           // Any protocol

    // Get a list of address structs
    if (strcmp(argv[2], "localhost") == 0) res = getaddrinfo(argv[1], "127.0.0.1", &address_info, &result);
    else res = getaddrinfo(argv[1], argv[2], &address_info, &result);
    if (res != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
        if (socket_fd == -1) continue;
        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1) break;
        close(socket_fd);
    }

    if (rp == NULL) { // No address succeeded
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    bzero(buffer, BUFFER_LENGTH);
    sprintf(buffer, "%s ", argv[3]);
    for (int i=4; i < argc; i++) {
        strcat(buffer, argv[i]);
        if (i < argc - 1) strcat(buffer, " ");
    }
    // Send request
    n = write(socket_fd, buffer, strlen(buffer));
    if (n < 0) error("ERROR writing to socket\n");
    bzero(buffer, BUFFER_LENGTH);

    // Read response from the server
    while ((n= read(socket_fd, buffer, BUFFER_LENGTH - 1)) > 0) { // -1 for \0
        if (n < 0) error("ERROR reading from socket\n");
        printf("%s", buffer);
        bzero(buffer, BUFFER_LENGTH);
    }
    close(socket_fd);
    return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}
