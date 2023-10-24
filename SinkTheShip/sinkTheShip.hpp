#ifndef SINKTHESHIP_HPP
#define SINKTHESHIP_HPP

#include <iostream>
#include <vector>
#include <memory>

#define MAX_GAMES 10
#define BOARD_SIZE 10
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
    std::shared_ptr<SocketState> socketState;
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

        void createBoards();

        std::string getStringBoard(int i);

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
        bool addPlayer(const int socket, std::shared_ptr<SocketState> socketState);

        Player& getPlayer(const int id) {
            return (id==1)? _player1: _player2;
        }

        void shoot(int socket, int col, int row);

        Player _player1;
        Player _player2;
        int _turn;
        bool _started;
        bool _free;

    private:
        Cell boardPlayer1[BOARD_SIZE][BOARD_SIZE];
        Cell boardPlayer2[BOARD_SIZE][BOARD_SIZE];
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
        void showBoard(const string &buffer);


    private:

        enum Cell {
            Boat,
            Water,
            Touched,
            Floaded,
            Unkwown,
        };

        int cellPosinChar2Int(const char c);
        
        int _boardSize;

        vector< vector<Cell> > _myBoard;

        vector< vector<Cell> > _opponentBoard;
};




/***********************************************************
 * Extra functionalities
************************************************************/
void clearScreen();

#endif










bool isValidPlacement(Cell board[BOARD_SIZE][BOARD_SIZE], int x, int y, int direction, int size);

void placeBoat(Cell board[BOARD_SIZE][BOARD_SIZE], int size);

std::string getCellString(Cell cell);
