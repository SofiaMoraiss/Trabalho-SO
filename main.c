#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "processo.h"
#include "fila.h"

int compare(const void* a, const void* b) {
    Processo *p1 = *(Processo **)a; // note o * extra
    Processo *p2 = *(Processo **)b;
    return get_t_chegada(p1) - get_t_chegada(p2);
}

typedef enum {
    FCFS = 1,
    RR = 2,
    PRIORITY = 3
} Escalonamento;

int main() {

    int quantum = 500; 
    FILE *file;
    int num, esc;

    file = fopen("entradas/2.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo.\n");
        return 1;
    }

    fscanf(file, "%d%*c", &num);
    
    Processo *processos[num];

    for (int i = 0; i < num; i++) {
        int PID=i+1, dur, prioridade, qtd_threads, t_chegada;
        fscanf(file, "%d%*c%d%*c%d%*c%d", &dur, &prioridade, &qtd_threads, &t_chegada);
        
        Processo *p = initProcesso(PID, dur, prioridade, qtd_threads, t_chegada);
        if (p == NULL) {
            fclose(file);
            return 1;
        }
        processos[i] = p;

        // Example usage of getters
    }
    for (int i = 0; i < num; i++) {
        Processo *p = processos[i];
        // printf("Process ID: %d\n", get_PID(p));
        // printf("Duration: %d\n", get_dur(p));
        // printf("Priority: %d\n", get_prioridade(p));
        // printf("Number of Threads: %d\n", get_qtd_threads(p));
        // printf("Arrival Time: %d\n", get_t_chegada(p));
    }
    qsort(processos, num, sizeof(Processo *), compare);
    fscanf(file, "%d", &esc);
    
    Escalonamento tipo_escalonamento = (Escalonamento)esc;

    if (esc < 1 || esc > 3) {
        fprintf(stderr, "Método Inválido.\n");
        fclose(file);
        return 1;
    }

    switch (tipo_escalonamento) {
        case FCFS:
            //monothreaded
            for (int y =0; y<num; y++) {
                Processo * p = processos[y];
                printf("[FCFS] Executando processo PID %d\n", get_PID(p));
                printf("[FCFS] Processo PID %d finalizado\n", get_PID(p));
                free(p);
            }
            break;
            
        case RR:
            //monothreaded
            int tempo_passado=0, y=0, nulos=0;
            while (1) {
            
                Processo * p = processos[y];
                if (p != NULL){
                    if (tempo_passado >= get_t_chegada(p)){
                        
                        printf("[RR] Executando processo PID %d com quantum 500ms\n", get_PID(p));
                        if (get_dur(p)- quantum<=0) {
                            printf("[RR] Processo PID %d finalizado\n", get_PID(p));
                            tempo_passado += get_dur(p);
                            nulos++;
                            free(processos[y]);
                            processos[y] = NULL;  
                        } else {
                            set_dur(p, get_dur(p) - quantum);
                            tempo_passado+=quantum;
                        }
                    }
                    else {
                        tempo_passado++;
                    }
                    
                }
                if (nulos == num) {
                    break;
                }
                y++;
                if (y==num){
                    y=0;
                }
            }
            break;
        case PRIORITY:
            printf("Selected scheduling: Priority Scheduling\n");
            break;
        default:
            printf("Invalid scheduling type selected.\n");
            break;
    }


    fclose(file);

    printf("Escalonador terminou execução de todos processos\n");

    // Return 0 to indicate successful execution
    return 0;
}