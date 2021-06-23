#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <cstring>
#include <string>
#include <chrono>
#include "car.h"
#include "states.h"


//Glebokosc kamery
#define camD 0.84
//Dlugosc segmentow "linii"
#define segL 200
#define roadW 2000

//Stale
#define screenX 1280
#define screenY 720

//  Zmienne globalne
float carAngle = 0.f;
int speed=0;
float playerX = 0;


SDL_Window *Window;
SDL_Renderer *renderer;
SDL_GLContext Context;



//Konstruktor linii
struct Line{
    float x,y,z;
    float X,Y,W;
    float curve,scale;
    Line() {curve=x=y=z=0;}
    
    //Skaluj linie wzgledem ekranu
    void project(int camX,int camY,int camZ)
    {
        scale = camD/(z-camZ);
        X = (1 + scale*(x - camX)) * screenX/2;
        Y = (1 - scale*(y - camY)) * screenY/2;
        W = scale * roadW  * screenX/2;
    }
};

void getkey(const Uint8* keys)
{
    carAngle = 0.f;
    if(speed > 0.f)
    {
        speed-=1.f;
    }
    if(speed < 0.f)
    {
        speed+=1.f;
    }
    //Sprawdz ktory klawisz jest wcisniety
    if(keys[SDL_SCANCODE_A])
    {
        carAngle = 20.f;
        if(playerX<-1.f || playerX>1.f){
            playerX+=0.05;
        }
        else{
            playerX+=0.1;
        }
    }
    if(keys[SDL_SCANCODE_D])
    {
        carAngle = -20.f;
        if(playerX<-1.f || playerX>1.f){
            playerX-=0.05;
        }
        else{
            playerX-=0.1;
        }
    }
    if(keys[SDL_SCANCODE_W])
    {
        if(playerX<-1.f || playerX>1.f){
            speed=200;
        }
        else{
            speed=400;
        }
    }
    if(keys[SDL_SCANCODE_S])
    {
        if(playerX<-1.f || playerX>1.f){
            speed=-100;
        }
        else{
            speed=-200;
        }
    }
}

int main(){
    const Uint8* keys;
    Window = SDL_CreateWindow("Outrun!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenX, screenY, SDL_WINDOW_OPENGL );
    if(Window == NULL)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(Window, -1, 0);
    Context = SDL_GL_CreateContext(Window);
    //  Tworzymu wektor konstruktorow
    //  i przypisujemy mu wartosci
    std::vector<Line> lines;
    for(int i=0;i<1600;i++)
    {
        Line line;
        line.z = i*segL;
        if (i>300 && i<700) line.curve=0.5;
        lines.push_back(line);
    }
    int pos = 0;
    int N = lines.size();

    bool done = false;
    SDL_Event event;
    GLfloat grass[3];
    GLfloat rumble[3];
    GLfloat road[3];
    float a,b = 0.f,delta;
    //float timer = 0.f;
    auto start = std::chrono::high_resolution_clock::now();
    while(!done)
    {
        a = SDL_GetTicks();
        //timer = SDL_GetTicks();
        delta = a - b;
        if (delta > 1000/144.0)
        {
            b = a;      
            int startPos = pos/segL;
            float x=0,dx=0;
            init();
            viewport();
            glClearColor(0.55, 0.71, 0.73, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();
            //Interuj przez nastepne widoczne paski
            for(int i=startPos;i<startPos+300;i++)
            {
                Line &l = lines[i%N];
                x+=dx;
                dx+=l.curve;
                l.project(playerX*roadW-x, 1500, startPos*segL - (i>=N?N*segL:0));
                x+=dx;
                //Zmien kolor co 3 pasek
                if((i/3)%2)
                {
                    grass[0] = 0.06;
                    grass[1] = 0.78;
                    grass[2] = 0.06;
                    for(int i = 0; i <3; i++){rumble[i] = 1.f;}
                    for(int i = 0; i <3; i++){road[i] = 0.43;}
                }
                else{
                    grass[0] = 0.f;
                    grass[1] = 0.6;
                    grass[2] = 0.f;
                    for(int i = 0; i <3; i++){rumble[i] = 0.f;}
                    for(int i = 0; i <3; i++){road[i] = 0.41;}
                }
                Line p = lines[(i-1)%N];
                // Rysuj auto, trewe, ulice oraz barierki
                drawCar(carAngle);
                drawRect(grass, 0, p.Y, screenX*2, 0, l.Y, screenX*2, -1.f);
                drawRect(rumble, p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2, -1.f);
                drawRect(road, p.X, p.Y, p.W, l.X, l.Y, l.W, -1.f);
            }
            // Zaktualizuj okno z zawartoscia OpenGL
            SDL_GL_SwapWindow(Window);
            while(SDL_PollEvent(&event))
            {
                if(event.type == SDL_QUIT) {done=1;}
                if(event.type == SDL_KEYDOWN)
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE) {done=1;}
                } 
            }
            //  Zapisz zmienna keys jako stan klawiatury
            keys = SDL_GetKeyboardState(NULL);
            getkey(keys);
            pos+=speed;
            //Gdy auto zatoczy kolo zrestartuj zegar
            printf("%d\n", pos);
            if(pos>=320000)
            {
                start = std::chrono::high_resolution_clock::now();
            }
            //  Zamien typ chrono w typ float
            float dur_seconds = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
            //  Zamien float w string (*char[])
            std::string buf = std::to_string(dur_seconds);
            //  Zmien dokladnosc do 3 znakow (razem z "." sa 4 )
            buf.resize(4);
            //  Ustaw nazwe okna
            SDL_SetWindowTitle(Window, buf.c_str());
            while (pos >= N*segL) pos-=N*segL;
            while (pos < 0) pos += N*segL;
        }
    }

    //Zniszcz okno i wyczysc pamiec
    SDL_DestroyWindow(Window);
    Window = NULL;
    //Oddaj sterowanie do systemu
    SDL_Quit();
    return 0;
}
