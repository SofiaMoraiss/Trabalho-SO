#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include <unistd.h> // para usleep()

// Esta função agora é responsável apenas por simular uma fatia de trabalho.
// Ela não acessa o PCB ou atualiza o tempo restante, evitando erros de memória.
static void* threadWork(void* arg) {
    (void)arg; // Declara que o parâmetro 'arg' é intencionalmente não utilizado.
    sleep(0.5); 
    return NULL;
}

// A função running é responsável por simular a execução e, em seguida,
// atualizar o tempo restante e o estado do PCB.
PCB* running(PCB* pcb, int tempo) {
    if (!pcb) return NULL;
    
    //printf("Escalonador: processo PID %d executando por %d ms\n", pcb->pid, tempo);
    
    run_threads(pcb);

    pcb->remaining_time -= tempo;
    
    if (pcb->remaining_time <= 0) {
        pcb->remaining_time = 0;
        pcb->state = TERMINATED;
        printf("[PRIORITY] Processo PID %d finalizado\n", pcb->pid);
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