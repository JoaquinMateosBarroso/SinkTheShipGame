#ifndef SINKTHESHIPSERVER_HPP
#define SINKTHESHIPSERVER_HPP

#include <GameManager.hpp>
#include <Handlers.hpp>
#include <unordered_map>
#include <memory>
#include <functional>

#define MAX_CLIENTS 30

// struct SocketState {
//     std::string user;
//     std::string password;
//     bool isLogged;
//     int game;
//     bool isYourTurn;
// };
class Server {
    private:
        int _server_socket;
        int _n_clients = 0;
        struct sockaddr_in _server_addr;
        struct sockaddr_in _client_addr;
        socklen_t _addr_size;
        int _clients[MAX_CLIENTS];
        char _buffer[MSG_SIZE];
        std::unordered_map<int, std::shared_ptr<SocketState>> _socket_states;
        std::unordered_map<std::string, std::function<void(int, std::shared_ptr<SocketState>, char*, int)>> _controllers;

    public:
        Server (int port);

        int accept_connection();

        void start();

        void exit_client(int socket, fd_set * readfds);

        void set_controller(std::string msg, std::function<void(int, std::shared_ptr<SocketState>, char*, int)> controller);

        void handleClientMsg(int socket, fd_set * readfds);

        std::string getFirstWord(const std::string& texto);
        
};

void handleFinish(int signal);

#endif