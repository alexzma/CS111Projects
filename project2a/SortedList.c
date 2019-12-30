#include "SortedList.h"
#include <string.h>
#include <sched.h>
#include <unistd.h>

int opt_yield;

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	const char* orig = element->key;
	if(list->key == NULL){
		list = list->next;
	}
	while(list->key != NULL){
		const char* temp = list->key;
		if(strcmp(orig, temp) <= 0){
			if(opt_yield & INSERT_YIELD)
				sched_yield();
			element->next = list;
			element->prev = list->prev;
			list->prev->next = element;
			list->prev = element;
			return;
		}
		list = list->next;
	}
	if(opt_yield & INSERT_YIELD)
		sched_yield();
	element->next = list;
	element->prev = list->prev;
	list->prev->next = element;
	list->prev = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element){
	if(element->next->prev != element || element->prev->next != element){
		return 1;
	}
	if(opt_yield & DELETE_YIELD)
		sched_yield();
	element->prev->next = element->next;
	element->next->prev = element->prev;
	return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
	if(key == NULL){
		while(list->key != NULL){
			if(opt_yield & LOOKUP_YIELD)
				sched_yield();
			list = list->next;
		}
		return list;
	}
	if(list->key == NULL){
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		list = list->next;
	}
	while(list->key != NULL){
		const char* temp = list->key;
		if(strcmp(key, temp) == 0){
			return list;
		}
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		list = list->next;
	}
	return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list){
	SortedList_t* start = list;
	if(list->next->prev != list || list->prev->next != list){
		return -1;
	}
	list = list->next;
	int count = 0;
	while(list != start){
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		if(list->next->prev != list || list->prev->next != list){
			return -1;
		}
		list = list->next;
		count++;
	}
	return count;
}
