#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "pcb.h" 
#include "fila.h"


typedef enum
{
    FCFS = 1,
    RR = 2,
    PRIORITY = 3
} Escalonamento;

int compare(const void *a, const void *b)
{
    PCB *p1 = *(PCB **)a;
    PCB *p2 = *(PCB **)b;
    return get_start_time(p1) - get_start_time(p2);
}



int main()
{
    int quantum = 500;
    FILE *file;
    int num, esc;

    file = fopen("entradas/2.txt", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Não foi possível abrir o arquivo.\n");
        return 1;
    }

    fscanf(file, "%d%*c", &num);

    PCB *processos[num];

    for (int i = 0; i < num; i++)
    {
        int PID = i + 1, dur, prioridade, qtd_threads, t_chegada;
        fscanf(file, "%d%*c%d%*c%d%*c%d", &dur, &prioridade, &qtd_threads, &t_chegada);

        PCB *p = initPCB(PID, dur, prioridade, qtd_threads, t_chegada);
        if (p == NULL)
        {
            fclose(file);
            return 1;
        }
        processos[i] = p;
    }

    qsort(processos, num, sizeof(PCB *), compare);
    fscanf(file, "%d", &esc);

    Escalonamento tipo_escalonamento = (Escalonamento)esc;

    if (esc < 1 || esc > 3)
    {
        fprintf(stderr, "Método Inválido.\n");
        fclose(file);
        return 1;
    }

    switch (tipo_escalonamento)
    {
    case FCFS:
        fcfs_mono(processos, num, quantum);
        //fcfs_multiprocessador(processos, num, quantum);
        break;

    case RR:
    {
        rr_mono(processos, num, quantum);
        //rr_multiprocessador(processos, num, quantum);
        break;
    }

    case PRIORITY:
    {
       priority_mono(processos, num, quantum);
       //priority_multi(processos, num, quantum);
        break;
    }

    default:
        printf("Invalid scheduling type selected.\n");
        break;
    }

    for (int i = 0; i < num; i++)
    {
        if (processos[i] != NULL)
        {
            destroyPCB(processos[i]);
        }
    }

    fclose(file);
    printf("Escalonador terminou execução de todos processos\n");
    return 0;
}
