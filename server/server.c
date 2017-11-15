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

struct client {
    int connected;
    int sockedfd;
    char name[8];
    int roomNumber;
};
typedef struct client Client;

void closeServer(Client *clientlist, int numOfClients, int listenFd);
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

    FD_ZERO(&fdsMaster);        //zeros all bits in fdsMaster so none are listened to
    FD_SET(0, &fdsMaster);       //sets fdsMaster[0] to 1 so it listens to the kb
    FD_SET(listenFd, &fdsMaster);    //adds socket fd
    fdsCopy = fdsMaster;        //bitwise copy


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
                                printf("%s\n", temp.name);
                                printf("%s\n", temp.name);                      //why are there 2 print statements?
                                temp.connected = 1;
                                FD_SET(newClient.sockedfd, &fdsMaster);
                                numberOfClients++;
                                write(newClient.sockedfd, "ok", 2);

                                int curClient;
                                char msg[] = newClient.name + " connected";
                                for (curClient = 0; curClient < numOfClients; curClient++) {
                                    /*the first client in the array that is connected gets replaced*/
                                    if(clients[curClient].connected != 1)
                                        clients[curClient] = newClient;
                                    /*print current client has connected to other clients*/
                                    if(curClient != temp.sockedfd)
                                        write(clientList[curClient].sockedfd, msg, strlen(msg));
                                }
                                printToClients("connected", clients, numberOfClients, "", -1);
                                printf("fd %d connected\n", currentFd);
                                //print to all clients that someone connected
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

                    if (strcmp(sendLine, "/quit")) {
                        /*if the server types '/quit' it removes all clients and closes socket*/
                        closeServer(clients, numberOfClients, listenFd);
                    }
                    else {
                        /*else print servers message*/
                        int curClient;

                        for (curClient = 0; curClient < numOfClients; curClient++) {
                            /*print server message to clients*/
                            write(clientList[curClient].sockedfd, sendLine, strlen(sendLine));
                        }
                    }
                }//end sever keyboard


                else if (currentFd > 0) {
                    /*The current fd is a client sending a message*/
                    if (read(currentFd, receiveLine, MAX)) {
                        /*if the client sends atleast 1 byte*/

                        ////////////////////////////////////////////////
                        /**use receiveLine to check if **/
                        if (strcmp(receiveLine, "/disconnect")) {
                            /*if client sends '/disconnect' then close its connection*/
                            printToClients("Client disconnected", clients, numberOfClients, "server", currentFd);
                            clients[currentFd].connected = 0;
                            numberOfClients--;
                        }
                        ////////////////////////////////////////////////



                        int curClient;

                        for (curClient = 0; curClient < numOfClients; curClient++) {
                            /*print current clients message to clients*/
                            if(curClient != currentFd)
                                write(clientList[curClient].sockedfd, receiveLine, strlen(receiveLine));
                        }

                    }
                    else {
                        /*current client did not send data, and is disconnecting*/
                        printToClients("Client disconnected", clients, numberOfClients, "server", currentFd);
                        clients[currentFd].connected = 0;
                        numberOfClients--;
                    }

                }

                break;
                //read current fd

            }//if current fd fired select

        }//for
    }//while
}


void closeServer(Client *clientlist, int numOfClients, int listenFd) {
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

int uniqueName(char userName[]) {
    return 1;
}
