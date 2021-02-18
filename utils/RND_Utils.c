#include <time.h>
#include <sys/time.h>
#include <RND_ErrMsg.h>

#include "RND_Utils.h"

double RND_getWallTime_usec()
{
    struct timeval time;
    if (gettimeofday(&time, NULL)) {
        RND_ERROR("gettimeofday returned error");
        return 0;
    }
    return (double)time.tv_sec * 1e6 + (double)time.tv_usec;
}

double RND_getCpuTime_usec()
{
    return (double)1e6 / CLOCKS_PER_SEC * clock();
}
