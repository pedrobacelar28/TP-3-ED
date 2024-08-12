#ifndef ESTACAO_H
#define ESTACAO_H

#define MAX_LENGTH 1024
#define MAX_RECHARGE_STATIONS 800000

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
    addr_t station;
    int active;
} station_state_t;

typedef struct knn {
    double dist;
    int id;
} knn_t, *ptr_knn_t;

extern station_state_t stations[MAX_RECHARGE_STATIONS];
extern int num_stations;

void activate_station(const char *idend);
void deactivate_station(const char *idend);
void find_nearest_stations(double x, double y, int n);
void printrecharge(addr_t *rechargevet);
double calculate_distance(double x1, double y1, double x2, double y2);
int load_csv(const char *filename);
void process_commands(const char *filename);

#endif
