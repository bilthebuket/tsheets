gcc -g -o0 src/tsheets.c src/calc_mode.c src/calc.c src/functions.c src/LL.c src/import_mode.c src/normal_mode.c src/select_mode.c src/save_mode.c src/input_mode.c src/io_tools.c src/sheet.c src/function_mode.c src/graph_mode.c src/print_graph_mode.c src/clipboard.c src/undo_redo.c -Iinclude -lncurses -lm -o tsheets
valgrind -s --show-leak-kinds=all --leak-check=full --track-origins=yes ./tsheets 2> valgrind.log
gcc testing/parse_logs.c src/LL.c -Iinclude -o parse_logs
./parse_logs
rm tsheets
rm parse_logs
rm valgrind.log
