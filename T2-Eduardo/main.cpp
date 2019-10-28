#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include "include/faseSelector.h"
#include "glcWavefrontObject.h"
#include <iostream>

#define NUM_OBJECTS 2
#define faces 20
using namespace std;

/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.


/// Globals

/*char objectFiles[NUM_OBJECTS][50] =
{
    "../data/obj/Love.obj",
    "../data/obj/rock1.obj"
};

typedef struct
{
    //GLMmodel* pmodel = NULL;
    glcWavefrontObject *pmodel = NULL;
} object;

object *objectList;

glcWavefrontObject *objectManager = NULL;*/

//variaveis presentes no codigo original
float zdist = 8.0;
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;
int   width = 1000, height = 600;

int selected = 0;
int selectedShading = SMOOTH_SHADING;
int selectedRender = USE_MATERIAL;

float barraH = 1, barraW = 6.0;
float barraX = -barraW/2, barraY = -barraH/2-10+barraH*1.5;
float xRebatedor = barraX + barraW/2, yRebatedor = 0;
float desiredFPS = 30; //define fps do deslocamento da esfera
float direction = 0.0;//direção do disparador em angulos
float sphereX = 0, sphereY = -9.5; //variavel que determinam a posição da esfera
float velX = 0, velY = 0, velInicial = 12.5; //variavel que determinam a velocidade da esfera
float velMouse = 0.25; // velocidade da barra
bool launched = false; //variavel que determina se a esfera foi lançada
bool projection = false; //variavel que determina que tipo de projeção utilizar
float dx, dy; //valores de x e y do vetor direção normalizados
float raioAreaImpacto = 4; //variavel que determina a distancia minima para verificar a possibilidade de colisão da esfera com um tijolo
bool paused = false; // variavel para determiar se o jogo esta pausado
bool fullscreen = false; // variavel para determiar se o jogo esta em fullscreen
bool movimentacao = false; // variavel para determiar se o jogo esta com movimentacao livre na projecao em perpectiva
bool terminou = false; // variável para determinar o fim do jogo
int vida = 5;
bool gameOver = false;
//variaveis utilizadas para definir a posição dos tijolos
float x = 1.0, y = 1.0;
faseSelector fs;
float mColor[matrizLinha][3];//matriz para corolir os tijolos

float PI = 3.1415927;
vertice v[faces+1];
vertice normal[faces+1];
float dist = 0;

vertice v1[faces+1];
vertice normal1[faces+1];
float dist12 = 0;

vertice v2[faces+1];
vertice normal2[faces+1];
float xLombadaE,yLombadaE=0,xLombadaD,yLombadaD=0;

float raiz3 = sqrt(3);
vertice normalBrick = {1/raiz3,1/raiz3,1/raiz3};

vertice objetos[2] = {{0,0,0.5},{0,0,0.5}};
int objCond[2] = {3,3};
float objEscala[2] = {0.5,0.5};
float chanceAparicao = 1;
float dirX[2] = {0,0}, dirY[2] = {0,0};
float velObjeto = 5;
vertice cores[4] = {{0,0,0},{0.5,0.5,0.5},{0,0,1},{1,0,0}};
/// Functions
void criaCirculo()
{

    float raio = (barraH*barraH + (barraW/2)*(barraW/2))/(2*barraH);
    float angulo = (2*asin(barraW/2/raio))*180/PI;
    float g =(180-angulo)/2;
    float k = angulo/faces;
    float aux = 0, auy = 0, auz = 0, tamanho=0;
    dist = (0.5+raio)*(0.5+raio);
    yRebatedor = barraY+barraH-raio;
    for(int i = 0; i < faces+1; i++)
    {

        v[i].x = raio * cos(g * PI / 180);
        v[i].y = raio * sin(g * PI / 180)+yRebatedor;
        v[i].z = 0;
        aux = 1 * cos(g * PI / 180);
        auy = 1 * sin(g * PI / 180);
        auz = 1;
        tamanho = sqrt(aux*aux+auy*auy+auz*auz);
        normal[i].x = aux/tamanho;
        normal[i].y = auy/tamanho;
        normal[i].z = auz/tamanho;
        g += k;
    }

    raio = (0.5*0.5 + 4*4)/(2*0.5);
    angulo = (2*asin(4/raio))*180/PI;
    g = 360-(angulo/2);
    k = angulo/faces;
    dist12 = (0.5+raio)*(0.5+raio);
    xLombadaE = -9.5 -raio;
    for(int i = 0; i < faces+1; i++)
    {

        v1[i].x = raio * cos(g * PI / 180)+xLombadaE;
        v1[i].y = raio * sin(g * PI / 180);
        v1[i].z = 0;
        aux = 1 * cos(g * PI / 180);
        auy = 1 * sin(g * PI / 180);
        auz = 1;
        tamanho = sqrt(aux*aux+auy*auy+auz*auz);
        normal1[i].x = aux/tamanho;
        normal1[i].y = auy/tamanho;
        normal1[i].z = auz/tamanho;
        g += k;
    }
    g = (360-angulo)/2;
    xLombadaD = 9.5+raio;
    for(int i = 0; i < faces+1; i++)
    {

        v2[i].x = raio * cos(g * PI / 180)+xLombadaD;
        v2[i].y = raio * sin(g * PI / 180);
        v2[i].z = 0;
        aux = 1 * cos(g * PI / 180);
        auy = 1 * sin(g * PI / 180);
        auz = 1;
        tamanho = sqrt(aux*aux+auy*auy+auz*auz);
        normal2[i].x = aux/tamanho;
        normal2[i].y = auy/tamanho;
        normal2[i].z = auz/tamanho;
        g += k;
    }
}

void initLight(int width, int height)
{
    glEnable(GL_LIGHTING);                 // Habilita luz
    glEnable(GL_LIGHT0);                   // habilita luz 0
    glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
    glEnable(GL_CULL_FACE);                // Habilita Backface-Culling
    glShadeModel (GL_SMOOTH);
    // Cor da fonte de luz (RGBA)
    GLfloat cor_luz[]     = { 1.0, 1.0, 1.0, 1.0};
    // Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
    GLfloat posicao_luz[] = { 0, 0, 8, 1.0};

   // Define parametros da luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz);
    glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);
}

void setColor(float r, float g, float b)
{
    // Parametros comuns para os lados da superfície
    GLfloat objeto_especular[] = { 0.626, 0.626, 0.626, 1.0 };
    GLfloat objeto_brilho[]    = { 90.0f };
    GLfloat objeto_ambient[]   = { 0.1, 0.1, 0.1, 1.0 };

    GLfloat objeto_difusa[]    = { r, g, b, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
    glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);
}

void colorir(int i)
{
    for (int i = 0; i < matrizLinha; i++)//definição de cores aleatórias para os tijolos
    {
        mColor[i][0] = float((rand() % 10)) / 9.0;
        mColor[i][1] = float((rand() % 10)) / 9.0;
        mColor[i][2] = float((rand() % 10)) / 9.0;
    }
    switch(i)
    {
    case 0:
        cores[0].x = 0;
        cores[0].y = 0;
        cores[0].z = 0;
        cores[1].x = 0.5;
        cores[1].y = 0.5;
        cores[1].z = 0.5;
        cores[2].x = 0;
        cores[2].y = 0;
        cores[2].z = 0.8;
        cores[3].x = 1;
        cores[3].y = 0;
        cores[3].z = 0;
        break;
    case 1:
        cores[0].x = 0.25;
        cores[0].y = 0.25;
        cores[0].z = 0.25;
        cores[1].x = 1;
        cores[1].y = 1;
        cores[1].z = 0;
        cores[2].x = 1;
        cores[2].y = 0;
        cores[2].z = 0;
        cores[3].x = 0;
        cores[3].y = 0;
        cores[3].z = 1;
        break;
    case 2:
        cores[0].x = 0.5;
        cores[0].y = 0.5;
        cores[0].z = 0.5;
        cores[1].x = 1;
        cores[1].y = 0;
        cores[1].z = 1;
        cores[2].x = 0;
        cores[2].y = 1;
        cores[2].z = 0;
        cores[3].x = 1;
        cores[3].y = 0;
        cores[3].z = 1;
        break;
    }
}

void init(void)
{
    criaCirculo();
    srand(time(0));
    initLight(width, height);
    fs.construirGrid(fs.seletor);
    colorir(fs.seletor);
    /*objectManager = new glcWavefrontObject();
    objectManager->SetNumberOfObjects(NUM_OBJECTS);
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        objectManager->SelectObject(i);
        objectManager->ReadObject(objectFiles[i]);
        objectManager->Unitize();
        objectManager->FacetNormal();
        objectManager->VertexNormals(90.0);
        objectManager->Scale(3.0);
    }*/
}

///Função para desenhar os tijolos, as paredes e a barra de lançamento
void drawBrick(float x, float y, float comprimento, float largura, float altura)
{
    vertice vBase[4] = {{x,y,0},
        {x+comprimento,y,0},
        {x+comprimento,y+largura,0},
        {x,y+largura,0}
    };

    vertice vTopo[4] = {{x,y,altura},
        {x+comprimento,y,altura},
        {x+comprimento,y+largura,altura},
        {x,y+largura,altura}
    };

    vertice frente[4] = {vBase[0],
                         vBase[1],
                         vTopo[1],
                         vTopo[0]
                        };

    vertice direito[4] = {vBase[1],
                          vBase[2],
                          vTopo[2],
                          vTopo[1]
                         };

    vertice costas[4] = {vBase[2],
                         vBase[3],
                         vTopo[3],
                         vTopo[2]
                        };

    vertice esquerdo[4] = {vBase[3],
                           vBase[0],
                           vTopo[0],
                           vTopo[3]
                          };

    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(vBase[0].x, vBase[0].y, vBase[0].z);
    glNormal3f(normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(vBase[1].x, vBase[1].y, vBase[1].z);
    glNormal3f(normalBrick.x,normalBrick.y,-normalBrick.z);
    glVertex3f(vBase[2].x, vBase[2].y, vBase[2].z);
    glNormal3f(-normalBrick.x,normalBrick.y,-normalBrick.z);
    glVertex3f(vBase[3].x, vBase[3].y, vBase[3].z);
    glEnd();

    ///Topo
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(vTopo[0].x, vTopo[0].y, vTopo[0].z);
    glNormal3f(normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(vTopo[1].x, vTopo[1].y, vTopo[1].z);
    glNormal3f(normalBrick.x,normalBrick.y,normalBrick.z);
    glVertex3f(vTopo[2].x, vTopo[2].y, vTopo[2].z);
    glNormal3f(-normalBrick.x,normalBrick.y,normalBrick.z);
    glVertex3f(vTopo[3].x, vTopo[3].y, vTopo[3].z);
    glEnd();

    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(frente[0].x, frente[0].y, frente[0].z);
    glNormal3f(normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(frente[1].x, frente[1].y, frente[1].z);
    glNormal3f(normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(frente[2].x, frente[2].y, frente[2].z);
    glNormal3f(-normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(frente[3].x, frente[3].y, frente[3].z);
    glEnd();

    ///Direita
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(direito[0].x, direito[0].y, direito[0].z);
    glNormal3f(normalBrick.x,normalBrick.y,-normalBrick.z);
    glVertex3f(direito[1].x, direito[1].y, direito[1].z);
    glNormal3f(normalBrick.x,normalBrick.y,normalBrick.z);
    glVertex3f(direito[2].x, direito[2].y, direito[2].z);
    glNormal3f(normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(direito[3].x, direito[3].y, direito[3].z);
    glEnd();

    ///Costas
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(normalBrick.x,normalBrick.y,-normalBrick.z);
    glVertex3f(costas[0].x, costas[0].y, costas[0].z);
    glNormal3f(-normalBrick.x,normalBrick.y,-normalBrick.z);
    glVertex3f(costas[1].x, costas[1].y, costas[1].z);
    glNormal3f(-normalBrick.x,normalBrick.y,normalBrick.z);
    glVertex3f(costas[2].x, costas[2].y, costas[2].z);
    glNormal3f(normalBrick.x,normalBrick.y,normalBrick.z);
    glVertex3f(costas[3].x, costas[3].y, costas[3].z);
    glEnd();

    ///Esquerda
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,normalBrick.y,-normalBrick.z);
    glVertex3f(esquerdo[0].x, esquerdo[0].y, esquerdo[0].z);
    glNormal3f(-normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(esquerdo[1].x, esquerdo[1].y, esquerdo[1].z);
    glNormal3f(-normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(esquerdo[2].x, esquerdo[2].y, esquerdo[2].z);
    glNormal3f(-normalBrick.x,normalBrick.y,normalBrick.z);
    glVertex3f(esquerdo[3].x, esquerdo[3].y, esquerdo[3].z);
    glEnd();
}

// Função para desenhar as paredes e a superficie
void drawEnviroment(void)
{
    //plano
    setColor(cores[0].x,cores[0].y,cores[0].z);
    glBegin(GL_TRIANGLE_FAN);

    glNormal3f(0,0,1);
    glVertex3f(10,10,0);
    glVertex3f(-10,10,0);
    glVertex3f(-10,-10,0);
    glVertex3f(10,-10,0);
    glEnd();
    setColor(cores[1].x,cores[1].y,cores[1].z);

    //Direita
    drawBrick(10,-10,0.5,20,1);
    //Esquerda
    drawBrick(-10.5,-10,0.5,20,1);
    //Cima
    drawBrick(-10,9.5,20,0.5,1);
    //portas
    setColor(cores[2].x,cores[2].y,cores[2].z);
    drawBrick(-7.5,9.5,5,0.5,1.5);
    drawBrick(2.5,9.5,5,0.5,1.5);

}

void drawLombadas()
{
    setColor(cores[1].x,cores[1].y,cores[1].z);
    float altura = 1;
    ///Lombada Esquerda
    ///Lateral
    for(int i = 0; i < faces; i++)
    {
        glBegin(GL_TRIANGLE_FAN);
            glNormal3f( normal1[i].x, normal1[i].y,-normal1[i].z);
            glVertex3f(v1[i].x, v1[i].y, 0);
            glNormal3f( normal1[i+1].x, normal1[i+1].y,-normal1[i].z);
            glVertex3f(v1[i+1].x, v1[i+1].y, 0);
            glNormal3f( normal1[i+1].x, normal1[i+1].y,normal1[i].z);
            glVertex3f(v1[i+1].x, v1[i+1].y,  altura);
            glNormal3f( normal1[i].x, normal1[i].y,normal1[i].z);
            glVertex3f(v1[i].x, v1[i].y,  altura);
        glEnd();
    }

    ///Topo
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( normal1[faces].x, normal1[faces].y,normal1[faces].z);
    glVertex3f(v1[faces].x, v1[faces].y, altura);
    for(int i = 0; i < faces; i++)
    {
        glNormal3f( normal1[i].x, normal1[i].y,normal1[i].z);
        glVertex3f(v1[i].x, v1[i].y, altura);
    }
    glEnd();
    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( normal1[faces].x, normal1[faces].y,-normal1[faces].z);
    glVertex3f(v1[faces].x, v1[faces].y, 0);
    for(int i = 0; i < faces; i++)
    {
        glNormal3f( normal1[i].x, normal1[i].y,-normal1[i].z);
        glVertex3f(v1[i].x, v1[i].y, 0);
    }
    glEnd();
    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(v1[faces].x, v1[faces].y, 0);
    glNormal3f(normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(v1[0].x, v1[0].y, 0);
    glNormal3f(normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(v1[0].x, v1[0].y, altura);
    glNormal3f(-normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(v1[faces].x, v1[faces].y, altura);
    glEnd();

    ///Lombada Direita
    ///Lateral
    for(int i = 0; i < faces; i++)
    {
        glBegin(GL_TRIANGLE_FAN);
            glNormal3f( normal2[i].x, normal2[i].y,-normal2[i].z);
            glVertex3f(v2[i].x, v2[i].y, 0);
            glNormal3f( normal2[i+1].x, normal2[i+1].y,-normal2[i].z);
            glVertex3f(v2[i+1].x, v2[i+1].y, 0);
            glNormal3f( normal2[i+1].x, normal2[i+1].y,normal2[i].z);
            glVertex3f(v2[i+1].x, v2[i+1].y,  altura);
            glNormal3f( normal2[i].x, normal2[i].y,normal2[i].z);
            glVertex3f(v2[i].x, v2[i].y,  altura);
        glEnd();
    }

    ///Topo
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( normal2[faces].x, normal2[faces].y,normal2[faces].z);
    glVertex3f(v2[faces].x, v2[faces].y, altura);
    for(int i = 0; i < faces; i++)
    {
        glNormal3f( normal2[i].x, normal2[i].y,normal2[i].z);
        glVertex3f(v2[i].x, v2[i].y, altura);
    }
    glEnd();
    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( normal2[faces].x, normal2[faces].y,-normal2[faces].z);
    glVertex3f(v2[faces].x, v2[faces].y, 0);
    for(int i = 0; i < faces; i++)
    {
        glNormal3f( normal2[i].x, normal2[i].y,-normal2[i].z);
        glVertex3f(v2[i].x, v2[i].y, 0);
    }
    glEnd();
    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(v2[faces].x, v2[faces].y, 0);
    glNormal3f(normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(v2[0].x, v2[0].y, 0);
    glNormal3f(normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(v2[0].x, v2[0].y, altura);
    glNormal3f(-normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(v2[faces].x, v2[faces].y, altura);
    glEnd();
}

void drawBarra()
{
    float altura = 1;
    ///Lateral
    for(int i = 0; i < faces; i++)
    {
        glBegin(GL_TRIANGLE_FAN);
            glNormal3f( normal[i].x, normal[i].y,-normal[i].z);
            glVertex3f(v[i].x+xRebatedor, v[i].y, 0);
            glNormal3f( normal[i+1].x, normal[i+1].y,-normal[i].z);
            glVertex3f(v[i+1].x+xRebatedor, v[i+1].y, 0);
            glNormal3f( normal[i+1].x, normal[i+1].y,normal[i].z);
            glVertex3f(v[i+1].x+xRebatedor, v[i+1].y,  altura);
            glNormal3f( normal[i].x, normal[i].y,normal[i].z);
            glVertex3f(v[i].x+xRebatedor, v[i].y,  altura);
        glEnd();
    }

    ///Topo
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( normal[faces].x, normal[faces].y,normal[faces].z);
    glVertex3f(v[faces].x+xRebatedor, v[faces].y, altura);
    for(int i = 0; i < faces; i++)
    {
        glNormal3f( normal[i].x, normal[i].y,normal[i].z);
        glVertex3f(v[i].x+xRebatedor, v[i].y, altura);
    }
    glEnd();
    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( normal[faces].x, normal[faces].y,-normal[faces].z);
    glVertex3f(v[faces].x+xRebatedor, v[faces].y, 0);
    for(int i = 0; i < faces; i++)
    {
        glNormal3f( normal[i].x, normal[i].y,-normal[i].z);
        glVertex3f(v[i].x+xRebatedor, v[i].y, 0);
    }
    glEnd();
    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(v[faces].x+xRebatedor, v[faces].y, 0);
    glNormal3f(normalBrick.x,-normalBrick.y,-normalBrick.z);
    glVertex3f(v[0].x+xRebatedor, v[0].y, 0);
    glNormal3f(normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(v[0].x+xRebatedor, v[0].y, altura);
    glNormal3f(-normalBrick.x,-normalBrick.y,normalBrick.z);
    glVertex3f(v[faces].x+xRebatedor, v[faces].y, altura);
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
    glutSetCursor(GLUT_CURSOR_NONE);
    //A variavel projection e utilizada para definir posição da camera
    if(!projection)
    {
        GLfloat posicao_luz[] = { 0, 0, 8, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);
        gluLookAt (0.0, 0.0, 8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
    else
    {
        GLfloat posicao_luz[] = { 0, -10, 14, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);
        gluLookAt (0.0, -10.0, 14, 0.0, -2.0, 0.0, 0.0, 1.0, 0.0);
    }

    //chama as funções de desenhar tijolos e ambiente, além de desenhar o disparador e a esfera
    glPushMatrix();
        glRotatef( rotationY, 0.0, 1.0, 0.0 );
        glRotatef( rotationX, 1.0, 0.0, 0.0 );
        setColor(cores[3].x,cores[3].y,cores[3].z);
        drawBarra();
        ///Pedra 1 entra nesse glPushMatrix
        glPushMatrix();
            setColor(1,0,0);
            glTranslatef(objetos[0].x,objetos[0].y,objetos[0].z);
            if(objCond[0]!=3)
                glutSolidSphere(objEscala[0],100,100);
        glPopMatrix();
        ///Pedra 2 entra nesse glPushMatrix
        glPushMatrix();
            setColor(0,0,1);
            glTranslatef(objetos[1].x,objetos[1].y,objetos[1].z);
            if(objCond[1]!=3)
                glutSolidSphere(objEscala[1],100,100);
        glPopMatrix();
        ///Esse seria o espaco para os coracoes
        /*glPushMatrix();
        glTranslatef(11.0,9.0,2.0);
        for (int i = 0; i < vida; i++)
        {
            glTranslatef(0.0,-1.5,0.0);
            objectManager->SelectObject(selected);
            objectManager->SetShadingMode(selectedShading); // Possible values: FLAT_SHADING e SMOOTH_SHADING
            objectManager->SetRenderMode(selectedRender);     // Possible values: USE_COLOR, USE_MATERIAL, USE_TEXTURE (not available in this example)
            objectManager->Unitize();
            objectManager->Draw();
        }
        glPopMatrix();*/
        ///Loop para desenhar a fase com cores aleatórias para cada linha do grid
        for(int i = 0; i < matrizLinha; i++)
        {
            setColor(mColor[i][0], mColor[i][1], mColor[i][2]);
            for(int j = 0; j < matrizColuna; j++)
            {
                if(fs.m[i][j].alive) // desenha tijolo apenas se n foi destruido
                {
                    drawBrick(fs.m[i][j].vMenor.x, fs.m[i][j].vMenor.y, compriment, largur, altur);
                }

            }
        }
        drawEnviroment();
        drawLombadas();
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

    //if(drawboundingbox) objectManager->DrawBoundingBox();
    //if(drawaxes)        drawAxes();
    glutSwapBuffers();
}

// função para calcular novos valores de dx e dy, a partir do vetor normal(nx,ny,0), sendo o vetor normal normalizado dentro da função
void calculaVetorNovo(float nx, float ny, int i)
{
    if(i<2)
    {
        //calcula direção do vetor novo utilizando a lei da reflexão
        float aux = 2*(-dirX[i]*nx + -dirY[i]*ny);
        dirX[i] = nx*aux + dirX[i];
        dirY[i] = ny*aux + dirY[i];
    }
    else
    {
        //calcula direção do vetor novo utilizando a lei da reflexão
        float aux = 2*(-dx*nx + -dy*ny);
        dx = nx*aux + dx;
        dy = ny*aux + dy;
    }
}

//função para detectar e responder a colisão da esfera com as paredes
void colisionAmbient(int i)
{
    if(i<2)
    {
        if(objetos[i].x > 9.5)
        {
            objetos[i].x = 9.5;
            calculaVetorNovo(-1,0,i);
        }
        if(objetos[i].x < -9.5)
        {
            objetos[i].x = -9.5;
            calculaVetorNovo(1,0,i);
        }
        if(objetos[i].y > 9)
        {
            objetos[i].y = 9;
            calculaVetorNovo(0,1,i);
        }
        if(objetos[i].y < -9.5)
        {
            objCond[i] = 2;
        }
    }
    else
    {
        if(sphereX > 9.5)
        {
            sphereX = 9.5;
            calculaVetorNovo(-1,0,i);
        }
        if(sphereX < -9.5)
        {
            sphereX = -9.5;
            calculaVetorNovo(1,0,i);
        }
        if(sphereY > 9)
        {
            sphereY = 9;
            calculaVetorNovo(0,1,i);
        }
        if(sphereY < -9.5)
        {
            launched = !launched;
            vida --;
            if (vida == 0)
                gameOver = true;
        }
    }
}
//funcao para determinar se houve colisao anteriormente com a mesma barra
bool distanciaNormalDirecao(float nx,float ny, int i)
{
    float d;
    if(i<2)
    {
        float auxX = nx - dirX[i];
        float auxY = ny - dirY[i];
        d = auxX*auxX + auxY*auxY;
    }
    else
    {
        float auxX = nx - dx;
        float auxY = ny - dy;
        d = auxX*auxX + auxY*auxY;
    }
    return d > 2;
}
//funcao para determinar se a barra esta a certa distancia da esfera
bool distanciaTijolo(float x, float y, float comprimento, float largura, int i)
{
    float auxX,auxY;
    if(i<2)
    {
        auxX = objetos[i].x - (x + compriment/2);
        auxY = objetos[i].y - (y + largur/2);
    }
    else
    {
        auxX = sphereX - (x + compriment/2);
        auxY = sphereY - (y + largur/2);
    }
    return auxX*auxX+auxY*auxY <= raioAreaImpacto*raioAreaImpacto;
}
//funcao para determinar se houve colisao entre a esfera e a barra
bool colisaoTijolo(float xTijolo, float yTijolo, float comprimento, float largura,int i)
{
    //colisao AABB deste livro: [Arvo 90] J. Arvo. “A Simple Method for Box-Sphere Intersection Testing.” In
    //Graphics Gems, edited by Andrew Glassner, pp. 335–339. San Diego, CA: Academic Press Professional, Inc., 1990.
    bool colidiu = false;
    float len_v = 0;
    float d = 0, aux = 0;
    float nx = 0, ny = 0;
    int xbool = 1, ybool = 1;
    float xEsfera, yEsfera;
    if(i<2)
    {
        xEsfera = objetos[i].x;
        yEsfera = objetos[i].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }

    if (xEsfera<xTijolo)
    {
        xbool = 0;
        aux = xEsfera - xTijolo;
        d = d+aux*aux;
    }
    else if (xEsfera>xTijolo+comprimento)
    {
        xbool = 2;
        aux = xEsfera - (xTijolo+comprimento);
        d = d+aux*aux;
    }

    if (yEsfera<yTijolo)
    {
        ybool = 0;
        aux = yEsfera - yTijolo;
        d = d+aux*aux;
    }
    else if (yEsfera>yTijolo+largura)
    {
        ybool = 2;
        aux = yEsfera - (yTijolo+largura);
        d = d+aux*aux;
    }

    if(d <= 0.25)
    {
        switch (xbool) // switch para determinar normal
        {
        case 0:
            switch (ybool)
            {
            case 0:
                nx = xEsfera - xTijolo;
                ny = yEsfera - yTijolo;
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
                nx = xEsfera - xTijolo;
                ny = yEsfera - (yTijolo+largura);
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
                nx = xEsfera - (xTijolo+comprimento);
                ny = yEsfera - yTijolo;
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
                nx = xEsfera - (xTijolo+comprimento);
                ny = yEsfera - (yTijolo+largura);
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            }
            break;
        }
        if (distanciaNormalDirecao(nx,ny,i)) // verifica se ja houve colisao com essa barra
        {
            calculaVetorNovo(nx,ny,i);
            colidiu = true;
        }
    }
    return colidiu;
}

void colisaoBarra(float xTijolo, float yTijolo, float comprimento, float largura, int i)
{
    //colisao AABB deste livro: [Arvo 90] J. Arvo. “A Simple Method for Box-Sphere Intersection Testing.” In
    //Graphics Gems, edited by Andrew Glassner, pp. 335–339. San Diego, CA: Academic Press Professional, Inc., 1990.
    float len_v = 0;
    float d = 0, aux = 0;
    float nx = 0, ny = 0;
    int ybool = 1;
    float xEsfera, yEsfera;
    if(i<2)
    {
        xEsfera = objetos[i].x;
        yEsfera = objetos[i].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }

    if (xEsfera<xTijolo)
    {
        aux = xEsfera - xTijolo;
        d = d+aux*aux;
    }
    else if (xEsfera>xTijolo+comprimento)
    {
        aux = xEsfera - (xTijolo+comprimento);
        d = d+aux*aux;
    }

    if (yEsfera<yTijolo)
    {
        ybool = 0;
        aux = yEsfera - yTijolo;
        d = d+aux*aux;
    }
    else if (yEsfera>yTijolo+largura)
    {
        ybool = 2;
        aux = yEsfera - (yTijolo+largura);
        d = d+aux*aux;
    }

    if(d <= 0.25)
    {
        if(ybool == 0)
        {
            return;
        }
        nx = xEsfera - xRebatedor;
        ny = yEsfera - yRebatedor;
        aux = nx*nx + ny*ny;
        //parte para normalizar o vetor normal
        len_v = sqrt(nx*nx + ny*ny);
        nx /= len_v;
        ny /= len_v;
        if (distanciaNormalDirecao(nx,ny,i) && aux <= dist) // verifica se ja houve colisao com essa barra
        {
            calculaVetorNovo(nx,ny,i);
            if(i<2)
            {
                objCond[i] = 2;
            }
        }
    }
}

void colisaoLombada(float xTijolo, float yTijolo, float comprimento, float largura, int lombada, int i)
{
    //colisao AABB deste livro: [Arvo 90] J. Arvo. “A Simple Method for Box-Sphere Intersection Testing.” In
    //Graphics Gems, edited by Andrew Glassner, pp. 335–339. San Diego, CA: Academic Press Professional, Inc., 1990.
    float len_v = 0;
    float d = 0, aux = 0;
    float nx = 0, ny = 0;
    int xbool = 1;
    float xlombada = 0;
    float xEsfera, yEsfera;
    if(i<2)
    {
        xEsfera = objetos[i].x;
        yEsfera = objetos[i].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }

    if (xEsfera<xTijolo)
    {
        xbool = 0;
        aux = xEsfera - xTijolo;
        d = d+aux*aux;
    }
    else if (xEsfera>xTijolo+comprimento)
    {
        xbool = 2;
        aux = xEsfera - (xTijolo+comprimento);
        d = d+aux*aux;
    }

    if (yEsfera<yTijolo)
    {
        aux = yEsfera - yTijolo;
        d = d+aux*aux;
    }
    else if (yEsfera>yTijolo+largura)
    {
        aux = yEsfera - (yTijolo+largura);
        d = d+aux*aux;
    }

    if(d <= 0.25)
    {
        if(lombada == 0)
        {
            if(xbool == 0)
            {
                return;
            }
            xlombada = xLombadaE;
        }
        else
        {
            if(xbool == 2)
            {
                return;
            }
            xlombada = xLombadaD;
        }
        nx = xEsfera - xlombada;
        ny = yEsfera - yLombadaD;
        aux = nx*nx + ny*ny;
        //parte para normalizar o vetor normal
        len_v = sqrt(nx*nx + ny*ny);
        nx /= len_v;
        ny /= len_v;
        if (distanciaNormalDirecao(nx,ny,i) && aux <= dist12) // verifica se ja houve colisao com essa barra
        {
            calculaVetorNovo(nx,ny,i);
        }
    }
}

void colisao(int k) //funcao para agrupar todas as colisoes
{
    colisionAmbient(k);
    for(int i = 0; i < matrizLinha; i++)
    {
        for(int j = 0; j < matrizColuna; j++)
        {
            float xTijolo = fs.m[i][j].vMenor.x, yTijolo = fs.m[i][j].vMenor.y;
            if((launched || k<2) && fs.m[i][j].alive && distanciaTijolo(xTijolo,yTijolo,compriment,largur,k))
            {
                if(colisaoTijolo(xTijolo,yTijolo,compriment,largur,k) && k == 2)
                {
                    fs.m[i][j].colidiu();
                }
            }
        }
    }
    if((launched || k<2) && distanciaTijolo(barraX,barraY,barraW,barraH,k))
    {
        colisaoBarra(barraX,barraY,barraW,barraH,k);
    }
    if((launched || k<2) && distanciaTijolo(-10,-4,0.5,8,k))
    {
        colisaoLombada(-10,-4,0.5,8,0,k);
    }
    if((launched || k<2) && distanciaTijolo(9.5,-4,0.5,8,k))
    {
        colisaoLombada(9.5,-4,0.5,8,1,k);
    }
}

void faseTerminou() //funcao para determinar se a fase terminou
{
    for(int i = 0; i < matrizLinha; i++)
    {
        for(int j = 0; j < matrizColuna; j++)
        {
            if(fs.m[i][j].alive)
            {
                return;
            }
        }
    }
    launched = false;
    fs.fase();
    if (fs.seletor == 3)
    {
        terminou = true;
        return;
    }
    else
    {
        fs.construirGrid(fs.seletor);
        colorir(fs.seletor);
    }
    return;
}

bool colisaoObjeto(int i)
{
    float auxX = sphereX - objetos[i].x;
    float auxY = sphereY - objetos[i].y;
    return auxX*auxX+auxY*auxY <= 1;
}

void idle ()
{
    float t, desiredFrameTime, frameTime;
    static float tLast = 0.0;
    int chance = 0;
    int aux;
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

    faseTerminou();
    if(!paused && !terminou && !gameOver)//verifica se o jogo esta pausado ou terminou a fase
    {
        colisao(2);
        if(launched)//verifica se a esfera foi lançada, se caso ocorrer movimenta a esfera com a direção e velocidades apropriados
        {
            velX = dx*velInicial;
            velY = dy*velInicial;
            sphereX+=velX* (1 / desiredFPS);
            sphereY+=velY* (1 / desiredFPS);
        }
        else//determina direção do disparador e posição inicial da esfera caso nao seja lançada
        {
            dx = cos((direction+90)*0.0174533);
            dy = sin((direction+90)*0.0174533);
            sphereX = barraX+barraW/2;
            sphereY = barraY + 0.5 + barraH;
        }

        for(int i = 0; i<2; i++)
        {
            switch(objCond[i])
            {
            case 0:
                objetos[i].y -= 0.1;
                if(objetos[i].y <= 9.5)
                    objCond[i] = 1;
                break;
            case 1:
                colisao(i);
                if(colisaoObjeto(i))
                {
                    objCond[i] = 2;
                    break;
                }
                objetos[i].x += dirX[i]*velObjeto* (1 / desiredFPS);
                objetos[i].y += dirY[i]*velObjeto* (1 / desiredFPS);
                break;
            case 2:
                objEscala[i] -= 0.1;
                if(objEscala[i] <= 0.1)
                    objCond[i] = 3;
                break;
            case 3:
                chance = rand()%100;
                if(chance <= chanceAparicao)
                {
                    objetos[i].y = 10.5;
                    aux = rand()%2;
                    if(aux)
                    {
                        objetos[i].x = 5;
                    }
                    else
                    {
                        objetos[i].x = -5;
                    }
                    chanceAparicao = 1;
                    objCond[i] = 0;
                    objEscala[i] = 0.5;
                    aux = rand()%100;
                    dirX[i] = aux;
                    aux = rand()%100;
                    dirY[i] = aux;
                    float len_v = sqrt(dirX[i]*dirX[i] + dirY[i]*dirY[i]);
                    dirX[i] /= len_v;
                    dirY[i] /= len_v;
                }
                else
                {
                    chanceAparicao +=0.1;
                }
                break;
            }
        }
    }
    tLast = t;
    glutWarpPointer(width / 2, height / 2);//lanca mouse para o meio da tela
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
    /*if(isdigit(key))
    {
        int val = atoi((const char *) &key);
        if(val > 0 && val <= NUM_OBJECTS )
            selected = val-1;
    }*/

    switch (tolower(key))
    {
    case 27:
        exit(0);
        break;
    case 'r':
        objCond[0] = 3;
        objCond[1] = 3;
        fs.seletor = 0;
        fs.construirGrid(fs.seletor);
        colorir(fs.seletor);
        launched = false;
        barraX = -barraW/2;
        direction = 0;
        vida = 5;
        paused = false;
        gameOver = false;
        terminou = false;
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
    float aux = (x - width / 2)*0.05;
    if(!paused && !terminou && !gameOver)
    {
        barraX +=aux;
        xRebatedor = barraX + barraW/2;
    }
    if(barraX > 10-barraW)
    {
        barraX = 10-barraW;
        xRebatedor = barraX + barraW/2;
    }
    else if (barraX < -10)
    {
        barraX = -10;
        xRebatedor = barraX + barraW/2;
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
        float aux = (x - width / 2)*0.05;
        if(!paused && !terminou && !gameOver)
        {
            barraX +=aux;
            xRebatedor = barraX + barraW/2;
        }
        if(barraX > 10-barraW)
        {
            barraX = 10-barraW;
            xRebatedor = barraX + barraW/2;
        }
        else if (barraX < -10)
        {
            barraX = -10;
            xRebatedor = barraX + barraW/2;
        }
        last_x = x;
        last_y = y;
    }
}
// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !launched && !paused && !terminou && !gameOver)
    {
        launched = true;
    }
    if(button == 3 && !paused && !terminou && !gameOver) // Scroll up
    {
        if(!launched && direction < 75)
        {
            direction +=1;
        }
    }
    if(button == 4 && !paused && !terminou && !gameOver) // Scroll Down
    {
        if(!launched && direction > -75)
        {
            direction -=1;
        }
    }
}
void f12 (int key, int x, int y)
{
    if(key == GLUT_KEY_F12)
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
