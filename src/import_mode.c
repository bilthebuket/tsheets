#include <stdio.h>
#include <stdlib.h>
#include "import_mode.h"
#include "io_tools.h"
#include "normal_mode.h"
#include "sheet.h"
#include "LL.h"
#include "undo_redo.h"

void import_mode(int ch)
{
	static bool next = false;
	static FILE* f;

	if (!next)
	{
		if (get_input(ch))
		{
			char* filename = linked_list_to_str(str, false);
			f = fopen(filename, "r");
			free(filename);
			free_list(str, 0, true);
			str = make_list();

			if (f == NULL)
			{
				print_message("File could not be opened");
				mode = &normal_mode;
			}

			print_message("For what cells? Format like this -> x_s,y_s,x_f,y_f");
			next = true;
		}
	}
	else
	{
		if (get_input(ch))
		{
			char* range = linked_list_to_str(str, false);
			int x_s, y_s, x_f, y_f;
			if (sscanf(range, "%d,%d,%d,%d", &x_s, &y_s, &x_f, &y_f) < 4)
			{
				import_csv(f, -1, -1, -1, -1);
			}
			else
			{
				import_csv(f, x_s, y_s, x_f, y_f);
			}
			free(range);
			free_list(str, 0, true);
			str = make_list();
			fclose(f);
			next = false;
			mode = &normal_mode;
		}
	}
}

void import_csv(FILE* f, int x_s, int y_s, int x_f, int y_f)
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
				num_columns = 1;
			}
		}

		index++;
	}

	if (x_s >= max_columns || y_s >= num_rows)
	{
		print_message("Invalid range of rows/columns");
		free(buf);
		return;
	}

	if (x_s == -1)
	{
		x_s = 0;
		y_s = 0;
		x_f = max_columns - 1;
		y_f = num_rows - 1;
	}

	if (x_f >= max_columns)
	{
		x_f = max_columns - 1;
	}
	if (y_f >= num_rows)
	{
		y_f = num_rows - 1;
	}

	// iterating again through the buffer to actually load the data into sheet, each comma means we move to the next cell in the row, each newline means we move to the next row
	int num_lines_passed = 0;
	index = 0;

	// navigating through the buffer to get to the first line we need to read/load
	if (y_s > 0)
	{
		for (index = 0; buf[index] != '\0'; index++)
		{
			if (buf[index] == '\n')
			{
				num_lines_passed++;

				if (num_lines_passed == y_s)
				{
					index++;
					break;
				}
			}
		}
	}

	Head* cells_to_be_overwritten = make_list();

	// iterating over each line and loading the data from buf
	for (int i = 0; i <= y_f - y_s; i++)
	{
		Head* row_to_be_overwritten = make_list();
		Head* cell;

		int num_commas_passed = 0;

		// navigating to the first cell we need to read on this line
		if (x_s > 0)
		{
			for (index = index; buf[index] != '\0'; index++)
			{
				if (buf[index] == ',')
				{
					num_commas_passed++;

					if (num_commas_passed == x_s)
					{
						index++;
						break;
					}
				}
			}
		}

		// reading the cells and loading them into the sheet, reprinting the cells that got updated
		for (int j = 0; j <= x_f - x_s; j++)
		{
			cell = make_list();

			for (; buf[index] != '\0' && buf[index] != ',' && buf[index] != '\n'; index++)
			{
				char* c = malloc(sizeof(char));
				*c = buf[index];
				add(cell, c, cell->num_elts);

			}
			
			void* overwritten_cell = set((Head*) get(sheet, i + y), linked_list_to_str(cell, false), j + x, -1);
			free_list(cell, 0, true);

			print_cell(j + x, i + y, j == 0 && i == 0);
			add(row_to_be_overwritten, overwritten_cell, row_to_be_overwritten->num_elts);

			if (buf[index] != '\n' && buf[index] != '\0')
			{
				index++;
			}
		}

		// navigating to the end of the line
		for (; buf[index] != '\0' && buf[index] != '\n'; index++) {}

		index++;

		add(cells_to_be_overwritten, row_to_be_overwritten, cells_to_be_overwritten->num_elts);
	}

	add_undo(cells_to_be_overwritten, x, y, x + x_f - x_s, y + y_f - y_s);

	free(buf);
}
