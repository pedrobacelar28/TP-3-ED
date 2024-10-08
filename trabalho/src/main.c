#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estacao.h"

int main(int argc, char *argv[]) {
    char *baseFile = NULL;
    char *eventFile = NULL;

    // Verifica e processa os argumentos da linha de comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            baseFile = argv[i + 1];
        } else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            eventFile = argv[i + 1];
        }
    }

    // Verifica se os arquivos foram especificados
    if (baseFile == NULL || eventFile == NULL) {
        return 1;
    }

    QuadTree *stationTree = NULL;

    // Carrega os dados da base
    load_csv(baseFile, &stationTree);

    // Abre o arquivo de eventos
    FILE *file = fopen(eventFile, "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo de eventos");
        return 1;
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Ignora linhas em branco ou que não começam com um comando válido
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') {
            continue;
        }

        char command;
        sscanf(line, "%c", &command);

        // Verifica se a linha possui um comando válido
        if (command != 'C' && command != 'A' && command != 'D') {
            continue;
        }

        // Imprime o comando antes de executá-lo
        printf("%s", line);

        if (command == 'C') {
            double x, y;
            int n;
            sscanf(line + 2, "%lf %lf %d", &x, &y, &n);
            find_nearest_stations(x, y, n, stationTree);
        } else if (command == 'A') {
            char idend[50];
            sscanf(line + 2, "%s", idend);
            activate_station(idend, stationTree);
        } else if (command == 'D') {
            char idend[50];
            sscanf(line + 2, "%s", idend);
            deactivate_station(idend, stationTree);
        }
    }

    fclose(file);

    return 0;
}
