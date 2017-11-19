#include "commands.h"
#include "server.h"

/*
 * Sends the client the available rooms
 */
void executeRoomList(const int cur, const Client *clients, Message *message) {
	int i = 0;
	for (; i < default_room_count; ++i) {
		sprintf(message->data, "%s, %d", def_rooms[i]->name, def_rooms[i]->id);
		writeMessage(clients[cur].sockedfd, message);
	}
}

/*
 * Sends the client the "help" text
 */
void executeHelp(const int cur, const Client *clients, Message *message) {
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_1);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_2);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_3);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_4);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_5);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_6);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_7);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_8);
	updateAndWriteMessage(clients[cur].sockedfd, message, LANG_HELP_9);
}

/*
 * /r
 * Iterates through all the clients and prints all clients
 */
void executeClientList(const int cur, const Client * clients, Message * message){
    int i = 0;
    for(; clients[i] != '\0'; i++){
        writeMessage(clients[cur].sockedfd, clients[i].name);
    }
}

/*
 * /q
 * Remove client from whatever room they are currently in.
 */
void executeExitRoom(const int cur, const Client * clients, Message * message){
    clients[cur].roomNumber = -1;
}

/*
 * /j roomName
 *
 */
void joinRoom(const int cur, const Client * clients, Message * message){
    char temp[strlen(message->data)];
    int startIndex;
    int endIndex;

    strncpy(temp+startIndex, message->data, endIndex-startIndex);

    int i = 0;
    for(; i[def_rooms] != '\0'; i++){
        if(def_rooms[i]->name == temp)
            clients[cur].roomNumber = def_rooms[i]->id;
    }
}

/*
 * /s name
 *
 */
void setClientName(const int cur, const Client * clients, Message * message){
    
}
