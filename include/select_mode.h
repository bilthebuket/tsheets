#ifndef SELECT_MODE_H
#define SELECT_MODE_H

/*
 * mode for highlighting cells
 * params:
 * ch: the keystoke the user entered
 * returns: void
*/
void select_mode(int ch);

/*
 * initializes x_s and y_s
 * params: none
 * returns: void
*/
void start_select_mode();

/*
 * updates the bounds of the highlighted rectangle
 * params: none
 * returns: void
*/
void update_bounds();

#endif
