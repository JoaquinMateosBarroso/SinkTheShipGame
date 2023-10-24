#include <sinkTheShip.hpp>

#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <Handlers.hpp>

using namespace std;





/********************************************************
 * SinkTheShipServer
*********************************************************/

void SinkTheShipServer::shoot(int socket, int col, int row) {
    Player playerWhoShoot;
    Player playerWhoIsShooted;
    Cell (*shootedBoard)[BOARD_SIZE];

    if (socket == _player1.socket)
    {
        playerWhoShoot = _player1;
        playerWhoIsShooted = _player2;
        shootedBoard = boardPlayer2;
    } else if (socket == _player2.socket)
    {
        playerWhoShoot = _player2;
        playerWhoIsShooted = _player1;
        shootedBoard = boardPlayer1;
    }
    
    char letter = 'A' - 1 + col;
    const char* response = string("+Ok. Disparo en " + to_string(letter) + "," + to_string(row)).c_str();
    send(playerWhoIsShooted.socket, response, strlen(response), 0);

    Cell shootedCell = shootedBoard[col][row];

    if (shootedCell == Cell::Boat)
    {
        shootedBoard[col][row] = Cell::Touched;
        const char* response = string("+Ok. TOCADO: " + to_string(letter) + "," + to_string(row)).c_str();
        send(playerWhoShoot.socket, response, strlen(response), 0);

        response = "+Ok. Turno de partida";
        send(playerWhoShoot.socket, response, strlen(response), 0);
    }else if (shootedCell == Cell::Touched)
    {
        shootedBoard[col][row] = Cell::Floaded;
        const char* response = string("+Ok. HUNDIDO: " + to_string(letter) + "," + to_string(row)).c_str();
        send(playerWhoShoot.socket, response, strlen(response), 0);
        
        response = "+Ok. Turno de partida";
        send(playerWhoShoot.socket, response, strlen(response), 0);
    } else if (shootedCell == Cell::Water)
    {
        const char* response = string("+Ok. AGUA: " + to_string(letter) + "," + to_string(row)).c_str();
        send(playerWhoShoot.socket, response, strlen(response), 0);
        
        playerWhoIsShooted.socketState -> isYourTurn = true;
        playerWhoShoot.socketState -> isYourTurn = false;

        response = "+Ok. Turno de partida";
        send(playerWhoIsShooted.socket, response, strlen(response), 0);
    }
    
    
    

}


bool SinkTheShipServer::addPlayer(const int socket, std::shared_ptr<SocketState> socketState) {
    if (_free) {
        _player1.socket = socket;
        _player1.username = socketState -> user;
        _player1.socketState = socketState;
        _free = false;
        return true;
    } else if (!_started) {
        _player2.socket = socket;
        _player2.username = socketState -> user;
        _player2.socketState = socketState;
        _started = true;
        return true;
    }
    return false;
}

void SinkTheShipServer::closeGame(int socket)
{
    
    const char* response = "+Ok. Tu oponente ha terminado la partida";
    send(
        _player1.socket == socket
            ? _player2.socket
            : _player1.socket, 
        response, 
        strlen(response), 
        0
    );

    _player1.socket = -1;
    _player1.username = "";
    _player2.socket = -1;
    _player2.username = "";

    _started = false;
    _free = true;
}




void SinkTheShipServer::start(){
    _started = true;
    const char* response = "+Ok. Turno de partida";
    _player1.socketState -> isYourTurn = true;
    send(_player1.socket, response, strlen(response), 0);
}


void SinkTheShipServer::createBoards()
{
    srand(time(nullptr));
    // Initialize cells to water
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
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

void placeBoat(Cell board[BOARD_SIZE][BOARD_SIZE], int size) {
    int x, y, direction;
    
    do {
        // Randomly choose a position and direction
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
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
bool isValidPlacement(Cell board[BOARD_SIZE][BOARD_SIZE], int x, int y, int direction, int size) {
    if (direction == 0) {
        if (y + size > BOARD_SIZE) return false; // Out of bounds
        for (int i = 0; i < size; i++) {
            if (board[x][y + i] != Water) return false; // Overlaps with existing boat
        }
    } else {
        if (x + size > BOARD_SIZE) return false; // Out of bounds
        for (int i = 0; i < size; i++) {
            if (board[x + i][y] != Water) return false; // Overlaps with existing boat
        }
    }
    return true;
}




std::string SinkTheShipServer::getStringBoard(int i)
{
    //TODO
    std::string out;

    auto board = (i==1? boardPlayer1: boardPlayer2);

    for (int j = 0; j < BOARD_SIZE; j++)
    {
        for (int k = 0; k < BOARD_SIZE; k++)
        {
            out += getCellString(board[j][k]);
        }
        out += (j==(BOARD_SIZE-1)? ".": ";");
    }

    return out;
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


    _opponentBoard = vector< vector<Cell> >(_boardSize, vector<Cell>(_boardSize, Cell::Unkwown));

    cout << "game starting" << endl;
}



int SinkTheShipClient::cellPosinChar2Int(const char c) {
    return c - 'A';
}

// Plays a turn
// @param buffer received from server
void SinkTheShipClient::playTurn(const std::string &buffer)
{
    int pos;
    int x;
    char y;
    if ((pos=buffer.find("AGUA: ")) != string::npos)
    {
        pos += strlen("AGUA: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2));
        _opponentBoard[cellPosinChar2Int(y)][x] = Water;
        showBoard(buffer);
        cout << "Agua" << endl;
    } else if ((pos=buffer.find("TOCADO: ")) != string::npos)
    {
        pos += strlen("TOCADO: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2));
        _opponentBoard[cellPosinChar2Int(y)][x] = Touched;
        showBoard(buffer);
        cout << "Tocado" << endl;
    } else if ((pos=buffer.find("HUNDIDO: ")) != string::npos)
    {
        pos += strlen("HUNDIDO: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2));
        _opponentBoard[cellPosinChar2Int(y)][x] = Floaded;
        showBoard(buffer);
        cout << "Hundido" << endl;
    } else if (buffer.find("Disparo en:"))
    {
        pos += strlen("HUNDIDO: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2));
        _opponentBoard[cellPosinChar2Int(y)][x] = Floaded;
        showBoard(buffer);
        cout << "El oponente ha disparado en " << y << x << endl;
    } else {
        cout << buffer << endl;
    }



    return ;
}

// Shows the board via stdout the board
void SinkTheShipClient::showBoard(const string &buffer)
{
    clearScreen();
    cout << "Here goes the board" << endl << endl;

    cout << "********* MY BOARD *********" << endl << endl;

    cout << "   A B C D E F G H I J" << endl;
    int index=1;
    for (auto row: _myBoard)
    {
        cout << index << ((index==10)?" ": "  ");
        index++;
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
    cout << endl << endl << endl;

    cout << "********* OPPONENT BOARD *********" << endl << endl;
    cout << "   A B C D E F G H I J" << endl;
    cout << "   -------------------" << endl;
    index = 1; 
    for (auto row: _opponentBoard)
    {
        cout << index << ((index==10)?" ": "  ");
        index++;
        for (auto i: row)
        {
            switch (i)
            {
                case Boat: cout << 'B'; break;
                case Water: cout << 'A'; break;
                case Touched: cout << 'T'; break;
                case Floaded: cout << 'H'; break;
                case Unkwown: cout << 'U'; break;
                default: throw runtime_error("Not allowed cell");
            }
            cout << ' ';
        }
        cout << endl;
    }
    cout << endl << endl << endl;
    










    if (buffer.find("Turno de partida") != string::npos)
        cout << "Es tu turno. Introduce el comando a ejecutar:" << endl;

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



std::string getCellString(Cell cell)
{
    switch (cell) {
        case Boat: return "B";
        case Water: return "A";
        case Floaded: return "H"; // Hundido
        case Touched: return "T"; // Tocado
        case Unkwown: return "D"; // Desconocido
    }
    return "Error";
}



int cellPosinChar2Int(const char c) {
    switch(c) {
        case 'A': return 1;
        case 'B': return 2;
        case 'C': return 3;
        case 'D': return 4;
        case 'E': return 5;
        case 'F': return 6;
        case 'G': return 7;
        case 'H': return 8;
        case 'I': return 9;
        case 'J': return 10;
        default: return -1;
    }
}



int main2()
{
    SinkTheShipClient game;
    game.start("A,A,A,B,B,A,A,B,B,B;A,A,A,A,A,A,A,A,A,A;B,B,A,B,B,A,A,B,B,B;A,A,A,A,A,A,A,A,A,A;A,B,A,B,A,B,A,B,A,A;A,A,A,A,A,A,A,A,A,A;A,A,B,B,B,B,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A", 10);
    game.showBoard("Turno de partida");

    return 0;
}
