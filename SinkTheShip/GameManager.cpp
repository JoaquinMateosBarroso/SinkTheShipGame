#include <GameManager.hpp>
#include <cstring>


GameManager* GameManager::instance = nullptr;

int GameManager::startGame(const int socket, const std::string &username)
{
    // 
    if (_waitingGame == -1)
    {
        const char* response = "+Ok. Esperando jugadores";
        send(socket, response, strlen(response), 0);
        return lookForGame(socket, username);
    }
    else if (_games[_waitingGame].addPlayer(socket, username))
    {
        const char* response = "+Ok. Empieza la partida";
        send(socket, response, strlen(response), 0);
        send(_games[_waitingGame].getPlayer(1).socket, response, strlen(response), 0);

        return _waitingGame;
    }
    else
    {
        send(socket, "-Err. This code should not be reachable", MSG_SIZE, 0);
        return -1;
    }
}



int GameManager::lookForGame(const int socket, const std::string &username)
{
    _waitingGame = 0;
    while (_games[_waitingGame].isGameStarted() and _waitingGame < 10)
    {
        _waitingGame++;
    }

    if (_waitingGame == 10)
    {
        _waitingGame = -1;
        send(socket, "-Err. No games avaiable", MSG_SIZE, 0);
        return -1;
    }
    else if (_games[_waitingGame].addPlayer(socket, username))
    {
        return _waitingGame;
    }
    else
        return -1;

}


GameManager* GameManager::getInstance() {
    if (instance == nullptr) {
        instance = new GameManager();
    }
    return instance;
}