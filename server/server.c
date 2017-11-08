#include <stdio.h>
#include <netinet/in.h>
#include <memory.h>
#include <sys/time.h>
#include <sys/types.h>
#include <malloc.h>
#include <unistd.h>

#define MAXCLIENTS 3
#define MAX 100
#define MAXWAITLIST 5

struct client{
    int connected;
    int sockedfd;
    char name[20];
};
typedef struct client Client;

void printToClients(char message[],Client * clientList, int numOfClients, char name[], int notToSend);
void closeServer(Client * clientlist, int numOfClients, int listenFd);

int main() {
    char receiveLine [MAX];
    char sendLine [MAX];
    int serverFd, listenFd;
    struct sockaddr_in servaddr;

    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);
    bind(listenFd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenFd, 10);




    fd_set fdsMaster, fdsCopy;
    char byteRead;
    int currentFd, numberOfClients = 0;
    Client waitlist[MAXWAITLIST];
    Client clients[MAXCLIENTS];

    FD_ZERO(&fdsMaster);        //zeros all bits in fdsMaster so none are listened to
    FD_SET(0,&fdsMaster);       //sets fdsMaster[0] to 1 so it listens to the kb
    FD_SET(listenFd,&fdsMaster);    //adds socket fd
    fdsCopy = fdsMaster;        //bitwise copy


    while(1){
        fdsCopy = fdsMaster;
        select(FD_SETSIZE,&fdsCopy,NULL,NULL,NULL);                       //dont care about writefds or exceptfds, last is to blobk indefinitly. returns 1 if something is ready

        for(currentFd = 0; currentFd < FD_SETSIZE; currentFd++) {
            if (FD_ISSET(currentFd, &fdsCopy)) {    //if current fd in the fd set is ready to be read

                if(currentFd == listenFd) {         //if current fd is the serverFd then its a pending connection
                    clients[currentFd-3].sockedfd = accept(listenFd, (struct sockaddr*) NULL, NULL);                                         ///////////////////////////////////////////do i pass struct pointer?


                    if (numberOfClients < MAXCLIENTS) { //if there is room for the additional client
                        //add client
                        clients[currentFd-3].connected = 1;
                        FD_SET(currentFd,&fdsMaster);
                        numberOfClients++;
                        read(currentFd,&receiveLine,MAX);
                        printToClients("connected",clients,numberOfClients,"", -1);
                        printf("fd %d connected",currentFd);
                        //print to all clients that someone connected
                    }
                    else {
                        //add client to waitlist
                        char message[] = "There are already ";
                                strcat(message,MAXCLIENTS);
                            strcat(message," people. You are being added to the waitlist."); //should add how many people are in the queue

                        /*   waitlist   */
                        write(clients[currentFd].sockedfd,message,strlen(message));
                        close(clients[currentFd].sockedfd);
                    }
                    break;
                }
                else if(currentFd == 0){
                    //kb
                    read(0,&sendLine, MAX);
                    if(strcmp(sendLine,"/quit")){
                        closeServer(clients, numberOfClients,listenFd);
                    }
                    else
                        printToClients(sendLine,clients, numberOfClients, "server",-1);                                                    //strcmp quit then close clients

                }
                else if(currentFd > 0){
                    //read client data
                    if(read(currentFd,receiveLine,MAX)){    //if read > 0 bytes
                        if(clients[currentFd-3].connected == 2){
                            strcpy(clients[currentFd-3].name,receiveLine); //save name
                            clients[currentFd-3].connected = 1;
                        }
                        else if(strcmp(receiveLine,"/disconnect")){
                            printToClients("Client disconnected",clients,numberOfClients,"server",currentFd);
                            clients[currentFd].connected = 0;
                            numberOfClients--;
                        }
                        printToClients(receiveLine, clients, numberOfClients,clients->name, currentFd);
                    }
                    else{
                        //read 0 client is disconnecting
                        printToClients("Client disconnected",clients,numberOfClients,"server",currentFd);
                        clients[currentFd].connected = 0;
                        numberOfClients--;
                    }

                }

                break;
                //read current fd

            }//if

        }//for
    }//while
}

void printToClients(char message[],Client * clientList, int numOfClients, char name[], int notToSend){
    int curClient;

    for(curClient = 0; curClient < numOfClients; curClient++){
        if(curClient != -1 && curClient != notToSend)write(clientList[curClient].sockedfd,message,strlen(message));
    }
};

void closeServer(Client * clientlist, int numOfClients, int listenFd){
    int currentFd;
    char message[] = "The server is shutting down. You are being removed.";
    for(currentFd = 0; currentFd < numOfClients; currentFd++){
        write(clientlist[currentFd].sockedfd,message,strlen(message));
        close(clientlist[currentFd].sockedfd);
        bzero(clientlist[currentFd].name,20);
        clientlist[currentFd].connected = 0;
        clientlist[currentFd].sockedfd = 0;
    }
    close(0);
    close(listenFd);
    _exit(1);
}