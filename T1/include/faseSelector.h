#ifndef FASESELECTOR_H
#define FASESELECTOR_H
#include <iostream>
#define largura 0.5
#define comprimento 2.0
#define altura 1.0
#define matrizLinha 10
#define matrizColuna 8
using namespace std;

class vertice
{
    public:
        float x,y,z;
};

class tijolo
{
    public:
        tijolo(){/**/};
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
        void construirGrid();

        tijolo m[matrizLinha][matrizColuna];

    protected:

    private:
};

#endif // FASESELECTOR_H
