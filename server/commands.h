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

int isCommand(const char * toParse);
int isValidCommand(const char * toParse);
void makeLowerCase(char * toParse);
void executeCommand(const char * toParse, const int cur, Client *clients, Message *message);
void removeSpaces(char * toParse);
void strip(char * toParse);


void executeRoomList(const int cur, const Client *clients, Message *message); // "/r"
void executeClientList(const int cur, const Client * clients, Message * message); // "/r"
void executeExitRoom(const int cur, const Client * clients, Message * message); // "/q"
void joinRoom(const int cur, const Client * clients, Message * message); // "j roomName"
void executeHelp(const int cur, const Client *clients, Message *message); // "/h"
void setClientName(const int cur, const Client * clients, Message * message, char * toParse); // "/s name"


void executeRoomList(const int cur, const Client *clients, Message *message);
void executeHelp(const int cur, const Client *clients, Message *message);
void executePChat(const int cur, Client *clients, Message *message);
void executeEndPChat(const int cur, Client *clients);

#endif
