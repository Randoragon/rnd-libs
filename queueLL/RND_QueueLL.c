#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_QueueLL.h"

RND_QueueLL *RND_queueLLCreate()
{
    return NULL;
}

int RND_queueLLPush(RND_QueueLL **queue, const void *data)
{
    if (*queue) {
        RND_QueueLL *new, *last = *queue;
        for (; last->next; last = last->next);
        if (!(new = (RND_QueueLL*)malloc(sizeof(RND_QueueLL)))) {
            RND_ERROR("malloc");
            return 1;
        }
        new->data = (void*)data;
        new->next = NULL;
        last->next = new;
    } else {
        if (!(*queue = (RND_QueueLL*)malloc(sizeof(RND_QueueLL)))) {
            RND_ERROR("malloc");
            return 1;
        }
        (*queue)->data = (void*)data;
        (*queue)->next = NULL;
    }
    return 0;
}

void *RND_queueLLPeek(const RND_QueueLL **queue)
{
    return (*queue)? (*queue)->data : NULL;
}

int RND_queueLLPop(RND_QueueLL **queue, int (*dtor)(const void*))
{
    if (!*queue) {
        RND_WARN("the queue is already empty");
        return 1;
    }
    RND_QueueLL *next = (*queue)->next;
    int error;
    if (dtor && (error = dtor((*queue)->data))) {
        RND_ERROR("dtor returned %d for data %p", error, (*queue)->data);
        return 2;
    }
    free(*queue);
    *queue = next;
    return 0;
}

int RND_queueLLRemove(RND_QueueLL **queue, size_t index, int (*dtor)(const void*))
{
    if (!*queue) {
        RND_WARN("the queue is already empty");
        return 1;
    }
    if (index == 0) {
        int error;
        if (dtor && (error = dtor((*queue)->data))) {
            RND_ERROR("dtor returned %d for data %p", error, (*queue)->data);
            return 2;
        }
        RND_QueueLL *tmp = *queue;
        *queue = (*queue)->next;
        free(tmp);
    } else {
        RND_QueueLL *prev = *queue;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                RND_ERROR("index %lu out of bounds (size %lu)", index, RND_queueLLSize((const RND_QueueLL**)queue));
                return 3;
            }
        }
        if (!prev->next) {
            RND_ERROR("index %lu out of bounds (size %lu)", index, RND_queueLLSize((const RND_QueueLL**)queue));
            return 3;
        }
        int error;
        if (dtor && (error = dtor(prev->next->data))) {
            RND_ERROR("dtor returned %d for data %p", error, prev->next->data);
            return 2;
        }
        RND_QueueLL *tmp;
        tmp = prev->next->next;
        free(prev->next);
        prev->next = tmp;
    }
    return 0;
}

int RND_queueLLClear(RND_QueueLL **queue, int (*dtor)(const void*))
{
    RND_QueueLL *i = *queue;
    while (i) {
        RND_QueueLL *j = i->next;
        int error;
        if (dtor && (error = dtor(i->data))) {
            RND_ERROR("dtor returned %d for data %p", error, i->data);
            return 1;
        }
        free(i);
        i = j;
    }
    *queue = NULL;
    return 0;
}

int RND_queueLLDestroy(RND_QueueLL **queue, int (*dtor)(const void*))
{
    return RND_queueLLClear(queue, dtor);
}

size_t RND_queueLLSize(const RND_QueueLL **queue)
{
    size_t ret = 0;
    for (const RND_QueueLL *e = *queue; e; e = e->next, ret++);
    return ret;
}

int RND_queueLLDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

// Run a function for every queue element
int RND_queueLLMap(RND_QueueLL **queue, int (*map)(RND_QueueLL*, size_t))
{
    if (!*queue || !map) {
        RND_WARN("queue or map function empty");
        return 1;
    }
    size_t p = 0;
    for (RND_QueueLL *q = *queue; q; q = q->next, p++) {
        int error;
        if ((error = map(q, p))) {
            RND_ERROR("map function returned %d for element no. %lu (%p)", error, p, (void*)q);
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing queue contents
int RND_queueLLPrintMap(const RND_QueueLL *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, (void*)elem, elem->data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_queueLLPrint(const RND_QueueLL **queue)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_queueLLMap((RND_QueueLL**)queue, (int (*)(RND_QueueLL*, size_t))RND_queueLLPrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}
