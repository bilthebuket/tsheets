tsheets: a terminal based spreadsheet editor written in c/ncurses

TO COMPILE AND RUN: bash run.sh filename.csv

where filename is the spreadsheet you would like to edit (optional)

FOR USING:

normal (default) mode:

wasd = move around
enter = enter insert mode (for editing a cell)
h = enter highlight mode
f = enter function mode
o = enter output mode (for saving to a csv file)
g = enter graph mode (for displaying graphs/plots)
q = quit
backspace = delete cell
c = copy cell
p = paste
u = undo
r = redo
m = calculator
n = open new tab
< = switch to left adjacent tab
> = switch to right adjacent tab
i = import mode
j = jump mode

insert mode:

type out what you want the cell to be, then press enter to save changes to the cell or escape to delete changes

highlight mode:

wasd = move around
c = copy selected cells
backspace = delete selected cells
o = enter output mode and save only the highlighted cells to a csv

output mode:

enter the filename you would like and press enter to save and quit

graph mode:

enter the name of the plot you would like to print and press enter to display graph, then press q to return to normal mode

function mode:

enter the function you would like to execute.

there are two options:
1. create a plot
2. manipulate cells

to create a plot, enter:

plotname=plot_function(x_column_code,y_column_code)

where plot_function is:

makeplot to just make a scatter plot
linreg to perform a linear regression

examples:

create a plot plt and run a linear regression, where its x values are in column C and its y values are in column D

plt=linreg(C,D)

to manipulate cells, you must enter function mode in the column that data is being entered in, then enter an arithmetic expression to be performed

ex:

if we want column C to be the sum of A and B, we would go to column C in normal mode, hit f, then enter:

A+B

new tab mode:

input the name of the file you want to open, or just press enter to open a blank sheet

calculator mode:

just enter an arithmetic expression and it will be evaluated

import mode:

enter the filename of a csv you want to import, then the range of cells to be imported.
the cells will be imported with the currently selected cell as the top left.
the cells won't be inserted into the sheet, as in push adjacent cells aside, it instead overwrites them.

ex:
filename.csv
3,2,4,6

this will import a block cells from filename.csv where the top left of the block is (3,2) and the bottom right is (4,6)

jump mode:

enter the coordinates for the cell you want to jump to
format like this -> x,y

IMPORTANT NOTES:

-all functions will ask for a range of rows to perform the function on. if you want to perform it on all rows, just do 0,999999999999
-pasting a group of cells will paste with the highlighted cell as the top left

FOR DEBUGGING/DEVLOPING:

bash debug.sh: compiles and runs in gdb

or

bash memcheck.sh: compiles and runs using valgrind, stores output from valgrind in tsheets.log.
this script also parses the valgrind log and removes memory leaks caused by ncurses.
the logs will say there are memory leaks, but the parser will remove all the blocks related to ncurses.
ex:

==27321== Memcheck, a memory error detector
==27321== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==27321== Using Valgrind-3.25.1 and LibVEX; rerun with -h for copyright info
==27321== Command: ./tsheets
==27321== 
==27321== 
==27321== HEAP SUMMARY:
==27321==     in use at exit: 1,885,484 bytes in 575 blocks
==27321==   total heap usage: 6,976 allocs, 6,401 frees, 1,996,429 bytes allocated
==27321== 
==27321== LEAK SUMMARY:
==27321==    definitely lost: 0 bytes in 0 blocks
==27321==    indirectly lost: 0 bytes in 0 blocks
==27321==      possibly lost: 201 bytes in 3 blocks
==27321==    still reachable: 1,885,283 bytes in 572 blocks
==27321==         suppressed: 0 bytes in 0 blocks
==27321== 

notice how it says there are memory leaks, but all the individual blocks that say what caused the memory leaks are gone because
the parser removed them.
