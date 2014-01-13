/*
 * Implemention of interfaces defined in "atom.h"
 */
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "atom.h"
#include "dbg.h"
#include "mem.h"

#define BUCKET_SIZE 2048

/* Atoms are stored in a hash table
 * +----+
 * |    |-->{node}->{node}-> ...
 * +----+
 * |    |
 * +----+
 * |    |-->{node}->{node}->...
 * +----+
 * | .  |
 * | .  |
 * +----+
 * */
static struct atom_t {
    struct atom_t *link;
    int len;
    char *str;
} *buckets[BUCKET_SIZE];

const char *atom_string(const char *str)
{
    assert(str);
    return atom_new(str, strlen(str));
}

const char *atom_int(long n)
{
    char str[43];
    char *s = str + sizeof(str);
    unsigned long m;

    if (n == LONG_MIN) {
        m = LONG_MAX + 1UL;
    } else if (n < 0) {
        m = -n;
    } else {
        m = n;
    }

    do {
        *--s = m % 10 + '0';
    } while ((m /= 10) > 0);
    
    if (n < 0) {
        *--s = '-';
    }

    return atom_new(s, sizeof(str) - (s-str));
}

/* hash function 
 * from: http://www.cse.yorku.ca/~oz/hash.html
 */
static inline unsigned long hash(const char *str, int len)
{
    unsigned long hash = 5381;
    int i;
    for (i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + str[i];   /* hash * 33 + c */
    }
    return hash;
}

const char *atom_new(const char *str, int len) 
{

    unsigned long hash_val = 0;
    int i;
    struct atom_t *p;
    int nelts = (sizeof(buckets)/sizeof(buckets[0]));

    assert(str);
    assert(len >= 0);
    hash_val = hash(str, len);
    hash_val %= nelts;

    /* search for an existing entry */
    for (p = buckets[hash_val]; p; p = p->link) {
        if (len == p->len) {
            for (i=0; i<len && p->str[i] == str[i]; i++) {
                /* pass */
            }
            if (i == len) {
                return p->str;
            }
        }
    }

    /* no existing entry 
     * allocate a new entry */
    p = zalloc(sizeof(*p) + len + 1);
    p->len = len;
    p->str = (char *)(p+1);
    if (len > 0) {
        memcpy(p->str, str, len);
    }
    p->str[len] = '\0';
    p->link = buckets[hash_val];
    buckets[hash_val] = p;

    return p->str;
}

int atom_length(const char *str)
{
    struct atom_t *p;
    int i;
    int nelts = (sizeof(buckets)/sizeof(buckets[0]));

    assert(str);
    for (i = 0; i < nelts; i++) {
        for (p = buckets[i]; p; p = p->link) {
            if (p->str == str) {
                return p->len;
            }
        }
    }

    log_err("atom_length: *str* is not an existing atom\n");
    assert(0);  /* indicates that str is not an existing atom */

    return 0;
}

void print_atom_table()
{
    struct atom_t *p;
    int i;
    int nelts = (sizeof(buckets)/sizeof(buckets[0]));

    for (i = 0; i < nelts; i++) {
        int j = 0;
        for (p = buckets[i]; p; p = p->link, j++) {
            log_info("buckets[%d][%d] = %s\n", i, j, p->str);
        }
    }
}
