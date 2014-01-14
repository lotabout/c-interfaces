#ifndef GETWORD_H
#define GETWORD_H

#include <stdio.h>

extern int getword(FILE *fp, char *buf, int size, int first(int c), int
                   rest(int c));


#endif /* end of include guard: GETWORD_H */
