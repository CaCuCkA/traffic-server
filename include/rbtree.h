#ifndef TRAFFIC_SERVER_RBTREE_H
#define TRAFFIC_SERVER_RBTREE_H

#include <stdint.h>
#include <arpa/inet.h>

#define RED 0
#define BLACK 1

typedef struct
rb_node {
    struct in_addr ip;
    unsigned int count;
    uint8_t color;
    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *parent;
} rb_node_t;

void         rotate_left(rb_node_t** root, rb_node_t *node);
void         rotate_right(rb_node_t** root, rb_node_t *node);
void         insert_fixup(rb_node_t** root, rb_node_t *node);
void         insert(rb_node_t** root, rb_node_t* node);
void         free_tree(rb_node_t* node);
void         save_rbtree(rb_node_t* root);
rb_node_t*   create_node(struct in_addr ip, int count);
rb_node_t*   find_node(rb_node_t* root, struct in_addr ip);
rb_node_t**  rebuild_rbtree();

#endif //TRAFFIC_SERVER_RBTREE_H
