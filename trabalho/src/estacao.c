#include "estacao.h"
#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Variáveis globais
QuadTree *stationTree = NULL;
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
int load_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo");
        return -1;
    }

    char line[MAX_LENGTH];
    Retangulo boundary = {{0, 0}, {100000000, 100000000}}; // Limites iniciais da QuadTree (ajuste conforme necessário)
    stationTree = criarQuadTree(boundary);
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
        station->active = 0;

        Ponto p = {.x = station->x, .y = station->y, .station_info = station};
        if (insere(stationTree, p)) {
            i++;
        }
    }

    fclose(file);
    num_stations = i;
    return i;
}

// Função para ativar uma estação
void activate_station(const char *idend) {
    int count = 0;
    Retangulo boundary = {{0, 0}, {100000000, 100000000}};
    Ponto *pontos = buscaIntervalo(stationTree, boundary, &count);

    for (int i = 0; i < count; i++) {
        if (strcmp(idend, pontos[i].station_info->idend) == 0) {
            pontos[i].station_info->active = 1;
            break;
        }
    }

    free(pontos);
}

// Função para desativar uma estação
void deactivate_station(const char *idend) {
    int count = 0;
    Retangulo boundary = {{0, 0}, {100000000, 100000000}};
    Ponto *pontos = buscaIntervalo(stationTree, boundary, &count);

    for (int i = 0; i < count; i++) {
        if (strcmp(idend, pontos[i].station_info->idend) == 0) {
            pontos[i].station_info->active = 0;
            break;
        }
    }

    free(pontos);
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
void find_nearest_stations(double x, double y, int n) {
    if (n <= 0) {
        printf("Número de estações a procurar deve ser maior que 0.\n");
        return;
    }

    Ponto p = {.x = x, .y = y, .station_info = NULL};
    Ponto *pontos = (Ponto*)malloc(n * sizeof(Ponto));
    int count = 0;

    buscaKNN(stationTree, p, n, pontos, &count);

    for (int i = 0; i < count; i++) {
        printrecharge(pontos[i].station_info);
        printf(" (%f)\n", (sqrt(pow(x - pontos[i].x, 2) + pow(y - pontos[i].y, 2) * 1.0)));
    }

    free(pontos);
}

// Função para processar os comandos do arquivo
void process_commands(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo de comandos");
        return;
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char command;
        sscanf(line, "%c", &command);

        if (command == 'C') {
            double x, y;
            int n;
            sscanf(line + 2, "%lf %lf %d", &x, &y, &n);
            find_nearest_stations(x, y, n);
        } else if (command == 'A') {
            char idend[50];
            sscanf(line + 2, "%s", idend);
            activate_station(idend);
        } else if (command == 'D') {
            char idend[50];
            sscanf(line + 2, "%s", idend);
            deactivate_station(idend);
        }
    }

    fclose(file);
}
