#include "include/colisao.h"
#include "glcTexture.h"
#include "glcWavefrontObject.h"

#define NUM_OBJECTS 2
#define faces 20
using namespace std;

/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.


char objectFiles[NUM_OBJECTS][50] =
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

glcWavefrontObject *objectManager = NULL;

/// Globals
colisao cl = colisao(1);
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;
int   width = 1000, height = 600;

int selected = 0;
int selectedShading = SMOOTH_SHADING;
int selectedRender = USE_MATERIAL;


float desiredFPS = 30; //define fps do deslocamento da esfera
float direction = 0.0;//direção do disparador em angulos
float velX = 0, velY = 0, velInicial = 12.5; //variavel que determinam a velocidade da esfera
bool projection = false; //variavel que determina que tipo de projeção utilizar
bool paused = false; // variavel para determiar se o jogo esta pausado
bool fullscreen = false; // variavel para determiar se o jogo esta em fullscreen
bool movimentacao = false; // variavel para determiar se o jogo esta com movimentacao livre na projecao em perpectiva
bool terminou = false; // variável para determinar o fim do jogo
bool intro = true;

glcTexture *textureManager;
int texturaSelecionada = 0;

//variaveis utilizadas para definir a posição dos tijolos
float x = 1.0, y = 1.0;
float mColor[matrizLinha][3];//matriz para corolir os tijolos

float PI = 3.1415927;
vertice v[faces+1];
vertice normal[faces+1];


vertice v1[faces+1];
vertice normal1[faces+1];


vertice v2[faces+1];
vertice normal2[faces+1];


float chanceAparicao = 1;
float velObjeto = 5;
vertice cores[4] = {{0,0,0},{0.5,0.5,0.5},{0,0,1},{1,0,0}};
/// Functions
void criaCirculo()
{
    float raio = (cl.barraH*cl.barraH + (cl.barraW/2)*(cl.barraW/2))/(2*cl.barraH);
    float angulo = (2*asin(cl.barraW/2/raio))*180/PI;
    float g =(180-angulo)/2;
    float k = angulo/faces;
    cl.dist = (0.5+raio)*(0.5+raio);
    cl.yRebatedor = cl.barraY+cl.barraH-raio;
    for(int i = 0; i < faces+1; i++)
    {

        v[i].x = raio * cos(g * PI / 180);
        v[i].y = raio * sin(g * PI / 180)+cl.yRebatedor;
        v[i].z = 0;
        normal[i].x = 1 * cos(g * PI / 180);
        normal[i].y = 1 * sin(g * PI / 180);
        normal[i].z = 0;
        g += k;
    }

    raio = (0.5*0.5 + 4*4)/(2*0.5);
    angulo = (2*asin(4/raio))*180/PI;
    g = 360-(angulo/2);
    k = angulo/faces;
    cl.dist12 = (0.5+raio)*(0.5+raio);
    cl.xLombadaE = -9.5 -raio;
    for(int i = 0; i < faces+1; i++)
    {

        v1[i].x = raio * cos(g * PI / 180)+cl.xLombadaE;
        v1[i].y = raio * sin(g * PI / 180);
        v1[i].z = 0;
        normal1[i].x = 1 * cos(g * PI / 180);
        normal1[i].y = 1 * sin(g * PI / 180);
        normal1[i].z = 0;
        g += k;
    }
    g = (360-angulo)/2;
    cl.xLombadaD = 9.5+raio;
    for(int i = 0; i < faces+1; i++)
    {

        v2[i].x = raio * cos(g * PI / 180)+cl.xLombadaD;
        v2[i].y = raio * sin(g * PI / 180);
        v2[i].z = 0;
        normal2[i].x = 1 * cos(g * PI / 180);
        normal2[i].y = 1 * sin(g * PI / 180);
        normal2[i].z = 0;
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
    for (int j = 0; j < matrizLinha; j++)//definição de cores aleatórias para os tijolos
    {
        mColor[j][0] = float((rand() % 10)) / 9.0;
        mColor[j][1] = float((rand() % 10)) / 9.0;
        mColor[j][2] = float((rand() % 10)) / 9.0;
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
    cl.fs.construirGrid(cl.fs.seletor);
    colorir(cl.fs.seletor);

    textureManager = new glcTexture();
    textureManager->SetNumberOfTextures(2);
    textureManager->CreateTexture("../data/pressStart.png", 0);
    textureManager->CreateTexture("../data/skybox.png", 1);

    objectManager = new glcWavefrontObject();
    objectManager->SetNumberOfObjects(NUM_OBJECTS);
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        objectManager->SelectObject(i);
        objectManager->ReadObject(objectFiles[i]);
        objectManager->Unitize();
        objectManager->FacetNormal();
        objectManager->VertexNormals(90.0);
        objectManager->Scale(3.0);
    }

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
    glNormal3f(0,0,-1);
    glVertex3f(vBase[0].x, vBase[0].y, vBase[0].z);
    glVertex3f(vBase[3].x, vBase[3].y, vBase[3].z);
    glVertex3f(vBase[2].x, vBase[2].y, vBase[2].z);
    glVertex3f(vBase[1].x, vBase[1].y, vBase[1].z);


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

    /*setColor(cores[0].x,cores[0].y,cores[0].z);
    glBegin(GL_TRIANGLE_FAN);

    glNormal3f(0,0,1);
    glVertex3f(10,10,0);
    glVertex3f(-10,10,0);
    glVertex3f(-10,-10,0);
    glVertex3f(10,-10,0);
    glEnd();*/

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
    glNormal3f(0,0,1);
    glVertex3f(v1[faces].x, v1[faces].y, altura);
    for(int i = 0; i < faces; i++)
    {
        glVertex3f(v1[i].x, v1[i].y, altura);
    }
    glEnd();
    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,-1);
    glVertex3f(v1[0].x, v1[0].y, 0);
    for(int i = faces; i > 0 ; i--)
    {
        glVertex3f(v1[i].x, v1[i].y, 0);
    }
    glEnd();
    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glVertex3f(v1[faces].x, v1[faces].y, 0);
    glVertex3f(v1[0].x, v1[0].y, 0);
    glVertex3f(v1[0].x, v1[0].y, altura);
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
    glNormal3f(0,0,1);
    glVertex3f(v2[faces].x, v2[faces].y, altura);
    for(int i = 0; i < faces; i++)
    {
        glVertex3f(v2[i].x, v2[i].y, altura);
    }
    glEnd();
    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,-1);
    glVertex3f(v2[0].x, v2[0].y, 0);
    for(int i = faces; i > 0; i--)
    {
        glVertex3f(v2[i].x, v2[i].y, 0);
    }
    glEnd();
    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glVertex3f(v2[faces].x, v2[faces].y, 0);
    glVertex3f(v2[0].x, v2[0].y, 0);
    glVertex3f(v2[0].x, v2[0].y, altura);
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
            glVertex3f(v[i].x+cl.xRebatedor, v[i].y, 0);
            glNormal3f( normal[i+1].x, normal[i+1].y,-normal[i].z);
            glVertex3f(v[i+1].x+cl.xRebatedor, v[i+1].y, 0);
            glNormal3f( normal[i+1].x, normal[i+1].y,normal[i].z);
            glVertex3f(v[i+1].x+cl.xRebatedor, v[i+1].y,  altura);
            glNormal3f( normal[i].x, normal[i].y,normal[i].z);
            glVertex3f(v[i].x+cl.xRebatedor, v[i].y,  altura);
        glEnd();
    }

    ///Topo
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,1);
    glVertex3f(v[faces].x+cl.xRebatedor, v[faces].y, altura);
    for(int i = 0; i < faces; i++)
    {
        glVertex3f(v[i].x+cl.xRebatedor, v[i].y, altura);
    }
    glEnd();
    ///Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,-1);
    glVertex3f(v[0].x+cl.xRebatedor, v[0].y, 0);
    for(int i = faces; i > 0; i--)
    {
        glVertex3f(v[i].x+cl.xRebatedor, v[i].y, 0);
    }
    glEnd();
    ///Frente
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glVertex3f(v[faces].x+cl.xRebatedor, v[faces].y, 0);
    glVertex3f(v[0].x+cl.xRebatedor, v[0].y, 0);
    glVertex3f(v[0].x+cl.xRebatedor, v[0].y, altura);
    glVertex3f(v[faces].x+cl.xRebatedor, v[faces].y, altura);
    glEnd();
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    //A variavel projection e utilizada para definir se a visão será ortogonal ou em perspectiva, codigo tirado do projection.cpp que esta no material da disciplina
    int ortho = 10;
    if  (intro)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glutSetCursor(GLUT_CURSOR_NONE);
        glOrtho (-ortho, ortho, -ortho*height/width, ortho*height/width, -100.0, 100.0);
        //glDisable(GL_LIGHTING);
        textureManager->Bind(texturaSelecionada);

        GLfloat posicao_luz[] = { 0, -10, 14, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

        gluLookAt (0.0, 0.0, 8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        glPushMatrix();
        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-10.0,-10.0,0);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(10.0,-10.0,0);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(10.0,10.0,0);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(-10.0,10.0,0);
        glEnd();
        glPopMatrix();
        glutSwapBuffers();
    }
    else
    {
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

            glPushMatrix();
                texturaSelecionada = 1;
                textureManager->Bind(texturaSelecionada);
                setColor(1,1,1);
                glBegin(GL_QUADS);
                ///Base
                glNormal3f(0,0,1);
                glTexCoord2f(0.5,0.66);
                glVertex3f(90,90,-90);
                glTexCoord2f(0.25,0.66);
                glVertex3f(-90,90,-90);
                glTexCoord2f(0.25,0.33);
                glVertex3f(-90,-90,-90);
                glTexCoord2f(0.5,0.33);
                glVertex3f(90,-90,-90);
                glEnd();

                ///Em Cima
                glBegin(GL_QUADS);
                glNormal3f(0,-1,0);
                glTexCoord2f(0.5,0.66);
                glVertex3f(90,90,-90);
                glTexCoord2f(0.5,1);
                glVertex3f(90,90,90);
                glTexCoord2f(0.25,1);
                glVertex3f(-90,90,90);
                glTexCoord2f(0.25,0.66);
                glVertex3f(-90,90,-90);
                glEnd();

                ///Direita
                glBegin(GL_QUADS);
                glNormal3f(-1,0,0);
                glTexCoord2f(0.5,0.33);
                glVertex3f(90,-90,-90);
                glTexCoord2f(0.75,0.33);
                glVertex3f(90,-90,90);
                glTexCoord2f(0.75,0.66);
                glVertex3f(90,90,90);
                glTexCoord2f(0.5,0.66);
                glVertex3f(90,90,-90);
                glEnd();


                ///Esquerda
                glBegin(GL_QUADS);
                glNormal3f(1,0,0);
                glTexCoord2f(0.25,0.66);
                glVertex3f(-90,90,-90);
                glTexCoord2f(0,0.66);
                glVertex3f(-90,90,90);
                glTexCoord2f(0,0.33);
                glVertex3f(-90,-90,90);
                glTexCoord2f(0.25,0.33);
                glVertex3f(-90,-90,-90);
                glEnd();


                ///Em Baixo
                glBegin(GL_QUADS);
                glNormal3f(0,1,0);
                glTexCoord2f(0.25,0.33);
                glVertex3f(-90,-88,-90);
                glTexCoord2f(0.25,0);
                glVertex3f(-90,-88,90);
                glTexCoord2f(0.5,0);
                glVertex3f(90,-88,90);
                glTexCoord2f(0.5,0.33);
                glVertex3f(90,-88,-90);
                glEnd();


                ///Teto
                glBegin(GL_QUADS);
                glNormal3f(0,0,-1);
                glTexCoord2f(0.75,0.33);
                glVertex3f(90,-90,90);
                glTexCoord2f(1.0,0.33);
                glVertex3f(-90,-90,90);
                glTexCoord2f(1.0,0.66);
                glVertex3f(-90,90,90);
                glTexCoord2f(0.75,0.66);
                glVertex3f(90,90,90);
                glEnd();
                textureManager->Disable();

            glPopMatrix();

            setColor(cores[3].x,cores[3].y,cores[3].z);
            drawBarra();
            setColor(1,1,0);
            ///Pedra 1 entra nesse glPushMatrix
            glPushMatrix();
                setColor(1,0,0);
                glTranslatef(cl.objetos[0].x,cl.objetos[0].y,cl.objetos[0].z);
                glScalef(cl.objEscala[0],cl.objEscala[0],cl.objEscala[0]);
                //if(cl.objCond[0]!=3)
                //    glutSolidSphere(cl.objEscala[0],100,100);
                if(cl.objCond[0]!=3)
                {
                    objectManager->SelectObject(1);
                    objectManager->SetShadingMode(selectedShading); // Possible values: FLAT_SHADING e SMOOTH_SHADING
                    objectManager->SetRenderMode(selectedRender);     // Possible values: USE_COLOR, USE_MATERIAL, USE_TEXTURE (not available in this example)

                    objectManager->Draw();
                }
            glPopMatrix();
            ///Pedra 2 entra nesse glPushMatrix
            glPushMatrix();
                setColor(0,0,1);
                glTranslatef(cl.objetos[1].x,cl.objetos[1].y,cl.objetos[1].z);
                //if(cl.objCond[1]!=3)
                //    glutSolidSphere(cl.objEscala[1],100,100);
                glScalef(cl.objEscala[1],cl.objEscala[1],cl.objEscala[1]);
                if(cl.objCond[1]!=3)
                {
                    objectManager->SelectObject(1);
                    objectManager->SetShadingMode(selectedShading); // Possible values: FLAT_SHADING e SMOOTH_SHADING
                    objectManager->SetRenderMode(selectedRender);     // Possible values: USE_COLOR, USE_MATERIAL, USE_TEXTURE (not available in this example)
                    //objectManager->Scale(10);
                    objectManager->Draw();
                }
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
                    if(cl.fs.m[i][j].alive) // desenha tijolo apenas se n foi destruido
                    {
                        drawBrick(cl.fs.m[i][j].vMenor.x, cl.fs.m[i][j].vMenor.y, compriment, largur, altur);
                    }

                }
            }
            drawEnviroment();
            drawLombadas();
            glPushMatrix();
                setColor(0,1,0);
                glTranslatef(cl.sphereX,cl.sphereY,0.5);
                glutSolidSphere(0.5,100,100);
            if(!cl.launched && !terminou && !cl.gameOver){//verifica se a bola foi lançada para desenhar o disparador
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
}

    //if(drawboundingbox) objectManager->DrawBoundingBox();
    //if(drawaxes)        drawAxes();
    glutSwapBuffers();
}

void faseTerminou() //funcao para determinar se a fase terminou
{
    for(int i = 0; i < matrizLinha; i++)
    {
        for(int j = 0; j < matrizColuna; j++)
        {
            if(cl.fs.m[i][j].alive)
            {
                return;
            }
        }
    }
    cl.launched = false;
    cl.fs.fase();
    if (cl.fs.seletor == 3)
    {
        terminou = true;
        return;
    }
    else
    {
        cl.fs.construirGrid(cl.fs.seletor);
        colorir(cl.fs.seletor);
    }
    return;
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
    if(!paused && !terminou && !cl.gameOver)//verifica se o jogo esta pausado ou terminou a fase
    {
        cl.testeColisao(2);
        if(cl.launched)//verifica se a esfera foi lançada, se caso ocorrer movimenta a esfera com a direção e velocidades apropriados
        {
            velX = cl.dx*velInicial;
            velY = cl.dy*velInicial;
            cl.sphereX+=velX* (1 / desiredFPS);
            cl.sphereY+=velY* (1 / desiredFPS);
        }
        else//determina direção do disparador e posição inicial da esfera caso nao seja lançada
        {
            cl.dx = cos((direction+90)*0.0174533);
            cl.dy = sin((direction+90)*0.0174533);
            cl.sphereX = cl.barraX+cl.barraW/2;
            cl.sphereY = cl.barraY + 0.5 + cl.barraH;
        }

        for(int i = 0; i<2; i++)
        {
            switch(cl.objCond[i])
            {
            case 0:
                cl.objetos[i].y -= 0.1;
                if(cl.objetos[i].y <= 9.5)
                    cl.objCond[i] = 1;
                break;
            case 1:
                cl.testeColisao(i);
                if(cl.colisaoObjeto(i))
                {
                    cl.objCond[i] = 2;
                    break;
                }
                cl.objetos[i].x += cl.dirX[i]*velObjeto* (1 / desiredFPS);
                cl.objetos[i].y += cl.dirY[i]*velObjeto* (1 / desiredFPS);
                break;
            case 2:
                cl.objEscala[i] -= 0.1;
                if(cl.objEscala[i] <= 0.1)
                    cl.objCond[i] = 3;
                break;
            case 3:
                chance = rand()%100;
                if(chance <= chanceAparicao)
                {
                    cl.objetos[i].y = 10.5;
                    aux = rand()%2;
                    if(aux)
                    {
                        cl.objetos[i].x = 5;
                    }
                    else
                    {
                        cl.objetos[i].x = -5;
                    }
                    chanceAparicao = 1;
                    cl.objCond[i] = 0;
                    cl.objEscala[i] = 0.5;
                    aux = rand()%100;
                    cl.dirX[i] = aux;
                    aux = rand()%100;
                    cl.dirY[i] = aux;
                    float len_v = sqrt(cl.dirX[i]*cl.dirX[i] + cl.dirY[i]*cl.dirY[i]);
                    cl.dirX[i] /= len_v;
                    cl.dirY[i] /= len_v;
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
        cl.objCond[0] = 3;
        cl.objCond[1] = 3;
        cl.fs.seletor = 0;
        cl.fs.construirGrid(cl.fs.seletor);
        colorir(cl.fs.seletor);
        cl.launched = false;
        cl.barraX = -cl.barraW/2;
        direction = 0;
        cl.vida = 5;
        paused = false;
        cl.gameOver = false;
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
    if(!paused && !terminou && !cl.gameOver)
    {
        cl.barraX +=aux;
        cl.xRebatedor = cl.barraX + cl.barraW/2;
    }
    if(cl.barraX > 10-cl.barraW)
    {
        cl.barraX = 10-cl.barraW;
        cl.xRebatedor = cl.barraX + cl.barraW/2;
    }
    else if (cl.barraX < -10)
    {
        cl.barraX = -10;
        cl.xRebatedor = cl.barraX + cl.barraW/2;
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
        if(!paused && !terminou && !cl.gameOver)
        {
            cl.barraX +=aux;
            cl.xRebatedor = cl.barraX + cl.barraW/2;
        }
        if(cl.barraX > 10-cl.barraW)
        {
            cl.barraX = 10-cl.barraW;
            cl.xRebatedor = cl.barraX + cl.barraW/2;
        }
        else if (cl.barraX < -10)
        {
            cl.barraX = -10;
            cl.xRebatedor = cl.barraX + cl.barraW/2;
        }
        last_x = x;
        last_y = y;
    }
}
// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !cl.launched && !paused && !terminou && !cl.gameOver && !intro)
    {
        cl.launched = true;
    }
    if(button == 3 && !paused && !terminou && !cl.gameOver) // Scroll up
    {
        if(!cl.launched && direction < 75)
        {
            direction +=1;
        }
    }
    if(button == 4 && !paused && !terminou && !cl.gameOver) // Scroll Down
    {
        if(!cl.launched && direction > -75)
        {
            direction -=1;
        }
    }
    if ( button == GLUT_LEFT_BUTTON && intro)
    {
        intro = false;
        cout << "----" << endl;
    }
}
void f12 (int key, int x, int y)
{
    if(key == GLUT_KEY_F12)
    {
        fullscreen = !fullscreen;
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
