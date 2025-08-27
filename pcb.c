#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "fila.h"
#include <unistd.h> // para usleep()

#define NUM_PROCESSADORES 2 // já definido no seu código

typedef enum
{
    FCFS = 1,
    RR = 2,
    PRIORITY = 3
} Escalonamento;

int comparePriority(void *a, void *b)
{
    PCB *p1 = (PCB *)a;
    PCB *p2 = (PCB *)b;
    return get_priority(p1) - get_priority(p2);
}

static void* threadWork(void* arg) {
    (void)arg; // Declara que o parâmetro 'arg' é intencionalmente não utilizado.
    sleep(0.5); 
    return NULL;
}

void fcfs_mono(PCB* processos[], int num) {
    for (int y = 0; y < num; y++)
    {
        PCB *p = processos[y];
        printf("[FCFS] Executando processo PID %d\n", get_pid(p));
        running(p, get_remaining_time(p), FCFS);
    }
}

void rr_mono(PCB* processos[], int num, int quantum) {
    int tempo_passado = 0, y = 0, nulos = 0;
        while (1)
        {
            PCB *p = processos[y];
            if (p != NULL)
            {
                if (tempo_passado >= get_start_time(p))
                {
                    printf("[RR] Executando processo PID %d com quantum 500ms\n", get_pid(p));
                    running(p, quantum, RR);
                    if (p->state == TERMINATED)
                    {
                        tempo_passado += quantum;
                        nulos++;
                        processos[y] = NULL; // arrumei aqui: antes você fazia `p = NULL`, mas isso não apagava do vetor
                    }
                    else
                    {
                        tempo_passado += quantum;
                    }
                }
                else
                {
                    tempo_passado += quantum;
                }
            }
            else
            {
                tempo_passado++;
            }

            if (nulos == num)
                break;

            y++;
            if (y == num)
                y = 0;
        }
}

void priority_mono(PCB* processos[], int num, int quantum) {
     Queue *prontos = createQueue();
        int tempo_passado = 0;
        int processos_finalizados = 0;
        int idx = 0;
        PCB *atual = NULL;

        while (processos_finalizados < num)
        {
            while (idx < num && get_start_time(processos[idx]) <= tempo_passado)
            {
                enqueue(prontos, processos[idx]);
                idx++;
            }

            if (atual == NULL)
            {
                if (isEmpty(prontos))
                {
                    tempo_passado++;
                    continue;
                }
                sortQueue(prontos, comparePriority);
                atual = (PCB *)dequeue(prontos);
                printf("[PRIORITY] Executando processo PID %d com prioridade %d\n",
                       get_pid(atual), get_priority(atual));
            }
            else
            {
                if (!isEmpty(prontos))
                {
                    sortQueue(prontos, comparePriority);
                    PCB *proximo = (PCB *)prontos->front->data;

                    if (get_priority(proximo) < get_priority(atual))
                    {
                        enqueue(prontos, atual);
                        atual = (PCB *)dequeue(prontos);
                        printf("[PRIORITY] Executando processo PID %d com prioridade %d\n",
                               get_pid(atual), get_priority(atual));
                    }
                }
            }

            running(atual, quantum, PRIORITY);
            tempo_passado += quantum;

            if (atual->state == TERMINATED)
            {
                processos_finalizados++;
                atual = NULL;
            }
        }
        destroyQueue(prontos);
}


PCB* running(PCB* pcb, int tempo, int tipo) {
    if (!pcb) return NULL;
    
    //printf("Escalonador: processo PID %d executando por %d ms\n", pcb->pid, tempo);
    
    run_threads(pcb);

    pcb->remaining_time -= tempo;
    
    if (pcb->remaining_time <= 0) {
        pcb->remaining_time = 0;
        pcb->state = TERMINATED;
        if (tipo == 3){
            printf("[PRIORITY] Processo PID %d finalizado\n", pcb->pid);
        }
        else if (tipo == 2) {
            printf("[RR] Processo PID %d finalizado\n", pcb->pid);
        }
        else {
            printf("[FCFS] Processo PID %d finalizado\n", pcb->pid);
        }
    }
    
    return pcb;
}


// Cria e espera por todas as threads de um processo.
void run_threads(PCB* pcb) {
    for (int i = 0; i < pcb->num_threads; i++) {
        pthread_create(&pcb->thread_ids[i], NULL, threadWork, NULL);
    }

    for (int i = 0; i < pcb->num_threads; i++) {
        pthread_join(pcb->thread_ids[i], NULL);
    }
}


void fcfs_multi(PCB* processos[], int num, int quantum) {
    // Array para simular os processadores
    PCB *processadores[NUM_PROCESSADORES] = {NULL};
    int processos_finalizados = 0;
    int idx_processos = 0;
    int tempo_passado = 0;

    printf("Iniciando FCFS com %d processadores.\n", NUM_PROCESSADORES);

    while (processos_finalizados < num) {
        // 1. Tenta alocar processos nas CPUs livres
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] == NULL) {
                // Encontra o próximo processo pronto na fila ordenada por tempo de chegada
                PCB *p = NULL;
                for (int j = 0; j < num; j++) {
                    if (processos[j]->state != TERMINATED && get_start_time(processos[j]) <= tempo_passado) {
                        p = processos[j];
                        break; 
                    }
                }

                if (p != NULL) {
                    processadores[i] = p;
                    // Lógica para alocar o mesmo processo a várias CPUs, se houver threads suficientes
                    for (int j = i + 1; j < NUM_PROCESSADORES; j++) {
                        if (p->num_threads > (j - i) && processadores[j] == NULL) {
                            processadores[j] = p;
                        } else {
                            break;
                        }
                    }

                    printf("[FCFS] Executando processo PID %d // processador %d\n", get_pid(p), i);
                }
            }
        }
        
        // 2. Executa os processos em cada processador
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] != NULL) {
                running(processadores[i], quantum, FCFS);
                
                if (processadores[i]->state == TERMINATED) {
                    // Libera todas as CPUs ocupadas pelo mesmo processo
                    PCB *terminado = processadores[i];
                    for (int j = 0; j < NUM_PROCESSADORES; j++) {
                        if (processadores[j] == terminado) {
                            processadores[j] = NULL;
                        }
                    }
                    printf("[FCFS] Processo PID %d finalizado\n", get_pid(terminado));
                    processos_finalizados++;
                }
            }
        }

        // Avança o tempo
        tempo_passado += quantum;
    }
    
    printf("Escalonador terminou execução de todos processos\n");
}
void rr_multiprocessador(PCB* processos[], int num, int quantum) {
    Queue *prontos = createQueue();
    PCB *processadores[NUM_PROCESSADORES] = {NULL};
    int processos_finalizados = 0;
    int idx_chegada = 0;
    int tempo_passado = 0;
    
    printf("Iniciando Round Robin com %d processadores.\n", NUM_PROCESSADORES);

    while (processos_finalizados < num) {
        // Adiciona processos que chegaram na fila de prontos
        while (idx_chegada < num && get_start_time(processos[idx_chegada]) <= tempo_passado) {
            enqueue(prontos, processos[idx_chegada]);
            idx_chegada++;
        }

        // Aloca processos da fila de prontos para CPUs livres
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] == NULL && !isEmpty(prontos)) {
                processadores[i] = (PCB *)dequeue(prontos);
                printf("[RR] Executando processo PID %d com quantum %dms // processador %d\n", 
                       get_pid(processadores[i]), quantum, i);
            }
        }
        
        // Executa um quantum em cada processador
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] != NULL) {
                // Salva o processo atual antes de chamar running
                PCB* processo_atual = processadores[i];
                running(processo_atual, quantum, RR);

                if (processo_atual->state == TERMINATED) {
                    // Impressão da mensagem de finalização
                    processos_finalizados++;
                    
                    // Libera todas as CPUs ocupadas pelo mesmo processo
                    for (int j = 0; j < NUM_PROCESSADORES; j++) {
                        if (processadores[j] != NULL && get_pid(processadores[j]) == get_pid(processo_atual)) {
                            processadores[j] = NULL;
                        }
                    }
                } else {
                    // Preempção: se não terminou, retorna para a fila de prontos
                    enqueue(prontos, processo_atual);
                    processadores[i] = NULL; // Libera o processador
                }
            }
        }
        
        // Avança o tempo
        tempo_passado += quantum;
    }
    
    destroyQueue(prontos);
}

void priority_multi(PCB* processos[], int num, int quantum) {
    Queue *prontos = createQueue();
    PCB *processadores[NUM_PROCESSADORES];
    for (int i = 0; i < NUM_PROCESSADORES; i++) {
        processadores[i] = NULL;
    }

    int tempo_passado = 0;
    int processos_finalizados = 0;
    int idx_processos = 0;

    while (processos_finalizados < num) {
        // 1. Adiciona processos que chegaram até o tempo atual
        while (idx_processos < num && get_start_time(processos[idx_processos]) <= tempo_passado) {
            enqueue(prontos, processos[idx_processos]);
            idx_processos++;
        }

        // 2. Preempção
        if (!isEmpty(prontos)) {
            sortQueue(prontos, comparePriority);
            PCB *proximo_na_fila = (PCB *)prontos->front->data;

            PCB *processo_a_preemptar = NULL;
            int processador_a_preemptar_idx = -1;

            for (int i = 0; i < NUM_PROCESSADORES; i++) {
                if (processadores[i] != NULL) {
                    if (processo_a_preemptar == NULL || get_priority(processadores[i]) > get_priority(processo_a_preemptar)) {
                        processo_a_preemptar = processadores[i];
                        processador_a_preemptar_idx = i;
                    }
                }
            }

            if (processo_a_preemptar != NULL && get_priority(proximo_na_fila) < get_priority(processo_a_preemptar)) {
                enqueue(prontos, processo_a_preemptar); 
                processadores[processador_a_preemptar_idx] = NULL; 
            }
        }

        // 3. Aloca processos em processadores livres considerando threads
        sortQueue(prontos, comparePriority);

        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] == NULL && !isEmpty(prontos)) {
                PCB *atual = (PCB *)dequeue(prontos);
                int threads = get_num_threads(atual);

                // O processo pode ocupar até "threads" CPUs, mas não mais do que CPUs livres
                int cpus_livres = 0;
                for (int j = 0; j < NUM_PROCESSADORES; j++) {
                    if (processadores[j] == NULL) cpus_livres++;
                }

                int cpus_para_ocupar = (threads < cpus_livres) ? threads : cpus_livres;

                for (int j = 0; j < NUM_PROCESSADORES && cpus_para_ocupar > 0; j++) {
                    if (processadores[j] == NULL) {
                        processadores[j] = atual; // mesma PCB em várias CPUs
                        printf("[PRIORITY] Executando processo PID %d // processador %d\n", get_pid(atual), j);
                        cpus_para_ocupar--;
                    }
                }
            }
        }

        // 4. Executa um quantum em cada processador
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] != NULL) {
                running(processadores[i], quantum, PRIORITY);

                if (processadores[i]->state == TERMINATED) {
                    // liberar todos os processadores ocupados pelo mesmo processo
                    for (int j = 0; j < NUM_PROCESSADORES; j++) {
                        if (processadores[j] == processadores[i]) {
                            processadores[j] = NULL;
                        }
                    }
                    processos_finalizados++;
                }
            }
        }

        tempo_passado += quantum;
    }

    destroyQueue(prontos);
}


PCB* initPCB(int pid, int dur, int prioridade, int qtd_threads, int t_chegada) {
    PCB *p = malloc(sizeof(PCB));
    if (!p) {
        perror("Erro ao alocar PCB");
        return NULL;
    }

    p->pid = pid;
    p->process_len = dur;
    p->remaining_time = dur;
    p->priority = prioridade;
    p->num_threads = qtd_threads;
    p->start_time = t_chegada;
    p->thread_time = dur / qtd_threads; 

    p->state = NEW;
    pthread_mutex_init(&p->mutex, NULL);
    pthread_cond_init(&p->cv, NULL);

    p->thread_ids = malloc(sizeof(pthread_t) * qtd_threads);
    if (!p->thread_ids) {
        perror("Erro ao alocar threads do PCB");
        free(p);
        return NULL;
    }

    return p;
}

void destroyPCB(PCB* pcb) {
    if (!pcb) return;
    free(pcb->thread_ids);
    pthread_mutex_destroy(&pcb->mutex);
    pthread_cond_destroy(&pcb->cv);
    free(pcb);
}

int get_remaining_time(PCB* pcb) {
    return pcb->remaining_time;
}

void set_remaining_time(PCB* pcb, int t) {
    pcb->remaining_time = t;
}

int get_priority(PCB* pcb) {
    return pcb->priority;
}

void set_priority(PCB* pcb, int p) {
    pcb->priority = p;
}

int get_start_time(PCB* pcb) {
    return pcb->start_time;
}

int get_pid(PCB* pcb) {
    return pcb->pid;
}

int get_num_threads(PCB* pcb) {
    return pcb->num_threads;
}

int get_process_len(PCB* pcb) {
    return pcb->process_len;
}

// PCB* thread_running(PCB* pcb, int tempo) {
//     pthread_mutex_lock(&pcb->mutex);

//     if (pcb->state == TERMINATED) {
//         pthread_mutex_unlock(&pcb->mutex);
//         return pcb;
//     }

//     pcb->state = RUNNING;
//     pthread_mutex_unlock(&pcb->mutex);

//     // Executa por 500 ms (independente do tempo lógico)
//     usleep(500000);

//     pthread_mutex_lock(&pcb->mutex);

//     // Desconta apenas o "tempo lógico" da thread
//     pcb->remaining_time -= tempo;

//     if (pcb->remaining_time <= 0) {
//         pcb->remaining_time = 0;
//         pcb->state = TERMINATED;
//         pthread_cond_broadcast(&pcb->cv);
//     } else {
//         pcb->state = READY;
//     }

//     pthread_mutex_unlock(&pcb->mutex);
//     return pcb;
// }

// PCB* running(PCB* pcb, int tempo) {
//     int tempo_restante = tempo;

//     printf("Processo PID %d iniciando execução por %d ms\n", pcb->pid, tempo);

//     while (tempo_restante > 0) {
//         pthread_mutex_lock(&pcb->mutex);
//         if (pcb->state == TERMINATED) {
//             pthread_mutex_unlock(&pcb->mutex);
//             return NULL; // encerra imediatamente se já terminou
//         }
//         pthread_mutex_unlock(&pcb->mutex);

//         // Roda uma thread, usando o tempo restante do quantum
//         thread_running(pcb, tempo_restante);

//         // Verifica logo depois da execução se terminou
//         pthread_mutex_lock(&pcb->mutex);
//         if (pcb->state == TERMINATED) {
//             pthread_mutex_unlock(&pcb->mutex);
//             return NULL; // encerra imediatamente se terminou durante a execução
//         }
//         pthread_mutex_unlock(&pcb->mutex);

//         tempo_restante -= 500; // cada execução simula 500ms de CPU
//         printf("Processo PID %d: tempo restante %d ms\n", pcb->pid, pcb->remaining_time);

//         // Se terminou de usar seu tempo, devolve ao escalonador
//         if (pcb->state == READY) break;
//     }

//     return pcb;
// }