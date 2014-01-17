/** @file seq.c
 * @brief implementation of library seq.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "seq.h"
#include "mem.h"

#include "dbg.h"

#define T seq_t

/* 
 * @note that in the book, seq_t contains a "struct array_t", here, I
 * re-implement the required array interface.
 *
 */
struct T {
    void **data;
    int nmemb;
    int length;
    int head;
};

T seq_new(int hint)
{
    T seq;

    assert(hint >= 0);
    seq = (T)zcalloc(1, sizeof(*seq));
    if (hint == 0) {
        hint = 16;
    }

    /* create a new array */
    seq->data = (void **)zcalloc(hint, sizeof(*seq->data));
    seq->nmemb = hint;
    seq->length = 0;
    seq->head = 0;

    return seq;
};

T seq_seq(void *x, ...)
{
    va_list ap;
    T seq = seq_new(0);

    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        seq_addhi(seq, x);
    }
    va_end(ap);

    return seq;
}

void seq_free(T *seq)
{
    assert(seq && *seq);
    zfree((*seq)->data);
    zfree(*seq);
    *seq = NULL;
}

int seq_length(T seq)
{
    assert(seq);
    return seq->length;
}

#define seq(i) (seq->data[(seq->head + (i)) % (seq->nmemb)])

void *seq_get(T seq, int idx)
{
    assert(seq);
    assert(idx >= 0 && idx < seq->length);

    return seq(idx);
}

void *seq_put(T seq, int idx, void *x)
{
    assert(seq);
    assert(idx >=0 && idx < seq->length);

    void *prev = seq(idx);
    seq(idx) = x;

    return prev;
}

void *seq_remlo(T seq)
{
    int idx = 0;
    void *x;
    assert(seq);
    assert(seq->length > 0);

    x = seq(idx); /* seq[0] */
    seq->head = (seq->head + 1) % seq->nmemb;
    --seq->length;

    return x;
}

void *seq_remhi(T seq)
{
    assert(seq);
    assert(seq->length > 0);
    int idx;
    idx = --seq->length;

    return seq(idx);
}

static void seq_expand(T seq)
{
    /* alloc new memory for seq */
    int new_size = seq->nmemb * 2;
    void **new = (void **)zcalloc(new_size, sizeof(*new));

    /* copy old entries */
    /* [0 ...... head ........ nmem-1] */
    memcpy(new, &seq->data[seq->head], (seq->nmemb-seq->head)*sizeof(*seq->data));
    memcpy(&new[seq->nmemb-seq->head], seq->data, seq->head * sizeof(*seq->data));

    zfree(seq->data);
    seq->data = new;
    seq->nmemb = new_size;
    seq->head = 0;
}

void *seq_addhi(T seq, void *x)
{
    assert(seq);
    if (seq->length == seq->nmemb) {
        seq_expand(seq);
    }
    int idx = seq->length ++;
    seq(idx) = x;
    return x;
}

void *seq_addlo(T seq, void *x)
{
    int idx = 0;
    assert(seq);

    if (seq->length == seq->nmemb) {
        seq_expand(seq);
    }
    if (--seq->head < 0) {
        seq->head = seq->nmemb - 1;
    }
    seq->length ++;
    seq(idx) = x;
    return x;
}
