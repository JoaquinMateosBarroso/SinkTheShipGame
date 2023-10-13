#include <GameManager.hpp>



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