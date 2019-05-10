/**
 * Module for creating and manipulating singly-linked list
 *
 * Created by CHEN LIGUO(liguoc@student.unimelb.edu.au), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct node Node;

typedef struct list List;
struct list {
	Node *head;
	int size;
};

#include "handle.h"

struct node {
	Node *next;
	player_t *player;
};

// create a new, empty list and return its pointer
List *new_list();

// destroy a list and free its memory
void free_list(List *list);

// add an element to the start of the list
void list_add(List *list, player_t *new_player);

// return the username of player with specified cookie
char *extract_username(List *list, int cookie);

// find player with specified cookie, and return its pointer
player_t *extract_player(List *list, int cookie);

// find unpaired player, and return its pointer
player_t *find_opponent(List *list, int cookie);

#endif
