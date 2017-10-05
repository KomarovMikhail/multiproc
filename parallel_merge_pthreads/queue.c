#include "queue.h"
#include <stdlib.h>

node_t * node_init(void * value, node_t * next, node_t * prev)
{
    node_t * node = (node_t *)malloc(sizeof(node_t));
    node->next = next;
    node->value = value;
    node->prev = prev;
    return node;
}

void node_destroy(node_t * node)
{
    free(node->value);
    free(node->next);
    free(node->prev);
    free(node);
}

queue_t * queue_init()
{
    queue_t * q = (queue_t *)malloc(sizeof(queue_t));
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;

    pthread_mutex_init(&(q->mutex), NULL);
    return q;
}

void queue_push(queue_t * q, void * val)
{
    pthread_mutex_lock(&q->mutex);
    if (q->size == 0)
    {
        node_t * node = node_init(val, NULL, NULL);
        q->tail = node;
        q->head = node;
    }
    else
    {
        node_t * node = node_init(val, q->tail, NULL);
        q->tail->prev = node;
        q->tail = node;
    }
    q->size++;
    pthread_mutex_unlock(&q->mutex);
}

void * queue_pop(queue_t * q)
{
    pthread_mutex_lock(&q->mutex);
    if (q->size == 0)
    {
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }

    void * res;
    if (q->size == 1)
    {
        res = q->head->value;
        //node_destroy(q->head);
        q->head = NULL;
        q->tail = NULL;
    }
    else
    {
        res = q->head->value;
        q->head = q->head->prev;
        //node_destroy(q->head->next);
        q->head->next = NULL;

    }
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return res;
}

size_t queue_get_size(queue_t * q)
{
    return q->size;
}

void queue_destroy(queue_t * q)
{
    while (q->size > 0)
    {
        queue_pop(q);
    }

    pthread_mutex_destroy(&(q->mutex));
    free(q);
}
