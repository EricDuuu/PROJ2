CC      := gcc
CFLAGS  := -Wall -Wextra -Werror
CFLAGS  += -g
LIB      := ar rcs

targets  := libuthread.a
objs     := queue.o uthread.o context.o preempt.o

all: $(targets)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

libuthread.a: $(objs)
	$(LIB) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f $(targets) $(objs) $(deps)
