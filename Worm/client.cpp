#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <thread> 
#include <string>
#include <iostream>
#include "worm.cpp"
using namespace std;

int SocketFD;
string current_id; //#

int create_client(){
	struct sockaddr_in stSockAddr;
	int Res;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	int n;
	
	if (-1 == SocketFD)
	{
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}
	
	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
	
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1100);
	Res = inet_pton(AF_INET, "192.168.1.60", &stSockAddr.sin_addr);
	
	if (0 > Res)
	{
		perror("error: first parameter is not a valid address family");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	else if (0 == Res)
	{
		perror("char string (second parameter does not contain valid ipaddress");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
	if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
	{
		perror("connect failed");
		close(SocketFD);	
		exit(EXIT_FAILURE);
	}
	return SocketFD;
}



void client_write(){
	string buffer;
	
	while(true){
		string buffer;
		cin >> buffer;
		buffer = "M" + current_id + buffer;
		write(SocketFD, buffer.c_str() , 3);
	}
}

void client_read(){
	string buffer;
	char buf[256];

	while (true){
		///P(new player) P# [Server -> all clients]
		bzero(buf,256);
		read(SocketFD , buf , 2);
		cout << "Nuevo jugador con avatar: " << buf[1] << endl;
	}
}
  
int main(void)
{

	SocketFD = create_client();

	///A(Avatar) A# [Client ---> Server]     --- create new player
	string buffer;
	cout << "Ingrese avatar: ";
	cin >> current_id;
	buffer = "A" + current_id;
	write(SocketFD, buffer.c_str() , 2);
	
	///V(Avatar is in use) [Server ---> Client]
	
	///O(Avatar has been 	assigned) [Server ---> Client]
	
	///U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
	///Q(send avatars of all players) Q04#P63 [Server ---> client] 
	
	
	thread t_read(client_read);
	//thread (client_write).detach();
	
	t_read.join();
	
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}

  
 // fuser -k -n tcp 1100
  
  
  
