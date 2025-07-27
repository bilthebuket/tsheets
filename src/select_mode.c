#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include "select_mode.h"
#include "io_tools.h"
#include "save_mode.h"
#include "normal_mode.h"
#include "LL.h"
#include "sheet.h"
#include "clipboard.h"
#include "undo_redo.h"

void select_mode(int ch)
{
	switch (ch)
	{
		// movement
		case 'w':
			if (y % cell_rows == 0 && page_y > 0)
			{
				page_y--;
				y--;
				update_bounds();
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				if (y > 0)
				{
					y--;
					update_bounds();
					print_cursor_info();
					print_page(page_x, page_y); // note: this is inefficent, in order to optimize this we should be reprinting only the cells that are newly highlighted/unhighlighted instead of the whole screen
				}
			}
			break;

		case 'a':
			if (x % cell_columns == 0 && page_x > 0)
			{
				page_x--;
				x--;
				update_bounds();
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				if (x > 0)
				{
					x--;
					update_bounds();
					print_cursor_info();
					print_page(page_x, page_y);
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
				update_bounds();
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				y++;
				update_bounds();
				print_cursor_info();
				print_page(page_x, page_y);
			}
			break;

		case 'd':
			if (x % cell_columns == cell_columns - 1)
			{
				if (((Head*) get(sheet, 0))->num_elts == x + 1)
				{
					Node* ptr = sheet->node; // because we are iterating im doing it manually as opposed to using get() so its O(n) instead of O(n^2)

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
				update_bounds();
				print_cursor_info();
				print_page(page_x, page_y);
			}
			else
			{
				x++;
				update_bounds();
				print_cursor_info();
				print_page(page_x, page_y);
			}
			break;

		// output highlighted cells to csv
		case 'o':
			x_f = x;
			y_f = y;

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
			
			mode = &save_mode;
			break;

		// copy highlighted cells
		case 'c':
			copy();
			x_s = -1;
			mode = &normal_mode;
			break;
		}

	// delete selected cells
	if (ch == KEY_BACKSPACE || ch == BACKSPACE_KEYCODE1 || ch == BACKSPACE_KEYCODE2 || ch == '\a')
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

		Node* row = get_helper(sheet, y_s);
		Node* cell = get_helper((Head*) row->elt, x_s);
		Head* cells_to_be_overwritten = make_list();

		for (int i = y_s; i <= y_f; i++)
		{
			cell = get_helper((Head*) row->elt, x_s);
			add(cells_to_be_overwritten, make_list(), i - y_s);

			for (int j = x_s; j <= x_f; j++)
			{
				add((Head*) get(cells_to_be_overwritten, i - y_s), cell->elt, j - x_s);
				cell->elt = make_list();
				print_cell(j, i, false);
				cell = cell->next;
			}

			row = row->next;
		}

		add_undo(cells_to_be_overwritten, x_s, y_s, x_f, y_f);
		x_s = -1;
		mode = &normal_mode;
	}
	else if (ch == ESCAPE_KEYCODE)
	{
		// exit select mode
		x_s = -1;
		print_page(page_x, page_y);
		mode = &normal_mode;
	}
}

void start_select_mode()
{
	x_s = x;
	y_s = y;
}

void update_bounds()
{
	x_f = x;
	y_f = y;
}
