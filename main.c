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
    PRIORITY = 3,
    CFS = 4
} Escalonamento;

int compare(const void *a, const void *b)
{
    PCB *p1 = *(PCB **)a;
    PCB *p2 = *(PCB **)b;
    return get_start_time(p1) - get_start_time(p2);
}

int main(int argc, char *argv[])
{
    int quantum = 500;
    FILE *file;
    int num, esc;
    char *modo_de_execucao;

    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s <caminho_para_o_arquivo_de_entrada> <modo_de_execucao>\n", argv[0]);
        fprintf(stderr, "Modos: mono, multi\n");
        return 1;
    }

    modo_de_execucao = argv[2];

    file = fopen(argv[1], "r");
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

    if (esc < 1 || esc > 4)
    {
        fprintf(stderr, "Método Inválido.\n");
        fclose(file);
        return 1;
    }

    if (strcmp(modo_de_execucao, "mono") == 0)
    {
        switch (tipo_escalonamento)
        {
        case FCFS:
            fcfs_mono(processos, num);
            break;
        case RR:
            rr_mono(processos, num, quantum);
            break;
        case PRIORITY:
            priority_mono(processos, num, quantum);
            break;
        case CFS:
            //CFS_mono(processos, num, quantum);
            break;
        default:
            printf("Invalid scheduling type selected.\n");
            break;
        }
    }
    else if (strcmp(modo_de_execucao, "multi") == 0)
    {
        switch (tipo_escalonamento)
        {
        case FCFS:
            fcfs_multi(processos, num);
            break;
        case RR:
            rr_multiprocessador(processos, num, quantum);
            break;
        case PRIORITY:
            priority_multi(processos, num, quantum);
            break;
        case CFS:
            //CFS_multi(processos, num, quantum);
            break;
        default:
            printf("Invalid scheduling type selected.\n");
            break;
        }
    }
    else
    {
        fprintf(stderr, "Modo de execução inválido: %s\n", modo_de_execucao);
        return 1;
    }

    printf("Escalonador terminou execução de todos processos\n");

    for (int i = 0; i < num; i++)
    {
        if (processos[i] != NULL)
        {
            destroyPCB(processos[i]);
        }
    }

    fclose(file);
    return 0;
}