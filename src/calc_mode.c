#include <stdio.h>
#include <stdbool.h>
#include "calc_mode.h"
#include "LL.h"
#include "io_tools.h"
#include "normal_mode.h"

void calc_mode(int ch)
{
	if (get_input(ch))
	{
		clear_whitespace(str);
		str = execute_operations(str);

		if (str == NULL)
		{
			print_message("Error performing calculation");
		}
		else
		{
			char* result = linked_list_to_str(str, false);
			print_message(result);
			free(result);
			free_list(str, 0, true);
			str = make_list();
		}

		mode = &normal_mode;
	}
}
