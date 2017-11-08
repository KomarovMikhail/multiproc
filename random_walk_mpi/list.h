#ifndef UNTITLED2_LIST_H
#define UNTITLED2_LIST_H

#include <stdlib.h>

typedef struct node_t {
    void *data;
    struct node_t *next;
} node_t;

typedef struct list_t {
    node_t *head;
    size_t size;
} list_t;

node_t *node_init(void *data, node_t *next);
void node_destroy(node_t *node);

list_t *list_init();
void list_destroy(list_t *list);
size_t get_list_size(const list_t *list);
void list_add(list_t *list, void *data);
node_t *get_next_node(list_t *list, node_t *prev);
void *list_remove(list_t *list, node_t *prev);

#endif //UNTITLED2_LIST_H
