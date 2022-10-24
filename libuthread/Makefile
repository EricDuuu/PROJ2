# Target library
lib = libuthread.a
CFLAGS := -Wall -Werror -Wextra

lib: queue.o
    gcc -Wall -Wextra -Werror -o $(lib) queue.o

queue.o: queue.c queue.h
    gcc -Wall -Wextra -Werror -c queue.o queue.c