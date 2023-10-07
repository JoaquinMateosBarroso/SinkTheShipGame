#include <sinkTheShip.hpp>

using namespace std;


/***********************************************************
 * SinkTheShipClient  
************************************************************/
SinkTheShipClient::SinkTheShipClient()
{
    _isGameOpen = false;
}

void SinkTheShipClient::start(string board)
{
    _isGameOpen = true;
    
    // TODO
    cout << "game starting" << endl;
}

bool SinkTheShipClient::isGameOpen()
{
    return _isGameOpen;
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
    std::cout << "Here goes the board" << endl << endl;
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

