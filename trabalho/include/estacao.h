#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef ESTACAO_H
#define ESTACAO_H

#include "quadtree.h"
#include "tipos_comuns.h" 

#define MAX_LENGTH 1024 // Define o tamanho máximo para buffers de strings

// Estrutura station_state_t para armazenar o estado de uma estação de recarga
typedef struct {
    addr_t station; // Endereço da estação (presumivelmente com informações detalhadas sobre a localização)
} station_state_t;

// Ponteiro para a árvore Quadtree que armazena as estações
extern QuadTree *stationTree; 

// Número total de estações carregadas
extern int num_stations; 

// Função para carregar dados das estações a partir de um arquivo CSV e armazená-las na Quadtree
int load_csv(const char *filename, QuadTree **stationTree);

// Função para ativar uma estação específica na Quadtree usando seu identificador
void activate_station(const char *idend, QuadTree *stationTree);

// Função para desativar uma estação específica na Quadtree usando seu identificador
void deactivate_station(const char *idend, QuadTree *stationTree);

// Função para imprimir informações das estações selecionadas para recarga
void printrecharge(addr_t *rechargevet);

// Função para encontrar as estações mais próximas de um ponto (x, y) e armazená-las
void find_nearest_stations(double x, double y, int n, QuadTree *stationTree);

#endif
