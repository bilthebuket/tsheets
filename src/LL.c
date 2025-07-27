#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "LL.h"

// the body of each cell displays 9 characters
#define NUM_CHARS_IN_CELL 9

void add_list(Head* lst, Head* lst_to_add, int index)
{
	int num_elts_added = lst_to_add->num_elts;

	if (lst->num_elts == 0)
	{
		lst->node = lst_to_add->node;
		lst->tail = lst_to_add->tail;
	}
	else
	{
		Node* n = get_helper(lst, index);

		if (index == 0)
		{
			lst->node->prev = lst_to_add->tail;
			lst_to_add->tail->next = lst->node;
			lst->node = lst_to_add->node;
		}
		else
		{
			if (index == lst->num_elts)
			{
				lst->tail->next = lst_to_add->node;
				lst_to_add->node->prev = lst->tail;
				lst->tail = lst_to_add->tail;
			}
			else
			{
				n->prev->next = lst_to_add->node;
				lst_to_add->node->prev = n->prev;
				n->prev = lst_to_add->tail;
				lst_to_add->tail->next = n;
			}
		}
	}

	// if we added the list infront of the last got node in the list that is being added to, we need to make sure lst->last_got_index
	// actually stores the index of the last got node
	if (lst->last_got_index != -1)
	{
		if (lst->last_got_index >= index)
		{
			lst->last_got_index += num_elts_added;
		}
	}

	lst->num_elts += lst_to_add->num_elts;

	// freeing the head of the list we added but not the nodes, as we added the nodes to the other list
	free(lst_to_add);
}
	


Head* str_to_linked_list(const char* str)
{
	Head* r = make_list();

	for (int i = 0; str[i] != '\0'; i++)
	{
		char* c = malloc(sizeof(char));
		*c = str[i];
		add(r, c, i);
	}

	return r;
}


char* linked_list_to_str(Head* str, bool pad)
{
	char* r;

	if (pad && str->num_elts < NUM_CHARS_IN_CELL)
	{
		r = malloc(sizeof(char) * NUM_CHARS_IN_CELL + 1);
	}
	else
	{
       		r = malloc(sizeof(char) * str->num_elts + 1);
	}

	Node* ptr = str->node;

	for (int i = 0; i < str->num_elts; i++)
	{
		r[i] = *(char*) ptr->elt;
		ptr = ptr->next;
	}

	if (pad && str->num_elts < 9)
	{
		for (int i = str->num_elts; i < NUM_CHARS_IN_CELL; i++)
		{
			r[i] = ' ';
		}
	}
	else
	{
		r[str->num_elts] = '\0';
	}

	if (pad)
	{
		r[9] = '\0';
	}

	return r;
}

Head* double_to_char_linked_list(double d)
{
	char* s = malloc(sizeof(char) * 100);
	snprintf(s, 99, "%lf", d);
	Head* r = str_to_linked_list(s);
	free(s);
	return r;
}

Head* make_list()
{
	Head* r = (Head*) malloc(sizeof(Head));

	r->num_elts = 0;
	r->node = NULL;
	r->tail = NULL;
	r->last_got = NULL;
	r->last_got_index = -1;

	return r;
}

void* get(Head* lst, int index)
{
	if (index < 0 || index >= lst->num_elts)
	{
		return NULL;
	}
	else
	{
		return get_helper(lst, index)->elt;
	}
}

Node* get_helper(Head* lst, int index)
{
	Node* ptr;

	// calculating the distance from the index we want to retrieve and the ones we already have pointers to (the first index, the last index, and the last index we retrieved)
	int distance_from_start = index;
	int distance_from_end = lst->num_elts - index - 1;
	int distance_from_last;
	if (lst->last_got_index == -1)
	{
		distance_from_last = 2147483647;
	}
	else
	{
		distance_from_last = abs(lst->last_got_index - index);
	}

	// finding out which distance is the least, then going to that node and traversing to the index we want to retrieve
	if (distance_from_start <= distance_from_end && distance_from_start <= distance_from_last)
	{
		ptr = lst->node;
		for (int i = 0; i < distance_from_start; i++)
		{
			ptr = ptr->next;
			if (ptr == NULL)
			{
				printf("Error: Null pointer in get_helper()\n");
				return NULL;
			}
		}
	}
	else
	{
		if (distance_from_end <= distance_from_start && distance_from_end <= distance_from_last)
		{
			ptr = lst->tail;
			for (int i = 0; i < distance_from_end; i++)
			{
				ptr = ptr->prev;
				if (ptr == NULL)
				{
					printf("Error: Null pointer in get_helper()\n");
					return NULL;
				}
			}
		}	
		else
		{
			ptr = lst->last_got;
			
			if (lst->last_got_index - index < 0)
			{
				for (int i = 0; i < distance_from_last; i++)
				{
					ptr = ptr->next;
					if (ptr == NULL)
					{
						printf("Error: Null pointer in get_helper()\n");
						return NULL;
					}
				}
			}
			else
			{
				for (int i = 0; i < distance_from_last; i++)
				{
					ptr = ptr->prev;
					if (ptr == NULL)
					{
						printf("Error: Null pointer in get_helper()\n");
						return NULL;
					}
				}
			}
		}
	}

	lst->last_got = ptr;
	lst->last_got_index = index;
	return ptr;
}

void add(Head* lst, void* elt, int index)
{
	Node* n = (Node*) malloc(sizeof(Node));
	n->elt = elt;
	
	// all of these if else's are for edge cases, like if we're adding the element to one of the ends of the list, or if there's only 0 or 1 elements in the list
	// the typical case if the last one in the if else, where we just use get_helper()
	if (lst->tail == NULL)
	{
		if (lst->node == NULL)
		{
			n->next = NULL;
			n->prev = NULL;
			lst->node = n;
		}
		else
		{
			if (index == 0)
			{
				n->prev = NULL;
				n->next = lst->node;

				lst->node->prev = n;
				lst->node->next = NULL;

				lst->tail = lst->node;
				lst->node = n;
			}
			else
			{
				lst->tail = n;

				lst->node->next = lst->tail;
				lst->tail->prev = lst->node;

				lst->tail->next = NULL;
				lst->node->prev = NULL;
			}
		}
	}
	else
	{
		if (index == 0)
		{
			n->next = lst->node;
			lst->node->prev = n;
			lst->node = n;
		}
		else
		{
			if (index == lst->num_elts)
			{
				n->next = NULL;
				n->prev = lst->tail;
				lst->tail->next = n;
				lst->tail = n;
			}
			else
			{
				Node* ptr = get_helper(lst, index);

				ptr->prev->next = n;
				n->prev = ptr->prev;
				n->next = ptr;
				ptr->prev = n;
			}
		}
	}

	// just like in add_list(), if we add an element infront of the last one we got we need to update the index number so it remains accurate
	if (lst->last_got_index == -1)
	{
		if (lst->last_got_index >= index)
		{
			lst->last_got_index++;
		}
	}
	lst->num_elts++;
}

void rm(Head* lst, int index, int num_recursive_frees)
{
	// checking for edge cases
	if (index == 0)
	{
		Node* ptr = lst->node->next;

		if (lst->num_elts > 1)
		{
			ptr->prev = NULL;
		}

		if (num_recursive_frees >= 0)
		{
			if (num_recursive_frees > 0)
			{
				free_list((Head*) lst->node->elt, num_recursive_frees - 1, true);
			}
			else
			{
				free(lst->node->elt);
			}
		}

		free(lst->node);

		if (lst->num_elts > 1)
		{
			lst->node = ptr;
		}
		else
		{
			lst->node = NULL;
			lst->tail = NULL;
		}
	}
	else
	{
		if (index == lst->num_elts - 1)
		{
			lst->tail->prev->next = NULL;
			
			if (num_recursive_frees >= 0)
			{
				if (num_recursive_frees > 0)
				{
					free_list((Head*) lst->tail->elt, num_recursive_frees - 1, true);
				}
				else
				{
					free(lst->tail->elt);
				}
			}

			Node* ptr = lst->tail;
			lst->tail = lst->tail->prev;
			
			free(ptr);
		}
		else
		{
			Node* ptr = get_helper(lst, index);

			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;

			if (num_recursive_frees >= 0)
			{
				if (num_recursive_frees > 0)
				{
					free_list((Head*) ptr->elt, num_recursive_frees - 1, true);
				}
				else
				{
					free(ptr->elt);
				}
			}

			free(ptr);
		}
	}

	lst->num_elts--;

	// making the same adjustment as add() and add_list(), correcting the last_got_index if we removed an element that fell before the last got node
	if (lst->last_got_index != -1)
	{
		if (lst->last_got_index == index)
		{
			lst->last_got = NULL;
			lst->last_got_index = -1;
		}
		else
		{
			if (lst->last_got_index > index)
			{
				lst->last_got_index--;
			}
		}
	}
}

void free_list(Head* lst, int num_recursive_frees, bool free_elts)
{
	Node* ptr = lst->node;

	if (ptr == NULL)
	{
		free(lst);
		return;
	}

	// to free the list we got to the next node and free the previous node until we reach the last node, then we free that node and free the list
	// for each node we free the element/make the recursive call if needed
	for (int i = 0; i < lst->num_elts - 1; i++)
	{
		ptr = ptr->next;

		if (num_recursive_frees > 0)
		{
			free_list((Head*) ptr->prev->elt, num_recursive_frees - 1, free_elts);
		}
		else
		{
			if (free_elts)
			{
				free(ptr->prev->elt);
			}
		}
		
		free(ptr->prev);
	}

	if (num_recursive_frees > 0)
	{
		free_list((Head*) ptr->elt, num_recursive_frees - 1, free_elts);
	}
	else
	{
		if (free_elts)
		{
			free(ptr->elt);
		}
	}

	free(ptr);

	free(lst);
}

void* set(Head* lst, void* val, int index, int num_recursive_frees)
{
	Node* ptr = get_helper(lst, index);
	void* r = ptr->elt;

	if (num_recursive_frees >= 0)
	{
		if (num_recursive_frees > 0)
		{
			free_list((Head*) ptr->elt, num_recursive_frees - 1, true);
		}
		else
		{
			free(ptr->elt);
		}

		r = NULL;
	}

	ptr->elt = val;
	return r;
}

Head* deep_copy(Head* lst, int recursive_copies, int elt_size)
{
	Head* r = make_list();

	if (recursive_copies == 0)
	{
		for (Node* n = lst->node; n != NULL; n = n->next)
		{
			void* elt = malloc(elt_size);

			// this allows us to manually copy the bytes from n->elt to elt, beacuse we do not know the datatype of what is stored in n->elt, only the number of bytes it occupies, thus
			// we cannot dereference n->elt in order to copy it into elt, thus we must copy it byte by byte
			unsigned char* x = (unsigned char*) elt;
			unsigned char* y = (unsigned char*) n->elt;
			for (int i = 0; i < elt_size; i++)
			{
				x[i] = y[i];
			}

			add(r, elt, r->num_elts);
		}
	}
	else
	{
		for (Node* n = lst->node; n != NULL; n = n->next)
		{
			Head* elt = deep_copy((Head*) n->elt, recursive_copies - 1, elt_size);
			add(r, elt, r->num_elts);
		}
	}
	
	return r;
}
