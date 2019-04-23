#include <string>
#include <iostream>
#include <stdio.h> 
#include <GL/glut.h>

#include <map>
#include <vector>

using namespace std;
#define KEY_ESC 27

const char head_symbol = 'O';
const char freebie = '*';

const int COLS = 80;
const int ROWS = 80;

vector< vector<float> > colors = {{255.0, 255.0, 0.0},{0.0, 255.0, 0.0}, {0.0, 255.0, 255.0}};

map < char, int > map_id; // id_player -> posicion en el vector players;

class Player{
public:
	char id;
	float rgb_red;
	float rgb_blue;
	float rgb_green;
	
	vector < pair <short, short> > snake;
	
	Player(char id_, vector<float>color):id(id_),snake(1, make_pair(40,40)), rgb_red(color[0]), rgb_blue(color[1]), rgb_green(color[2]){}
};



class Worm{
public:
	vector<Player*> players;
	char tablero[ROWS][COLS];
	vector < pair <short, short> > freebies;
	
	Worm (){
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = '0';
			}			
		}
	}
	
	Worm( string tablero_, string players_){
		//U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
		//Q(send avatars of all players) Q04#P63 [Server ---> client] 
		
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = tablero_[i*ROWS + j];
			}			
		}
		
		for(int i = 0; i < players_.size(); ++i){
			players.push_back(new Player(players_[i], colors[i]));
			map_id[ players_[i] ] = i;
		}
	}
	
	void add_player(char id){
		players.push_back(new Player( id , colors[players.size()] ));
		map_id[ id ] = players.size()-1;
		tablero[40][40] = head_symbol; //head of every player starts with o
	}
	
	
	void erase_tail( char player_id, int i, int j ){
		/*if(getpixel(x,y) == oldcolor)
		{
			putpixel(x,y,newcolor);
			floodFill(x+1,y,oldcolor,newcolor);
			floodFill(x,y+1,oldcolor,newcolor);
			floodFill(x-1,y,oldcolor,newcolor);
			floodFill(x,y-1,oldcolor,newcolor);
		}*/
	}
	
	bool move(char player_id, char direction){	///M(Move TDRL) M#L
		if( direction == 'T'){	//arriba
			int cur = map_id[player_id];

			if( players[cur]->snake[0].first + 1 > ROWS -1) return false;
			
			int i = players[cur]->snake[0].first;
			int j = players[cur]->snake[0].second;
				
			tablero[ i + 1 ][ j ] = head_symbol;
			tablero[ i ][ j ] = player_id;
			players[cur]->snake.insert( players[cur]->snake.begin(), make_pair(i + 1 , j) ) ;
			
			if ( tablero[ i + 1 ][ j ] != freebie ){	//solo borrar la cola si no come freezbie
				players[cur]->snake.pop_back();
			}
			
			//erase_tail( player_id , players[i]->i_head , players[i]->j_head );
			return true;
			
			
		}
	}
	
	//------------------------------------__OPENGL functions -------------------------------------//
	
	void drawSquare(int sidelength = 10.0)
	{
		double halfside = sidelength / 2;
		glClearColor(0.0, 0.0, 0.0, 0.0);         // black background 
		
		glBegin(GL_QUADS);
		
		//draw snakes of each player
		for(int p = 0; p < players.size(); ++p){
			//draw head_symbol
			glColor3f(250.0, 0.0, 0.0); //cabeza;
			double x1 = players[p]->snake[0].first*10;
			double y1 = players[p]->snake[0].second*10;
			
			glVertex2d(x1 , y1);
			glVertex2d(x1 , y1 + halfside);
			glVertex2d(x1 + halfside, y1);
			glVertex2d(x1 + halfside, y1 + halfside);
			//end draw head_symbol
			
			//draw body of the snake
			float red = players[ p ]->rgb_red;
			float blue = players[ p ]->rgb_blue;
			float green = players[ p ]->rgb_green;
			glColor3f( red , blue , green); 
			
			for(int s = 1; s < players[p]->snake.size(); ++s){
				
				x1 = players[p]->snake[s].first*10;
				y1 = players[p]->snake[s].second*10;
				
				glVertex2d(x1 , y1);
				glVertex2d(x1 , y1 + halfside);
				glVertex2d(x1 + halfside, y1);
				glVertex2d(x1 + halfside, y1 + halfside);
			}
		}
		
		/*
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				if (tablero[i][j] == '0')	continue;
				
				double x1 = i*10;
				double y1 = j*10;

				if (tablero[i][j] == freebie )	glColor3f(255.0, 255.0, 255.0); //un freebie;
				else if (tablero[i][j] == head_symbol)	glColor3f(250.0, 0.0, 0.0); //cabeza;
				else {
					float red = players[ map_id[tablero[i][j]] ]->rgb_red;
					float blue = players[ map_id[tablero[i][j]] ]->rgb_blue;
					float green = players[ map_id[tablero[i][j]] ]->rgb_green;
					
					glColor3f( red , blue , green); 
				}
				
				//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				glVertex2d(x1 , y1);
				glVertex2d(x1 , y1 + halfside);
				glVertex2d(x1 + halfside, y1);
				glVertex2d(x1 + halfside, y1 + halfside);
			}			
		}
		*/
		
		glEnd();
		glFlush(); 
	}
	
	

};

Worm* worm;

void display(void) 
{  
	glClear( GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glOrtho(-10.0f,  820.0f, -10.0f, 820.0f, -1.0f, 1.0f);  
	worm->drawSquare();
	
	glutPostRedisplay();
}

void window_key_glut(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		worm->move('#','T');
		break;
	case GLUT_KEY_DOWN:
		worm->move('#','D');
		break;
	case GLUT_KEY_RIGHT:
		worm->move('#','R');
		break;
	case GLUT_KEY_LEFT:
		worm->move('#','L');
		break;
	default:
		break;
	}
	//glutPostRedisplay();
}



int main(int argc, char **argv) 
{ 
	worm = new Worm;	
	worm->add_player('#');
	
	glutInit(&argc, argv); 
	glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowPosition(20,0); 
	glutInitWindowSize(800,800); 
	glutCreateWindow ("Worm");

	glMatrixMode(GL_PROJECTION);              // setup viewing projection 
	glutDisplayFunc(display); 
	glutSpecialFunc(&window_key_glut);	
	
	glutMainLoop();
	
	return 0; 
}
//g++ worm.cpp -o worm -lGL -lglut -lGLU


