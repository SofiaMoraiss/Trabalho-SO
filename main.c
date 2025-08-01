#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "processo.h"

typedef enum {
    FCFS = 1,
    RR = 2,
    PRIORITY = 3
} Escalonamento;

int main() {
    // Print a welcome message

    FILE *file;
    int num, esc;

    file = fopen("entradas/3.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }

    fscanf(file, "%d%*c", &num);
    
    for (int i = 0; i < num; i++) {
        int PID=i+1, dur, prioridade, qtd_threads, t_chegada;
        fscanf(file, "%d%*c%d%*c%d%*c%d", &dur, &prioridade, &qtd_threads, &t_chegada);
        
        Processo *p = initProcesso(PID, dur, prioridade, qtd_threads, t_chegada);
        if (p == NULL) {
            fprintf(stderr, "Failed to initialize process.\n");
            fclose(file);
            return 1;
        }

        // Example usage of getters
        printf("Process ID: %d\n", get_PID(p));
        printf("Duration: %d\n", get_dur(p));
        printf("Priority: %d\n", get_prioridade(p));
        printf("Number of Threads: %d\n", get_qtd_threads(p));
        printf("Arrival Time: %d\n", get_t_chegada(p));

        // Free the allocated memory for the process
        free(p);
    }
    fscanf(file, "%d", &esc);
    
    if (esc < 1 || esc > 3) {
        fprintf(stderr, "Método Inválido.\n");
        fclose(file);
        return 1;
    }

    Escalonamento tipo_escalonamento = (Escalonamento)esc;

    switch (tipo_escalonamento) {
        case FCFS:
            printf("Selected scheduling: First-Come, First-Served (FCFS)\n");
            break;
        case RR:
            printf("Selected scheduling: Round Robin (RR)\n");
            break;
        case PRIORITY:
            printf("Selected scheduling: Priority Scheduling\n");
            break;
        default:
            printf("Invalid scheduling type selected.\n");
            break;
    }
    fclose(file);

    printf("Welcome to the program!\n");

    // Return 0 to indicate successful execution
    return 0;
}