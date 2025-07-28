#ifndef IO_TOOLS_H
#define IO_TOOLS_H

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "LL.h"
#include "calc.h"

// the number of rows and columns in the terminal window for characters
extern int char_rows;
extern int char_columns;

// the number of rows and columns in the terminal windows for cells
// this number is rounded down, and also leaves space for the input, cursor info, and message lines at the bottom
extern int cell_rows;
extern int cell_columns;

// the location of the cursor in the sheet
extern int x;
extern int y;

// what page the cursor is on
// what i mean by this is when the app first starts the cursor is on point (0,0) and page (0,0)
// the screen can only display so many cells, so when the user reaches the edge of the current page and goes one cell further, they will traverse to an adjacent page
// we keep track of what page they're on in these two variables
extern int page_x;
extern int page_y;

// this is for highlighting cells
// these stand for "x start", "y start", "x finish", and "y finish"
// the highlighted block of cells is a rectangle with upper left corner (x_s, y_s) and lower right corner (x_f, y_f)
extern int x_s;
extern int y_s;
extern int x_f;
extern int y_f;

// this is what mode the app is in (normal mode, input mode, select/highlight mode, etc)
// the way im doing io is every time there's an input character, we send that character to a "mode" function, which then handles the input however it needs to
// these different modes will switch between themselves on their own
// ex: when we are in input mode, the characters the user inputs are sent to the input_mode function by this function pointer, which handles the user input
// until the user presses enter, finishing the input, in which case input_mode will update the sheet and then change the value of this pointer to &normal_mode
extern void (*mode)(int);

// the app runs until terminate becomes true
extern bool terminate;

// this is where get_input stores the string
extern Head* str;

// two different keycodes for backspace
extern const int BACKSPACE_KEYCODE1;
extern const int BACKSPACE_KEYCODE2;

extern const int ESCAPE_KEYCODE;

// if the terminal window has been resized, this will be true. false otherwise
extern bool resize_requested;

// if the screen is big enough to print all necessary information as well as at least one cell, this will be true, false otherwise
extern bool screen_big_enough_huh;

/*
 * removes the spaces and tabs from a linked list string
 * params:
 * s: the string that we are removing the whitespace from
 * returns: void
*/
void clear_whitespace(Head* s);

/*
 * prints a new page to the terminal
 * params:
 * x: the x value of what page we're on
 * y: the y value of what page we're on
 * returns:
 * void
*/
void print_page(int x, int y);

/*
 * clears the line where user input is taken (the bottom line)
 * params:
 * none
 * returns:
 * void
*/
void clear_input_line();

/*
 * reprints a cell
 * params:
 * x/y: the coordinates of the cell
 * highlight: if true, text will be highlighted, if false, it will not be hightlighted
 * returns:
 * void
*/
void print_cell(int x, int y, bool highlight);

/*
 * prints the input string to the input line
 * params:
 * none
 * returns:
 * void
*/
void print_input_line();

/*
 * initalizes the window and gets information about the rows and columns of the screen
 * params:
 * none
 * returns:
 * void
*/
void initialize_window();

/*
 * takes in a string input from the user and stores it in the global variable str
 * params:
 * ch: the character entered by the user
 * returns:
 * true if the string is finished, false otherwise
*/
bool get_input(char ch);

/*
 * displays information about the cursor location for the user (the coordinates, what column its in as an alphabetical code, what page its on)
 * params: none
 * returns: void
*/
void print_cursor_info();

/*
 * prints a message for the user ("Function not found", etc.)
 * params:
 * s: the message that will be printed
 * returns: void
*/
void print_message(const char* s);

/*
 * converts an integer to an alphabetical encoding (0 = AAA, 1 = AAB, 2 = AAC, etc.)
 * params:
 * num: the number that will be encoded
 * returns:
 * a string containing the encoded number
 * note:
 * preceding A's are omitted, so num = 1 retunrns "B", and num = 26 returns "BA"
*/
char* int_to_alph(int num);

/*
 * converts an alphanumeric code to an integer (the inverse of int_to_alph)
 * params:
 * s: the code that will be converted
 * returns:
 * the integer representation of that code
*/
int alph_to_int(char* s);

/*
 * clears the message line (prints whitespace across the whole line)
 * params: none
 * returns: void
*/
void clear_message_line();

/*
 * clears the screen and prints a graph
 * params:
 * plot: the graph that will be printed
 * returns: void
*/
void print_graph(Plot* plot);

/*
 * clears the screen (fills it with whitespace)
 * params: none
 * returns: void
*/
void clear_screen();

/*
 * updates char_rows, char_columns, cell_rows, and cell_columns based on the size of the terminal window
 * params: none
 * returns: void
*/
void update_screensize_vals();

/*
 * handles updating row/column variables, the memory allocations for the open sheets, and what is printing on screen when the terminal window is resized
 * params: none
 * returns: void
*/
void handle_screen_resize();

#endif
