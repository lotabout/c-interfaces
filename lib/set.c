#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include "mem.h"
#include "set.h"
#include <dbg.h>
#include <stdio.h>

#define T set_t

struct T {
    int length;
    unsigned timestamp;
    int (*cmp)(const void *a, const void *b);
    unsigned (*hash)(const void *x);
    int size;
    struct member {
        struct member *link;
        const void *member;
    } **buckets;
};

static unsigned default_hash(const void *key)
{
    return (unsigned long)key >> 2;
}

static int default_cmp(const void *a, const void *b)
{
    return a != b;
}

T set_new(int hint, int cmp(const void *a, const void *b),
          unsigned hash(const void *x))
{
    T set;
    int i;
    static int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771,
        65521, INT_MAX};

    assert(hint >= 0);
    for (i = 1; primes[i] < hint; i++) {
        /* pass */
    }

    set = (T)zalloc(sizeof(*set) + primes[i-1]*sizeof(set->buckets[0]));
    set->size = primes[i-1];
    set->cmp = cmp ? cmp : default_cmp;
    set->hash = hash ? hash : default_hash;
    set->buckets = (struct member **)(set + 1);
    for (i = 0; i < set->size; i++) {
        set->buckets[i] = NULL;
    }
    set->length = 0;
    set->timestamp = 0;

    return set;
}

bool set_member(T set, const void *member)
{
    struct member *p = NULL;

    assert(set);
    assert(member);

    /* search for member */
    unsigned hash_val = 0;
    hash_val = (*set->hash)(member) % set->size;

    for (p = set->buckets[hash_val]; p; p = p->link) {
        if ((*set->cmp)(member, p->member) == 0) {
            break;
        }
    }

    return p != NULL;
}

void set_put(T set, const void *member)
{
    unsigned hash_val = 0;
    struct member *p = NULL;

    assert(set);
    assert(member);

    /* search for member */
    hash_val = (*set->hash)(member) % set->size;
    for (p = set->buckets[hash_val]; p; p = p->link) {
        if ((*set->cmp)(member, p->member) == 0) {
            break;
        }
    }

    if (p == NULL) {
        /* add member to set */
        p = (struct member *)zalloc(sizeof(*p));
        p->member = member;
        p->link = set->buckets[hash_val];
        set->buckets[hash_val] = p;
        set->length ++;
    } else {
        p->member = member;
    }
    set->timestamp ++;
}

void *set_remove(T set, const void *member)
{
    unsigned hash_val = 0;
    struct member **pp;

    assert(set);
    assert(member);

    set->timestamp ++;
    hash_val = (*set->hash)(member) % set->size;

    for (pp = &set->buckets[hash_val]; *pp; pp = &(*pp)->link) {
        if ((*set->cmp)(member, (*pp)->member) == 0) {
            struct member *p = *pp;
            *pp = p->link;
            member = p->member;
            zfree(p);
            set->length --;
            return (void *)member;
        }
    }

    return NULL;
}

int set_length(T set)
{
    assert(set);
    return set->length;
}

void set_free(T *set, void destroy(void *))
{
    assert(set && *set);
    if ((*set)->length > 0) {
        int i;
        struct member *p, *q;
        for (i = 0; i < (*set)->size; i++) {
            for (p=(*set)->buckets[i]; p; p = q) {
                q = p->link;
                if (destroy != NULL) {
                    destroy((void *)p->member);
                }
                zfree(p);
            }
        }
    }

    zfree(*set);
    *set = NULL;
}

void set_map(T set, void apply(const void *member, void *cl), void *cl)
{
    int i;
    struct member *p;

    assert(set);
    assert(apply);

    unsigned timestamp = set->timestamp;
    for (i = 0; i < set->size; i++) {
        for (p=set->buckets[i]; p; p=p->link) {
            apply(p->member, cl);
            assert(set->timestamp == timestamp);
        }
    }
}

void **set_to_array(T set, void *end)
{
    int i,j = 0;
    void **array;
    struct member *p;

    assert(set);
    array = (void **)zalloc((set->length + 1) * sizeof(*array));
    for (i = 0; i < set->size; i++) {
        for (p = set->buckets[i]; p; p = p->link) {
            array[j++] = (void *)p->member;
        }
    }
    array[j] = end;

    return array;
}

inline int max(int a, int b)
{
    return (a > b) ? a : b;
}

inline int min(int a, int b)
{
    return (a < b) ? a : b;
}

/** @brief clone a set
 */
static T copy(T t, int hint)
{
    T set;

    assert(t);
    set = set_new(hint, t->cmp, t->hash);

    /* for each member q in t, add it to set */
    int i;
    struct member *q;
    struct member *p;
    unsigned hash_val = 0;

    for (i = 0; i < t->size; i++) {
        for (q = t->buckets[i]; q; q = q->link) {
            p = (struct member *)zalloc(sizeof(*p));
            p->member = q->member;
            hash_val = set->hash(p->member) % set->size;

            p->link = set->buckets[hash_val];
            set->buckets[hash_val] = p;
            set->length ++;
        }
    }

    return set;
}

T set_union(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return copy(t, t->size);
    } else if (t == NULL) {
        assert(s);
        return copy(s, s->size);
    } else if (s->length < t->length) {
        return set_union(t, s);
    } else {
        T set = copy(s, max(s->size, t->size));
        assert(s->cmp == t->cmp && s->hash == t->hash);

        int i;
        struct member *q;
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                set_put(set, q->member);
            }
        }
        return set;
    }
}


T set_inter(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return set_new(t->size, t->cmp, t->hash);
    } else if (t == NULL) {
        assert(s);
        return set_new(s->size, s->cmp, s->hash);
    } else if (s->length < t->length) {
        return set_inter(t, s);
    } else {
        T set = set_new(min(s->size, t->size), s->cmp, s->hash);
        assert(s->cmp == t->cmp && s->hash == t->hash);

        /* for each member q in t, if q is in s, add it the new set */
        int i;
        struct member *q;
        struct member *p;
        unsigned hash_val;
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                if (set_member(s, q->member)) {
                    p = (struct member *)zalloc(sizeof(*p));
                    p->member = q->member;
                    hash_val = set->hash(p->member) % set->size;

                    p->link = set->buckets[hash_val];
                    set->buckets[hash_val] = p;
                    set->length ++;
                }
            }
        }
        return set;
    }
}

T set_minus(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return set_new(t->size, t->cmp, t->hash);
    } else if (t == NULL) {
        assert(s);
        return copy(s, s->size);
    } else {
        T set = set_new(s->size, s->cmp, s->hash);
        assert(s->cmp == t->cmp && s->hash == t->hash);

        /* for each member q in s, if it is not in t, add it to new set */
        int i;
        struct member *q;
        struct member *p;
        unsigned hash_val;
        for (i = 0; i < s->size; i++) {
            for (q = s->buckets[i]; q; q = q->link) {
                if (!set_member(t, q->member)) {
                    p = (struct member *)zalloc(sizeof(*p));
                    p->member = q->member;
                    hash_val = set->hash(p->member) % set->size;

                    p->link = set->buckets[hash_val];
                    set->buckets[hash_val] = p;
                    set->length ++;
                }
            }
        }
        return set;
    }

}

T set_diff(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return copy(t, t->size);
    } else if (t == NULL) {
        return copy(s, s->size);
    } else {
        T set = set_new(min(s->size, t->size), s->cmp, t->hash);
        assert(s->cmp == t->cmp && s->hash == t->hash);

        int i;
        struct member *p;
        struct member *q;
        unsigned hash_val = 0;

        /* for each member p in s, if p isn't in t, add it to new set */
        for (i = 0; i < s->size; i++) {
            for (p = s->buckets[i]; p; p = p->link) {
                if (! set_member(t, p->member)) {
                    q = (struct member *)zalloc(sizeof(*q));
                    q->member = p->member;
                    hash_val = set->hash(q->member) % set->size;
                    q->link = set->buckets[hash_val];
                    set->buckets[hash_val] = q;
                    set->length ++;
                }
            }
        }
        
        /* for each member p in t, if p isn't in s, add it to new set */
        T u = t;
        t = s;
        s = u;
        /* copy exactly from the paragraph above */
        for (i = 0; i < s->size; i++) {
            for (p = s->buckets[i]; p; p = p->link) {
                if (! set_member(t, p->member)) {
                    q = (struct member *)zalloc(sizeof(*q));
                    q->member = p->member;
                    hash_val = set->hash(q->member) % set->size;
                    q->link = set->buckets[hash_val];
                    set->buckets[hash_val] = q;
                    set->length ++;
                }
            }
        }

        return set;
    }
}
