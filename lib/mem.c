#include <stdlib.h>
#include <assert.h>
#include "mem.h"

void *zalloc(size_t size)
{
    void *p = malloc(size);
    assert(p);
    return p;
}

void zfree(void *ptr)
{
    assert(ptr);
    free(ptr);
}

void *zcalloc(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size);
    assert(p);
    return p;
}
