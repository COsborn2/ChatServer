#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <limits.h>
#include "file/file.h"

#define NAMELENGTH 8
#define MAX_BUFF 512

/*This program acts as a client and takes input from keyboard to sends to
 *a server.
 *
 *Authors: Kenneth White, Casey White, Cameron Osborne, Andy Robinson, Arrin Torno
*/

//void *readT(void *sockfd);
void getUserName(char userName[]);

int containsFileT(char *str);

void containsDisconnect(char *str);

int main(int argc, char **argv) {

    int sockfd, n, nb;
    char sendline[MAX_BUFF], ack[3], userName[NAMELENGTH];
    bzero(ack, 3);
    struct sockaddr_in servaddr;
    pthread_t thread1;
    char IP[20] = "127.0.0.1";
    uint16_t port = 22000;
    getUserName(userName);
    fd_set masterList;
    fd_set tempList;


/*Take an optional IP address, or IP Address and port no. from command line*/
    if (argc >= 2) {
    	strncpy(IP, argv[1], 20);
    }
    if (argc >= 3) {
    	long int rval = strtol(argv[2], NULL, 10);
    	
		if (rval >= SHRT_MIN && rval <= SHRT_MAX)
		    port = rval;
    	else {
    		printf("Port number is not within the proper range!\n");
    		return 0;
    	}
    }

    /*Initialize Socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &(servaddr.sin_addr));

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    FD_ZERO(&masterList);
    FD_SET(sockfd, &masterList);
    FD_SET(0, &masterList);
    tempList = masterList;
    read(sockfd, ack, 2);

    int debug;
    if (strcmp(ack, "ok") != 0) {
        goto joinFail;
    }//invalid connect

    //Re-prompt name until given ok from server
    while (1) {
        write(sockfd, userName, NAMELENGTH);
        bzero(ack, 3);
        //waits on input from server
        read(sockfd, ack, 2);
        if (strcmp(ack, "ok") != 0) {
            printf("Entered username is not unique\n");
            getUserName(userName);
        } else { break; }
    }//name loop

    printf("Client: Press enter to send message to Server:\n");
    //Primary listen loop
    while (1) {
        tempList = masterList;
        select(10, &tempList, NULL, NULL, NULL);

        //socket ready to read
        if (FD_ISSET(sockfd, &tempList)) {
            read:
            bzero(sendline, MAX_BUFF);

            do {
                nb = read(sockfd, sendline, MAX_BUFF-1);  //receive
                if (nb > 0) {
                    printf("%s\n", sendline);
                    if (containsFileT(sendline)) {
                        recFile(sendline, sockfd);
                        printf("Client: File received.\n");
                    }
                }
                else{//server socket down
                    close(sockfd);
                    printf("\n\n***Lost connection with server. Exiting.***");
                    exit(0);
                }

            } while (nb == MAX_BUFF-1);//if true, buffer not empty
            
            continue;
        }//END if sockfd


        //client ready to write
        if (FD_ISSET(0, &tempList)) {
            bzero(sendline, MAX_BUFF);   //clean char arrays

            fgets(sendline, MAX_BUFF-1, stdin);  //gets input

            if (containsFileT(sendline)) {
                if (sendFile(sendline, sockfd)) {
                    printf("\nClient: File Sent Successfully.");
                } else { printf("\nClient: File failed to send."); }
            }

            write(sockfd, sendline, strlen(sendline) + 1);
            containsDisconnect(sendline);
        }// END if KB
    }// END listen loop

    joinFail:
    close(sockfd);
    printf("Failed to join with Server ack '%s' instead of 'ok'\n", ack);
}

//Implemented via select()
///*Thread used to read from server*/
//void *readT(void *sockfd) {
//    char recvline[100];
//    int sd = *(int *) sockfd;
//    int tst;
//
//    while (1) {
//
//    }
//    pthread_exit(0);
//}

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

void containsDisconnect(char *str) {
    if (str != NULL && strlen(str) >= 2) {
        if (str[0] == '/' && str[1] == 'x') {
            printf("Goodbye...\n\n");
            exit(0);
        }
    }
}
