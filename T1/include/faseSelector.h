#ifndef FASESELECTOR_H
#define FASESELECTOR_H
#include <iostream>
#define largura 0.5
#define comprimento 1.5
#define altura 1.0

using namespace std;

class vertice
{
    public:
        float x,y,z;
};

class tijolo
{
    public:
        tijolo(){cout << "teste";};
        void setVertice(float vx, float vy);
        void colidiu();

        bool alive = false;
        vertice vMenor;
};

class faseSelector
{
    public:
        faseSelector();
        ~faseSelector();
        construirGrid();

    protected:

    private:
};

#endif // FASESELECTOR_H
