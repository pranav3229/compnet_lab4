#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[256];
    unsigned char buff_offset[10]; // buffer to send the File offset value
    unsigned char buff_command[2]; // buffer to send the Complete File (0) or Partial File Command (1)
    int offset; // required to get the user input for offset in case of partial file command
    int command; // required to get the user input for command

    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;

    /* Create a socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    /* Initialize sockaddr_in data structure */
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5005); // port
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return 1;
    }

    /* Create file where data will be stored */
    FILE *fp;
    fp = fopen("destination_file.txt", "ab");
    if (NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);
    fclose(fp);

    fp = fopen("destination_file.txt", "ab");
    if (NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    printf("Enter (0) to get complete file, (1) to specify offset, (2) calculate the offset value from local file\n");
    scanf("%d", &command);
    sprintf(buff_command, "%d", command);
    sendto(sockfd, buff_command, 2, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (command == 1 || command == 2) // We need to specify the offset
    {
        if (command == 1) // get the offset from the user
        {
            printf("Enter the value of File offset\n");
            scanf("%d", &offset);
        }
        // otherwise offset = size of local partial file, that we have already calculated
        sprintf(buff_offset, "%d", offset);
        /* sending the value of file offset */
        sendto(sockfd, buff_offset, 10, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    }
    // Else { command = 0 then no need to send the value of offset }

    /* Receive data in chunks of 256 bytes */
    socklen_t addr_len = sizeof(serv_addr);
    while ((bytesReceived = recvfrom(sockfd, recvBuff, 256, 0, (struct sockaddr *)&serv_addr, &addr_len)) > 0)
    {
        printf("Bytes received %d\n", bytesReceived);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1, bytesReceived, fp);
        // printf("%s \n", recvBuff);
    }

    if (bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }

    return 0;
}
