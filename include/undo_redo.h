#ifndef UNDO_REDO_H
#define UNDO_REDO_H

#include "LL.h"

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
 * frees undos and redos
 * params: none
 * returns: void
*/
void free_prevstates();

#endif
