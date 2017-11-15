#ifndef server_h
#define server_h

#define MAXCLIENTS 3
#define MAX 100
#define MAXWAITLIST 5

struct client {
    int connected;
    int sockedfd;
    char name[8];
    int roomNumber;
};
typedef struct client Client;

struct room{
    char roomName[8];
    Client clients[MAXCLIENTS];
};
typedef struct room Room;

#endif
