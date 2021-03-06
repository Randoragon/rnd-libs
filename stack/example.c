#include <stdio.h>
#include <RND_Stack.h>

int push(RND_Stack *stack, int val)
{
    int *new;
    if (!(new = (int*)malloc(sizeof(int)))) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    *new = val;
    return RND_stackPush(stack, new);
}

int main(int argc, char **argv)
{
    RND_Stack *test = RND_stackCreate(10);
    push(test, 9);
    push(test, 7);
    push(test, 9);
    push(test, 8);
    push(test, 5);
    push(test, 3);
    push(test, 5);
    push(test, 6);
    push(test, 2);
    push(test, 9);
    push(test, 5);
    push(test, 1);
    push(test, 4);
    push(test, 1);
    push(test, 3);
    RND_stackPrint(test);
    RND_stackRemove(test, 3, RND_stackDtorFree);
    int *val;
    val = (int*)RND_stackPeek(test);
    printf("value on top: %p -> %d\n", val, *val);
    printf("capacity: %lu\n", test->capacity);
    RND_stackPop(test, RND_stackDtorFree);
    RND_stackPrint(test);
    RND_Stack *copy = malloc(sizeof(RND_Stack));
    RND_stackCopy(copy, test, NULL);
    RND_stackPrint(copy);
    RND_stackDestroy(test, RND_stackDtorFree);
    RND_stackDestroy(copy, NULL);

    return EXIT_SUCCESS;
}
