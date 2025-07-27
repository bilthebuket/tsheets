gcc -g -o0 src/functions.c src/calc_mode.c src/tsheets.c src/LL.c src/import_mode.c src/normal_mode.c src/select_mode.c src/save_mode.c src/input_mode.c src/io_tools.c src/sheet.c src/function_mode.c src/calc.c src/graph_mode.c src/print_graph_mode.c src/clipboard.c src/undo_redo.c -Iinclude -lncurses -lm -o tsheets
gdb ./tsheets
rm tsheets
