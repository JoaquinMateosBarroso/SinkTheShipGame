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