#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "getword.h"

int getword(FILE *fp, char *buf, int size, int first(int c), int rest(int c))
{
    int i =0, c;

    assert(fp && buf && size > 1 && first && rest);

    /* get the first character that fits condition *first* */
    c = getc(fp);
    for (; c != EOF; c = getc(fp)) {
        if (first(c)) {
            /* stores c in buf if it fits */
            if (i < size-1) {
                buf[i++] = c;
            }

            c = getc(fp);
            break;
        }
    }

    for (; c != EOF && rest(c); c = getc(fp)) {
        /* stores c in buf if it fits */
        if (i < size-1) {
            buf[i++] = c;
        }
    }

    if (i < size) {
        buf[i] = '\0';
    } else {
        buf[size-1] = '\0';
    }

    if (c != EOF) {
        ungetc(c, fp);
    }

    return i > 0;
}
