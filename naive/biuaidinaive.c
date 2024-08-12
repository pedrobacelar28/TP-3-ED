#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINES 500 // Máximo de linhas que podem ser lidas do CSV
#define MAX_LENGTH 1024 // Máximo de caracteres em uma linha

// struct that contains the information about the recharging locations
typedef struct{
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

// struct that contain the distance information between the origin location and
// each recharging location.
typedef struct knn{
  double dist; // distance between origin location and recharging location
  int id;      // recharging location id
} knn_t, *ptr_knn_t;

// vector containing the information about the recharging locations
int load_csv(const char *filename, addr_t *rechargevet) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo");
        return -1;
    }

    char line[MAX_LENGTH];
    int i = 0;

    while (fgets(line, sizeof(line), file) && i < MAX_LINES) {
        sscanf(line, "%49[^;];%ld;%9[^;];%99[^;];%d;%49[^;];%49[^;];%d;%lf;%lf",
               rechargevet[i].idend,
               &rechargevet[i].id_logrado,
               rechargevet[i].sigla_tipo,
               rechargevet[i].nome_logra,
               &rechargevet[i].numero_imo,
               rechargevet[i].nome_bairr,
               rechargevet[i].nome_regio,
               &rechargevet[i].cep,
               &rechargevet[i].x,
               &rechargevet[i].y);
        i++;
    }

    fclose(file);
    return i; // Retorna o número de linhas carregadas
}

// qsort comparison function between distances to recharging locations
int cmpknn(const void* a, const void * b){
  ptr_knn_t k1 = (ptr_knn_t) a;
  ptr_knn_t k2 = (ptr_knn_t) b;
  if (k1->dist > k2->dist) return 1;
  else if (k1->dist < k2->dist) return -1;
  else return 0;
}

// calculates Euclidean distance between (x1,y1) and (x2,y2)
double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// print the recharge location information
void printrecharge(addr_t *rechargevet, int pos){
  printf("%s %s, %d, %s, %s, %d\n",rechargevet[pos].sigla_tipo,
         rechargevet[pos].nome_logra, rechargevet[pos].numero_imo,
         rechargevet[pos].nome_bairr, rechargevet[pos].nome_regio,
         rechargevet[pos].cep);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s x y\n", argv[0]);
        return 1;
    }

    double x = atof(argv[1]);
    double y = atof(argv[2]);

    addr_t rechargevet[MAX_LINES];
    int num_locations = load_csv("enderecobh.csv", rechargevet);

    if (num_locations == -1) {
        return 1;
    }

    // Encontra as 10 localizações mais próximas
    knn_t nearest[10];
    for (int i = 0; i < 10; i++) {
        nearest[i].dist = calculate_distance(x, y, rechargevet[i].x, rechargevet[i].y);
        nearest[i].id = i;
    }

    for (int i = 10; i < num_locations; i++) {
        double dist = calculate_distance(x, y, rechargevet[i].x, rechargevet[i].y);
        for (int j = 0; j < 10; j++) {
            if (dist < nearest[j].dist) {
                nearest[j].dist = dist;
                nearest[j].id = i;
                break;
            }
        }
    }

    printf("10 localizações mais próximas:\n");
    for (int i = 0; i < 10; i++) {
        int id = nearest[i].id;
        printrecharge(rechargevet, id);
        printf("Distância: %.2f\n", nearest[i].dist);
    }

    return 0;
}
