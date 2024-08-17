#include "quadtree.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
QuadTree* criarQuadTree(Retangulo boundary) {
    QuadTree* qt = (QuadTree*)malloc(sizeof(QuadTree));
    qt->limites = boundary;
    qt->ponto = NULL;
    qt->northWest = NULL;
    qt->northEast = NULL;
    qt->southWest = NULL;
    qt->southEast = NULL;
    return qt;
}

bool insere(QuadTree* qt, Ponto p) {
    if (!contemPonto(qt->limites, p))
        return false;

    if (qt->ponto == NULL) {
        qt->ponto = (Ponto*)malloc(sizeof(Ponto));
        *(qt->ponto) = p;
        return true;
    }

    if (qt->northWest == NULL)
        dividir(qt);

    if (insere(qt->northWest, p)) return true;
    if (insere(qt->northEast, p)) return true;
    if (insere(qt->southWest, p)) return true;
    if (insere(qt->southEast, p)) return true;

    return false;
}


bool contemPonto(Retangulo r, Ponto p) {
    return (p.x >= r.lb.x && p.x <= r.rt.x && p.y >= r.lb.y && p.y <= r.rt.y);
}

bool interseccao(Retangulo r1, Retangulo r2) {
    return !(r2.lb.x > r1.rt.x || r2.rt.x < r1.lb.x || r2.lb.y > r1.rt.y || r2.rt.y < r1.lb.y);
}




void dividir(QuadTree* qt) {
    double midX = (double)(qt->limites.lb.x + qt->limites.rt.x) / 2.0;
    double midY = (double)(qt->limites.lb.y + qt->limites.rt.y) / 2.0;

    Retangulo nw = {{qt->limites.lb.x, midY}, {midX, qt->limites.rt.y}};
    Retangulo ne = {{midX, midY}, {qt->limites.rt.x, qt->limites.rt.y}};
    Retangulo sw = {{qt->limites.lb.x, qt->limites.lb.y}, {midX, midY}};
    Retangulo se = {{midX, qt->limites.lb.y}, {qt->limites.rt.x, midY}};    

    qt->northWest = criarQuadTree(nw);
    qt->northEast = criarQuadTree(ne);
    qt->southWest = criarQuadTree(sw);
    qt->southEast = criarQuadTree(se);
}

double calcularDistancia(Ponto a, Ponto b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
bool pontoJaNoHeap(PontoDistancia* heap, int tamanhoHeap, Ponto* ponto) {
    for (int i = 0; i < tamanhoHeap; i++) {
        if (heap[i].ponto == ponto) {
            return true; // Ponto já está no heap
        }
    }
    return false;
}
void maxHeapify(PontoDistancia* heap, int tamanhoHeap, int i) {
    int maior = i;
    int esquerda = 2 * i + 1;
    int direita = 2 * i + 2;

    if (esquerda < tamanhoHeap && heap[esquerda].distancia > heap[maior].distancia)
        maior = esquerda;

    if (direita < tamanhoHeap && heap[direita].distancia > heap[maior].distancia)
        maior = direita;

    if (maior != i) {
        PontoDistancia temp = heap[i];
        heap[i] = heap[maior];
        heap[maior] = temp;

        maxHeapify(heap, tamanhoHeap, maior);
    }
}
// Função para inserir no max-heap (fila de prioridade) para armazenar os K pontos mais próximos
void inserirPontoProximo(PontoDistancia* heap, int* tamanhoHeap, int K, Ponto* ponto, double distancia) {
    if (pontoJaNoHeap(heap, *tamanhoHeap, ponto)) {
        return; // Não insere duplicatas
    }

    if (*tamanhoHeap < K) {
        // Inserir diretamente se ainda não atingimos K elementos
        heap[*tamanhoHeap].ponto = ponto;
        heap[*tamanhoHeap].distancia = distancia;
        (*tamanhoHeap)++;
        // Reorganiza o heap para manter a propriedade de max-heap
        for (int i = (*tamanhoHeap / 2) - 1; i >= 0; i--) {
            maxHeapify(heap, *tamanhoHeap, i);
        }
    } else if (distancia < heap[0].distancia) {
        // Substitui a raiz (maior elemento) e reorganiza o heap
        heap[0].ponto = ponto;
        heap[0].distancia = distancia;
        maxHeapify(heap, *tamanhoHeap, 0);
    }
}

void buscaKNNRecursiva(QuadTree* qt, Ponto p, PontoDistancia* heap, int* tamanhoHeap, int K) {
    if (qt == NULL) return;

    // Calcula a distância do ponto na QuadTree ao ponto de referência
    if (qt->ponto != NULL && qt->ponto->station_info != NULL && qt->ponto->station_info->active) {
        double distancia = calcularDistancia(*qt->ponto, p);
        inserirPontoProximo(heap, tamanhoHeap, K, qt->ponto, distancia);
    }

    // Ordena os quadrantes para explorar os mais próximos primeiro
    double distancias[4] = {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX};
    QuadTree* quadrantes[4] = {qt->northWest, qt->northEast, qt->southWest, qt->southEast};

    if (qt->northWest != NULL) 
        distancias[0] = calcularDistancia(p, (Ponto){(qt->limites.lb.x + qt->limites.rt.x)/2, qt->limites.rt.y, NULL});
    if (qt->northEast != NULL) 
        distancias[1] = calcularDistancia(p, (Ponto){qt->limites.rt.x, (qt->limites.lb.y + qt->limites.rt.y)/2, NULL});
    if (qt->southWest != NULL) 
        distancias[2] = calcularDistancia(p, (Ponto){qt->limites.lb.x, (qt->limites.lb.y + qt->limites.rt.y)/2, NULL});
    if (qt->southEast != NULL) 
        distancias[3] = calcularDistancia(p, (Ponto){(qt->limites.lb.x + qt->limites.rt.x)/2, qt->limites.lb.y, NULL});

    // Ordena os quadrantes em ordem de proximidade
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (distancias[j] < distancias[i]) {
                double tempDist = distancias[i];
                distancias[i] = distancias[j];
                distancias[j] = tempDist;

                QuadTree* tempQt = quadrantes[i];
                quadrantes[i] = quadrantes[j];
                quadrantes[j] = tempQt;
            }
        }
    }

    // Explora os quadrantes na ordem de proximidade
    for (int i = 0; i < 4; i++) {
        if (quadrantes[i] != NULL) {
            buscaKNNRecursiva(quadrantes[i], p, heap, tamanhoHeap, K);
        }
    }
}


