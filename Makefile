all:
	gcc -o server.out ./src/server.c ./src/utils.c

run:
	./server.out
