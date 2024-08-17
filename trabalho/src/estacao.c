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
            *str = '.';  // Substitui vírgulas por pontos na string
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
    Retangulo boundary = {{0, 0}, {100000000, 100000000}}; // Define os limites iniciais da QuadTree
    *stationTree = criarQuadTree(boundary); // Inicializa a QuadTree
    int i = 0;

    // Lê cada linha do arquivo CSV
    while (fgets(line, sizeof(line), file)) {
        replace_comma_with_dot(line);  // Substitui as vírgulas por pontos na linha atual

        addr_t *station = (addr_t*)malloc(sizeof(addr_t));  // Aloca memória para os dados da estação
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
        station->active = 1;  // Marca a estação como ativa

        // Insere a estação na QuadTree
        Ponto p = {.x = station->x, .y = station->y, .station_info = station};
        if (insere(*stationTree, p)) {
            i++;
        }
    }

    fclose(file);  // Fecha o arquivo
    num_stations = i;  // Atualiza o número de estações carregadas
    return i;
}

// Função para ativar uma estação de recarga
void activate_station(const char *idend, QuadTree *stationTree) {
    if (stationTree == NULL) {
        printf("QuadTree não inicializada.\n");
        return;
    }

    // Verifica se o ponto atual corresponde ao ID
    if (stationTree->ponto != NULL && strcmp(stationTree->ponto->station_info->idend, idend) == 0) {
        if (stationTree->ponto->station_info->active) {
            printf("Ponto de recarga %s já estava ativo.\n", idend);
        } else {
            stationTree->ponto->station_info->active = 1;
            printf("Ponto de recarga %s ativado.\n", idend);
        }
        return;
    }

    // Busca recursiva nos quadrantes filhos
    if (stationTree->northWest) activate_station(idend, stationTree->northWest);
    if (stationTree->northEast) activate_station(idend, stationTree->northEast);
    if (stationTree->southWest) activate_station(idend, stationTree->southWest);
    if (stationTree->southEast) activate_station(idend, stationTree->southEast);
}

// Função para desativar uma estação de recarga
void deactivate_station(const char *idend, QuadTree *stationTree) {
    if (stationTree == NULL) {
        printf("QuadTree não inicializada.\n");
        return;
    }

    // Verifica se o ponto atual corresponde ao ID
    if (stationTree->ponto != NULL && strcmp(stationTree->ponto->station_info->idend, idend) == 0) {
        if (!stationTree->ponto->station_info->active) {
            printf("Ponto de recarga %s já estava desativado.\n", idend);
        } else {
            stationTree->ponto->station_info->active = 0;
            printf("Ponto de recarga %s desativado.\n", idend);
        }
        return;
    }

    // Busca recursiva nos quadrantes filhos
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
void find_nearest_stations(double x, double y, int n, QuadTree *stationTree) {
    // Cria o ponto de referência para a busca
    Ponto referencia = {x, y, NULL};

    // Aloca espaço para os n pontos mais próximos
    PontoDistancia* heap = (PontoDistancia*)malloc(n * sizeof(PontoDistancia));
    if (heap == NULL) {
        printf("Erro ao alocar memória para o heap.\n");
        return;
    }

    int tamanhoHeap = 0;

    // Inicializa o heap com distâncias infinitas
    for (int i = 0; i < n; i++) {
        heap[i].ponto = NULL;
        heap[i].distancia = DBL_MAX;
    }

    // Chama a busca KNN recursiva para preencher o heap com os n pontos mais próximos
    buscaKNNRecursiva(stationTree, referencia, heap, &tamanhoHeap, n);

    // Ordena os resultados encontrados por distância (bubble sort)
    for (int i = 0; i < tamanhoHeap - 1; i++) {
        for (int j = 0; j < tamanhoHeap - i - 1; j++) {
            if (heap[j].distancia > heap[j + 1].distancia) {
                PontoDistancia temp = heap[j];
                heap[j] = heap[j + 1];
                heap[j + 1] = temp;
            }
        }
    }   

    // Imprime as estações mais próximas
    for (int i = 0; i < tamanhoHeap; i++) {
        if (heap[i].ponto != NULL) {
            printrecharge(heap[i].ponto->station_info);
            printf(" (%.3f)\n", heap[i].distancia);  // Exibe a distância
        }
    }

    // Libera a memória alocada
    free(heap);
}
