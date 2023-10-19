#ifndef SINKTHESHIPCLIENT_HPP
#define SINKTHESHIPCLIENT_HPP

#include <sinkTheShip.hpp>
#include <user.hpp>

#include <iostream>

#define BUFFER_SIZE 250

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

        char _buffer[BUFFER_SIZE];

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

        /**
         * @return whether the game must begin
        */
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