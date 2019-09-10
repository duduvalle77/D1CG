#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "src/extras.h"
#define NUMPRISMAS 8
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include "include/faseSelector.h"
#include <iostream>
using namespace std;
/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.


/// Globals
//variaveis presentes no codigo original
float zdist = 8.0;
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;
int   width, height;

float desiredFPS = 60; //define fps do deslocamento da esfera
float direction = 0.0;//direção do disparador em angulos
float sphereX = 0, sphereY = -9.5; //variavel que determinam a posição da esfera
float velX = 0, velY = 0, velInicial = 0.5; //variavel que determinam a velocidade da esfera
bool launched = false; //variavel que determina se a esfera foi lançada
bool projection = true; //variavel que determina que tipo de projeção utilizar
bool colide = false; //variavel que determina se a esfera colidiu com algum prisma
float dx, dy; //valores de x e y do vetor direção normalizados
float timet = 0; //variavel que guarda o ultimo instante em que ocorreu colisçao da esfera com um prisma
float intensidade = 2; //variavel para determinar intensidade da velocidade
float raioAreaImpacto = 5; //variavel que determina a distancia minima para verificar a possibilidade de colisão da esfera com um prisma

//variaveis utilizadas para definir a posição dos prismas
float x = 1.0, y = 1.0;

/// Functions
void init(void)
{
    srand(time(NULL));
    initLight(width, height);
    tijolo t = tijolo();
    cout << t.alive << "\t";
    t.setVertice(10,20);
    cout << t.alive << endl;
}

// Função para desenhar as paredes e a superficie
void drawEnviroment(void)
{
        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0,0,1);
            glVertex3f(10,10,0);
            glVertex3f(-10,10,0);
            glVertex3f(-10,-10,0);
            glVertex3f(10,-10,0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(-1,0,0);
            glVertex3f(10,10,0);
            glVertex3f(10,-10,0);
            glVertex3f(10,-10,1);
            glVertex3f(10,10,1);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0,-1,0);
            glVertex3f(10,10,0);
            glVertex3f(10,10,1);
            glVertex3f(-10,10,1);
            glVertex3f(-10,10,0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(1,0,0);
            glVertex3f(-10,10,0);
            glVertex3f(-10,10,1);
            glVertex3f(-10,-10,1);
            glVertex3f(-10,-10,0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0,1,0);
            glVertex3f(10,-10,0);
            glVertex3f(-10,-10,0);
            glVertex3f(-10,-10,1);
            glVertex3f(10,-10,1);
        glEnd();
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    //A variavel projection e utilizada para definir se a visão será ortogonal ou em perspectiva, codigo tirado do projection.cpp que esta no material da disciplina
    int ortho = 10;
    if(!projection)
    {
        if (width <= height)
            glOrtho (-ortho, ortho, -ortho*height/width, ortho*height/width, -100.0, 100.0);
        else
            glOrtho (-ortho*width/height, ortho*width/height, -ortho, ortho, -100.0, 100.0);
    }
    else
        gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 0.01, 200.0);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //A variavel projection e utilizada para definir posição da camera
    if(!projection)
        gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    else
        gluLookAt (0.0, -18.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //corDisparador define o valor do segundo parametro de setColor do disparador
    float corDisparador = (3-velInicial)/5;

    //chama as funções de desenhar prismas e ambiente, além de desenhar o disparador e a esfera
    glPushMatrix();
        glRotatef( rotationY, 0.0, 1.0, 0.0 );
        glRotatef( rotationX, 1.0, 0.0, 0.0 );
        setColor(1.0,0.0,0.0);
   //     for (int i = 0; i < NUMPRISMAS; i++)
   //         drawObject(vBases[i], vTopos[i]);
        setColorBase();
        drawEnviroment();
        glPushMatrix();
            setColor(0,1,0);
            glTranslatef(sphereX,sphereY,0.5);
            glutSolidSphere(0.5,100,100);
        if(!launched){//verifica se a bola foi lançada para desenhar o disparador
                glRotatef(direction,0,0,1);
                setColor(1,corDisparador,0);
                glTranslatef(0,1.75,0);
                glBegin(GL_TRIANGLE_FAN);
                    glNormal3f(0,0,1);
                    glVertex3f(0.5,1.75,0);
                    glVertex3f(-0.5,1.75,0);
                    glVertex3f(-0.5,-1.75,0);
                    glVertex3f(0.5,-1.75,0);
                glEnd();
                glTranslatef(0,2.4,0);
                glBegin(GL_TRIANGLES);
                    glNormal3f(0,0,1);
                    glVertex3f(0.75,-0.65,0);
                    glVertex3f(0,0.65,0);
                    glVertex3f(-0.75,-0.65,0);
                glEnd();
        }
        glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

// função para calcular novos valores de dx e dy, a partir do vetor normal(nx,ny,0), sendo o vetor normal normalizado dentro da função
void calculaVetorNovo(float nx, float ny)
{
    //parte para normalizar o vetor normal
    float len_v = sqrt(nx*nx + ny*ny);
    nx /= len_v;
    ny /= len_v;
    //calcula direção do vetor novo utilizando a lei da reflexão
    float aux = 2*(-dx*nx + -dy*ny);
    dx = nx*aux + dx;
    dy = ny*aux + dy;
}

//função para detectar e responder a colisão da esfera com as paredes
void colisionAmbient()
{
    if(sphereX > 9.5)
    {
        sphereX = 9.5;
        calculaVetorNovo(-1,0);
    }
    if(sphereX < -9.5)
    {
        sphereX = -9.5;
        calculaVetorNovo(1,0);
    }
    if(sphereY > 9.5)
    {
        sphereY = 9.5;
        calculaVetorNovo(0,1);
    }
    if(sphereY < -9.5)
    {
        sphereY = -9.5;
        calculaVetorNovo(0,-1);
    }
}

void idle ()
{
    float t, desiredFrameTime, frameTime;
    static float tLast = 0.0;
    // Get elapsed time
    t = glutGet(GLUT_ELAPSED_TIME);
    // convert milliseconds to seconds
    t /= 1000.0;

    // Calculate frame time
    frameTime = t - tLast;
    // Calculate desired frame time
    desiredFrameTime = 1.0 / (float) (desiredFPS);

    // Check if the desired frame time was achieved. If not, skip animation.
    if( frameTime <= desiredFrameTime)
        return;

    //verifica colisão com o ambiente
    colisionAmbient();

    float step = 1;
    if(launched)//verifica se a esfera foi lançada, se caso ocorrer movimenta a esfera com a direção e velocidades apropriados
    {
        velX = dx*velInicial*intensidade;
        velY = dy*velInicial*intensidade;
        sphereX+=velX* (step / desiredFPS);
        sphereY+=velY* (step / desiredFPS);
    }
    else//determina direção do disparador e posição inicial da esfera caso nao seja lançada
    {
        dx = cos((direction+90)*0.0174533);
        dy = sin((direction+90)*0.0174533);
        sphereX = 0;
        sphereY = -9.5;
    }

    tLast = t;
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    width = w;
    height = h;

    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard (unsigned char key, int x, int y)
{

    switch (tolower(key))
    {
        case 27:
            exit(0);
            break;
        case 'w':
            if(!launched && velInicial < 2.5)
            {
                velInicial +=0.5;
            }
            break;
        case 's':
            if(!launched && velInicial > 0.5)
            {
                velInicial -=0.5;
            }
            break;
        case 'd':
            if(!launched && direction > -75)
            {
                direction -=1;
            }
            break;
        case 'a':
            if(!launched && direction < 75)
            {
                direction +=1;
            }
            break;
        case 32:
            launched = !launched;
            break;
        case 'p':
            projection = !projection;
            break;
    }
}

// Motion callback
void motion(int x, int y )
{
    rotationX += (float) (y - last_y);
    rotationY += (float) (x - last_x);

    last_x = x;
    last_y = y;
}

// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        last_x = x;
        last_y = y;
    }
    if(button == 3) // Scroll up
    {
        zdist+=1.0f;
    }
    if(button == 4) // Scroll Down
    {
        zdist-=1.0f;
    }
}

/// Main
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (1000, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
