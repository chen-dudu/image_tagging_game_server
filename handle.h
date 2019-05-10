/**
 * part of the code in this file is 
 * taken from the sample code from LMS
 * Modified by CHEN LIGUO(liguoc@student.unimelb.edu.au), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

#ifndef HANDLE_H
#define HANDLE_H

#include <stdbool.h>

// maximum length of username
#define MAX_USERNAME_LEN 30
// maximum length of all the words player has entered
#define MAX_WORDLIST_LEN 1024

typedef struct player player_t;
typedef struct player {
	int cookie;
	char name[MAX_USERNAME_LEN];     // store username
	char words[MAX_WORDLIST_LEN];    // store words entered
	bool active;        // true if player is in the game
	bool is_end;        // true if player is at endgame state
	bool is_over;       // true if player is at gameover state
	player_t *opponent; // null if not matched with any player
} player_t;

#include "list.h"
 
// represents the types of method
typedef enum {
    GET,
    POST,
    UNKNOWN
} METHOD;

// respond to client request according to its state
bool handle_http_request(int sockfd, int *available_cookie, \
	                     int *round_number, List *player_list);

#endif
