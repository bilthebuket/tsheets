#ifndef INPUT_MODE_H
#define INPUT_MODE_H

/*
 * loads the current value in the cell into the input string
 * params: none
 * returns: void
*/
void load_string_in_input();

/*
 * takes user input and replaces the value in the highlighted cell with that value
 * params:
 * ch: the character the user inputted
 * returns: void
*/
void input_mode(int ch);

#endif
