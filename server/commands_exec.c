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
 /*
 * Opens a private chat between cur and specified user
 */
void executePChat(const int cur, Client *clients, Message *message) {

    //Client already in priv chat
    if(clients[cur].privChat >= 0){
        dprintf(clients[cur].sockedfd, "\nServer: You are already in a private chat, "
                "please end the private\n chat using '/q' before connecting to a new client.\n");
        return;
    }


    //increment past '/r '
	char* temp;
	strcpy(temp, message->data);
	if(temp[2] == ' '){temp = temp +3;}
	else{temp = temp + 2;}




	//sets privChat to 1 in both client, adds index in client for priv_chats array
	int x = 0;
	for(; x < MAXCLIENTS; x++){
		if(strcmp(temp, clients[x].name) == 0){
            if(clients[x].privChat >= 0){
                dprintf(clients[cur].sockedfd, "\nServer: User %s is already in a private chat.\n", clients[x].name);
                return;
            }
			clients[x].privChat = clients[cur].sockedfd;
			clients[cur].privChat = clients[x].sockedfd;
			return;
		}
	}
	//failed to create private chat
	dprintf(clients[cur].sockedfd, "\nServer: Could not find user with that name.\n");
}

void executeEndPChat(const int cur, Client *clients) {

	//not in a priv chat
	if(clients[cur].privChat < 0){
		dprintf(clients[cur].sockedfd, "Server: You are not connected to a private chat. \n");
		return;
	}

	//sets privChat to 0 in both client, sets inUse to 0 in priv_Chats[]
	int x = 0;
	int cliOther = clients[cur].privChat;

	for(; x < MAXCLIENTS; x++){
		//remove clients from priv chat
		if(clients[x].sockedfd == cliOther) {
			clients[x].privChat = -1;
			clients[cur].privChat = -1;

			//inform both clients
			dprintf(clients[x].sockedfd, "\nServer: Disconnected from private chat with %s.\n",clients[cur].name );
			dprintf(clients[cur].sockedfd, "\nServer: Disconnected from private chat with %s.\n",clients[x].name );
            return;
		}
	}
	//failed to quit private chat
	dprintf(clients[cur].sockedfd, "\nServer: Could not find user with that name.\n");
}
