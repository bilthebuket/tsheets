#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LL.h"

bool str_contains_str(char* big_str, const char* little_str)
{
	for (int i = 0; big_str[i] != '\0'; i++)
	{
		bool is_same = true;

		for (int j = 0; little_str[j] != '\0'; j++)
		{
			if (big_str[i + j] != little_str[j] || big_str[i + j] == '\0')
			{
				is_same = false;
				break;
			}
		}

		if (is_same)
		{
			return true;
		}
	}
	
	return false;
}

int main(void)
{
	FILE* original = fopen("valgrind.log", "r");
	FILE* modified = fopen("tsheets.log", "w");

	Head* text_blob = make_list();

	char* line = malloc(sizeof(char) * 200);

	bool removing_text_blob = false;

	while (fgets(line, 200, original))
	{
		if (str_contains_str(line, "initialize_window") || str_contains_str(line, "wgetch") || str_contains_str(line, "wrefresh") || str_contains_str(line, "mvprintw"))
		{
			free_list(text_blob, 0, true);
			text_blob = make_list();
			removing_text_blob = true;
			free(line);
		}
		else
		{
			int i;
			for (i = 2; line[i] != '='; i++) {}
			i += 3;

			if (line[i] == '\n')
			{
				if (removing_text_blob)
				{
					removing_text_blob = false;
					free(line);
				}
				else
				{
					add(text_blob, line, text_blob->num_elts);
					for (Node* n = text_blob->node; n != NULL; n = n->next)
					{
						fprintf(modified, (char*) n->elt);
					}
					free_list(text_blob, 0, true);
					text_blob = make_list();
				}
			}
			else
			{
				if (!removing_text_blob)
				{
					add(text_blob, line, text_blob->num_elts);
				}
				else
				{
					free(line);
				}
			}
		}

		line = malloc(sizeof(char) * 200);
	}

	free(line);
	free_list(text_blob, 0, true);
	fclose(original);
	fclose(modified);
}
