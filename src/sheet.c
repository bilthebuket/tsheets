#include <stdio.h>
#include "sheet.h"
#include "LL.h"
#include "io_tools.h"
#include "normal_mode.h"

Head* sheet;

Head* open_sheets;

int sheet_index = 0;

Tab* make_tab(int x, int y, int page_x, int page_y, Head* sheet)
{
	Tab* r = malloc(sizeof(Tab));
	r->x = x;
	r->y = y;
	r->page_x = page_x;
	r->page_y = page_y;
	r->sheet = sheet;
	return r;
}

void new_tab_mode(int ch)
{
	if (get_input(ch))
	{
		Tab* t = (Tab*) get(open_sheets, sheet_index);
		t->x = x;
		t->y = y;
		t->page_x = page_x;
		t->page_y = page_y;

		char* fname = linked_list_to_str(str, false);
		FILE* f = fopen(fname, "r");
		free(fname);
		sheet = initialize_sheet(f);

		sheet_index++;
		add(open_sheets, make_tab(0, 0, 0, 0, sheet), sheet_index);

		if (f != NULL)
		{
			fclose(f);
		}

		free_list(str, 0, true);
		str = make_list();

		x = 0;
		y = 0;
		page_x = 0;
		page_y = 0;

		print_page(page_x, page_y);
		mode = &normal_mode;
	}
}

Head* initialize_sheet(FILE* f)
{
	Head* sheet = make_list();

	if (f == NULL)
	{
		// allocating inital memory for the 3d linked list.
		// if no csv is passed, then memory will be allocated for enough cells to fill the screen, more can be allocated as the user needs it
		// sheet is a linked list of row heads, which are linked lists of cells for their row
		// each cell is a linked list of characters
		for (int i = 0; i < cell_rows; i++)
		{
			Head* row_head = make_list();
			
			for (int j = 0; j < cell_columns; j++)
			{
				Head* cell = make_list();
				add(row_head, cell, j);
			}
		
			add(sheet, row_head, i);
		}
	}

	// loading the .csv into memory if one was passed
	else
	{
		// finding the size of the file and reading the entire thing into one buffer
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		rewind(f);

		char* buf = malloc(size + 1);
		size_t bytesRead = fread(buf, 1, size, f);
		buf[bytesRead] = '\0';

		// iterating through the buffer to determine how many rows/columns are needed
		int index = 0;

		// the number of columns could be different for each row so we need to find the "max number" of the columns, but the number of rows is just the number of lines
		int max_columns = 1;

		int num_columns = 1;
		int num_rows = 1;
		while (buf[index] != '\0')
		{
			if (buf[index] == ',')
			{
				num_columns++;
			}
			else
			{
				if (buf[index] == '\n')
				{
					num_rows++;
					if (num_columns > max_columns)
					{
						max_columns = num_columns;
					}
				}
			}

			index++;
		}

		// now we need to allocate memory accordingly
		// these two lines ensure there is exactly enough cell allocated to fill the screen no matter what page

		max_columns += cell_columns - max_columns % cell_columns;
		num_rows += cell_rows - num_rows % cell_rows;

		for (int i = 0; i < num_rows; i++)
		{
			Head* row_head = make_list();
			
			for (int j = 0; j < max_columns; j++)
			{
				Head* cell = make_list();
				add(row_head, cell, j);
			}
		
			add(sheet, row_head, i);
		}

		// iterating again through the buffer to actually load the data into sheet, each comma means we move to the next cell in the row, each newline means we move to the next row
		// im not using the set() function and am instead manually iterating because using get() would require iterating through the list to find the element every time
		// however, i can use add() to add the characters to the cell because a pointer to the tail in maintained, thus add() is O(1)
		index = 0;
		Node* row; // the row we are at in the list of rows
		Node* cell; // the cell we are at in the list of cells for this row

		row = sheet->node;
		cell = ((Head*) row->elt)->node;
		while (buf[index] != '\0')
		{
			if (buf[index] == ',')
			{
				cell = cell->next;
			}
			else
			{
				if (buf[index] == '\n')
				{
					row = row->next;
					cell = ((Head*) row->elt)->node;
				}
				else
				{
					char* c = malloc(sizeof(char));
					*c = buf[index];
					add((Head*) cell->elt, c, ((Head*) cell->elt)->num_elts);
				}
			}

			index++;
		}

		free(buf);
	}

	return sheet;
}
