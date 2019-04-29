/**
 * Module containing functions that provide simple service
 * to support other modules
 * Created by CHEN LIGUO(student ID: 851090), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

/* libraries required */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#include "util.h"

/****************************************************************************/

/* constants used in the program */

// maximum number of digits of cookie
#define MAX_COOKIE 2
// maximum length of a word
#define MAX_WORD 30

/****************************************************************************/

// extract word with type from source, store into dest
void extract_word(char *dest, char *source, char *type) {
	assert(dest);
	assert(source);
	assert(type);

	char *temp = (char*)malloc(sizeof(dest)*sizeof(*temp));
	assert(temp);

	// locating the start of the word to be extracted
	char *curr = strstr(source, type) + strlen(type);
	assert(curr);

	int i;
	for(i = 0; isalnum(*(curr+i)); i++) {
		temp[i] = *(curr + i);
	}
	temp[i] = 0;
	strcpy(dest, temp);
	free(temp);
}

// extract cookie from buff
int extract_cookie(char *buff) {
	assert(buff);

	char temp[MAX_COOKIE];
	extract_word(temp, buff, "Cookie: ");
	return atoi(temp);
}

// check if str1 is a substring of str2
bool contains(char *str1, char *str2) {
	assert(str1);
	assert(str2);

	// empty string
	if(strlen(str1) == 0 || strlen(str2) == 0)
		return false;

	char *probe;
	// no occurrence
	if((probe = strstr(str2, str1)) == NULL)
		return false;

	// at the middle of a word
	if(isalnum(*(probe - 1)))
		return false;
	
	char temp[MAX_WORD];
	bzero(temp, sizeof(temp));
	int i = 0;
	for(i = 0; isalnum(*(probe+i)); i++)
		temp[i] = *(probe + i);
	temp[i] = 0;
	return strcmp(temp, str1) == 0;
}

// convert letters in string str to lower case
void to_lower_case(char *str) {
	assert(str);

	for(int i  = 0; i < strlen(str); i++) {
		str[i] = tolower(str[i]);
	}
}
