#include <stdio.h>
#include "jump_mode.h"
#include "io_tools.h"
#include "normal_mode.h"
#include "sheet.h"

#define PAGE_X_FLAG (1)
#define PAGE_Y_FLAG (1 << 1)

void jump_mode(int ch)
{
	if (get_input(ch))
	{
		clear_whitespace(str);
		char* s = linked_list_to_str(str, false);
		int new_x, new_y;

		if (sscanf(s, "%d,%d", &new_x, &new_y) != 2)
		{
			print_message("Could not parse the given coordinates. Please check formatting");
			free(s);
			free_list(str, 0, true);
			str = make_list();
			mode = &normal_mode;
			return;
		}

		if (new_y < sheet->num_elts && new_x < ((Head*) get(sheet, 0))->num_elts)
		{
			x = new_x;
			y = new_y;

			reevaluate_page_numbers(PAGE_X_FLAG + PAGE_Y_FLAG);
			print_page(page_x, page_y);
		}

		free(s);
		free_list(str, 0, true);
		str = make_list();
		mode = &normal_mode;
	}
}
