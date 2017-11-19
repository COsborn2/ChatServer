#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>





int sendFile(char* initStr, int sd);
unsigned int fileSize(char* fileName);
int recFile(char* initStr, int sd);
