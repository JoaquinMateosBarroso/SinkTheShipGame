#ifndef SINKTHESHIP_HPP
#define SINKTHESHIP_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <Handlers.hpp>
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
            boardPlayer1 = vector<vector<Cell>>(10);
            for (int i = 0; i < 10; i++) {
                boardPlayer1[i].resize(10);
            }
            boardPlayer2 = vector<vector<Cell>>(10);
            for (int i = 0; i < 10; i++) {
                boardPlayer2[i].resize(10);
            }
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
        vector<vector<Cell>> boardPlayer1;
        vector<vector<Cell>> boardPlayer2;
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



        int cellPosinChar2Int(const char c);
        
        int _boardSize;

        vector< vector<Cell> > _myBoard;

        vector< vector<Cell> > _opponentBoard;
};




/***********************************************************
 * Extra functionalities
************************************************************/
void clearScreen();










bool isValidPlacement(vector<vector<Cell>> board, int x, int y, int direction, int size);

void placeBoat(vector<vector<Cell>>& board, int size);

std::string getCellString(Cell cell);





bool isShipSunk(vector< vector<Cell> > board, int row, int col, vector<pair<int,int>> &visited);



void markShipAsSunk(vector< vector<Cell> > board, int row, int col);



#endif

