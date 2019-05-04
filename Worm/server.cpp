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


void read_client( int client){
	//---------------------------------------create new player before add to game-------------------------//
	string buffer;
	char buf[256];
	
	///A(Avatar) A# [Client ---> Server] 
	bzero(buf,256);
	read(  client , buf , 2 );
	if ( buf[0] != 'A' ) {
		cout << "error de protocolo al enviar avatar";
		return;
	}
	
	while( worm->exist_player(buf[1]) ){
		///V(Avatar is in use) [Server ---> Client]
		buf[0] = 'V';
		write( client, buf , 1 );  
	}
	///O(Avatar has been assigned) [Server ---> Client]	  //responder al cliente que su avatar fue creado
	buf[0] = 'O';
	write( client, buf , 1 );  

	worm->add_new_player(buf[1]);//agregar nuevo jugador
	cout << "Nuevo jugador ingreso con avatar: " << buf[1] << endl;
	
	///P(new player) P# [Server -> all clients]		//comunicar a todos los demas player del nuevo jugador
	buffer = "P";
	buffer.push_back(buf[1]);
	for (int i=0; i<clients.size(); ++i){
		write( clients[i], buffer.c_str() , 2 );     
	}
	
	///U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
	buffer = "U" + worm->get_tablero();
	write( client, buffer.c_str() , buffer.size() );  
	
	///Q(send avatars of all players) Q04#P63 [Server ---> client] 
	buffer = "Q" + worm->get_players();
	write( client, buffer.c_str() , buffer.size() );  
	
	
	//--------------------------------------- END create new player before add to game-------------------------//
	
	while( true ){
		///Read type of message
		bzero(buf,256);
		read( client , buf , 1 );
		
		if ( buf[0] == 'M' ){	///M(Move TDRL) M#L [Client ---> Server]
			read( client , buf , 2 );
			worm->move(buf[0],buf[1]);
			cout << "Jugador " << buf[0] << " movio: " << buf[1] <<endl;	
			
			///M(Move TDRL) M#L [Server ---> All Clients]
			buffer = buf;
			buffer.insert(0,"M");
			for (int i=0; i<clients.size(); ++i){
				write( clients[i], buffer.c_str() , 3 );     
			}

			if(worm->check_add_freezbie()){
				///F(Frebies) F0405 [Server ---> all clients]	enviar notificacion a todos los jugadores de que hay un nuevo freezbie
				buffer = "F" + worm->get_freezbiex() + worm->get_freezbiey();
				for (int i=0; i<clients.size(); ++i){
					write( clients[i], buffer.c_str() , buffer.size() );     
				}
			}
		}	
	}
	
	shutdown(client, SHUT_RDWR);
	close(client);
}


//-------------------------------------------_OPENGL------------------------------------------//

void display(void) 
{  
	glClear( GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glOrtho(-10.0f,  COLS + 10, -10.0f, ROWS + 10, -1.0f, 1.0f);  
	worm->drawSquare();
	
	glutPostRedisplay();
}

void window_key_glut(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		//worm->add_freezbie();
		
		break;
	default:
		break;
	}
	//glutPostRedisplay();
}

void InitGl(){
	glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowPosition(20,0); 
	glutInitWindowSize(800,800); 
	glutCreateWindow ("Worm");
	
	glClearColor(0.0, 0.0, 0.0, 0.0);         // black background 
	glMatrixMode(GL_PROJECTION);              // setup viewing projection 
	glutDisplayFunc(display); 
	glutSpecialFunc(&window_key_glut);	
	glutMainLoop();
}




int main(int argc, char **argv)
{
	SocketFD = create_socket();

	worm = new Worm;
	
	while(true){
		int client =  accept(SocketFD, NULL, NULL);
		if(SocketFD < 0) continue;
		
		clients.push_back( client );
		thread(read_client, client).detach();
	}
	glutInit(&argc, argv); 
	InitGl();

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
