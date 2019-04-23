/* Server code in C */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
using namespace std;
int jash=0;
string client;
unsigned int sizeMs;

const long long int buff_size = 1000;

int hash(string buff){
  int T=0;
  unsigned int sis = buff.length();
  for(unsigned int i=0;i<sis;++i){
    T += (int)buff[i];
  }
  return T;
}

string fill_string(string a, int size){  // llena con 0 por delante
	for(int i=a.size(); i<size;++i){
		a = "0" + a;
	}
	return a;
}

int main(void)
{
	struct sockaddr_in stSockAddr;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	char bufffer[buff_size];
	string buffer;

	int n;

	if(-1 == SocketFD)
	{
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1110);
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
	for(;;){
		int ConnectFD = accept(SocketFD, NULL, NULL);

		if(0 > ConnectFD)
		{
			perror("error accept failed");
			close(SocketFD);
			exit(EXIT_FAILURE);
		}

		for(;;){
			bzero(bufffer,buff_size);
			read(ConnectFD,bufffer,1);
			if( bufffer[0] != 'A' ) continue;
			
			///read user id
			bzero(bufffer,buff_size);
			read(ConnectFD,bufffer,6);
			buffer = bufffer;
			cout<< "user id: " << buffer << endl;
			
			///read size
			bzero(bufffer,buff_size);
			read(ConnectFD,bufffer,6);
			sizeMs = atoi(bufffer);
			cout<< "size Message: " << sizeMs << endl;
			
			///read message
			for (int i = 0; i< sizeMs; i+=buff_size){
				bzero(bufffer,buff_size);
				read(ConnectFD,bufffer,buff_size);
														//cout << bufffer;
				buffer = bufffer;
				jash += ::hash(buffer);
			}
			bzero(bufffer,buff_size);
			read(ConnectFD,bufffer,sizeMs % buff_size);
														//cout << bufffer<<endl;
			buffer = bufffer;
			jash += ::hash(buffer);
			
			cout<< "hash: " << jash << endl;
			
			string jash_ = to_string(jash);
			string size_hash = fill_string( to_string(jash_.size()), 6 );
			buffer = "B" + size_hash + jash_.c_str();	//1+6+msg.size()
			//cout << buffer << endl;
			n = write(ConnectFD, buffer.c_str() , jash_.size()+7);
			if (n < 0) perror("ERROR writing to socket");
		}

		shutdown(ConnectFD, SHUT_RDWR);
		close(ConnectFD);
		close(SocketFD);
		return 0;
	}
}
