#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef TIPOS_COMUNS_H
#define TIPOS_COMUNS_H

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
    int active;
} addr_t;

#endif // TIPOS_COMUNS_H
