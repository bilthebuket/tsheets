#include <ncurses.h>
#include "input_mode.h"
#include "normal_mode.h"
#include "io_tools.h"
#include "sheet.h"
#include "undo_redo.h"

void load_string_in_input()
{
	void* cell = get((Head*) get(sheet, y), x);
	
	if (cell != NULL)
	{
		char* s = (char*) cell;

		for (int i = 0; s[i] != '\0'; i++)
		{
			char* c = malloc(sizeof(char));
			*c = s[i];
			add(str, c, str->num_elts);
		}
	}

	print_input_line();
}

void input_mode(int ch)
{
	if (get_input(ch))
	{
		// setting the cell to the new value, adding the old value to the undo stack
		
		void* elt;
		
		if (str->num_elts > 0)
		{
			elt = set((Head*) get(sheet, y), linked_list_to_str(str, false), x, -1);
		}
		else
		{
			elt = set((Head*) get(sheet, y), NULL, x, -1);
		}
		add_undo(elt, x, y, -1, -1);
		print_cell(x, y, true);
		free_list(str, 0, true);
		str = make_list();
		mode = &normal_mode;
	}
}
