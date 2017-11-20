#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "server.h"

/*
 * Clears all data from a message's buffer
 */
void clearMessage(Message *message) {
	memset(message, 0, sizeof(Message));
}

/*
 * Updates the message's data safely
 */
void updateMessage(Message *message, const char *nstring) {
	strncpy(message->data, nstring, MAX);
}

/*
 * Sends the message to the given fd
 */
void writeMessage(int commfd, Message *message) {
	write(commfd, (char *)message, strlen(message->data));
}

/*
 * Reads a message from the given fd
 */
int readMessage(int commfd, Message *message) {
	return read(commfd, (char *)message, MAX);
}

/*
 * Updates the message and sends it to the given fd
 */
void updateAndWriteMessage(int commfd, Message *message, const char *nstring) {
	bzero(message->data, MAX);
	updateMessage(message, nstring);
	printf("updateandwritemessage(): commfd: %d\n", commfd);
	write(commfd, message->data, strlen(message->data));
}
