#ifndef RBTREE_H
#define RBTREE_H

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int key; 
    void *value;
    struct RBNode *left, *right;
    Color color;
} RBNode;

// Funções de comparação e liberação
typedef int (*CompareFunc)(void *, void *); 
typedef void (*FreeFunc)(void *);

typedef struct {
    RBNode *root;
    CompareFunc cmp;
    FreeFunc free_value;
} RBTree;

// Funções públicas
RBTree *rbtree_create(CompareFunc cmp, FreeFunc free_value);
void rbtree_insert(RBTree *tree, int key, void *value);
void *rbtree_search(RBTree *tree, int key);
void *rbtree_min_vruntime_value(RBTree *tree);
int rbtree_size(RBTree *tree);
void rbtree_delete(RBTree* tree, int key);
void rbtree_free(RBTree *tree);

#endif