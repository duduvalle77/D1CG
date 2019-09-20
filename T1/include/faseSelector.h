#ifndef FASESELECTOR_H
#define FASESELECTOR_H
#include <iostream>
#define matrizLinha 1
#define matrizColuna 1
#define largur 0.5
#define compriment 2
#define altur 1
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
