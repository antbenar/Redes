/*********************************************************************************
	Autores:
	- Anthony Benavides Arce
	- Juan Pablo Heredia Parillo

	Requiere:
	- Linux Ubuntu
	- Opengl2.6
	- C++11
/*********************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <utility>
#include <thread> 
#include <string>
#include <iostream>
#include "Cars.cpp"
using namespace std;

int SocketFD;
string current_id; //#
Cars* cars;
int var_y = 15;
int speed = 2;
vector<pair<int,int>> derecha;
vector<pair<int,int>> izquierda;

void window_key_glut(int key, int x, int y);



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
	char buffer[256];
	string buf;
	
	for(;;){
		bzero(buffer,256);
		read(SocketFD, buffer, 1);
		
		if(buffer[0] == 'T'){
			
			//--------------------leer flag
			bzero(buffer,256);
			read(SocketFD, buffer, 1);
			string flag_ = buffer;
			int flag = atoi(flag_.c_str());
			
			//--------------------leer cantidad de jugadores
			bzero(buffer,256);
			read(SocketFD, buffer, 2);
			string amount_players_ = buffer;
			int amount_players = atoi(amount_players_.c_str());
			
			bzero(buffer,256);
			read(SocketFD, buffer, 5*amount_players);

			cars->move(amount_players, flag, buffer);
			var_y -= speed;
			cout<<var_y<<endl;
		}
		else if(buffer[0] == 'S'){
			bzero(buffer,256);
			read(SocketFD, buffer, 1);
			if(buffer[0] == 'W')cout << "GANASTE!" << endl;
			else cout << "Perdiste :(" << endl;
		}
		
		
		if(SocketFD < 0) break;
	}
}
  

void write_move( string direction ){	//------------------------------Escribir teclas cliente
	string buffer = "M" + direction;
	write(SocketFD, buffer.c_str() , 2);
}
void load(){
	for(int i = -(ROWS+10);i<(ROWS+15);++i){
		izquierda.push_back(make_pair(rand()%2-2,i));
		derecha.push_back(make_pair(rand()%2+COLS+1,i));
	}
}

void display(void) 
{  
	glClear( GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glOrtho( -3.0f,  COLS + 3,-(ROWS + 3), 4.0f, -1.0f, 1.0f);  
	glPushMatrix();
		glTranslatef(0,var_y,0);
		int x = rand()%2-2;
		int x_2 = rand()%2 + COLS+2;
		glPushMatrix();
		glColor3f(0.3f,1.0f,0.2f);
		glBegin(GL_LINE_STRIP);
		for(int i=0;i<izquierda.size();++i)
			glVertex2d(izquierda[i].first , izquierda[i].second);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for(int i=0;i<derecha.size();++i)
			glVertex2d(derecha[i].first , derecha[i].second);
		glEnd();
		if(var_y < -15) var_y = 8;
	glPopMatrix();
	
	cars->drawSquare();
	
	glPushMatrix();
		glColor3f(1.0f,0.0f,0.0f);
		glBegin(GL_LINES);
			glVertex2d(-3.0f , -(ROWS+1));
			glVertex2d(COLS +3 , -(ROWS+2));
		glEnd();
	glPopMatrix();
//	glutPostRedisplay();
	glutSwapBuffers();
//	glFlush();
	glutPostRedisplay();
}

void window_key_glut(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		write_move( "U" );
		//cars->move("0310615#0528@1430*");//2
		break;
		
	case GLUT_KEY_DOWN:
		write_move( "D" );
		//cars->move(3,1,"0616#0529@1431*");//3
		break;
		
	case GLUT_KEY_RIGHT:
		write_move( "R" );
		//cars->move(3,1,"0617#0530@1432*");//4
		break;
		
	case GLUT_KEY_LEFT:
		write_move( "L" );
		//cars->move(3,1,"0614#0527@1429*");//1
		break;
	default:
		break;
	}
	//glutPostRedisplay();
}

void InitGl(){
	glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowPosition(20,0); 
	glutInitWindowSize(300,500); 
	glutCreateWindow ("Cars");
	
	glClearColor(0.2, 0.1, 0.1, 0.0);         // black background 
	glMatrixMode(GL_PROJECTION);              // setup viewing projection 
	glutDisplayFunc(display); 
	glutSpecialFunc(&window_key_glut);	
	glutMainLoop();
}



int main(int argc, char **argv)
{
	load();
	char buffer[256];
	string buf;
	char answer = 'N';
	
	SocketFD = create_client();
	
	//------------------------------Inicializar cliente
	
	while ( answer == 'N' ){
		cout << "Ingrese id: " << endl;
		cin >> current_id;
		write(SocketFD, ("L"+current_id).c_str(), 2);
		read(SocketFD, buffer , 2);
		
		if(buffer[0] == 'R')
			answer = buffer[1];
	}
	thread(read_server).detach();
	
	cars = new Cars;	
	//cars->move(3,1,"0614#0527@1429*");
	
	glutInit(&argc, argv); 
	InitGl();

	
	
	//------------------------------Cerrar socket cliente
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}

  
 // fuser -k -n tcp 1100
  
  
  
