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
int isCommand(char *toParse) {
    if (toParse != NULL) {
        if (strlen(toParse) >= 1) {
            /* removeSpaces(toParse); Removed, since this gets called on all incoming text, which would prevent all spaces in talking */
            if (toParse[0] == '/')
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
int isValidCommand(char *toParse) {
    if (toParse != NULL) {
        if (toParse[0] == '/' && strlen(toParse) >= 2) { //starts with command character. Definitely not a chat message

            char * temporaryS = (char *)malloc(strlen(toParse) + 1);
            strcpy(temporaryS, toParse);
            makeLowerCase(temporaryS);
            char toTest = temporaryS[1];

            printf("In isValidCommand(). toParse: %s\n", toParse);
            printf("In isValidCommand(). toTest: %c\n", toTest);

            switch (toTest) {
                case 's' : //sets name
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) > 2;
                case 'r' : //list the rooms on the server
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) == 2;
                case 'l' : //lists people in the current room
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) == 2;
                case 'x' : //close the connection and log off the server
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) == 2;
                case 'q' : //end private chat
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) == 2;
                case 'h' : //help
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) == 2;
                case 'p' : //private chat
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) > 2;
                case 'j' : //joins the given room
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) > 2;
                case 'f' : //send file
                    free(temporaryS);
                    temporaryS = NULL;
                    return strlen(toParse) > 2;
                default : //doesn't match any command
                    free(temporaryS);
                    temporaryS = NULL;
                    return 0;
            }
        }
    }
    return 0;
}

/*
 * Take in validated String with a command and run associated action
 */
void executeCommand(char *toParse, const int cur, Client *clients, Message *message) {
    char * temporaryS = (char *)malloc(strlen(toParse) + 1);
    strcpy(temporaryS, toParse);
    makeLowerCase(temporaryS);
    char toTest = temporaryS[1];

    printf("In executeCommand(). toParse: %s\n", toParse);
    printf("In executeCommand(). toTest: %c\n", toTest);

    switch (toTest) {
        case 'x' : //disconnects
            disconnectClient(cur, clients, message);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 's' : //sets name
            setClientName(cur, clients, toParse + 2);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'r' : //list the rooms on the server
            executeRoomList(cur, clients, message);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'l' : //lists people in the current room
            executeClientList(cur, clients, message);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'q' : //end private chat
            executeEndPChat(cur, clients);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'h' : //help
            executeHelp(cur, clients, message);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'p' : //private chat
            executePChat(cur, clients, message);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'j' : //joins the given room
            executeJoinRoom(cur, clients, toParse + 2);
            free(temporaryS);
            temporaryS = NULL;
            break;
        case 'f' : //send file
            //command is valid, do something
            free(temporaryS);
            temporaryS = NULL;
            break;
        default : //doesn't match any command
            free(temporaryS);
            temporaryS = NULL;
            break;
    }

}

/*
 * Take incoming String and convert it to lower case
 */
void makeLowerCase(char *toParse) {
    if (strlen(toParse) != 0) {
        unsigned int i;
        for (i = 0; i < strlen(toParse); i++) {
            //toParse[i] =
            tolower(toParse[i]);
        }
    }
}

/*
 * Take in a char array and remove spaces
 */
void removeSpaces(char *toParse) {
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
void strip(char *toParse) {
    if (toParse != NULL) {
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
