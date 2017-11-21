#include "commands.h"

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
    int roomToPrint = clients[cur].roomNumber;
    for(; i < MAXCLIENTS; i++){
        if( clients[i].roomNumber == roomToPrint)
            updateAndWriteMessage(clients[cur].sockedfd, message,clients[i].name);
    }
}


/*
 * /j roomName
 * Puts user in a room
 */
void executeJoinRoom(const int cur, Client * clients, char * toParse){
    Message message;

    /*user cannot join a room with the default name*/
    if(strcmp(clients[cur].name, DEFAULT_CLIENT_NAME) == 0){
        updateAndWriteMessage(clients[cur].sockedfd,&message, LANG_DEFAULT_NAME);
        printf("executeJoinRoom(): clients[%d] tried connecting to %s\n",cur, toParse);//debug
        return;
    }

    int roomIndex = -1, i = 0;

    /*verify if its a valid room*/
    for(; i < default_room_count; i++){
        if(strcmp(def_rooms[i]->name, toParse) == 0) {
            roomIndex = i;
            break;
        }
    }
    if(roomIndex == -1){
        /*room is invalid*/
        updateAndWriteMessage(clients[cur].sockedfd, &message, LANG_NO_SUCH_ROOM);
        printf("executeJoinRoom(): %s tried connecting to %s\n",clients[cur].name, toParse);//debug
    }
    else{

        /*counts the number of people in the room, adds client to the room if not full*/
        int numberOfClients = 0;
        i=0;
        for(; i < MAXCLIENTS; i++){
            if(clients[i].roomNumber == def_rooms[roomIndex]->id)
                numberOfClients++;
        }

        /*either adds client to room, or informs client that the room is full*/
        if(numberOfClients < MAX_ROOM_SIZE) {
            clients[cur].roomNumber = def_rooms[roomIndex]->id;
            snprintf(message.data, MAX, "%s has joined %s", clients[cur].name, def_rooms[roomIndex]->name);
            printf("executeJoinRoom(): %s\n",message.data);//debug
            printToOthersInRoom(clients,cur,&message);

        }
        else {
            updateAndWriteMessage(clients[cur].sockedfd, &message, LANG_ROOM_FULL);
            printf("executeJoinRoom(): %s tried connecting to %s but room is full\n",clients[cur].name, def_rooms[roomIndex]->name);//debug
        }
    }

}


/*
 * /s name
 * Takes prompt for username. If name is already taken tells user and exits
 */
int setClientName(const int cur, Client * clients, char * suggestedName){
    int taken = 0;

    //make sure client name is not taken
    int i = 0;
    for(; i < MAXCLIENTS; i++){
        if(suggestedName == clients[i].name) {
            taken = 1;
            break;
        }
    }
    if(strcmp(suggestedName,DEFAULT_CLIENT_NAME))
        taken = 1;

    Message message;
    if(taken != 1){
        char oldName[MAX_NAME];
        strncpy(oldName,clients[cur].name, MAX_NAME);//debug
        strncpy(clients[cur].name, suggestedName, MAX_NAME);
        printf("setClientName(): client[%d].name changed to %s from %s\n", cur,clients[cur].name,oldName);//debug
        updateAndWriteMessage(clients[cur].sockedfd, &message, "ok");
        return 1;
    }
    else{ //name taken, retry
        updateAndWriteMessage(clients[cur].sockedfd, &message, "no");
        printf("setClientName(): client[%d] suggested name \"%s\" but it was already in use\n", cur, suggestedName);//debug
        return 0;
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


 /*
 * end private chat
 */
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
