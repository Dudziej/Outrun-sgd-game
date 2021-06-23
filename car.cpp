#include "car.h"

/*  
*   Funkcja do rysowania "quad'u" czyli czworokata
*/

void drawRect(GLfloat color[3], float x1, float y1, float w1, float x2, float y2, float w2, float z)
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
        glTranslatef(0,0,z);
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_QUADS);
            glVertex3f(x1-w2,y2,0.f);
            glVertex3f(x2-w1,y1,0.f);
            glVertex3f(x2+w1,y1,0.f);
            glVertex3f(x1+w2,y2,0.f);
        glEnd();
    glPopMatrix();
}

/*  
*   Funkcja do rysowania auta
*/

void drawCar(float angle)
{
    GLfloat red[3] = {1.f,0.f,0.f};
    GLfloat black[3] = {0.f,0.f,0.f};

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    drawRect(black,490.f+angle,810.f,50.f,490.f-angle,610.f,50.f,-2.f);
    drawRect(black,790.f+angle,810.f,50.f,790.f-angle,610.f,50.f,-2.f);
    drawRect(red,1280.f/2.f,720.f+480.f,300.f,1280.f/2.f,720.f,100.f,-2.f);
    drawRect(black,350.f+angle,1250.f,50.f,350.f-angle,1050.f,50.f,-2.f);
    drawRect(black,930.f+angle,1250.f,50.f,930.f-angle,1050.f,50.f,-2.f); 
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}
