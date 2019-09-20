#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "../src/extras.h"
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
int   width = 1000, height = 600;
float barraH = 1, barraW = 4.0;
float barraX = -barraW/2, barraY = -barraH/2-10+barraH*1.5;
float desiredFPS = 30; //define fps do deslocamento da esfera
float direction = 0.0;//direção do disparador em angulos
float sphereX = 0, sphereY = -9.5; //variavel que determinam a posição da esfera
float velX = 0, velY = 0, velInicial = 12.5; //variavel que determinam a velocidade da esfera
float velMouse = 0.25;
bool launched = false; //variavel que determina se a esfera foi lançada
bool projection = false; //variavel que determina que tipo de projeção utilizar
float dx, dy; //valores de x e y do vetor direção normalizados
float raioAreaImpacto = 5; //variavel que determina a distancia minima para verificar a possibilidade de colisão da esfera com um prisma
bool paused = false;
bool fullscreen = false;
bool movimentacao = false;
//variaveis utilizadas para definir a posição dos prismas
float x = 1.0, y = 1.0;
faseSelector fs;
float mColor[matrizLinha][3];
/// Functions
void init(void)
{
    srand(time(0));
    initLight(width, height);
    fs.construirGrid();
    for (int i = 0; i < matrizLinha; i++)
    {
        mColor[i][0] = float((rand() % 10)) / 9.0;
        mColor[i][1] = float((rand() % 10)) / 9.0;
        mColor[i][2] = float((rand() % 10)) / 9.0;
    }
}

void drawBrick(float x, float y, float comprimento, float largura, float altura)
{
    vertice vBase[4] = {{x,y,0},
                        {x+comprimento,y,0},
                        {x+comprimento,y+largura,0},
                        {x,y+largura,0}};

    vertice vTopo[4] = {{x,y,altura},
                        {x+comprimento,y,altura},
                        {x+comprimento,y+largura,altura},
                        {x,y+largura,altura}};

    vertice frente[4] = {vBase[0],
                         vBase[1],
                         vTopo[1],
                         vTopo[0]};

    vertice direito[4] = {vBase[1],
                          vBase[2],
                          vTopo[2],
                          vTopo[1]};

    vertice costas[4] = {vBase[2],
                         vBase[3],
                         vTopo[3],
                         vTopo[2]};

    vertice esquerdo[4] = {vBase[3],
                           vBase[0],
                           vTopo[0],
                           vTopo[3]};

    ///Base
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,0,-1);
        glVertex3f(vBase[0].x, vBase[0].y, vBase[0].z);
        glVertex3f(vBase[1].x, vBase[1].y, vBase[1].z);
        glVertex3f(vBase[2].x, vBase[2].y, vBase[2].z);
        glVertex3f(vBase[3].x, vBase[3].y, vBase[3].z);
    glEnd();

    ///Topo
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,0,1);
        glVertex3f(vTopo[0].x, vTopo[0].y, vTopo[0].z);
        glVertex3f(vTopo[1].x, vTopo[1].y, vTopo[1].z);
        glVertex3f(vTopo[2].x, vTopo[2].y, vTopo[2].z);
        glVertex3f(vTopo[3].x, vTopo[3].y, vTopo[3].z);
    glEnd();

    ///Frente
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,-1,0);
        glVertex3f(frente[0].x, frente[0].y, frente[0].z);
        glVertex3f(frente[1].x, frente[1].y, frente[1].z);
        glVertex3f(frente[2].x, frente[2].y, frente[2].z);
        glVertex3f(frente[3].x, frente[3].y, frente[3].z);
    glEnd();

    ///Direita
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(1,0,0);
        glVertex3f(direito[0].x, direito[0].y, direito[0].z);
        glVertex3f(direito[1].x, direito[1].y, direito[1].z);
        glVertex3f(direito[2].x, direito[2].y, direito[2].z);
        glVertex3f(direito[3].x, direito[3].y, direito[3].z);
    glEnd();

    ///Costas
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,1,0);
        glVertex3f(costas[0].x, costas[0].y, costas[0].z);
        glVertex3f(costas[1].x, costas[1].y, costas[1].z);
        glVertex3f(costas[2].x, costas[2].y, costas[2].z);
        glVertex3f(costas[3].x, costas[3].y, costas[3].z);
    glEnd();

    ///Esquerda
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(-1,0,0);
        glVertex3f(esquerdo[0].x, esquerdo[0].y, esquerdo[0].z);
        glVertex3f(esquerdo[1].x, esquerdo[1].y, esquerdo[1].z);
        glVertex3f(esquerdo[2].x, esquerdo[2].y, esquerdo[2].z);
        glVertex3f(esquerdo[3].x, esquerdo[3].y, esquerdo[3].z);
    glEnd();
}

// Função para desenhar as paredes e a superficie
void drawEnviroment(void)
{
        //plano
        setColor(0,0,0);
        glBegin(GL_TRIANGLE_FAN);

            glNormal3f(0,0,1);
            glVertex3f(10,10,0);
            glVertex3f(-10,10,0);
            glVertex3f(-10,-10,0);
            glVertex3f(10,-10,0);
        glEnd();
        setColor(0.5,0.5,0.5);

        //Esquerda
        drawBrick(9.5,-10,0.5,20,1);
        //Direita
        drawBrick(-10,-10,0.5,20,1);
        //Cima
        drawBrick(-10,9.5,20,0.5,1);

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
    glutSetCursor(GLUT_CURSOR_NONE);
    //A variavel projection e utilizada para definir posição da camera
    if(!projection)
    {
        zdist = 8;
        gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
    else
    {
        zdist = 14;
        gluLookAt (0.0, -10.0, zdist, 0.0, -2.0, 0.0, 0.0, 1.0, 0.0);
    }

    //chama as funções de desenhar prismas e ambiente, além de desenhar o disparador e a esfera
    glPushMatrix();
        glRotatef( rotationY, 0.0, 1.0, 0.0 );
        glRotatef( rotationX, 1.0, 0.0, 0.0 );
        setColor(1,0,0);
        drawBrick(barraX,barraY,barraW,barraH,altur);
        for(int i = 0; i < matrizLinha; i++)
        {
            setColor(mColor[i][0], mColor[i][1], mColor[i][2]);
            for(int j = 0; j < matrizColuna; j++)
            {
                if(fs.m[i][j].alive)
                {
                    drawBrick(fs.m[i][j].vMenor.x, fs.m[i][j].vMenor.y, compriment, largur, altur);
                }

            }
        }
        drawEnviroment();
        glPushMatrix();
            setColor(0,1,0);
            glTranslatef(sphereX,sphereY,0.5);
            glutSolidSphere(0.5,100,100);
        if(!launched){//verifica se a bola foi lançada para desenhar o disparador
                glRotatef(direction,0,0,1);
                setColor(1,0,0);
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
    //calcula direção do vetor novo utilizando a lei da reflexão
    float aux = 2*(-dx*nx + -dy*ny);
    dx = nx*aux + dx;
    dy = ny*aux + dy;
}

//função para detectar e responder a colisão da esfera com as paredes
void colisionAmbient()
{
    if(sphereX > 9)
    {
        sphereX = 9;
        calculaVetorNovo(-1,0);
    }
    if(sphereX < -9)
    {
        sphereX = -9;
        calculaVetorNovo(1,0);
    }
    if(sphereY > 9)
    {
        sphereY = 9;
        calculaVetorNovo(0,1);
    }
    if(sphereY < -9.5)
    {
        launched = !launched;
    }
}

bool distanciaNormalDirecao(float nx,float ny)
{
    float auxX = nx - dx;
    float auxY = ny - dy;
    float d = auxX*auxX + auxY*auxY;
    return d > 2;
}

bool distanciaTijolo(float x, float y, float comprimento, float largura)
{
    float auxX = sphereX - (x + compriment/2);
    float auxY = sphereY - (y + largur/2);
    return auxX*auxX+auxY*auxY <= raioAreaImpacto*raioAreaImpacto;
}

bool colisaoTijolo(float xTijolo, float yTijolo, float comprimento, float largura)
{
    bool colidiu = false;
    float len_v = 0;
    float d = 0, aux = 0;
    float nx = 0, ny = 0;
    int xbool = 1, ybool = 1;
    if (sphereX<xTijolo)
    {
        xbool = 0;
        aux = sphereX - xTijolo;
        d = d+aux*aux;
    }
    else if (sphereX>xTijolo+comprimento)
    {
        xbool = 2;
        aux = sphereX - (xTijolo+comprimento);
        d = d+aux*aux;
    }

    if (sphereY<yTijolo)
    {
        ybool = 0;
        aux = sphereY - yTijolo;
        d = d+aux*aux;
    }
    else if (sphereY>yTijolo+largura)
    {
        ybool = 2;
        aux = sphereY - (yTijolo+largura);
        d = d+aux*aux;
    }

    if(d <= 0.25)
    {
        switch (xbool)
        {
        case 0:
            switch (ybool)
            {
            case 0:
                nx = sphereX - xTijolo;
                ny = sphereY - yTijolo;
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            case 1:
                nx = -1;
                ny = 0;
                break;
            case 2:
                nx = sphereX - xTijolo;
                ny = sphereY - (yTijolo+largura);
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            }
            break;
        case 1:
            switch (ybool)
            {
            case 0:
                nx = 0;
                ny = -1;
                break;
            case 2:
                nx = 0;
                ny = 1;
                break;
            }
            break;
        case 2:
            switch (ybool)
            {
            case 0:
                nx = sphereX - (xTijolo+comprimento);
                ny = sphereY - yTijolo;
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            case 1:
                nx = 1;
                ny = 0;
                break;
            case 2:
                nx = sphereX - (xTijolo+comprimento);
                ny = sphereY - (yTijolo+largura);
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            }
            break;
        }
        if (distanciaNormalDirecao(nx,ny))
        {
            calculaVetorNovo(nx,ny);
            colidiu = true;
        }
    }
    return colidiu;
}

void colisao()
{
    colisionAmbient();
    for(int i = 0; i < matrizLinha; i++)
    {
        for(int j = 0; j < matrizColuna; j++)
        {
            float xTijolo = fs.m[i][j].vMenor.x , yTijolo = fs.m[i][j].vMenor.y;
            if(launched && fs.m[i][j].alive && distanciaTijolo(xTijolo,yTijolo,compriment,largur))
            {
                if(colisaoTijolo(xTijolo,yTijolo,compriment,largur))
                {
                    fs.m[i][j].colidiu();
                }
            }
        }
    }
    if(launched && distanciaTijolo(barraX,barraY,barraW,barraH))
    {
        bool colidiu = colisaoTijolo(barraX,barraY,barraW,barraH);
    }
}

bool faseTerminou()
{
    for(int i = 0; i < matrizLinha; i++)
    {
        for(int j = 0; j < matrizColuna; j++)
        {
            if(fs.m[i][j].alive)
            {
                return false;
            }
        }
    }
    return true;
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

    if(!paused && !faseTerminou())
    {
        colisao();
        float step = 1;
        if(launched)//verifica se a esfera foi lançada, se caso ocorrer movimenta a esfera com a direção e velocidades apropriados
        {
            velX = dx*velInicial;
            velY = dy*velInicial;
            sphereX+=velX* (step / desiredFPS);
            sphereY+=velY* (step / desiredFPS);
        }
        else//determina direção do disparador e posição inicial da esfera caso nao seja lançada
        {
            dx = cos((direction+90)*0.0174533);
            dy = sin((direction+90)*0.0174533);
            sphereX = barraX+barraW/2;
            sphereY = barraY + 0.5 + barraH;
        }
    }
        tLast = t;
        glutWarpPointer(width / 2, height / 2);
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
        case 'r':
            fs.construirGrid();
            launched = false;
            barraX = -barraW/2;
            direction = 0;
            paused = false;
            break;
        case 32:
            paused = !paused;
            break;
        case 'p':
            if(movimentacao)
            {
                movimentacao = false;
                rotationX = 0;
                rotationY = 0;
            }
            projection = !projection;
            break;
        case 'c':
            if(projection)
            {
                paused = true;
                movimentacao = !movimentacao;
            }
            if(!movimentacao)
            {
                rotationX = 0;
                rotationY = 0;
            }
            break;
    }
}

// Motion callback
void motion(int x, int y )
{
    if(x - width / 2>0 && !paused)
    {
        barraX+=velMouse;
    }
    else if(x - width / 2<0 && !paused)
    {
        barraX-=velMouse;
    }
    if(barraX > 9.5-barraW)
    {
        barraX = 9.5-barraW;
    }
    else if (barraX < -9.5)
    {
        barraX = -9.5;
    }
    if(x<0 || x>width)
    {
        glutWarpPointer(width / 2, height / 2);
    }
    last_x = x;
    last_y = y;
}

void motionClick(int x, int y )
{
    if(movimentacao)
    {
        rotationX += (float) (y - last_y);
        rotationY += (float) (x - last_x);


    }
    else
    {
        if(x - width / 2>0 && !paused)
        {
            barraX+=velMouse;
        }
        else if(x - width / 2<0 && !paused)
        {
            barraX-=velMouse;
        }
        if(barraX > 9.5-barraW)
        {
            barraX = 9.5-barraW;
        }
        else if (barraX < -9.5)
        {
            barraX = -9.5;
        }
    last_x = x;
    last_y = y;
    }
}
// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !launched && !paused)
    {
        launched = true;
    }
    if(button == 3 && !paused) // Scroll up
    {
        if(!launched && direction < 75)
            {
                direction +=1;
            }
    }
    if(button == 4 && !paused) // Scroll Down
    {
        if(!launched && direction > -75)
            {
                direction -=1;
            }
    }
}
void f12 (int key, int x, int y)
{
    if(key = GLUT_KEY_F12)
    {
        fullscreen = !fullscreen;
    }
    if(fullscreen)
    {
        glutFullScreen();
    }
    else
    {
        glutReshapeWindow(1000,600);
        glutPositionWindow(100,100);
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
    glutMotionFunc( motionClick );
    glutPassiveMotionFunc( motion );
    glutSpecialFunc(f12);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
