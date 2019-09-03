#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "extras.h"
#define NUMPRISMAS 8
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.
class vertice
{
    public:
        float x,y,z;
};

class triangle
{
    public:
        vertice v[3];
};

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
float tamPrisma = 1.5;
vertice vBases[NUMPRISMAS][3] = {0};
vertice vTopos[NUMPRISMAS][3] = {0};
vertice centros[NUMPRISMAS] = {0};

/// Functions
void init(void)
{
    srand(time(NULL));
    initLight(width, height);
    for (int i = 0; i < NUMPRISMAS; i ++)
    {
        int r = rand() % 2; // r e ry servem para decidir se o prima tera sua posição em x e y invertida
        int ry = rand() % 2;
        int rot = rand() % 10; // rot serve para decidir se o prisma vai ser rotacionado
        int angle = rand() % 90; // Gera um ângulo aletório entre 0 e 88 graus para a rotação
        x = rand() % 7; // 7 foi usado para impedir o prisma qualquer vértice do prisma fique fora do tabuleiro
        if (r==0)
            x = x * -1;
        y = rand() % 7;
        if (ry==0)
            y = y * -1;
        if (rot % 2)
        {
            float xaux[3] = {0}; // vetores que guardam as mudanças de posição usadas na rotação
            float yaux[3] = {0};

            /// Cálculo da rotação em Z para cada vértice
            xaux[0] = x*cos(angle*0.0174533) - y*sin(angle*0.0174533);
            yaux[0] = x*sin(angle*0.0174533) + y*cos(angle*0.0174533);

            xaux[1] = (x+tamPrisma)*cos(angle*0.0174533) - y*sin(angle*0.0174533);
            yaux[1] = (x+tamPrisma)*sin(angle*0.0174533) + y*cos(angle*0.0174533);

            xaux[2] = x*cos(angle*0.0174533) - (y+tamPrisma)*sin(angle*0.0174533);
            yaux[2] = x*sin(angle*0.0174533) + (y+tamPrisma)*cos(angle*0.0174533);

            vBases[i][0] = {xaux[0],yaux[0],0.0};
            vBases[i][1] = {xaux[1],yaux[1],0.0};
            vBases[i][2] = {xaux[2],yaux[2],0.0};

            /// Os vértices a seguir mudam apenas em altura, para fazer o topo do triângulo
            vTopos[i][0] = {xaux[0],yaux[0],1.0};
            vTopos[i][1] = {xaux[1],yaux[1],1.0};
            vTopos[i][2] = {xaux[2],yaux[2],1.0};


        }
        else
        {
            vBases[i][0] = {x,y,0.0};
            vBases[i][1] = {tamPrisma+x,y,0.0};
            vBases[i][2] = {x,tamPrisma+y,0.0};

            /// Os vértices a seguir mudam apenas em altura, para fazer o topo do triângulo
            vTopos[i][0] = {x,y,1.0};
            vTopos[i][1] = {tamPrisma+x,y,1.0};
            vTopos[i][2] = {x,tamPrisma+y,1.0};
        }

        /// Cálculo do centro do prisma
        float xCentro = (vBases[i][0].x + vBases[i][1].x + vBases[i][2].x)/3;
        float yCentro = (vBases[i][0].y + vBases[i][1].y + vBases[i][2].y)/3;
        centros[i] = {xCentro, yCentro, 0.0};
    }
}

/* Exemplo de cálculo de vetor normal que são definidos a partir dos vértices do triângulo;
  v_2
  ^
  |\
  | \
  |  \       'vn' é o vetor normal resultante
  |   \
  +----> v_1
  v_0
*/
void CalculaNormal(triangle t, vertice *vn)
{
    vertice v_0 = t.v[0],
            v_1 = t.v[1],
            v_2 = t.v[2];
    vertice v1, v2;
    double len;

    /* Encontra vetor v1 */
    v1.x = v_1.x - v_0.x;
    v1.y = v_1.y - v_0.y;
    v1.z = v_1.z - v_0.z;

    /* Encontra vetor v2 */
    v2.x = v_2.x - v_0.x;
    v2.y = v_2.y - v_0.y;
    v2.z = v_2.z - v_0.z;

    /* Calculo do produto vetorial de v1 e v2 */
    vn->x = (v1.y * v2.z) - (v1.z * v2.y);
    vn->y = (v1.z * v2.x) - (v1.x * v2.z);
    vn->z = (v1.x * v2.y) - (v1.y * v2.x);

    /* normalizacao de n */
    len = sqrt(pow(vn->x,2) + pow(vn->y,2) + pow(vn->z,2));

    vn->x /= len;
    vn->y /= len;
    vn->z /= len;
}

void drawObject(vertice vBase[], vertice vTopo[])
{
   vertice vetorNormal;

    /// Os vetores abaixo são os 4 vértices necessários para delimitar cada lado do prisma
    vertice Lado1[4] = {vBase[0],
                        vBase[1],
                        vTopo[1],
                        vTopo[0]};
    vertice Lado2[4] = {vBase[0],
                        vTopo[0],
                        vTopo[2],
                        vBase[2]};
    vertice Lado3[4] = {vBase[2],
                        vTopo[2],
                        vTopo[1],
                        vBase[1]};

    /// O vetor a seguir armazena os vértices dos triângulos que iram compor a base e o topo dos prismas
    triangle t[2] = {{vBase[0], vBase[1], vBase[2]},
                     {vTopo[0], vTopo[1], vTopo[2]}};

    /// Os proximos triângulos serão as bases para os lados
    triangle t1 = {Lado1[0], Lado1[1], Lado1[2]};
    triangle t2 = {Lado2[0], Lado2[1], Lado2[2]};
    triangle t3 = {Lado3[0], Lado3[1], Lado3[2]};


    glBegin(GL_TRIANGLES);
        for(int i = 0; i < 2; i++) // Triangulos da base e do topo
        {
            CalculaNormal(t[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
            glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
            for(int j = 0; j < 3; j++) // Vertices do triangulo
                glVertex3f(t[i].v[j].x, t[i].v[j].y, t[i].v[j].z);
        }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        CalculaNormal(t1, &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++)
            glVertex3f(t1.v[j].x, t1.v[j].y, t1.v[j].z);
        glVertex3f(Lado1[3].x, Lado1[3].y, Lado1[3].z); // Esse último vértice é adicionado para formar o lado
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        CalculaNormal(t2, &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++)
            glVertex3f(t2.v[j].x, t2.v[j].y, t2.v[j].z);
        glVertex3f(Lado2[3].x, Lado2[3].y, Lado2[3].z);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        CalculaNormal(t3, &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++)
            glVertex3f(t3.v[j].x, t3.v[j].y, t3.v[j].z);
        glVertex3f(Lado3[3].x, Lado3[3].y, Lado3[3].z);
    glEnd();
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
        for (int i = 0; i < NUMPRISMAS; i++)
            drawObject(vBases[i], vTopos[i]);
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

//função para calcular distancia entre centro da esfera para um determinado vertice
float distanciaEsferaVertice (vertice ponto)
{
    float aux = pow(sphereX - ponto.x,2) + pow(sphereY - ponto.y,2);
    return sqrt(aux);
}

//função para calcular distancia entre centro da esfera para um determinado vertice
float distancia2vertices (vertice ponto1, vertice ponto2)
{
    float aux = pow(ponto2.x - ponto1.x,2) + pow(ponto2.y - ponto1.y,2);
    return sqrt(aux);
}

//função para verificar se a projeção do centro da esfera na linha definido pela aresta do prisma(definido pelos vertices ponto1 e ponto2) estaria na aresta
//retirado de https://stackoverflow.com/questions/17581738/check-if-a-point-projected-on-a-line-segment-is-not-outside-it
bool colisaoAresta (vertice ponto1,vertice ponto2) {
    double dx = ponto1.x - ponto2.x;
    double dy = ponto1.y - ponto2.y;
    double innerProduct = (sphereX - ponto2.x)*dx + (sphereY - ponto2.y)*dy;
    return 0 <= innerProduct && innerProduct <= dx*dx + dy*dy;
}

//função para verificar se a esfera colidiu com a aresta, retirada deste link: https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
bool colisionSphereLine(vertice ponto1,vertice ponto2)
{
    bool colidiu = false;
    float aux = fabs((ponto2.y-ponto1.y)*sphereX - (ponto2.x-ponto1.x)*sphereY + ponto2.x*ponto1.y - ponto2.y*ponto1.x);
    if(aux/distancia2vertices(ponto1,ponto2) <= 0.5)
    {
        colidiu = !colidiu;
    }
    return colidiu ;
}

//função para verificar se a esfera colidiu com algum prisma
void colision()
{
    float nx, ny;
    //itera para cada prisma
    for(int i = 0; i < NUMPRISMAS; i++)
    {
        vector< pair <float,int> > vect;// par de distancia dos vertices do prisma ao centro da esfera e o indice que o representa em vBase
        if(distanciaEsferaVertice(centros[i]) < raioAreaImpacto) // se estiver próximo da esfera continua com os testes
        {
            for(int j = 0; j < 3; j++)
            {
                vect.push_back( make_pair(distanciaEsferaVertice(vBases[i][j]),j) );//preenche o vetor vect com as distancias dos vertices do prisma ao centro da esfera e o indice que o representa em vBase
            }
            sort(vect.begin(), vect.end()); //ordena o vetor

            if(colisaoAresta(vBases[i][vect[0].second],vBases[i][vect[1].second]))//verifica se havera possibilidade de colisao entre a esfera e a aresta dos vertices mais proximos da esfera
            {
                if(colisionSphereLine(vBases[i][vect[0].second],vBases[i][vect[1].second]))//verifica se houve colisão
                {
                    //define a normal utilizando o perproduct apresentado nesse link https://www.metanetsoftware.com/technique/tutorialA.html#arvo
                    float ny = -(vBases[i][vect[0].second].x - vBases[i][vect[1].second].x);
                    float nx = vBases[i][vect[0].second].y - vBases[i][vect[1].second].y;
                    vertice auxV1 = {nx+vBases[i][vect[0].second].x,ny+vBases[i][vect[0].second].y,0};
                    vertice auxV2 = {-(nx+vBases[i][vect[0].second].x),-(ny+vBases[i][vect[0].second].y),0};
                    if(distancia2vertices(auxV1,centros[i]) < distancia2vertices(auxV2,centros[i]))//esta condição serve para definir o sentido da normal
                    {
                        nx *= -1;
                        ny *= -1;
                    }
                    calculaVetorNovo(nx,ny);
                    colide = true;
                    timet = glutGet(GLUT_ELAPSED_TIME)/1000.0;
                    break;
                }
            }
            else if(colisaoAresta(vBases[i][vect[0].second],vBases[i][vect[2].second]))//verifica se havera possibilidade de colisao entre a esfera e a aresta do vertices mais proximo e do mais distante da esfera
            {
                if(colisionSphereLine(vBases[i][vect[0].second],vBases[i][vect[2].second]))//verifica se houve colisão
                {
                    //define a normal utilizando o perproduct apresentado nesse link https://www.metanetsoftware.com/technique/tutorialA.html#arvo
                    float ny = -(vBases[i][vect[0].second].x - vBases[i][vect[2].second].x);
                    float nx = vBases[i][vect[0].second].y - vBases[i][vect[2].second].y;
                    vertice auxV1 = {nx+vBases[i][vect[0].second].x,ny+vBases[i][vect[0].second].y,0};
                    vertice auxV2 = {-(nx+vBases[i][vect[0].second].x),-(ny+vBases[i][vect[0].second].y),0};
                    if(distancia2vertices(auxV1,centros[i]) < distancia2vertices(auxV2,centros[i]))//esta condição serve para definir o sentido da normal
                    {
                        nx *= -1;
                        ny *= -1;
                    }
                    calculaVetorNovo(nx,ny);
                    colide = true;
                    timet = glutGet(GLUT_ELAPSED_TIME)/1000.0;
                    break;
                }
            }
            else if(vect[0].first <= 0.5)//se nao houve colisão com as arestas, testa se ocorreu colisão com o vertice mais proximo
            {
                calculaVetorNovo(sphereX-vBases[i][vect[0].second].x,sphereY-vBases[i][vect[0].second].y);
                colide = true;
                timet = glutGet(GLUT_ELAPSED_TIME)/1000.0;
                break;
            }
        }
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
    //esta condição impede que ocorra mais de uma resposta para a mesma colisão da esfera com um determinado prisma, já que impede de verificar colisão ate passar alguns milisegundos
    if(colide)
    {
        if(t-timet > 0.09)
            colide = false;
    }
    else
    {
        colision();//verifica colisão da esfera com os prismas
    }

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
