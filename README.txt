tsheets: a terminal based spreadsheet editor written in c/ncurses

TO COMPILE AND RUN: bash compile_and_run.sh

TO RUN (once compiled):

./tsheets filename.csv

where filename is the spreadsheet you would like to edit (optional)

FOR DEBUGGING:

bash compile_and_debug.sh

this will compile with debugging flags -g and -o0 then runs with valgrind
ncurses generates a lot of safe memory leaks in valgrind, so a seperate program is run in this bash script after tsheets is terminated to parse the valgrind log for safe memory leaks and removes them from the file for readability

HOW TO USE:

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

note: undo and redo are unfinished and not useable
note2: pasting a group of cells will paste with the highlighted cell as the top left

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

IMPORTANT NOTES:

-all functions will ask for a range of rows to perform the function on. if you want to perform it on all rows, just do 0,999999999999
-no whitespace in any function calls
-there is no order of operations, you must order everything yourself. A+B+C will not work, you must use parenthesis around every operation, so A+B+C should be (A+B)+C or A+(B+C) or ((A+B)+C)
