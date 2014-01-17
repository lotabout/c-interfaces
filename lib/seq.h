/** @file seq.h
 * @brief interface for library seq
 * depends on dynamic *array*
 */

#ifndef SEQ_H
#define SEQ_H

#include <stdarg.h>

#define T seq_t
typedef struct T *T;

/* exported functions */

/** @brief create a new sequence with size *hint* */
extern T seq_new(int hint);

/** @brief use a sequence of void * pointers to build a new seq 
 * example: 
 * seq_t names;
 * names = seq_seq("C", "ML", "C++", "Icon", "AWK", NULL);
 * @note that the last one shoudl be NULL.
 * */
extern T seq_seq(void *x, ...);

/** @brief destroy a sequence structure 
 * @note the parameter type is (T *) instead of (T)
 * */
extern void seq_free(T *seq);

/** @brief return the number of elements in a sequence */
extern int seq_length(T seq);

/** @brief get the idx-th element of a *seq* 
 * @return *x* */
extern void *seq_get(T seq, int idx);

/** @brief set the *idx*-th element of *seq* to *x* 
 * @return the previously stored value */
extern void *seq_put(T seq, int idx, void *x);

/** @brief add *x* to the low end of *seq* 
 * @return *x* .  */
extern void *seq_addlo(T seq, void *x);

/** @brief add *x* to the high end of *seq* 
 * @return *x* .  */
extern void *seq_addhi(T seq, void *x);

/** @brief remove and returns the value at the low end of *seq* */
extern void *seq_remlo(T seq);
/** @brief remove and returns the value at the high end of *seq* */
extern void *seq_remhi(T seq);

#endif /* end of include guard: SEQ_H */
