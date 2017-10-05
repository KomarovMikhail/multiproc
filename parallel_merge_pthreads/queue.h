#ifndef PARALLEL_MEGRE_PTHREADS_QUEUE_H
#define PARALLEL_MEGRE_PTHREADS_QUEUE_H

#include <stdio.h>
#include <pthread.h>

typedef struct node_t {
    void * value;
    struct node_t * next;
    struct node_t * prev;
} node_t;

typedef struct queue_t {
    size_t size;
    pthread_mutex_t mutex;
    struct node_t * head, * tail;

} queue_t;

node_t * node_init(void * value, node_t * next, node_t * prev);
void node_destroy(node_t * node);

queue_t * queue_init();
void queue_push(queue_t * q, void * val);
void * queue_pop(queue_t * q);
size_t queue_get_size(queue_t * q);
void queue_destroy(queue_t * q);

#endif //PARALLEL_MEGRE_PTHREADS_QUEUE_H
