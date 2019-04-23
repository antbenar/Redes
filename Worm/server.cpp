#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <thread> 
#include <vector>
#include <string>
#include <iostream>
#include "worm.cpp"
using namespace std;

Worm* worm;
vector <int> clients;
int SocketFD;
	
int create_socket(){
	struct sockaddr_in stSockAddr;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	int n;

	if(-1 == SocketFD)
	{
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1100);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;

	if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
	{
		perror("error bind failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if(-1 == listen(SocketFD, 10))
	{
		perror("error listen failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	return SocketFD;
}


void server_read( int client ){
	string buffer;
	char buf[256];
	
	while( true ){
		///Read type of message (A, P, V, etc)
		bzero(buf,256);
		read( client , buf , 1 );
		
		cout << "Type of message: " << buf[0] << " " << buf[1] << " " << buf[2] <<endl;
		/*
		if ( buf[0] == 'M' ){
		///M(Move TDRL) M#L [Client ---> Server][Server ---> All Clients]
		read( clients['#'] , buf , 2 );
		cout << "Jugador " << buf[0] << " movio: " << buf[1] <<endl;	
		
		}
		
		///F(Frebies) F0405 [Server ---> all clients]
		*/
		
	}
	
}

void accept_clients( int client){
	//---------------------------------------create new player before add to game-------------------------//
	string buffer;
	char buf[256];
	
	///A(Avatar) A# [Client ---> Server] 
	bzero(buf,256);
	read(  client , buf , 2 );
	cout << "Nuevo jugador ingreso con avatar: " << buf[1] << endl;
	
	///V(Avatar is in use) [Server ---> Client]
	
	///O(Avatar has been assigned) [Server ---> Client]			//responder al cliente que su avatar fue creado
	
	
	///P(new player) P# [Server -> all clients]
	buffer = "P";
	buffer.push_back(buf[1]);
	for (int i=0; i<clients.size(); ++i){
		write( clients[i], buffer.c_str() , 2 );     
	}
	
	///U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
	///Q(send avatars of all players) Q04#P63 [Server ---> client] 
	
	
	//--------------------------------------- END create new player before add to game-------------------------//
	
	//thread(server_read, client).detach();
	
	shutdown(client, SHUT_RDWR);
	close(client);
}

int main()
{
	SocketFD = create_socket();

	worm = new Worm;
	
	while(true){
		int client =  accept(SocketFD, NULL, NULL);
		clients.push_back( client );
		
		thread(accept_clients, client).detach();
	}
	
	//for (auto& th : threads) th.join();
	
	close(SocketFD);
	return 0;
}
	
   // fuser -k -n tcp 1100 //cancelar socket

  /*
	Marix 80 x 80

	Msg Types:
	A(Avatar) A# [Client ---> Server] 
		V(Avatar is in use) [Server ---> Client]
		O(Avatar has been assigned) [Server ---> Client]
			/// nuevo player asi que tiene que empezar con la matriz que recibe
			U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
			Q(send avatars of all players) Q04#P63 [Server ---> client] 
  
			P(new player) P# [Server -> all clients] 

	M(Move TDRL) M#L [Client ---> Server][Server ---> All Clients]

	F(Frebies) F0405 [Server ---> all clients]

*/
