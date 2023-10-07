#ifndef SINKTHESHIP_HPP
#define SINKTHESHIP_HPP

class SinkTheShipClient {
    public:
        SinkTheShipClient();


        // Plays a turn
        // @param buffer received from server
        void playTurn(string buffer);

};


#endif