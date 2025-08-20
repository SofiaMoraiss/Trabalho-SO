#ifndef PROCESSO_H
#define PROCESSO_H

typedef struct Processo {
    int PID;
    int dur;
    int prioridade;
    int qtd_threads;
    int t_chegada;
    int running; // flag para indicar se o processo esta em execução
} Processo;

Processo* initProcesso(int PID, int dur, int prioridade, int qtd_threads, int t_chegada);

int get_prioridade(Processo *p);

void set_prioridade(Processo *p, int prioridade);

int get_dur(Processo *p);

void set_dur(Processo *p, int dur);

int get_qtd_threads(Processo *p);

void set_qtd_threads(Processo *p, int qtd_threads);

int get_t_chegada(Processo *p);

void set_t_chegada(Processo *p, int t_chegada);

int get_PID(Processo *p);

void set_PID(Processo *p, int PID);
#endif // PROCESSO_H