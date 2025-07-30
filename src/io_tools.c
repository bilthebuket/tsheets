#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <math.h>
#include "io_tools.h"
#include "LL.h"
#include "sheet.h"
#include "normal_mode.h"
#include "calc.h"

// the cells looks like this:
// ---------------------
// |         |         |
// ---------------------
// so for each cell, the body is 1 row and 9 columns, but we also have to take into account the border lines
// thus we say the cell height is 2 and the width is 10
#define CELL_HEIGHT 2
#define CELL_WIDTH 10

// number of rows at the bottom for input, messages, cursor info, etc
#define NUM_ROWS_AT_BOTTOM 3

// these are special lines for io, the actual index for these lines is char_rows - LINE_NUMBER
#define INPUT_LINE 1
#define MESSAGE_LINE 2
#define CURSOR_INFO_LINE 3

#define NUM_LETTERS_IN_ALPHABET 26

#define FLOATING_POINT_CORRECTION .0001

int char_rows;
int char_columns;
int cell_rows;
int cell_columns;

int x = 0;
int y = 0;
int page_x = 0;
int page_y = 0;

int x_s = -1;
int y_s;
int x_f;
int y_f;

bool terminate = false;

void (*mode)(int);

Head* str;

const int BACKSPACE_KEYCODE1 = 127;
const int BACKSPACE_KEYCODE2 = 8;

const int ESCAPE_KEYCODE = 27;

bool resize_requested = false;
bool screen_big_enough_huh = false;

bool get_input(char ch)
{
	// we let the user add/subtract from the string until either enter or escape is pressed
	// if enter is pressed we return true to let the call know the string is complete
	// if escape is pressed we switch back to normal mode and return false so the caller doesn't do anything (the user pressed escape to cancel whatever they were initially doing)
	if (ch == '\n' || ch == '\r' || ch == KEY_ENTER)
	{
		clear_input_line();
		return true;
	}
	else
	{
		// 127 and 8 are different keycodes for backspace
		if (ch == KEY_BACKSPACE || ch == BACKSPACE_KEYCODE1 || ch == BACKSPACE_KEYCODE2 || ch == '\a')
		{
			if (str->num_elts > 0)
			{
				rm(str, str->num_elts - 1, 0);
			}
		}
		else
		{
			if (ch == ESCAPE_KEYCODE)
			{
				clear_input_line();
				mode = &normal_mode;
				free_list(str, 0, true);
				str = make_list();
				return false;
			}
			else
			{
				char* c = malloc(sizeof(char));
				*c = (char) ch;
				add(str, c, str->num_elts);
			}
		}

		// TODO: optimize by only printed what character changed rather than printed the whole line again
		print_input_line();
		return false;
	}	
}

void clear_input_line()
{
	for (int i = 0; i < char_columns; i++)
	{
		mvprintw(char_rows - INPUT_LINE, i, " ");
	}
}

void print_input_line()
{
	clear_input_line();
	char* s = linked_list_to_str(str, false);
	mvprintw(char_rows - INPUT_LINE, 0, s);
	delch();
	free(s);

}

void clear_whitespace(Head* s)
{
	for (int i = 0; i < s->num_elts;)
	{
		char c = *(char*) get(s, i);

		if (c == ' ' || c == '\t')
		{
			rm(s, i, 0);
		}
		else
		{
			i++;
		}
	}
}

void print_cell(int x, int y, bool highlight)
{
	// this if statement checks if the requested cell to print is part of the page we are on
	if (y >= page_y * cell_rows && y <= (page_y + 1) * cell_rows && x >= page_x * cell_columns && x <= (page_x + 1) * cell_columns)
	{
		if (highlight)
		{
			attron(A_REVERSE);
		}

		void* cell = get((Head*) get(sheet, y), x);

		if (cell == NULL)
		{
			mvprintw(1 + (y % cell_rows) * CELL_HEIGHT, 1 + (x % cell_columns) * CELL_WIDTH, "         ");
		}
		else
		{
			char* cell_str = (char*) cell;

			int len;
			for (len = 0; cell_str[len] != '\0'; len++) {}
			len++;

			if (len < CELL_WIDTH)
			{
				char* s = malloc(sizeof(char) * CELL_WIDTH);

				int i;
				for (i = 0; cell_str[i] != '\0'; i++)
				{
					s[i] = cell_str[i];
				}
				for (; i < CELL_WIDTH - 1; i++)
				{
					s[i] = ' ';
				}
				s[CELL_WIDTH - 1] = '\0';

				mvprintw(1 + (y % cell_rows) * CELL_HEIGHT, 1 + (x % cell_columns) * CELL_WIDTH, s);
				free(s);
			}
			else
			{
				// we only want to print enough characters to fill the cell, so we need to terminate the cell
				// and then un terminate it
				char temp = cell_str[CELL_WIDTH - 1];
				cell_str[CELL_WIDTH - 1] = '\0';
				mvprintw(1 + (y % cell_rows) * CELL_HEIGHT, 1 + (x % cell_columns) * CELL_WIDTH, cell_str);
				cell_str[CELL_WIDTH - 1] = temp;
			}
		}

		if (highlight)
		{
			attroff(A_REVERSE);
		}
	}
}

void print_page(int page_x, int page_y)
{
	clear_screen();

	if (x_s == -1)
	{
		// printing the top line on the screen
		for (int i = 0; i < char_columns - (char_columns - 1) % CELL_WIDTH; i++)
		{
			mvprintw(0, i, "-");
		}

		// for each row we print the cell contents of each column, then print a dividing line below it

		for (int i = 0; i < cell_rows; i++)
		{
			mvprintw(i * CELL_HEIGHT + 1, 0, "|");

			for (int j = 0; j < cell_columns; j++)
			{
				print_cell(page_x * cell_columns + j, page_y * cell_rows + i, false);
				mvprintw(i * CELL_HEIGHT + 1, j * CELL_WIDTH + CELL_WIDTH, "|");
			}

			for (int j = 0; j < cell_columns * CELL_WIDTH + 1; j++)
			{
				mvprintw(i * CELL_HEIGHT + CELL_HEIGHT, j, "-");
			}

		}

		print_cell(x, y, true);
	}
	else
	{
		// this is to ensure that x_s and y_s stay the same but we can still highlight if x_f and y_f are greater than or less than x_s and y_s
		// otherwise the highlighting only works if x_f > x_s and y_f > y_s
		int x_l;
		int x_r;
		int y_t;
		int y_b;

		if (x_s < x_f)
		{
			x_l = x_s;
			x_r = x_f;
		}
		else
		{
			x_l = x_f;
			x_r = x_s;
		}

		if (y_s < y_f)
		{
			y_t = y_s;
			y_b = y_f;
		}
		else
		{
			y_t = y_f;
			y_b = y_s;
		}

		// prints dashes across the first row of the screen
		for (int i = 0; i < char_columns - (char_columns - 1) % CELL_WIDTH; i++)
		{
			mvprintw(0, i, "-");
		}

		// same logic as above but we need to print highlighted characters for the cells which are currently highlighted

		for (int i = 0; i < cell_rows; i++)
		{
			mvprintw(i * CELL_HEIGHT + 1, 0, "|");

			for (int j = 0; j < cell_columns; j++)
			{
				bool highlight = i + page_y * cell_rows >= y_t && i + page_y * cell_rows <= y_b && j + page_x * cell_columns >= x_l && j + page_x * cell_columns <= x_r;
				print_cell(page_x * cell_columns + j, page_y * cell_rows + i, highlight);
				mvprintw(i * CELL_HEIGHT + 1, j * CELL_WIDTH + CELL_WIDTH, "|");
			}

			for (int j = 0; j < cell_columns * CELL_WIDTH + 1; j++)
			{
				mvprintw(i * CELL_HEIGHT + CELL_HEIGHT, j, "-");
			}
		}
	}

	print_cursor_info();
}

void update_screensize_vals()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	char_rows = w.ws_row;
	char_columns = w.ws_col;

	// we have to subtract one from both of these to reserve space for the line at the end of the chain of cells
	// ---------------------
	// |         |         | <-- this line
	// ---------------------
	// ^^^^^^^^^^^^^^^^^^^^^
	// |||||||||||||||||||||
	// and this line

	// 9 characters for the cell, one for the line (|) separating each cell
	cell_columns = (char_columns - 1) / CELL_WIDTH;

	// rows alternate between cells and lines separating cells
	cell_rows = (char_rows - 1 - NUM_ROWS_AT_BOTTOM) / CELL_HEIGHT;

	screen_big_enough_huh = char_columns >= CELL_WIDTH + 1 && char_rows >= CELL_HEIGHT + NUM_ROWS_AT_BOTTOM + 1;
}

void handle_screen_resize()
{
	// telling ncurses to update screen dimensions and what not
	endwin();
	refresh();
	clear();

	update_screensize_vals();

	// this is for if the screen gets bigger, we make sure every tab has enough cells allocated to fill the screen
	for (int i = 0; i < open_sheets->num_elts; i++)
	{
		Head* sh = ((Tab*) get(open_sheets, i))->sheet;

		if (sh->num_elts < cell_rows)
		{
			while (sh->num_elts != cell_rows)
			{
				add(sh, make_list(), sh->num_elts);
			}
		}

		for (int j = 0; j < sh->num_elts; j++)
		{
			Head* row = (Head*) get(sh, j);

			if (row->num_elts < cell_columns)
			{
				while (row->num_elts != cell_columns)
				{
					add(row, NULL, row->num_elts);
				}
			}
		}
	}

	// redetermining which page the user is on based on the new screen size
	
	if (cell_columns == 0)
	{
		page_x = 0;
	}
	else
	{
		page_x = (x - (x % cell_columns)) / cell_columns;
	}

	if (cell_rows == 0)
	{
		page_y = 0;
	}
	else
	{
		page_y = (y - (y % cell_rows)) / cell_rows;
	}

	if (screen_big_enough_huh)
	{
		print_page(page_x, page_y);
	}
}

void handle_winch(int sig)
{
	resize_requested = true;
}

void initialize_window()
{
	// initalize ncurses window
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	// initialze window resize handler
	struct sigaction sa;
	sa.sa_handler = handle_winch;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGWINCH, &sa, NULL);

	// initialize row/column variables
	update_screensize_vals();
}

void print_cursor_info()
{
	for (int i = 0; i < char_columns; i++)
	{
		mvprintw(char_rows - NUM_ROWS_AT_BOTTOM, i, " ");
	}
	char* s = int_to_alph(x);
	mvprintw(char_rows - CURSOR_INFO_LINE, 0, "cell (%d, %d); page (%d, %d); column %s", x, y, page_x, page_y, s);
	free(s);
}

char* int_to_alph(int num)
{
	// the number of characters for the numeric representation (includes a null character)
	int size;

	if (num == 0)
	{
		size = 2;
	}
	else
	{
		// the number of characters to represent a number is the log base 26 of said number, which we then round down, so we need to add one for rounding it down, and add another for the null character
		size = ((int) (log(num) / log(NUM_LETTERS_IN_ALPHABET) + FLOATING_POINT_CORRECTION)) + 2;
	}
	char* r = malloc(sizeof(char) * size);
	r[size - 1] = '\0';

	// converting the number to base 26, and then converting the base 26 digits to ascii for A-Z
	for (int i = 0; i < size - 1; i++)
	{
		r[i] = (num / (int) (pow(NUM_LETTERS_IN_ALPHABET, size - i - 2) + FLOATING_POINT_CORRECTION)) + 'A';
		num = num % (int) (pow(NUM_LETTERS_IN_ALPHABET, size - i - 2) + FLOATING_POINT_CORRECTION);
	}

	return r;
}

int alph_to_int(char* s)
{
	// converting the string to base 26, and then to base 10
	int r = 0;
	int size;
	for (size = 0; s[size] != '\0'; size++) {}
	size++;

	for (int i = 0; s[i] != '\0'; i++)
	{
		r += (s[i] - 'A') * (int) (pow(NUM_LETTERS_IN_ALPHABET, size - i - 2) + FLOATING_POINT_CORRECTION);
	}

	return r;
}

void clear_message_line()
{
	for (int i = 0; i < char_columns; i++)
	{
		mvprintw(char_rows - MESSAGE_LINE, i, " ");
	}
}

void print_message(const char* s)
{
	clear_message_line();
	mvprintw(char_rows - MESSAGE_LINE, 0, s);
}

void print_graph(Plot* plot)
{
	clear_screen();

	double xmin = plot->xvals[0];
	double xmax = plot->xvals[0];
	double ymin = plot->yvals[0];
	double ymax = plot->yvals[0];

	for (int i = 1; i < plot->num_points; i++)
	{
		if (plot->xvals[i] < xmin)
		{
			xmin = plot->xvals[i];
		}
		if (plot->xvals[i] > xmax)
		{
			xmax = plot->xvals[i];
		}
		if (plot->yvals[i] < ymin)
		{
			ymin = plot->yvals[i];
		}
		if (plot->yvals[i] > ymax)
		{
			ymax = plot->yvals[i];
		}
	}

	// these are essentially the scale of the screen
	// every single printed graph will be fitted so that the smallest and largest x and y values are on the edges
	// because we have a discrete number of cells in a terminal window, we need to calculate how much change in value a change in cell represents
	double change_per_column = (xmax - xmin) / ((double) char_columns);
	double change_per_row = (ymax - ymin) / ((double) char_rows);

	// determining if the x or y axis are on screen and printing them if so
	if (xmin < 0 && xmax > 0)
	{
		int y_axis_col = (int) (-xmin / change_per_column);
		for (int i = 0; i < char_rows; i++)
		{
			mvprintw(i, y_axis_col, "|");
		}
	}

	if (ymin < 0 && ymax > 0)
	{
		int x_axis_row = (int) (ymax / change_per_row);
		for (int i = 0; i < char_columns; i++)
		{
			mvprintw(x_axis_row, i, "-");
		}
	}

	// if this plot has a regression we need to print that as well, so we calculate y-hat for each column on the graph to get coordinate pairs which we then print
	if (plot->reg != NULL)
	{
		for (int i = 0; i < char_columns; i++)
		{
			double y_hat = (*plot->reg)(((double) i) * change_per_column + xmin, plot->coefs);
			int y_hat_screen_pos = (int) ((ymax - y_hat) / change_per_row);
			mvprintw(y_hat_screen_pos, i, "O");
		}
	}

	// printing the actual coordainte pairs of data
	for (int i = 0; i < plot->num_points; i++)
	{
		int x_screen_pos = (int) ((plot->xvals[i] - xmin) / change_per_column);
		int y_screen_pos = (int) ((ymax - plot->yvals[i]) / change_per_row);

		mvprintw(y_screen_pos, x_screen_pos, "X");
	}
}

void clear_screen()
{
	for (int i = 0; i < char_rows; i++)
	{
		for (int j = 0; j < char_columns; j++)
		{
			mvprintw(i, j, " ");
		}
	}
}
