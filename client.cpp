#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error al crear el socket del cliente." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(2000);
    serverAddress.sin_addr.s_addr =  inet_addr("127.0.0.1");
    

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error al conectar al servidor." << std::endl;
        return 1;
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        std::cerr << "Error al recibir el mensaje del servidor." << std::endl;
        return 1;
    }

    std::cout << "Mensaje del servidor: " << buffer << std::endl;

    close(clientSocket);

    return 0;
}