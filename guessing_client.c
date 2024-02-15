/* Guessing game client */

#include <stdio.h>    // printf
#include <string.h>   // memset
#include <stdlib.h>   // exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN 512    // Max length of buffer
#define PORT 8888     // The port on which to send data

void die(char *s) {
    perror(s);
    exit(1);
}

int main(void) {
    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
    char buf[BUFLEN];
    int guess;

    // Create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket");
    }

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        printf("Enter your guess (a number between 1 and 6): ");
        scanf("%d", &guess);

        // Convert the guess to string
        sprintf(buf, "%d", guess);

        // Send the guess to the server
        if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&si_other, slen) == -1) {
            die("sendto()");
        }

        // Receive reply from server
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen) == -1) {
            die("recvfrom()");
        }

        // Print the server's reply
        printf("Server says: %s\n", buf);
    }

    close(s);
    return 0;
}
