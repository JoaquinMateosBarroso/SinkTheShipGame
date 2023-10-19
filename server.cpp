#include <server.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <Handlers.hpp>

#include <iostream>
#include <cstring>
#include <unistd.h>


#define SERVER_PORT 2007


int main() {
    Server server = Server(SERVER_PORT);
    server.set_controller("REGISTRO", &Handlers::handleRegister);
    server.set_controller("USER", &Handlers::handleUser);
    server.set_controller("INICIO-JUEGO", &Handlers::handleStartGame);
    server.start();

    return 0;
}

Server::Server (int port) {
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1) {
        perror("Error al crear el socket del servidor.");
        close(_server_socket);
        exit(-1);
    }

    int reuseAddr = 1;
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1) {
        perror("Error configurando SO_REUSEADDR");
        close(_server_socket);
        exit(-1);
    }

    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(SERVER_PORT);
    _server_addr.sin_addr.s_addr = INADDR_ANY;
}

int Server::accept_connection() {
    int client_socket;

    _addr_size = sizeof(struct sockaddr_in);

    if ((client_socket = accept(_server_socket, (struct sockaddr*) &_client_addr, &_addr_size)) == -1) {
        perror("Error aceptando conexiones.");
        exit(-1);
    }

    return client_socket;
}

void Server::start() {
    if (bind(_server_socket, (struct sockaddr *) &_server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Error al vincular el socket a la dirección del servidor.");
        close(_server_socket);
        exit(-1);
    }

    if (listen(_server_socket, 1) == -1) {
        perror("Error al escuchar conexiones entrantes.");
        close(_server_socket);
        exit(-1);
    }

    std::cout << "El servidor está esperando conexiones..." << std::endl;

    fd_set readfds, auxfds;

    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(_server_socket, &readfds);
    FD_SET(0, &readfds);


    while (true)
    {
        auxfds = readfds;

        int n_received_msg = select(FD_SETSIZE, &auxfds, nullptr, nullptr, nullptr);

        if (n_received_msg == -1)
        {
            perror("Error al escuchar recibir el mensaje del cliente.");
            close(_server_socket);
            exit(-1);
        }

        for (int socket = 0; socket < FD_SETSIZE; socket++)
        {
            if (FD_ISSET(socket, &auxfds))
            {
                if (socket == _server_socket) {

                    int client_socket = accept_connection();

                    if(_n_clients < MAX_CLIENTS){
                        std::cout << "Nueva conexión: " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << std::endl;

                        int newClientSocket = client_socket;
                        _clients[_n_clients++] = newClientSocket;

                        FD_SET(client_socket, &readfds);

                        const char* response = "+Ok. Usuario conectado";
                        send(client_socket, response, strlen(response), 0); cout << "send1" << endl;
                    }
                    else
                    {
                        bzero(_buffer, sizeof(_buffer));
                        strcpy(_buffer, "Demasiados clientes conectados");
                        send(client_socket, _buffer, sizeof(_buffer), 0); cout << "send2" << endl;
                        close(client_socket);
                    }
                } else if (socket == STDIN_FILENO) { 
                    printf("Fileno"); 
                    fflush(stdin);
                    bzero(_buffer, sizeof(_buffer));
                    fgets(_buffer, sizeof(_buffer),stdin);
                    
                    if(strcmp(_buffer, "SALIR") == 0){
                        for (int j = 0; j < _n_clients; j++){
                            bzero(_buffer, sizeof(_buffer));
                            strcpy(_buffer, "Desconexión servidor"); 
                            send(_clients[j], _buffer, sizeof(_buffer), 0);cout << "send3" << endl;
                            close(_clients[j]);
                            FD_CLR(_clients[j], &readfds);
                        }
                        close(_server_socket);
                        exit(-1);
                    }
                } else {
                    bzero(_buffer,sizeof(_buffer));
                    
                    int n_recv = recv(socket, _buffer, sizeof(_buffer),0);
                    
                    if(n_recv > 0){
                        handleClientMsg(socket, &readfds);    
                    }
                    if(n_recv== 0)
                    {
                        std::cout << "El socket " << socket << " ha introducido ctrl+c" << std::endl;
                        exit_client(socket, &readfds);
                    }
                }
            }
            
        }
    }
                
}

void Server::exit_client(int socket, fd_set * readfds){    
    close(socket);
    FD_CLR(socket,readfds);
    
    int j;

    for (j = 0; j < _n_clients - 1; j++)
        if (_clients[j] == socket)
            break;
    for (; j < _n_clients - 1; j++)
        (_clients[j] = _clients[j+1]);
    
    _n_clients--;

    std::cout << "El socket " << socket << " se ha desconectado." << std::endl;

    // Doing something when a client exits
}

void Server::set_controller(std::string msg, std::function<void(int, std::shared_ptr<SocketState>, char*, int)> controller) {
    _controllers[msg] = controller;
}

void Server::handleClientMsg(int socket, fd_set * readfds) {
    if(strcmp(_buffer, "SALIR\n") == 0){
        exit_client(socket, readfds);
        _socket_states.erase(socket);
        return;
    }
    std::string keyword = getFirstWord(_buffer);

    std::shared_ptr<SocketState> ptr_state;
    if (_socket_states.find(socket) != _socket_states.end()) {
        ptr_state = std::make_shared<SocketState>();
        ptr_state -> game = -1;
        ptr_state -> isLogged = false;
        ptr_state -> isYourTurn = false;
        ptr_state -> password = nullptr;
        ptr_state -> user = nullptr;
        _socket_states[socket] = ptr_state; 
    }else
    {
        ptr_state = _socket_states[socket];
    }

    bool keywordFound = (_controllers.find(keyword) != _controllers.end());
    if (!keywordFound)
    {
        string sresponse = "--Err. Comando no reconocido";
        const char* response = sresponse.c_str();
        send(socket, response, strlen(response), 0); cout << "Comando no reconocido" << endl;
        return;
    }

    _controllers[keyword](socket, ptr_state, _buffer, (int)sizeof(_buffer));
    cout << "Aquí llego";
}

std::string Server::getFirstWord(const std::string& texto) {
    std::istringstream iss(texto);
    std::string primeraPalabra;
    iss >> primeraPalabra;
    return primeraPalabra;
}