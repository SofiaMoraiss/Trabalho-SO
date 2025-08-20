#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"

// Inicializa um PCB
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

    p->state = NEW;
    pthread_mutex_init(&p->mutex, NULL);
    pthread_cond_init(&p->cv, NULL);

    // aloca espaço para as threads
    p->thread_ids = malloc(sizeof(pthread_t) * qtd_threads);
    if (!p->thread_ids) {
        perror("Erro ao alocar threads do PCB");
        free(p);
        return NULL;
    }

    return p;
}

// Libera memória do PCB
void destroyPCB(PCB* pcb) {
    if (!pcb) return;
    free(pcb->thread_ids);
    pthread_mutex_destroy(&pcb->mutex);
    pthread_cond_destroy(&pcb->cv);
    free(pcb);
}

// Getters e Setters
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
