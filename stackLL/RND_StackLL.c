#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_StackLL.h"

RND_StackLL *RND_stackLLCreate()
{
    return NULL;
}

int RND_stackLLPush(RND_StackLL **stack, const void *data)
{
    RND_StackLL *first = *stack;
    if (!(*stack = (RND_StackLL*)malloc(sizeof(RND_StackLL)))) {
        RND_ERROR("malloc");
        return 1;
    }
    (*stack)->data = (void*)data;
    (*stack)->next = first;
    return 0;
}

void *RND_stackLLPeek(const RND_StackLL **stack)
{
    return (*stack)? (*stack)->data : NULL;
}

int RND_stackLLPop(RND_StackLL **stack, int (*dtor)(const void*))
{
    if (!*stack) {
        RND_WARN("the stack is already empty");
        return 1;
    }
    RND_StackLL *next = (*stack)->next;
    int error;
    if (dtor && (error = dtor((*stack)->data))) {
        RND_ERROR("dtor returned %d for data %p", error, (*stack)->data);
        return 2;
    }
    free(*stack);
    *stack = next;
    return 0;
}

int RND_stackLLRemove(RND_StackLL **stack, size_t index, int (*dtor)(const void*))
{
    if (!*stack) {
        RND_WARN("the stack is already empty");
        return 1;
    }
    RND_StackLL *target = *stack,
                *prev   = NULL;
    for (int i = 0; i < index; i++) {
        if (!target) {
            RND_ERROR("index out of range");
            return 3;
        }
        prev = target;
        target = target->next;
    }
    int error;
    if (dtor && (error = dtor(target->data))) {
        RND_ERROR("dtor returned %d for data %p", error, target->data);
        return 2;
    }
    if (prev) {
        prev->next = target->next;
    } else {
        *stack = target->next;
    }
    free(target);
    return 0;
}

int RND_stackLLClear(RND_StackLL **stack, int (*dtor)(const void*))
{
    RND_StackLL *i = *stack;
    while (i) {
        RND_StackLL *j = i->next;
        int error;
        if (dtor && (error = dtor(i->data))) {
            RND_ERROR("dtor returned %d for data %p", error, i->data);
            return 1;
        }
        free(i);
        i = j;
    }
    *stack = NULL;
    return 0;
}

int RND_stackLLDestroy(RND_StackLL **stack, int (*dtor)(const void*))
{
    return RND_stackLLClear(stack, dtor);
}

size_t RND_stackLLSize(const RND_StackLL **stack)
{
    size_t ret = 0;
    for (const RND_StackLL *e = *stack; e; e = e->next, ret++);
    return ret;
}

int RND_stackLLDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

// Run a function for every stack element
int RND_stackLLMap(RND_StackLL **stack, int (*map)(RND_StackLL*, size_t))
{
    if (!*stack || !map) {
        RND_WARN("stack or map function empty");
        return 1;
    }
    size_t p = 0;
    for (RND_StackLL *q = *stack; q; q = q->next, p++) {
        int error;
        if ((error = map(q, p))) {
            RND_ERROR("map function returned %d for element no. %lu (%p)", error, p, (void*)q);
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing stack contents
int RND_stackLLPrintMap(const RND_StackLL *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, (void*)elem, elem->data);
    return 0;
}

// Default method of printing stack contents (for convenience)
int RND_stackLLPrint(const RND_StackLL **stack)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_stackLLMap((RND_StackLL**)stack, (int (*)(RND_StackLL*, size_t))RND_stackLLPrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}
