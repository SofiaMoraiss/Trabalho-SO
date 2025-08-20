#include <stdio.h>
#include <stdlib.h>
#include "processo.h"


Processo* initProcesso(int PID, int dur, int prioridade, int qtd_threads, int t_chegada) {
    Processo *p = malloc(sizeof(Processo));
    if (p == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    p->dur = dur;
    p->prioridade = prioridade;
    p->qtd_threads = qtd_threads;
    p->t_chegada = t_chegada;
    p->PID = PID;
    p->running = 0; 

    return p;
}

Processo* FCFS(Processo *p) {
    

    return p; 
}   

int get_prioridade(Processo *p) {
    return p->prioridade;
}

void set_prioridade(Processo *p, int prioridade) {
    p->prioridade = prioridade;
}

int get_dur(Processo *p) {
    return p->dur;
}

void set_dur(Processo *p, int dur) {
    p->dur = dur;
}

int get_qtd_threads(Processo *p) {
    return p->qtd_threads;
}

void set_qtd_threads(Processo *p, int qtd_threads) {
    p->qtd_threads = qtd_threads;
}

int get_t_chegada(Processo *p) {
    return p->t_chegada;
}

void set_t_chegada(Processo *p, int t_chegada) {
    p->t_chegada = t_chegada;
}

int get_PID(Processo *p) {
    return p->PID;
}

void set_PID(Processo *p, int PID) {
    p->PID = PID;
}