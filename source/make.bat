gcc -c -Wall -ansi -pedantic commands.c
gcc -c -Wall -ansi -pedantic encoding.c
gcc -c -Wall -ansi -pedantic firstrun.c
gcc -c -Wall -ansi -pedantic input.c
gcc -c -Wall -ansi -pedantic labels.c
gcc -c -Wall -ansi -pedantic macros.c
gcc -c -Wall -ansi -pedantic preprocessor.c
gcc -c -Wall -ansi -pedantic secondrun.c
gcc -c -Wall -ansi -pedantic utilities.c
gcc -c -Wall -ansi -pedantic entries.c
gcc -c -Wall -ansi -pedantic externals.c
gcc -c -Wall -ansi -pedantic cleanup.c

gcc -Wall -ansi -pedantic assembler.c commands.o encoding.o cleanup.o entries.o externals.o firstrun.o input.o labels.o macros.o preprocessor.o secondrun.o utilities.o -o program
pause