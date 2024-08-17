#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef QUADTREE_H
#define QUADTREE_H
#define DBL_MAX 100000000000000

#include <stdbool.h>
#include "tipos_comuns.h"  // Inclui tipos comuns necessários para a manipulação de dados

// Definição da estrutura Ponto
typedef struct {
    double x, y; // Coordenadas do ponto
    addr_t *station_info; // Informação da estação associada ao ponto
} Ponto;

// Definição da estrutura Retangulo
typedef struct {
    Ponto lb;  // Lower Bound (inferior esquerdo)
    Ponto rt;  // Upper Bound (superior direito)
} Retangulo;

// Definição da estrutura PontoDistancia para KNN
typedef struct {
    Ponto* ponto; // Ponto associado
    double distancia; // Distância do ponto ao ponto de referência
} PontoDistancia;

// Definição da estrutura QuadTree
typedef struct QuadTree {
    Retangulo limites; // Limites da área que a QuadTree cobre
    Ponto* ponto; // Ponto armazenado na QuadTree
    struct QuadTree* northWest; // Subárvore noroeste
    struct QuadTree* northEast; // Subárvore nordeste
    struct QuadTree* southWest; // Subárvore sudoeste
    struct QuadTree* southEast; // Subárvore sudeste
} QuadTree;

// Função para criar uma nova QuadTree
QuadTree* criarQuadTree(Retangulo boundary);

// Função para inserir um ponto na QuadTree
bool insere(QuadTree* qt, Ponto p);

// Função para verificar se um ponto está dentro de um retângulo (repetida)
bool contemPonto(Retangulo r, Ponto p);

// Função para verificar se dois retângulos se interceptam (repetida)
bool interseccao(Retangulo r1, Retangulo r2);

// Função para dividir a QuadTree em quatro sub-regiões
void dividir(QuadTree* qt);

// Função para calcular a distância euclidiana entre dois pontos
double calcularDistancia(Ponto a, Ponto b);

// Função para verificar se um ponto já está no heap
bool pontoJaNoHeap(PontoDistancia* heap, int tamanhoHeap, Ponto* ponto);

// Função para reorganizar o heap em forma de max-heap
void maxHeapify(PontoDistancia* heap, int tamanhoHeap, int i);

// Função para inserir um ponto próximo no heap para o KNN
void inserirPontoProximo(PontoDistancia* heap, int* tamanhoHeap, int K, Ponto* ponto, double distancia);

// Função recursiva para realizar a busca KNN na QuadTree
void buscaKNNRecursiva(QuadTree* qt, Ponto p, PontoDistancia* heap, int* tamanhoHeap, int K);

#endif // QUADTREE_H
