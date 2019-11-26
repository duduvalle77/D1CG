#ifndef COLISAO_H
#define COLISAO_H
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include "faseSelector.h"
#include "glcWavefrontObject.h"
#include <iostream>

class colisao
{
    public:
        colisao(int i);
        ~colisao();
        void testeColisao(int k);
        bool colisaoObjeto(int i);
        float sphereX, sphereY;
        float dx, dy;
        bool gameOver;
        int vida;
        faseSelector fs;
        vertice objetos[2];
        int objCond[2];
        float objEscala[2];
        float dirX[2], dirY[2];
        bool launched;
        float barraH, barraW;
        float barraX, barraY;
        float xRebatedor, yRebatedor;
        float dist12;
        float dist;
        float xLombadaE,yLombadaE,xLombadaD,yLombadaD;
    protected:
    private:
        bool colisaoTijolo(float xTijolo, float yTijolo, float comprimento, float largura,int i);
        void colisaoBarra(float xTijolo, float yTijolo, float comprimento, float largura,int i);
        void colisaoLombada(float xTijolo, float yTijolo, float comprimento, float largura,int lombada,int i);
        void colisionAmbient(int i);
        void calculaVetorNovo(float nx, float ny, int i);
        bool distanciaNormalDirecao(float nx,float ny, int i);
        bool distanciaTijolo(float x, float y, float comprimento, float largura, int i);
};

#endif // COLISAO_H
