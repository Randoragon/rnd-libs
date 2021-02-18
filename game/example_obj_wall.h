#include <RND_Game.h>
#include <RND_ErrMsg.h>

typedef struct
{
    int x, y;
    short durability;
} ObjectWall;

int objectWallCtor(RND_GameInstance *self)
{
    ObjectWall *o = (ObjectWall*)self->data;
    o->x = 0;
    o->y = 0;
    o->durability = 15;
    return 0;
}
