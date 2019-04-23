  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <string>
  #include <iostream>
  using namespace std;
  
  
  
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

 
int main(void)
{
	int SocketFD = create_socket();
 
    for(;;)
    {
		int ConnectFD = accept(SocketFD, NULL, NULL);

		string buffer;
		char buf[256];

		char client_user[256];
		read(ConnectFD,client_user,255);
		string server_user = "Mario";
		write(ConnectFD,server_user.c_str(),server_user.size());

		cout << "Server: [" << server_user <<"] - Client: [" << client_user << "]" << endl;
     
 	 
		while( buffer != "END" ){
			bzero(buf,256);
			read(ConnectFD,buf,255);
			cout << "[" << client_user << "]: " << buf << endl;

			if ( buf == "END" ) break;

			cout << "[" << server_user << "]: ";
			cin >> buffer;
			write(ConnectFD,buffer.c_str()	, buffer.size());

		} 

		shutdown(ConnectFD, SHUT_RDWR);

		close(ConnectFD);
	}

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

	F(Frebies) F0405 [Server ---> all clients]
	M(Move TDRL) M#L [Client ---> Server][Server ---> All Clients]

*/
