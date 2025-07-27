#ifndef IMPORT_MODE_H
#define IMPORT_MODE_H

/*
 * imports a csv file and adds it to the current sheet
 * params:
 * ch: the keystroke the user inputs
 * returns: void
*/
void import_mode(int ch);

/*
 * imports cells from a csv file into the sheet for a specified range of cells
 * params:
 * f: the file we are importing from
 * x_s/y_s/x_f/y_f: the range of cells to import from f. (x_s, y_s) is the top left corner of the block and (x_f, y_f) is the bottom right corner of the block
 * returns: void
*/
void import_csv(FILE* f, int x_s, int y_s, int x_f, int y_f);

#endif
