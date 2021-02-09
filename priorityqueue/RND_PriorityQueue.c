#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <RND_ErrMsg.h>
#include "RND_PriorityQueue.h"

RND_PriorityQueue *RND_priorityQueueCreate(size_t capacity)
{
    RND_PriorityQueue *queue;
    if (!(queue = malloc(sizeof(RND_PriorityQueue)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!capacity) {
        RND_ERROR("capacity must be a positive value");
        free(queue);
        return NULL;
    }
    if (!(queue->data = malloc(sizeof(RND_PriorityQueuePair) * capacity))) {
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

int RND_priorityQueuePush(RND_PriorityQueue *queue, const void *data, int priority)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (queue->size == queue->capacity) {
        queue->capacity *= 2;
        RND_PriorityQueuePair *new;
        if (!(new = malloc(sizeof(RND_PriorityQueuePair) * queue->capacity))) {
            RND_ERROR("malloc");
            return 2;
        }
        for (size_t i = 0; i < queue->size; i++) {
            memcpy(new + i, queue->data + ((queue->head - queue->data + i) % queue->size), sizeof(RND_PriorityQueuePair));
        }
        free(queue->data);
        queue->data = new;
        queue->head = queue->data;
        queue->tail = queue->data + queue->size - 1;
    }
    if (!queue->size) {
        queue->tail->value = (void*)data;
        memcpy((void*)(&queue->tail->priority), &priority, sizeof(int));
        queue->size++;
    } else {
        RND_PriorityQueuePair *edge = queue->data + queue->capacity - 1,
                              *pos  = queue->head,
                              *end  = (queue->tail == edge)? queue->data : queue->tail + 1;

        if (queue->size > 4) {
            /* Binary search doublets of adjacent elements to find the right spot for insertion.
             * The first and last spots are checked separately before starting bsearch for
             * simplicity.
             */
            if (priority >= queue->tail->priority) {
                pos = end;
            } else if (priority >= queue->head->priority) {
                size_t lidx = 0,
                       ridx = queue->size;
                while (1) {
                    size_t idx = (lidx + ridx) / 2;
                    pos = queue->data + ((queue->head - queue->data + idx) % queue->capacity);
                    RND_PriorityQueuePair *left  = (pos == queue->data)? edge : pos - 1;
                    if (priority >= pos->priority) {
                        lidx = idx + 1;
                    } else if (priority < left->priority) {
                        ridx = idx;
                    } else {
                        break;
                    }
                }
            } else {
            }
        } else {
            // Fallback to linear search
            for (;pos != end && priority > pos->priority;
                    pos = (pos == edge)? queue->data : pos + 1);
        }
        for (
                RND_PriorityQueuePair *dest = end,
                                      *src  = queue->tail;
                dest != pos;
                dest = src,
                src  = (src == queue->data)? queue->data + queue->capacity - 1 : src - 1) {
            dest->value = src->value;
            memcpy((void*)(&dest->priority), &src->priority, sizeof(int));
        }
        pos->value = (void*)data;
        memcpy((void*)(&pos->priority), &priority, sizeof(int));
        queue->tail = end;
        queue->size++;
    }
    return 0;
}

void *RND_priorityQueuePeek(const RND_PriorityQueue *queue)
{
    return queue? queue->head->value : NULL;
}

int RND_priorityQueuePop(RND_PriorityQueue *queue, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if (dtor && (error = dtor(queue->head->value))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, queue->head->value);
        return 2;
    }
    queue->head = (queue->head == queue->data + queue->capacity - 1)? queue->data : queue->head + 1;
    queue->size--;
    return 0;
}

int RND_priorityQueueRemove(RND_PriorityQueue *queue, size_t index, int (*dtor)(const void *))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (index >= queue->size) {
        RND_ERROR("index out of range");
        return 3;
    }
    int error;
    RND_PriorityQueuePair *elem = queue->head,
                          *src;
    for (int i = 0; i < index; i++) {
        elem = (elem == queue->data + queue->capacity - 1)? queue->data : elem + 1;
    }
    if (dtor && (error = dtor(elem->value))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, elem->value);
        return 2;
    }
    src = (elem == queue->data + queue->capacity - 1)? queue->data : elem + 1;
    while (elem != queue->tail) {
        elem->value = src->value;
        memcpy((void*)(&elem->priority), &src->priority, sizeof(int));
        elem = src;
        src = (src == queue->data + queue->capacity - 1)? queue->data : src + 1;
    }
    queue->tail = (queue->tail == queue->data)? queue->data + queue->capacity - 1 : queue->tail - 1;
    queue->size--;
    return 0;
}

int RND_priorityQueueClear(RND_PriorityQueue *queue, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (dtor) {
        while (queue->size) {
            int error;
            if ((error = dtor(queue->head->value))) {
                RND_ERROR("dtor %p returned %d for data %p", dtor, error, queue->data[queue->size].value);
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

int RND_priorityQueueDestroy(RND_PriorityQueue *queue, int (*dtor)(const void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if ((error = RND_priorityQueueClear(queue, dtor))) {
        RND_ERROR("RND_queueClear returned error %d", error);
        return error;
    }
    free(queue->data);
    free(queue);
    return 0;
}

size_t RND_priorityQueueSize(const RND_PriorityQueue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 0;
    }
    return queue->size;
}

int RND_priorityQueueDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_priorityQueuePrint(const RND_PriorityQueue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+----------------------------------------------------+\n");
    printf("| INDEX | PRIORITY |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------+----------------+----------------|\n");
    for (size_t i = 0; i < queue->size; i++) {
        RND_PriorityQueuePair *adr = queue->data + ((queue->head - queue->data + i) % queue->capacity);
        printf("| %5lu | %8d | %14p | %14p |\n", i, adr->priority, adr, adr->value);
    }
    printf("+----------------------------------------------------+\n");
    return 0;
}

int RND_priorityQueueCopy(RND_PriorityQueue *dest, const RND_PriorityQueue *src, void* (*cpy)(const void *))
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
    if (!(dest->data = calloc(src->capacity, sizeof(RND_PriorityQueuePair)))) {
        RND_ERROR("calloc");
        return 1;
    }
    for (RND_PriorityQueuePair *s = src->data, *d = dest->data; s < src->data + src->capacity; s++, d++) {
        if (cpy != NULL) {
            void *new;
            if (!(new = cpy(s->value))) {
                RND_ERROR("cpy function returned error for %p", s->value);
                return 3;
            }
            d->value = new;
        } else {
            d->value = s->value;
        }
        memcpy((int*)&d->priority, &s->priority, sizeof s->priority);
    }
    dest->head = dest->data + (src->head - src->data);
    dest->tail = dest->data + (src->tail - src->data);
    return 0;
}
