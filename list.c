/**
 * Module for creating and manipulating singly-linked list
 *
 * Created by CHEN LIGUO(student ID: 851090), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

/* libraries required */

#include <stdlib.h>
#include <assert.h>

#include "list.h"

/****************************************************************************/

/* helper functions */

// create a new node and return its address
Node *new_node();

// clear memory of a node
void free_node(Node *node);

/****************************************************************************/

// create a new, empty list and return its pointer
List *new_list() {
	List *list = malloc(sizeof *list);
	assert(list);

	list->head = NULL;
	list->size = 0;

	return list;
}

// destroy a list and free its memory
void free_list(List *list) {
	assert(list);

	Node *node = list->head;
	Node *next;
	while(node) {
		next = node->next;
		free(node->player);
		free_node(node);
		node = next;
	}
	free(list);
}

// helper function to create a new node and return its address
Node *new_node() {
	Node *node = malloc(sizeof *node);
	assert(node);

	return node;	
}

// helper function to clear memory of a node
void free_node(Node *node) {
	free(node);
}

// add an element to the start of the list
void list_add(List *list, player_t *new_player) {
	assert(list);
	assert(new_player);

	Node *new = new_node(new_player);
	new->player = new_player;
	new->next = list->head;
	list->head = new;
	
	list->size++;
}

// return the username of player with specified cookie
char *extract_username(List *list, int cookie) {
	assert(list);

	Node *node = list->head;
	while(node) {
		if(node->player->cookie == cookie) {
			return node->player->name;
		}
		node = node->next;
	}
	return NULL;
}

// find player with specified cookie, and return its pointer
player_t *extract_player(List *list, int cookie) {
	assert(list);

	Node *node = list->head;
	while(node) {
		if(node->player->cookie == cookie)
			return node->player;
		node = node->next;
	}
	// no player specified cookie found
	return NULL;
}

// find unpaired player, and return its pointer
player_t *find_opponent(List *list, int cookie) {
	assert(list);

	Node *node = list->head;
	while(node) {
		// find a player that is not the current player, 
		// active and unmatched
		if(node->player->cookie != cookie && \
		   node->player->active && \
		   node->player->opponent == NULL) {
			return node->player;
		}
		node = node->next;
	}
	// no player with valid status found
	return NULL;
}
