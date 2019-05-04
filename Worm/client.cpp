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
Worm* worm;

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
	Res = inet_pton(AF_INET, "192.168.122.1", &stSockAddr.sin_addr);
	
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

void read_server(){
	string buffer;
	char buf[100];
	
	while (true){
		
		bzero(buf,100);
		read(SocketFD , buf , 1);
		
		if(buf[0] == 'M'){		///M(Move TDRL) M#L [Client ---> Server][Server ---> All Clients]
			bzero(buf,100);
			read(SocketFD , buf , 2);
			worm->move(buf[0], buf[1]);
			cout << "Movimiento ejecutado: " << buf << endl;
		}
		else if(buf[0] == 'F'){		///F(Frebies) F0405 [Server ---> all clients]
			bzero(buf,100);
			read(SocketFD , buf , 4);
			
			string buffer = buf;
			worm->add_freezbie( atoi(buffer.substr(0,2).c_str()) , atoi(buffer.substr(2,2).c_str()) );
			cout << "Freezbie anadido: " << buffer.substr(0,2).c_str() << " " << buffer.substr(2,2).c_str() << endl;
		}
		else if(buf[0] == 'P'){		///P(new player) P# [Server -> all clients]
			bzero(buf,100);
			read(SocketFD , buf , 1);
			//worm->add_new_player( buf[0] );	
			cout << "Nuevo jugador con avatar: " << buf[0] << endl;
		}
		else if(buf[0] == 'U'){	///U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
			char megabuf[10000];
			bzero(megabuf,10000);
			read(SocketFD , megabuf , COLS*ROWS);
			buffer = megabuf;
			cout << "Matriz actualizada(size): " << buffer.size()	 << endl;
			worm->change_tab(buffer);
		}
		else if(buf[0] == 'Q'){	///Q(send avatars of all players) 02Q04056778#08094546 [Server ---> client] id head tail
			bzero(buf,100);
			read(SocketFD , buf , 2);
			int cantidad = atoi(buf);
			
			bzero(buf,100);
			read(SocketFD , buf , 9*cantidad);
			worm->add_many_players(cantidad, buf);
			cout << "Lista de jugadores actualizada: " << buf << endl;
		}
	}
}


void write_move( string direction ){	//------------------------------Escribir teclas cliente
	string buffer = "M" + current_id + direction;
	write(SocketFD, buffer.c_str() , 3);
}

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
		write_move("T");
		break;
	case GLUT_KEY_DOWN:
		write_move("D");
		break;
	case GLUT_KEY_RIGHT:
		write_move("R");
		break;
	case GLUT_KEY_LEFT:
		write_move("L");
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
	char buffer[256];
	string buf;
	char answer = 'V';
	
	SocketFD = create_client();
	
	//------------------------------Inicializar cliente
	
	while ( answer == 'V' ){
		///A(Avatar) A# [Client ---> Server]     --- create new player
		string buf;
		cout << "Ingrese nuevo avatar: ";
		cin >> current_id;
		buf = "A" + current_id;
		write(SocketFD, buf.c_str() , 2);
		
		///V(Avatar is in use) [Server ---> Client]
		///O(Avatar has been assigned) [Server ---> Client]
		read(SocketFD, buffer , 1);
		answer = buffer[0];
		
		if(answer == 'V'){
			cout << "Avatar existente." << endl;
			
		}
		else {
			cout << "Avatar asignado." << endl;
		}
	}
	thread(read_server).detach();

	
	worm = new Worm;	
	//worm->add_new_player( '#' );
	
	glutInit(&argc, argv); 
	InitGl();
		
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}

  
 // fuser -k -n tcp 1100
  
  
  
