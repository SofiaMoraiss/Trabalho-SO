#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include <unistd.h>   // para usleep()

// Inicializa um PCB

PCB* thread_running(PCB* pcb, int tempo) {
    const int quantum = 0.5;

    pthread_mutex_lock(&pcb->mutex);

    if (pcb->state == TERMINATED) {
        pthread_mutex_unlock(&pcb->mutex);
        return pcb;
    }

    pcb->state = RUNNING;
    pthread_mutex_unlock(&pcb->mutex);

    // Simula execução por 500 ms (mesmo se precisar de menos CPU)
    sleep(quantum);

    pthread_mutex_lock(&pcb->mutex);

    // Desconta o "tempo lógico" da thread (thread_time ou tempo recebido)
    pcb->remaining_time -= tempo;
    if (pcb->remaining_time <= 0) {
        pcb->remaining_time = 0;
        pcb->state = TERMINATED;
        pthread_cond_broadcast(&pcb->cv); // acorda todas para encerrar
    } else {
        pcb->state = READY;
    }

    pthread_mutex_unlock(&pcb->mutex);
    return pcb;
}


PCB* running(PCB* pcb, int tempo) {
    // Enquanto houver tempo restante no processo
    while (1) {
        pthread_mutex_lock(&pcb->mutex);
        if (pcb->state == TERMINATED) {
            pthread_mutex_unlock(&pcb->mutex);
            break;
        }
        pthread_mutex_unlock(&pcb->mutex);

        // Escalona todas as threads do processo
        for (int i = 0; i < pcb->num_threads; i++) {
            thread_running(pcb, tempo);
        }
    }

    return pcb;
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

int get_num_threads(PCB* pcb) {
    return pcb->num_threads;
}

int get_process_len(PCB* pcb) {
    return pcb->process_len;
}
 
