/** @file array.h
 * @brief Interfaces for dynamic array 
 */
#ifndef ARRAY_H
#define ARRAY_H

#define T array_t
typedef struct T *T;

/* exported functions */

/** @brief create a new array
 * @param length the number of elements of the new array
 * @param size the size of one element
 * @return a new array
 */
extern T array_new(int nmemb, int size);

/** @brief free the array */
extern void array_free(T *array);

/** @brief return the number of elements in an array */
extern int array_length(T array);
/** @brief return the size of a single element stored in the array */
extern int array_size(T array);

/* @brief get the idx-th value of a array
 * @param array
 * @idx the index to get, range in [0, array_length - 1]
 * @return the *idx*-th element.
 */
extern void *array_get(T array, int idx);

/* @brief insert a new element into the *idx*-th position
 * @param array
 * @param idx, the index into which we insert the *elem*
 * @param elem the element to be inserted
 * @return the third parameter, i.e. *elem*
 */
extern void *array_put(T array, int idx, void *elem);

/** @brief adjust the array to a new size, member->size as the unit */
extern void array_resize(T array, int nmemb);

/** @brief copy [0, nmemb-1] of array to a new array */
extern T array_copy(T array, int nmemb);
#undef T
#endif /* end of include guard: ARRAY_H */
