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
 * /l
 * Iterates through all the clients and prints all clients in current room
 */
void executeClientList(const int cur, const Client * clients, Message * message){
    int i = 0;
    for(; clients[i] != '\0' && clients[i].roomNumber == clients[cur].roomNumber; i++){
        updateAndWriteMessage(clients[cur].sockedfd, message,clients[i].name);
    }
}

/*
 * /j roomName
 * Puts user in a room
 */
void executeJoinRoom(const int cur, const Client * clients, Message * message, char * toParse){
    int roomExists = 0;

    int i = 0;
    for(; def_rooms[i] != '\0'; i++){
        if(def_rooms[i]->name == toParse) {
            clients[cur].roomNumber = def_rooms[i]->id;
            roomExists = 1;
        }
    }
    if(roomExists == 0){
        updateAndWriteMessage(clients[cur].sockedfd, message, NO_SUCH_ROOM);
    }
}

/*
 * /s name
 * Takes prompt for username. If name is already taken tells user and exits
 */
void setClientName(const int cur, const Client * clients, Message * message, char * toParse){
    int taken = 0;

    //make sure client name is not taken
    int i = 0;
    for(; clients[i] != '\0'; i++){
        if(toParse == clients[i].name)
            taken = 1;
    }

    if(taken != 1){
        strcpy(clients[cur].name, toParse);
    }else{ //name taken, retry
        updateAndWriteMessage(clients[cur].sockedfd, message, NAME_TAKEN);
    }
}
