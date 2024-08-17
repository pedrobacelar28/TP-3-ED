#include "estacao.h"
#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Variáveis globais
int num_stations = 0;

// Função auxiliar para substituir vírgulas por pontos em uma string
void replace_comma_with_dot(char *str) {
    while (*str) {
        if (*str == ',') {
            *str = '.';
        }
        str++;
    }
}

// Função para carregar os dados das estações de um arquivo CSV
int load_csv(const char *filename, QuadTree **stationTree) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo");
        return -1;
    }

    char line[MAX_LENGTH];
    Retangulo boundary = {{0, 0}, {100000000, 100000000}}; // Limites iniciais da QuadTree (ajuste conforme necessário)
    *stationTree = criarQuadTree(boundary); // Modificação para atualizar o ponteiro original
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        replace_comma_with_dot(line);

        addr_t *station = (addr_t*)malloc(sizeof(addr_t));
        sscanf(line, "%49[^;];%ld;%9[^;];%99[^;];%d;%49[^;];%49[^;];%d;%lf;%lf",
               station->idend,
               &station->id_logrado,
               station->sigla_tipo,
               station->nome_logra,
               &station->numero_imo,
               station->nome_bairr,
               station->nome_regio,
               &station->cep,
               &station->x,
               &station->y);
        station->active = 1;

        Ponto p = {.x = station->x, .y = station->y, .station_info = station};
        if (insere(*stationTree, p)) {
            i++;
        }
    }

    fclose(file);
    num_stations = i;
    return i;
}

void activate_station(const char *idend, QuadTree *stationTree) {
    if (stationTree == NULL) {
        printf("QuadTree não inicializada.\n");
        return;
    }

    if (stationTree->ponto != NULL && strcmp(stationTree->ponto->station_info->idend, idend) == 0) {
        if (stationTree->ponto->station_info->active) {
            printf("Ponto de recarga %s já estava ativo.\n", idend);
        } else {
            stationTree->ponto->station_info->active = 1;
            printf("Ponto de recarga %s ativado.\n", idend);
        }
        return;
    }

    if (stationTree->northWest) activate_station(idend, stationTree->northWest);
    if (stationTree->northEast) activate_station(idend, stationTree->northEast);
    if (stationTree->southWest) activate_station(idend, stationTree->southWest);
    if (stationTree->southEast) activate_station(idend, stationTree->southEast);
}

void deactivate_station(const char *idend, QuadTree *stationTree) {
    if (stationTree == NULL) {
        printf("QuadTree não inicializada.\n");
        return;
    }

    if (stationTree->ponto != NULL && strcmp(stationTree->ponto->station_info->idend, idend) == 0) {
        if (!stationTree->ponto->station_info->active) {
            printf("Ponto de recarga %s já estava desativado.\n", idend);
        } else {
            stationTree->ponto->station_info->active = 0;
            printf("Ponto de recarga %s desativado.\n", idend);
        }
        return;
    }

    if (stationTree->northWest) deactivate_station(idend, stationTree->northWest);
    if (stationTree->northEast) deactivate_station(idend, stationTree->northEast);
    if (stationTree->southWest) deactivate_station(idend, stationTree->southWest);
    if (stationTree->southEast) deactivate_station(idend, stationTree->southEast);
}

// Função para imprimir informações da estação de recarga
void printrecharge(addr_t *rechargevet) {
    printf("%s %s, %d, %s, %s, %d", 
           rechargevet->sigla_tipo,
           rechargevet->nome_logra, 
           rechargevet->numero_imo,
           rechargevet->nome_bairr, 
           rechargevet->nome_regio,
           rechargevet->cep);
}

// Função para encontrar as estações mais próximas
// Função para encontrar as estações mais próximas

// Função para encontrar as estações mais próximas
void find_nearest_stations(double x, double y, int n, QuadTree *stationTree) {
    // Cria o ponto de referência (a localização de onde queremos encontrar as estações mais próximas)
    Ponto referencia = {x, y, NULL};

    // Aloca espaço para os n pontos mais próximos
    PontoDistancia* heap = (PontoDistancia*)malloc(n * sizeof(PontoDistancia));
    if (heap == NULL) {
        printf("Erro ao alocar memória para o heap.\n");
        return;
    }

    int tamanhoHeap = 0;

    // Inicializa o heap de PontoDistancia com distâncias infinitas
    for (int i = 0; i < n; i++) {
        heap[i].ponto = NULL;
        heap[i].distancia = DBL_MAX;
    }

    // Chama a busca KNN recursiva para preencher o heap com os n pontos mais próximos
    buscaKNNRecursiva(stationTree, referencia, heap, &tamanhoHeap, n);
    for (int i = 0; i < tamanhoHeap - 1; i++) {
        for (int j = 0; j < tamanhoHeap - i - 1; j++) {
            if (heap[j].distancia > heap[j + 1].distancia) {
                PontoDistancia temp = heap[j];
                heap[j] = heap[j + 1];
                heap[j + 1] = temp;
            }
        }
    }   

    // Imprime os n pontos mais próximos encontrados, agora ordenados
    for (int i = 0; i < tamanhoHeap; i++) {
        if (heap[i].ponto != NULL) {
            printrecharge(heap[i].ponto->station_info);
            printf(" (%.3f)\n", heap[i].distancia);
        }
    }

    // Libera a memória alocada
    free(heap);
}




