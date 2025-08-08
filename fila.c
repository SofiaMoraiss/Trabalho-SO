#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

// Função para criar uma nova fila circular
Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

// Função para verificar se a fila está vazia
int isEmpty(Queue* queue) {
    return queue->front == NULL;
}

// Função para enfileirar um elemento na fila circular
void enqueue(Queue* queue, void* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    if (queue->rear == NULL) {
        newNode->next = newNode; // Circular link
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        newNode->next = queue->front; // Circular link
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Função para desenfileirar um elemento da fila circular
void* dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        return NULL;
    }
    Node* temp = queue->front;
    void* value = temp->data;
    if (queue->front == queue->rear) {
        // Single element in the queue
        queue->front = NULL;
        queue->rear = NULL;
    } else {
        queue->front = queue->front->next;
        queue->rear->next = queue->front; // Maintain circular link
    }
    free(temp);
    return value;
}

// Função para exibir os elementos da fila circular
void displayQueue(Queue* queue) {
    if (isEmpty(queue)) {
        return;
    }
    Node* current = queue->front;
    int i = 0;
    do {
        i++;
        printf("%d ", i);
        current = current->next;
    } while (current != queue->front);
    printf("\n");
}

// Função para ordenar a fila circular usando uma função de comparação
void sortQueue(Queue* queue, int (*compare)(void*, void*)) {
    if (isEmpty(queue)) {
        return;
    }

    // Convert queue to array for sorting
    int size = 0;
    Node* current = queue->front;
    do {
        size++;
        current = current->next;
    } while (current != queue->front);

    void** array = (void**)malloc(size * sizeof(void*));
    current = queue->front;
    for (int i = 0; i < size; i++) {
        array[i] = current->data;
        current = current->next;
    }

    // Sort the array using the comparison function
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (compare(array[j], array[j + 1]) > 0) {
                void* temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }

    // Rebuild the queue from the sorted array
    current = queue->front;
    for (int i = 0; i < size; i++) {
        current->data = array[i];
        current = current->next;
    }

    free(array);
}
