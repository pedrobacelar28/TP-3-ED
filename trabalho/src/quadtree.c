#include "quadtree.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

bool contemPonto(Retangulo r, Ponto p) {
    return (p.x >= r.lb.x && p.x <= r.rt.x && p.y >= r.lb.y && p.y <= r.rt.y);
}

bool interseccao(Retangulo r1, Retangulo r2) {
    return !(r2.lb.x > r1.rt.x || r2.rt.x < r1.lb.x || r2.lb.y > r1.rt.y || r2.rt.y < r1.lb.y);
}

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

void dividir(QuadTree* qt) {
    int midX = (qt->limites.lb.x + qt->limites.rt.x) / 2;
    int midY = (qt->limites.lb.y + qt->limites.rt.y) / 2;

    Retangulo nw = {{qt->limites.lb.x, midY}, {midX, qt->limites.rt.y}};
    Retangulo ne = {{midX, midY}, {qt->limites.rt.x, qt->limites.rt.y}};
    Retangulo sw = {{qt->limites.lb.x, qt->limites.lb.y}, {midX, midY}};
    Retangulo se = {{midX, qt->limites.lb.y}, {qt->limites.rt.x, midY}};

    qt->northWest = criarQuadTree(nw);
    qt->northEast = criarQuadTree(ne);
    qt->southWest = criarQuadTree(sw);
    qt->southEast = criarQuadTree(se);
}

int buscaPonto(QuadTree* qt, Ponto p) {
    if (qt->ponto != NULL && qt->ponto->x == p.x && qt->ponto->y == p.y)
        return 1;

    if (qt->northWest != NULL) {
        if (buscaPonto(qt->northWest, p)) return 1;
        if (buscaPonto(qt->northEast, p)) return 1;
        if (buscaPonto(qt->southWest, p)) return 1;
        if (buscaPonto(qt->southEast, p)) return 1;
    }

    return 0; // Ponto não encontrado
}

Ponto* buscaIntervalo(QuadTree* qt, Retangulo r, int* count) {
    *count = 0;
    Ponto* pontos = NULL;

    if (!interseccao(qt->limites, r))
        return pontos;

    if (qt->ponto != NULL && contemPonto(r, *(qt->ponto))) {
        pontos = (Ponto*)malloc(sizeof(Ponto));
        *pontos = *(qt->ponto);
        (*count)++;
    }

    if (qt->northWest == NULL)
        return pontos;

    int nwCount, neCount, swCount, seCount;
    Ponto* nwPontos = buscaIntervalo(qt->northWest, r, &nwCount);
    Ponto* nePontos = buscaIntervalo(qt->northEast, r, &neCount);
    Ponto* swPontos = buscaIntervalo(qt->southWest, r, &swCount);
    Ponto* sePontos = buscaIntervalo(qt->southEast, r, &seCount);

    int totalCount = nwCount + neCount + swCount + seCount;
    if (totalCount > 0) {
        pontos = (Ponto*)realloc(pontos, (*count + totalCount) * sizeof(Ponto));
        if (nwPontos) {
            memcpy(pontos + *count, nwPontos, nwCount * sizeof(Ponto));
            free(nwPontos);
        }
        if (nePontos) {
            memcpy(pontos + *count + nwCount, nePontos, neCount * sizeof(Ponto));
            free(nePontos);
        }
        if (swPontos) {
            memcpy(pontos + *count + nwCount + neCount, swPontos, swCount * sizeof(Ponto));
            free(swPontos);
        }
        if (sePontos) {
            memcpy(pontos + *count + nwCount + neCount + swCount, sePontos, seCount * sizeof(Ponto));
            free(sePontos);
        }
        *count += totalCount;
    }

    return pontos;
}

void buscaKNN(QuadTree* qt, Ponto p, int K, Ponto* pontos, int* count) {
    if (!contemPonto(qt->limites, p))
        return;

    if (qt->ponto != NULL) {
        double dist = sqrt(pow(p.x - qt->ponto->x, 2) + pow(p.y - qt->ponto->y, 2));
        if (*count < K) {
            pontos[*count] = *(qt->ponto);
            (*count)++;
        } else {
            double maxDist = sqrt(pow(p.x - pontos[0].x, 2) + pow(p.y - pontos[0].y, 2));
            int maxIndex = 0;
            for (int i = 1; i < K; i++) {
                double tempDist = sqrt(pow(p.x - pontos[i].x, 2) + pow(p.y - pontos[i].y, 2));
                if (tempDist > maxDist) {
                    maxDist = tempDist;
                    maxIndex = i;
                }
            }
            if (dist < maxDist) {
                pontos[maxIndex] = *(qt->ponto);
            }
        }
    }

    if (qt->northWest != NULL) {
        buscaKNN(qt->northWest, p, K, pontos, count);
        buscaKNN(qt->northEast, p, K, pontos, count);
        buscaKNN(qt->southWest, p, K, pontos, count);
        buscaKNN(qt->southEast, p, K, pontos, count);
    }
}
