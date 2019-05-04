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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <utility>
#include <algorithm>
#include <time.h>

#define seg 1000000 ///un segundo
#define min 60000000 ///un minuto
#define per 300000 ///tiempo de refresco
#define gameTime 200///tiempo de juego, por refresco

using namespace std;

typedef const unsigned int uin;
uin ancho = 10, alto = 25; ///dimencion de la matrix
char scream[alto][ancho];
class car{
public:
    char avatar=' ';
    pair<int,int> pos;
    int veloci=3;
    bool acc = 0;
    bool lose = 0;
    car(){}
    ~car(){}
    void moveR(){
        if(pos.second+2 >= ancho)return;
        pos.second = pos.second+2;
        cout<<avatar<<" :"<<pos.second<<endl;
    }
    void moveL(){
        if(pos.second-2 < 0)return;
        pos.second = pos.second-2;
        cout<<avatar<<" :"<<pos.second<<endl;
    }
    void moveU(){
        if(veloci+1 > 5)return;
        veloci += 1;
        cout<<avatar<<" ->"<<veloci<<endl;
    }
    void moveD(){
        if(veloci-1 < 1)return;
        veloci -=1;
        cout<<avatar<<" ->"<<veloci<<endl;
    }
};
vector<pair<int,car>> clientes;
unsigned int anima=0; ///bool para animar
int perdedor1=0, perdedor2=0,ganador=0; ///quienes pierden o ganan
unsigned int CLar = 3, CAnc = 2; /// dimenciones del car

bool sortPos(const pair<int,car> &a,  
               const pair<int,car> &b){ 
       return (a.second.pos.first < b.second.pos.first); 
}
void collision(){
    for(size_t x =0;x<clientes.size();++x){
        if(clientes[x].second.lose)continue;
        if(clientes[x].second.pos.first >= alto){ ///si el car llega a la death line
            perdedor1 = clientes[x].first;
            continue;
        }
        int f = clientes[x].second.pos.first, s=clientes[x].second.pos.second;
        for(size_t i =0;i<clientes.size();++i){
            if(clientes[i].second.lose)continue;
            if(i == x){
                continue;
            }
            int fi = clientes[i].second.pos.first, si=clientes[i].second.pos.second;
            int df = abs(f-fi);
            int ds = abs(s-si);
            if(df <= CLar and ds <= CAnc){ ///check colisiones
                if(clientes[x].second.veloci < clientes[i].second.veloci){
                    perdedor1 = clientes[x].first;
                }else if(clientes[x].second.veloci > clientes[i].second.veloci){
                    perdedor1 = clientes[i].first;              
                }else{ /// igual velocidad => pierden los dos
                    perdedor1 = clientes[x].first;
                    perdedor2 = clientes[i].first;
                }
                break;
            }
        }
    }
}
void newPos(){
    for(size_t x =0;x<clientes.size();++x){
        if(clientes[x].second.lose)continue;
        scream[clientes[x].second.pos.first][clientes[x].second.pos.second]=' ';
        clientes[x].second.pos.first -= clientes[x].second.veloci;
    } ///actualiza la posicion
    sort(clientes.begin(),clientes.end(),sortPos);
    car* aux = &(clientes[0].second);///el primer puesto
    int a=0;
    if(aux->pos.first < 0){///si "avanza" activamos anima
        a = abs(aux->pos.first - 2);///diferencia del primero con la ultima posicion aceptada
        anima = 1;
    }
    for(size_t x =0;x<clientes.size();++x){///ajustamos la posicion a la matrix
        if(clientes[x].second.lose)continue;
        clientes[x].second.pos.first += a;
        scream[clientes[x].second.pos.first][clientes[x].second.pos.second]=clientes[x].second.avatar;
    }
    scream[aux->pos.first][aux->pos.second]=aux->avatar;
}

void move(char m, int cli){
    car *aux;
    for(size_t x =0;x<clientes.size();++x){
        if(clientes[x].second.lose)continue;
        if(clientes[x].first == cli){
            aux = &(clientes[x].second);
            break;
        }
    }
    scream[aux->pos.first][aux->pos.second]=' ';
    switch (m){
    case 'L':
        aux->moveL();
        break;
    case 'D':
        aux->moveD();
        break;
    case 'R':
        aux->moveR();
        break;
    case 'U':
        aux->moveU();
        break;
    default:
        break;
    }
    scream[aux->pos.first][aux->pos.second]=aux->avatar;
}
void lose(bool x){
    int i=0;
    if(x){
        while(clientes[i].first !=perdedor1) i++;
        write(perdedor1, "SL", 2);
        perdedor1 = 0;
        scream[clientes[i].second.pos.first][clientes[i].second.pos.second]=' ';
        clientes[i].second.lose = 1;
    }else{
        while(clientes[i].first !=perdedor2) i++;
        write(perdedor2, "SL", 2);
        perdedor2 = 0;
        scream[clientes[i].second.pos.first][clientes[i].second.pos.second]=' ';
        clientes[i].second.lose = 1;
    }
}
string findCar(int j,int i){///retorna el avatar y su pos en la matr
    string c(1,scream[j][i]) , a;
    a=to_string(j);
    while(a.length()<2) a.insert(0,"0");
    a.insert(0,to_string(i));
    while(a.length()<4) a.insert(0,"0");

    for(size_t x=0;x<clientes.size();++x){
        if(clientes[x].second.avatar == scream[j][i])
            c.insert(0,a);
    }
    return c;
}
void sendMatrix(int cli){
    int cant=0;
    string S="T";
    string ca;
    for(int j =0;j<alto;++j){
        for(int i =0;i<ancho;++i){
            if(scream[j][i]!=' '){
                S += findCar(j,i);
                cant++;
            }
        }
    }
    ca = to_string(cant);
    while(ca.length() < 2){
        ca.insert(0,"0");
    }
    S.insert(1,ca);
    S.insert(1,to_string(anima));///anima, al principio no hay animacion
    write(cli,S.c_str(), S.length());
}
void toAvatar(char a,int x){
    if (a == '|' or a == ' '){
        write(x, "RN", 2);
        return;
    }
    for(size_t i=0;i<clientes.size();++i){
        if(clientes[i].first != x)
            if(clientes[i].second.avatar == a){
                write(x, "RN", 2); ///si ya existe otro cliente con el mismo avatar
                return;
            }
    }
    for(size_t i=0;i<clientes.size();++i){
        if(clientes[i].first == x){
            clientes[i].second.avatar = a;
            clientes[i].second.acc = 1;
            clientes[i].second.pos = make_pair(alto-CLar,(i*(CAnc+1))); ///pos de inicio
            cout<<"inicia "<<clientes[i].second.avatar<<": "<<clientes[i].second.pos.second<<endl;
            scream[alto-CLar][(i*(CAnc+1))] = a;
        }
    }
    string aux = "RY";
    cout<<"avatar---> "<< aux<<" = "<<a<<endl;
    write(x, aux.c_str(), 2);
}

void leer_de(int SocketFD){
    char mensaje[255];
    for(;;){
        bzero(mensaje,255);
        read(SocketFD, mensaje, 255);
        printf(": %s \n",mensaje);
        switch (mensaje[0]){
            case 'L':
                cout << "Solicitud de Avatar de " << SocketFD << endl;
                toAvatar(mensaje[1],SocketFD);
                break;
            case 'M':
                cout << "Movimiento de " << SocketFD << endl;
                move(mensaje[1],SocketFD);
                break;
        }
        if(string(mensaje) == "exit"){
            for(int i = 0; i < clientes.size(); ++i){
                if(clientes[i].first == SocketFD){
                    write(clientes[i].first, "adios", 5);
                    clientes.erase(clientes.begin()+i);
                }
            }
            break;
        }
    }
}
void Enviar(){
    int timen = 0;
    bool d = 0;
    for(;;){
        while(clientes.size()>=2){ /// Empieza el juego con '3' jugadores
            d=1;
            for(size_t x=0;x<clientes.size();++x){
                if(!clientes[x].second.acc){
                    d = 0; ///No todos han sido aceptados
                }
            }
            if(!d)continue;
            for(size_t x=0;x<clientes.size();++x){
                if(clientes[x].second.lose)continue;
                sendMatrix(clientes[x].first);
            }
            usleep(seg);//usleep(per);
            timen++;
            cout<<timen<<"\' time"<<endl;
            if(d){
                newPos();
                collision();
            }
            if( perdedor1 != 0){
                cout<<"perdio "<< perdedor1<<endl;
                lose(1);
            }
            if (perdedor2 != 0){
                cout<<"perdio "<< perdedor2<<endl;
                lose(0);}
            for(int j = 0; j < alto ; ++j){
                cout<<"|";
                for(int i =0;i<ancho;++i){
                    cout<<scream[j][i];
                }
                cout<<"|"<<endl;
            }
            if(timen == 30){ //if(timen == gameTime){ /// Fin de la carrera
                sort(clientes.begin(),clientes.end(),sortPos);
                int fist = clientes[0].first;
                for(size_t x=0;x<clientes.size();++x){
                    if(clientes[x].second.lose)continue;
                    if(clientes[x].first == fist){
                        write(clientes[x].first, "SW", 2);
                        ganador = clientes[x].first;
                        continue;
                    }
                    write(clientes[x].first, "SL", 2);
                    clientes[x].second.lose = 1;
                }
                break;
            }
        }
    }
}
void buil(){
    for(int j = 0; j < alto ; ++j){
        for(int i =0;i<ancho;++i){
            scream[j][i]=' ';
        }
    }
    cout<<"matriz creada"<<endl;
}
int main()
{
    srand(time(NULL));
    buil();
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));

    listen(SocketFD, 10);
    thread(Enviar).detach();
    for(;;){
        int ConnectFD = accept(SocketFD, NULL, NULL);
        if(ConnectFD < 0) continue;
        thread(leer_de, ConnectFD).detach();
        car aux;
        clientes.push_back(make_pair(ConnectFD,aux));
        if(clientes.empty())break;
    }

    close(SocketFD);
    return 0;
}
