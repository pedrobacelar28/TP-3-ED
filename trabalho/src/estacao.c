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

void activate_station(const char *idend,QuadTree *stationTree) {
    int count = 0;
    Retangulo boundary = {{0, 0}, {100000000, 100000000}};
    Ponto *pontos = buscaIntervalo(stationTree, boundary, &count);

    for (int i = 0; i < count; i++) {
        if (strcmp(pontos[i].station_info->idend, idend) == 0) {
            if (pontos[i].station_info->active) {
                printf("Ponto de recarga %s já estava ativo.\n", idend);
            } else {
                pontos[i].station_info->active = 1;
                printf("Ponto de recarga %s ativado.\n", idend);
            }
            free(pontos);  // Libera a memória alocada antes de retornar
            return;
        }
    }
    
    printf("Ponto de recarga %s não encontrado.\n", idend);
    free(pontos);  // Libera a memória alocada após a busca
}

void deactivate_station(const char *idend,QuadTree *stationTree) {
    int count = 0;
    Retangulo boundary = {{0, 0}, {100000000, 100000000}};
    Ponto *pontos = buscaIntervalo(stationTree, boundary, &count);

    for (int i = 0; i < count; i++) {
        if (strcmp(pontos[i].station_info->idend, idend) == 0) {
            if (!pontos[i].station_info->active) {
                printf("Ponto de recarga %s já estava desativado.\n", idend);
            } else {
                pontos[i].station_info->active = 0;
                printf("Ponto de recarga %s desativado.\n", idend);
            }
            free(pontos);  // Libera a memória alocada antes de retornar
            return;
        }
    }

    printf("Ponto de recarga %s não encontrado.\n", idend);
    free(pontos);  // Libera a memória alocada após a busca
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
int compararDistancias(const void *a, const void *b) {
    PontoDistancia *pdA = (PontoDistancia *)a;
    PontoDistancia *pdB = (PontoDistancia *)b;

    if (pdA->distancia < pdB->distancia) return -1;
    if (pdA->distancia > pdB->distancia) return 1;
    return 0;
}
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

    // Ordena o heap (vetor) de pontos mais próximos pela distância
    qsort(heap, tamanhoHeap, sizeof(PontoDistancia), compararDistancias);

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




