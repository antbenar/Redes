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
  
int main(void)
{

	int SocketFD = create_client();

	string buffer;
	char buf[256];

	string client_user = "Anthony";
	write(SocketFD,client_user.c_str(),client_user.size());
	char server_user[256];
	read(SocketFD,server_user,255);

	cout << "Server: [" << server_user <<"] - Client: [" << client_user << "]" << endl;

	while( buf != "END" ){
		cout << "[" << client_user << "]: ";
		cin >> buffer;
		write(SocketFD, buffer.c_str()	, buffer.size());

		if ( buffer == "END" ) break;

		bzero(buf,256);
		read(SocketFD,buf,255);
		cout << "[" << server_user << "]: " << buf << endl;
	}    


	shutdown(SocketFD, SHUT_RDWR);

	close(SocketFD);
	return 0;
}

  
 // fuser -k -n tcp 1100
  
  
  
