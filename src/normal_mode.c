#include <ncurses.h>
#include "normal_mode.h"
#include "LL.h"
#include "sheet.h"
#include "save_mode.h"
#include "input_mode.h"
#include "function_mode.h"
#include "io_tools.h"
#include "select_mode.h"
#include "print_graph_mode.h"
#include "clipboard.h"
#include "undo_redo.h"
#include "import_mode.h"
#include "calc_mode.h"

void normal_mode(int ch)
{
	// enter and backspace are not part of the switch statement because we need to check for multiple keystroke types

	// for modifying an individual cell
	if (ch == '\n' || ch == '\r' || ch == KEY_ENTER)
	{
		load_string_in_input();
		mode = &input_mode;
	}
	// for deleting an individual cell
	if (ch == KEY_BACKSPACE || ch == BACKSPACE_KEYCODE1 || ch == BACKSPACE_KEYCODE2)
	{
		void* elt = set((Head*) get(sheet, y), make_list(), x, -1);
		add_undo((Head*) elt, x, y, -1, -1);
		print_cell(x, y, true);
	}

	switch (ch)
	{
		// for saving to a csv
		case 'o':
			move(0, char_rows - 1);
			mode = &save_mode;
			break;

		// wasd are for moving up down left right, each case needs to check if we are moving to a new page (say we are on a cell on the right edge then move to the right
		// we need to print the new page)
		case 'w':
			if (y % cell_rows == 0 && page_y > 0)
			{
				page_y--;
				y--;
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				if (y > 0)
				{
					print_cell(x, y, false);
					y--;
					print_cursor_info();
					print_cell(x, y, true);
				}
			}
			break;

		case 'a':
			if (x % cell_columns == 0 && page_x > 0)
			{
				page_x--;
				x--;
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				if (x > 0)
				{
					print_cell(x, y, false);
					x--;
					print_cursor_info();
					print_cell(x, y, true);
				}
			}
			break;

		case 's':
			if (y % cell_rows == cell_rows - 1)
			{
				if (sheet->num_elts == y + 1)
				{
					for (int i = y + 1; i < y + 1 + cell_rows; i++)
					{
						Head* row_head = make_list();

						for (int j = 0; j < ((Head*) get(sheet, 0))->num_elts; j++)
						{
							Head* cell = make_list();
							add(row_head, cell, j);
						}

						add(sheet, row_head, i);
					}
				}

				page_y++;
				y++;
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				print_cell(x, y, false);
				y++;
				print_cursor_info();
				print_cell(x, y, true);
			}
			break;

		case 'd':
			if (x % cell_columns == cell_columns - 1)
			{
				if (((Head*) get(sheet, 0))->num_elts == x + 1)
				{
					Node* ptr = sheet->node;

					for (int i = 0; i < sheet->num_elts; i++)
					{
						for (int j = 0; j < cell_columns; j++)
						{
							Head* cell = make_list();
							add((Head*) ptr->elt, cell, ((Head*) ptr->elt)->num_elts);
						}

						ptr = ptr->next;
					}
				}

				page_x++;
				x++;
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				print_cell(x, y, false);
				x++;
				print_cursor_info();
				print_cell(x, y, true);
			}
			break;
		
		// quit the open sheet, if there are no sheets left, quit the program
		case 'q':
			if (open_sheets->num_elts == 1)
			{
				terminate = true;
			}
			else
			{
				free_list(sheet, 2, true);
				rm(open_sheets, sheet_index, 0);
				if (sheet_index == open_sheets->num_elts)
				{
					sheet_index--;
				}
				Tab* t = (Tab*) get(open_sheets, sheet_index);
				sheet = t->sheet;
				x = t->x;
				y = t->y;
				page_x = t->page_x;
				page_y = t->page_y;
				print_page(page_x, page_y);
			}
			break;
		
		// highlight cells
		case 'h':
			start_select_mode();
			mode = &select_mode;
			break;
		
		// take in a function command to modify cells/make a plot
		case 'f':
			print_message("Enter Command:");
			move(char_rows - 1, 0);
			mode = &function_mode;
			break;

		// use the calculator
		case 'm':
			print_message("Enter Expression:");
			move(char_rows - 1, 0);
			mode = &calc_mode;
			break;
		
		// print a plot
		case 'g':
			move(char_rows - 1, 0);
			mode = &print_graph_mode;
			break;

		case 'c':
			copy();
			break;

		case 'p':
			paste();
			break;

		case 'u':
			undo();
			break;

		case 'r':
			redo();
			break;

		case 'i':
			print_message("Enter filename of csv to import:");
			mode = &import_mode;
			break;

		case '<':
			if (sheet_index > 0)
			{
				Tab* t = (Tab*) get(open_sheets, sheet_index);
				t->x = x;
				t->y = y;
				t->page_x = page_x;
				t->page_y = page_y;

				sheet_index--;
				t = (Tab*) get(open_sheets, sheet_index);
				sheet = t->sheet;

				x = t->x;
				y = t->y;
				page_x = t->page_x;
				page_y = t->page_y;

				print_page(page_x, page_y);
			}
			break;

		case '>':
			if (sheet_index < open_sheets->num_elts - 1)
			{
				Tab* t = (Tab*) get(open_sheets, sheet_index);
				t->x = x;
				t->y = y;
				t->page_x = page_x;
				t->page_y = page_y;

				sheet_index++;
				t = (Tab*) get(open_sheets, sheet_index);
				sheet = t->sheet;

				x = t->x;
				y = t->y;
				page_x = t->page_x;
				page_y = t->page_y;

				print_page(page_x, page_y);
			}
			break;

		case 'n':
			mode = &new_tab_mode;
			break;
	}
}
