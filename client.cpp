#include <client.hpp>
#include <sinkTheShip.hpp>


#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <err.h>


using namespace std;
int main ( )
{
    SinkTheShipClient game;
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;
	
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
        exit (1);
	}

   
    
	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do {
        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        if(FD_ISSET(sd, &auxfds)){ // Message received from server
            
            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);

            string sbuffer = buffer;
            
            bool isError = manageError(sbuffer);
            if (!isError)
            {
                if (game.isGameOpen())
                {
                    game.playTurn(sbuffer);
                    game.showBoard();
                }
                else
                {
                    bool gameBegins = manageNonGameOk(sbuffer);
                    if (gameBegins)
                    {
                        game.start(sbuffer.substr(24));
                        game.showBoard();
                    }
                }
            }
        }
        else // Message not received from server
        {
            if(FD_ISSET(0,&auxfds)){ // Text introduced via keyboard
                bzero(buffer,sizeof(buffer));
                
                fgets(buffer,sizeof(buffer),stdin);
                
                if(strcmp(buffer,"SALIR\n") == 0){
                        fin = 1;
                }

                send(sd,buffer,sizeof(buffer),0);
            }
        }				
    } while(!fin);
		
    close(sd);

    return 0;
		
}




bool manageError(string buffer)
{
    if (buffer.compare(0, 3, "-Err")) // It is not -Err
    {
        return false;
    }

    cout << buffer << endl;

    return true;
}

bool manageNonGameOk(string buffer)
{
    if (buffer.compare(0, 3, "+Ok")) // It is not +Ok
    {
        throw runtime_error(
            "A not recognized code was received\n:"+buffer);
    }

    bool gameBegins = !buffer.find("+Ok. Empezamos partida");
    if (!gameBegins)
    {
        cout << "Message received from server:" << endl <<
                buffer << endl;
    }

    return gameBegins;
}
