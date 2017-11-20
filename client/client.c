#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include "file/file.h"

#define NAMELENGTH 8

/*
This program acts as a client and takes input from keyboard to send to
a server.

Authors: Kenneth White, Casey White, Cameron Osborne, Andy Robinson
*/
void *readT(void *sockfd);

void getUserName(char userName[]);

int containsFileT(char *str);

int main(int argc, char **argv) {

    int sockfd, n, nb;
    char sendline[100], ack[3], userName[NAMELENGTH];
    bzero(ack, 3);
    struct sockaddr_in servaddr;
    pthread_t thread1;
    char IP[20] = "127.0.0.1";
    uint16_t port = 22000;
    getUserName(userName);

/*Take an optional IP address, or IP Address and port no. from command line*/
    if (argc == 2) {
        IP[0] = *argv[1];
    } else if (argc == 3) {
        IP[0] = *argv[1];
        port = (uint16_t) argv[2];
    }

    /*Initialize Socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &(servaddr.sin_addr));

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    read(sockfd, ack, 2);
    int debug;
    if (strcmp(ack, "ok") == 0){
        while (1) {
            write(sockfd, userName, NAMELENGTH);
            bzero(ack, 3);
            read(sockfd, ack, 2);
            if (strcmp(ack, "ok") == 0) {
                pthread_create(&thread1, NULL, readT, &sockfd);

                /*Takes input from client, writes to server*/
                while (1) {
                    bzero(sendline, 100);   //clean char arrays
                    printf("\nEnter String to send to Server: ");
                    fgets(sendline, 100, stdin);  //gets input

                    if (containsFileT(sendline)) {
                        if (sendFile(sendline, sockfd)) {
                            printf("\nFile Sent Successfully.");
                        } else { printf("\nFile failed to send."); }
                    }

                    write(sockfd, sendline, strlen(sendline) + 1);
                }
            } else {
                printf("Entered username is not unique\n");
                getUserName(userName);
            }
        }//while 1
}
    close(sockfd);
    printf("ack: %s debug: %d", ack, debug);
}

/*Thread used to read from server*/
void *readT(void *sockfd) {
    char recvline[100];
    int sd = *(int *) sockfd;
    int tst;

    while (1) {
        bzero(recvline, 100);
        tst = read(sd, recvline, 100);  //receive
        if (tst > 0) {
            printf("\n%s", recvline);
            if (containsFileT(recvline)) {
                recFile(recvline, sd);
            }
        }
    }
    pthread_exit(0);
}

void getUserName(char userName[]) {
    char temp[100];
    do {
        bzero(temp, 100);
        printf("Enter username (max 8 characters): ");
        fgets(temp, 100, stdin);
        temp[strcspn(temp, "\n")] = '\0';//remove newline
    } while (strlen(temp) > NAMELENGTH);
    strncpy(userName, temp, NAMELENGTH);
}

int containsFileT(char *str) {
    if (str != NULL && strlen(str) >= 2) {
        if (str[0] == '/') {
            return str[1] == 'f';
        }
    }
    return 0;
}