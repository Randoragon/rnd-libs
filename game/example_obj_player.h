#include <RND_Game.h>
#include <malloc.h>
#include <RND_ErrMsg.h>

#define OBJECT_PLAYER_MAX_NAME_LENGTH 20

typedef struct
{
    int x, y;
    float health;
    char *name;
} ObjectPlayer;

int objectPlayerCtor(RND_GameInstance *self)
{
    ObjectPlayer *o = (ObjectPlayer*)self->data;
    if (!(o->name = (char*)malloc(sizeof(char) * OBJECT_PLAYER_MAX_NAME_LENGTH))) {
        RND_ERROR("malloc");
        return 1;
    }
    o->name[0] = '\0';
    o->x = 0;
    o->y = 0;
    o->health = 20.0;
    return 0;
}

int objectPlayerDtor(RND_GameInstance *self)
{
    ObjectPlayer *o = (ObjectPlayer*)self->data;
    free(o->name);
    return 0;
}
