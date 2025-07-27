#ifndef FUNCTION_MODE_H
#define FUNCTION_MODE_H

#include <stdbool.h>
#include "LL.h"
#include "calc.h"

// linked list of a plots
extern Head* plots;

/*
not currently in use
typedef struct Func
{
	char* name; // the string that the user will use to call the function

	int param_min; // min and max number of function parameters
	int param_max;

	void (*func)(Head*); // the actual function that will be executed
} Func;
*/

/*
 * executes functions from the user (performing computations on cells, creating plots)
 * params:
 * ch: the ascii character taken as input
 * returns:
 * void
*/
void function_mode(int ch);

/*
 * loads the list of functions (not currently in use)
*/
void load_functions();

/*
 * frees the list of functions (not currently in use)
*/
void free_functions();

/*
 * adds columns together (not currently in use)
*/
void add_func(Head* params);

/*
 * parses the command passed by the user and executes the operations
 * params:
 * none;
 * returns:
 * void
*/
void parse_command_execute_operations();

/*
 * makes a new plot and adds it to plots
 * params:
 * name: the name of the plot (passed by user)
 * reg: the regression function (is NULL if the user just made a plot and didn't perform a regression)
 * returns:
 * void
*/
void parse_doubles_make_table(char* name, void (*reg)(Plot*));

#endif
