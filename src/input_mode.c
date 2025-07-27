#include <ncurses.h>
#include "input_mode.h"
#include "normal_mode.h"
#include "io_tools.h"
#include "sheet.h"
#include "undo_redo.h"

void load_string_in_input()
{
	Head* cell = (Head*) get((Head*) get(sheet, y), x);
	Node* ptr = cell->node;

	for (int i = 0; i < cell->num_elts; i++)
	{
		char* c = malloc(sizeof(char));
		*c = *(char*) ptr->elt;
		add(str, c, str->num_elts);
		ptr = ptr->next;
	}

	print_input_line();
}

void input_mode(int ch)
{
	if (get_input(ch))
	{
		// setting the cell to the new value, adding the old value to the undo stack
		void* elt = set((Head*) get(sheet, y), str, x, -1);
		add_undo((Head*) elt, x, y, -1, -1);
		print_cell(x, y, true);
		str = make_list();
		mode = &normal_mode;
	}
}
