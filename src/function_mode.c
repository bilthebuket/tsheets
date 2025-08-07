#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ncurses.h>
#include "function_mode.h"
#include "normal_mode.h"
#include "io_tools.h"
#include "sheet.h"
#include "LL.h"
#include "calc.h"
#include "undo_redo.h"
#include "functions.h"

// stores the command from the user
Head* command = NULL;

// stores the rows for which to apply the command from the user
Head* rows = NULL;

// the start and ending rows will be read from rows into these two ints
int start_row = 0;
int end_row = 0;

// plots created by the user
Head* plots;

Head* piecharts;

void function_mode(int ch)
{
	// we need to get two strings from the user, command and rows, thus we need this static bool to keep track of which input we are on
	static bool next = false;

	// getting the two strings and then executing the command
	if (!next)
	{
		if (get_input(ch))
		{
			command = str;
			str = make_list();
			next = true;
			print_message("For what rows (Please format like this -> start_row,end_row):");
			move(char_rows - 1, 0);
		}
	}
	else
	{
		if (get_input(ch))
		{
			rows = str;
			str = make_list();
			next = false;
			parse_command_execute_operations();
		}
	}
}

void parse_command_execute_operations()
{
	Node* n;

	// reading the starting and ending rows into the two ints
	char* rows_str = linked_list_to_str(rows, false);
	if (sscanf(rows_str, "%d,%d", &start_row, &end_row) < 2)
	{
		start_row = 0;
		end_row = sheet->num_elts - 1;
	}
	free(rows_str);
	
	// some error handling, ensuring the starting row is less than or equal to the ending row, and that all rows in the range are actually loaded in memory
	if (start_row > end_row)
	{
		print_message("Invalid range (start was greater than end)");
		free_list(command, 0, true);
		free_list(rows, 0, true);
		mode = &normal_mode;
		return;
	}

	if (start_row >= sheet->num_elts)
	{
		print_message("Invalid range (start row was greater than the number of rows in memory)");
		free_list(command, 0, true);
		free_list(rows, 0, true);
		mode = &normal_mode;
		return;
	}

	if (end_row >= sheet->num_elts)
	{
		end_row = sheet->num_elts - 1;
	}

	// there are two different types of commands: performing arithmetic on the cells and creating/performing regressions on plots
	// if you wish to perform arithmetic on cells the command looks like this --> A+B
	// if you wish to make a plot it looks like this --> plot_name=makeplot(column_one,column_two)
	// thus, to determine which type of command we are performing we need to see if the string contains an equals sign
	bool has_equals = false;

	for (n = command->node; n != NULL; n = n->next)
	{
		if (*(char*) n->elt == '=')
		{
			has_equals = true;
			break;
		}
	}

	if (has_equals)
	{
		// beacuse the command has an equal sign we will be creating a plot
		Head* plot_name = make_list();
		Head* func_name = make_list();

		// reading the name of the plot
		for (n = command->node; *(char*) n->elt != '=' && n != NULL; n = n->next)
		{
			char* c = malloc(sizeof(char));
			*c = *(char*) n->elt;
			add(plot_name, c, plot_name->num_elts);
		}

		// Null check
		if (n == NULL)
		{
			print_message("Command could not be parsed, please check your syntax.");
			free_list(plot_name, 0, true);
			free_list(func_name, 0, true);
			free_list(command, 0, true);
			free_list(rows, 0, true);
			mode = &normal_mode;
			return;
		}

		n = n->next;

		// reading the name of the function (makeplot, linreg, etc.)
		for (n = n; *(char*) n->elt != '(' && n != NULL; n = n->next)
		{
			char* c = malloc(sizeof(char));
			*c = *(char*) n->elt;
			add(func_name, c, func_name->num_elts);
		}

		// Null check
		if (n == NULL)
		{
			print_message("Command could not be parsed, please check your syntax.");
			free_list(plot_name, 0, true);
			free_list(func_name, 0, true);
			free_list(command, 0, true);
			free_list(rows, 0, true);
			mode = &normal_mode;
			return;
		}

		char* func_s = linked_list_to_str(func_name, false);

		// determining which command to execute

		bool found = false;

		for (int i = 0; i < NUM_FUNCS; i++)
		{
			if (!strcmp(func_s, funcs[i]->name))
			{
				if (!strcmp(func_s, "piechart"))
				{
					parse_doubles_make_piechart(linked_list_to_str(plot_name, false));
				}
				else
				{
					parse_doubles_make_table(linked_list_to_str(plot_name, false), funcs[i]->func);
				}

				found = true;
				break;
			}
		}
		
		if (!found)
		{
			print_message("Function not found!");
		}

		free(func_s);
		free_list(plot_name, 0, true);
		free_list(func_name, 0, true);
	}
	else
	{
		Head* cells_to_be_overwritten = make_list();
		// because there's no equals sign, we are parsing for an arithmetic expression to be performed on the cells
		// for each row, we will take the expression and substitute the column codes for the actual values in the cells for that row, then we will evaluate the expression to find the value that will be placed in the resulting cell
		for (int i = start_row; i <= end_row; i++)
		{
			Head* s = make_list();
			Head* column_code;
			bool reading_column_code = false;

			int index = 0;

			n = command->node;

			// this loop finds column codes and replaces them with the values in the cells
			// essentially if we find a capital letter than means we're reading a column code
			// if we find don't find a captial letter but are supposed to be reading a column code, then we know the code has finished and we can substitute the value
			// note: this doesn't replace values in the command, it builds a new string that is exactly the same but with the columns codes replaced, thus
			// this needs to be optimized by just replacing the values rather than making a whole new string
			for (int j = 0; j < command->num_elts; j++)
			{
				if (*(char*) n->elt >= 'A' && *(char*) n->elt <= 'Z' && reading_column_code)
				{
					// continuing to read the column code
					char* c = malloc(sizeof(char));
					*c = *(char*) n->elt;
					add(column_code, c, column_code->num_elts);
				}
				else
				{
					if (*(char*) n->elt >= 'A' && *(char*) n->elt <= 'Z')
					{
						// starting to read the column code
						column_code = make_list();
						reading_column_code = true;
						char* c = malloc(sizeof(char));
						*c = *(char*) n->elt;
						add(column_code, c, column_code->num_elts);
					}
					else
					{
						if (reading_column_code)
						{
							// finished reading column code, we need to subsitute the value in that column
							reading_column_code = false;

							char* code = linked_list_to_str(column_code, false);
							int col = alph_to_int(code);
							free(code);

							void* cell = get((Head*) get(sheet, i), col);

							if (cell != NULL)
							{
								char* cell_str = (char*) cell;

								for (int k = 0; cell_str[k] != '\0'; k++)
								{
									char* c = malloc(sizeof(char));
									*c = cell_str[k];
									add(s, c, s->num_elts);
								}
							}

							char* c = malloc(sizeof(char));
							*c = *(char*) n->elt;
							add(s, c, s->num_elts);

							free_list(column_code, 0, true);
						}
						else
						{
							// adding a non column code character to the new string
							char* c = malloc(sizeof(char));
							*c = *(char*) n->elt;
							add(s, c, s->num_elts);
						}
					}
				}

				n = n->next;
			}

			// edge case for if the string ends with a column code
			if (reading_column_code)
			{
				reading_column_code = false;

				char* code = linked_list_to_str(column_code, false);
				int col = alph_to_int(code);
				free(code);

				void* cell = get((Head*) get(sheet, i), col);

				if (cell != NULL)
				{
					char* cell_str = (char*) cell;

					for (int k = 0; cell_str[k] != '\0'; k++)
					{
						char* c = malloc(sizeof(char));
						*c = cell_str[k];
						add(s, c, s->num_elts);
					}
				}

				free_list(column_code, 0, true);
			}

			// execute operations on the expression and print the results to the sheet, adding the old values to the undo stack
			clear_whitespace(s);
			s = execute_operations(s);

			if (s == NULL)
			{
				print_message("Error executing expression. Please check syntax and the values in the involved cells.");
				s = make_list();
				
				char* c = malloc(sizeof(char));
				*c = 'N';
				add(s, c, s->num_elts);

				c = malloc(sizeof(char));
				*c = 'a';
				add(s, c, s->num_elts);

				c = malloc(sizeof(char));
				*c = 'N';
				add(s, c, s->num_elts);
			}

			void* elt = set((Head*) get(sheet, i), linked_list_to_str(s, false), x, -1);

			Head* row = make_list();
			add(row, elt, 0);
			add(cells_to_be_overwritten, row, cells_to_be_overwritten->num_elts);

			if (y == i)
			{
				print_cell(x, y, true);
			}
			else if (i >= page_y * cell_rows && i < (page_y + 1) * cell_rows)
			{
				print_cell(x, i, false);
			}

			free_list(s, 0, true);
		}

		add_undo(cells_to_be_overwritten, x, start_row, x, end_row);
	}

	free_list(rows, 0, true);
	free_list(command, 0, true);
	mode = &normal_mode;
}

int read_columns_codes(int* left_col, int* right_col)
{
	Node* n;

	// skipping to the parameters part of the command
	// ex command: a=linreg(BA,A)
	// we need to skip to the BA,A part then parse the codes and convert them to numbers
	for (n = command->node; *(char*) n->elt != '('; n = n->next) {}
	n = n->next;

	Head* column_code = make_list();

	// reading the independent variable (the first command param)
	for (; n != NULL && *(char*) n->elt != ','; n = n->next)
	{
		char* c = malloc(sizeof(char));
		*c = *(char*) n->elt;
		add(column_code, c, column_code->num_elts);
	}

	if (n == NULL)
	{
		free_list(column_code, 0, true);
		print_message("Could not parse columns, please check input formatting");
		return -1;
	}

	n = n->next;

	char* s = linked_list_to_str(column_code, false);
	*left_col = alph_to_int(s);
	free(s);
	free_list(column_code, 0, true);
	column_code = make_list();

	// now reading the second
	for (; n != NULL && *(char*) n->elt != ')'; n = n->next)
	{
		char* c = malloc(sizeof(char));
		*c = *(char*) n->elt;
		add(column_code, c, column_code->num_elts);
	}

	s = linked_list_to_str(column_code, false);
	*right_col = alph_to_int(s);
	free(s);
	free_list(column_code, 0, true);

	return 0;
}

void parse_doubles_make_piechart(char* name)
{
	int num_points = end_row - start_row + 1;

	double* vals = malloc(sizeof(double) * num_points);
	char** labels = malloc(sizeof(char*) * num_points);

	for (int i = 0; i < num_points; i++)
	{
		labels[i] = NULL;
	}

	int label_col;
	int data_col;

	if (!read_columns_codes(&label_col, &data_col))
	{
		free(vals);
		free(labels);
		free(name);
		return;
	}

	// reading the values in the cells for each row from the two columns into the two arrays
	for (int i = start_row; i <= end_row; i++) //TODO: optimize by getting the node for start_row and incrementing it down to end_row instead of repeatedly get()'ing it
	{
		void* cell = get((Head*) get(sheet, i), xcol);

		if (cell == NULL)
		{
			s = NULL;
		}
		else
		{
			s = (char*) cell;
		}

		int num_chars;

		for (num_chars = 0; s[num_chars] != '\0'; num_chars++) {}
		num_chars++;

		char* copy = malloc(sizeof(char) * num_chars);
		
		for (int j = 0; j < num_chars; j++)
		{
			copy[j] = s[j];
		}

		labels[i - start_row] = copy;
		
		cell = get((Head*) get(sheet, i), ycol);

		if (cell == NULL)
		{
			s = NULL;
		}
		else
		{
			s = (char*) cell;
		}

		if (sscanf(s, "%lf", &vals[i - start_row]) != 1)
		{
			for (int j = 0; j < num_points; j++)
			{
				if (labels[j] == NULL)
				{
					break;
				}
				else
				{
					free(labels[j]);
				}
			}
			free(vals);
			free(labels);
			free(name);
			print_message("Error parsing values from cells, table was not created.");
			return;
		}
	}

	// making the plot, running the regression if one was passed, and adding it to the list of plots
	Plot* p = make_piechart(vals, labels, num_points, name);

	add(piecharts, p, piecharts->num_elts);
}

void parse_doubles_make_table(char* name, void (*reg)(Plot*))
{
	// for each row we need to fetch the independent and dependent variables, then apply the regression if one was passed
	int num_points = end_row - start_row + 1;

	double* xvals = malloc(sizeof(double) * num_points);
	double* yvals = malloc(sizeof(double) * num_points);

	// the column numbers for the independent and dependent variables
	int xcol;
	int ycol;

	if (!read_columns_codes(&xcol, &ycol))
	{
		free(xvals);
		free(yvals);
		free(name);
		return;
	}

	// reading the values in the cells for each row from the two columns into the two arrays
	for (int i = start_row; i <= end_row; i++) //TODO: optimize by getting the node for start_row and incrementing it down to end_row instead of repeatedly get()'ing it
	{
		void* cell = get((Head*) get(sheet, i), xcol);

		if (cell == NULL)
		{
			s = NULL;
		}
		else
		{
			s = (char*) cell;
		}

		if (sscanf(s, "%lf", &xvals[i - start_row]) != 1)
		{
			free(s);
			free(xvals);
			free(yvals);
			free(name);
			print_message("Error parsing values from cells, table was not created.");
			return;
		}
		
		cell = get((Head*) get(sheet, i), ycol);

		if (cell == NULL)
		{
			s = NULL;
		}
		else
		{
			s = (char*) cell;
		}

		if (sscanf(s, "%lf", &yvals[i - start_row]) != 1)
		{
			free(xvals);
			free(yvals);
			free(name);
			print_message("Error parsing values from cells, table was not created.");
			return;
		}
	}

	// making the plot, running the regression if one was passed, and adding it to the list of plots
	Plot* plot = make_plot(xvals, yvals, num_points, name);

	if (reg != NULL)
	{
		(*reg)(plot);
	}

	add(plots, plot, plots->num_elts);
}
