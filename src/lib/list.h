/*
 * List is a sequence of zero or more pointers. A list with zero pointers is
 * an empty list. The number of pointers in a list is its length.
 *
 * This file is to provide an general list interface.
 */
#ifndef LIST_H
#define LIST_H

#include <stdarg.h>

typedef struct list_t {
    struct list_t *rest;
    void *first;
} list_t;

/** @brief: append a list to the tail of another list.
 * @param list: the head list.
 * @param tail: the list to be append;
 * @return, a new list with *tail* appended to *list*
 * @note: *list* will be modified.
 */
extern list_t *list_append(list_t *list, list_t *tail);

/** @brief: copy a list to another
 * @param list: the list to be copied
 * @return the generated new list.
 */
extern list_t *list_copy(list_t *list);

/** @brief: generate a new list using the given argusments.
 * @param x, ... the items to be used.
 * @return a new list containning the given arguments.
 * @note that you should add a *NULL* at the end of the list, i.e.
 * list_list(a, b, c, d, NULL)
 */
extern list_t *list_list(void *x, ...);

/** @brief: pop out one element from a list
 * @param list: the list to be opearted.
 * @param x [out]: contains the pointer to the popped out element.
 * @return the remaining list.
 */
extern list_t *list_pop(list_t *list, void **x);

/** @brief: push a new item into a list
 * @param list the list we want to operate on
 * @param x the pointer to the new item
 * @return the new list.
 */
extern list_t *list_push(list_t *list, void *x);

/** @brief: reverse a list
 * @param list: the list to be operated on
 * @return the reversed list.
 */
extern list_t *list_reverse(list_t *list);

/** @brief: return the length of a list
 * This will traverse the whole list.
 * @param list: the list whose length is requested.
 * @return the length of a list
 */
extern int list_length(list_t *list);

/** @brief: free a list
 * @param list: the list to be destroyed.
 * @param destroy: the function to destroy a single element.
 * @return void
 */
extern void list_free(list_t **list, void (*destroy)(void *));

/** @brief: map a function through a list
 * @param list: the list to be operated on
 * @param apply: the function to be apply.
 * @param cl: a client-specific pointer cl:
 * Note that it is (void **) that *apply* take as the first argument. This is
 * for cases where *apply* wants to modify the list elements, for example,
 * free the elements and set the pointer to NULL.
 */
extern void list_map(list_t *list, void apply(void **x, void *cl), void *cl);

/** @brief: convert a list to an array
 * It will allocates an N+1-element array to hold the pointers in an N-element
 * list, and copies the pointers into the array.
 * @param list: the list to be converted.
 * @param end: the N+1 element.
 * @return an array converted from the list.
 */
extern void **list_to_array(list_t *list, void *end);


#endif /* end of include guard: LIST_H */
