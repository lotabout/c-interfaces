/* @file xref.c
 * @brief show which lines an identifier is used by files 
 */

/* xref includes */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <atom.h>
#include <table.h>
#include <set.h>
#include <mem.h>

#include "getword.h"

/* Data structure of *xref*
 *
 * identifiers
 * +------+
 * |id x  |
 * +------+       files
 * |id y  | ---> +------+
 * +------+      |file 1| ---> set | 1 6 2 3 7 9 | 
 * |id t  |      +------+
 * +------+      |file 2| ---> set | 8 2 4 3 1 7 |
 * |      |      +------+
 * | ...  |      | ...  |
 * +------+      +------+
 */

/* xref prototypes */

/* xref global data */
static int linenum;

/* xref functions */

int first(int c) 
{
    if (c == '\n')
        linenum++;

    return isalpha(c) || c == '_';
}

int rest(int c)
{
    return isalpha(c) || c == '_' || isdigit(c);
}

/* used to compare two identifiers(char *), used in qsort(identifiers ||
 * files)*/
int cmp(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

/* used in generate a new set, compare two integer pointers */
int set_cmp_int(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

/* used in generate a new set, to generate a hash value from an integer
 * pointers */
unsigned set_hash_int(const void *x)
{
    return *(int *)x;
}

/* used in qsort(), compare two pointers which points to pointer to integers */
int cmp_int(const void *a, const void *b)
{
    return **(int **)a - **(int **)b;
}

/* build xref database */
void xref(const char *name, FILE *fp, table_t identifiers)
{
    char buf[BUFSIZ];

    if (name == NULL) {
        name = "";
    }
    name = atom_string(name);
    linenum = 1;

    while (getword(fp, buf, sizeof(buf), first, rest)) {
        set_t set;
        table_t files;
        const char *id = atom_string(buf);

        /* files <- file table in identifiers associated with id */
        files = table_get(identifiers, id);
        if (files == NULL) {
            files = table_new(0, NULL, NULL);
            table_put(identifiers, id, files);
        }

        /* set <- set in files associated with name */
        set = table_get(files, name);
        if (set == NULL) {
            set = set_new(0, set_cmp_int, set_hash_int);
            table_put(files, name, set);
        }

        /* add linenum to set, if necessary */
        int *p = &linenum;
        if (!set_member(set, p)) {
            p = (unsigned *)zalloc(sizeof(*p));
            *p = linenum;
            set_put(set, p);
        }
    }
}


void print_set(set_t set)
{
    int i;
    void **lines = set_to_array(set, NULL);
    qsort(lines, set_length(set), sizeof(*lines), cmp_int);

    for (i=0; lines[i]; i++) {
        printf(" %d", *(int *)lines[i]);
    }
    zfree(lines);
}

/* format and print a single entry of an identifier 
 *
 * idenfier --> file 1 -> line 1 3 5 6 9
 *          --> file 2 -> line 18 28 3 13
 *          --> ...
 *
 * */
void print_files(table_t files)
{
    int i;
    void ** array = table_to_array(files, NULL);

    qsort(array, table_length(files), 2*sizeof(*array), cmp);

    for (i=0; array[i]; i+=2) {
        /* check if a filename is vaid (!="") */
        if (*(char *)array[i] != '\0') {
            printf("\t%s:", (char *)array[i]);
        }
        /* print the line numbers in the set array[i+1] */
        print_set((set_t)array[i+1]);

        printf("\n");
    }
    zfree(array);
}

/* format the output and print out all identifiers */
void print_identifiers(table_t identifiers)
{
    int i;
    void **array = table_to_array(identifiers, NULL);
    qsort(array, table_length(identifiers), 2*sizeof(*array), cmp);

    for (i=0; array[i]; i+=2) {
        /* print the idenfier name */
        printf("%s:\n", (char *)array[i]);
        /* print the files and lines with reference to this idenfier */
        print_files((table_t) array[i+1]);
    }

    zfree(array);   /* in lib/mem.h */
}

int main(int argc, const char *argv[])
{
    int i;
    table_t identifiers = table_new(0, NULL, NULL);

    for (i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: cannot open '%s' (%s)\n", argv[0], argv[i],
                    strerror(errno));
        } else {
            xref(argv[i], fp, identifiers);
            fclose(fp);
        }
    }
    if (argc == 1) {
        xref(NULL, stdin, identifiers);
    }

    /* print the identifiers */
    print_identifiers(identifiers);

    /* TODO: free the table */

    return 0;
}
