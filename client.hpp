#ifndef SINKTHESHIPCLIENT_HPP
#define SINKTHESHIPCLIENT_HPP

#include <sinkTheShip.hpp>
#include <user.hpp>

#include <iostream>

using namespace std;


class Client {
    public:
        Client(string ip_addr, int port);

        void start();

        void finish();

        void openSocket();

        void connectSocket();

        void initStructures();

        void waitForMessage();

        void readMessage();
        
        void manageServerMessage();

  
        
        bool manageTerminalInput();
    
    private:
        int _socketDescriptor;

        char _buffer[250];

        fd_set _readfds, _auxfds;

        int _serverPort;
        string _serverIpAddr;

        SinkTheShipClient _game;

        enum State {
            Connected,
            WaitingForGame,
            Playing,
        } _state;


        void manageConnectedMessage();

        void manageWaitingForGameMessage();

        void managePlayingMessage();

        bool manageNonGameOk(string buffer);
};




// Manages a possible error received from server
// @return whether it was an actual error
bool manageError(string buffer);

// Manages a +Ok. received from server that is not 
// a game event
// @return whether the game has begun
bool manageNonGameOk(string buffer);


#endif