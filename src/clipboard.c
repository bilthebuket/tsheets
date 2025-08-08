#include <stdio.h>
#include "clipboard.h"
#include "LL.h"
#include "io_tools.h"
#include "sheet.h"
#include "undo_redo.h"

// 2d linked list of char*'s (a list of rows, which are each lists of cells, which are each arrays of characters)
Head* clipboard = NULL;

void copy(void)
{
	if (clipboard != NULL)
	{
		free_clipboard();
	}

	clipboard = make_list();

	// if x_s == -1 that means we are copying a single cell
	// even though its just one cell we make it a 3d linked list to keep it consistent
	if (x_s == -1)
	{
		add(clipboard, make_list(), 0);
		add((Head*) get(clipboard, 0), deep_copy_cell(get((Head*) get(sheet, y), x)), 0);
	}
	// otherwise we need to copy multiple cells
	else
	{
		if (x_s > x_f)
		{
			int temp = x_s;
			x_s = x_f;
			x_f = temp;
		}
		if (y_s > y_f)
		{
			int temp = y_s;
			y_s = y_f;
			y_f = temp;
		}

		// adding a deep copy of each highlighted cell to the clipboard in the same arrangement that they appear in the sheet
		for (int i = y_s; i <= y_f; i++)
		{
			add(clipboard, make_list(), clipboard->num_elts);
			for (int j = x_s; j <= x_f; j++)
			{
				add((Head*) get(clipboard, i - y_s), deep_copy_cell((Head*) get((Head*) get(sheet, i), j)), j - x_s);
				print_cell(j, i, false);
			}
		}
	}
}

void paste(void)
{
	if (clipboard == NULL)
	{
		return;
	}

	// iterating through the sheet and replacing each element with the one in the clipboard

	// maintaining a pointer to what row we're on in the sheet
	Node* s = get_helper(sheet, y);

	// maintaining a pointer to what row we're on in the clipboard
	Node* c = clipboard->node;

	// these are the cells we are overwriting which will be added to the top of the undo stack
	Head* cells_to_be_overwritten = make_list();

	for (int i = 0; i < clipboard->num_elts; i++)
	{
		// maintaining a pointer to what cell we're on in the current row for the clipboard and sheet
		Node* n_s = get_helper((Head*) s->elt, x);
		Node* n_c = ((Head*) c->elt)->node;

		add(cells_to_be_overwritten, make_list(), i);

		// we need to take all the elements in the linked list that makes up the cell and copy them to the LL that will be added to the undo stack, then replacing them with
		// what's in the clipboard
		for (int j = 0; j < ((Head*) c->elt)->num_elts; j++)
		{
			add((Head*) get(cells_to_be_overwritten, i), n_s->elt, j);
			n_s->elt = n_c->elt;

			n_s = n_s->next;
			n_c = n_c->next;
		}

		s = s->next;
		c = c->next;
	}

	add_undo(cells_to_be_overwritten, x, y, x + ((Head*) get(clipboard, 0))->num_elts - 1, y + clipboard->num_elts - 1);

	// now that we've put the clipboard elements into the sheet, we need to make a deep copy in case the user pastes again
	Head* temp = make_list();

	for (int i = 0; i < clipboard->num_elts; i++)
	{
		Head* row = make_list();

		for (int j = 0; j < ((Head*) get(clipboard, i))->num_elts; j++)
		{
			add(row, deep_copy_cell(get((Head*) get(clipboard, i), j)), j);
		}

		add(temp, row, i);
	}

	// freeing the lists that make up the clipboard but not the cells that we copied into the sheet
	free_list(clipboard, 1, false);

	clipboard = temp;
	print_page(page_x, page_y); // TODO: optimize by only printing the cells that changed
}

void free_clipboard(void)
{
	if (clipboard != NULL)
	{
		free_list(clipboard, 1, true);
	}
}

char* deep_copy_cell(void* cell)
{
	if (cell == NULL)
	{
		return NULL;
	}

	char* s = (char*) cell;

	int len;
	for (len = 0; s[len] != '\0'; len++) {}
	len++;

	char* r = malloc(sizeof(char) * len);

	for (int i = 0; i < len; i++)
	{
		r[i] = s[i];
	}

	return r;
}
