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
