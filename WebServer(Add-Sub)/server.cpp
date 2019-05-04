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
#include <vector>
#include <utility>
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

vector<pair<string,string>> separete(string P){
    vector<pair<string,string>> Param;
    size_t siz = P.size() , i;
    string id = "",val = "";
    bool s = 1;

    pair<string, string> d = make_pair("", "");
    for(i=0; i<siz ; ++i){
        if (P[i] == '='){
            s = 0;
            d.first = id;
            ++i;
        }
        if(s)
            id += P[i];
        else
            val += P[i];
        if (P[i] == '&'){
            val.erase(val.begin() + (val.size() -1));
            d.second = val;
            id = "";
            val = "";
            Param.push_back(d);
            s=1;
        }
    }
    d.second = val;
    Param.push_back(d);
    return Param;
}

string read_page( string& url ){	//leer el archivo al que se redirecciona la pagina (add.html?T1=1&T2=2&T3=3&B1=Add) -> add.html
	string res = "";
	int i;
	for(i=0; i<url.size() ; ++i){	//recorrer hasta la parte de los atributos
		if(url[i] == '?') break;
		res += url[i];
	}
	if ( res.size() == url.size())return res;

	url.erase(url.begin(), url.begin()+i+1);
	return res;
}



string add_html( vector<pair<string,string>> values ){
	int suma = atoi(values[0].second.c_str()) + atoi(values[1].second.c_str());
	string html = "<!DOCTYPE html><html><head><title>WEB SERVER - Calculator</title></head><body><h1>Add Operation</h1><p><a href='/'>----main</a></p><p>The result is: </p>"+values[0].second + " + " + values[1].second + " = " + to_string(suma) + "</body></html>";
	return html;
}

string sub_html( vector <pair<string,string>> values ){
	int resta = atoi(values[0].second.c_str()) - atoi(values[1].second.c_str());
	string html = "<!DOCTYPE html><html><head><title>WEB SERVER - Calculator</title></head><body><h1>Sub Operation</h1><p><a href='/'>----main</a></p><p>The result is: </p>"+values[0].second + " - " + values[1].second + " = " + to_string(resta) + "</body></html>";
	return html;
}

void leer_de(int ConnectFD){
	char bufffer[buff_size];
	string buffer, response, html;

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
	if(url == ""){
		url = "index.html";
		html = abrir( "html/" + url );
		response = header_response + to_string(html.size()) + "\n\n" + html;
	}
	else {
		string cabecera = read_page(url);
		cout << cabecera << endl;
		if (url.size() != cabecera.size() ) {
			vector <pair<string,string>> values = separete(url);
			if( cabecera == "add.html"){
				html = add_html(values);
			}
			else if( cabecera == "sub.html"){
				html = sub_html(values);
			}
			response = header_response + to_string(html.size()) + "\n\n" + html;
		}		
	}
	
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