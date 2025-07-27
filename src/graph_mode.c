#include <stdio.h>
#include <ncurses.h>
#include "normal_mode.h"
#include "io_tools.h"
#include "graph_mode.h"

void graph_mode(int ch)
{
	// the graph will be displayed until the user presses q, at which points it will switch back to normal mode
	switch (ch)
	{
		case 'q':
			print_page(page_x, page_y);
			mode = &normal_mode;
			break;
	}
}
