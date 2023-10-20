#ifndef SINKTHESHIP_HPP
#define SINKTHESHIP_HPP

#include <iostream>
#include <vector>

#define MAX_CLIENTS 10
#define MSG_SIZE 250

#define BARCOS_4CUADRADOS 1
#define BARCOS_3CUADRADOS 2
#define BARCOS_2CUADRADOS 2

using namespace std;

enum Cell {
    Boat,
    Water,
    Touched,
    Floaded,
    Unkwown,
};

struct Player {
    int socket = -1;
    string username = "";
};


class SinkTheShipServer {
    public:
        SinkTheShipServer() {
            _started = false;
            _free = true;
        }

        // Gets the object prepared for a game with a new board
        // @param board received from server, but cleaned 
        void start();

        // @return Is the game started?
        bool isGameStarted() {
            return _started;
        }

        // @return Is the game free?
        bool isGameFree() {
            return _free;
        }

        void closeGame(int socket);

        // @return Could the player be added to a game?
        bool addPlayer(const int socket, const string &username);

        Player& getPlayer(const int id) {
            return (id==1)? _player1: _player2;
        }

        Player _player1;
        Player _player2;
        bool _started;
        bool _free;
    private:



        Cell boardPlayer1[MAX_CLIENTS][MAX_CLIENTS];
        Cell boardPlayer2[MAX_CLIENTS][MAX_CLIENTS];
};



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




/***********************************************************
 * Extra functionalities
************************************************************/
void clearScreen();

#endif