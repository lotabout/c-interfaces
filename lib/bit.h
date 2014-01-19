/** @file bit.h
 * @brief interface of bit vector
 */

#ifndef BIT_H
#define BIT_H

#include <stdbool.h>

#define T bit_t

typedef struct T *T;

/** @brief create a new bit vector with size *length* */
extern T bit_new(int length);

/** @brief return the number of bits of a bit vector */
extern int bit_length(T set);

/** @brief returns the number of ones in *set**/
extern int bit_count(T set);

/** @brief destroy a bit vector */
extern void bit_free(T *set);

/** @brief get the n-th bit of *set* */
extern int bit_get(T set, int n);

/** @brief set the n-th bit of set to *bit*(0 or 1) 
 * @return the previous value stored */
extern int bit_put(T set, int n, int bit);

/** @brief clear the bits between [lo, hi], *lo* and *hi* are included */
extern void bit_clear(T set, int lo, int hi);

/** @brief set the bits between [lo, hi], *lo* and *hi* are included */
extern void bit_set(T set, int lo, int hi);

/** @brief inverts the bits between [lo, hi], *lo* and *hi* are included */
extern void bit_not(T set, int lo, int hi);

/** @brief return true if *s* is in *t* and s != t, false otherwise */
extern bool bit_lt(T s, T t);

/** @brief return true if *s* is equal to *t*, that means they contains the
 * same elements */
extern bool bit_eq(T s, T t);

/** @brief return true if *s* is in *t* or *s* is equal to *t* */
extern bool bit_leq(T s, T t);

/** @brief apply a function to a set of bits 
 * @note the parameters of apply: *n* is the bit number, *bit* is the value of
 * bit, *cl* is the client-specified pointer. */
extern void bit_map(T set, void apply(int n, int bit, void *cl), void *cl);

extern T bit_union(T s, T t);
extern T bit_inter(T s, T t);
extern T bit_minus(T s, T t);
extern T bit_diff(T s, T t);


#undef T
#endif /* end of include guard: BIT_H */
