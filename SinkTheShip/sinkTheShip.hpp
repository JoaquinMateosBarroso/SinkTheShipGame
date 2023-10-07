#ifndef SINKTHESHIP_HPP
#define SINKTHESHIP_HPP

#include <iostream>

using namespace std;

// A class that will be used by the TCP client
class SinkTheShipClient {
    public:
        SinkTheShipClient();

        // Gets the object prepared for a game with a new board
        // @param board received from server, but cleaned 
        void start(string board);

        // @return Is the game being played?
        bool isGameOpen();


        // Plays a turn
        // @param buffer received from server
        void playTurn(const std::string &buffer);


        // Shows the board via stdout
        void showBoard();


    private:
        bool _isGameOpen;
};


/***********************************************************
 * Extra functionalities
************************************************************/
void clearScreen();

#endif