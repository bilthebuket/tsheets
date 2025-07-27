#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "io_tools.h"
#include "LL.h"
#include "graph_mode.h"
#include "function_mode.h"
#include "normal_mode.h"

void print_graph_mode(int ch)
{
	if (get_input(ch))
	{
		// if the string the user inputs matches a plot, it will be printed, else we return to normal mode

		char* s = linked_list_to_str(str, false);

		for (Node* n = plots->node; n != NULL; n = n->next)
		{
			if (!strcmp(s, ((Plot*) n->elt)->name))
			{
				print_graph((Plot*) n->elt);
				mode = &graph_mode;
				break;
			}
		}

		free(s);
		free_list(str, 0, true);
		str = make_list();
		print_message("Could not find plot.");
		mode = &normal_mode;
	}
}
