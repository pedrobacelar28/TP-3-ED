#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LENGTH 1024 // Máximo de caracteres em uma linha
#define MAX_RECHARGE_STATIONS 800000
// struct que contém as informações sobre os locais de recarga
typedef struct {
    char idend[50];
    long id_logrado;
    char sigla_tipo[10];
    char nome_logra[100];
    int numero_imo;
    char nome_bairr[50];
    char nome_regio[50];
    int cep;
    double x;
    double y;
} addr_t, *ptr_addr_t;

typedef struct {
    addr_t station;  // Dados da estação de recarga
    int active;      // 1 se ativo, 0 se inativo
} station_state_t;

station_state_t stations[MAX_RECHARGE_STATIONS];
int num_stations = 0;

// struct que contém a informação da distância entre a localização de origem e cada local de recarga
typedef struct knn {
    double dist; // distância entre a localização de origem e a localização de recarga
    int id;      // id da localização de recarga
} knn_t, *ptr_knn_t;

// Declarações de funções
void activate_station(const char *idend);
void deactivate_station(const char *idend);
void find_nearest_stations(double x, double y, int n);
void printrecharge(addr_t *rechargevet);
double calculate_distance(double x1, double y1, double x2, double y2);
int cmpknn(const void* a, const void* b);

// Função para substituir vírgulas por pontos em uma string
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
        // Substitui vírgulas por pontos na linha antes de fazer o sscanf
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
        stations[i].active = 0;  // Inicialmente, todas as estações estão inativas
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

// Função de comparação para qsort entre distâncias para as localizações de recarga
int cmpknn(const void* a, const void* b) {
    ptr_knn_t k1 = (ptr_knn_t) a;
    ptr_knn_t k2 = (ptr_knn_t) b;
    if (k1->dist > k2->dist) return 1;
    else if (k1->dist < k2->dist) return -1;
    else return 0;
}

// Calcula a distância euclidiana entre (x1, y1) e (x2, y2)
double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

// Imprime as informações da localização de recarga
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
        if (stations[i].active) { // Considerar apenas estações ativas
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

    int kmax = (n < count) ? n : count; // Seleciona o mínimo entre n e o número de estações ativas
    for (int i = 0; i < kmax; i++) {
        printrecharge(&stations[kvet[i].id].station); // Passa o ponteiro para a estação correspondente
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

int main() {

    load_csv("enderecobh.csv");
    
    process_commands("comandos.txt");

    return 0;
}
