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


void faseSelector::construirGrid(int select)
{
    switch (select)
    {
    case 0:
        {
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
            break;
        }
    case 1:
        {
         float y = 2.0;
            for (int i = 0; i < matrizLinha; i++)
            {
                float x = -3.934;
                for (int j = 0; j < matrizColuna; j++)
                {
                    if(j <= i)
                    {
                        m[i][j].alive = true;
                        m[i][j].vMenor.x = x * (compriment + 0.3);//espaçamento horizontal e vertical entre os tijolos de 0,3
                        m[i][j].vMenor.y = y * (largur + 0.3);
                        m[i][j].vMenor.z = 0;
                    }
                    else
                        m[i][j].alive = false;
                    x++;
                }
                y++;
            }
            break;
        }
    case 2:
        {
            float y = 2.0;
            for (int i = 0; i < matrizLinha; i++)
            {
                float x = -3.934;
                for (int j = 0; j < matrizColuna; j++)
                {
                    if (i != j && (i + j) != (matrizColuna-1))
                    {
                        m[i][j].alive = true;
                        m[i][j].vMenor.x = x * (compriment + 0.3);//espaçamento horizontal e vertical entre os tijolos de 0,3
                        m[i][j].vMenor.y = y * (largur + 0.3);
                        m[i][j].vMenor.z = 0;
                    }
                    else
                        m[i][j].alive = false;
                    x++;
                }
                y++;
            }
            break;
        }
    }
    ///O grid começa a ser construído a partir de x = -3.934 e y = 2.0 para ter um espaçamento bom entre as paredes, os tijolos e a barra de lançamento

}

void faseSelector::fase()
{
    if (seletor != 3)
        seletor++;
}
