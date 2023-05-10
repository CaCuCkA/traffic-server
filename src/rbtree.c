#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rbtree.h"

static rb_node_t*
get_uncle(rb_node_t* node) {
    if (!node->parent || !node->parent->parent) {
        return NULL;
    }

    if (node->parent == node->parent->parent->left) {
        return node->parent->parent->right;
    } else {
        return node->parent->parent->left;
    }
}

static void
save_rbtree_helper(rb_node_t* node, FILE* fp) {
    if (node == NULL) {
        return;
    }

    save_rbtree_helper(node->left, fp);

    fwrite(&node->ip, sizeof(node->ip), 1, fp);
    fwrite(&node->count, sizeof(node->count), 1, fp);
    fwrite(&node->color, sizeof(node->color), 1, fp);

    save_rbtree_helper(node->right, fp);
}


rb_node_t*
create_node(struct in_addr ip, int count) {
    rb_node_t* new_node = (rb_node_t*) malloc(sizeof(rb_node_t));
    new_node->ip = ip;
    new_node->count = count;
    new_node->color = RED;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    return new_node;
}

void
rotate_left(rb_node_t** root, rb_node_t* node) {
    if (!node || !node->right) {
        return;
    }
    rb_node_t* right_child = node->right;
    node->right = right_child->left;
    if (node->right != NULL) {
        node->right->parent = node;
    }
    right_child->parent = node->parent;
    if (node->parent == NULL) {
        (*root) = right_child;
    } else if (node == node->parent->left) {
        node->parent->left = right_child;
    } else {
        node->parent->right = right_child;
    }
    right_child->left = node;
    node->parent = right_child;
}

void
rotate_right(rb_node_t** root, rb_node_t* node) {
    if (!node || !node->left) {
        return;
    }
    rb_node_t *left_child = node->left;
    node->left = left_child->right;
    if (left_child->right != NULL) {
        left_child->right->parent = node;
    }
    left_child->parent = node->parent;
    if (left_child->parent == NULL) {
        *(root) = left_child;
    } else if (node == node->parent->left) {
        node->parent->left = left_child;
    } else {
        node->parent->right = left_child;
    }
    left_child->right = node;
    node->parent = left_child;
}

void
insert_fixup(rb_node_t **root, rb_node_t *node) {
    while (node != *root && node != (*root)->left && node != (*root)->right && node->parent->color == RED) {
        rb_node_t *uncle = get_uncle(node);

        if (uncle && uncle->color == RED) {
            node->parent->color = BLACK;
            uncle->color = BLACK;
            node->parent->parent->color = RED;
            node = node->parent->parent;
        } else {
            if (node->parent == node->parent->parent->left) {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotate_left(root, node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rotate_right(root, node->parent->parent);
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotate_right(root, node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rotate_left(root, node->parent->parent);
            }
        }
    }

    (*root)->color = BLACK;
}

void
insert(rb_node_t** root, rb_node_t* node) {
    if (*root == NULL) {
        node->color = BLACK;
        *root = node;
        return;
    }

    rb_node_t* parent = NULL;
    rb_node_t* current = *root;
    while (current != NULL) {
        parent = current;
        if (node->ip.s_addr < current->ip.s_addr) {
            current = current->left;
        } else if (node->ip.s_addr > current->ip.s_addr) {
            current = current->right;
        } else {
            current->count++;
            return;
        }
    }

    node->parent = parent;
    if (node->ip.s_addr < parent->ip.s_addr) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    insert_fixup(root, node);
}

rb_node_t*
find_node(rb_node_t* root, struct in_addr ip) {
    while (root != NULL) {
        if (ip.s_addr < root->ip.s_addr) {
            root = root->left;
        }
        else if (ip.s_addr > root->ip.s_addr) {
            root = root->right;
        }
        else {
            return root;
        }
    }
    return NULL;
}

void
free_tree(rb_node_t* node) {
    if (node == NULL) {
        return;
    }

    free_tree(node->left);
    free_tree(node->right);

    free(node);
}

void
save_rbtree(rb_node_t *root) {
    FILE *fp = fopen("Data.txt", "wb");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    save_rbtree_helper(root, fp);

    fclose(fp);
}

rb_node_t**
rebuild_rbtree() {
    int number = 0;
    uint8_t color = 0;
    struct in_addr ip;
    rb_node_t** root = malloc(sizeof(rb_node_t*));

    FILE* fp = fopen("Data.txt", "rb");
    if (fp == NULL) {
        return root;
    }

    while (1) {
        size_t count = fread(&ip, sizeof(ip), 1, fp);

        if (count != 1) {
            break;
        }
        fread(&number, sizeof(number), 1, fp);
        fread(&color, sizeof(color), 1, fp);

        rb_node_t* node = create_node(ip, number);

        insert(root, node);
    }

    fclose(fp);

    return root;
}

