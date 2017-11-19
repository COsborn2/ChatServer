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
            		return strlen(toParse) > 2;
                case 'r' : //list the rooms on the server
                    return strlen(toParse) == 2;
                case 'l' : //lists people in the current room
                    return strlen(toParse) == 2;
                case 'x' : //close the connection and log off the server
                    return strlen(toParse) == 2;
                case 'q' : //end private chat
                    return strlen(toParse) == 2;
                case 'h' : //help
                    return strlen(toParse) == 2;
                case 'p' : //private chat
                    return strlen(toParse) > 2;
                case 'j' : //joins the given room
                    return strlen(toParse) > 2;
                case 'f' : //send file
                    return strlen(toParse) > 2;
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
            	break;
            case 'l' : //lists people in the current room
                //command is valid, do something
                break;
            case 'q' : //end private chat
                executeEndPChat(cur, clients);
                break;
            case 'h' : //help
            	executeHelp(cur, clients, message);
            	break;
            case 'p' : //private chat
                executePChat(cur, clients, message);
                break;
            case 'j' : //joins the given room
                //command is valid, do something
                executeJoinRoom(cur, clients, message, toParse + 2);
                break;
            case 'f' : //send file
                //command is valid, do something
                break;
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
