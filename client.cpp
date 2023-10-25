#include <client.hpp>
#include <sinkTheShip.hpp>


#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <err.h>


#define SERVER_PORT 2007
#define SERVER_IP "127.0.0.1"

#define BOARD_SIZE 10

using namespace std;
int main( )
{

	Client client(SERVER_IP, SERVER_PORT);
    
    client.start();

    client.finish();

    return 0;
		
}


Client::Client(string serverIpAddr, int serverPort)
{
    _serverIpAddr = serverIpAddr;
    _serverPort = serverPort;
}

void Client::start()
{
  	openSocket();

    connectSocket();

    initStructures();

    
    bool fin = false;
	do {
        readMessage();
        
        if(FD_ISSET(_socketDescriptor, &_auxfds)) {
            manageServerMessage();
        }
        else if(FD_ISSET(0,&_auxfds)) {
            fin = manageTerminalInput();
        }
        else {
            perror("Message receive from an unkwown socket");
            exit(-1);
        }
    } while(!fin);
}


void Client::openSocket()
{
    _socketDescriptor = socket (AF_INET, SOCK_STREAM, 0);
	if (_socketDescriptor == -1)
	{
		perror("No se puede abrir el socket cliente\n");
        exit (1);
	}
}

void Client::initStructures()
{
    FD_ZERO(&_auxfds);
    FD_ZERO(&_readfds);
    
    FD_SET(0,&_readfds);
    FD_SET(_socketDescriptor,&_readfds);
}

void Client::connectSocket()
{
    struct sockaddr_in _sockName;
    _sockName.sin_family = AF_INET;
	_sockName.sin_port = htons(_serverPort);
	_sockName.sin_addr.s_addr = inet_addr(_serverIpAddr.c_str());
    
    int len_sockname = sizeof(_sockName);
	
	if (connect(_socketDescriptor, (struct sockaddr *)&_sockName, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}
    _state = Connected;
}

void Client::readMessage()
{
    _auxfds = _readfds;

    int n_received_msg;
    n_received_msg = select(_socketDescriptor+1,&_auxfds,NULL,NULL,NULL);

    if (n_received_msg == -1)
    {
        perror("Error al escuchar recibir el mensaje del cliente.");
        close(_socketDescriptor);
        exit(-1);
    }
}

void Client::manageServerMessage()
{
    bzero(_buffer, BUFFER_SIZE);
    recv(_socketDescriptor, _buffer, BUFFER_SIZE,0);
    bool isError = manageError(_buffer);

    if (!isError)
    {
        switch (_state)
        {
            case Connected: manageConnectedMessage(); break;
            case WaitingForGame: manageWaitingForGameMessage(); break;
            case Playing: managePlayingMessage(); break;
            default: perror("Estado desconocido"); exit(-1);
        }
    }
}

bool Client::manageTerminalInput()
{
    bool fin = false;
    bzero(_buffer,sizeof(_buffer));
            
    fgets(_buffer,sizeof(_buffer),stdin);
    
    if(strcmp(_buffer,"SALIR\n") == 0){
            fin = 1;
    }

    // Erase final \n
    if (strlen(_buffer) >= 2)
        _buffer[strlen(_buffer) - 1] = '\0';


    send(_socketDescriptor, _buffer, sizeof(_buffer),0);
    
    return fin;
}



void Client::finish()
{
    close(_socketDescriptor);
}




bool manageError(string buffer)
{
    if (buffer.compare(0, 3, "-Err")) // It is not -Err
    {
        return false;
    }

    cout << buffer << endl;

    return true;
}

bool Client::manageNonGameOk(string buffer)
{
    bool gameBegins = false;
    if (_state == Connected or _state == WaitingForGame)
    {
            if (buffer.find("+Ok") == string::npos) // It is not +Ok
            {
                if(strcmp(_buffer,"Desconexión servidor") == 0){
                    cout << "El servidor se ha desconectado, salimos" << endl;
                    close(_socketDescriptor);
                }
                return false;
            }

            gameBegins = (buffer.find("+Ok. Empezamos partida") != std::string::npos);
            return gameBegins;
    }
        
    perror("Part of code that should not be accesible");
    exit(-1);
}



void Client::manageConnectedMessage()
{
    bool gameBegins = manageNonGameOk(std::string(_buffer));
    cout << _buffer << endl;
    if (gameBegins)
    {
        _game.start(string(_buffer).substr(strlen("+Ok. Empezamos partida.")), BOARD_SIZE);
        _game.showBoard(_buffer);
        _state = Playing;
    }
}

void Client::manageWaitingForGameMessage()
{
    bool gameBegins = manageNonGameOk(_buffer);
    cout << _buffer << endl;
    if (gameBegins)
    {
        _game.start(string(_buffer).substr(strlen("+Ok. Empezamos partida")), BOARD_SIZE);
        _game.showBoard(_buffer);
        cout << "Es tu turno. Escribe tu comando:" << endl;
        _state = Playing;
    }
    cout << "hemos llegado desde waiting" << endl;
}

void Client::managePlayingMessage()
{
    string sbuffer = _buffer;
    if (sbuffer.find("Tu oponente ha terminado la partida") != string::npos)
    {
        _state = Connected;
        return;
    }
    
    if (sbuffer.find("-Err") != string::npos or sbuffer.find("Turno de partida") != string::npos)
        cout << _buffer << endl;
    else
    {
        _game.playTurn(_buffer);

        if (sbuffer.find("Turno de partida") != string::npos)
            cout << "Es tu turno. Introduce el comando a ejecutar:" << endl;



    }

}