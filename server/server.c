#include <stdio.h>
#include <netinet/in.h>
#include <memory.h>

#include <unistd.h>

#include "server.h"

void closeServer(Client *clientlist, int numOfClients, int listenFd);
void printToClients(char message[],Client * roomClients, int sendingFD);
int uniqueName(char userName[]);

int main() {
    char receiveLine[MAX];
    char sendLine[MAX];
    int serverFd, listenFd;
    struct sockaddr_in servaddr;

    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
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

    FD_ZERO(&fdsMaster);             //zeros all bits in fdsMaster so none are listened to
    FD_SET(0, &fdsMaster);           //sets fdsMaster[0] to 1 so it listens to the kb
    FD_SET(listenFd, &fdsMaster);    //adds socket fd
    fdsCopy = fdsMaster;             //bitwise copy


    while (1) {
        fdsCopy = fdsMaster;
        select(FD_SETSIZE, &fdsCopy, NULL, NULL, NULL);                       //dont care about writefds or exceptfds, last is to blobk indefinitly. returns 1 if something is ready

        for (currentFd = 0; currentFd < FD_SETSIZE; currentFd++) {
            if (FD_ISSET(currentFd, &fdsCopy)) {
                /*if current fd in the fd set is ready to be read*/

                if (currentFd == listenFd) {
                    /*if current fd is the listenFd then its a pending connection*/
                    Client newClient;
                    newClient.sockedfd = accept(listenFd, (struct sockaddr *) NULL, NULL);                                         //do i pass struct pointer?


                    if (numberOfClients < MAXCLIENTS) {
                        /*if there is room to add another client*/
                        write(newClient.sockedfd, "ok", 2);

                        ////////////////////////////////////////////////
                        /**This entire while loop should be another thread so server doesnt hang**/
                        while (1) {
                            /*while the name is not unique, reprompt*/
                            bzero(receiveLine, MAX);
                            read(newClient.sockedfd, &receiveLine, 8);
                            if (uniqueName(receiveLine)) {
                                /*add new client*/
                                strcpy(newClient.name, receiveLine);
                                printf("%s\n", newClient.name);
                                newClient.connected = 1;
                                FD_SET(newClient.sockedfd, &fdsMaster);
                                numberOfClients++;
                                write(newClient.sockedfd, "ok", 2);

                                //print to all clients that someone connected
                                char msg[25];
                                strcat(msg, newClient.name);
                                strcat(msg," connected");
                                printf("fd %d connected\n", currentFd);
                                printToClients(msg, clients, newClient.sockedfd);


                                /**this should be an addClient method that adds client to the wait room **/
                                int curClient;
                                for (curClient = 0; curClient < MAXCLIENTS; curClient++) {
                                    /*the first client in the array that is not connected gets replaced*/
                                    if (clients[curClient].connected != 1) {
                                        clients[curClient] = newClient;
                                        numberOfClients++;
                                        break;
                                    }

                                }

                                break;

                            } else {
                                /*reject clients name*/
                                write(newClient.sockedfd, "no", 2);
                            }
                        }//end unique name loop
                        ////////////////////////////////////////////////

                    }//end add new client

                    else {
                        /*else there is not room, so add client to waitlist*/
                        char message[100] = "There are already ";
                        strcat(message, MAXCLIENTS);
                        strcat(message," people. You are being added to the waitlist.");

                        /*   waitlist   */
                        write(newClient.sockedfd, message, strlen(message));

                        newClient.connected = 1;
                        int i;
                        for(i = 0; i < MAXWAITLIST; i++){
                            if(waitlist[i].connected = -1)
                                waitlist[i] = newClient;
                        }

                    }//end add to waitlist
                    break;
                }//end new connection


                else if (currentFd == 0) {
                    /*if current fd == 0, it is the keyboard on server*/
                    read(0, &sendLine, MAX);

                    if (strcmp(sendLine, "/quit") == 0) {
                        /*if the server types '/quit' it removes all clients and closes socket*/
                        closeServer(clients, numberOfClients, listenFd);
                    } else {
                        /*else print servers message*/
                        printf("Server  : %s",sendLine);
                        printToClients(sendLine, clients, 0);
                    }//end sever keyboard
                }


                else if (currentFd > 0) {
                    /*The current fd is a client sending a message*/
                    if (read(currentFd, receiveLine, MAX)) {
                        /*if the client sends atleast 1 byte*/

                        ////////////////////////////////////////////////
                        /**use receiveLine to check if is command**/

                        if (strcmp(receiveLine, "/disconnect")) {
                            /*if client sends '/disconnect' then close its connection*/
                            //printToClients("Client disconnected", clients, numberOfClients, "server", currentFd);
                            clients[currentFd].connected = 0;
                            numberOfClients--;
                        }
                        ////////////////////////////////////////////////
                        else
                            printToClients(receiveLine,clients,currentFd);

                    }
                    else {
                        /*current client did not send data, and is disconnecting*/
                        char msg[50];
                        strcat(msg,"Client ");
                        strcat(msg, currentFd);
                        strcat(msg," disconnected");
                        printToClients(msg, clients, currentFd);
                        //clients[currentFd].connected = 0; need to find client with this fd
                        numberOfClients--;
                    }

                }

                break;
                //read current fd

            }//if current fd fired select

        }//for
    }//while
}



/**
 * writes a message to all clients in the room
 * @param message message The message to be sent
 * @param roomClients The list of people in the room
 * @param sendingFD The file descriptor that is sending, and should not be sent to
 */
void printToClients(char message[],Client * roomClients, int sendingFD){

    int curClient;
    char name[8] = "Server ";
    for(curClient = 0; curClient < MAXCLIENTS; curClient++){
        if(roomClients[curClient].sockedfd == sendingFD) {
            strcpy(name,roomClients[curClient].name);
            break;
        }
    }
    char toSend[strlen(message)+8];
    strcat(toSend,message);
    strcat(toSend,name);

    for(curClient = 0; curClient < MAXCLIENTS; curClient++){
        if(roomClients[curClient].sockedfd != sendingFD)
            write(roomClients[curClient].sockedfd,toSend,strlen(toSend));
    }
};

/**
 * Closes the socket connections, and exits the program
 * @param clientlist The list of clients to disconnect
 * @param numOfClients The number of clients in the list
 * @param listenFd The servers file descriptor to close
 */
void closeServer(Client *clientlist, int numOfClients, int listenFd) {          //needs updated
    int currentFd;
    char message[] = "The server is shutting down. You are being removed.";
    for (currentFd = 0; currentFd < numOfClients; currentFd++) {
        write(clientlist[currentFd].sockedfd, message, strlen(message));
        close(clientlist[currentFd].sockedfd);
        bzero(clientlist[currentFd].name, 20);
        clientlist[currentFd].connected = 0;
        clientlist[currentFd].sockedfd = 0;
    }
    close(0);
    close(listenFd);
    _exit(1);
}

/**
 * Determines if a name is unique
 * @param userName The name to check uniqueness
 * @return 1 if the name is not already on server
 */
int uniqueName(char userName[]) {
    return 1;
}
