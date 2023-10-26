#include <sinkTheShip.hpp>

#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <algorithm>

#include <Handlers.hpp>


using namespace std;





/********************************************************
 * SinkTheShipServer
*********************************************************/

bool isTheGameFinished(vector<vector<Cell>>& shootedBoard) {
    for (auto i:shootedBoard) {
        for (auto j: i) {
            if (j == Cell::Boat)
            {
                return false;
            }
            
        }
    }
    return true;
}

void makeShooted(vector<vector<Cell>>& shootedBoard, Player playerWhoShoot, Player playerWhoIsShooted, int col, int row) {
    char letter = 'A' + col;
    string response = string("+Ok. Disparo en: ") + letter + "," + to_string(row+1);
    send(playerWhoIsShooted.socket, response.c_str(), response.length(), 0); 

    Cell shootedCell = shootedBoard[row][col];

    if (shootedCell == Cell::Boat or shootedCell == Cell::Touched)
    {
        shootedBoard[row][col] = Touched;
        vector<pair<int,int>> visited;
        if (isShipSunk(shootedBoard, row, col, visited))
        {
            markShipAsSunk(shootedBoard, row, col);
            response = string("+Ok. HUNDIDO: ") + letter + "," + to_string(row+1);

            if (isTheGameFinished(shootedBoard)) {
                response = "+Ok. " + playerWhoShoot.username + " ha ganado, número de disparos " + to_string(playerWhoShoot.nShoots);
                send(playerWhoIsShooted.socket, response.c_str(), response.length(), 0);
                send(playerWhoShoot.socket, response.c_str(), response.length(), 0);
                return;
            }
        }
        else
        {
            shootedBoard[row][col] = Cell::Touched;
            response = string("+Ok. TOCADO: ") + letter + "," + to_string(row+1);
        }    
        
        send(playerWhoShoot.socket, response.c_str(), response.length(), 0);

        playerWhoIsShooted.socketState -> isYourTurn = true;
        playerWhoShoot.socketState -> isYourTurn = false;

        response = "+Ok. Turno de partida";
        send(playerWhoIsShooted.socket, response.c_str(), response.length(), 0);
    } else if (shootedCell == Cell::Water)
    {
        string response = std::string("+Ok. AGUA: ") + letter + "," + to_string(row+1);
        send(playerWhoShoot.socket, response.c_str(), response.length(), 0);
        
        playerWhoIsShooted.socketState -> isYourTurn = true;
        playerWhoShoot.socketState -> isYourTurn = false;

        response = "+Ok. Turno de partida";
        send(playerWhoIsShooted.socket, response.c_str(), response.length(), 0);
    }
    
}

void SinkTheShipServer::shoot(int socket, int col, int row) {
    
    if (socket == _player1.socket)
    {
        _player1.nShoots++;
        makeShooted(boardPlayer2, _player1, _player2, col, row);
    } else if (socket == _player2.socket)
    {
        _player2.nShoots++;
        makeShooted(boardPlayer1, _player2, _player1, col, row);
    }

}


bool SinkTheShipServer::addPlayer(const int socket, std::shared_ptr<SocketState> socketState) {
    if (_free) {
        _player1.socket = socket;
        _player1.username = socketState -> user;
        _player1.socketState = socketState;
        _player1.nShoots = 0;
        _free = false;
        return true;
    } else if (!_started) {
        _player2.socket = socket;
        _player2.username = socketState -> user;
        _player2.socketState = socketState;
        _player2.nShoots = 0;
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
    string response = "+Ok. Turno de partida";
    _player1.socketState -> isYourTurn = true;
    send(_player1.socket, response.c_str(), response.length(), 0);
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
    // placeBoat(boardPlayer1, 4);
    // placeBoat(boardPlayer1, 3);
    // placeBoat(boardPlayer1, 3);
    // placeBoat(boardPlayer1, 2);
    placeBoat(boardPlayer1, 2);
    
    // placeBoat(boardPlayer2, 4);
    // placeBoat(boardPlayer2, 3);
    // placeBoat(boardPlayer2, 3);
    // placeBoat(boardPlayer2, 2);
    placeBoat(boardPlayer2, 2);

}

void placeBoat(vector<vector<Cell>>& board, int size) {
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
bool isValidPlacement(vector<vector<Cell>> board, int x, int y, int direction, int size) {
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
            out += getCellString(board[j][k]) + (k<BOARD_SIZE-1? ",": "");
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
    cout << "board:" << board << endl;
    _boardSize = boardSize;

    // Initialize a vector to store the matrix
    _myBoard = vector< vector<Cell> >(_boardSize, vector<Cell>(_boardSize, Cell::Water));


    vector<string> filas;
    size_t startPos = 0, end = 0;
    while (((end = board.find(';', startPos)) != string::npos) or ((end = board.find('.', startPos)) != string::npos)) {
        string fila = board.substr(startPos, end - startPos);
        filas.push_back(fila);
        startPos = end + 1;
        cout << "fila" << filas.back() << endl;
    }

    for (int i = 0; i < _boardSize; ++i) {
        for (int j = 0; j < _boardSize; ++j) {
            char valor = filas[i][2*j];

            // Asignar Water o Boat según el valor del carácter
            if (valor == 'A') {
                _myBoard[i][j] = Cell::Water;
            } else if (valor == 'B') {
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
    size_t pos;
    int x;
    char y;
    if ((pos=buffer.find("AGUA: ")) != string::npos)
    {
        pos += strlen("AGUA: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2))-1;
        _opponentBoard[x][cellPosinChar2Int(y)] = Water;
        showBoard(buffer);
        cout << "Agua" << endl;
    } else if ((pos=buffer.find("TOCADO: ")) != string::npos)
    {
        pos += strlen("TOCADO: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2))-1;
        _opponentBoard[x][cellPosinChar2Int(y)] = Touched;
        showBoard(buffer);
        cout << "Tocado" << endl;
    } else if ((pos=buffer.find("HUNDIDO: ")) != string::npos)
    {
        pos += strlen("HUNDIDO: ");
        y = buffer[pos];
        x = stoi(buffer.substr(pos+2))-1;
        markShipAsSunk(_opponentBoard, x, cellPosinChar2Int(y));
        showBoard(buffer);
        cout << "Hundido" << endl;
    } else if ((pos=buffer.find("Disparo en: ")) != string::npos)
    { 
        pos+= strlen("Disparo en: ");
        cout << buffer << endl;
        cout << "Columna:" <<  buffer[pos] <<", fila:"<< buffer[pos+2] <<endl;
        y = buffer[pos];
        x= stoi(buffer.substr(pos+2))-1;
        bool touched = (_myBoard[x][cellPosinChar2Int(y)]==Boat);

        if (touched)
        {
            _myBoard[x][cellPosinChar2Int(y)]=Touched;
            vector <pair<int,int> > visited;
            if (isShipSunk(_myBoard, x, cellPosinChar2Int(y), visited))
            {
                touched = false;
                markShipAsSunk(_myBoard, x, cellPosinChar2Int(y));
                cout << "hundido" << endl;
            }
        }
        showBoard(buffer);
        cout << "El oponente ha disparado en " << y << x+1;
        if (touched)
            cout << ". Ha tocado barco" << endl;
        else if (_myBoard[x][cellPosinChar2Int(y)] == Floaded)
            cout << ". Ha hundido un barco" << endl;
        else
            cout << ". Ha tocado agua" << endl;
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
    cout << "   -------------------" << endl;
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
                case Unkwown: cout << 'D'; break;
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
    return (int) c-'A';
}





bool buscarPar(const std::vector<std::pair<int, int>>& vec, const std::pair<int, int>& elemento) {
    for (const auto& par : vec) {
        if (par == elemento) {
            return true; // Elemento encontrado
        }
    }
    return false; // Elemento no encontrado
}


bool isInsideBoard(int row, int col, int numRows, int numCols) {
    return row >= 0 && row < numRows && col >= 0 && col < numCols;
}


bool isShipSunk(vector <vector <Cell> > board, int row, int col, vector<pair<int,int>> &visited)
{
    if (buscarPar(visited, pair<int,int>(row, col)))
    {
        return true;
    }

    visited.push_back(pair<int,int>(row, col));

    if (board[row][col] == Boat)
        return false;
    
    if (board[row][col] == Water)
        return true;
    
    int i, j;

    // Abajo
    i = row-1; 
    j = col;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()) && !isShipSunk(board, i, j, visited))
        return false;
    
    // Izquierda
    i = row; 
    j = col-1;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()) && !isShipSunk(board, i, j, visited))
        return false;

    // Arriba
    i = row+1; 
    j = col;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()) && !isShipSunk(board, i, j, visited))
        return false;

    // Derecha
    i = row; 
    j = col+1;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()) && !isShipSunk(board, i, j, visited))
        return false;


    // Si todas las celdas del barco están tocadas, entonces el barco está hundido
    return true;
}










void markShipAsSunk(vector< vector <Cell> > &board, int row, int col)
{
    board[row][col] = Floaded;
    // Comprobar hacia arriba
    for (int r = row - 1; r >= 0; r--) {
        if (board[r][col] == Cell::Touched) {
            board[r][col] = Cell::Floaded;
        } else {
            break; // No hay más celdas del barco hacia arriba
        }
    }
    // Comprobar hacia abajo
    for (int r = row + 1; r < BOARD_SIZE; r++) {
        if (board[r][col] == Touched) {
            board[r][col] = Floaded;
        } else {
            break; // No hay más celdas del barco hacia abajo
        }
    }
    // Comprobar hacia la izquierda
    for (int c = col - 1; c >= 0; c--) {
        if (board[row][c] == Touched) {
            board[row][c] = Floaded;
        } else {
            break; // No hay más celdas del barco hacia la izquierda
        }
    }
    // Comprobar hacia la derecha
    for (int c = col + 1; c < BOARD_SIZE; c++) {
        if (board[row][c] == Cell::Touched) {
            board[row][c] = Cell::Floaded;
        } else {
            break; // No hay más celdas del barco hacia la derecha
        }
    }

}


void markShipAsSunk(vector <vector <Cell> > &board, int row, int col, vector<pair<int,int>> &visited)
{
    if (buscarPar(visited, pair<int,int>(row, col))) // Está ya explorado?
    {
        visited.push_back(pair<int,int>(row, col));
        return ;
    }

    visited.push_back(pair<int,int>(row, col));

    if (board[row][col] != Touched)
        return ;
    
    int i, j;

    // Abajo
    i = row-1; 
    j = col;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()))
        markShipAsSunk(board, i, j, visited);

    // Izquierda
    i = row; 
    j = col-1;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()))
        markShipAsSunk(board, i, j, visited);

    // Arriba
    i = row+1; 
    j = col;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()))
        markShipAsSunk(board, i, j, visited);

    // Derecha
    i = row; 
    j = col+1;
    if (i >= 0 && i < int(board.size()) && j >= 0 && j < int(board[0].size()))
        markShipAsSunk(board, i, j, visited);


}
