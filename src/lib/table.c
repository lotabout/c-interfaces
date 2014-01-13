/* implementation of *Table*
 */
#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include "table.h"
#include "mem.h"

#include "dbg.h"

#define T table_t

struct T {
    struct binding {
        struct binding *link;
        const void *key;
        void *value;
    } **buckets;
    int size;
    int length;
    unsigned timestamp;     /* used in table_map to indicate that table should
                               not change while doing *map* */
    int (*cmp)(const void *a, const void *b);
    unsigned (*hash)(const void *key);
};

static unsigned default_hash(const void *key)
{
    return (unsigned long)key >> 2;
}

static int default_cmp(const void *a, const void *b)
{
    return a != b;
}

T table_new(int hint, 
            int cmp(const void *a, const void *b), 
            unsigned hash(const void *key))
{
    T table;
    int i;
    static int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771,
        65521, INT_MAX};

    assert(hint >= 0);

    for (i=1; primes[i] < hint; i++) {
        /* pass */
    }
    table = (T) zalloc(sizeof(*table) + primes[i-1]*sizeof(table->buckets[0]));
    table->size = primes[i-1];
    table->cmp = cmp ? cmp: default_cmp;
    table->hash = hash ? hash: default_hash;
    table->buckets = (struct binding **) (table+1);

    /* initialize */
    for (i = 0; i < table->size; i++) {
        table->buckets[i] = NULL;
    }
    table->length = 0;
    table->timestamp = 0;

    return table;
}

void *table_get(T table, const void *key)
{
    int hash_val;
    struct binding *p;
    assert(table);
    assert(key);

    /* search the table for the given key */
    hash_val = (*table->hash)(key) % table->size;
    for (p = table->buckets[hash_val]; p; p = p->link) {
        if ((*table->cmp)(key, p->key) == 0) {
            break;
        }
    }

    return p ? p->value: NULL;
}

void *table_put(T table, const void *key, void *value)
{
    int hash_val;
    struct binding *p;
    void *prev;

    assert(table);
    assert(key);

    /* search table for key */
    hash_val = (*table->hash)(key) % table->size;
    for (p = table->buckets[hash_val]; p; p = p->link) {
        if ((*table->cmp)(key, p->key) == 0) {
            break;
        }
    }

    if (p == NULL) {
        p = (struct binding *)zalloc(sizeof(*p));
        p->key = key;
        p->link = table->buckets[hash_val];
        table->buckets[hash_val] = p;
        table->length ++;
        prev = NULL;
    } else {
        prev = p->value;
    }

    /* assign new value */
    p->value = value;
    table->timestamp ++;
    return prev;
}

int table_length(T table)
{
    assert(table);
    return table->length;
}

void table_map(T table, 
               void apply(const void *key, void **value, void *cl),
               void *cl)
{
    int i;
    unsigned timestamp;
    struct binding *p;

    assert(table);
    assert(apply);
    timestamp = table->timestamp;

    for (i = 0; i < table->size; i++) {
        for (p=table->buckets[i]; p; p = p->link) {
            assert(table->timestamp == timestamp);
            apply(p->key, &p->value, cl);
        }
    }
}

void *table_remove(T table, const void *key)
{
    int hash_val;
    struct binding **pp;
    void *value;

    assert(table);
    assert(key);
    table->timestamp ++;

    hash_val = (*table->hash)(key) % table->size;

    for (pp=&table->buckets[hash_val]; *pp; pp = &(*pp)->link) {
        if ((*table->cmp)(key, (*pp)->key) == 0) {
            struct binding *p = *pp;
            value = p->value;
            *pp = p->link;
            zfree(p);

            table->length --;
            return value;
        }
    }

    return NULL;
}

void **table_to_array(T table, void *end)
{
    int i,j=0;
    void **array;
    struct binding *p;

    assert(table);
    array = (void **)zalloc((2*table->length + 1) * sizeof (*array));
    for (i = 0; i < table->size; i++) {
        for (p=table->buckets[i]; p; p=p->link) {
            array[j++] = (void *)p->key;
            array[j++] = (void *)p->value;
        }
    }
    array[j] = end;

    return array;
}

void table_free(T *table, void (*destroy)(const void *key, void *data))
{
    assert(table && *table);
    if ((*table)->length > 0) {
        int i;
        struct binding *p, *q;
        for (i=0; i< (*table)->size; i++) {
            for (p=(*table)->buckets[i]; p; p=q) {
                q = p->link;
                if (destroy != NULL) {
                    destroy(p->key, p->value);
                }
                zfree(p);
            }
        }
    }
    zfree(*table);
    *table = NULL;
}

extern void print_table(T table)
{
    assert(table);

    int i;
    struct binding *p;
    for (i = 0; i < table->size; i++) {
        p = table->buckets[i];
        if (p == NULL) {
            continue;
        }

        log_info("table[%d] contains: \n", i);
        for (; p; p=p->link) {
            log_info("\t(key = '%p', \tval= '%p'\n", p->key, p->value);
        }
    }
}
