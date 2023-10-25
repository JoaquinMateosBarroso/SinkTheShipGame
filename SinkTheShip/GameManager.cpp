#include <GameManager.hpp>
#include <cstring>
#include <Handlers.hpp>


GameManager* GameManager::instance = nullptr;

int GameManager::startGame(const int socket, std::shared_ptr<SocketState> socketState)
{
    // 
    if (_waitingGame == -1)
    {
        const char* response = "+Ok. Esperando jugadores";
        send(socket, response, strlen(response), 0);
        return lookForGame(socket, socketState);
    }
    else if (_games[_waitingGame].addPlayer(socket, socketState))
    {
        _games[_waitingGame].createBoards();
        std::string response1 = "+Ok. Empezamos partida." + _games[_waitingGame].getStringBoard(1);
        std::string response2 = "+Ok. Empezamos partida." + _games[_waitingGame].getStringBoard(2);
        send(_games[_waitingGame].getPlayer(1).socket, response1.c_str(), response1.length(), 0);
        send(socket, response2.c_str(), response2.length(), 0);
        _games[_waitingGame].start();

        int aux = _waitingGame;
        _waitingGame = -1;

        return aux;
    }
    else
    {
        send(socket, "-Err. This code should not be reachable", MSG_SIZE, 0);
        return -1;
    }
}



int GameManager::lookForGame(const int socket, std::shared_ptr<SocketState> socketState)
{
    for (_waitingGame = 0; (_waitingGame < 10) and (!_games[_waitingGame].isGameFree()); _waitingGame++)
    {}

    if (_waitingGame == 10)
    {
        _waitingGame = -1;
        send(socket, "-Err. No hay juegos disponibles", MSG_SIZE, 0);
        return -1;
    }
    else if (_games[_waitingGame].addPlayer(socket, socketState))
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