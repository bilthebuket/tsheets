#ifndef SAVE_MODE_H
#define SAVE_MODE_H

/*
 * writes data from the sheet to a file as a csv
 * params:
 * f: the file that will be written to
 * returns:
 * void
 * notes:
 * ensure x1 < x2 and y1 < y2
*/
void output_data(FILE* f);

/*
 * takes input for the name of the file to save to
 * params:
 * ch: the keystroke the user input
 * returns: void
*/
void save_mode(int ch);

#endif
