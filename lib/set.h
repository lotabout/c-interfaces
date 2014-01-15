/* Provide *set* related interface */
#ifndef SET_H
#define SET_H

#include <stdbool.h>

#define T set_t
typedef struct T *T;

/* exported functions */

/** @brief create a new set
 * @param hint the size of the set
 * @param cmp the function used to compare two set elements.
 * @param hash hash function used to generate values for set elemtns.
 * @return a new set.
 */
extern T set_new(int hint,
                 int cmp(const void *a, const void *b),
                 unsigned hash(const void *x));

/** @brief destroy a set
 * @param set a pointer to the set.
 * @param destroy the function to destroy a single element, set to NULL to
 * skip destroy the stored data.
 * @return void
 */
extern void set_free(T *set, void destroy(void *));

/** @brief return the number of elements in the set
 * @param set the set
 * @return the number of elements in the set.
 */
extern int set_length(T set);

/** @brief test if an item is a member of a set
 * @param set the set to be tested
 * @param member the member to be tested
 * @return true if *member* is in *set*, false otherwise.
 */
extern bool set_member(T set, const void *member);

/** @brief add a new item to the set
 * @param set the set to be added into
 * @param member the item to be added.
 * @return void
 */
extern void set_put(T set, const void *member);

/** @brief remove *member* from the set
 * @param set the set to be removed from
 * @param member the item to be removed
 * @return the removed item, NULL if item not found
 */
extern void *set_remove(T set, const void *member);

/** @brief set_map is just like table_map, it will apply a function over each
 * elements of a set.
 * @param set the set to be mapped.
 * @param apply the function to apply
 * @param cl client specific pointer
 * @return void
 */
extern void set_map(T set,
                    void apply(const void *member, void *cl),
                    void *cl);

/** @brief convert a set into an array
 * @param set the set to be convertted
 * @param end the element to be added to the end of the generated array
 * @return an array converted from a set
 */
extern void **set_to_array(T set, void *end);

/***********************************************************************
 * set operations
 ***********************************************************************/
extern T set_union(T s, T t);
extern T set_inter(T s, T t);
extern T set_minus(T s, T t);
extern T set_diff(T s, T t);

#undef T
#endif /* end of include guard: SET_H */
