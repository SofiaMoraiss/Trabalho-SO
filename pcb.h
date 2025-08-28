#ifndef PCB_H
#define PCB_H

#include <pthread.h>
#include <unistd.h>   // para usleep()


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
    int thread_time;        // Tempo de CPU por thread

    ProcessState state;      // Estado atual do processo
    pthread_mutex_t mutex;   // Mutex para sincronização
    pthread_cond_t cv;       // Variável de condição para acordar threads
    pthread_t *thread_ids;   // Array de threads
} PCB;

// Funções utilitárias
PCB* running(PCB* pcb, int tempo, int tipo);
PCB* thread_running(PCB* pcb, int tempo);
void* thread_func(void* arg);
void run_threads(PCB* pcb);
//void running_multiprocessador(PCB* cpu0, PCB* cpu1, int quantum, int tipo);
void fcfs_multi(PCB* processos[], int num); 
void fcfs_mono(PCB* processos[], int num); 
void rr_mono(PCB* processos[], int num, int quantum);
void priority_mono(PCB* processos[], int num, int quantum); 
void rr_multiprocessador(PCB* processos[], int num, int quantum); 
void priority_multi(PCB* processos[], int num, int quantum); 
PCB* initPCB(int pid, int dur, int prioridade, int qtd_threads, int t_chegada);
void destroyPCB(PCB* pcb);

int get_remaining_time(PCB* pcb);
void set_remaining_time(PCB* pcb, int t);
int get_priority(PCB* pcb);
void set_priority(PCB* pcb, int p);
int get_start_time(PCB* pcb);
int get_pid(PCB* pcb);
int get_num_threads(PCB* pcb);
int get_process_len(PCB* pcb);

#endif
