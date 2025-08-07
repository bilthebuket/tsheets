#ifndef FUNCTION_MODE_H
#define FUNCTION_MODE_H

#include <stdbool.h>
#include "LL.h"
#include "calc.h"

// linked list of a plots
extern Head* plots;

// linked list of piecharts
extern Head* piecharts

/*
 * executes functions from the user (performing computations on cells, creating plots)
 * params:
 * ch: the ascii character taken as input
 * returns:
 * void
*/
void function_mode(int ch);

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

/*
 * makes a new piechart and adds it to piecharts
 * params:
 * name: the name of the piechart to be added
 * returns: void
*/
void parse_doubles_make_piechart(char* name);

/*
 * reads the column codes for a function and loads their int representation into the parameters
 * params:
 * left_col/right_col: the integers to read the column codes into
 * ex: if the function was a=makeplot(A,B), left_col will become 0 and right_col will becomes 1
 * returns:
 * 0 if the reading was successful, -1 otherwise
*/
int read_column_codes(int* left_col, int* right_col);

#endif
