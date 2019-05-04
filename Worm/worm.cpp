#include <string>
#include <iostream>
#include <stdio.h> 
#include <GL/glut.h>
#include <string>
#include <map>
#include <vector>

using namespace std;
#define KEY_ESC 27

const char head_symbol = 'O';
const char freezbie = '*';

const int COLS = 80;
const int ROWS = 80;

vector< vector<float> > colors = {{255.0, 255.0, 0.0},{0.0, 255.0, 0.0}, {0.0, 255.0, 255.0}, {0.0, 0.0, 255.0}};

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
	Player(char id_, int i_head_, int j_head_ , int i_tail_ ,int j_tail_, vector<float>color):id(id_),i_head(i_head_),j_head(j_head_), i_tail(i_tail_),j_tail(j_tail_), rgb_red(color[0]), rgb_blue(color[1]), rgb_green(color[2]){}
};



class Worm{
private:
	vector<Player*> players;
	char tablero[ROWS][COLS];
	int freezbie_x, freezbie_y;
	
	void delete_snake( int i , int j, char player_id ){
		tablero[ i ][ j ] == '0';
		if( i+1 < ROWS && (tablero[ i+1 ][ j ] == player_id ) ){
			delete_snake( i+1 , j , player_id );
			return;
		}
		if( i-1 >= 0 && (tablero[ i-1 ][ j ] == player_id) ){
			delete_snake( i-1 , j , player_id );
			return;
		}
		if( j+1 < COLS && (tablero[ i ][ j + 1 ] == player_id) ){
			delete_snake( i , j+1 , player_id );
			return;
		}
		if( j-1 >= 0 && (tablero[ i ][ j - 1 ] == player_id) ){
			delete_snake( i , j-1  , player_id);
			return;	
		}
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
	
	
	void delete_player ( char id_player ){
		int id = map_id[ id_player ];
		map<char,int>::iterator it = map_id.find(id_player);
		map_id.erase (it); 
		
		delete_snake( players[id]->i_head, players[id]->j_head , id_player );
		players.erase(players.begin()+id, players.begin()+id+1);
	}
	
	void make_move( Player* p ){
		if ( tablero[ p->i_head ][ p->j_head ] == '0' ){	//solo borrar la cola si no come freezbie
			tablero[ p->i_head ][ p->j_head ] = head_symbol;
			tablero[ p->i_tail ][ p->j_tail ] = '0';
			next_in_tail( p->id, p->i_tail, p->j_tail);	
		}
		else if ( tablero[ p->i_head ][ p->j_head ] == freezbie ){
			tablero[ p->i_head ][ p->j_head ] = head_symbol;
			//tablero[rand()%80][rand()%80] = freezbie;
		}
		else if( tablero[ p->i_head ][ p->j_head ] != p->id ){	//eliminar jugador comido;
			char id_player_to_delete = tablero[ p->i_head ][ p->j_head ];
			delete_player( id_player_to_delete );
		}
		tablero[ p->i_head ][ p->j_head ] = head_symbol;
	}
	
	
public:	
	///------------------------------------ FUNCTION TO SERVER USE --------------------------------------------//
	Worm (){
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = '0';
			}			
		}
		check_add_freezbie();
	}
	
	string get_freezbiex(){
		string result = to_string(freezbie_x);
		if(result.size() < 2) 
			result = "0" + result;
		return result;
	}

	string get_freezbiey(){
		string result = to_string(freezbie_y);
		if(result.size() < 2) 
			result = "0" + result;
		return result;
	}

	bool check_add_freezbie(){
		if( tablero[freezbie_x][freezbie_y] != freezbie ){
			freezbie_x = rand()%80;
			freezbie_y = rand()%80;
			tablero[freezbie_x][freezbie_y] = freezbie;
			return true;
		}
		return false;
	}
	
	void add_new_player(char id){	//starting from the center
		players.push_back(new Player( id , colors[players.size()] ));
		map_id[ id ] = players.size()-1;
		tablero[40][40] = head_symbol; //head of every player starts with o
	}
	
	
	string get_tablero(){//U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
		string result = "";
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				result += tablero[i][j];
			}			
		}
		return result;
	}
	
	string get_players(){///Q(send avatars of all players) 02Q04056778#08094546 [Server ---> client] id head tail
		string result = "";
		result += to_string( players.size() );
		while(result.size() < 2) result.insert(0,"0");
		
		for(int i = 0; i < players.size(); ++i){
			result += players[i]->id;

			//add head
			if(players[i]->i_head<10) result += "0";
			result += to_string(players[i]->i_head);
			if(players[i]->j_head<10) result += "0";	
			result += to_string(players[i]->j_head);

			//add tail
			if(players[i]->i_tail<10) result += "0";
			result += to_string(players[i]->i_tail);
			if(players[i]->j_tail<10) result += "0";
			result += to_string(players[i]->j_tail);
		}
		return result;
	}
	
	bool exist_player( char id ){//Q(send avatars of all players) Q04#P63 [Server ---> client] 
		for(int i = 0; i < players.size(); ++i){
			if(players[i]->id == id) return true;
		}
		return false;
	}	
	
	
	///------------------------------------ FUNCTION TO CLIENT USE --------------------------------------------//
	
	void change_tab ( string tablero_){
		//U(send the Matrix) VVVVO###VVVVVVVVVVV....VVV(6400) [Server ---> client]
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				tablero[i][j] = tablero_[i*ROWS + j];
			}			
		}
	}
	
	void add_many_players( int cantidad, string players_ ){///Q(send avatars of all players) 02Q04056778#08094546 [Server ---> client] id head tail
		for(int i = 0; i < cantidad; ++i){
			string cur_player = players_.substr(0,9);
			if(players.size() > 9)
				players_ = players_.substr(9,players_.size()-9);
				
			char id = cur_player[0];
			int i_head = atoi( cur_player.substr(1,2).c_str() );
			int j_head = atoi( cur_player.substr(3,2).c_str() );
			int i_tail = atoi( cur_player.substr(5,2).c_str() );
			int j_tail = atoi( cur_player.substr(7,2).c_str() );
			
			players.push_back(new Player( id, i_head, j_head, i_tail, j_tail, colors[i]));
			map_id[ id ] = i;
		}
	}	
	
	void add_freezbie( int x, int y ){
		tablero[x][y] = freezbie;
	}
	
	
	///------------------------------------ FUNCTIONS SERVER CLIENT --------------------------------------------//
	
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
	
	void drawSquare(int sidelength = 1)
	{
		glBegin(GL_QUADS);
		
		for(int i = 0; i < ROWS; ++i){
			for(int j = 0; j < COLS; ++j){
				double x1 = j;
				double y1 = i;
				
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
				glVertex2d(x1 , y1 + sidelength);
				glVertex2d(x1 + sidelength, y1);
				glVertex2d(x1 + sidelength, y1 + sidelength);
			}			
		}
		
		glEnd();
		glFlush(); 
	}
	
	

};

/*
Worm* worm;

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
	worm->add_new_player('#');	

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
*/

//g++ worm.cpp -o worm -lGL -lglut -lGLU

