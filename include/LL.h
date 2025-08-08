#ifndef LL_H
#define LL_H

#include <stdbool.h>

typedef struct Node
{
	// the element in this node
	void* elt;

	// the next node
	struct Node* next;
	
	// the previous node
	struct Node* prev;
} Node;

typedef struct Head
{
	// the last node in the list
	struct Node* tail;

	// the first node in the list
	struct Node* node;
	
	// the last node retrieved with get_helper()
	struct Node* last_got;

	// the index in the list of last_got
	int last_got_index;

	// the number of elements in the list
	int num_elts;
} Head;

/*
 * makes a linked list
 * params:
 * none
 * returns:
 * the head of the created list
*/
Head* make_list(void);

/*
 * sets the value at an index
 * params:
 * lst: the list
 * val: the value which will be set
 * index: the index of the value we are changing
 * num_recursive_frees: the number of times we have to free the element which is being replaced. if num_recursive_frees is -1, the element that is being replaced will not be freed
 * returns:
 * the element that is being replaced (if it isnt freed), otherwise returns NULL
*/
void* set(Head* lst, void* val, int index, int num_recursive_frees);

/*
 * returns an element at an index of a linked list
 * params:
 * lst: the pointer to the start of the list
 * index: the index of the elt that will be retrieved
 * returns:
 * the element at the index
*/
void* get(Head* lst, int index);

/*
 * does the same thing as get put returns the node instead of the element, used as a helper function in add() and rm()
 * params:
 * lst: the list
 * index: the index we are getting
 * returns:
 * pointer to node at index
*/
Node* get_helper(Head* lst, int index);

/*
 * adds an element to a linked list
 * params
 * lst: the list
 * elt: the element
 * index: where the element will be placed (ex: if index = 1, the element will become the second element in the list, the former second element will be pushed to the third position)
 * returns:
 * void
*/
void add(Head* lst, void* elt, int index);

/*
 * removes an item from the list
 * params:
 * lst: the list
 * index: index of item to be removed
 * num_recursive_frees: dimension of the list. if its a list of nums, this value should be zero. if its a list of linked lists, this would be 1, etc.
 * returns:
 * void
*/
void rm(Head* lst, int index, int num_recursive_frees);

/*
 * frees a list
 * params:
 * lst: list to be freed
 * num_recursive_frees: dimension of the list. if its a list of nums, this value should be zero. if its a list of linked lists, this would be 1, etc.
 * free_elts: if false, we will only free the head and nodes, if true, everything will be freed. if num_recursive_frees > 0, only the nested elements will remain unfreed.
 * ex: if you have a linked list of linked lists of characters and free_elts is false, everything will be freed except the characters within the nested linked lists
 * returns:
 * void
*/
void free_list(Head* lst, int num_recursive_frees, bool free_elts);

/*
 * converts a linked list of characters to a malloced string
 * params:
 * str: the string we are converting
 * pad: if this is true and the string is less than 9 characters long, spaces will be added to pad the string to nine characters (the number of characters displayed in a cell) + the null
 * this means that if a string is longer than nine characters, it will be cut off at nine, and if it is less than nine characters, whitespace will be added to push it to nine characters
 * returns:
 * the converted string
 * note:
 * if you call this function you must eventually free the converted string
*/
char* linked_list_to_str(Head* str, bool pad);

/*
 * adds one list to another
 * params:
 * lst: the list that is recieving the new elements from the old list
 * lst_to_add: the list which we are adding to the other list
 * index: where the list will be added. the element in the list that is currently at this index will be pushed to be the end of the list that is added.
 * example -> if {1, 4, 2} is added to {5, 4, 7, 8} at index 1, the new list will be {5, 1, 4, 2, 4, 7, 8}
 * returns: void
*/
void add_list(Head* lst, Head* lst_to_add, int index);

/*
 * converts a string to a linked list of characters
 * params:
 * str: the string that will be converted to a linked list
 * returns:
 * a linked list representation of str
*/
Head* str_to_linked_list(const char* str);

/*
 * converts a double to a string representation, which is returned as a linked list of characters
 * params:
 * d: the double which is being converted to a string
 * returns:
 * the string representaiton of d as a linked listt of characters
*/
Head* double_to_char_linked_list(double d);

/*
 * makes a deep copy of a list
 * params:
 * lst: the list that will be copied
 * recursive_copies: the number of nested lists to be deep copied (ex: if you have a list of lists recursive_copies should be 1)
 * elt_size: the number of bytes the 
 * returns:
 * pointer to deep copied list
*/
Head* deep_copy(Head* lst, int recursive_copies, int elt_size);

#endif
