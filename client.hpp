#ifndef SINKTHESHIPCLIENT_HPP
#define SINKTHESHIPCLIENT_HPP

#include <sinkTheShip.hpp>
#include <user.hpp>

#include <iostream>

using namespace std;


// Manages a possible error received from server
// @return whether it was an actual error
bool manageError(string buffer);

// Manages a +Ok. received from server that is not 
// a game event
// @return whether the game has begun
bool manageNonGameOk(string buffer);


#endif