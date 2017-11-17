#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "commands.h"
#include "lang.h"
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
	
	unsigned int listenFd, maxfd;
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
	
	while (1) {
		cmpl = masterList;
		int toRead = select(maxfd + 1, &cmpl, NULL, NULL, NULL);
		
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
					}
					if (isCommand(recMessage.data)) {
						/* Fixes the command before parsing */
						removeSpaces(recMessage.data);
						/* Sets the command to lowercase before processing it (not everything should be lower, as otherwise names will now not allow capitals) */
						if (strlen(recMessage.data) > 1)
							recMessage.data[1] = tolower(recMessage.data[1]);
						
						if (isValidCommand(recMessage.data)) {
							executeCommand(recMessage.data, c, clients, &sendMessage);
						}
						else {
							updateAndWriteMessage(clients[c].sockedfd, &sendMessage, LANG_BAD_COMMAND);
						}
					}
					else {
						/* If not command, they talked and we should send that to others in the room */
						if (clients[c].roomId != roomStarting.id) {
							snprintf(sendMessage.data, MAX, "%s: %s", clients[c].name, recMessage.data);
							printToOthersInRoom(clients, c, &sendMessage);
						}
						else {
							updateAndWriteMessage(clients[c].sockedfd, &sendMessage, LANG_NO_TALK);
						}
					}
				}
			}
		}
		
		/* Doesn't bother to continue if nothing else to read */
		if (toRead <= 0)
			continue;
		
		if (FD_ISSET(listenFd, &cmpl)) {
			unsigned int commfd = accept(listenFd, NULL, NULL);
			int i = 0;
			for (; i < MAXCLIENTS; ++i) {
				/* checking for client not in use */
				if (clients[i].connected == 0) {
					clients[i].connected = 1;
					clients[i].sockedfd = commfd;
					clients[i].roomNumber = roomStarting.id;
					strncpy(clients[i].name, defaultName, MAX_NAME);
					break;
				}
			}
			FD_SET(commfd, &masterList);
			
			updateAndWriteMessage(commfd, &sendMessage, LANG_WELCOME);
			if (commfd > maxfd)
				maxfd = commfd;
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
		if (i != cur &&clients[i].roomNumber == clients[cur].roomNumber)
			writeMessage(clients[i].sockedfd, message);
	}
}

/*
 * Disconnects the current client, and tells everyone in the room
 */
void disconnectClient(int cur, Client *clients, Message *sendMessage) {
	close(clients[cur].sockedfd);
	clients[cur].connected = 0;
	FD_CLR(clients[cur].sockedfd, &masterList);
	sprintf(sendMessage->data, LANG_DISCONNECT, clients[cur].name);
	printToOthersInRoom(clients, cur, sendMessage);
}
