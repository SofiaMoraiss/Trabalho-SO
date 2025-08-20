#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "processo.h"
#include "fila.h"

int compare(const void *a, const void *b)
{
    Processo *p1 = *(Processo **)a; // note o * extra
    Processo *p2 = *(Processo **)b;
    return get_t_chegada(p1) - get_t_chegada(p2);
}

int comparePriority(void *a, void *b)
{
    Processo *p1 = (Processo *)a;
    Processo *p2 = (Processo *)b;
    return get_prioridade(p1) - get_prioridade(p2);
    // se prioridade menor = mais importante
}

typedef enum
{
    FCFS = 1,
    RR = 2,
    PRIORITY = 3
} Escalonamento;

int main()
{

    int quantum = 500;
    FILE *file;
    int num, esc;

    file = fopen("entradas/3.txt", "r"); // abre o arquivo de entrada
    if (file == NULL)
    {
        fprintf(stderr, "Não foi possível abrir o arquivo.\n");
        return 1;
    }

    fscanf(file, "%d%*c", &num); // lê o número de processos

    Processo *processos[num];

    for (int i = 0; i < num; i++)
    {
        int PID = i + 1, dur, prioridade, qtd_threads, t_chegada;
        fscanf(file, "%d%*c%d%*c%d%*c%d", &dur, &prioridade, &qtd_threads, &t_chegada);

        Processo *p = initProcesso(PID, dur, prioridade, qtd_threads, t_chegada);
        if (p == NULL)
        {
            fclose(file);
            return 1;
        }
        processos[i] = p; // armazena em vetor de ponteiros para processos

        // Example usage of getters
    }
    // for (int i = 0; i < num; i++) {
    //     Processo *p = processos[i];
    // printf("Process ID: %d\n", get_PID(p));
    // printf("Duration: %d\n", get_dur(p));
    // printf("Priority: %d\n", get_prioridade(p));
    // printf("Number of Threads: %d\n", get_qtd_threads(p));
    // printf("Arrival Time: %d\n", get_t_chegada(p));
    // }
    qsort(processos, num, sizeof(Processo *), compare); // ordena os processos pelo tempo de chegada
    fscanf(file, "%d", &esc);                           // lê o tipo de escalonamento

    Escalonamento tipo_escalonamento = (Escalonamento)esc;

    if (esc < 1 || esc > 3)
    {
        fprintf(stderr, "Método Inválido.\n");
        fclose(file);
        return 1;
    }

    switch (tipo_escalonamento)
    {
    case FCFS: // First-Come, First-Served: executa os processos de acordo com a ordem de chegada
        // monothreaded
        for (int y = 0; y < num; y++)
        {
            Processo *p = processos[y];
            printf("[FCFS] Executando processo PID %d\n", get_PID(p));
            printf("[FCFS] Processo PID %d finalizado\n", get_PID(p));
            free(p);
        }
        break;

    case RR:
        // monothreaded
        int tempo_passado = 0, y = 0, nulos = 0;
        while (1)
        {

            Processo *p = processos[y];
            if (p != NULL)
            {
                if (tempo_passado >= get_t_chegada(p))
                { // verifica se o processo chegou

                    printf("[RR] Executando processo PID %d com quantum 500ms\n", get_PID(p));
                    if (get_dur(p) - quantum <= 0)
                    { // processo termina
                        printf("[RR] Processo PID %d finalizado\n", get_PID(p));
                        tempo_passado += get_dur(p);
                        nulos++;
                        free(processos[y]);
                        processos[y] = NULL;
                    }
                    else
                    {
                        set_dur(p, get_dur(p) - quantum);
                        tempo_passado += quantum;
                    }
                }
                else
                { // se o processo ainda não chegou, o tempo corre;
                    tempo_passado++;
                }
            }
            if (nulos == num)
            {
                break;
            }
            y++;
            if (y == num)
            {
                y = 0;
            }
        }
        break;
    case PRIORITY:
    {
        // printf("[PRIORITY] Escalonamento por prioridade preemptiva\n");

        Queue *prontos = createQueue();
        int tempo_passado = 0;
        int processos_finalizados = 0;
        int idx = 0; // índice para novos processos chegando

        while (processos_finalizados < num)
        {
            // Insere novos processos que chegaram
            while (idx < num && get_t_chegada(processos[idx]) <= tempo_passado)
            {
                enqueue(prontos, processos[idx]);
                idx++;
            }

            // Se a fila está vazia, o tempo anda
            if (isEmpty(prontos))
            {
                tempo_passado++;
                continue;
            }

            // Ordena por prioridade (menor valor = maior prioridade)
            sortQueue(prontos, comparePriority);

            // Pega o próximo processo
            Processo *atual = (Processo *)dequeue(prontos);

            if (atual->running == 0)
            {
                printf("[PRIORITY] Executando processo PID %d com prioridade %d\n",
                       get_PID(atual), get_prioridade(atual));
                atual->running = 1; // Marca que o processo foi iniciado
            }

            // Executa por um quantum ou até terminar
            int restante = get_dur(atual);
            if (restante <= quantum)
            {
                printf("[PRIORITY] Processo PID %d finalizado\n", get_PID(atual));
                tempo_passado += restante;
                processos_finalizados++;
                free(atual);
            }
            else
            {
                set_dur(atual, restante - quantum);
                tempo_passado += quantum;

                // Inserir novos processos que chegaram durante esse quantum
                while (idx < num && get_t_chegada(processos[idx]) <= tempo_passado)
                {
                    enqueue(prontos, processos[idx]);
                    idx++;
                }

                // Ordena a fila
                sortQueue(prontos, comparePriority);

                // Verifica se existe alguém mais prioritário
                if (!isEmpty(prontos))
                {
                    Processo *proximo = (Processo *)prontos->front->data;

                    if (get_prioridade(proximo) < get_prioridade(atual))
                    {
                        // Tem alguém com prioridade maior → preempção
                        atual->running = 0;
                        enqueue(prontos, atual);
                    }
                    else
                    {
                        // Ele ainda é o mais prioritário → continua rodando
                        enqueue(prontos, atual); // MAS pode deixar rodando direto sem "re-executar"
                    }
                }
                else
                {
                    // Nenhum outro processo, ele continua rodando
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

    // Return 0 to indicate successful execution
    return 0;
}