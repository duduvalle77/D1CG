#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <ctime>

#include "extras.h"

#define NUMPRISMAS 4

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
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;
int   width, height;
float x = 1.0, y = 1.0, z = 1.0;
/*vertice vBase[3] = {{1.0, 1.0, 0.0},
                    {2.0, 1.0, 0.0},
                    {1.0, 2.0, 0.0}};
vertice vBase1[3] = {{3.0, 3.0, 0.0},
                    {4.0, 3.0, 0.0},
                    {3.0, 4.0, 0.0}};
vertice vTopo[3] = {{1.0, 1.0, 1.0},
                    {2.0, 1.0, 1.0},
                    {1.0, 2.0, 1.0}};
vertice vTopo1[3] = {{3.0, 3.0, 1.0},
                    {4.0, 3.0, 1.0},
                    {3.0, 4.0, 1.0}};*/

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

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt (0.0, -18.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();
     // Função extra para tratar iluminação.
    setColorBase();
        glRotatef( rotationY, 0.0, 1.0, 0.0 );
        glRotatef( rotationX, 1.0, 0.0, 0.0 );
        glBegin(GL_POLYGON);
            glNormal3f(0,0,1);
            glVertex3f(10,10,0);
            glVertex3f(-10,10,0);
            glVertex3f(-10,-10,0);
            glVertex3f(10,-10,0);
        glEnd();

        glBegin(GL_POLYGON);
            glNormal3f(-1,0,0);
            glVertex3f(10,10,0);
            glVertex3f(10,-10,0);
            glVertex3f(10,-10,1);
            glVertex3f(10,10,1);
        glEnd();

        glBegin(GL_POLYGON);
            glNormal3f(0,-1,0);
            glVertex3f(10,10,0);
            glVertex3f(10,10,1);
            glVertex3f(-10,10,1);
            glVertex3f(-10,10,0);
        glEnd();

        glBegin(GL_POLYGON);
            glNormal3f(1,0,0);
            glVertex3f(-10,10,0);
            glVertex3f(-10,10,1);
            glVertex3f(-10,-10,1);
            glVertex3f(-10,-10,0);
        glEnd();

        glBegin(GL_POLYGON);
            glNormal3f(0,1,0);
            glVertex3f(10,-10,0);
            glVertex3f(-10,-10,0);
            glVertex3f(-10,-10,1);
            glVertex3f(10,-10,1);
        glEnd();
    setColor(1.0,0.0,0.0);
    for (int i = 0; i < NUMPRISMAS; i++)
        drawObject(vBases[i], vTopos[i]);

    glPopMatrix();

    glutSwapBuffers();
}

void idle ()
{
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    width = w;
    height = h;

    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.01, 200.0);
}

void keyboard (unsigned char key, int x1, int y)
{

    switch (tolower(key))
    {
        case 27:
            exit(0);
            break;
        case 'a':
//            x ++;
//            printf("%f",x);
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
