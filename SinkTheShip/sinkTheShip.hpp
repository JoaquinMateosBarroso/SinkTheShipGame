#ifndef SINKTHESHIP_HPP
#define SINKTHESHIP_HPP

#include <iostream>
#include <vector>

using namespace std;

// A class that will be used by the TCP client
class SinkTheShipClient {
    public:
        SinkTheShipClient() {}

        // Gets the object prepared for a game with a new board
        // @param board received from server, but cleaned 
        void start(const string &board, const int boardSize);

        // @return Is the game being played?
        bool isGameOpen();


        // Plays a turn
        // @param buffer received from server
        void playTurn(const std::string &buffer);


        // Shows the board via stdout
        void showBoard();


    private:

        enum Cell {
            Boat,
            Water,
            Touched,
            Floaded,
            Unkwown,
        };
        
        int _boardSize;

        vector< vector<Cell> > _myBoard;

        vector< vector<Cell> > _opponentBoard;
};


// A class that will be used by the TCP client
class SinkTheShipServer {
    public:
        // SinkTheShipClient() {}

        // Gets the object prepared for a game with a new board
        // @param board received from server, but cleaned 
        void start(const string &board, const int boardSize);

        // @return Is the game being played?
        bool isGameOpen();


        // Plays a turn
        // @param buffer received from server
        void playTurn(const std::string &buffer);


        // Shows the board via stdout
        void showBoard();


    private:

        enum Cell {
            Boat,
            Water,
            Touched,
            Floaded,
            Unkwown,
        };
        
        vector<SinkTheShipClient> _games;
};


/***********************************************************
 * Extra functionalities
************************************************************/
void clearScreen();

#endif