#ifndef SINKTHESHIPCLIENT_HPP
#define SINKTHESHIPCLIENT_HPP

#include <sinkTheShip.hpp>
#include <user.hpp>

#include <iostream>

using namespace std;

// Manages a possible error received from server
// @return whether it was an actual error
bool manageError(string buffer);

void clearScreen();

#endif