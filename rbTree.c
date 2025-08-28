#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "rbTree.h"
#include "pcb.h"

// Funções auxiliares (devem ser estáticas)
static int rbnode_size(RBNode* node);
static RBNode* fix_up(RBNode* h);
static RBNode* move_red_left(RBNode* h);
static RBNode* move_red_right(RBNode* h);
static RBNode* rb_delete_min(RBNode* h, FreeFunc free_value);
static RBNode* rb_delete(RBNode* h, int key, FreeFunc free_value);
static RBNode* rb_insert(RBNode *h, int key, void *value, CompareFunc cmp);
static RBNode *rb_min_node(RBNode *node);

// Funções de manipulação da árvore
int is_red(RBNode *node) {
    return node != NULL && node->color == RED;
}

RBNode *rotate_left(RBNode *h) {
    RBNode *x = h->right;
    h->right = x->left;
    x->left = h;
    x->color = h->color;
    h->color = RED;
    return x;
}

RBNode *rotate_right(RBNode *h) {
    RBNode *x = h->left;
    h->left = x->right;
    x->right = h;
    x->color = h->color;
    h->color = RED;
    return x;
}

void flip_colors(RBNode *h) {
    h->color = RED;
    if (h->left) h->left->color = BLACK;
    if (h->right) h->right->color = BLACK;
}

static RBNode *rb_min_node(RBNode *node) {
    if (node == NULL) return NULL;
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

static RBNode* rb_insert(RBNode *h, int key, void *value, CompareFunc cmp) {
    if (!h) {
        RBNode *node = malloc(sizeof(RBNode));
        node->key = key;
        node->value = value;
        node->left = node->right = NULL;
        node->color = RED;
        return node;
    }

    if (key < h->key) {
        h->left = rb_insert(h->left, key, value, cmp);
    } else if (key > h->key) {
        h->right = rb_insert(h->right, key, value, cmp);
    } else {
        // A chave já existe, apenas atualiza o valor
        h->value = value;
    }

    if (is_red(h->right) && !is_red(h->left))
        h = rotate_left(h);
    if (is_red(h->left) && is_red(h->left->left))
        h = rotate_right(h);
    if (is_red(h->left) && is_red(h->right))
        flip_colors(h);

    return h;
}

void rbtree_insert(RBTree *tree, int key, void *value) {
    tree->root = rb_insert(tree->root, key, value, tree->cmp);
    tree->root->color = BLACK;
}

void *rbtree_search(RBTree *tree, int key) {
    RBNode *x = tree->root;
    while (x) {
        if (key < x->key)
            x = x->left;
        else if (key > x->key)
            x = x->right;
        else
            return x->value;
    }
    return NULL;
}

RBTree *rbtree_create(CompareFunc cmp, FreeFunc free_value) {
    RBTree *tree = malloc(sizeof(RBTree));
    tree->root = NULL;
    tree->cmp = cmp;
    tree->free_value = free_value;
    return tree;
}

// Funções para deleção
static RBNode* move_red_left(RBNode* h) {
    flip_colors(h);
    if (h->right != NULL && is_red(h->right->left)) {
        h->right = rotate_right(h->right);
        h = rotate_left(h);
        flip_colors(h);
    }
    return h;
}
static RBNode* move_red_right(RBNode* h) {
    flip_colors(h);
    if (h->left != NULL && is_red(h->left->left)) {
        h = rotate_right(h);
        flip_colors(h);
    }
    return h;
}
static RBNode* fix_up(RBNode* h) {
    if (is_red(h->right) && !is_red(h->left)) {
        h = rotate_left(h);
    }
    if (is_red(h->left) && is_red(h->left->left)) {
        h = rotate_right(h);
    }
    if (is_red(h->left) && is_red(h->right)) {
        flip_colors(h);
    }
    return h;
}

static RBNode* rb_delete_min(RBNode* h, FreeFunc free_value) {
    if (h->left == NULL) {
        if (free_value) free_value(h->value);
        free(h);
        return NULL;
    }
    if (!is_red(h->left) && h->left != NULL && !is_red(h->left->left)) {
        h = move_red_left(h);
    }
    h->left = rb_delete_min(h->left, free_value);
    return fix_up(h);
}

static RBNode* rb_delete(RBNode* h, int key, FreeFunc free_value) {
    if (key < h->key) {
        if (!is_red(h->left) && h->left != NULL && !is_red(h->left->left)) {
            h = move_red_left(h);
        }
        h->left = rb_delete(h->left, key, free_value);
    } else {
        if (is_red(h->left)) {
            h = rotate_right(h);
        }
        if (key == h->key && (h->right == NULL)) {
            if (free_value) free_value(h->value);
            free(h);
            return NULL;
        }
        if (!is_red(h->right) && h->right != NULL && !is_red(h->right->left)) {
            h = move_red_right(h);
        }
        if (key == h->key) {
            RBNode* x = rb_min_node(h->right);
            if (free_value) free_value(h->value);
            h->key = x->key;
            h->value = x->value;
            h->right = rb_delete_min(h->right, NULL); 
        } else {
            h->right = rb_delete(h->right, key, free_value);
        }
    }
    return fix_up(h);
}

void rbtree_delete(RBTree* tree, int key) {
    if (rbtree_search(tree, key) == NULL) return;
    tree->root = rb_delete(tree->root, key, tree->free_value);
    if (tree->root != NULL) {
        tree->root->color = BLACK;
    }
}

// Funções de busca e tamanho
static void* rb_find_min_vruntime_node_value(RBNode* node) {
    if (node == NULL) {
        return NULL;
    }

    void* min_value = node->value;
    void* left_value = rb_find_min_vruntime_node_value(node->left);
    void* right_value = rb_find_min_vruntime_node_value(node->right);

    if (left_value != NULL && ((PCB*)left_value)->vrun_time < ((PCB*)min_value)->vrun_time) {
        min_value = left_value;
    }
    if (right_value != NULL && ((PCB*)right_value)->vrun_time < ((PCB*)min_value)->vrun_time) {
        min_value = right_value;
    }

    return min_value;
}

void *rbtree_min_vruntime_value(RBTree *tree) {
    if (tree == NULL || tree->root == NULL) {
        return NULL;
    }
    return rb_find_min_vruntime_node_value(tree->root);
}

static int rbnode_size(RBNode* node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + rbnode_size(node->left) + rbnode_size(node->right);
}

int rbtree_size(RBTree* tree) {
    if (tree == NULL) {
        return 0;
    }
    return rbnode_size(tree->root);
}

void rbnode_free(RBNode *node, FreeFunc free_value) {
    if (!node) return;
    rbnode_free(node->left, free_value);
    rbnode_free(node->right, free_value);
    if (free_value) free_value(node->value);
    free(node);
}

void rbtree_free(RBTree *tree) {
    rbnode_free(tree->root, tree->free_value);
    free(tree);
}