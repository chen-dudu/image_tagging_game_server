/**
 * Module containing functions that provide simple service
 * to support other modules
 * Created by CHEN LIGUO(student ID: 851090), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

#include <stdbool.h>

// extract word with type from source, store into dest
void extract_word(char *dest, char *source, char *type);
// extract cookie from buff
int extract_cookie(char *buff);
// check if str1 is a substring of str2
bool contains(char *str1, char *str2);
// convert letters in string str to lower case
void to_lower_case(char *str);
