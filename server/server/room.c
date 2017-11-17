#include "server.h"

const Room roomStarting = {-1, "The Void"};
const Room roomHobby = {330, "Hobby"};
const Room roomNetwork = {340, "Network"};
const int default_room_count = 2;

/*
 * An array of all rooms that people can choose to join
 * Must be extern to prevent getting included more than once
 */
const Room *def_rooms[2] = {&roomHobby, &roomNetwork};
