#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

//Stale
#define screenX 1280
#define screenY 720
//Glebokosc kamery
#define camD 0.84
//Dlugosc segmentow "linii"
#define segL 200
#define roadW 2000

SDL_Window *Window;
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

void drawRect(GLfloat color[3], float x1, float y1, float w1, float x2, float y2, float w2)
{
    /*  Ze wzgledu, ze SDL2 nie pozwala
    *   na rysowanie niczego innego niz
    *   rectangle jestesmy zmuszeni
    *   zejsc na poziom OpenGL
    */  

    /*  OpenGL przyjmuje wartosci z zakresu 0 do 1
    *   dlatego dzielimy pozycje X,Y na ekranie
    *   przez wartosci Wysokosci i Szerokosci ekranu
    */
    x1 = x1/screenX;
    x2 = x2/screenX;
    y1 = y1/screenY;
    y2 = y2/screenY;
    w1 = w1/screenX;
    w2 = w2/screenX;

    glPushMatrix();
        glColor3f(color[0], color[1], color[2]);
        //glTranslatef(-0.125,0.f,0.f);
        glBegin(GL_QUADS);
            glVertex3f(x1-w2,y2,0.f);
            glVertex3f(x2-w1,y1,0.f);
            glVertex3f(x2+w1,y1,0.f);
            glVertex3f(x1+w2,y2,0.f);
        glEnd();
    glPopMatrix();
}

void init()
{
    //Wyczysc ekran i glebie kolorow
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void viewport()
{
    // Przejdz na projektowanie projekcyjne
    glMatrixMode( GL_PROJECTION );
    // Ustaw wartosci macierzy na domyslna
    glLoadIdentity();
    // Ustaw macierz projekcyjna na ortograficzna
    gluOrtho2D( 1.0 , 0.0 , 1.0, 0.0);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( ); 
}

int speed=0;
float playerX = 0;

void getkey(const Uint8* keys)
{
    //Sprawdz ktory klawisz jest wcisniety
    if(keys[SDL_SCANCODE_A])
    {
        if(playerX<-1.f || playerX>1.f){
            playerX+=0.05;
        }
        else{
            playerX+=0.1;
        }
    }
    if(keys[SDL_SCANCODE_D])
    {
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
            speed=100;
        }
        else{
            speed=200;
        }
    }
    if(keys[SDL_SCANCODE_S])
    {
        if(playerX<-1.f || playerX>1.f){
            speed=80;
        }
        else{
            speed=60;
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
    while(!done)
    {
        int startPos = pos/segL;
        float x=0,dx=0;
        init();
        viewport();
        glClearColor(0.55, 0.71, 0.73, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
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
            drawRect(grass, 0, p.Y, screenX, 0, l.Y, screenX);
            drawRect(rumble, p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2);
            drawRect(road, p.X, p.Y, p.W, l.X, l.Y, l.W);
        }
        SDL_GL_SwapWindow(Window);
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) {done=1;}
            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE) {done=1;}
            } 
        }
        keys = SDL_GetKeyboardState(NULL);
        getkey(keys);
        pos+=speed;
        while (pos >= N*segL) pos-=N*segL;
        while (pos < 0) pos += N*segL;
    }

    SDL_DestroyWindow(Window);
    SDL_Quit();
    return 0;
}
