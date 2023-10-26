#ifndef SINKTHESHIPCLIENT_HPP
#define SINKTHESHIPCLIENT_HPP

#include <sinkTheShip.hpp>

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
        bool manageNonGameOk(const string &buffer);

        bool manageError(const string &buffer);
};






#endif