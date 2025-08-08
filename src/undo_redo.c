#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "undo_redo.h"
#include "sheet.h"
#include "io_tools.h"

Head* undos = NULL;
Head* redos = NULL;

void undo(void)
{
	// undoing the last action and adding it to the stack of redos
	if (undos != NULL)
	{
		if (undos->num_elts > 0)
		{
			if (redos == NULL)
			{
				redos = make_list();
				Tab* t = (Tab*) get(open_sheets, sheet_index);
				t->redos = redos;
			}

			add(redos, execute_prevstate(get(undos, 0)), 0);
			rm(undos, 0, -1);
		}
	}
}

void redo(void)
{
	// redoing the last undo and adding it to the stack of undos
	if (redos != NULL)
	{
		if (redos->num_elts > 0)
		{
			if (undos == NULL)
			{
				undos = make_list();
				Tab* t = (Tab*) get(open_sheets, sheet_index);
				t->undos = undos;
			}

			add(undos, execute_prevstate(get(redos, 0)), 0);
			rm(redos, 0, -1);
		}
	}

}

PrevState* execute_prevstate(PrevState* ps)
{
	// the prevstate for the cells we are overriding. this will be returned
	PrevState* redo = malloc(sizeof(PrevState));
	redo->x_s = ps->x_s;
	redo->y_s = ps->y_s;
	redo->x_f = ps->x_f;
	redo->y_f = ps->y_f;

	// iterating over the cells that ps covers and replacing them, adding the overwritten cells to the prevstate
	// that will be returned
	if (ps->x_f == -1)
	{
		void* elt = set((Head*) get(sheet, ps->y_s), ps->elts, ps->x_s, -1);
		redo->elts = (Head*) elt;
		print_cell(ps->x_s, ps->y_s, x_s == x && y_s == y);
	}
	else
	{
		Head* elts = make_list();

		for (int i = ps->y_s; i <= ps->y_f; i++)
		{
			add(elts, make_list(), i - ps->y_s);

			for (int j = ps->x_s; j <= ps->x_f; j++)
			{
				void* elt = set((Head*) get(sheet, i), get((Head*) get(ps->elts, i - ps->y_s), j - ps->x_s), j, -1);
				add((Head*) get(elts, i - ps->y_s), elt, j - ps->x_s);
				print_cell(j, i, i == x && j == y);
			}
		}

		redo->elts = elts;

		free_list(ps->elts, 1, false);
	}

	free(ps);
	return redo;
}

void add_undo(void* elts, int x_s, int y_s, int x_f, int y_f)
{
	if (undos == NULL)
	{
		undos = make_list();
		Tab* t = (Tab*) get(open_sheets, sheet_index);
		t->undos = undos;
	}

	PrevState* ps = malloc(sizeof(PrevState));
	ps->elts = elts;
	ps->x_s = x_s;
	ps->y_s = y_s;
	ps->x_f = x_f;
	ps->y_f = y_f;

	add(undos, ps, 0);
}

void free_prevstate(Head* undos, Head* redos)
{
	if (undos != NULL)
	{
		for (Node* n = undos->node; n != NULL; n = n->next)
		{
			PrevState* ps = (PrevState*) n->elt;
			if (ps->x_f == -1)
			{
				if (ps->elts != NULL)
				{
					free(ps->elts);
				}
			}
			else
			{
				free_list(ps->elts, 1, true);
			}
		}

		free_list(undos, 0, true);
	}

	if (redos != NULL)
	{
		for (Node* n = redos->node; n != NULL; n = n->next)
		{
			PrevState* ps = (PrevState*) n->elt;
			if (ps->x_f == -1)
			{
				if (ps->elts != NULL)
				{
					free(ps->elts);
				}
			}
			else
			{
				free_list(ps->elts, 1, true);
			}
		}

		free_list(redos, 0, true);
	}
}

void free_prevstates(void)
{
	for (int i = 0; i < open_sheets->num_elts; i++)
	{
		Tab* t = (Tab*) get(open_sheets, i);
		free_prevstate(t->undos, t->redos);
	}
}
