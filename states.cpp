#include "states.h"

void init()
{
    //Wyczysc ekran i glebie kolorow
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    // Wlacz glebie przestrzenii
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
}

void viewport()
{
    // Przejdz na projektowanie projekcyjne
    glMatrixMode( GL_PROJECTION );
    // Ustaw wartosci macierzy na domyslna
    glLoadIdentity();
    // Ustaw macierz projekcyjna na ortograficzna
    //glOrtho(1.0 , 0.0 , 1.0, 0.0, 0.1, 100);
    gluPerspective(70.f,1280/720,0.1,100);
    glTranslatef(0.5,0.5,0);
    // Oryginalnie wystąpił bląd gdzie wszystko rysowało się do góry nogami
    // ale po obróceniu kamery o 180 stopni w osi Z naprawiło to
    glRotatef(180,0,0,1);
    // Wroc do rzutowania modelowego
    glMatrixMode( GL_MODELVIEW );
    // Zresetuj wszystkie transformacje
    glLoadIdentity( ); 
}