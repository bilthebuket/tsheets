#ifndef SHEET_H
#define SHEET_H

#include <stdio.h>
#include "LL.h"

// the sheet the user is currently looking at
extern Head* sheet;

// the index of sheet in open_sheets
extern int sheet_index;

// all currently open sheets
extern Head* open_sheets;

typedef struct Tab
{
	int x;
	int y;
	int page_x;
	int page_y;

	Head* sheet;

	Head* undos;
	Head* redos;
} Tab;

/*
 * essentially a constructor for a Tab
*/
Tab* make_tab(int x, int y, int page_x, int page_y, Head* sheet);

/*
 * mode for creating a new tab
 * params:
 * ch: the character passed by the user
 * returns: void
*/
void new_tab_mode(int ch);

/*
 * allocates memory for and loads a sheet from a file (if passed)
 * params:
 * f: if NULL, blank sheet created, otherwise points to file which will be read from
 * returns:
 * the sheet (a 2d linked list of strings. so its a linked lists of rows, where each row is a linked list of char*'s)
*/
Head* initialize_sheet(FILE* f);

#endif
