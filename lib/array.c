/** @file array.c
 * @breif implementations of dynamic array
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"
#include "mem.h"

#define T array_t

struct T {
    int nmemb;
    int size;
    char *array;
};

T array_new(int nmemb, int size)
{
    assert(nmemb >= 0 && size > 0);
    T array;
    array = (T)zcalloc(nmemb, size);
    array->nmemb = nmemb;
    array->size = size;
    array->array = (nmemb > 0) ? (zcalloc(nmemb, size)) : NULL;

    return array;
}

void array_free(T *array)
{
    assert(array && *array);
    zfree((*array)->array);
    zfree(*array);
    *array = NULL;
}

void *array_get(T array, int idx)
{
    assert(array);
    assert(idx >= 0 && idx < array->nmemb);

    return array->array + idx * array->size;
}

void *array_put(T array, int idx, void *elem)
{
    assert(array);
    assert(idx >= 0 && idx < array->nmemb);
    assert(elem);

    memcpy(array->array + idx*array->size, elem, array->size);

    return elem;
}

int array_length(T array)
{
    assert(array);
    return array->nmemb;
}

int array_size(T array)
{
    assert(array);
    return array->size;
}

static inline int min(int a, int b)
{
    return (a < b) ? a : b;
}


void array_resize(T array, int nmemb)
{
    assert(array);
    assert(nmemb >= 0);

    if (nmemb == 0) {
        zfree(array->array);
        array->array = NULL;
    } else if (array->nmemb == 0) {
        array->array =(char *)zalloc(nmemb * array->size);
    } else {
        void *p = zcalloc(nmemb, array->size);
        memcpy(p, array->array, min(array->nmemb, nmemb) * array->size);
        zfree(array->array);
        array->array = (char *)p;
    }

    array->nmemb = nmemb;
}

T array_copy(T array, int nmemb)
{
    T copy;
    assert(array);
    assert(nmemb >= 0);

    copy = array_new(nmemb, array->size);
    memcpy(copy->array, array->array, 
           min(copy->nmemb, array->nmemb) * array->size);

    return copy;
}
