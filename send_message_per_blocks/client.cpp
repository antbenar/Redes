/* Client code in C */
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <string>

using namespace std;

const long long int buff_size = 1000;

int hash(string buff){
  int T=0;
  unsigned int sis = buff.length();
  for(unsigned int i=0;i<sis;++i){
    T += (int)buff[i];
  }
  return T;
}

char* read_text ( string directory, int& size ) {

  ifstream is ( ("texts/" + directory + ".txt").c_str(), std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    size = is.tellg();
    is.seekg (0, is.beg);

    char * buffer = new char [size];

    // read data as a block:
    is.read (buffer,size);

    if (is);
      //::cout << buffer << endl;
    else
      std::cout << "error: only " << is.gcount() << " could be read";
    
    is.close();
    return buffer;
  }
  return 0;
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
	int Res;
	int ConnectFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	int n;
	char buffer[buff_size];
	string aux;

	if (-1 == ConnectFD)
	{
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1110);
	Res = inet_pton(AF_INET, "192.168.122.1", &stSockAddr.sin_addr);

	if (0 > Res)
	{
		perror("error: first parameter is not a valid address family");
		close(ConnectFD);
		exit(EXIT_FAILURE);
	}
	else if (0 == Res)
	{
		perror("char string (second parameter does not contain valid ipaddress");
		close(ConnectFD);
		exit(EXIT_FAILURE);
	}

	if (-1 == connect(ConnectFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
	{
		perror("connect failed");
		close(ConnectFD);
		exit(EXIT_FAILURE);
	}

	int SocketFD = accept(ConnectFD, NULL, NULL);

	
	string userId = " Julio", textDirectory = "story1";
	/*
	cout << "Put the user id: ";
	cin >> userId;

	cout << "text to read: ";
	cin >> textDirectory;
	*/

	int size;
	char* bufferStory = read_text( textDirectory , size);

	string buf = "A" + userId + fill_string(to_string(size),6) + bufferStory;
	//cout << buf << endl;

	///Send message by blocks
	for (int i = 0; i< size; i+=buff_size){
		write(ConnectFD, buf.substr(i,buff_size).c_str() , buff_size);
		//cout << buf.substr(i,buff_size);
	}
	write(ConnectFD,buf.substr(size - (size % buff_size),size % buff_size).c_str(), size % buff_size);
	//cout << buf.substr(size - (size % buff_size),size % buff_size)<<endl;

	bzero(buffer,buff_size);
	n = read(ConnectFD,buffer,1);
	
	if (buffer[0] == 'B'){
		bzero(buffer,buff_size);
		read(ConnectFD,buffer,6);
		//convertir buffer a entero
		int sizeMessage = atoi(buffer);
		
		bzero(buffer,buff_size);
		read(ConnectFD,buffer,sizeMessage);
		
		int hash_server = atoi(buffer);
		int hash_client = ::hash(bufferStory);
		//cout << bufferStory << endl;
		
		cout << hash_server << " " << hash_client << endl; 
		if ( hash_server == hash_client ) cout << "OK" << endl;
		else cout << "Different hash" << endl;
	}
	
	shutdown(ConnectFD, SHUT_RDWR);
	close(ConnectFD);

	return 0;
}



