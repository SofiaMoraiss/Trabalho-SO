#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "pcb.h"   // agora é pcb.h
#include "fila.h"

int compare(const void *a, const void *b) {
    PCB *p1 = *(PCB **)a;
    PCB *p2 = *(PCB **)b;
    return get_start_time(p1) - get_start_time(p2);
}

int comparePriority(void *a, void *b) {
    PCB *p1 = (PCB *)a;
    PCB *p2 = (PCB *)b;
    return get_priority(p1) - get_priority(p2);
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

    file = fopen("entradas/1.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo.\n");
        return 1;
    }

    fscanf(file, "%d%*c", &num);

    PCB *processos[num];

    for (int i = 0; i < num; i++) {
        int PID = i + 1, dur, prioridade, qtd_threads, t_chegada;
        fscanf(file, "%d%*c%d%*c%d%*c%d", &dur, &prioridade, &qtd_threads, &t_chegada);

        PCB *p = initPCB(PID, dur, prioridade, qtd_threads, t_chegada);
        if (p == NULL) {
            fclose(file);
            return 1;
        }
        processos[i] = p;
    }

    qsort(processos, num, sizeof(PCB *), compare);
    fscanf(file, "%d", &esc);

    Escalonamento tipo_escalonamento = (Escalonamento)esc;

    if (esc < 1 || esc > 3) {
        fprintf(stderr, "Método Inválido.\n");
        fclose(file);
        return 1;
    }

    switch (tipo_escalonamento) {
    case FCFS:
        for (int y = 0; y < num; y++) {
            PCB *p = processos[y];
            printf("[FCFS] Executando processo PID %d\n", get_pid(p));
            printf("[FCFS] Processo PID %d finalizado\n", get_pid(p));
            free(p);
        }
        break;

    case RR: {
        int tempo_passado = 0, y = 0, nulos = 0;
        while (1) {
            PCB *p = processos[y];
            if (p != NULL) {
                if (tempo_passado >= get_start_time(p)) {
                    printf("[RR] Executando processo PID %d com quantum 500ms\n", get_pid(p));
                    if (get_remaining_time(p) - quantum <= 0) {
                        printf("[RR] Processo PID %d finalizado\n", get_pid(p));
                        tempo_passado += get_remaining_time(p);
                        nulos++;
                        free(processos[y]);
                        processos[y] = NULL;
                    } else {
                        set_remaining_time(p, get_remaining_time(p) - quantum);
                        tempo_passado += quantum;
                    }
                } else {
                    tempo_passado++;
                }
            }
            if (nulos == num) break;
            y++;
            if (y == num) y = 0;
        }
        break;
    }

    case PRIORITY: {
        Queue *prontos = createQueue();
        int tempo_passado = 0;
        int processos_finalizados = 0;
        int idx = 0;

        while (processos_finalizados < num) {
            while (idx < num && get_start_time(processos[idx]) <= tempo_passado) {
                enqueue(prontos, processos[idx]);
                idx++;
            }

            if (isEmpty(prontos)) {
                tempo_passado++;
                continue;
            }

            sortQueue(prontos, comparePriority);

            PCB *atual = (PCB *)dequeue(prontos);

            if (atual->state == NEW || atual->state == READY) {
                printf("[PRIORITY] Executando processo PID %d com prioridade %d\n",
                       get_pid(atual), get_priority(atual));
                atual->state = RUNNING;
            }

            int restante = get_remaining_time(atual);
            if (restante <= quantum) {
                printf("[PRIORITY] Processo PID %d finalizado\n", get_pid(atual));
                tempo_passado += restante;
                processos_finalizados++;
                free(atual);
            } else {
                set_remaining_time(atual, restante - quantum);
                tempo_passado += quantum;

                while (idx < num && get_start_time(processos[idx]) <= tempo_passado) {
                    enqueue(prontos, processos[idx]);
                    idx++;
                }
                sortQueue(prontos, comparePriority);

                if (!isEmpty(prontos)) {
                    PCB *proximo = (PCB *)prontos->front->data;
                    if (get_priority(proximo) < get_priority(atual)) {
                        atual->state = READY;
                        enqueue(prontos, atual);
                    } else {
                        enqueue(prontos, atual);
                    }
                } else {
                    enqueue(prontos, atual);
                }
            }
        }
        break;
    }

    default:
        printf("Invalid scheduling type selected.\n");
        break;
    }

    fclose(file);
    printf("Escalonador terminou execução de todos processos\n");
    return 0;
}
