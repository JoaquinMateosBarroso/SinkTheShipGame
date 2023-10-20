#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <sinkTheShip.hpp>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <sstream>



class GameManager {
public:

    GameManager() {
        _waitingGame = -1;
    }

    /**
     * @brief Starts a game
     * @return -1 if game could not be reserved, otherwise returns the game id (position in the vector)
    */
    int startGame(const int socket, const std::string &username);


    SinkTheShipServer& getGame(const int id) {
        return _games[id];
    }
    static GameManager* getInstance();

    int _waitingGame;
private:
    SinkTheShipServer _games[10];
    static GameManager* instance;  

    int lookForGame(const int socket, const std::string &username);
};



#endif