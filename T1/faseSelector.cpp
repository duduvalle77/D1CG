#include "include/faseSelector.h"

void tijolo::setVertice(float vx, float vy)
{
    vMenor.x = vx;
    vMenor.y = vy;
    vMenor.z = 0.0;
    alive = true;
}

void tijolo::colidiu()
{
    alive = false;
}

faseSelector::faseSelector()
{
    //ctor
}

faseSelector::~faseSelector()
{
    //dtor
}


void faseSelector::construirGrid()
{
    float y = 2.0;
    for (int i = 0; i < matrizLinha; i++)
    {
        float x = -4.0;
        for (int j = 0; j < matrizColuna; j++)
        {
            m[i][j].alive = true;
            m[i][j].vMenor.x = x * (comprimento + 0.3);
            m[i][j].vMenor.y = y * (largura + 0.3);
            m[i][j].vMenor.z = 0;
            x++;
        }
        y++;
    }
}
