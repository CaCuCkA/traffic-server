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

rb_node_t*  create_node(struct in_addr ip, uint32_t count);
void        rotate_left(rb_node_t** root, rb_node_t *node);
void        rotate_right(rb_node_t** root, rb_node_t *node);
void        insert_fixup(rb_node_t** root, rb_node_t *node);
void        insert(rb_node_t** root, rb_node_t* node);
void        inorder(rb_node_t* root);

#endif //TRAFFIC_SERVER_RBTREE_H
