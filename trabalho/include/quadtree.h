#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef QUADTREE_H
#define QUADTREE_H
#define DBL_MAX 100000000000000

#include <stdbool.h>
#include "tipos_comuns.h"  // Inclua o novo arquivo de tipos comuns
// Definição da estrutura Ponto
typedef struct {
    double x, y;
    addr_t *station_info; // Informação da estação
} Ponto;

// Definição da estrutura Retangulo
typedef struct {
    Ponto lb;  // Lower Bound (inferior esquerdo)
    Ponto rt;  // Upper Bound (superior direito)
} Retangulo;
// Definição da estrutura PontoDistancia para KNN
typedef struct {
    Ponto* ponto;
    double distancia;
} PontoDistancia;
// Função para verificar se um ponto está dentro de um retângulo
bool contemPonto(Retangulo r, Ponto p);
bool interseccao(Retangulo r1, Retangulo r2);

// Definição da estrutura QuadTree
typedef struct QuadTree {
    Retangulo limites;
    Ponto* ponto;
    struct QuadTree* northWest;
    struct QuadTree* northEast;
    struct QuadTree* southWest;
    struct QuadTree* southEast;
} QuadTree;

// Funções da QuadTree
QuadTree* criarQuadTree(Retangulo boundary);
bool insere(QuadTree* qt, Ponto p);
bool contemPonto(Retangulo r, Ponto p);
bool interseccao(Retangulo r1, Retangulo r2);
void dividir(QuadTree* qt);
Ponto* buscarPontoPorIdend(QuadTree *qt, const char *idend);
double calcularDistancia(Ponto a, Ponto b);
bool pontoJaNoHeap(PontoDistancia* heap, int tamanhoHeap, Ponto* ponto);
void maxHeapify(PontoDistancia* heap, int tamanhoHeap, int i);
void inserirPontoProximo(PontoDistancia* heap, int* tamanhoHeap, int K, Ponto* ponto, double distancia);
void buscaKNNRecursiva(QuadTree* qt, Ponto p, PontoDistancia* heap, int* tamanhoHeap, int K);

#endif // QUADTREE_H
