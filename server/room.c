#include "server.h"

/*
 * An array of all rooms that people can choose to join
 * Must be extern to prevent getting included more than once
 */
const Room *def_rooms[2] = {&roomHobby, &roomNetwork};
