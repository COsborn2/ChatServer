#include "commands.h"
#include "lang.h"

/*
 * int isCommand(char * toParse) -> Return 1 if true, 0 if false.
 * int isValidCommand(char * toParse) -> Takes command as string and checks if it is valid
 * void executeCommand(char * toParse) -> Run associated command
 * void makeLowerCase(char * toParse) -> Replace elements with lowercase equivalent
 * void removeSpaces(char * toParse) -> Remove spaces from passed in char array
 * void strip(char * toParse) -> Takes in string and removes newline and tab characters
 *
 *
 * “/r” - list the rooms on the server
 * “/l” - lists people in the current room
 * “/x” - close the connection and log off the server
 * “/q” - end private chat
 * “/h” - help
 * “/p name"      - private chat
 * “/j roomname ” - joins the given room
 * “/f filename"  - send file
 * "/s name" - Sets client name
 */

/*
 * Check if first character in array is /. If it is, the string is a command, not a chat message
 * Returns 0 if false. 1 if true
 */
int isCommand(const char * toParse){
    if(toParse != NULL){
        if(strlen(toParse) >= 1){
            /* removeSpaces(toParse); Removed, since this gets called on all incoming text, which would prevent all spaces in talking */
            if(toParse[0] == '/')
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

/* return 1 if the command is VALID. Returns 0 if the command is NOT valid
 * Take command in form: /j testRoom
 */
int isValidCommand(const char * toParse){
    if(toParse != NULL && strlen(toParse) >= 2){
        if(toParse[0] == '/') { //starts with command character. Definitely not a chat message

            char temporaryS[strlen(toParse)+1];
            strcpy(temporaryS, toParse);
            char toTest = temporaryS[1];

            switch(toTest) {
            	case 's' : //sets name
            		if(strlen(toParse) > 2)
            			return 1;
            		else
            			return 0;
                case 'r' : //list the rooms on the server
                    if(strlen(toParse) == 2)
                        return 1;
                    else
                        return 0;
                case 'l' : //lists people in the current room
                    if(strlen(toParse) == 2)
                        return 1;
                    else
                        return 0;
                case 'x' : //close the connection and log off the server
                    if(strlen(toParse) == 2)
                        return 1;
                    else
                        return 0;
                case 'q' : //end private chat
                    if(strlen(toParse) == 2)
                        return 1;
                    else
                        return 0;
                case 'h' : //help
                    if(strlen(toParse) == 2)
                        return 1;
                    else
                        return 0;
                case 'p' : //private chat
                    if(strlen(toParse) > 2)
                        return 1;
                    else
                        return 0;
                case 'j' : //joins the given room
                    if(strlen(toParse) > 2)
                        return 1;
                    else
                        return 0;
                case 'f' : //send file
                    if(strlen(toParse) > 2)
                        return 1;
                    else
                        return 0;
                default : //doesn't match any command
                    return 0;
            }
        }
    }
    return 0;
}

/*
 * Take in validated String with a command and run associated action
 */
void executeCommand(const char * toParse, const int cur, Client *clients, Message *message) {
    if(isValidCommand(toParse)) {
        char temporaryS[strlen(toParse)+1];
        strcpy(temporaryS, toParse);
        char toTest = temporaryS[1];

        switch (toTest) {
        	case 'x' : //disconnects
        		disconnectClient(cur, clients, message);
        		break;
        	case 's' : //sets name
        		break;
            case 'r' : //list the rooms on the server
            	executeRoomList(cur, clients, message);
            case 'l' : //lists people in the current room
                //command is valid, do something
            case 'q' : //end private chat
                //command is valid, do something
            case 'h' : //help
            	executeHelp(cur, clients, message);
            	break;
            case 'p' : //private chat
                //command is valid, do something
            case 'j' : //joins the given room
                //command is valid, do something
            case 'f' : //send file
                //command is valid, do something
            default : //doesn't match any command
                break;
        }
    }
}

/*
 * Take incoming String and convert it to lower case
 */
void makeLowerCase(char * toParse){
    if(strlen(toParse) != 0) {
        unsigned int i;
        for (i = 0; i < strlen(toParse); i++) {
            toParse[i] = tolower(toParse[i]);
        }
    }
}

/*
 * Take in a char array and remove spaces
 */
void removeSpaces(char * toParse) {
    unsigned int count = 0;
    unsigned int i = 0;

    for (i = 0; i < strlen(toParse); i++) {
        if (toParse[i] != ' ') {
            toParse[count++] = toParse[i];
        }
    }
    toParse[count] = '\0';
}

/*
 * Takes in string and removes newline and tab characters
 */
void strip(char * toParse) {
    if(toParse != NULL) {
        int len = strlen(toParse);
        int i = 0;

        while (toParse[i] != '\0' && i < len) {
            if (toParse[i] == '\r')
                toParse[i] = '\0';

            else if (toParse[i] == '\n')
                toParse[i] = '\0';

            i++;

        }// end while
    }
}// end strip


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