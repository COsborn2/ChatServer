#ifndef commands_h
#define commands_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#include "server.h"

typedef struct client Client;
typedef struct message Message;

int isCommand(char * toParse);
int isValidCommand(char * toParse);
void makeLowerCase(char * toParse);
void executeCommand(char * toParse, const int cur, Client *clients, Message *message);
void removeSpaces(char * toParse);
void strip(char * toParse);


void executeRoomList(const int cur, const Client *clients, Message *message); // "/r"
void executeClientList(const int cur, const Client * clients, Message * message); // "/l"
void executeJoinRoom(const int cur, Client * clients, char * toParse); // "j roomName"
void executeHelp(const int cur, const Client *clients, Message *message); // "/h"
int setClientName(const int cur, Client * clients, char * toParse); // "/s name"
void executePChat(const int cur, Client *clients, Message *message); // "/p name"
void executeEndPChat(const int cur, Client *clients); // "/q"

#endif
