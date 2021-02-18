#include <RND_Game.h>
#include <stdbool.h>
#include <RND_ErrMsg.h>

typedef struct
{
    int x, y;
    float health;
    bool dead;
} ObjectEnemy;

int objectEnemyCtor(RND_GameInstance *self)
{
    ObjectEnemy *o = (ObjectEnemy*)self->data;
    o->x = 0;
    o->y = 0;
    o->health = 20.0;
    o->dead = false;
    return 0;
}
