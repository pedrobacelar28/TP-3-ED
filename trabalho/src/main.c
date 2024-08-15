#include "estacao.h"

int main() {
    QuadTree *stationTree = NULL;

    // Passa o endereço do ponteiro stationTree para permitir modificação
    load_csv("data/geracarga.base", &stationTree);

    FILE *file = fopen("data/geracarga.ev", "r");
    if (!file) {
        perror("Não foi possível abrir o arquivo de comandos");
        return 0;
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
