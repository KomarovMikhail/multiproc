#include <stdlib.h>
#include "thread_pool.h"

task_t * task_init(void (*function)(void*), void * args)
{
    task_t * task = (task_t*)malloc(sizeof(task_t));
    task->function = function;
    task->args = args;
    return task;
}

void task_destroy(task_t * task)
{
    free(task);
}

thread_pool_t * create_thread_pool(size_t size)
{
    thread_pool_t * tp = (thread_pool_t *)malloc(sizeof(thread_pool_t));

    tp->size = 0;
    tp->threads = (pthread_t *)malloc(sizeof(pthread_t) * size);
    tp->queue = queue_init();
    tp->count = 0;
    tp->shutdown = 0;
    tp->started = 0;

    pthread_mutex_init(&tp->lock, NULL);
    pthread_cond_init(&tp->notify, NULL);

    for(size_t i = 0; i < size; i++)
    {
        pthread_create(tp->threads + i, NULL, thread_worker, (void *)tp);
        tp->size++;
        tp->started++;
    }

    return tp;
}

void destroy_thread_pool(thread_pool_t *tp)
{
    pthread_mutex_lock(&(tp->lock));

    tp->shutdown = 1;
    pthread_cond_broadcast(&(tp->notify));

    pthread_mutex_unlock(&(tp->lock));

    for(size_t i = 0; i < tp->size; i++)
    {
        pthread_join(tp->threads[i], NULL);
    }

    thread_pool_free(tp);

}

void thread_pool_free(thread_pool_t * tp)
{
    if (tp == NULL || tp->started > 0)
    {
        return;
    }


    free(tp->threads);
    queue_destroy(tp->queue);
    pthread_mutex_destroy(&(tp->lock));
    pthread_cond_destroy(&(tp->notify));
    free(tp);
}


void add_task(thread_pool_t *tp, void (*function)(void *), void * args)
{
    task_t * task = task_init(function, args);

    pthread_mutex_lock(&(tp->lock));

    if (tp->shutdown)
    {
        printf("Thread pool is shutdown\n");
        return;
    }

    queue_push(tp->queue, (void *)task);
    tp->count++;
    pthread_cond_signal(&(tp->notify));

    pthread_mutex_unlock(&(tp->lock));
}

static void * thread_worker(void * thread_pool)
{
    thread_pool_t * tp = (thread_pool_t *)thread_pool;
    task_t * task;

    while(1)
    {
        pthread_mutex_lock(&(tp->lock));

        while ((tp->count == 0) && (!tp->shutdown))
        {
            pthread_cond_wait(&(tp->notify), &(tp->lock));
        }

        if (tp->shutdown)
        {
            break;
        }

        task = (task_t *)queue_pop(tp->queue);

//        tp->count--;

        pthread_mutex_unlock(&(tp->lock));

        (*(task->function))(task->args);
        //task_destroy(task);
    }

    tp->started--;
    pthread_mutex_unlock(&(tp->lock));
    pthread_exit(NULL);
    return NULL;
}

void job(void * args)
{
    int i = * (int *) args;
    printf("here the int: %d\n", i);
}

int main(){

    thread_pool_t * tp = create_thread_pool(4);

    for (int i = 0; i < 10; i++)
    {
        printf("iteration: %d\n", i);
        add_task(tp, (void *)job, (void *) (&i));
    }
    destroy_thread_pool(tp);
    return 0;
}
