#ifndef UNDO_REDO_H
#define UNDO_REDO_H

#include "LL.h"

// the undos and redos of the active sheet
extern Head* undos;
extern Head* redos;

typedef struct PrevState
{
	// a prevstate is the block of cells from before the last action performed by the user
	void* elts;
	int x_s;
	int y_s;
	int x_f;
	int y_f;
} PrevState;

/*
 * returns the open sheet to the most recent prevstate (the one on top of the undos stack), and adds the current state to the redos
 * params: none
 * returns: void
*/
void undo();

/*
 * inverse of undo()
*/
void redo();

/*
 * creates a prevstate and adds it to the undos stack
 * params:
 * all parameters are just the fields of the prevstate that will be added to the undos
 * returns: void
*/
void add_undo(void* elts, int x_s, int y_s, int x_f, int y_f);

/*
 * takes a prevstate and applies it to the open sheet
 * params:
 * ps: the prevstate to be executed
 * returns:
 * a new prevstate for the cells that were modified by the prevstate that was executed
*/
PrevState* execute_prevstate(PrevState* ps);

/*
 * frees one set of undos and redos
 * params:
 * undos: the undos to free
 * redos: the redos to free
 * returns: void
*/
void free_prevstate(Head* undos, Head* redos);

/*
 * frees the undos and redos in the open_sheets list
 * params: none
 * returns: void
*/
void free_prevstates();

#endif
