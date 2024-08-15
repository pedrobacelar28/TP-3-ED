#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef ESTACAO_H
#define ESTACAO_H
#include "quadtree.h"
#include "tipos_comuns.h" // Inclua o novo arquivo de tipos comuns

#define MAX_LENGTH 1024

typedef struct {
    addr_t station;
} station_state_t;



extern QuadTree *stationTree;
extern int num_stations;

void activate_station(const char *idend,QuadTree *stationTree);
void deactivate_station(const char *idend,QuadTree *stationTree);
void find_nearest_stations(double x, double y, int n,QuadTree *stationTree);
void printrecharge(addr_t *rechargevet);
double calculate_distance(double x1, double y1, double x2, double y2);
int load_csv(const char *filename, QuadTree **stationTree);

#endif
