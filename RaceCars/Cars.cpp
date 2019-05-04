#include <string>
#include <iostream>
#include <stdio.h> 
#include <GL/glut.h>

#include <map>
#include <vector>
using namespace std;
#define KEY_ESC 27

const int ROWS = 25;
const int COLS = 10;


vector< vector<float> > colors = {{255.0, 255.0, 0.0},{0.0, 255.0, 0.0}, {0.0, 255.0, 255.0}};

class Cars{
public:
	char tablero[ROWS][COLS];
	
	Cars(){
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = '0';
			}			
		}
	}
	
	void print_tablero(){
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				cout << tablero[i][j] << " ";
			}			
			cout << endl;
		}cout << "-------------------" << endl;
		/*
		for(int i = 0; i < ROWS; ++i){
		tablero[i][COLS/2] = '|';
		}
		*/
	}
	
	void clear_tablero(){
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = '0';
			}			
		}
		/*
		for(int i = 0; i < ROWS; ++i){
			tablero[i][COLS/2] = '|';
		}
		*/
	}
	
	void move ( int num_players, int flag, string tablero_){	//T001(xxyy#) -> solo recibe el xxyy#
		string aux;
		string id;
		int x, y;
		
		clear_tablero();
			
		for(int i = 0; i < num_players; ++i){
			aux = tablero_.substr( i*5 , 2 );
			x = atoi(aux.c_str());
			
			aux = tablero_.substr( i*5 + 2 , 2 );
			y = atoi(aux.c_str());
			
			id = tablero_.substr( i*5 + 4 , 1 );
			
			//cout << x << " " << y << " " << id << endl;
			tablero[y][x] = id[0];
		}
	}
	//------------------------------------__OPENGL functions -------------------------------------//
	
	void drawSquare(double sidelength = 1)
	{
		//double halfside = sidelength / 2;
		
		glBegin(GL_QUADS);
		
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				double x1 = j;
				double y1 = -1*i;
				
				if (tablero[i][j] == '0');//tablero vacio;
				else if ( tablero[i][j] =='|' ) glColor3f(255.0, 255.0, 255.0); //tablero vacio;
				else { 
				//	cout << x1 << " " << y1 << endl;
					char id = tablero[i][j];
					float red = id%256;
					float blue = (id+100)%256 ; 
					float green = (id+200)%256 ;
					
					glColor3f( red/255 , blue/255 , green/255); 
					
					//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
					glVertex2d(x1 , y1);
					glVertex2d(x1 + 2*sidelength, y1);
					glVertex2d(x1 + 2*sidelength, y1 +3*sidelength);
					glVertex2d(x1 , y1 + 3*sidelength);
				}
			}			
		}
		
		glEnd();
		glFlush(); 
	}
	
	

};

/*
Cars* cars;

void display(void) 
{  
	glClear( GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glOrtho(-10.0f,  COLS*10 +20, -10.0f, COLS*10 +20, -1.0f, 1.0f);  
	cars->drawSquare();
	
	glutPostRedisplay();
}

void window_key_glut(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		write_move( 'T' );
		//cars->move("0310615#0528@1430*");//2
		break;
	
	case GLUT_KEY_DOWN:
		cars->move("0310616#0529@1431*");//3
		break;
	
	case GLUT_KEY_RIGHT:
		cars->move("0310617#0530@1432*");//4
		break;
		
	case GLUT_KEY_LEFT:
		cars->move("0310614#0527@1429*");//1
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
	glutCreateWindow ("Cars");
	
	glClearColor(0.0, 0.0, 0.0, 0.0);         // black background 
	glMatrixMode(GL_PROJECTION);              // setup viewing projection 
	glutDisplayFunc(display); 
	glutSpecialFunc(&window_key_glut);	
	glutMainLoop();
}



int main(int argc, char **argv) 
{ 
	cars = new Cars;	
	cars->move("0310614#0527@1429*");
	
	glutInit(&argc, argv); 
	InitGl();
	
	return 0; 
}
*/
