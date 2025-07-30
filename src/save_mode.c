#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LL.h"
#include "io_tools.h"
#include "normal_mode.h"
#include "save_mode.h"
#include "sheet.h"
#include "select_mode.h"

void output_data(FILE* f)
{
	if (x_s == -1)
	{
		x_s = 0;
		y_s = 0;

		y_f = sheet->num_elts - 1;
		x_f = ((Head*) get(sheet, 0))->num_elts - 1;
	}

	Node* row = get_helper(sheet, y_s);

	for (int i = y_s; i <= y_f; i++)
	{
		Node* cell = get_helper((Head*) row->elt, x_s);

		for (int j = x_s; j <= x_f - 1; j++)
		{
			if (cell->elt == NULL)
			{
				fprintf(f, ",");
			}
			else
			{
				void* c = cell->elt;
				if (c != NULL)
				{
					char* s = (char*) c;
					fprintf(f, "%s,", s);
				}
			}

			print_cell(j, i, false);
			cell = cell->next;
		}

		if (cell->elt == NULL)
		{
			fprintf(f, "\n");
		}
		else
		{
			void* c = cell->elt;
			if (c != NULL)
			{
				char* s = (char*) c;
				fprintf(f, "%s,", s);
			}
		}

		print_cell(x_f, i, false);
		row = row->next;
	}
}

void save_mode(int ch)
{
	// get the filename from the user and then output data to that file if it can be successfully fopen'd
	if (get_input(ch))
	{
		char* s = linked_list_to_str(str, false);
		FILE* out = fopen(s, "w");
		free(s);
						
		if (out == NULL)
		{
			clear_input_line();
			print_message("Could not open file.");
		}
		else
		{
			output_data(out);
			fclose(out);
		}

		free_list(str, 0, true);
		str = make_list();
		mode = &normal_mode;
	}
}
