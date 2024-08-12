#include "estacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

station_state_t stations[MAX_RECHARGE_STATIONS];
int num_stations = 0;

void replace_comma_with_dot(char *str) {
    while (*str) {
        if (*str == ',') {
            *str = '.';
        }
        str++;
    }
}

int load_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo");
        return -1;
    }

    char line[MAX_LENGTH];
    int i = 0;

    while (fgets(line, sizeof(line), file) && i < MAX_RECHARGE_STATIONS) {
        replace_comma_with_dot(line);

        sscanf(line, "%49[^;];%ld;%9[^;];%99[^;];%d;%49[^;];%49[^;];%d;%lf;%lf",
               stations[i].station.idend,
               &stations[i].station.id_logrado,
               stations[i].station.sigla_tipo,
               stations[i].station.nome_logra,
               &stations[i].station.numero_imo,
               stations[i].station.nome_bairr,
               stations[i].station.nome_regio,
               &stations[i].station.cep,
               &stations[i].station.x,
               &stations[i].station.y);
        stations[i].active = 0;
        i++;
    }

    fclose(file);
    num_stations = i;
    return i;
}

void activate_station(const char *idend) {
    for (int i = 0; i < num_stations; i++) {
        if (strcmp(stations[i].station.idend, idend) == 0) {
            if (stations[i].active) {
                printf("Ponto de recarga %s já estava ativo.\n", idend);
            } else {
                stations[i].active = 1;
                printf("Ponto de recarga %s ativado.\n", idend);
            }
            return;
        }
    }
    printf("Ponto de recarga %s não encontrado.\n", idend);
}

void deactivate_station(const char *idend) {
    for (int i = 0; i < num_stations; i++) {
        if (strcmp(stations[i].station.idend, idend) == 0) {
            if (!stations[i].active) {
                printf("Ponto de recarga %s já estava desativado.\n", idend);
            } else {
                stations[i].active = 0;
                printf("Ponto de recarga %s desativado.\n", idend);
            }
            return;
        }
    }
    printf("Ponto de recarga %s não encontrado.\n", idend);
}

int cmpknn(const void* a, const void* b) {
    ptr_knn_t k1 = (ptr_knn_t) a;
    ptr_knn_t k2 = (ptr_knn_t) b;
    if (k1->dist > k2->dist) return 1;
    else if (k1->dist < k2->dist) return -1;
    else return 0;
}

double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

void printrecharge(addr_t *rechargevet) {
    printf("%s %s, %d, %s, %s, %d", 
           rechargevet->sigla_tipo,
           rechargevet->nome_logra, 
           rechargevet->numero_imo,
           rechargevet->nome_bairr, 
           rechargevet->nome_regio,
           rechargevet->cep);
}

void find_nearest_stations(double x, double y, int n) {
    if (n <= 0) {
        printf("Número de estações a procurar deve ser maior que 0.\n");
        return;
    }

    ptr_knn_t kvet = (ptr_knn_t) malloc(num_stations * sizeof(knn_t));
    int count = 0;

    for (int i = 0; i < num_stations; i++) {
        if (stations[i].active) {
            kvet[count].id = i;
            kvet[count].dist = calculate_distance(x, y, stations[i].station.x, stations[i].station.y);
            count++;
        }
    }

    if (count == 0) {
        printf("Nenhuma estação ativa encontrada.\n");
        free(kvet);
        return;
    }

    qsort(kvet, count, sizeof(knn_t), cmpknn);

    int kmax = (n < count) ? n : count;
    for (int i = 0; i < kmax; i++) {
        printrecharge(&stations[kvet[i].id].station);
        printf(" (%f)\n", kvet[i].dist);
    }

    free(kvet);
}

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
