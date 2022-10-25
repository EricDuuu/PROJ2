libuthread.a: queue.o
	ar rcs libuthread.a queue.o

queue.o: queue.c queue.h
	gcc -Wall -Werror -g -c -o queue.o queue.c

clean: 
	rm -f libuthread.a queue.o