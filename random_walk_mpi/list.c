#include "list.h"

node_t *node_init(void *data, node_t *next)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->data = data;
    node->next = next;
    return node;
}

void node_destroy(node_t *node)
{
    free(node);
}

list_t *list_init()
{
    list_t *list = (list_t*)malloc(sizeof(list_t));
    list->size = 0;
    list->head = NULL;
    return list;
}

void list_destroy(list_t *list)
{
    free(list);
}

size_t get_list_size(const list_t *list)
{
    return list->size;
}

void list_add(list_t *list, void *data)
{
    node_t *node = node_init(data, list->head);
    list->head = node;
    list->size++;
}

node_t *get_next_node(list_t *list, node_t *prev)
{
    if (prev == NULL)
    {
        return list->head;
    }
    return prev->next;
}

void *list_remove(list_t *list, node_t *prev)
{
    if (list->size == 0 || prev->next == NULL)
    {
        return NULL;
    }

    node_t *node = prev->next;
    void *data = node->data;
    node *new_next = node->next;
    node_destroy(node);
    prev->next = new_next;
    return data;
}
