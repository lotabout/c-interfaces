/* @file: mem.h
 * @brief: simple wrapper of malloc and free 
 *
 * functions are 'z' prefixed, this is a personal favour.
 */
#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
void *zalloc(size_t size);
void zfree(void *ptr);
void *zcalloc(size_t nmemb, size_t size);

#endif /* end of include guard: MEM_H */
