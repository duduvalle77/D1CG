#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <iostream>
#include "extras.h"
#define NUMPRISMAS 10
#include <ctime>
#include <vector>
#include <algorithm>
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
float zdist = 8.0;
float rotationX = 0.0, rotationY = 0.0, direction = 0.0;
int   last_x, last_y;
int   width, height;
float sphereX = 0, sphereY = -9.5, velX = 0, velY = 0, velInicial = 0.5;
float desiredFPS = 30;
bool launched = false , projection = true, colide = false;
float dx, dy;
float x = 1.0, y = 1.0, z = 1.0;
float timet = 0;
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
        int r = rand() % 2;
        int ry = rand() % 2;
        int rot = rand() % 10;
        int xrot = 0;
        x = rand() % 9;
        if (r==0)
            x = x * -1;
        y = rand() % 9;
        if (ry==0)
            y = y * -1;
        if (rot % 2)
            xrot = 1;
        vBases[i][0] = {x,y,0.0};
        vBases[i][1] = {1.0+x-xrot,y-xrot,0.0};
        vBases[i][2] = {x-xrot,1.0+y-xrot,0.0};
        vTopos[i][0] = {x,y,1.0};
        vTopos[i][1] = {1.0+x-xrot,y-xrot,1.0};
        vTopos[i][2] = {x-xrot,1.0+y-xrot,1.0};
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

    triangle t[2] = {{vBase[0], vBase[1], vBase[2]},
                     {vTopo[0], vTopo[1], vTopo[2]}};

    triangle t1 = {Lado1[0], Lado1[1], Lado1[2]};
    triangle t2 = {Lado2[0], Lado2[1], Lado2[2]};
    triangle t3 = {Lado3[0], Lado3[1], Lado3[2]};


    glBegin(GL_TRIANGLES);
        for(int i = 0; i < 2; i++) // triangulos
        {
            CalculaNormal(t[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
            glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
            for(int j = 0; j < 3; j++) // vertices do triangulo
                glVertex3f(t[i].v[j].x, t[i].v[j].y, t[i].v[j].z);
        }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        //CalculaNormal(t1, &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        //glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);

        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3f(t1.v[j].x, t1.v[j].y, t1.v[j].z);
        glVertex3f(Lado1[3].x, Lado1[3].y, Lado1[3].z);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        //CalculaNormal(t2, &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        //glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3f(t2.v[j].x, t2.v[j].y, t2.v[j].z);
        glVertex3f(Lado2[3].x, Lado2[3].y, Lado2[3].z);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        //CalculaNormal(t3, &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        //glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3f(t3.v[j].x, t3.v[j].y, t3.v[j].z);
        glVertex3f(Lado3[3].x, Lado3[3].y, Lado3[3].z);
    glEnd();
}

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
    if(!projection)
        gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    else
        gluLookAt (0.0, -18.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    float corDisparador = (3-velInicial)/5;

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
        if(!launched){
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

void calculaVetorNovo(float nx, float ny)
{
    float len_v = sqrt(nx*nx + ny*ny);
    nx /= len_v;
    ny /= len_v;
    float aux = 2*(-dx*nx + -dy*ny);
    dx = nx*aux + dx;
    dy = ny*aux + dy;
}

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

float distanceSphereToSomething (vertice ponto)
{
    float aux = pow(sphereX - ponto.x,2) + pow(sphereY - ponto.y,2);
    return sqrt(aux);
}

float distance2vertices (vertice ponto1, vertice ponto2)
{
    float aux = pow(ponto2.x - ponto1.x,2) + pow(ponto2.y - ponto1.y,2);
    return sqrt(aux);
}

bool inRange (vertice ponto1,vertice ponto2) {
    double dx = ponto1.x - ponto2.x;
    double dy = ponto1.y - ponto2.y;
    double innerProduct = (sphereX - ponto2.x)*dx + (sphereY - ponto2.y)*dy;
    return 0 <= innerProduct && innerProduct <= dx*dx + dy*dy;
}

bool colisionSphereLine(vertice ponto1,vertice ponto2)
{
    bool colidiu = false;
    float aux = fabs((ponto2.y-ponto1.y)*sphereX - (ponto2.x-ponto1.x)*sphereY + ponto2.x*ponto1.y - ponto2.y*ponto1.x);
    if(aux/distance2vertices(ponto1,ponto2) <= 0.5)
    {
        colidiu = !colidiu;
        //cout << aux/distance2vertices(ponto1,ponto2) << endl;
    }
    return colidiu ;
}

void colision()
{
    float nx, ny;
    for(int i = 0; i < NUMPRISMAS; i++)
    {
        vector< pair <float,int> > vect;// par indice de vertice
        if(distanceSphereToSomething(centros[i]) < 3.5)
        {
            for(int j = 0; j < 3; j++)
            {
                vect.push_back( make_pair(distanceSphereToSomething(vBases[i][j]),j) );
            }
            sort(vect.begin(), vect.end());

            if(inRange(vBases[i][vect[0].second],vBases[i][vect[1].second]))
            {
                if(colisionSphereLine(vBases[i][vect[0].second],vBases[i][vect[1].second]))
                {
                    float ny = -(vBases[i][vect[0].second].x - vBases[i][vect[1].second].x);
                    float nx = vBases[i][vect[0].second].y - vBases[i][vect[1].second].y;
                    vertice auxV1 = {nx+vBases[i][vect[0].second].x,ny+vBases[i][vect[0].second].y,0};
                    vertice auxV2 = {-(nx+vBases[i][vect[0].second].x),-(ny+vBases[i][vect[0].second].y),0};
                    if(distance2vertices(auxV1,centros[i]) < distance2vertices(auxV2,centros[i]))
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
            else if(inRange(vBases[i][vect[0].second],vBases[i][vect[2].second]))
            {
                if(colisionSphereLine(vBases[i][vect[0].second],vBases[i][vect[2].second]))
                {
                    float ny = -(vBases[i][vect[0].second].x - vBases[i][vect[2].second].x);
                    float nx = vBases[i][vect[0].second].y - vBases[i][vect[2].second].y;
                    vertice auxV1 = {nx+vBases[i][vect[0].second].x,ny+vBases[i][vect[0].second].y,0};
                    vertice auxV2 = {-(nx+vBases[i][vect[0].second].x),-(ny+vBases[i][vect[0].second].y),0};
                    if(distance2vertices(auxV1,centros[i]) < distance2vertices(auxV2,centros[i]))
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
            else if(vect[0].first <= 0.5)
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

    colisionAmbient();
    if(colide)
    {
        if(t-timet > 0.09)
            colide = false;
    }
    else
    {
       colision();
    }

    float step = 1;
    if(launched)
    {
        velX = dx*velInicial*2;
        velY = dy*velInicial*2;
        sphereX+=velX* (step / desiredFPS);
        sphereY+=velY* (step / desiredFPS);
    }
    else
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
