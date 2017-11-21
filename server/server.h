#ifndef SERVER_H
#define SERVER_H

#include <sys/select.h>
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
#include <pthread.h>

#define MAXCLIENTS 30
#define MAX 100
#define MAX_ROOM_SIZE 3

#define MAX_NAME 8
#define MAX_ROOM_NAME_LENGTH 16
#define DEFAULT_CLIENT_NAME "DEFAULT"
#define SERVER_PORT 22000

typedef struct client {
    int connected;
    int sockedfd;
    char name[MAX_NAME];
    int roomNumber;
	int privChat;
} Client;


typedef struct room {
	int id;
	char name[MAX_ROOM_NAME_LENGTH];
} Room;

typedef struct message {
	char data[MAX];
} Message;

extern const Room roomStarting;
extern const Room roomHobby;
extern const Room roomNetwork;
extern const int default_room_count;
extern const Room *def_rooms[2];


void clearMessage(Message *message);
void updateMessage(Message *message, const char *nstring);
void writeMessage(int commfd, Message *message);
int readMessage(int commfd, Message *message);
void updateAndWriteMessage(int commfd, Message *message, const char *nstring);

void printToAll(const Client *clients, Message *message);
void printToOthersInRoom(const Client *clients, int cur, Message *message);
void printToPrivChat(Client *clients, int cur,  Message * message);
void disconnectClient(int cur, Client *clients, Message *sendMessage);
void * completeNameHS(int sd, Client * clients, Message* msg);

#endif
