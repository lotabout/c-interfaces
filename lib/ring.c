/* @file ring.c
 * @brief The implementations of library *ring*
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "ring.h"
#include "mem.h"

#define T ring_t

/* we represent a ring in a circular doubly linked list */
struct T {
    struct node {
        struct node *next;
        struct node *prev;
        void *value;
    } head;
    int length;
};

T ring_new(void)
{
    T ring;
    ring = (T)zcalloc(1, sizeof(*ring));
    ring->head.next = &ring->head;
    ring->head.prev = &ring->head;
    ring->head.value = NULL;
    ring->length = 0;

    return ring;
}

T ring_ring(void *x, ...)
{
    va_list ap;
    T ring = ring_new();

    va_start(ap, x);

    for (; x; x = va_arg(ap, void *)) {
        ring_addhi(ring, x);
    }
    va_end(ap);

    return ring;
}

void ring_free(T *ring)
{
    struct node *p;
    struct node *q;
    struct node *head = &((*ring)->head);

    assert(ring && *ring);
    /* don't try to maintain list properties(doubly linked) because they are
     * to be destroyed */
    for (p=head->next; p != head; p = q) {
        q = p->next;
        zfree(p);
    }

    free(*ring);
    *ring = NULL;
}

int ring_length(T ring)
{
    assert(ring);
    return ring->length;
}

static struct node *ring_get_node(T ring, int idx)
{
    struct node *p;
    struct node *head = &(ring->head);
    assert(ring);
    assert(idx >= (-ring->length) && idx < ring->length);

    idx = (idx + ring->length) % ring->length;

    /* p <- ith node */
    int n;
    /* if idx > length/2 then search reversely */
    if (idx <= ring->length/ 2) {
        n = idx;
        p = head->next;
        while (n-- > 0) {
            p = p->next;
        }
    } else {
        n = ring->length - idx - 1;
        p = head->prev;
        while (n-- > 0) {
            p = p->prev;
        }
    }

    return p;
}

void *ring_get(T ring, int idx)
{
    struct node *p;

    assert(ring);
    assert(idx >= (-ring->length) && idx < ring->length);

    p = ring_get_node(ring, idx);

    return p->value;
}

void *ring_set(T ring, int idx, void *x)
{
    struct node *p;
    void *old_val;

    assert(ring);
    assert(idx >= (-ring->length) && idx < ring->length);

    p = ring_get_node(ring, idx);

    old_val = p->value;
    p->value = x;
    return old_val;
    
}


/* insert a *new* entry after the specified node */
static inline void node_add_after(struct node *new, struct node *n)
{
    new->next = n->next;
    new->prev = n;
    n->next->prev = new;
    n->next = new;
}

/* insert a *new* entry before the specified node */
static inline void node_add_before(struct node *new, struct node *n)
{
    new->next = n;
    new->prev = n->prev;
    n->prev->next = new;
    n->prev = new;
}

/* remove a node [...prev p next ...] => [...prev next...]*/
static inline void node_del(struct node *p)
{
    p->next->prev = p->prev;
    p->prev->next = p->next;
}

void *ring_add_before(T ring, int idx, void *x)
{
    assert(ring);
    assert(idx >= (-ring->length) && idx < ring->length);

    struct node *p;
    p = (struct node *)zalloc(sizeof(*p));
    p->value = x;

    struct node *old;
    old = ring_get_node(ring, idx);
    node_add_before(p, old);

    ring->length ++;
    return x;
}

void *ring_add_after(T ring, int idx, void *x)
{
    assert(ring);
    assert(idx >= (-ring->length) && idx < ring->length);

    struct node *p;
    p = (struct node *)zalloc(sizeof(*p));
    p->value = x;

    struct node *old;
    old = ring_get_node(ring, idx);
    node_add_after(p, old);

    ring->length ++;
    return x;
}

void *ring_addlo(T ring, void *x)
{
    struct node *p;
    struct node *head = &(ring->head);

    assert(ring);
    p = (struct node *)zalloc(sizeof(*p));
    p->value = x;

    /* insert the new element into the last pos in a ring */
    node_add_after(p, head);

    ring->length++;
    return x;
}

void *ring_addhi(T ring, void *x)
{
    struct node *p;
    struct node *head = &(ring->head);

    assert(ring);
    p = (struct node *)zalloc(sizeof(*p));
    p->value = x;

    /* insert the new element into the last pos in a ring */
    node_add_before(p, head);

    ring->length++;
    return x;
}

void *ring_remove(T ring, int idx)
{
    assert(ring);
    assert(ring->length > 0);
    assert(idx >= (-ring->length) && idx < ring->length);

    void *x;
    struct node *p;
    p = ring_get_node(ring, idx);
    x = p->value;
    node_del(p);
    zfree(p);

    ring->length --;
    return x;
}

void *ring_remlo(T ring)
{
    assert(ring);
    assert(ring->length > 0);
    void *x;
    struct node *p;

    p = ring->head.next;
    x = p->value;

    node_del(p);
    zfree(p);

    ring->length --;
    return x;
}

void *ring_remhi(T ring)
{
    assert(ring);
    assert(ring->length > 0);
    void *x;
    struct node *p;

    p = ring->head.prev;
    x = p->value;

    node_del(p);
    zfree(p);

    ring->length --;
    return x;
}

void ring_rotate(T ring, int n)
{
    assert(ring);
    int idx = n % (ring->length);
    if (idx < 0) {
        idx += ring->length;
    }

    struct node *head = &ring->head;
    struct node *p;

    p = ring_get_node(ring, -idx);  /* seek backwards */
    node_del(head);
    node_add_before(head, p);

    return;
}
