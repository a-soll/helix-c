#ifndef UTIL_H
#define UTIL_H

#include "json-c/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// returns NULL if key not found in json
const char *get_key(json_object *from, const char *key);
/* concatenate numbers in the thousands. returns original value if not at least 4 digits.
 *  ex: 1100 -> 1.1k
 */
int abbreviate_number(char from[], char to[]);
// void get_json_array(Response *response, const char *key);
int fmt_string(char *to, size_t size, const char *s, ...);
char *concat(char *dst, char *src, char term, size_t size);
void print_json(json_object *json);
int replace_substr(char *dst, char *from, char *repl, char *with);

#endif /* util_h */
