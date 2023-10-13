#include <server.hpp>
#include <string>
#include <sstream>
#include <fstream>


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <sstream>

#define MAX_CLIENTS 10
#define SERVER_PORT 2007
#define MSG_SIZE 250

class Server {
    private:
        int _server_socket;
        int _n_clients = 0;
        struct sockaddr_in _server_addr;
        struct sockaddr_in _client_addr;
        socklen_t _addr_size;
        int _clients[MAX_CLIENTS];
        char _buffer[MSG_SIZE];

    public:
        Server (int port) {
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

        int accept_connection() {
            int client_socket;

            _addr_size = sizeof(struct sockaddr_in);

            if ((client_socket = accept(_server_socket, (struct sockaddr*) &_client_addr, &_addr_size)) == -1) {
                perror("Error aceptando conexiones.");
                exit(-1);
            }

            return client_socket;
        }

        void start() {
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
                                send(client_socket, response, strlen(response), 0);
                            }
                            else
                            {
                                bzero(_buffer, sizeof(_buffer));
                                strcpy(_buffer, "Demasiados clientes conectados\n");
                                send(client_socket, _buffer, sizeof(_buffer), 0);
                                close(client_socket);
                            }
                        } else if (socket == STDIN_FILENO) { printf("Fileno\n"); fflush(stdin);
                            bzero(_buffer, sizeof(_buffer));
                            fgets(_buffer, sizeof(_buffer),stdin);
                            
                            if(strcmp(_buffer, "SALIR\n") == 0){
                                for (int j = 0; j < _n_clients; j++){
                                    bzero(_buffer, sizeof(_buffer));
                                    strcpy(_buffer, "Desconexión servidor\n"); 
                                    send(_clients[j], _buffer, sizeof(_buffer), 0);
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
                                if(strcmp(_buffer, "SALIR\n") == 0){
                                    exit_client(socket, &readfds);
                                }
                                else{
                                    handleClientMsg(socket, &readfds);
                                }     
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

        void exit_client(int socket, fd_set * readfds){    
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

        void handleClientMsg(int socket, fd_set * readfds) {
            std::string keyword = getFirstWord(_buffer);
            // TODO
            // ejecuta la función que corresponda segun la keyword.
            // Será un diccionario en el que la clave sera la keyword
            // y el valor será un puntero a la funcion a ejecutar
        }

        std::string getFirstWord(const std::string& texto) {
            std::istringstream iss(texto);
            std::string primeraPalabra;
            iss >> primeraPalabra;
            return primeraPalabra;
        }
};

int main() {

    Server server = Server(SERVER_PORT);

    server.start();

    return 0;
}


std::string Handlers::getParam(const std::string& buffer, const std::string &option)
{
    std::string::size_type pos = buffer.find(option);
    if (pos == std::string::npos) {
        return "";
    }

    std::string::size_type pos2 = buffer.find(" ", pos + option.length() + 1);
    if (pos2 == std::string::npos) {
        return "";
    }

    return buffer.substr(pos + option.length() + 1, pos2 - pos - option.length() - 1);
}

void Handlers::registerUser(const std::string &username, const std::string &password)
{
    std::ofstream usersFile("users.txt", std::ios_base::app);
    usersFile << username << " " << password << std::endl;
    usersFile.close();
}

bool Handlers::isRegistered(const std::string &username)
{
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

std::string Handlers::getPassword(const std::string &user)
{
    std::ifstream usersFile;
    usersFile.open("users.txt");

    std::string line;
    while (std::getline(usersFile, line)) {
        std::string sLine = std::string(line);
        std::string aux = sLine.substr(sLine.find(" ")+1, sLine.length());
        return aux;
    }

    return "";
}




// @param buffer will contain: REGISTRO -u <username> -p <password>
// @brief checks whether the username is already registered, and, if it is not,
// registers the user and saves it into the textfile users.txt
void Handlers::handleRegister(int socket, SocketState &socketState, char* buffer, int bufferSize)
{
    std::string username = getParam(buffer, "-u");
    std::string password = getParam(buffer, "-p");

    if (username == "" || password == "") {
        const char* response = "-Err. No se ha podido registrar el usuario";
        send(socket, response, strlen(response), 0);
        return;
    }

    if (isRegistered(username)) {
        const char* response = "-Err. El usuario ya está registrado";
        send(socket, response, strlen(response), 0);
        return;
    }

    registerUser(username, password);

    const char* response = "+Ok. Usuario registrado";
    send(socket, response, strlen(response), 0);
}