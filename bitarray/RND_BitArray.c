#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include <string.h>
#include "RND_BitArray.h"

RND_BitArray *RND_bitArrayCreate(size_t size)
{
    RND_BitArray *bitarray;
    if (!(bitarray = (RND_BitArray*)malloc(sizeof(RND_BitArray)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    bitarray->size = size;
    size = (size + 7) / 8;
    if (!(bitarray->bits = (uint8_t*)calloc(size, sizeof(uint8_t)))) {
        RND_ERROR("calloc");
        free(bitarray);
        return NULL;
    }
    return bitarray;
}

bool RND_bitArrayGet(const RND_BitArray *bitarray, size_t index)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 0;
    }
    if (index >= bitarray->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 0;
    }
    return bitarray->bits[index / 8] & (0x80 >> (index % 8));
}

int RND_bitArraySet(RND_BitArray *bitarray, size_t index, bool value)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    if (index >= bitarray->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 1;
    }
    uint8_t *field = bitarray->bits + (index / 8);
    *field = value? (*field | (0x80 >> (index % 8))) : (*field & ~(0x80 >> (index % 8)));
    return 0;
}

int RND_bitArraySetf(RND_BitArray *bitarray, const char *format)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    const char *c = format;
    int base = 0, i = 0;
    if (format != NULL && *format != '\0') {
        while (*c) {
            if (*c != ' ' && *c != '\t') {
                if (i == 0 && *c != '0') {
                    break;
                } else if (i == 1) {
                    if (*c == 'x') {
                        base = 16;
                    } else if (*c == 'b') {
                        base = 2;
                    } else if (*c >= '0' && *c < '8') {
                        base = 8;
                        c--;
                    }
                    break;
                }
                i++;
            }
            c++;
        }
    }
    if (base == 0) {
        RND_ERROR("invalid format string");
        return 2;
    }
    const char *p = format + strlen(format) - 1;
    i = bitarray->size - 1;
    while (p != c && i > 0) {
        uint8_t *field, val;
        switch (base) {
            case 2:
                if ('0' <= *p && *p < '2') {
                    val = (*p - '0');
                    field = bitarray->bits + (i / 8);
                    *field = val? (*field | (0x80 >> (i % 8))) : (*field & ~(0x80 >> (i % 8)));
                    i--;
                } else if (*p != ' ' && *p != '\t') {
                    base = 0;
                }
                break;
            case 8:
                if ('0' <= *p && *p < '8') {
                    val = (*p - '0');
                    for (int j = 2; j >= 0; j--) {
                        field = bitarray->bits + (i / 8);
                        *field = (val & (04 >> j))? (*field | (0x80 >> (i % 8))) : (*field & ~(0x80 >> (i % 8)));
                        i--;
                    }
                } else if (*p != ' ' && *p != '\t') {
                    base = 0;
                }
                break;
            case 16:
                if (('0' <= *p && *p <= '9') || ('a' <= *p && *p <= 'f') || ('A' <= *p && *p <= 'F')) {
                    if ('0' <= *p && *p <= '9') {
                        val = (*p - '0');
                    } else if ('a' <= *p && *p <= 'f') {
                        val = (*p - 'a' + 10);
                    } else if ('A' <= *p && *p <= 'F') {
                        val = (*p - 'A' + 10);
                    }
                    for (int j = 3; j >= 0; j--) {
                        field = bitarray->bits + (i / 8);
                        *field = (val & (0x8 >> j))? (*field | (0x80 >> (i % 8))) : (*field & ~(0x80 >> (i % 8)));
                        i--;
                    }
                } else if (*p != ' ' && *p != '\t') {
                    base = 0;
                }
                break;
        }
        if (base == 0) {
            RND_ERROR("invalid format string");
            return 2;
        }
        p--;
    }
    while (i > 0) {
        uint8_t *field = bitarray->bits + (i / 8);
        *field &= ~(0x80 >> (i % 8));
        i--;
    }
    return 0;
}

int RND_bitArrayToggle(RND_BitArray *bitarray, size_t index)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    if (index >= bitarray->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 1;
    }
    bool newval = !(bitarray->bits[index / 8] & (0x80 >> (index % 8)));
    uint8_t *field = bitarray->bits + (index / 8);
    *field = newval? (*field | (0x80 >> (index % 8))) : (*field & ~(0x80 >> (index % 8)));
    return 0;
}

int RND_bitArrayDestroy(RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_WARN("the bitarray does not exist");
        return 1;
    }
    free(bitarray->bits);
    free(bitarray);
    return 0;
}

size_t RND_bitArraySize(const RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 0;
    }
    return bitarray->size;
}

int RND_bitArrayPrint(const RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    int linecount = 0;
    int bitcount  = bitarray->size;
    for (size_t i = 0; i < (bitarray->size + 7) / 8; i++) {
        int jmax = (bitcount > 8)? 8 : bitcount;
        for (int j = 0; j < jmax; j++, bitcount--) {
            printf("%d", !!(bitarray->bits[i] & (0x80 >> j)));
        }
        printf(" ");
        if (++linecount % 4 == 0)
            putc('\n', stdout);
    }
    if (bitarray->size % 32)
        putc('\n', stdout);

    return 0;
}

int RND_bitArrayAnd(RND_BitArray *dest, const RND_BitArray *src)
{
    if (!dest) {
        RND_ERROR("the destination bitarray does not exist");
        return 1;
    }
    if (!src) {
        return 0;
    }
    size_t i = dest->size,
           j = src->size;
    bool jstop = false;
    if (i % 8 == 0 && j % 8 == 0) {
        i = (i + 7) / 8, j = (j + 7) / 8;
        while (i-- > 0) {
            if (!jstop && j-- > 0) {
                dest->bits[i] &= src->bits[j];
            } else {
                jstop = true;
                dest->bits[i] = 0x00;
            }
        }
    } else {
        while (i-- > 0) {
            uint8_t *field = dest->bits + (i / 8);
            bool val1 = (*field & (0x80 >> (i % 8)));
            if (!jstop && j-- > 0 && val1) {
                bool val2 = (src->bits[j / 8] & (0x80 >> (j % 8)));
                *field = (val1 && val2)? *field : (*field & ~(0x80 >> (i % 8)));
            } else {
                jstop = true;
                *field &= ~(0x80 >> (i % 8));
            }
        }
    }
    
    return 0;
}

int RND_bitArrayOr(RND_BitArray *dest, const RND_BitArray *src)
{
    if (!dest) {
        RND_ERROR("the destination bitarray does not exist");
        return 1;
    }
    if (!src) {
        return 0;
    }
    size_t i = dest->size,
           j = src->size;
    if (i % 8 == 0 && j % 8 == 0) {
        i = (i + 7) / 8, j = (j + 7) / 8;
        while (i-- > 0 && j-- > 0) {
            dest->bits[i] |= src->bits[j];
        }
    } else {
        while (i-- > 0 && j-- > 0) {
            uint8_t *field = dest->bits + (i / 8);
            bool val1 = (*field & (0x80 >> (i % 8))),
                 val2 = (src->bits[j / 8] & (0x80 >> (j % 8)));
            *field = (val1 || val2)? (*field | (0x80 >> (i % 8))) : *field;
        }
    }
    
    return 0;
}

int RND_bitArrayXor(RND_BitArray *dest, const RND_BitArray *src)
{
    if (!dest) {
        RND_ERROR("the destination bitarray does not exist");
        return 1;
    }
    if (!src) {
        return 0;
    }
    size_t i = dest->size,
           j = src->size;
    if (i % 8 == 0 && j % 8 == 0) {
        i = (i + 7) / 8, j = (j + 7) / 8;
        while (i-- > 0 && j-- > 0) {
            dest->bits[i] ^= src->bits[j];
        }
    } else {
        while (i-- > 0 && j-- > 0) {
            uint8_t *field = dest->bits + (i / 8);
            bool val1 = (*field & (0x80 >> (i % 8))),
                 val2 = (src->bits[j / 8] & (0x80 >> (j % 8)));
            *field = ((val1 && !val2) || (!val1 && val2))? (*field | (0x80 >> (i % 8))) : (*field & ~(0x80 >> (i % 8)));
        }
    }
    
    return 0;
}

int RND_bitArrayNegate(RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    for (size_t i = 0; i < (bitarray->size + 7) / 8; i++) {
        bitarray->bits[i] = ~bitarray->bits[i];
    }
    return 0;
}

RND_BitArray *RND_bitArrayCopy(const RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return NULL;
    }
    RND_BitArray *new;
    new = RND_bitArrayCreate(bitarray->size);
    if (new == NULL) {
        RND_ERROR("RND_bitArrayCreate returned NULL");
        return NULL;
    }
    for (size_t i = 0; i < (bitarray->size + 7) / 8; i++) {
        new->bits[i] = bitarray->bits[i];
    }
    return new;
}
