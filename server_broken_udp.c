#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5005);

    if(bind(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("Failed to bind\n");
        return -1;
    }

    while(1)
    {
        unsigned char offset_buffer[10] = {'\0'};
        unsigned char command_buffer[2] = {'\0'};
        int offset;
        int command;
        struct sockaddr_in cli_addr;
        socklen_t len = sizeof(cli_addr);

        printf("Waiting for client to send the command (Full File (0) Partial File (1)\n");

        if(recvfrom(sockfd, command_buffer, 2, 0, (struct sockaddr *) &cli_addr, &len) < 0)
            continue;

        sscanf(command_buffer, "%d", &command);

        if(command == 0)
            offset = 0;
        else
        {
            printf("Waiting for client to send the offset\n");
            if(recvfrom(sockfd, offset_buffer, 10, 0, (struct sockaddr *) &cli_addr, &len) < 0)
                continue;
            sscanf(offset_buffer, "%d", &offset);
        }

        /* Open the file that we wish to transfer */
        FILE *fp = fopen("source_file.txt","rb");
        if(fp == NULL)
        {
            printf("File open error");
            return 1;
        }

        /* Read data from file and send it */
        fseek(fp, offset, SEEK_SET);

        while(1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[256]={0};
            int nread = fread(buff, 1, 256, fp);
            printf("Bytes read %d \n", nread);

            /* If read was success, send data. */
            if(nread > 0)
            {
                printf("Sending \n");
                sendto(sockfd, buff, nread, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
            }

            /*
            * There is something tricky going on with read ..
            * Either there was error, or we reached end of file.
            */
            if (nread < 256)
            {
                if (feof(fp))
                    printf("End of file\n");
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
        }

        fclose(fp);
        sleep(1);
    }

    return 0;
}
