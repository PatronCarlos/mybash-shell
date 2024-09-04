#ifndef _STREXTRA_H_
#define _STREXTRA_H_

#include <stdbool.h>
#include <stddef.h>

char *strmerge(char *s1, char *s2);
/*
 * Concatena las cadenas en s1 y s2 devolviendo nueva memoria (debe ser
 * liberada por el llamador con free())
 *
 * USAGE:
 *
 * merge = strmerge(s1, s2);
 *
 * REQUIRES:
 *     s1 != NULL &&  s2 != NULL
 *
 * ENSURES:
 *     merge != NULL && strlen(merge) == strlen(s1) + strlen(s2)
 *
 */

bool is_string_in_array(const char *str, const char *array[],
                               size_t arr_size);
/*
 * Verifica si la cadena str está presente en el arreglo de cadenas array
 *
 * USAGE:
 *     bool is_in_array = is_string_in_array(s,arr,n)
 *
 * REQUIRES:
 *     str != NULL
 *
 * ENSURES:
 *     str ∈ array => is_string_in_array(str,array,n) = true
 *
 */

#endif
