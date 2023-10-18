#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <string>
#include <memory>

struct SocketState {
    std::string user;
    std::string password;
    bool isLogged;
    int game;
    bool isYourTurn;
};

class Handlers
{
private:
    static bool isRegistered(const std::string &username);

    static void registerUser(const std::string &username, const std::string &password);

    static std::string getPassword(const std::string &user);

    static std::string getParam(const std::string& buffer, const std::string &option);
public:
    
    /**
     * @brief Handle the register message "REGISTER -u <username> -p <password>"
     * 
     * @param socket 
     * @param socketState
     * @param buffer 
     * @param bufferSize
    */
    static void handleRegister(int socket, std::shared_ptr<SocketState>socketState, char* buffer, int bufferSize);
    /**
     * TODO
     * @brief Handle the user message "USER <username>"
    */
    static void handleUser(int socket, std::shared_ptr<SocketState>socketState, char* buffer, int bufferSize);
    /** 
     * TODO
     * @brief Handle the password message "PASSWORD <password>"
    */
    static void handlePassword(int socket, SocketState &socketState, char* buffer, int bufferSize);

    /** 
     * TODO
     * @brief Handle the message "INICIO-JUEGO"
    */
    static void handleStartGame(int socket, std::shared_ptr<SocketState>socketState, char* buffer, int bufferSize);

    /** 
     * TODO
     * @brief Handle the message "DISPATO <letra>, <numero>"
    */
    static void handleShoot(int socket, SocketState &socketState, char* buffer, int bufferSize);

    /** 
     * TODO
    */
    static void handleExit(int socket, SocketState &socketState, char* buffer, int bufferSize);
};



#endif