#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int sendFile(char* fileName, int sd);
unsigned int fileSize(char* fileName);
int recFile(int sd);
