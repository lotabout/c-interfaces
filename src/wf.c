#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "getword.h"
#include <atom.h>
#include <table.h>
#include <mem.h>

void wf(char *name, FILE *fp);

int first(int c) 
{
    return isalpha(c);
}

int rest(int c)
{
    return isalpha(c) || c == '_';
}

int cmp(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

void vfree(const void *key, void *val)
{
    zfree(val);
}

void wf(char *name, FILE *fp) 
{
    struct table_t *table = table_new(0, NULL, NULL);
    char buf[BUFSIZ];

    while (getword(fp, buf, sizeof(buf), first, rest)) {
        const char *word;
        int i, *count;
        for (i=0; buf[i] != '\0'; i++)
            buf[i] = tolower(buf[i]);
        word = atom_string(buf);
        count = table_get(table, word);
        if (count) {
            (*count) ++;
        } else {
            count = (int *)zalloc(sizeof (*count));
            *count = 1;
            table_put(table, word, count);
        }
    }

    if (name) {
        printf("%s:\n", name);
    }
    /* print the words */
    int i;
    void **array = table_to_array(table, NULL);
    qsort(array, table_length(table), 2*sizeof(*array), cmp);
    for (i = 0; array[i]; i+=2) {
        printf("%d\t%s\n", *(int *)array[i+1], (char *)array[i]);
    }
    zfree(array);

    /* destroy the table */
    table_free(&table, vfree);
}

int main(int argc, const char *argv[])
{
    int i;
    for (i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: can't open '%s' (%s)\n",
                    argv[0], argv[1], strerror(errno));

            return EXIT_FAILURE;
        } else {
            wf(argv[i], fp);
            fclose(fp);
        }
    }
    if (argc == 1) {
        wf(NULL, stdin);
    }
    return 0;
}
