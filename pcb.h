#ifndef PCB_H
#define PCB_H

#include <pthread.h>

// Estados possíveis do processo
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// Estrutura do Bloco de Controle de Processo (PCB)
typedef struct {
    int pid;                 // Identificador único
    int process_len;         // Tempo total de execução
    int remaining_time;      // Tempo restante
    int priority;            // Prioridade
    int num_threads;         // Número de threads do processo
    int start_time;          // Tempo de chegada

    ProcessState state;      // Estado atual do processo
    pthread_mutex_t mutex;   // Mutex para sincronização
    pthread_cond_t cv;       // Variável de condição para acordar threads
    pthread_t *thread_ids;   // Array de threads
} PCB;

// Funções utilitárias
PCB* initPCB(int pid, int dur, int prioridade, int qtd_threads, int t_chegada);
void destroyPCB(PCB* pcb);

int get_remaining_time(PCB* pcb);
void set_remaining_time(PCB* pcb, int t);
int get_priority(PCB* pcb);
void set_priority(PCB* pcb, int p);
int get_start_time(PCB* pcb);
int get_pid(PCB* pcb);

#endif
