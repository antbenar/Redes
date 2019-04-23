#include <string>
#include <iostream>
#include <stdio.h> 
#include <GL/glut.h>

#include <map>
#include <vector>

using namespace std;
#define KEY_ESC 27

const char head_symbol = 'O';
const char freezbie = '*';

const int COLS = 80;
const int ROWS = 80;

vector< vector<float> > colors = {{255.0, 255.0, 0.0},{0.0, 255.0, 0.0}, {0.0, 255.0, 255.0}};

map < char, int > map_id; // id_player -> posicion en el vector players;

class Player{
public:
	char id;
	int i_head, j_head;
	int i_tail, j_tail;
	float rgb_red;
	float rgb_blue;
	float rgb_green;
	
	Player(char id_, vector<float>color):id(id_),i_head(40),j_head(40), i_tail(40),j_tail(40), rgb_red(color[0]), rgb_blue(color[1]), rgb_green(color[2]){}
};



class Worm{
public:
	vector<Player*> players;
	char tablero[ROWS][COLS];
	
	Worm (){
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = '0';
			}			
		}
		tablero[rand()%80][rand()%80] = freezbie;
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
	
	
	void next_in_tail( char player_id, int& i, int& j ){
		if( i+1 < ROWS && (tablero[ i+1 ][ j ] == player_id or tablero[ i+1 ][ j ] == head_symbol) ){
			i += 1;
			return;
		}
		if( i-1 >= 0 && (tablero[ i-1 ][ j ] == player_id or tablero[ i-1 ][ j ] == head_symbol) ){
			i -= 1;
			return;
		}
		if( j+1 < COLS && (tablero[ i ][ j + 1 ] == player_id or tablero[ i ][ j + 1 ] == head_symbol) ){
			j += 1;
			return;
		}
		if( j-1 >= 0 && (tablero[ i ][ j - 1 ] == player_id or tablero[ i ][ j - 1 ] == head_symbol) ){
			j -= 1;
			return;	
		}
	}
	
	void make_move( Player* p ){
		if ( tablero[ p->i_head ][ p->j_head ] != freezbie ){	//solo borrar la cola si no come freezbie
			tablero[ p->i_head ][ p->j_head ] = head_symbol;
			tablero[ p->i_tail ][ p->j_tail ] = '0';
			next_in_tail( p->id, p->i_tail, p->j_tail);	
		}
		else tablero[ p->i_head ][ p->j_head ] = head_symbol;
	}
	
	bool move(char player_id, char direction){	///M(Move TDRL) M#L
		int cur = map_id[player_id];
		
		if( direction == 'T'){	//arriba
			if( players[cur]->i_head + 1 >= ROWS or tablero[ players[cur]->i_head + 1 ][ players[cur]->j_head ] == player_id ) 
				return false;
			
			tablero[ players[cur]->i_head ][ players[cur]->j_head ] = player_id;
			players[cur]->i_head += 1;
		}
		else if( direction == 'D'){	//abajo
			if( players[cur]->i_head - 1 < 0 or tablero[ players[cur]->i_head - 1 ][ players[cur]->j_head ] == player_id) 
				return false;
			
			tablero[ players[cur]->i_head ][ players[cur]->j_head ] = player_id;
			players[cur]->i_head -= 1;
		}
		else if( direction == 'R'){	//right
			int cur = map_id[player_id];
			
			if( players[cur]->j_head + 1 >= COLS or tablero[ players[cur]->i_head ][ players[cur]->j_head + 1 ] == player_id) 
				return false;
			
			tablero[ players[cur]->i_head ][ players[cur]->j_head ] = player_id;
			players[cur]->j_head += 1;
		}
		else if( direction == 'L'){	//left
			int cur = map_id[player_id];
			
			if( players[cur]->j_head - 1 < 0 or tablero[ players[cur]->i_head ][ players[cur]->j_head - 1 ] == player_id) 
				return false;
			
			tablero[ players[cur]->i_head ][ players[cur]->j_head ] = player_id;
			players[cur]->j_head -= 1;
		}
		
		make_move(players[cur]);
		return true;
	}
	
	//------------------------------------__OPENGL functions -------------------------------------//
	
	void drawSquare(int sidelength = 10.0)
	{
		double halfside = sidelength / 2;
		
		glBegin(GL_QUADS);
		
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				double x1 = i*10;
				double y1 = j*10;
				
				if (tablero[i][j] == '0')	glColor3f(0.0, 0.0, 0.0); //tablero vacio;
				else if (tablero[i][j] == freezbie )	glColor3f(255.0, 255.0, 255.0); //un frizbie;
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
	
	glClearColor(0.0, 0.0, 0.0, 0.0);         // black background 
	glMatrixMode(GL_PROJECTION);              // setup viewing projection 
	glutDisplayFunc(display); 
	glutSpecialFunc(&window_key_glut);	
	glutMainLoop();
	
	return 0; 
}
//g++ worm.cpp -o worm -lGL -lglut -lGLU

