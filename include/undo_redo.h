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

void undo();

void redo();

void add_undo(void* elts, int x_s, int y_s, int x_f, int y_f);

PrevState* execute_prevstate(PrevState* ps);

void free_prevstates();

#endif
