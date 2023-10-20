#include <sinkTheShip.hpp>

#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>

using namespace std;





/********************************************************
 * SinkTheShipServer
*********************************************************/


bool SinkTheShipServer::addPlayer(const int socket, const string &username) {
    if (_free) {
        _player1.socket = socket;
        _player1.username = username;
        _free = false;
        return true;
    } else if (!_started) {
        _player2.socket = socket;
        _player2.username = username;
        _started = true;
        return true;
    }
    return false;
}

void SinkTheShipServer::closeGame(int socket)
{
    if (_player1.socket != -1)
    {
        //string sresponse = "La partida ha finalizado\n";
        //const char* response = sresponse.c_str();
        //send(socket, response, strlen(response), 0);
    }
    


    _started = false;
    _free = true;
}




void SinkTheShipServer::start(){
    _started = true;
    const char* response = "+Ok. Turno de partida";
    send(_player1.socket, response, strlen(response), 0);
}


void SinkTheShipServer::createBoards()
{
    srand(time(nullptr));
    // Initialize cells to water
    for (int i = 0; i < MAX_CLIENTS; i++) {
        for (int j = 0; j < MAX_CLIENTS; j++) {
            boardPlayer1[i][j] = Water;
            boardPlayer2[i][j] = Water;
        }
    }

    // Place boats randomly on both boards
    placeBoat(boardPlayer1, 4);
    placeBoat(boardPlayer1, 3);
    placeBoat(boardPlayer1, 3);
    placeBoat(boardPlayer1, 2);
    placeBoat(boardPlayer1, 2);
    
    placeBoat(boardPlayer2, 4);
    placeBoat(boardPlayer2, 3);
    placeBoat(boardPlayer2, 3);
    placeBoat(boardPlayer2, 2);
    placeBoat(boardPlayer2, 2);
    

}

void placeBoat(Cell board[MAX_CLIENTS][MAX_CLIENTS], int size) {
    int x, y, direction;
    
    do {
        // Randomly choose a position and direction
        x = rand() % MAX_CLIENTS;
        y = rand() % MAX_CLIENTS;
        direction = rand() % 2; // 0 for horizontal, 1 for vertical
    } while (!isValidPlacement(board, x, y, direction, size));
    
    // Place the boat on the board
    for (int i = 0; i < size; i++) {
        if (direction == 0) {
            board[x][y + i] = Boat;
        } else {
            board[x + i][y] = Boat;
        }
    }
}

// Function to check if a boat placement is valid
bool isValidPlacement(Cell board[MAX_CLIENTS][MAX_CLIENTS], int x, int y, int direction, int size) {
    if (direction == 0) {
        if (y + size > MAX_CLIENTS) return false; // Out of bounds
        for (int i = 0; i < size; i++) {
            if (board[x][y + i] != Water) return false; // Overlaps with existing boat
        }
    } else {
        if (x + size > MAX_CLIENTS) return false; // Out of bounds
        for (int i = 0; i < size; i++) {
            if (board[x + i][y] != Water) return false; // Overlaps with existing boat
        }
    }
    return true;
}




std::string SinkTheShipServer::getStringBoard(int i)
{
    //TODO
    
}











/***********************************************************
 * SinkTheShipClient  
************************************************************/

void SinkTheShipClient::start(const string &board, const int boardSize)
{
    
    _boardSize = boardSize;

    // Initialize a vector to store the matrix
    _myBoard = vector< vector<Cell> >(_boardSize, vector<Cell>(_boardSize, Cell::Water));

    // Loop through the matrix and populate it
    for (int i = 0; i < _boardSize; ++i) {
        for (int j = 0; j < _boardSize; j++)
        {
            if (board[(2*_boardSize*i) + 2*j] == 'B')
            {
                _myBoard[i][j] = Cell::Boat;
            }
        }
    }
    // TODO the same but for opponent
    cout << "game starting" << endl;
}





// Plays a turn
// @param buffer received from server
void SinkTheShipClient::playTurn(const std::string &buffer)
{
    // TODO

    return ;
}

// Shows the board via stdout the board
void SinkTheShipClient::showBoard()
{
    clearScreen();
    cout << "Here goes the board" << endl << endl;

    cout << "********* MY BOARD *********" << endl << endl;

    for (auto row: _myBoard)
    {
        for (auto i: row)
        {
            switch (i)
            {
                case Boat: cout << 'B'; break;
                case Water: cout << 'A'; break;
                case Touched: cout << 'T'; break;
                case Floaded: cout << 'H'; break;
                default: throw runtime_error("Not allowed cell");
            }
            cout << ' ';
        }
        cout << endl;
    }
    

    // TODO the same but for opponent
}








/***********************************************************
 * Extra functionalities  
************************************************************/
void clearScreen() {
#ifdef _WIN32
    // For Windows
    system("cls");
#else
    // For Linux and macOS
    system("clear");
#endif
}





int main2()
{
    SinkTheShipClient game;
    game.start("A,A,A,B,B,A,A,B,B,B;A,A,A,A,A,A,A,A,A,A;B,B,A,B,B,A,A,B,B,B;A,A,A,A,A,A,A,A,A,A;A,B,A,B,A,B,A,B,A,A;A,A,A,A,A,A,A,A,A,A;A,A,B,B,B,B,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A", 10);
    game.showBoard();

    return 0;
}