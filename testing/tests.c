#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "calc.h"
#include "LL.h"

bool equals(double x, double y);

const char* bool_to_result(bool b);

void test_execute_operations(const char* expression, const char* solution);

int main(void)
{
	test_execute_operations("(3!)-(((4/2)*(4^(1+2)))+(sin(.5)-log(100)))", "-120.4794255386");
	test_execute_operations("54.23", "54.23");
	test_execute_operations("ln(2.71828)+cos(0)", "2");
	test_execute_operations("(2^4)*(4-2)", "32");
	test_execute_operations("5+2*10^2-1", "204");
	test_execute_operations("10*(5+2)*-1", "-70");
	test_execute_operations("38(39fjdsl39))99390(\\+12", NULL);
}

void test_execute_operations(const char* expression, const char* solution)
{
	static int test_number = 1;

	Head* executed = execute_operations(str_to_linked_list(expression));

	if (executed == NULL && solution == NULL)
	{
		printf("Test %d: %s\n", test_number, "Passed");
	}
	else if (executed == NULL)
	{
		printf("Test %d: %s: Expected %s, Found NULL\n", test_number, "Failed", solution);
	}
	else
	{
		char* string = linked_list_to_str(executed, false);

		if (solution == NULL)
		{
			printf("Test %d: %s: Expected NULL, Found %s\n", test_number, "Failed", string);
		}
		else
		{
			double x;
			double y;
			sscanf(string, "%lf", &x);
			sscanf(solution, "%lf", &y);

			if (equals(x, y))
			{
				printf("Test %d: %s\n", test_number, "Passed");
			}
			else
			{
				printf("Test %d: %s: Expected %lf, Found %lf\n", test_number, "Failed", y, x);
			}
		}

		free_list(executed, 0, true);
		free(string);
	}

	test_number++;
}

bool equals(double x, double y)
{
	return abs(x - y) <= .0001;
}

const char* bool_to_result(bool b)
{
	if (b)
	{
		return "Passed";
	}
	else
	{
		return "Failed";
	}
}
