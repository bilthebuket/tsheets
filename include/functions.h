#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "calc.h"

typedef struct Func
{
	// this stores a regression function and its name
	// ex: for a linear regression the user will say ___=linreg(___,____), so we need to relate the lin_reg() function to "linreg" using this struct so we know what the user is asking
	const char* name;
	void (*func)(Plot*);
} Func;

// the list of regression functions the user can call
extern Func** funcs;

// the number of functions in funcs
extern const int NUM_FUNCS;

/*
 * creates a Func struct for each regression function and adds them to Funcs
 * params: none
 * returns: void
*/
void initialize_funcs();

/*
 * frees Funcs
 * params: none
 * returns: void
*/
void free_funcs();

/*
 * adds two numbers (l + r)
 * params:
 * l: the left number
 * r: the right number
 * returns:
 * the sum of the two numbers
*/
double add_nums(double l, double r);

/*
 * subtracts two numbers (l - r)
 * params:
 * l: the left number
 * r: the right number
 * returns:
 * the difference between the two numbers
*/
double sub_nums(double l, double r);

/*
 * multiplies two numbers (l * r)
 * params:
 * l: the left number
 * r: the right number
 * returns:
 * the product of the two numbers
*/
double mult_nums(double l, double r);

/*
 * divides two numbers (l / r)
 * params:
 * l: the left number
 * r: the right number
 * returns:
 * the ratio of the two numbers
*/
double div_nums(double l, double r);

/*
 * raises one number to the power of the other (l ^ r) <-- ^ = exponent, not exclusive or
 * params:
 * l: the left number
 * r: the right number
 * returns:
 * l raised to the power of r
*/
double pow_nums(double l, double r);

/*
 * takes the factorial of a number (d!)
 * params:
 * d: the number that we are computing the factorial of
 * returns:
 * the factorial of d
 */
int fact(int d);

/*
 * performs a linear regression on a plot of data points, adds a pointer to the lin_func function to plot as well as the coefficents
 * params:
 * plot: the plot on which to perform the linear regression
 * returns:
 * void
*/
void lin_reg(Plot* plot);

// all of the reg functions do that same as lin_reg, except for their respective functions
// quad = quadratic, cube = cubic, quart = quartic, sin = sine, cos = cosine, tan = tangent, exp = exponentional, log = logarithmic
void quad_reg(Plot* plot);

void cube_reg(Plot* plot);

void quart_reg(Plot* plot);

void sin_reg(Plot* plot);

void cos_reg(Plot* plot);

void tan_reg(Plot* plot);

void exp_reg(Plot* plot);

void log_reg(Plot* plot);

/* performs a binary probability logisitic regression **not started
*/
void logistic_reg(Plot* plot);

/*
 * computes the value of a linear function for some value (f(x) = ax + b, where a and b are found in coefs)
 * params:
 * x: the independent variable value for which the function value will be computed
 * coefs: the coefficents for the linear function (a and b in ax + b)
 * returns:
 * the functional value for x
 */
double lin_func(double x, double* coefs);

// all of the func functions are the same as lin_func, except for their respective functions
// the names correspond to the same functions as they did for the regression functions (quad = quadratic, etc.)
double sin_func(double d, double* coefs);

double cos_func(double d, double* coefs);

double tan_func(double d, double* coefs);

double log_func(double d, double* coefs);

double log10_func(double d, double* coefs);

double polynomial_func(double x, double* coefs, int degree);

double quad_func(double x, double* coefs);

double cube_func(double x, double* coefs);

double quart_func(double x, double* coefs);

#endif
