#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "calc.h"

Func** funcs;
const int NUM_FUNCS = 11;

void initialize_funcs()
{
	// making the array of functions

	funcs = malloc(sizeof(Func*) * NUM_FUNCS);

	for (int i = 0; i < NUM_FUNCS; i++)
	{
		funcs[i] = malloc(sizeof(Func));
	}

	// these indicies/the order these functions appear does not matter as the array will always be iterated through, so i'm using magic numbers

	funcs[0]->name = "makeplot";
	funcs[0]->func = NULL;

	funcs[1]->name = "linreg";
	funcs[1]->func = &lin_reg;

	funcs[2]->name = "quadreg";
	funcs[2]->func = &quad_reg;

	funcs[3]->name = "cubereg";
	funcs[3]->func = &cube_reg;

	funcs[4]->name = "quartreg";
	funcs[4]->func = &quart_reg;

	funcs[5]->name = "sinreg";
	funcs[5]->func = &sin_reg;

	funcs[6]->name = "cosreg";
	funcs[6]->func = &cos_reg;

	funcs[7]->name = "tanreg";
	funcs[7]->func = &tan_reg;

	funcs[8]->name = "expreg";
	funcs[8]->func = &exp_reg;

	funcs[9]->name = "logreg";
	funcs[9]->func = &log_reg;

	funcs[10]->name = "piechart";
	funcs[10]->func = NULL;
}

void free_funcs()
{
	for (int i = 0; i < NUM_FUNCS; i++)
	{
		free(funcs[i]);
	}

	free(funcs);
}

double add_nums(double l, double r)
{
	return l + r;
}

double sub_nums(double l, double r)
{
	return l - r;
}

double mult_nums(double l, double r)
{
	return l * r;
}

double div_nums(double l, double r)
{
	return l / r;
}

double pow_nums(double l, double r)
{
	return pow(l, r);
}

int fact(int d)
{
	int r = 1;

	for (int i = 2; i <= d; i++)
	{
		r *= i;
	}

	return r;
}

double polynomial_func(double x, double* coefs, int degree)
{
	double sum = 0;

	for (int i = 0; i < degree + 1; i++)
	{
		sum += coefs[i] * pow(x, degree - i);
	}

	return sum;
}

double lin_func(double x, double* coefs)
{
	return polynomial_func(x, coefs, 1);
}

double quad_func(double x, double* coefs)
{
	return polynomial_func(x, coefs, 2);
}

double cube_func(double x, double* coefs)
{
	return polynomial_func(x, coefs, 3);
}

double quart_func(double x, double* coefs)
{
	return polynomial_func(x, coefs, 4);
}

void lin_reg(Plot* plot)
{
	// performing the lin_reg by the formula

	double xmean = 0;
	double ymean = 0;
	plot->coefs = malloc(sizeof(double) * 2);
	plot->reg = &lin_func;

	for (int i = 0; i < plot->num_points; i++)
	{
		xmean += plot->xvals[i];
		ymean += plot->yvals[i];
	}

	xmean /= plot->num_points;
	ymean /= plot->num_points;

	// num = numerator, denom = denominator
	double num = 0;
	double denom = 0;

	for (int i = 0; i < plot->num_points; i++)
	{
		num += (plot->xvals[i] - xmean) * (plot->yvals[i] - ymean);
		denom += pow(plot->xvals[i] - xmean, 2);
	}

	plot->coefs[0] = num / denom;
	plot->coefs[1] = ymean - plot->coefs[0] * xmean;
}

void quad_reg(Plot* plot)
{

}

void cube_reg(Plot* plot)
{

}

void quart_reg(Plot* plot)
{
	
}

void sin_reg(Plot* plot)
{

}

void cos_reg(Plot* plot)
{

}

void tan_reg(Plot* plot)
{

}

void exp_reg(Plot* plot)
{

}

void log_reg(Plot* plot)
{

}

void logistic_reg(Plot* plot)
{
	
}

double sin_func(double d, double* coefs)
{
	if (coefs == NULL)
	{
		return sin(d);
	}
	else
	{
		return coefs[0] * sin(coefs[1] * d) + coefs[2];
	}
}

double cos_func(double d, double* coefs)
{
	if (coefs == NULL)
	{
		return cos(d);
	}
	else
	{
		return coefs[0] * cos(coefs[1] * d) + coefs[2];
	}
}

double tan_func(double d, double* coefs)
{
	if (coefs == NULL)
	{
		return tan(d);
	}
	else
	{
		return coefs[0] * tan(coefs[1] * d) + coefs[2];
	}
}

double log_func(double d, double* coefs)
{
	if (coefs == NULL)
	{
		return log(d);
	}
	else
	{
		return coefs[0] * log(coefs[1] * d) + coefs[2];
	}
}

double log10_func(double d, double* coefs)
{
	if (coefs == NULL)
	{
		return log10(d);
	}
	else
	{
		return coefs[0] * log10(coefs[1] * d) + coefs[2];
	}
}
