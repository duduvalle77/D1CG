#ifndef FASESELECTOR_H
#define FASESELECTOR_H
#include <iostream>
#define matrizLinha 3
#define matrizColuna 3
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
        tijolo(){alive = false;};
        void setVertice(float vx, float vy); //função para preencher os pontos x e y do vMenor e colocar alive como verdadeiro
        void colidiu();//função para modificar alive para falso quando colidir

        bool alive;
        vertice vMenor;
};

class faseSelector
{
    public:
        ///Construtor e destrutor do seletor de fases
        faseSelector();
        ~faseSelector();
        void construirGrid(int select);//função para criar o grid, baseado na matriz m
        tijolo m[matrizLinha][matrizColuna];
        int seletor;
        void fase();

    protected:

    private:
};

#endif // FASESELECTOR_H
