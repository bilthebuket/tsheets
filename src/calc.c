#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "calc.h"
#include "LL.h"
#include "function_mode.h"
#include "io_tools.h"
#include "functions.h"

#define DOUBLE_INF 0x7FF0000000000000

Head* execute_operations(Head* s)
{
	// the logic of this function is as follows:
	// we are assuming the very strict syntax was met propery (any sort of errors will lead to undefined behavior/seg faults, i will add less strict syntax and error handling later)

	// if there are any parenthesis is the string we need to find one of the lowest parenthesis expression and execute the operation in there
	// ex: in "((1+2)-3)+(4*(5/2))", we need to find either (1+2) or (5/2), the lowest expressions, and execute those operations
	// so we will iterate through the string, keeping track of the most recent open parenthesis found until we find a close parenthesis, them we will execute the operation in that expression
	// we also need to check for function calls like log(2), so whenever we find a close parenthesis we will check the characters in front of the corresponding open parenthesis to see if there was a function call
	
	// if there are no parenthesis, then we (again assume) know that there's at most one operation left, so we will perform the operation if there is one then return the final result

	// checking for parenthesis, because if there's no parenthesis that means that there's only one operation to perform left
	bool has_parens = false;
	int paren_tracker = 0;

	for (int i = 0; i < s->num_elts; i++)
	{
		char c = *((char*) get(s, i));

		switch (c)
		{
			case '(':
				paren_tracker++;
				has_parens = true;
				break;
			
			case ')':
				paren_tracker--;
				if (paren_tracker < 0)
				{
					free_list(s, 0, true);
					return NULL;
				}
				else
				{
					has_parens = true;
				}
				break;
		}
	}

	if (paren_tracker != 0)
	{
		free_list(s, 0, true);
		return NULL;
	}

	Node* n = s->node;

	// if there's no parenthesis we're just performing a single operation between two numbers
	if (!has_parens)
	{
		char* s2 = linked_list_to_str(s, false);

		// the character that represents what operation we're performing
		char c = '\0';

		// the index in the list where the operation is
		int operation_index = -1;

		// if true, we're perfoming a factorial, false otherwise (because factorial is the only operation this can handle which only involves one number)
		bool f = false;

		// points to the function which performs the operation we need to perform
		double (*op)(double, double);
	
		// determining which operation we need to perform
		for (int i = 0; i < s->num_elts; i++)
		{
			bool found_exponent = false;

			switch (s2[i])
			{
				case '+':
					if (c == '\0')
					{
						c = '+';
						operation_index = i;
						op = &add_nums;
					}
					break;

				case '-':
					// we need to distinguish between minus and negative signs. if the character to the left of the '-' is part of a number, then its a minus sign
					// if its the first character in the string or the character to the left is an operator then its a negative sign
					if (c == '\0')
					{
						if (i > 0)
						{
							if ((s2[i - 1] >= '0' && s2[i - 1] <= '9') || s2[i - 1] == '.')
							{
								c = '-';
								op = &sub_nums;
								operation_index = i;
							}
						}
					}
					break;

				case '*':
					if (c == '+' || c == '-' || c == '\0')
					{
						c = '*';
						op = &mult_nums;
						operation_index = i;
					}
					break;

				case '/':
					if (c == '+' || c == '-' || c == '\0')
					{
						c = '/';
						op = &div_nums;
						operation_index = i;
					}
					break;

				case '^':
					c = '^';
					op = &pow_nums;
					found_exponent = true;
					operation_index = i;
					break;

				case '!':
					if (c == '+' || c == '-' || c == '\0')
					{
						c = '!';
						f = true;
						operation_index = i;
					}
					break;
			}

			if (found_exponent)
			{
				break;
			}
		}

		// if c is null that means theres no operation to perform and we can just return s
		if (c != '\0')
		{
			if (f)
			{
				double d;
				int index = get_left_val(s, operation_index, &d, true);

				if (d == DOUBLE_INF)
				{
					free(s2);
					free_list(s, 0, true);
					return NULL;
				}

				int i = fact((int) d);
				
				Head* val = double_to_char_linked_list((double) i);

				rm(s, index, 0);
				add_list(s, val, index);
			}
			else
			{
				double left;
				double right;
				int index = get_left_val(s, operation_index, &left, true);
				index = get_right_val(s, index, &right, true);

				if (left == DOUBLE_INF || right == DOUBLE_INF)
				{
					free(s2);
					free_list(s, 0, true);
					return NULL;
				}

				Head* val = double_to_char_linked_list((*op)(left, right));

				rm(s, index, 0);
				add_list(s, val, index);
			}

			free(s2);
			return execute_operations(s);
		}
		else
		{
			free(s2);
			return s;
		}
	}
	else
	{
		double (*func)(double, double*) = NULL; // for function calls (sin, cos, ln, etc)
		double* coefs = NULL;

		// this is the index of the first character of a function call
		// example: if our arithmetic expression is (A+B)*log(C), then first_func_char_index would be the index of the letter l
		int first_func_char_index;

		int index = 0;

		// the index of the last open parenthesis we found
		int last_open_paren = -1;
				       
		// if there are parenthesis, we need to find the first operation to perform
		// i.e. if the expression is 2 + (5 * (3 - 2)), we need to locate and perform 3 - 2 first
		// we do that by finding the first closed parenthesis and finding which expression that coressponds to
		while (n != NULL)
		{
			switch (*(char*) n->elt)
			{
				case '(':
					// checking if this is a function call (sin, cos, ln, etc), then storing this index so when we find the closed parenthesis we can come back to this to find the full expression
					
					Head* func_LL = make_list();
					last_open_paren = index;

					first_func_char_index = index;

					// building a string that contains all the lowercase letters immediately before the last open parenthesis
					for (Node* n2 = n->prev; n2 != NULL && *(char*) n2->elt >= 'a' && *(char*) n2->elt <= 'z'; n2 = n2->prev)
					{
						char* c = malloc(sizeof(char));
						*c = *(char*) n2->elt;
						add(func_LL , c, 0);
						first_func_char_index--;
					}

					// if the string we just made has any elements in it, that means theres a function call we need to perform
					if (func_LL->num_elts > 0)
					{
						char* func_str = linked_list_to_str(func_LL, false);
						bool found = false;

						coefs = NULL;

						// big ugly if else to check if the parsed string matches any function name
						if (!strcmp(func_str, "sin"))
						{
							func = &sin_func;
							found = true;
						}
						else if (!strcmp(func_str, "cos"))
						{
							func = &cos_func;
							found = true;
						}
						else if (!strcmp(func_str, "tan"))
						{
							func = &tan_func;
							found = true;
						}
						else if (!strcmp(func_str, "ln"))
						{
							func = &log_func;
							found = true;
						}
						else if (!strcmp(func_str, "log"))
						{
							func = &log10_func;
							found = true;
						}
						else
						{
							for (int i = 0; i < plots->num_elts; i++)
							{
								if (!strcmp(func_str, ((Plot*) get(plots, i))->name))
								{
									func = ((Plot*) get(plots, i))->reg;
									coefs = ((Plot*) get(plots, i))->coefs;
									found = true;
									break;
								}
							}
						}

						if (!found)
						{
							print_message("Function not found.");
						}

						free(func_str);
					}

					free_list(func_LL, 0, true);
					break;

				case ')':
					if (last_open_paren != -1)
					{
						// reading the operation and then executing it
						Head* sub_op = make_list();
						n = n->prev;
						for (int i = index - 1; i > last_open_paren; i--)
						{
							char* c = malloc(sizeof(char));
							*c = *(char*) n->elt;
							add(sub_op, c, 0);
							n = n->prev;
						}

						sub_op = execute_operations(sub_op);

						// executing the function call if there was one
						if (func != NULL)
						{
							double d;
							char* string = linked_list_to_str(sub_op, false);
							sscanf(string, "%lf", &d);
							d = (*func)(d, coefs);
							free(string);
							free_list(sub_op, 0, true);
							sub_op = double_to_char_linked_list(d);

							// removing the expression we performed from the main expression
							for (int i = first_func_char_index; i <= index; i++)
							{
								rm(s, first_func_char_index, 0);
							}

							// adding the result of the expression we performed to the main expression
							add_list(s, sub_op, first_func_char_index);
						}
						else
						{
							// removing the expression we performed from the main expression
							for (int i = last_open_paren; i <= index; i++)
							{
								rm(s, last_open_paren, 0); //TODO: make this more efficent by having a function which can remove multiple elements without calling get_helper() over and over again (it maintains one pointer)
							}

							// adding the result of the expression we performed to the main expression
							add_list(s, sub_op, last_open_paren);
						}

						// recurse to complete the rest of the operations
						return execute_operations(s);
					}
					else
					{
						free_list(s, 0, true);
						return NULL;
					}

					break;
			}

			index++;
			n = n->next;
		}
	}
}

int get_left_val(Head* s, int index, double* d, bool remove_elts)
{
	Head* left_val = make_list();

	int i = index - 1;
	char ch;
	void* v = get(s, i);
	if (v == NULL)
	{
		*d = DOUBLE_INF;
		free_list(left_val, 0, true);
		return -1;
	}

	ch = *(char*) v;

	if (!(ch >= '0' && ch <= '9'))
	{
		*d = DOUBLE_INF;
		free_list(left_val, 0, true);
		return -1;
	}
	else if (ch == '.')
	{
		if (remove_elts)
		{
			rm(s, i, 0);
		}

		i--;
		v = get(s, i);

		if (v == NULL)
		{
			*d = DOUBLE_INF;
			free_list(left_val, 0, true);
			return -1;
		}

		ch = *(char*) v;

		if (!(ch >= '0' && ch <= '9'))
		{
			*d = DOUBLE_INF;
			free_list(left_val, 0, true);
			return -1;
		}
	}

	while ((ch >= '0' && ch <= '9') || ch == '.')
	{
		char* c = malloc(sizeof(char));
		*c = ch;
		add(left_val, c, 0);
		if (remove_elts)
		{
			rm(s, i, 0);
		}

		i--;
		v = get(s, i);
		if (v == NULL)
		{
			break;
		}
		else
		{
			ch = *(char*) v;
		}
	}

	i++;

	if (v != NULL)
	{
		if (ch == '-')
		{
			if (get(s, i - 1) == NULL)
			{
				char* c = malloc(sizeof(char));
				*c = '-';
				add(left_val, c, 0);
				if (remove_elts)
				{
					rm(s, i, 0);
				}
			}
			else
			{
				ch = *(char*) get(s, i - 1);
				if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '!' || ch == '^')
				{
					char* c = malloc(sizeof(char));
					*c = '-';
					add(left_val, c, 0);
					if (remove_elts)
					{
						rm(s, i, 0);
					}
				}
			}
		}
	}

	char* left_val_string = linked_list_to_str(left_val, false);

	if (sscanf(left_val_string, "%lf", d) != 1)
	{
		*d = DOUBLE_INF;
		i = -1;
	}

	free(left_val_string);
	free_list(left_val, 0, true);

	return i;
}

int get_right_val(Head* s, int index, double* d, bool remove_elts)
{
	Head* right_val = make_list();

	int i = index + 1;
	void* v = get(s, i);
	if (v == NULL)
	{
		*d = DOUBLE_INF;
		free_list(right_val, 0, true);
		return -1;
	}

	char ch = *(char*) v;

	if (!(ch >= '0' && ch <= '9'))
	{
		if (ch == '.' || ch == '-')
		{
			char* c = malloc(sizeof(char));
			*c = ch;
			add(right_val, c, right_val->num_elts);

			if (remove_elts)
			{
				rm(s, i, 0);
			}
			else
			{
				i++;
			}

			v = get(s, i);

			if (v == NULL)
			{
				*d = DOUBLE_INF;
				free_list(right_val, 0, true);
				return -1;
			}

			ch = *(char*) v;

			if (!(ch >= '0' && ch <= '9'))
			{
				*d = DOUBLE_INF;
				free_list(right_val, 0, true);
				return -1;
			}
		}
		else
		{
			*d = DOUBLE_INF;
			free_list(right_val, 0, true);
			return -1;
		}
	}

	if (ch == '-')
	{
		char* c = malloc(sizeof(char));
		*c = '-';
		add(right_val, c, right_val->num_elts);
		if (remove_elts)
		{
			rm(s, i, 0);
		}
		else
		{
			i++;
		}
		v = get(s, i);
		if (v == NULL)
		{
			*d = DOUBLE_INF;
			free_list(right_val, 0, true);
			return -1;
		}
		ch = *(char*) v;
	}

	while ((ch >= '0' && ch <= '9') || ch == '.')
	{
		char* c = malloc(sizeof(char));
		*c = ch;
		add(right_val, c, right_val->num_elts);

		if (remove_elts)
		{
			rm(s, i, 0);
		}
		else
		{
			i++;
		}

		v = get(s, i);
		if (v == NULL)
		{
			break;
		}
		else
		{
			ch = *(char*) v;
		}
	}



	char* right_val_string = linked_list_to_str(right_val, false);
	sscanf(right_val_string, "%lf", d);
	free(right_val_string);
	
	free_list(right_val, 0, true);

	return index;
}

Plot* make_plot(double* xvals, double* yvals, int num_points, char* name)
{
	Plot* r = malloc(sizeof(Plot));

	r->xvals = xvals;
	r->yvals = yvals;
	r->num_points = num_points;
	r->name = name;
	r->reg = NULL;
	r->coefs = NULL;

	return r;
}

void free_plot(Plot* plot)
{
	free(plot->xvals);
	free(plot->yvals);
	free(plot->name);

	if (plot->coefs != NULL)
	{
		free(plot->coefs);
	}

	free(plot);
}
