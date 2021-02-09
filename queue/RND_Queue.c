#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_Queue.h"

RND_Queue *RND_queueCreate(size_t capacity)
{
    RND_Queue *queue;
    if (!(queue = malloc(sizeof(RND_Queue)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!capacity) {
        RND_ERROR("capacity must be a positive value");
        free(queue);
        return NULL;
    }
    if (!(queue->data = malloc(sizeof(void*) * capacity))) {
        RND_ERROR("malloc");
        free(queue);
        return NULL;
    }
    queue->size = 0;
    queue->capacity = capacity;
    queue->head = queue->data;
    queue->tail = queue->data;
    return queue;
}

int RND_queuePush(RND_Queue *queue, const void *data)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (queue->size == queue->capacity) {
        void **new;
        queue->capacity *= 2;
        if (!(new = malloc(sizeof(void*) * queue->capacity))) {
            RND_ERROR("malloc");
            return 2;
        }
        for (size_t i = 0; i < queue->size; i++) {
            new[i] = queue->data[((queue->head - queue->data + i) % queue->size)];
        }
        free(queue->data);
        queue->data = new;
        queue->head = queue->data;
        queue->tail = queue->data + queue->size - 1;
    }
    if (queue->size) {
        queue->tail = (queue->tail == queue->data + queue->capacity - 1)? queue->data : queue->tail + 1;
    }
    *queue->tail = (void*)data;
    queue->size++;
    return 0;
}

void *RND_queuePeek(const RND_Queue *queue)
{
    return queue? *queue->head : NULL;
}

int RND_queuePop(RND_Queue *queue, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if (dtor && (error = dtor(*queue->head))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, *queue->head);
        return 2;
    }
    queue->head = (queue->head == queue->data + queue->capacity - 1)? queue->data : queue->head + 1;
    queue->size--;
    return 0;
}

int RND_queueRemove(RND_Queue *queue, size_t index, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (index >= queue->size) {
        RND_ERROR("index out of range");
        return 3;
    }
    void **elem = queue->head,
         **src;
    for (int i = 0; i < index; i++) {
        elem = (elem == queue->data + queue->capacity - 1)? queue->data : elem + 1;
    }
    int error;
    if (dtor && (error = dtor(*elem))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, *elem);
        return 2;
    }
    src = (elem == queue->data + queue->capacity - 1)? queue->data : elem + 1;
    while (elem != queue->tail) {
        *elem = *src;
        elem = src;
        src = (src == queue->data + queue->capacity - 1)? queue->data : src + 1;
    }
    queue->tail = (queue->tail == queue->data)? queue->data + queue->capacity - 1 : queue->tail - 1;
    queue->size--;
    return 0;
}

int RND_queueClear(RND_Queue *queue, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (dtor) {
        while (queue->size) {
            int error;
            if ((error = dtor(*queue->head))) {
                RND_ERROR("dtor %p returned %d for data %p", dtor, error, queue->data[queue->size]);
                return 2;
            }
            queue->head = (queue->head == queue->data + queue->capacity - 1)? queue->data : queue->head + 1;
            queue->size--;
        }
    } else {
        queue->size = 0;
        queue->head = queue->tail;
    }
    return 0;
}

int RND_queueDestroy(RND_Queue *queue, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if ((error = RND_queueClear(queue, dtor))) {
        RND_ERROR("RND_queueClear returned error %d", error);
        return error;
    }
    free(queue->data);
    free(queue);
    return 0;
}

size_t RND_queueSize(RND_Queue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 0;
    }
    return queue->size;
}

int RND_queueDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_queuePrint(RND_Queue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    for (size_t i = 0; i < queue->size; i++) {
        void **adr = queue->data + ((queue->head - queue->data + i) % queue->capacity);
        printf("| %5lu | %14p | %14p |\n", i, adr, *adr);
    }
    printf("+-----------------------------------------+\n");
    return 0;
}

int RND_queueCopy(RND_Queue *dest, const RND_Queue *src, void* (*cpy)(const void *))
{
    if (!dest) {
        RND_ERROR("the dest queue does not exist");
        return 2;
    }
    if (!src) {
        RND_ERROR("the src queue does not exist");
        return 2;
    }
    dest->size = src->size;
    dest->capacity = src->capacity;
    if (!(dest->data = calloc(src->capacity, sizeof(void*)))) {
        RND_ERROR("calloc");
        return 1;
    }
    for (void **s = src->data, **d = dest->data; s < src->data + src->capacity; s++, d++) {
        if (cpy != NULL) {
            void *new;
            if (!(new = cpy(*s))) {
                RND_ERROR("cpy function returned error for %p", *s);
                return 3;
            }
            *d = new;
        } else {
            *d = *s;
        }
    }
    dest->head = dest->data + (src->head - src->data);
    dest->tail = dest->data + (src->tail - src->data);
    return 0;
}
