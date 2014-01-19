/** @file bit.c
 * @brief the implementations of library bit vector
 */

#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "bit.h"
#include "mem.h"

#define T bit_t

/* note that *bytes* and *words* points to the same place */
struct T {
    int length;
    unsigned char *bytes;
    unsigned long *words;
};

/* macros */

/* bits per word */
#define BPW (8 * sizeof(unsigned long))
#define nwords(len) ((((len) + BPW-1)&(~(BPW-1)))/BPW)

#define nbytes(len) ((((len) + 8-1)&(~(8-1)))/8)

/* static data */
unsigned char msbmask[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
unsigned char lsbmask[] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

T bit_new(int length)
{
    T set;

    assert(length > 0);
    set = (T)zalloc(sizeof(*set));
    if (length > 0) {
        set->words = zcalloc(nwords(length), sizeof(unsigned long));
    } else {
        set->words = NULL;
    }

    set->bytes = (unsigned char *)set->words;
    set->length = length;

    return set;
}

void bit_free(T *set)
{
    assert(set && *set);
    zfree((*set)->words);
    zfree(*set);
    *set = NULL;
}

int bit_length(T set)
{
    assert(set);

    return set->length;
}

/* member operations */

int bit_count(T set)
{
    int length = 0;
    int i;
    static char count[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

    assert(set);
    for (i = nbytes(set->length); --i > 0; ) {
        unsigned char c = set->bytes[i];
        length += count[c & 0x0F] + count[(c >> 4) & 0x0F];
    }

    return length;
}

int bit_get(T set, int n)
{
    assert(set);
    assert(0 <= n && n < set->length);

    return (((set->bytes[n/8]) >> (n % 8)) & 0x01);
}

int bit_put(T set, int n, int bit)
{
    int prev;

    assert(set);
    assert(0 <= n && n < set->length);
    assert(bit == 0 || bit == 1);

    prev = (((set->bytes[n/8]) >> (n % 8)) & 0x01);

    if (bit == 1) {
        set->bytes[n/8] |= 1 << (n%8);
    } else {
        set->bytes[n/8] &= ~(1 << (n%8));
    }

    return prev;
}

void bit_set(T set, int lo, int hi)
{
    assert(set);
    assert(0 <= lo && lo <= hi && hi < set->length);

    if (lo/8 < hi/8) {
        /* means that [lo, hi] is across two bytes
         * +--------+--------+
         * |   (XXXX|XXXXX)  |
         * +--------+--------+
         *     `lo        `hi
         * */
        /* 1. set the most significant bits in bytes lo/8 */
        set->bytes[lo/8] |= msbmask[lo%8];

        /* 2. set all bits in byts [lo/8 + 1, hi/8 - 1] */
        int i;
        for (i=lo/8+1; i<hi/8; i++) {
            set->bytes[i] |= 0xFF;
        }

        /* 3. set the least significant bits in bytes hi/8 */
        set->bytes[hi/8] |= lsbmask[hi%8];
    } else {
        /* set bits [lo%8..hi%8] in byte lo/8 */
        set->bytes[lo/8] |= (msbmask[lo%8] & lsbmask[hi%8]);
    }
}

void bit_clear(T set, int lo, int hi)
{
    assert(set);
    assert(0 <= lo && lo <= hi && hi < set->length);

    if (lo/8 < hi/8) {
        /* means that [lo, hi] is across two bytes
         * +--------+--------+
         * |   (XXXX|XXXXX)  |
         * +--------+--------+
         *     `lo        `hi
         * */
        /* 1. clear the most significant bits in bytes lo/8 */
        set->bytes[lo/8] &= ~msbmask[lo%8];

        /* 2. clear all bits in byts [lo/8 + 1, hi/8 - 1] */
        int i;
        for (i=lo/8+1; i<hi/8; i++) {
            set->bytes[i] = 0x00;
        }

        /* 3. clear the least significant bits in bytes hi/8 */
        set->bytes[hi/8] &= ~lsbmask[hi%8];
    } else {
        /* clear bits [lo%8..hi%8] in byte lo/8 */
        set->bytes[lo/8] &= ~(msbmask[lo%8] & lsbmask[hi%8]);
    }
}

void bit_not(T set, int lo, int hi)
{
    assert(set);
    assert(0 <= lo && lo <= hi && hi < set->length);

    if (lo/8 < hi/8) {
        /* means that [lo, hi] is across two bytes
         * +--------+--------+
         * |   (XXXX|XXXXX)  |
         * +--------+--------+
         *     `lo        `hi
         * */
        /* 1. reverse the most significant bits in bytes lo/8 */
        set->bytes[lo/8] ^= msbmask[lo%8];

        /* 2. reverse all bits in byts [lo/8 + 1, hi/8 - 1] */
        int i;
        for (i=lo/8+1; i<hi/8; i++) {
            set->bytes[i] ^= 0xFF;
        }

        /* 3. reverse the least significant bits in bytes hi/8 */
        set->bytes[hi/8] ^= lsbmask[hi%8];
    } else {
        /* reverse bits [lo%8..hi%8] in byte lo/8 */
        set->bytes[lo/8] ^= (msbmask[lo%8] & lsbmask[hi%8]);
    }
}

void bit_map(T set, void apply(int n, int bit, void *cl), void *cl)
{
    assert(set);
    int i;
    for (i = 0; i < set->length; i++) {
        apply(i, (((set->bytes[i/8]) >> (i % 8)) & 0x01), cl);
    }
}

/* comparisons */
bool bit_lt(T s, T t)
{
    assert(s && t);
    assert(s->length == t->length);

    int i, lt = 0;
    for (i = nwords(s->length); --i >= 0; ) {
        if ((s->words[i] & ~(t->words[i])) != 0) {
            return 0;
        } else if (s->words[i] != t->words[i]) {
            lt |= 1;
        }
    }

    return lt;
}

bool bit_eq(T s, T t)
{
    assert(s && t);
    assert(s->length == t->length);

    int i;
    for (i = nwords(s->length); --i >= 0; ) {
        if (s->words[i] != t->words[i]) {
            return 0;
        }
    }

    return 1;
}

bool bit_leq(T s, T t)
{
    assert(s && t);
    assert(s->length == t->length);

    int i;
    for (i = nwords(s->length); --i >= 0; ) {
        if ((s->words[i] & ~(t->words[i])) != 0) {
            return 0;
        }
    }

    return 1;
}

/* set operations */

/* copy a bit vector */
static T copy(T set)
{
    T new_set;

    assert(set);
    new_set = bit_new(set->length);
    if (set->length > 0) {
        memcpy(new_set->bytes, set->bytes, nbytes(set->length));
    }

    return new_set;
}

#define setop(sequal, snull, tnull, op) \
    if (s == t) {assert(s); return sequal;} \
    else if (s == NULL) { assert(t); return snull; } \
    else if (t == NULL) { assert(s); return tnull; } \
    else { \
        int i; T set; \
        assert(s->length == t->length); \
        set = bit_new(s->length); \
        for (i=nwords(s->length); --i >= 0; ) \
            set->words[i] = s->words[i] op t->words[i]; \
        return set; \
    }


T bit_union(T s, T t)
{
    setop(copy(t), copy(t), copy(s), |);
}

T bit_inter(T s, T t)
{
    setop(copy(t), bit_new(t->length), bit_new(s->length), &);
}

T bit_minus(T s, T t)
{
    setop(bit_new(s->length), bit_new(t->length), copy(s), & ~);
}

T bit_diff(T s, T t)
{
    setop(bit_new(s->length), copy(t), copy(s), ^);
}
