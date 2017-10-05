#ifndef PARALLEL_MEGRE_PTHREADS_THREAD_POOL_H
#define PARALLEL_MEGRE_PTHREADS_THREAD_POOL_H

#include <pthread.h>
#include "queue.h"

typedef struct task_t {
    void (*function)(void*);
    void * args;
} task_t;

typedef struct thread_pool_t {
    pthread_t * threads;
    queue_t * queue;
    size_t size;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    int started;
    int count;
    int shutdown;
} thread_pool_t;

task_t * task_init(void (*function_p)(void*), void * args);
void task_destroy(task_t * task);

thread_pool_t * create_thread_pool(size_t size);
void destroy_thread_pool(thread_pool_t *tp);
void add_task(thread_pool_t *tp, void (*function)(void *), void * args);
void thread_pool_free(thread_pool_t * tp);


static void * thread_worker(void * thread_pool);

#endif //PARALLEL_MEGRE_PTHREADS_THREAD_POOL_H
