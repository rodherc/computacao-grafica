#include "clip-rectangle.h"
#include <iostream>
#include <cmath>

using namespace std;

ClipRectangle::ClipRectangle (double xmin, double xmax, double ymin, double ymax)
    : mXmin(xmin), mXmax(xmax), mYmin(ymin), mYmax(ymax), mInitialized(true) {}

void ClipRectangle::Set(double xmin, double xmax, double ymin, double ymax) {
    mXmin = xmin;
    mXmax = xmax;
    mYmin = ymin;
    mYmax = ymax;
    mInitialized = true;
}
 // definindo a região onde ocorrerá o recorte
 // 1000 acima
 // 0100 abaixo
 // 0010 direita
 // 0001 esquerda
unsigned int ClipRectangle::code(const Point p) {
    unsigned int codigo = 0;

    if (p.mY > this->mYmax) { // acima do retângulo de recorte 
        codigo += 8;
    }
    if (p.mY < this->mYmin) { // abaixo do retângulo de recorte.
        codigo += 4;
    }
    if (p.mX > this->mXmax) { // à direita
        codigo += 2;
    }
    if (p.mX < this->mXmin) {  // à esquerda
        codigo += 1;
    }

    return codigo;
}

// Retorna um ponteiro para uma linha recem alocada inteiramente dentro da area de recorte
// ou NULL se l for completamente descartada.
// A linha alocada aqui será desalocada na função CleanUp().
Line* ClipRectangle::Clip(const Line &l) {
    Line* novaL = new Line(l.mP0, l.mP1);

    unsigned int codigoP0 = code(l.mP0);
    unsigned int codigoP1 = code(l.mP1);
    unsigned int codigoSup;

    bool acabou = false;
    bool aceitou = false;
    
    double x = 0;
    double y = 0;

    do {
        // ambos os códigos forem zero, o segmento está todo dentro da região de recorte 
        // e não é necessário calcular nenhum ponto de interseção
        if ((codigoP0 == 0) and (codigoP1 == 0)) {
            acabou = true;
            aceitou = true;
        }
        
        // o resultado de um operação de “e” lógico bit a bit for diferente de zero, 
        // então os dois códigos tem um bit em comum e portanto o segmento está
        // totalmente fora do retângulo de recorte
        else if (codigoP0 & codigoP1) {
            acabou = true;
        }
        else {
            //se codigoSup = codigoP0, verdade codigop0, falso codigoP1
            codigoSup = codigoP0 ? codigoP0 : codigoP1;
            
            // calculando a interseção por semelhança de triangulos
            // x = x0 + ( (x1 - x0) * (y - y0) / (y1 - y0) )  
            if (codigoSup & 8) {
                x = novaL->mP0.mX + (((novaL->mP1.mX - novaL->mP0.mX) * (this->mYmax -
                    novaL->mP0.mY)) / (novaL->mP1.mY - novaL->mP0.mY));
                y = this->mYmax;
            }
            else if (codigoSup & 4) {
                x = novaL->mP0.mX + (((novaL->mP1.mX - novaL->mP0.mX) * (this->mYmin -
                    novaL->mP0.mY)) / (novaL->mP1.mY - novaL->mP0.mY));
                y = this->mYmin;
            }
            else if (codigoSup & 2) {
                y = novaL->mP0.mY + (((novaL->mP1.mY - novaL->mP0.mY) * (this->mXmax -
                novaL->mP0.mX)) / (novaL->mP1.mX - novaL->mP0.mX));
                x = this->mXmax;
            }
            else if (codigoSup & 1) {
                y = novaL->mP0.mY + (((novaL->mP1.mY - novaL->mP0.mY) * (this->mXmin -
                    novaL->mP0.mX)) / (novaL->mP1.mX - novaL->mP0.mX));
                x = this->mXmin;
            }
            // a interseção é classificada como dentro da área de recorte, fazendo com que o segmento superior
            // da subdivisão precise ser recortado com o lado superior novamente
            if (codigoSup == codigoP0) {
                novaL->mP0.mX = x;
                novaL->mP0.mY = y;
                codigoP0 = code(novaL->mP0);
            }
            // a interseção é classificada como acima da área de recorte, fazendo com que o segmento inferior
            // do da subdivisão precise ser recortado com o lado superior novamente.
            else {
                novaL->mP1.mX = x;
                novaL->mP1.mY = y;
                codigoP1 = code(novaL->mP1);
            }
        }
    } while(!acabou);

    if (aceitou) {
        return novaL;
    }

    return NULL;
}

void ClipRectangle::Read(const string &prompt) {
    cout << prompt;
    cin >> mXmin >> mXmax >> mYmin >> mYmax;
    if (not mInitialized)
        mInitialized = true;
}

ostream &operator << (ostream &os, const ClipRectangle &r) {
    return os << "(Clipping Rectangle: " << r.mXmin << ", " << r.mXmax << ", " 
              << r.mYmin << ", " << r.mYmax << ")";
}
