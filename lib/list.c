/* Implementations of list ADT
 */

#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include "mem.h"
#include "list.h"
#include "dbg.h"

list_t *list_push(list_t *list, void *x)
{
    list_t *p;
    p = (list_t *)zalloc(sizeof(*p));
    p->first = x;
    p->rest = list;

    return p;
}

list_t *list_list(void *x, ...)
{
    va_list ap;
    list_t *list;
    list_t **p = &list;

    va_start(ap, x);
    /* the usage of pointer to pointer is so clever!! */
    for (; x; x = va_arg(ap, void *)) {
        *p = (list_t *)zalloc(sizeof(**p));
        (*p)->first = x;
        p = &(*p)->rest;
    }
    *p = NULL;
    va_end(ap);

    return list;
}

list_t *list_append(list_t *list, list_t *tail)
{
    list_t **p = &list;

    /* travel to the end of *list* */
    while (*p) {
        p = &(*p)->rest;
    }
    *p = tail;

    return list;
}

list_t *list_copy(list_t *list)
{
    list_t *head;
    list_t **p = &head;

    for (; list; list = list->rest) {
        *p = (list_t *)zalloc(sizeof(**p));
        (*p)->first = list->first;
        p = &(*p)->rest;
    }

    *p = NULL;

    return head;
}

list_t *list_pop(list_t *list, void **x)
{
    if (list == NULL) {
        return NULL;
    }

    list_t *head = list->rest;
    if (x != NULL) {
        *x = list->first;

        zfree(list);
        return head;
    }

    return NULL;    /* normally, this should not happend. */
}

list_t *list_reverse(list_t *list)
{
    list_t *head = NULL;
    list_t *next;

    for (; list; list = next) {
        next = list->rest;
        list->rest = head;
        head = list;
    }

    return head;
}

int list_length(list_t *list)
{
    int n;
    for (n=0; list; list = list->rest) {
        n ++;
    }

    return n;
}

void list_free(list_t **list, void (*destroy)(void *))
{
    list_t *next;

    assert(list);
    for (; *list; *list = next) {
        next = (*list)->rest;
        if (destroy != NULL) {
            destroy((*list)->first);
        }
        zfree(*list);
    }

    *list = NULL;
}

void list_map(list_t *list, void apply(void **x, void *cl), void *cl)
{
    assert(apply);
    for (; list; list = list->rest) {
        apply(&list->first, cl);
    }
}

void **list_to_array(list_t *list, void *end)
{
    int i, n = list_length(list);
    void **array = (void **)zalloc((n+1) * sizeof(*array));

    for (i = 0; i < n; i++) {
        array[i] = list->first;
        list = list->rest;
    }

    array[i] = end;

    return array;
}
