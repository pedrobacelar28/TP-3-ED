#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef QUADTREE_H
#define QUADTREE_H

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
void dividir(QuadTree* qt);
int buscaPonto(QuadTree* qt, Ponto p);
Ponto* buscaIntervalo(QuadTree* qt, Retangulo r, int* count);
void buscaKNN(QuadTree* qt, Ponto p, int K, Ponto* pontos, int* count);

#endif // QUADTREE_H
