#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "LL.h"

/*
 * copies the cell(s) highlighted by the user into the clipboard
 * params:
 * none (uses the global variables for user position/highlighting)
 * returns:
 * void
*/
void copy();

/*
 * pastes the cells in the clipboard to the user's position (the user's cursor will be the top left of the pasted section)
 * params:
 * none (again, will use global variables for user's position)
 * returns:
 * void
*/
void paste();

/*
 * frees the clipboard
 * params:
 * none;
 * returns:
 * void
*/
void free_clipboard();

#endif
