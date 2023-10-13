#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <sinkTheShip.hpp>


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


int GameManager::startGame(const int socket, const std::string &username)
{
    // TODO
    int thisGame = 0;
    while (_games[thisGame].isGameStarted() and thisGame < 10)
    {
        thisGame++;
    }

    if (thisGame == 10)
    {
        send(socket, "-Err. No games avaiable", MSG_SIZE, 0);
        return -1;
    }
    else if (_games[thisGame].addPlayer(socket, username))
        return thisGame;
    else
        return -1;

}


#endif