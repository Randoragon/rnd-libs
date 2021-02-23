#include <malloc.h>
#include <string.h>
#include <RND_ErrMsg.h>
#include "RND_HashMap.h"

RND_HashMap *RND_hashMapCreate(size_t size, size_t (*hash)(const char *key, size_t size))
{
    RND_HashMap *new;
    if (!(new = (RND_HashMap*)malloc(sizeof(RND_HashMap)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    new->size = size;
    new->hash = hash? hash : RND_hashMapDefaultHashFunction;

    if (!(new->data = (RND_LinkedList**)malloc(sizeof(RND_LinkedList*) * size))) {
        RND_ERROR("malloc");
        free(new);
        return NULL;
    }
    for (size_t i = 0; i < size; i++) {
        new->data[i] = RND_linkedListCreate();
    }
    return new;
}

size_t RND_hashMapDefaultHashFunction(const char *key, size_t size)
{
    /* This hash function is called djb2. It was first reported
     * by Dan Bernstein.
     * Source: https://www.sparknotes.com/cs/searching/hashtables/section2/
     */
    size_t ret, c;
    ret = 5381;
    while ((c = *(key++))) {
        ret = ((ret << 5) + ret) + c;
    }
    return (ret * 33 + c) % size;
}

int RND_hashMapAdd(RND_HashMap *map, const char *key, const void *value)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    size_t index = map->hash(key, map->size);
    RND_HashMapPair *new;
    if (!(new = (RND_HashMapPair*)malloc(sizeof(RND_HashMapPair)))) {
        RND_ERROR("malloc");
        return 2;
    }
    if (!(new->key = (char*)malloc(sizeof(char) * (strlen(key) + 1)))) {
        RND_ERROR("malloc");
        free(new);
        return 2;
    }
    strcpy((char*)new->key, key);
    new->value = (void*)value;
    int error;
    if ((error = RND_linkedListAdd(map->data + index, new))) {
        RND_ERROR("RND_linkedListAdd returned %d for hash index %lu, data %p", error, index, (void*)new);
        free((void*)new->key);
        free(new);
        return 3;
    }
    return 0;
}

void *RND_hashMapGet(const RND_HashMap *map, const char *key)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return NULL;
    }
    RND_LinkedList *list;
    size_t index;
    index = map->hash(key, map->size);
    list = map->data[index];
    for (size_t i = 0; i < RND_linkedListSize(&list); i++) {
        RND_HashMapPair *pair = (RND_HashMapPair*)RND_linkedListGet(&list, i);
        if (pair && strcmp(pair->key, key) == 0) {
            return pair->value;
        }
    }
    RND_WARN("hashmap has no key \"%s\"", key);
    return NULL;
}

int RND_hashMapRemove(RND_HashMap *map, const char *key, int (*dtor)(const void*))
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    size_t p = map->hash(key, map->size), q = 0;
    for (RND_LinkedList *elem = map->data[p]; elem; elem = elem->next, q++) {
        RND_HashMapPair *pair = elem->data;
        if (elem && strcmp(pair->key, key) == 0) {
            int error;
            if (dtor && (error = dtor(pair->value))) {
                RND_ERROR("dtor function returned %d for key \"%s\", value %p", error, pair->key, (void*)pair->value);
                return 2;
            }
            if ((error = RND_linkedListRemove(map->data + p, q, NULL))) {
                RND_ERROR("RND_linkedListRemove returned %d for key \"%s\", value %p", error, pair->key, (void*)pair->value);
                return 3;
            }
            free((void*)pair->key);
            free(pair);
            return 0;
        }
    }
    RND_WARN("key \"%s\" not found", key);
    return 0;
}

size_t RND_hashMapSize(const RND_HashMap *map)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 0;
    }
    size_t ret = 0;
    for (size_t i = 0; i < map->size; i++) {
        ret += RND_linkedListSize(map->data + i);
    }
    return ret;
}

RND_HashMapPair *RND_hashMapIndex(const RND_HashMap *map, size_t index)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return NULL;
    }
    size_t p = 0, q = 0, s = 0;
    while (p <= index && q < map->size) {
        s = RND_linkedListSize(map->data + q);
        p += s;
        q++;
    }
    return (RND_HashMapPair*)RND_linkedListGet(map->data + q - 1, s - (p - index));
}

int RND_hashMapClear(RND_HashMap *map, int (*dtor)(const void*))
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    for (size_t i = 0; i < map->size; i++) {
        for (RND_LinkedList *elem = map->data[i]; elem; elem = elem->next) {
            RND_HashMapPair *pair = elem->data;
            if (pair != NULL) {
                int error;
                if (dtor && (error = dtor(pair->value))) {
                    RND_ERROR("dtor function returned %d for key \"%s\", value %p", error, pair->key, (void*)pair->value);
                    return 2;
                }
                free((void*)pair->key);
                free(pair);
            }
        }
        int error;
        if ((error = RND_linkedListDestroy(map->data + i, NULL))) {
            RND_ERROR("RND_linkedListDestroy returned %d for list index %lu", error, i);
            return 3;
        }
    }
    return 0;
}

int RND_hashMapDestroy(RND_HashMap *map, int (*dtor)(const void*))
{
    int error;
    if ((error = RND_hashMapClear(map, dtor))) {
        RND_ERROR("RND_hashMapClear returned %d for map %p", error, (void*)map);
        return 1;
    }
    free(map->data);
    free(map);
    return 0;
}

int RND_hashMapDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

void RND_hashMapPrint(const RND_HashMap *map)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return;
    }
    for (size_t i = 0; i < map->size; i++) {
        printf("[%lu]: ", i);
        if (map->data[i]) {
            printf("\n");
            for (RND_LinkedList *j = map->data[i]; j; j = j->next) {
                RND_HashMapPair *pair = j->data;
                printf("\t{ \"%s\": %p }\n", pair->key, pair->value);
            }
        } else {
            printf("-----\n");
        }
    }
}

int RND_hashMapCopy(RND_HashMap *dest, const RND_HashMap *src, void* (*cpy)(const void*))
{
    if (!src) {
        RND_ERROR("src hashmap does not exist");
        return 2;
    }
    if (!dest) {
        RND_ERROR("dest is NULL");
        return 2;
    }
    dest->size = src->size;
    dest->hash = src->hash;
    if (!(dest->data = (RND_LinkedList**)malloc(sizeof(RND_LinkedList*) * dest->size))) {
        RND_ERROR("malloc");
        return 1;
    }
    for (size_t i = 0; i < dest->size; i++) {
        dest->data[i] = NULL;
        RND_LinkedList *tmp = RND_linkedListCreate();
        for (RND_LinkedList *elem = src->data[i]; elem; elem = elem->next) {
            RND_HashMapPair *old = elem->data,
                            *new;
            if (!(new = malloc(sizeof(RND_HashMapPair)))) {
                RND_ERROR("malloc");
                free(dest->data);
                return 1;
            }
            size_t keysize = strlen(old->key) + 1;
            if (!(new->key = malloc(sizeof(char) * keysize))) {
                RND_ERROR("malloc");
                free(new);
                free(dest->data);
                return 1;
            }
            memcpy((void*)new->key, (void*)old->key, keysize);
            if (cpy) {
                if ((new->value = cpy(old->value)) == NULL) {
                    RND_ERROR("cpy function returned NULL");
                    free((void*)new->key);
                    free(new);
                    free(dest->data);
                    return 3;
                }
            } else {
                new->value = old->value;
            }
            int error;
            if ((error = RND_linkedListAdd(&tmp, new))) {
                RND_ERROR("RND_linkedListAdd returned %d", error);
                free((void*)new->key);
                free(new);
                free(dest->data);
                return 4;
            }
            dest->data[i] = (dest->data[i] == NULL)? tmp : dest->data[i];
            for (; tmp->next; tmp = tmp->next);
        }
    }
    return 0;
}
