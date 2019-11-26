#include "include/colisao.h"

colisao::colisao(int i)
{
    sphereX = 0;
    sphereY = -9.5;
    gameOver = false;
    vida = 5;
    objetos[0].x = 0;
    objetos[0].y = 0;
    objetos[0].z = 0.5;
    objetos[1].x = 0;
    objetos[1].y = 0;
    objetos[1].z = 0.5;
    objCond[0] = 3;
    objCond[1] = 3;
    objEscala[0] = 0.5;
    objEscala[0] = 0.5;
    dirX[0] = 0;
    dirX[1] = 0;
    dirY[2] = 0;
    dirY[2] = 0;
    launched = false;
    barraH = 1;
    barraW = 6.0;
    barraX = -barraW/2;
    barraY = -barraH/2-10+barraH*1.5;
    xRebatedor = barraX + barraW/2;
    yRebatedor = 0;
    dist12 = 0;
    dist = 0;
    yLombadaE=0;
    yLombadaD=0;
}
colisao::~colisao()
{
}

void colisao::calculaVetorNovo(float nx, float ny, int i)
{
    if(i<2)
    {
        //calcula direção do vetor novo utilizando a lei da reflexão
        float aux = 2*(-dirX[i]*nx + -dirY[i]*ny);
        dirX[i] = nx*aux + dirX[i];
        dirY[i] = ny*aux + dirY[i];
    }
    else
    {
        //calcula direção do vetor novo utilizando a lei da reflexão
        float aux = 2*(-dx*nx + -dy*ny);
        dx = nx*aux + dx;
        dy = ny*aux + dy;
    }
}

//função para detectar e responder a colisão da esfera com as paredes
void colisao::colisionAmbient(int i)
{
    if(i<2)
    {
        if(objetos[i].x > 9.5)
        {
            objetos[i].x = 9.5;
            calculaVetorNovo(-1,0,i);
        }
        if(objetos[i].x < -9.5)
        {
            objetos[i].x = -9.5;
            calculaVetorNovo(1,0,i);
        }
        if(objetos[i].y > 9)
        {
            objetos[i].y = 9;
            calculaVetorNovo(0,1,i);
        }
        if(objetos[i].y < -9.5)
        {
            objCond[i] = 2;
        }
    }
    else
    {
        if(sphereX > 9.5)
        {
            sphereX = 9.5;
            calculaVetorNovo(-1,0,i);
        }
        if(sphereX < -9.5)
        {
            sphereX = -9.5;
            calculaVetorNovo(1,0,i);
        }
        if(sphereY > 9)
        {
            sphereY = 9;
            calculaVetorNovo(0,1,i);
        }
        if(sphereY < -9.5)
        {
            launched = !launched;
            vida --;
            if (vida == 0)
                gameOver = true;
        }
    }
}
//funcao para determinar se houve colisao anteriormente com a mesma barra
bool colisao::distanciaNormalDirecao(float nx,float ny, int i)
{
    float d;
    if(i<2)
    {
        float auxX = nx - dirX[i];
        float auxY = ny - dirY[i];
        d = auxX*auxX + auxY*auxY;
    }
    else
    {
        float auxX = nx - dx;
        float auxY = ny - dy;
        d = auxX*auxX + auxY*auxY;
    }
    return d > 2;
}
//funcao para determinar se a barra esta a certa distancia da esfera
bool colisao::distanciaTijolo(float x, float y, float comprimento, float largura, int i)
{
    float auxX,auxY;
    if(i<2)
    {
        auxX = objetos[i].x - (x + compriment/2);
        auxY = objetos[i].y - (y + largur/2);
    }
    else
    {
        auxX = sphereX - (x + compriment/2);
        auxY = sphereY - (y + largur/2);
    }
    return auxX*auxX+auxY*auxY <= raioAreaImpacto*raioAreaImpacto;
}
//funcao para determinar se houve colisao entre a esfera e a barra
bool colisao::colisaoTijolo(float xTijolo, float yTijolo, float comprimento, float largura,int i)
{
    //colisao AABB deste livro: [Arvo 90] J. Arvo. “A Simple Method for Box-Sphere Intersection Testing.” In
    //Graphics Gems, edited by Andrew Glassner, pp. 335–339. San Diego, CA: Academic Press Professional, Inc., 1990.
    bool colidiu = false;
    float len_v = 0;
    float d = 0, aux = 0;
    float nx = 0, ny = 0;
    int xbool = 1, ybool = 1;
    float xEsfera, yEsfera;
    if(i<2)
    {
        xEsfera = objetos[i].x;
        yEsfera = objetos[i].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }

    if (xEsfera<xTijolo)
    {
        xbool = 0;
        aux = xEsfera - xTijolo;
        d = d+aux*aux;
    }
    else if (xEsfera>xTijolo+comprimento)
    {
        xbool = 2;
        aux = xEsfera - (xTijolo+comprimento);
        d = d+aux*aux;
    }

    if (yEsfera<yTijolo)
    {
        ybool = 0;
        aux = yEsfera - yTijolo;
        d = d+aux*aux;
    }
    else if (yEsfera>yTijolo+largura)
    {
        ybool = 2;
        aux = yEsfera - (yTijolo+largura);
        d = d+aux*aux;
    }

    if(d <= 0.25)
    {
        switch (xbool) // switch para determinar normal
        {
        case 0:
            switch (ybool)
            {
            case 0:
                nx = xEsfera - xTijolo;
                ny = yEsfera - yTijolo;
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            case 1:
                nx = -1;
                ny = 0;
                break;
            case 2:
                nx = xEsfera - xTijolo;
                ny = yEsfera - (yTijolo+largura);
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            }
            break;
        case 1:
            switch (ybool)
            {
            case 0:
                nx = 0;
                ny = -1;
                break;
            case 2:
                nx = 0;
                ny = 1;
                break;
            }
            break;
        case 2:
            switch (ybool)
            {
            case 0:
                nx = xEsfera - (xTijolo+comprimento);
                ny = yEsfera - yTijolo;
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            case 1:
                nx = 1;
                ny = 0;
                break;
            case 2:
                nx = xEsfera - (xTijolo+comprimento);
                ny = yEsfera - (yTijolo+largura);
                //parte para normalizar o vetor normal
                len_v = sqrt(nx*nx + ny*ny);
                nx /= len_v;
                ny /= len_v;
                break;
            }
            break;
        }
        if (distanciaNormalDirecao(nx,ny,i)) // verifica se ja houve colisao com essa barra
        {
            calculaVetorNovo(nx,ny,i);
            colidiu = true;
        }
    }
    return colidiu;
}

void colisao::colisaoBarra(float xTijolo, float yTijolo, float comprimento, float largura, int i)
{
    float len_v = 0;
    float aux = 0;
    float nx = 0, ny = 0;
    float xEsfera, yEsfera;
    if(i<2)
    {
        xEsfera = objetos[i].x;
        yEsfera = objetos[i].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }
    if(yEsfera<yTijolo)
    {
        return;
    }

    nx = xEsfera - xRebatedor;
    ny = yEsfera - yRebatedor;
    aux = nx*nx + ny*ny;
    //parte para normalizar o vetor normal
    len_v = sqrt(nx*nx + ny*ny);
    nx /= len_v;
    ny /= len_v;
    if (distanciaNormalDirecao(nx,ny,i) && aux <= dist) // verifica se ja houve colisao com essa barra
    {
        calculaVetorNovo(nx,ny,i);
        if(i<2)
        {
            objCond[i] = 2;
        }
    }
}

void colisao::colisaoLombada(float xTijolo, float yTijolo, float comprimento, float largura, int lombada, int i)
{
    float len_v = 0;
    float aux = 0;
    float nx = 0, ny = 0;
    int xbool = 1;
    float xlombada = 0;
    float xEsfera, yEsfera;
    if(i<2)
    {
        xEsfera = objetos[i].x;
        yEsfera = objetos[i].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }
    if (xEsfera<xTijolo)
    {
        xbool = 0;
    }
    else if (xEsfera>xTijolo+comprimento)
    {
        xbool = 2;
    }
    if(lombada == 0)
    {
        if(xbool == 0)
        {
            return;
        }
        xlombada = xLombadaE;
    }
    else
    {
        if(xbool == 2)
        {
            return;
        }
        xlombada = xLombadaD;
    }
    nx = xEsfera - xlombada;
    ny = yEsfera - yLombadaD;
    aux = nx*nx + ny*ny;
    //parte para normalizar o vetor normal
    len_v = sqrt(nx*nx + ny*ny);
    nx /= len_v;
    ny /= len_v;
    if (distanciaNormalDirecao(nx,ny,i) && aux <= dist12) // verifica se ja houve colisao com essa barra
    {
        calculaVetorNovo(nx,ny,i);
    }
}

void colisao::testeColisao(int k) //funcao para agrupar todas as colisoes
{
    float xEsfera, yEsfera;
    if(k<2)
    {
        xEsfera = objetos[k].x;
        yEsfera = objetos[k].y;
    }
    else
    {
        xEsfera = sphereX;
        yEsfera = sphereY;
    }
    colisionAmbient(k);
    for(int i = 0; i < matrizLinha; i++)
    {
        for(int j = 0; j < matrizColuna; j++)
        {
            float xTijolo = fs.m[i][j].vMenor.x, yTijolo = fs.m[i][j].vMenor.y;
            if((launched || k<2) && fs.m[i][j].alive && distanciaTijolo(xTijolo,yTijolo,compriment,largur,k))
            {
                if(colisaoTijolo(xTijolo,yTijolo,compriment,largur,k) && k == 2)
                {
                    fs.m[i][j].colidiu();
                }
            }
        }
    }
    if((launched || k<2) && yEsfera<-5)
    {
        colisaoBarra(barraX,barraY,barraW,barraH,k);
    }
    if((launched || k<2)&& xEsfera<-5)
    {
        colisaoLombada(-10,-4,0.5,8,0,k);
    }
    if((launched || k<2)&& xEsfera>5)
    {
        colisaoLombada(9.5,-4,0.5,8,1,k);
    }
}

bool colisao::colisaoObjeto(int i)
{
    float auxX = sphereX - objetos[i].x;
    float auxY = sphereY - objetos[i].y;
    float auxTotal = auxX*auxX+auxY*auxY;
    bool colidiu = auxTotal <= 1;
    float len_v = sqrt(auxTotal);
    auxX /= len_v;
    auxY /= len_v;
    if (distanciaNormalDirecao(auxX,auxY,2) && colidiu) // verifica se ja houve colisao com essa barra
    {
        calculaVetorNovo(auxX,auxY,2);
    }
    return colidiu;
}
