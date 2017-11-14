#ifndef commands_h
#define commands_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int isCommand(char * toParse);
int isValidCommand(char * toParse);
void makeLowerCase(char * toParse);
void executeCommand(char * toParse);
void removeSpaces(char * toParse);

#endif
