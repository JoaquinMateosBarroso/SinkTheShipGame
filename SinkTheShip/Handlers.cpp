#include <string>
#include <sstream>
#include <fstream>
#include <Handlers.hpp>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <GameManager.hpp>



void Handlers::handleExit(int socket, std::shared_ptr<SocketState> socketState, char* buffer, int bufferSize) {
    GameManager * gameManager = GameManager::getInstance();

    if (socketState -> game != -1)
    {
        SinkTheShipServer game = gameManager -> getGame(socketState -> game);
        if (!game.isGameStarted())
        {
            game._free = true;
            gameManager->_waitingGame = -1;
            game._player1.socket = -1;
        }else {
            game.closeGame(socket);
        }
        
    }
}
int Handlers::letterToColumn(char letter) {
    return letter - 'A';
}

void Handlers::handleShoot(int socket, std::shared_ptr<SocketState> socketState, char* buffer, int bufferSize) {
    if (!socketState->isLogged) {
        const char* response = "-Err. Usuario no logueado";
        send(socket, response, strlen(response), 0);
        return;
    }

    if (socketState->game == -1) {
        const char* response = "-Err. El usuario no está en partida";
        send(socket, response, strlen(response), 0);
        return;
    }

    if (!socketState->isYourTurn) {
        const char* response = "-Err. Debe esperar su turno";
        send(socket, response, strlen(response), 0);
        return;
    }


    char letter = buffer[8];
    int col = letterToColumn(letter);
    cout << "letter" << letter << col << endl; 
    int row = stoi(string(buffer).substr(10))-1;

    if (col > 9 or row > 9 or col < 0 or row < 0)
    {
        const char* response = "-Err. Las coordenadas dadas no son correctas";
        send(socket, response, strlen(response), 0);
        return;
    }
    
    GameManager* gameManager = GameManager::getInstance();

    SinkTheShipServer game = gameManager -> getGame(socketState -> game);

    game.shoot(socket, col, row);
}

void Handlers::handleRegister(int socket, std::shared_ptr<SocketState>socketState, char* buffer, int bufferSize)
{
    std::string sbuffer = buffer;
    sbuffer.erase(sbuffer.size());

    std::string username = getParam(buffer, "-u");
    std::string password = getParam(buffer, "-p");

    if (username == "" || password == "") {
        const char* response = "-Err. Usuario incorrecto";
        send(socket, response, strlen(response), 0);
        return;
    }

    if (isRegistered(username)) {
        const char* response = "-Err. Usuario ya registrado";
        send(socket, response, strlen(response), 0);
        return;
    }

    registerUser(username, password);

    const char* response = "+Ok. Usuario registrado";
    send(socket, response, strlen(response), 0);
}

void Handlers::handleUser(int socket, std::shared_ptr<SocketState>socketState, char* buffer, int bufferSize)
{
    std::string username = getParam(buffer, "USUARIO");

    if (username == "") {
        const char* response = "-Err. No se ha podido iniciar sesión";
        send(socket, response, strlen(response), 0);
        return;
    }

    if (!isRegistered(username)) {
        const char* response = "-Err. El usuario no está registrado";
        send(socket, response, strlen(response), 0);
        return;
    }

    socketState->user = username;
    socketState->password = getPassword(username);

    const char* response = "+Ok. Usuario correcto";
    send(socket, response, strlen(response), 0);
}

void Handlers::handlePassword(int socket, std::shared_ptr<SocketState> socketState, char* buffer, int bufferSize)
{
    std::string password = getParam(buffer, "PASSWORD");

    if (password != socketState->password) {
        const char* response = "–Err. Error en la validación";
        send(socket, response, strlen(response), 0);
        return;
    }

    socketState -> isLogged = true;

    const char* response = "+Ok. Usuario validado";
    send(socket, response, strlen(response), 0);
}

void Handlers::handleStartGame(int socket, std::shared_ptr<SocketState>socketState, char* buffer, int bufferSize)
{
    if (!socketState->isLogged) {
        const char* response = "-Err. Usuario no logueado";
        send(socket, response, strlen(response), 0);
        return;
    }
    
    GameManager* gameManager = GameManager::getInstance();

    socketState->game = gameManager -> startGame(socket, socketState);

}

bool Handlers::isRegistered(const std::string &username){
    std::ifstream usersFile;
    usersFile.open("users.txt");

    std::string line;
    while (std::getline(usersFile, line)) {
        std::string user = line.substr(0, line.find(" "));
        if (user == username) {
            return true;
        }
    }

    return false;
}

void Handlers::registerUser(const std::string &username, const std::string &password){
    std::ofstream usersFile("users.txt", std::ios_base::app);
    usersFile << username << " " << password << std::endl;
    usersFile.close();
}

std::string Handlers::getPassword(const std::string &user)
{
    std::ifstream usersFile;
    usersFile.open("users.txt");

    std::string line;
    while (std::getline(usersFile, line)) {
        std::string sLine = std::string(line);
        std::string userInLine = sLine.substr(0, sLine.find(" ")); 
        std::string aux = sLine.substr(sLine.find(" ")+1, sLine.length());
        if (user == userInLine)
        {
            return aux;
        }
    }

    return "";
}

std::string Handlers::getParam(const std::string& buffer, const std::string &option){
    std::string::size_type pos = buffer.find(option);
    if (pos == std::string::npos) {
        return "";
    }

    std::string::size_type pos2 = buffer.find(" ", pos + option.length()+1);
    if (pos2 == std::string::npos) {
        std::string::size_type pos3 = buffer.find("\0", pos + option.length()+1);
        if (pos3 == std::string::npos) {
            return "";
        }
    }

    return buffer.substr(pos + option.length() + 1, pos2 - pos - option.length() - 1);
}