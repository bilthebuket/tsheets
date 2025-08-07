#ifndef CALC_H
#define CALC_H

#include "LL.h"

typedef struct Plot
{
	// the name for the plot given by the user
	char* name;
	
	// coordinate pairs
	double* xvals;
	double* yvals;

	// the number of points
	int num_points;

	// the regression function for the plot
	double (*reg)(double, double*);

	// the coefficents for the regression function
	double* coefs;

	// ex: say the fit this plot to a linear function, then coefs would include a and b (ax + b), and reg would point to lin_func
} Plot;

typedef struct Piechart
{
	// the name of the piechart
	char* name;

	// the labels for the data
	char** labels;
	
	// the data that the piechart displays
	double* data;

	// the number of values in labels and data
	int num_points;
} Piechart;

/*
 * does arithmetic on a linked list string which contains an arithmetic expression
 * params:
 * s: the linked list string that contains the arithmetic expression
 * returns:
 * a linked list string containing the solution to the arithmetic expression
 * notes:
 * s GETS FREED, intended use is to do:
 * Head* s = whatever;
 * s = execute_operations(s);
 * VERY STRICT FORMATTING REQUIRED
 * no whitespace
 * there is no order of operations, and every single operation must be ordered with parenthesis
 * ex:
 * (10 + 5) * 7! + 2 --> won't work
 * ((10 + 5) * (7!)) + 2 --> will work
*/
Head* execute_operations(Head* s);

/*
 * get the value on the left side of an index
 * params:
 * s: the linked list string which the value will be parsed from
 * index: the value will be parsed from the left side of this index
 * d: where the parsed double will be stored
 * remove_elts: if true, the nodes that were parsed into the double will be removed from s
 * returns:
 * the index of the node that the parameter index pointed to
 * ex: if s is "12+29", your index is 2, and remove_elts is true, then s will become "+29", 12 will be stored in d, and 0 will be returned
*/
int get_left_val(Head* s, int index, double* d, bool remove_elts);


// exact same as get_left_val but for the right value
int get_right_val(Head* s, int index, double* d, bool remove_elts);

/*
 * creates a plot
 * params:
 * xvals/yvals: the x's and y's that make of the data points
 * num_points: the number of data points
 * name: the name of the plot
 * returns:
 * the plot struct
*/
Plot* make_plot(double* xvals, double* yvals, int num_points, char* name);

/*
 * piechart constructor
*/
Piechart* make_piechart(double* vals, char** labels, int num_points, char* name);

/*
 * frees a plot
 * params:
 * plot: the plot that will be freed
 * returns:
 * void
*/
void free_plot(Plot* plot);

/*
 * frees a piechart
 * params:
 * p: the piechart that will be freed
 * returns: void
*/
void free_piechart(Piechart* p);

#endif
