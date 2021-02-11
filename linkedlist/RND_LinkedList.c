#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_LinkedList.h"

RND_LinkedList *RND_linkedListCreate()
{
    return NULL;
}

int RND_linkedListAdd(RND_LinkedList **list, const void *data)
{
    if (*list) {
        RND_LinkedList *new, *last = *list;
        for (; last->next; last = last->next);
        if (!(new = (RND_LinkedList*)malloc(sizeof(RND_LinkedList)))) {
            RND_ERROR("malloc");
            return 1;
        }
        new->data = (void*)data;
        new->next = NULL;
        last->next = new;
    } else {
        if (!(*list = (RND_LinkedList*)malloc(sizeof(RND_LinkedList)))) {
            RND_ERROR("malloc");
            return 1;
        }
        (*list)->data = (void*)data;
        (*list)->next = NULL;
    }
    return 0;
}

int RND_linkedListInsert(RND_LinkedList **list, size_t index, const void *data)
{
    RND_LinkedList *new;
    if (!(new = (RND_LinkedList*)malloc(sizeof(RND_LinkedList)))) {
        RND_ERROR("malloc");
        return 1;
    }
    new->data = (void*)data;

    if (index == 0) {
        new->next = *list;
        *list = new;
    } else {
        RND_LinkedList *prev = *list;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                RND_ERROR("index %lu out of bounds (size %lu)", index, RND_linkedListSize(list));
                free(new);
                return 2;
            }
        }
        new->next = prev->next;
        prev->next = new;
    }
    return 0;
}

void *RND_linkedListGet(RND_LinkedList **list, size_t index)
{
    if (!*list) {
        RND_ERROR("the list is empty");
        return NULL;
    }
    const RND_LinkedList *ret = *list;
    for (int i = 0; i < index; i++) {
        if (ret->next) {
            ret = ret->next;
        } else {
            RND_ERROR("index %lu out of bounds (size %lu)", index, RND_linkedListSize(list));
            return NULL;
        }
    }
    return ret->data;
}

int RND_linkedListRemove(RND_LinkedList **list, size_t index, int (*dtor)(const void *))
{
    if (!*list) {
        RND_WARN("the list is already empty");
        return 1;
    }
    if (index == 0) {
        int error;
        if (dtor && (error = dtor((*list)->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, (*list)->data);
            return 2;
        }
        RND_LinkedList *tmp = *list;
        *list = (*list)->next;
        free(tmp);
    } else {
        RND_LinkedList *prev = *list;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                RND_ERROR("index %lu out of bounds (size %lu)", index, RND_linkedListSize(list));
                return 3;
            }
        }
        if (!prev->next) {
            RND_ERROR("index %lu out of bounds (size %lu)", index, RND_linkedListSize(list));
            return 3;
        }
        int error;
        if (dtor && (error = dtor(prev->next->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, prev->next->data);
            return 2;
        }
        RND_LinkedList *tmp;
        tmp = prev->next->next;
        free(prev->next);
        prev->next = tmp;
    }
    return 0;
}

int RND_linkedListClear(RND_LinkedList **list, int (*dtor)(const void *))
{
    RND_LinkedList *i = *list;
    while (i) {
        RND_LinkedList *j = i->next;
        if (dtor && dtor(i->data)) {
            RND_ERROR("dtor %p returned non-0 for data %p", dtor, i->data);
            return 1;
        }
        free(i);
        i = j;
    }
    *list = NULL;
    return 0;
}

int RND_linkedListDestroy(RND_LinkedList **list, int (*dtor)(const void *))
{
    return RND_linkedListClear(list, dtor);
}

size_t RND_linkedListSize(RND_LinkedList **list)
{
    size_t ret = 0;
    for (const RND_LinkedList *e = *list; e; e = e->next, ret++);
    return ret;
}

int RND_linkedListDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

// Run a function for every list element
int RND_linkedListMap(RND_LinkedList **list, int (*map)(RND_LinkedList*, size_t))
{
    if (!*list || !map) {
        RND_WARN("list or map function empty");
        return 1;
    }
    size_t p = 0;
    for (RND_LinkedList *q = *list; q; q = q->next, p++) {
        int error;
        if ((error = map(q, p))) {
            RND_ERROR("map function %p returned %d for element no. %lu (%p)", map, error, p, q);
            return 2;
        }
    }
    return 0;
}

// Remove all elements yielding true from a filter function
int RND_linkedListFilter(RND_LinkedList **list, bool (*filter)(RND_LinkedList*, size_t), int (*dtor)(const void*))
{
    if (!*list || !filter) {
        RND_WARN("list or filter function empty");
        return 1;
    }
    size_t p = 0, q = 0;
    for (RND_LinkedList *r = *list, *s = NULL; r; s = r, r = r->next, p++, q++) {
        if (filter(r, q)) {
            RND_linkedListRemove(list, p, dtor);
            r = s;
            p--;
        }
    }
    return 0;
}

// Map function used for the default method of printing list contents
int RND_linkedListPrintMap(RND_LinkedList *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, elem, elem->data);
    return 0;
}

// Default method of printing list contents (for convenience)
int RND_linkedListPrint(RND_LinkedList **list)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_linkedListMap((RND_LinkedList**)list, (int (*)(RND_LinkedList*, size_t))RND_linkedListPrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}

int RND_linkedListCopy(RND_LinkedList **dest, RND_LinkedList **src, void* (*cpy)(const void*))
{
    *dest = RND_linkedListCreate();
    if (!*src) {
        RND_WARN("copying an empty list");
        return 0;
    }
    if (!(*dest = malloc(sizeof(RND_LinkedList)))) {
        RND_ERROR("malloc");
        return 1;
    }
    RND_LinkedList *next = NULL,
                   *prev = *dest;
    if (cpy != NULL) {
        prev->data = cpy((*src)->data);
        if (prev->data == NULL) {
            RND_ERROR("cpy function returned NULL");
            return 2;
        }
    } else {
        prev->data = (*src)->data;
        prev->next = NULL;
    }
    for (RND_LinkedList *s = (*src)->next; s != NULL; s = s->next) {
        if (!(next = malloc(sizeof(RND_LinkedList)))) {
            RND_ERROR("malloc");
            return 1;
        }
        if (cpy != NULL) {
            next->data = cpy(s->data);
            if (next->data == NULL) {
                RND_ERROR("cpy function returned NULL");
                return 2;
            }
        } else {
            next->data = s->data;
        }
        prev->next = next;
        next->next = NULL;
        prev = next;
    }
    return 0;
}
