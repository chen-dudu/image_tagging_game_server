/**
 * part of the code in this file is 
 * taken from the sample code from LMS
 * Modified by CHEN LIGUO(student ID: 851090), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

/* libraries required */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include <unistd.h>

#include "handle.h"
#include "util.h"
#include "respond.h"

/****************************************************************************/

/* constants used in the program */

static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\n\
Content-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\n\
Content-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;

// maximum length for a word entered by player
#define MAX_WORD 30
// separator between words entered by player
#define SEPARATOR " "
// the length of GET method in request message
#define GET_LEN 4
// the length of POST method in request message
#define POST_LEN 5

// constants for html file name
#define WELCOME "resource/1_intro.html"
#define START "resource/2_start.html"
#define TRY "resource/3_first_turn.html"
#define ACCEPT "resource/4_accepted.html"
#define DISCARD "resource/5_discarded.html"
#define COMPLETE "resource/6_endgame.html"
#define GAMEOVER "resource/7_gameover.html"

/****************************************************************************/

/* helper functions */

// deal with the different cases of GET request
bool handle_get(int sockfd, char *buff, List *player_list, \
	            int *round_num, int *cookie_to_use);
// respond when plyaer clicks quit
bool handle_quit(int sockfd, char *buff, List *player_list);
// reset player status to initial value
void clear_status(player_t *player);
// find unpaired player, match with current player
void matching(List *player_list, int cookie);

/****************************************************************************/

// respond to client request according to its state
bool handle_http_request(int sockfd, int *available_cookie, \
	                     int *round_num, List *player_list) {
	// try to read the request
    char read_buff[2049];
    int n = read(sockfd, read_buff, sizeof(read_buff));
   	
    if (n <= 0) {
        if (n < 0)
            perror("read");
        return false;
    }
    // adding null byte to the end of string
    read_buff[n] = 0;

    char *curr = read_buff;
    // parse the method
    METHOD method = UNKNOWN;
    if (strncmp(curr, "GET ", GET_LEN) == 0)
        method = GET;
    else if (strncmp(curr, "POST ", POST_LEN) == 0)
        method = POST;
    // method can't be recognised
    else if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0) {
        perror("write");
        return false;
    }

    player_t *curr_player, *opponent;
    bool status;
    int cookie;
    // requesting icon file
    if(strstr(read_buff, "favicon") != NULL) {
    	n = write(sockfd, HTTP_404, HTTP_404_LENGTH);
    	if(n < 0) {
    		perror("write");
    		return false;
    	}
    }
    else if(method == GET) {
    	status = handle_get(sockfd, read_buff, player_list, \
    		                round_num, available_cookie);
    	if(!status)
    		return false;
    }
    // player enters username
    else if(strstr(read_buff, "user=") != NULL) {
    	player_t *new_player = (player_t*)malloc(sizeof(*new_player));
    	assert(new_player);

    	new_player->cookie = extract_cookie(read_buff);
    	extract_word(new_player->name, read_buff, "user=");

        list_add(player_list, new_player);

        status = respond_with_insertion(sockfd, START, \
        	                            new_player->name, NULL);
        if(!status)
        	return false;
    }
    // player enters word
    else if(strstr(read_buff, "guess=") != NULL) {
    	cookie = extract_cookie(read_buff);
    	curr_player = extract_player(player_list, cookie);
    	opponent = curr_player->opponent;

    	// opponent has already quitted the game
    	if(curr_player->is_over) {
    		// show game over page
    		curr_player->active = false;    		
    		bzero(curr_player->words, sizeof(curr_player->words));
    		status = respond(sockfd, GAMEOVER);
    		if(!status)
    			return false;
    	}
    	// opponent gets a match
    	else if(curr_player->is_end) {
    		// show game complete page
    		bzero(curr_player->words, sizeof(curr_player->words));
    		curr_player->active = false;
    		curr_player->is_end = false;
    		status = respond(sockfd, COMPLETE);
    		if(!status)
    			return false;
    	}
    	// not yet matched with any player or opponent is inavtive
    	// or opponent has already finished
    	else if(curr_player->opponent == NULL || 
    		    curr_player->opponent->active == false ||
    		    curr_player->opponent->is_end == true) {
    		status = respond_with_insertion(sockfd, DISCARD, \
    			                            NULL, round_num);
    		if(!status)
    			return false;
    	}
    	// word accepted
    	else {
    		char word[MAX_WORD];
    		extract_word(word, read_buff, "keyword=");
    		to_lower_case(word);
    		if(contains(word, opponent->words)) {
    			// have a match, game finish
    			// update flags, show game complete page
    			(*round_num)++;
    			curr_player->active = false;
    			curr_player->is_end = true;
    			curr_player->opponent->is_end = true;
      			status = respond(sockfd, COMPLETE);
      			if(!status)
      				return false;
    		}
    		else {
    			strcat(curr_player->words, word);
    			strcat(curr_player->words, SEPARATOR);
    			status = respond_with_insertion(sockfd, ACCEPT, \
    				                            curr_player->words, round_num);
    			if(!status)
    				return false;
    		}
    	}
    }
    else if(strstr(read_buff, "quit=") != NULL) {
		return handle_quit(sockfd, read_buff, player_list);
    }
    return true;
}

// deal with the different cases of GET request
bool handle_get(int sockfd, char *buff, List *player_list, \
	            int *round_num, int *cookie_to_use) {
	assert(buff);
	assert(player_list);
	assert(round_num);
	assert(cookie_to_use);

	int cookie;
	bool status;
	player_t *curr_player;
	// player clicks start, search for opponent
    if(strstr(buff, "start=") != NULL) {
    	cookie = extract_cookie(buff);
    	curr_player = extract_player(player_list, cookie);
    	// initialise player status
    	curr_player->active = true;
    	curr_player->is_end = false;
    	// find opponent
    	matching(player_list, cookie);
    	status = respond_with_insertion(sockfd, TRY, NULL, round_num);
    	if(!status)
    		return false;
    }
    // player rejoins the game
    else if(strstr(buff, "Cookie: ") != NULL) {
    	cookie = extract_cookie(buff);
    	curr_player = extract_player(player_list, cookie);
    	clear_status(curr_player);
    	status = respond_with_insertion(sockfd, START, \
    		                            curr_player->name, NULL);
    	if(!status)
    		return false;
    }
    // player joins game for the first time
    else {
    	status = respond_with_cookie(sockfd, WELCOME, cookie_to_use);
    	if(!status)
    		return false;
    }
    return true;
}

// respond when plyaer clicks quit
bool handle_quit(int sockfd, char *buff, List *player_list) {
	assert(buff);
	assert(player_list);

	int cookie = extract_cookie(buff);
    player_t *curr_player = extract_player(player_list, cookie);

  	// paired with other player, affect the status of opponent
    if(curr_player->opponent != NULL) {
       curr_player->opponent->is_over = true;
    }
    // reset player status, clear useless data
	curr_player->active = false;
	curr_player->is_over = true;
	bzero(curr_player->words, sizeof(curr_player->words));

    respond(sockfd, GAMEOVER);
    // return false to let main close socket
    return false;
}

// reset player status to initial value
void clear_status(player_t *player) {
	assert(player);

	bzero(player->words, sizeof(player->words));
	player->active = false;
	player->is_end = false;
	player->is_over = false;
	player->opponent = NULL;
}

// find unpaired player, match with current player
void matching(List *player_list, int cookie) {
	assert(player_list);

	player_t *curr_player, *opponent;
	curr_player = extract_player(player_list, cookie);
	opponent = find_opponent(player_list, cookie);
	bzero(curr_player->words, sizeof(curr_player->words));

	// successfully find a player with valid status
	if(opponent != NULL) {
		curr_player->opponent = opponent;
		opponent->opponent = curr_player;
		bzero(opponent->words, sizeof(opponent->words));
	}
}
