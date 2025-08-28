#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "fila.h"
#include "rbTree.h"
#include <unistd.h> 

#define NUM_PROCESSADORES 2

typedef enum
{
    FCFS = 1,
    RR = 2,
    PRIORITY = 3,
    CFS = 4
} Escalonamento;

int comparePriority(void *a, void *b)
{
    PCB *p1 = (PCB *)a;
    PCB *p2 = (PCB *)b;
    return get_priority(p1) - get_priority(p2);
}

int compareVruntime(int a, int b)
{
    if (a < b) return -1;
    if (a > b )return 1;
    return 0;
}

int comparePIDs(void *a, void *b) {
    PCB *p1 = (PCB *)a;
    PCB *p2 = (PCB *)b;
    return get_pid(p1) - get_pid(p2);
}


static void* threadWork(void* arg) {
    (void)arg; 
    sleep(0.5); 
    return NULL;
}


void CFS_mono(PCB* processos[], int num, int quantum) {
    // int tempo_passado = 0;
    // int processos_finalizados = 0;
    
    // // Altere a função de comparação para usar o PID
    // RBTree* prontos = rbtree_create(comparePIDs, destroyPCB);

    // printf("Processos que iniciam no tempo 0:\n");
    // for (int i = 0; i < num; i++) {
    //     if (get_start_time(processos[i]) == 0) {
    //         printf("Processo PID %d adicionado na fila de prontos\n", get_pid(processos[i]));
    //         rbtree_insert(prontos, get_pid(processos[i]), processos[i]);
    //         processos[i] = NULL;
    //     }
    // }

    // while (processos_finalizados < num) {
    //     // Adiciona processos que chegam no tempo atual
    //     for (int i = 0; i < num; i++) {
    //         if (processos[i] != NULL && get_start_time(processos[i]) == tempo_passado) {
    //             printf("Processo PID %d adicionado na fila de prontos\n", get_pid(processos[i]));
    //             rbtree_insert(prontos, get_pid(processos[i]), processos[i]);
    //             processos[i] = NULL;
    //         }
    //     }
        
    //     // Use a nova função para encontrar o processo com a menor vruntime
    //     PCB* p = (PCB*)rbtree_min_vruntime_value(prontos);

    //     if (p != NULL) {
    //         int qtd_prontos = rbtree_size(prontos);
    //         int tempo_exec = quantum / (qtd_prontos > 0 ? qtd_prontos : 1);
    //         if (tempo_exec < 50) {
    //             tempo_exec = 50;
    //         }
    //         printf("[CFS] Executando processo PID %d com quantum %dms\n", get_pid(p), tempo_exec);
    //         running(p, tempo_exec, CFS);

    //         tempo_passado += tempo_exec;

    //         if (p->state == TERMINATED) {
    //             processos_finalizados++;
    //             rbtree_delete(prontos, get_pid(p));
    //         } else {
    //             set_vruntime(p, get_vruntime(p) + tempo_exec);
    //             rbtree_delete(prontos, get_pid(p));
    //             rbtree_insert(prontos, get_pid(p), p);
    //         }
    //     } else {
    //         tempo_passado++;
    //     }
    // }
    // rbtree_free(prontos);
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
                        destroyPCB(p);
                        processos[y] = NULL; 
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


int running(PCB* pcb, int tempo, int tipo) {
    if (!pcb) return 0;
        
    int tempo_passado_total= run_threads(pcb);

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
    
    return tempo_passado_total;
}

int run_threads(PCB* pcb) {
    int i;
 
    int tempo_por_thread = ((get_thread_time(pcb)+499)/500) * 500; // arredonda para o próximo múltiplo de 500ms

    for (i = 0; i < pcb->num_threads; i++) {
        pthread_create(&pcb->thread_ids[i], NULL, threadWork, NULL);
    }

    for (i = 0; i < pcb->num_threads; i++) {
        pthread_join(pcb->thread_ids[i], NULL);
    }

    return tempo_por_thread*i;
}


void fcfs_multi(PCB* processos[], int num) {
    int tempo_passado = 0, y = 0, processos_finalizados = 0, menor_tempo=0;
    PCB *processadores[NUM_PROCESSADORES] = {NULL};
    Queue *prontos = createQueue();

    for (int i = 0; i < num; i++) {
        if (get_start_time(processos[i]) == 0){
            enqueue(prontos, processos[i]);
        }
    }
    while (1)
    {
        PCB *p = (PCB *)dequeue(prontos);
        // printf("-------- INICIO WHILE Processo PID %d\n", get_pid(p)); 
        if (p != NULL)
        {
            if (tempo_passado >= get_start_time(p))
            {
                if (isEmpty(prontos)){ 
                    int exec_time = get_remaining_time(p)/NUM_PROCESSADORES;
                    processadores[0] = p;
                    processadores[1]=processadores[0];
                    printf("[FCFS] Executando processo PID %d // processador %d\n", get_pid(p), 0);
                    printf("[FCFS] Executando processo PID %d // processador %d\n", get_pid(p), 1);
                    tempo_passado+=running(p, exec_time, FCFS);
                    tempo_passado+=running(p, exec_time, FCFS);
                    processos_finalizados++;
                    y++;
                    for (int j = 0; j < NUM_PROCESSADORES; j++) {
                        if (processadores[j] != NULL && get_pid(processadores[j]) == get_pid(p)) {
                            processadores[j] = NULL;
                        }
                    }

                }
                else{
                        processadores[0]=p;
                        PCB *p2 = (PCB *)dequeue(prontos);
                        processadores[1]=p2;
                        if (get_remaining_time(p) < get_remaining_time(p2)){
                            menor_tempo = get_remaining_time(p);
                        }
                        else {
                            menor_tempo = get_remaining_time(p2);
                        }
                        printf("[FCFS] Executando processo PID %d // processador %d\n", get_pid(processadores[0]), 0);
                        printf("[FCFS] Executando processo PID %d // processador %d\n", get_pid(processadores[1]), 1);
                        tempo_passado+=running(processadores[0], menor_tempo, FCFS);
                        tempo_passado+=running(processadores[1], menor_tempo, FCFS);
                        if (processadores[0]->state == TERMINATED){
                            processos_finalizados++;
                            y++;
                            if (processadores[1]->state == TERMINATED){
                                processos_finalizados++;
                                y++;
                            }
                            else {
                                enqueue(prontos, processadores[1]);
                            }
                        }
                        else if (processadores[1]->state == TERMINATED){
                            enqueue(prontos, processadores[0]);
                            processos_finalizados++;
                            y++;
                        }

                        for (int j = 0; j < NUM_PROCESSADORES; j++) {
                            if (processadores[j] != NULL && get_pid(processadores[j]) == get_pid(p)) {
                                processadores[j] = NULL;
                            }
                        }
                        y++;
                }
            }
            else {
                tempo_passado++;
            }
        }
    
    for (int j=y; j < num; j++){
        if (tempo_passado >= get_start_time(processos[j]) && processos[j] != NULL){
            if (processos[j] != NULL){
                enqueue(prontos, processos[j]);
            }
        }
    }
    if (processos_finalizados>= num){
        break;
    }
    }
    
    destroyQueue(prontos);
}


void rr_multiprocessador(PCB* processos[], int num, int quantum) {
    Queue *prontos = createQueue();
    PCB *processadores[NUM_PROCESSADORES] = {NULL};
    int processos_finalizados = 0;
    int idx_chegada = 0;
    int tempo_passado = 0;
    
    while (processos_finalizados < num) {
        while (idx_chegada < num && get_start_time(processos[idx_chegada]) <= tempo_passado) {
            enqueue(prontos, processos[idx_chegada]);
            idx_chegada++;
        }
        
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] == NULL && !isEmpty(prontos)) {
                processadores[i] = (PCB *)dequeue(prontos);
                if (i == 0 && isEmpty(prontos)) {
                    processadores[i+1] = processadores[i]; // ocupa a próxima CPU também
                    
                    printf("[RR] Executando processo PID %d com quantum %dms // processador %d\n", 
                           get_pid(processadores[i]), quantum, i);
                    printf("[RR] Executando processo PID %d com quantum %dms // processador %d\n", 
                           get_pid(processadores[i+1]), quantum, i+1);
                    break;
                } else{
                    printf("[RR] Executando processo PID %d com quantum %dms // processador %d\n", 
                           get_pid(processadores[i]), quantum, i);

                }
            }
        }
        
        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] != NULL) {
                PCB* processo_atual = processadores[i];
                tempo_passado+=running(processo_atual, quantum, RR);

                if (processo_atual->state == TERMINATED) {
                    processos_finalizados++;
                    
                    for (int j = 0; j < NUM_PROCESSADORES; j++) {
                        if (processadores[j] != NULL && get_pid(processadores[j]) == get_pid(processo_atual)) {
                            processadores[j] = NULL;
                        }
                    }
                } else {
                    enqueue(prontos, processo_atual);
                    processadores[i] = NULL; 
                }
            }
        }
        
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
        while (idx_processos < num && get_start_time(processos[idx_processos]) <= tempo_passado) {
            enqueue(prontos, processos[idx_processos]);
            idx_processos++;
        }

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

        sortQueue(prontos, comparePriority);

        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] == NULL && !isEmpty(prontos)) {
                PCB *atual = (PCB *)dequeue(prontos);
                int threads = get_num_threads(atual);

                int cpus_livres = 0;
                for (int j = 0; j < NUM_PROCESSADORES; j++) {
                    if (processadores[j] == NULL) cpus_livres++;
                }

                int cpus_para_ocupar = (threads < cpus_livres) ? threads : cpus_livres;

                for (int j = 0; j < NUM_PROCESSADORES && cpus_para_ocupar > 0; j++) {
                    if (processadores[j] == NULL) {
                        processadores[j] = atual; 
                        printf("[PRIORITY] Executando processo PID %d // processador %d\n", get_pid(atual), j);
                        cpus_para_ocupar--;
                    }
                }
            }
        }

        for (int i = 0; i < NUM_PROCESSADORES; i++) {
            if (processadores[i] != NULL) {
                running(processadores[i], quantum, PRIORITY);

                if (processadores[i]->state == TERMINATED) {
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
    p->vrun_time = 0;

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

void destroyPCB(void* p) {
    PCB* pcb = (PCB*)p;
    if (!pcb) return;
    if (pcb->thread_ids) {
        free(pcb->thread_ids);
    }
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

int get_thread_time(PCB* pcb) {
    return pcb->thread_time;
}

void set_thread_time(PCB* pcb, int t) {
    pcb->thread_time = t;
}

int get_num_threads(PCB* pcb) {
    return pcb->num_threads;
}

int get_vruntime(PCB* pcb) {
    return pcb->vrun_time;
}   

void set_vruntime(PCB* pcb, int vt) {
    pcb->vrun_time = vt;
}

int get_process_len(PCB* pcb) {
    return pcb->process_len;
}