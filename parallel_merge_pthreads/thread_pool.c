#include <stdlib.h>
#include "thread_pool.h"

task_t * task_init(void (*function)(void*), void * args)
{
    task_t * task = (task_t) {
            .function = function,
            .args = args
    };
}

thread_pool_t * create_thread_pool(size_t size)
{
    thread_pool_t * tp = (thread_pool_t) {
            .size = size,
            .threads = (pthread_t *)malloc(sizeof(pthread_t) * size),
            .queue = queue_init()
    };

}

void destroy_thread_pool(thread_pool_t *tp)
{
    free(tp->threads);
}

void add_task(thread_pool_t *tp, void (*function)(void *), void * args)
{
    task_t * task = task_init(function, args);
}

