/* Guessing game server */

#include <stdio.h>    // printf
#include <string.h>   // memset
#include <stdlib.h>   // exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>     // srand, rand

#define BUFLEN 512    // Max length of buffer
#define PORT 8888     // The port on which to listen for incoming data

void die(char *s) {
    perror(s);
    exit(1);
}

int main(void) {
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    char buf[BUFLEN];
    char *answer;
    int guess;

    // Seed random number generator
    srand(time(0));

    // Generate a random number between 1 and 6
    int number = (rand() % 6) + 1;
    printf("Server has chosen a number between 1 and 6.\n");

    // Create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket");
    }

    // Zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
        die("bind");
    }

    // Keep listening for data
    while (1) {
        printf("Waiting for a guess...\n");
        fflush(stdout);

        // Try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1) {
            die("recvfrom()");
        }

        // Convert received data to integer
        buf[recv_len] = '\0';
        guess = atoi(buf);

        // Check if guess is correct
        if (guess == number) {
            answer = "Congratulations! You guessed the correct number!";
        } else {
            answer = "Sorry, wrong guess. Try again!";
        }

        // Reply to the client
        if (sendto(s, answer, strlen(answer), 0, (struct sockaddr *)&si_other, slen) == -1) {
            die("sendto()");
        }
    }

    close(s);
    return 0;
}
