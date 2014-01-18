/* @file ring.h
 * @brief interface of a ring.
 *
 * Description cited from "C Interfaces and Implementations":
 * A ring is much like a sequence: It holds N values associated with the
 * inteter indices zero through N-1 when N is positive. An empty ring holds no
 * values. Values are pointers. Like the values in a sequence, values in a
 * ring may be accessed by indexing.
 * Unlike a sequence, however, values can be added to a ring ANYWHERE and any
 * value in a ring can be removed. In addition, the values can be renumbered:
 * "rotating" a ring left decrements the index of each values by one modulo
 * the length of the ring length; rotating it right increments the indices by
 * one modulo the ring length. The price for the flexibility of adding values
 * to and removing values from arbitrary locations in a ring is that accessing
 * the ith value is not guaranteed to take constant time.
 *
 * @ note that the negative index is counted backwards, just like below:
 *
 *    0     1     2     3     4
 *    |     |     |     |     |
 *    v     v     v     v     v
 * [  0  |  1  |  2  |  3  |  4  ]
 *    ^     ^     ^     ^     ^
 *    |     |     |     |     |
 *   -5    -4    -3    -2    -1
 * 
 */

#ifndef RING_H
#define RING_H

#define T ring_t
typedef struct T *T;

/* exported functions */

/** @brief create a new ring */
extern T ring_new(void);

/** @brief build a new ring give a series of void * pointers 
 * @note that the last one should be NULL. */
extern T ring_ring(void *x, ...);

/** @brief free an existing ring 
 * @note that the stored pointers will not be handled(freed or others). */
extern void ring_free(T *ring);

/** @brief return the number of elements stored in a ring */
extern int ring_length(T ring);

/** @brief return the idx-th element in a ring 
 * @note that the index starts from zero(0) */
extern void *ring_get(T ring, int idx);

/** @brief set the idx-th slot in a ring to *x*
 * @return the previously stored value.
 */
extern void *ring_set(T ring, int idx, void *x);

/** @brief, add *x* before the idx-th element */
extern void *ring_add_before(T ring, int idx, void *x);

/** @breif, add *x* after the idx-th element */
extern void *ring_add_after(T ring, int idx, void *);

/** @brief add a new item *x* into the low end of a ring */
extern void *ring_addlo(T ring, void *x);

/** @brief add a new item *x* into the high end of a ring */
extern void *ring_addhi(T ring, void *x);

/** @brief removes and returns the idx-th value in *ring* */
extern void *ring_remove(T ring, int idx);

/** @brief removes ane return the value at the low end of *ring* */
extern void *ring_remlo(T ring);

/** @brief removes ane return the value at the high end of *ring* */
extern void *ring_remhi(T ring);

/** @brief rotate a ring
 * if *n* is positive, *ring* is rotated to the right -- clockwise
 * if *n* is negative, *ring* is rotated to the left -- counterclockwise
 * n = 1;  [0 1 2 3 4] => [4 0 1 2 3]
 * n = -1; [0 1 2 3 4] => [1 2 3 4 0]
 */
extern void ring_rotate(T ring, int n);

#undef T
#endif /* end of include guard: RING_H */
