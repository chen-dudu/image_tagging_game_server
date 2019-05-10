/**
 * Module containing functions that respond to http request
 * sent by browser
 * Created by CHEN LIGUO(liguoc@student.unimelb.edu.au), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

#include <stdbool.h>

// normal response with html file intact
bool respond(int sockfd, char *filename);
// response with new cookie sent
bool respond_with_cookie(int sockfd, char *filename, int *cookie_to_use);
// response with something inserted into html file
// to_insert might be username or words entered
bool respond_with_insertion(int sockfd, char *filename, \
	                   char *to_insert, int *round_num);
