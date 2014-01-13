/* @file: atom.h
 *
 * An *atom is a pointer to a unique, immutable sequence of zero or more
 * arbitrary bytes. Most atoms are pointers to null-terminated strings, but a
 * pointer to any sequence of bytes can be an atom.
 *
 * There is a single instance of any atom. For example, two strings:
 * char *str1 = "abcdefg";
 * char *str2 = "abcdefg";
 * When representing them as atoms, they will point to the same instance:
 * atom(str1) == atom(str2);
 *
 * This is the interface file.
 */
#ifndef ATOM_H
#define ATOM_H

/**
 * @param str is an existing atom
 * @return the length of an atom
 */
extern int atom_length(const char *str);

/** @brief Create a new atom or return the atom if already exists.
 * @param str The contents of the new atom.
 * @param len The length of the new atom.
 * @return a const pointer to the new atom or the atom that already exists.
 */
extern const char *atom_new(const char *str, int len);

/** @brief Given a string, create a new atom or return the one existed.
 * @param str The contents of the new atom in null-terminated character
 *      string.
 * @return a const pointer to the new atom or the atom that already exists.
 */
extern const char *atom_string(const char *str);

/** @brief Given an integer, create an atom.
 * First convert the integer into string and then create an atom according to
 * the string.
 * @param n the integer
 * @return a const pointer to the new atom or the atom that already exists.
 */
extern const char *atom_int(long n);

#endif /* end of include guard: ATOM_H */
