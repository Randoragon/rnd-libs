#include <stdio.h>
#include "RND_BitArray.h"

int main(int argc, char **argv)
{
    RND_BitArray *arr1 = RND_bitArrayCreate(19), *arr2 = RND_bitArrayCreate(8), *arr3 = RND_bitArrayCreate(8);
    RND_bitArraySet(arr1, 3, true);
    RND_bitArraySet(arr1, 16, true);
    RND_bitArrayToggle(arr1, 18);
    RND_bitArrayPrint(arr1);
    RND_bitArraySetf(arr1, "0b 1011 1010 1100 001");
    RND_bitArrayPrint(arr1);
    printf("----------------\n");
    RND_bitArraySetf(arr2, "0b0101 0110");
    RND_bitArraySetf(arr3, "0b0011 1101");
    RND_bitArrayPrint(arr2);
    RND_bitArrayPrint(arr3);
    printf("&=\n");
    RND_bitArrayAnd(arr2, arr3);
    RND_bitArrayPrint(arr2);
    printf("----------------\n");
    RND_bitArraySetf(arr2, "0b0101 0110");
    RND_bitArrayPrint(arr2);
    RND_bitArrayPrint(arr3);
    printf("|=\n");
    RND_bitArrayOr(arr2, arr3);
    RND_bitArrayPrint(arr2);
    printf("----------------\n");
    RND_bitArraySetf(arr2, "0b0101 0110");
    RND_bitArrayPrint(arr2);
    RND_bitArrayPrint(arr3);
    printf("^=\n");
    RND_bitArrayXor(arr2, arr3);
    RND_bitArrayPrint(arr2);
    printf("----------------\n");
    RND_bitArrayPrint(arr2);
    printf("~=\n");
    RND_bitArrayNegate(arr2);
    RND_bitArrayPrint(arr2);

    RND_bitArrayDestroy(arr1);
    RND_bitArrayDestroy(arr2);
    RND_bitArrayDestroy(arr3);
    return EXIT_SUCCESS;
}
