CF=-DLXI_PRINT_ERRORS
LF=

.phony: all

all: run

run: main.o lxi.o
	gcc ${LF} $^ -o $@

main.o: main.c lxi.h
	gcc ${CF} -c $< -o $@

lxi.o: lxi.c lxi.h
	gcc ${CF} -c $< -o $@
