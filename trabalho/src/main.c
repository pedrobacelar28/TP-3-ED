#include "estacao.h"

int main() {
    load_csv("data/enderecobh.csv");
    process_commands("data/comandos.txt");
    return 0;
}
