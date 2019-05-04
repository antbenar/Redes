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
#include <fstream>
#include <thread>
using namespace std;

const int buff_size = 1000;
string client;
string header_response;


int createServer(){
	struct sockaddr_in stSockAddr;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(-1 == SocketFD)
	{
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(80);
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


string abrir(string tex){
    string msn = "";
    string line;
    ifstream file(tex.c_str());
    while(!file.eof()){
        getline(file, line);
        msn += line;
        msn += '\n';
    }
    file.close();
	msn.erase(msn.end() - 1,msn.end());
	return msn;
}

string getRequest(string &MSN){
    size_t siz = MSN.size() , i;
    bool space = 0;
    string req = "";
    for(i=0; i<siz ; ++i){
        if (MSN[i] == '/')
            space = 1;
        if (space)
            req += MSN[i];
        if(space && MSN[i] == ' ')
            break;
    }
	req.erase(req.begin(),req.begin()+1);
    req.erase(req.begin() + (req.size() -1));
    return req;
}

void leer_de(int ConnectFD){
	char bufffer[buff_size];
	string buffer;

	int n;

   bzero(bufffer,buff_size);
	/*
	while (true){
		read(ConnectFD,bufffer,1);
		buffer.push_back(bufffer[0]);
		if(bufffer[0] == '\n'){
			read(ConnectFD,bufffer,1);
			if(bufffer[0] == '\n') break;
			buffer.push_back(bufffer[0]);
		}
	}
	*/
    bzero(bufffer,1000);////
    read(ConnectFD, bufffer, 1000);///
	buffer = bufffer;

	cout << buffer;

	string url = getRequest(buffer);
	cout << url << endl;
	if(url == "") url = "index.html";

	string html = abrir( "html/" + url );

	string response = header_response + to_string(html.size()) + "\n\n" + html;
	//cout << response << endl;

	n = write(ConnectFD, response.c_str() , response.size());
	if (n < 0) perror("ERROR writing to socket");

	shutdown(ConnectFD, SHUT_RDWR);
	close(ConnectFD);
}

int main(void)
{
	char bufffer[buff_size];
	string buffer;

	int n;

	int SocketFD = createServer();

	header_response = abrir("header_response.txt");
	//header_response.erase(header_response.end() - 1,header_response.end());

	for(;;){
		int ConnectFD = accept(SocketFD, NULL, NULL);
		if(ConnectFD<0) return 0;

		thread(leer_de, ConnectFD).detach();
		
	}
	close(SocketFD);
	return 0;
}
//sudo nc -l 80

/*
//sudo nc ucsp.edu.pe 80
GET / HTTP/1.1
Host: ucsp.edu.pe
User-Agent: Mozilla/5.0
*/