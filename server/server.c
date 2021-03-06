
#include "server.h"

/*
 * Changed the server loop a lot to account for new requirements (/s command for name changing instead of on join, etc.)
 * All the name checking code is missing, since it will need to be put with the rest of the commands
 * This part should be done, just needs the code added in case all of the client slots are full
 */
 
fd_set masterList;

int main() {
	char defaultName[] = DEFAULT_CLIENT_NAME;
	Client clients[MAXCLIENTS];
	memset(clients, 0, MAXCLIENTS * sizeof(Client));
	Message recMessage;
	Message sendMessage;

	int listenFd, maxfd;
	struct sockaddr_in svaddr;
	
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htons(INADDR_ANY);
	svaddr.sin_port = htons(SERVER_PORT);
	
	bind(listenFd, (struct sockaddr *)&svaddr, sizeof(svaddr));

	listen(listenFd, 10);
	maxfd = listenFd;
	
	fd_set cmpl;
	FD_ZERO(&masterList);
	FD_SET(listenFd, &masterList);

	printf("The server is listening on port:%d\n", SERVER_PORT);//debug
	while (1) {
		cmpl = masterList;
		int toRead = select(maxfd + 1, &cmpl, NULL, NULL, NULL);

        /*adds a new client that is connecting*/
        if (FD_ISSET(listenFd, &cmpl)) {
            int commfd = accept(listenFd, NULL, NULL);
            int i = 0;
            for (; i < MAXCLIENTS; ++i) {
                /* checking for client not in use */
                if (clients[i].connected == 0) {
                    clients[i].connected = 1;
                    clients[i].sockedfd = commfd;
                    clients[i].roomNumber = roomStarting.id;
                    clients[i].privChat = -1;
                    strncpy(clients[i].name, defaultName, MAX_NAME);
                    updateAndWriteMessage(commfd, &sendMessage, "ok");
                    completeNameHS(i, clients, &recMessage);
                    printf("client %s connected on socket descriptor %d\n",clients[i].name,commfd);//debug
                    break;
                }
            }
            FD_SET(commfd, &masterList);

            updateAndWriteMessage(commfd, &sendMessage, LANG_WELCOME);
            if (commfd > maxfd)
                maxfd = commfd;

            continue;
        }


		/* Loops through all clients to see if any have data to be read */
		int c = 0;
		for (; c < MAXCLIENTS; ++c) {
			if (clients[c].connected) {
				if (FD_ISSET(clients[c].sockedfd, &cmpl)) {
					/* Resets message data before being read (bzero) */
					clearMessage(&recMessage);
					clearMessage(&sendMessage);
					
					int n = readMessage(clients[c].sockedfd, &recMessage);
					strip(recMessage.data);
					/* client is disconnecting */	
					if (n == 0) {
						disconnectClient(c, clients, &sendMessage);
						continue;
					}
					if (isCommand(recMessage.data)) {
						/* Fixes the command before parsing */
						removeSpaces(recMessage.data);
						/* Sets the command to lowercase before processing it (not everything should be lower, as otherwise names will now not allow capitals) */
						if (strlen(recMessage.data) > 1)
							recMessage.data[1] = (char) tolower(recMessage.data[1]);

						if (isValidCommand(recMessage.data)) {
							executeCommand(recMessage.data, c, clients, &sendMessage);
						}
						else {
							updateAndWriteMessage(clients[c].sockedfd, &sendMessage, LANG_BAD_COMMAND);
						}
					}
					else {
						/* If not command, they talked and we should send that to others in the room */
						if (clients[c].roomNumber != roomStarting.id) {
							//if in priv chat, send only to other client
							if(clients[c].privChat >= 0) {
								snprintf(sendMessage.data, MAX, "%s: %s", clients[c].name, recMessage.data);
								printf("private chat from sd %d to sd %d: %s\n", c, clients->privChat ,sendMessage.data);//debug
								writeMessage(clients[c].privChat, &sendMessage);
							}
							else{
								snprintf(sendMessage.data, MAX, "%s: %s", clients[c].name, recMessage.data);
								printf("%s\n",sendMessage.data);//debug
								printToOthersInRoom(clients, c, &sendMessage);
							}
						}
						else {
                            if (clients[c].privChat >= 0) {
                                snprintf(sendMessage.data, MAX, "%s: %s", clients[c].name, recMessage.data);
                                printf("private chat from sd %d to sd %d: %s\n", c, clients->privChat,
                                       sendMessage.data);//debug
                                writeMessage(clients[c].privChat, &sendMessage);
                            }
                            else {
                                updateAndWriteMessage(clients[c].sockedfd, &sendMessage, LANG_NO_TALK);
                                updateAndWriteMessage(clients[c].sockedfd, &sendMessage, LANG_CHOOSE_ROOM);
                                printf("%s\n", sendMessage.data);//debug
                            }
                        }
					}
				}
                //break;
                //Dont break, continue checking other clients
			}

		}

	}
	
	return 0;
}

void printToAll(const Client *clients, Message *message) {
	int i = 0;
	for (; i < MAXCLIENTS; ++i)
		writeMessage(clients[i].sockedfd, message);
}

void printToOthersInRoom(const Client *clients, int cur, Message *message) {
	int i = 0;
	for (; i < MAXCLIENTS; ++i) {
		/* Client must be in same room, and not be the current person */
		if (i != cur &&clients[i].roomNumber == clients[cur].roomNumber && clients[i].roomNumber != -1 && clients[i].connected == 1)
			writeMessage(clients[i].sockedfd, message);
	}
}

/*
 * Disconnects the current client, and tells everyone in the room
 */
void disconnectClient(int cur, Client *clients, Message *sendMessage) {
    /*if client is in private chat, it removes the other person, and notifies them they are public*/
    if(clients[cur].privChat >= 0){
        int i = 0;
        for(; i < MAXCLIENTS; i++){
            if(clients[i].privChat == clients[cur].sockedfd){
                clients[i].privChat = -1;

                updateAndWriteMessage(clients[i].sockedfd ,sendMessage, LANG_PRIVATE_CHAT);
                break;
            }
        }
    }

    /*closes client*/
    printf("disconnectClient(): client %s disconnecting\n",clients[cur].name);//debug
	clients[cur].connected = 0;
	FD_CLR(clients[cur].sockedfd, &masterList);
	sprintf(sendMessage->data, LANG_DISCONNECT, clients[cur].name);
	printToOthersInRoom(clients, cur, sendMessage);
    clients[cur].roomNumber= -1;
    strcpy(clients[cur].name, DEFAULT_CLIENT_NAME); //set client name back to default
}

//Threaded method to confirm name
void * completeNameHS(const int cur, Client * clients, Message* msg){

	do{
		read(clients[cur].sockedfd, msg->data, MAX);
	}while(!setClientName(cur, clients, msg->data));
}
