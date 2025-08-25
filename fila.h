#ifndef FILA_H
#define FILA_H

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

// Estrutura para a fila
typedef struct Queue {
    Node* front; // Aponta para o início da fila
    Node* rear;  // Aponta para o final da fila
} Queue;

// Função para criar uma nova fila
Queue* createQueue();

// Função para verificar se a fila está vazia
int isEmpty(Queue* queue);

// Função para enfileirar um elemento
void enqueue(Queue* queue, void* value);

// Função para desenfileirar um elemento
void * dequeue(Queue* queue);

// Função para exibir os elementos da fila
void displayQueue(Queue* queue);

void sortQueue(Queue* queue, int (*compare)(void*, void*));

// Função para destruir a fila e liberar memória
void destroyQueue(Queue* queue);

#endif // FILA_H
