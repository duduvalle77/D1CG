#ifndef FASESELECTOR_H
#define FASESELECTOR_H
#include <iostream>
#define matrizLinha 10
#define matrizColuna 8
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
        tijolo(){/*Construtor vazio do Tijolo*/};
        void setVertice(float vx, float vy); //fun��o para preencher os pontos x e y do vMenor e colocar alive como verdadeiro
        void colidiu();//fun��o para modificar alive para falso quando colidir

        bool alive = false;
        vertice vMenor;
};

class faseSelector
{
    public:
        ///Construtor e destrutor do seletor de fases
        faseSelector();
        ~faseSelector();
        void construirGrid();//fun��o para criar o grid, baseado na matriz m

        tijolo m[matrizLinha][matrizColuna];

    protected:

    private:
};

#endif // FASESELECTOR_H
