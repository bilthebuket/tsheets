#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include "normal_mode.h"
#include "select_mode.h"
#include "input_mode.h"
#include "save_mode.h"
#include "function_mode.h"
#include "io_tools.h"
#include "LL.h"
#include "sheet.h"
#include "clipboard.h"
#include "undo_redo.h"
#include "functions.h"

int main(int argc, char* argv[])
{
	initialize_window();
	open_sheets = make_list();
	
	if (argc == 1)
	{
		sheet = initialize_sheet(NULL);
		add(open_sheets, make_tab(0, 0, 0, 0, sheet), 0);
	}
	else
	{
		if (argc == 2)
		{
			FILE* f = fopen(argv[1], "r");
			if (f == NULL)
			{
				printf("Error reading file.");
				return 1;
			}
			else
			{
				sheet = initialize_sheet(f);
				add(open_sheets, make_tab(0, 0, 0, 0, sheet), 0);
				fclose(f);
			}
		}
		else
		{
			printf("Expected one or less arguments, recieved %d.", argc - 1);
			return 1;
		}
	}

	// printing the initial screen which will then be continuously updated by the loop
	print_page(0, 0);

	int ch;

	// coordinates of the cell the user is currently selecting
	x = 0;
	y = 0;

	page_x = 0;
	page_y = 0;

	str = make_list();
	plots = make_list();

	// load_functions();
	initialize_funcs();

	mode = &normal_mode;

	// the core of the program is have a function pointer to different "modes" (like vim), and every time the user gives a keystroke we send it to that mode
	// the modes will switch between themselves, all we need to do is send the keystrokes to the current mode
	while (true)
	{
		ch = getch();

		if (screen_big_enough_huh)
		{
			(*mode)(ch);
		}
		else
		{
			mvprintw(0, 0, "Screen not big enough. Please resize window to add more space.");
		}

		if (resize_requested)
		{
			handle_screen_resize();
			resize_requested = false;
		}

		if (terminate)
		{
			break;
		}

		refresh();
	}

	free_list(str, 0, true);
	endwin();

	for (int i = 0; i < open_sheets->num_elts; i++)
	{
		free_list(((Tab*) get(open_sheets, i))->sheet, 1, true);
	}

	free_list(open_sheets, 0, true);

	if (plots->num_elts > 0)
	{
		if (plots->num_elts == 1)
		{
			Node* n = plots->node;
			free_plot((Plot*) n->elt);
			free(n);
		}
		else
		{
			Node* n = plots->node;
			free_plot((Plot*) n->elt);
			n = n->next;

			for (int i = 0; i < plots->num_elts - 2; i++)
			{
				free(n->prev);
				free_plot((Plot*) n->elt);
				n = n->next;
			}

			free(n->prev);
			free_plot((Plot*) n->elt);
			free(n);
		}
	}

	free(plots);
	free_clipboard();
	free_prevstates();
	free_funcs();
	// free_functions();
}
