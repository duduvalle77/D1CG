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
    ///O grid começa a ser construído a partir de x = -3.934 e y = 2.0 para ter um espaçamento bom entre as paredes, os tijolos e a barra de lançamento
    float y = 2.0;
    for (int i = 0; i < matrizLinha; i++)
    {
        float x = -3.934;
        for (int j = 0; j < matrizColuna; j++)
        {
            m[i][j].alive = true;
            m[i][j].vMenor.x = x * (compriment + 0.3);//espaçamento horizontal e vertical entre os tijolos de 0,3
            m[i][j].vMenor.y = y * (largur + 0.3);
            m[i][j].vMenor.z = 0;
            x++;
        }
        y++;
    }
}
