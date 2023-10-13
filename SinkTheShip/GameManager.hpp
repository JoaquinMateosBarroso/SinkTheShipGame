#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <sinkTheShip.hpp>
#include <sinkTheShip.cpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <sstream>



class GameManager {
public:
    GameManager() { }
    /**
     * @brief Starts a game
     * @return -1 if game could not be started, otherwise returns the game id (position in the vector)
    */
    int startGame(const int socket, const std::string &username);

private:
    SinkTheShipServer _games[10];

};



#endif