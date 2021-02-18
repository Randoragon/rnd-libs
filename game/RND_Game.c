#include <RND_LinkedList.h>
#include <RND_ErrMsg.h>
#include <string.h>

#include "RND_Game.h"

// Variable Definitions
RND_GameObjectMeta *RND_objects_meta;
RND_GameInstance *RND_instances;
RND_GameInstanceId RND_instances_size;
RND_GameInstanceId RND_next_instance_id;
RND_GameHandlerFunc *RND_ctors, *RND_dtors;
RND_LinkedList *RND_handlers;

// Function Definitions
int RND_gameInit()
{
    if (!(RND_objects_meta = (RND_GameObjectMeta*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameObjectMeta)))) {
        RND_ERROR("calloc");
        return 1;
    }
    RND_next_instance_id = 1;
    RND_instances_size = RND_GAME_INSTANCES_INIT_SIZE;
    if (!(RND_instances = (RND_GameInstance*)calloc(RND_instances_size, sizeof(RND_GameInstance)))) {
        RND_ERROR("calloc");
        return 1;
    }
    if (!(RND_ctors = (RND_GameHandlerFunc*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameHandlerFunc)))) {
        RND_ERROR("calloc");
        return 1;
    }
    if (!(RND_dtors = (RND_GameHandlerFunc*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameHandlerFunc)))) {
        RND_ERROR("calloc");
        return 1;
    }
    RND_handlers = RND_linkedListCreate();
    return 0;
}

void RND_gameCleanup()
{
    for (RND_GameInstanceId i = 1; i < RND_instances_size; i++) {
        RND_GameInstance *inst = RND_instances + i;
        if (inst->is_alive) {
            if (RND_dtors[inst->index]) {
                int error;
                if ((error = RND_dtors[inst->index](inst))) {
                    RND_WARN("object %d (%s)'s destructor returned %d for instance id %lu",
                            inst->index, RND_gameObjectGetName(inst->index), error, i);
                }
            }
            free(inst->data);
        }
    }
    free(RND_instances);
    for (RND_GameObjectIndex i = 0; i < RND_GAME_OBJECT_MAX; i++) {
        if (RND_objects_meta[i].name)
            free((void*)RND_objects_meta[i].name);
    }
    free(RND_objects_meta);
    free(RND_ctors);
    free(RND_dtors);
    RND_linkedListDestroy(&RND_handlers, RND_gameHandlerListDtor);
}

int RND_gameObjectAdd(const char *name, RND_GameObjectIndex index, size_t size)
{
    if (!name) {
        RND_ERROR("name string must not be empty!");
        return 1;
    }
    if (RND_objects_meta[index].name) {
        RND_ERROR("object index %u is already taken!", index);
        return 2;
    }
    char *newname;
    if (!(newname = (char*)malloc(sizeof(char) * (strlen(name) + 1)))) {
        RND_ERROR("malloc");
        return 3;
    }
    strcpy(newname, name);
    RND_objects_meta[index].name = newname;
    RND_objects_meta[index].size = size;
    return 0;
}

RND_GameInstanceId RND_gameInstanceSpawn(RND_GameObjectIndex index)
{
    if (!RND_objects_meta[index].name) {
        RND_ERROR("object indexed %u does not exist!", index);
        return 0;
    }

    // If instance ids exhausted, try to double array size
    if (RND_next_instance_id >= RND_instances_size) {
        if (RND_instances_size & (1lu << 63)) {
            RND_ERROR("maximum instance id reached (%lu)", RND_instances_size);
            return 0;
        }
        RND_GameInstance *new;
        if (!(new = realloc(RND_instances, sizeof(RND_GameInstance) * RND_instances_size * 2))) {
            RND_ERROR("realloc");
            return 0;
        }
        RND_instances = new;
        uint64_t half = sizeof(RND_GameInstance) * RND_instances_size;
        memset(RND_instances + half, 0, half);
        RND_instances_size *= 2;
    }

    RND_GameInstanceId id = RND_next_instance_id++;
    RND_GameInstance *new = RND_instances + id;
    new->is_alive = true;
    new->index    = index;
    if (!(new->data = malloc(RND_objects_meta[index].size))) {
        RND_ERROR("malloc");
        return 0;
    }
    if (RND_ctors[index]) {
        int error;
        if ((error = RND_ctors[index](new))) {
            RND_WARN("RND_ctors[%u] (%s) returned %d", index, RND_gameObjectGetName(index), error);
        }
    }

    for (RND_LinkedList *elem = RND_handlers; elem; elem = elem->next) {
        RND_GameHandler *h = elem->data;
        RND_GameHandlerOp *hop;
        if (!(hop = malloc(sizeof(RND_GameHandlerOp)))) {
            RND_ERROR("malloc");
            return 0;
        }
        if (!(hop->id = (RND_GameInstanceId*)malloc(sizeof(RND_GameInstanceId)))) {
            RND_ERROR("malloc");
            free(hop);
            return 0;
        }
        hop->opcode = 1;
        *hop->id = id;
        int error;
        if ((error = RND_queuePush(h->queue_pending_changes, hop))) {
            RND_ERROR("RND_queuePush returned %d for instance id %lu, object %u (%s)",
                    error, *hop->id, index, RND_gameObjectGetName(index));
            free(hop->id);
            free(hop);
            return 0;
        }
    }
    return id;
}

int RND_gameInstanceKill(RND_GameInstanceId id)
{
    if (!RND_instances[id].is_alive) {
        RND_WARN("instance id %lu is not alive", id);
        return 0;
    }
    RND_GameInstance *inst = RND_instances + id;
    inst->is_alive = false;
    if (RND_dtors[inst->index]) {
        int error;
        if ((error = RND_dtors[inst->index](inst))) {
            RND_ERROR("RND_dtors[%u] (%s) returned %d for instance id %lu",
                    inst->index, RND_gameObjectGetName(inst->index), error, id);
            return 1;
        }
    }
    free(inst->data);
    inst->data = NULL;

    for (RND_LinkedList *elem = RND_handlers; elem; elem = elem->next) {
        RND_GameHandler *h = elem->data;
        RND_GameHandlerOp *hop;
        if (!(hop = malloc(sizeof(RND_GameHandlerOp)))) {
            RND_ERROR("malloc");
            return 2;
        }
        if (!(hop->id = malloc(sizeof(RND_GameInstanceId)))) {
            RND_ERROR("malloc");
            free(hop);
            return 2;
        }
        hop->opcode = 0;
        int error;
        if ((error = RND_queuePush(h->queue_pending_changes, hop))) {
            RND_ERROR("RND_queuePush returned %d for instance id %lu (%s)",
                    error, *hop->id, RND_gameObjectGetName(RND_instances[*hop->id].index));
            free(hop->id);
            free(hop);
            return 3;
        }
    }
    return 0;
}

RND_GameHandler *RND_gameHandlerCreate(int (*priority_func)(RND_GameObjectIndex))
{
    RND_GameHandler *new;
    if (!(new = (RND_GameHandler*)malloc(sizeof(RND_GameHandler)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!(new->handlers = (RND_GameHandlerFunc*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameHandlerFunc)))) {
        RND_ERROR("calloc");
        free(new);
        return NULL;
    }
    if (!(new->queue = RND_priorityQueueCreate(128))) {
        RND_ERROR("RND_priorityQueueCreate failed");
        free(new);
        return NULL;
    }
    if (!(new->queue_pending_changes = RND_queueCreate(128))) {
        RND_ERROR("RND_queueCreate failed");
        RND_priorityQueueDestroy(new->queue, NULL);
        free(new);
        return NULL;
    }
    int err;
    new->priority_func = priority_func;
    if ((err = RND_linkedListAdd(&RND_handlers, new))) {
        RND_ERROR("RND_linkedListAdd failed (error code %d)\n", err);
        RND_priorityQueueDestroy(new->queue, NULL);
        RND_queueDestroy(new->queue_pending_changes, NULL);
        free(new);
        return NULL;
    }
    return new;
}

int RND_gameHandlerRun(RND_GameHandler *handler)
{
    {
        int error;
        if ((error = RND_gameHandlerUpdateQueue(handler))) {
            RND_ERROR("RND_gameHandlerUpdateQueue returned error %d", error);
            return -1;
        }
    }

    int ret = 0;
    RND_PriorityQueue *q = handler->queue;
    RND_PriorityQueuePair *elem = q->head,
                          *end  = (q->tail == q->data + q->capacity - 1)? q->data : q->tail + 1;
    bool is_first = (RND_priorityQueueSize(q) > 0);
    while (elem != end || is_first) {
        is_first = false;
        RND_GameInstanceId id  = *(RND_GameInstanceId*)elem->value;
        RND_GameInstance *inst = RND_instances + id;
        if (inst->data && handler->handlers[inst->index]) {
            int error;
            if ((error = handler->handlers[inst->index](inst))) {
                RND_ERROR("handler %p returned %d for instance id %lu of object %u (%s)",
                        handler + inst->index, error, id, inst->index, RND_gameObjectGetName(inst->index));
                ret++;
            }
        }
        elem = (elem == q->data + q->capacity - 1)? q->data : elem + 1;
    }

    {
        int error;
        if ((error = RND_gameHandlerUpdateQueue(handler))) {
            RND_ERROR("RND_gameHandlerUpdateQueue returned error %d", error);
            return -1;
        }
    }

    return ret;
}

int RND_gameHandlerDestroy(RND_GameHandler *handler)
{
    if (!handler) {
        RND_WARN("handler does not exist!");
        return 0;
    }
    free(handler->handlers);
    int error;
    if ((error = RND_priorityQueueDestroy(handler->queue, RND_priorityQueueDtorFree))) {
        RND_ERROR("RND_priorityQueueDestroy returned %d for handler %p\n", error, handler);
        return 1;
    }
    if ((error = RND_queueDestroy(handler->queue_pending_changes, RND_gameHandlerOpQueueDtor))) {
        RND_ERROR("RND_queueDestroy returned %d for handler %p\n", error, handler);
        return 2;
    }
    free(handler);
    return 0;
}

int RND_gameHandlerListDtor(const void *handler)
{
    if (!handler) {
        RND_ERROR("handler does not exist!");
        return 1;
    }
    const RND_GameHandler *h = handler;
    free(h->handlers);
    int error;
    if ((error = RND_priorityQueueDestroy(h->queue, RND_priorityQueueDtorFree))) {
        RND_ERROR("RND_priorityQueueDestroy returned %d for handler %p\n", error, h);
        return 2;
    }
    if ((error = RND_queueDestroy(h->queue_pending_changes, RND_gameHandlerOpQueueDtor))) {
        RND_ERROR("RND_queueDestroy returned %d for handler %p\n", error, h);
        return 3;
    }
    return 0;
}

int RND_gameHandlerOpQueueDtor(const void *hop)
{
    const RND_GameHandlerOp *p = hop;
    free(p->id);
    free((void*)hop);
    return 0;
}

int RND_gameHandlerUpdateQueue(RND_GameHandler *handler)
{
    if (!handler) {
        RND_ERROR("handler does not exist!");
        return 1;
    }
    RND_Queue *q = handler->queue_pending_changes;
    RND_PriorityQueue *pq = handler->queue;
    while (q->size > 0) {
        RND_GameHandlerOp *hop = *q->head;
        if (hop == NULL) {
            RND_ERROR("failed to retrieve next operation");
            return 2;
        }
        RND_GameInstanceId *id = hop->id;
        if (hop->opcode == 0) {
            size_t index = 0;
            for (RND_PriorityQueuePair *i = pq->head; i; i = (i == pq->data + pq->capacity - 1)? pq->data : i + 1, index++) {
                if (*id == *(RND_GameInstanceId*)(i->value)) {
                    int error;
                    if ((error = RND_priorityQueueRemove(pq, index, RND_priorityQueueDtorFree))) {
                        RND_ERROR("RND_priorityQueueRemove returned %d for instance id %lu (%s), index %lu",
                                error, *id, RND_gameObjectGetName(RND_instances[*id].index), index);
                        return 3;
                    }
                    break;
                }
            }
            free(id);
        } else {
            RND_GameObjectIndex index;
            int priority;
            index = RND_instances[*id].index;
            priority = (handler->priority_func)? handler->priority_func(index) : 0;
            int err;
            if ((err = RND_priorityQueuePush(pq, id, priority))) {
                RND_ERROR("RND_priorityQueuePush returned %d for instance %lu (%s)",
                        err, *id, RND_gameObjectGetName(index));
                return 4;
            }
        }
        RND_queuePop(q, RND_queueDtorFree); /* Don't use RND_gameHandlerOpQueueDtor on purpose here,
                                             * because we don't want to free the id stored within. */
    }
    return 0;
}
