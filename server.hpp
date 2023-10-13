#ifndef SINKTHESHIPSERVER_HPP
#define SINKTHESHIPSERVER_HPP

#include <sinkTheShip.hpp>
#include <GameManager.hpp>
#include <user.hpp>

struct SocketState {
    std::string user;
    std::string password;
    bool isLogged;
    int game;
    bool isYourTurn;
};

class Handlers {

    public:
        /**
         * @brief Handle the register message "REGISTER -u <username> -p <password>"
         * 
         * @param socket 
         * @param socketState
         * @param buffer 
         * @param bufferSize
        */
        void handleRegister(int socket, SocketState &socketState, char* buffer, int bufferSize);
        
        /**
         * TODO
         * @brief Handle the user message "USER <username>"
        */
        void handleUser(int socket, SocketState &socketState, char* buffer, int bufferSize);
        /** 
         * TODO
         * @brief Handle the password message "PASSWORD <password>"
        */
        void handlePassword(int socket, SocketState &socketState, char* buffer, int bufferSize);

        /** 
         * TODO
         * @brief Handle the message "INICIO-JUEGO"
        */
        void handleStartGame(int socket, SocketState &socketState, char* buffer, int bufferSize);

        /** 
         * TODO
         * @brief Handle the message "DISPATO <letra>, <numero>"
        */
        void handleShoot(int socket, SocketState &socketState, char* buffer, int bufferSize);

        /** 
         * TODO
        */
        void handleExit(int socket, SocketState &socketState, char* buffer, int bufferSize);

    private:
        bool isRegistered(const std::string &username);

        void registerUser(const std::string &username, const std::string &password);

        std::string getPassword(const std::string &user);

        std::string getParam(const std::string& buffer, const std::string &option);


        // Attributes
        GameManager _gameManager;
};



#endif