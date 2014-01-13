/** @file: table.h
 * This file provide a general table library.
 */
#ifndef TABLE_H
#define TABLE_H

/* this time, we hide the details about table_t, put it in the implementation.
 * */
#define T table_t
typedef struct T *T;

/** @brief: create a new table 
 * @param hint: the size of the table.
 * @param cmp: the function used to compare elements of the table.
 * @param hash: hash function used to generate hash value from *key* 
 * @return a pointer to the new table. 
 * */
extern T table_new(int hint,
                   int cmp(const void *a, const void *b),
                   unsigned hash(const void *key));

/** @brief: free a table
 * @param table: table to be freed
 * @param destroy: the function used to destroy an element, can be NULL.
 * @return void
 * if you do not destroy elements manually used table_map, and you do not need
 * the data stored in the table any more, then please specify a destroy
 * function, otherwise it will cause memory leaks.
 */
extern void table_free(T *table, void (*destroy)(const void *key, void *value));

/** @brief: return the length of a table
 * Note that this is the elements a table, could be larger than a table's size
 * because it uses linked list to represent it.
 * @return the length of *table*
 */
extern int table_length(T table);

/** @brief: search for a key and if it finds it, change the associated value.
 * If the key is not found, it allocates and initializes a new binding.
 * @param table: the table to be operated on
 * @param key: the key to search
 * @param value: the new value
 * @return the previous associated value, NULL if *key* is not found.
 */
void *table_put(T table, const void *key, void *value);

/** @brief: get the value of a given *key* in the *table*
 * @param table: the table from which we will get the value.
 * @param key: the key string
 * @return the value of the corresponding key stored in the table, NULL is
 * returned if not found.
 */
extern void *table_get(T table, const void *key);

/** @brief: remove a table entry indexed by *key*
 * @param table: the table to be operated on
 * @param key: the key string
 * @return the previously stored value.
 */
extern void *table_remove(T table, const void *key);

/* @brief: map the function *apply* over all elements in the table 
 * @param cl: the the client-specific pointer
 * @return void
 */
extern void table_map(T table,
                      void apply(const void *key, void **value, void *cl),
                      void *cl);

/** @brief: convert a table to an array, it will store the (key, value) pairs.
 * for example table["key"] = "value", then on the returned array, there will
 * be an index: array[i]="key", array[i+1] = "value"
 * @param table: the table to be operated on
 * @param end: the element to be added to the end of the array.
 * @return an array converted from *table* plus the *end* element.
 */
extern void **table_to_array(T table, void *end);
    
/* debug function */
extern void print_table(T table);
#undef T
#endif /* end of include guard: TABLE_H */
