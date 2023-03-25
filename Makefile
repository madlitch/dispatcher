CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99
SOURCES = hostd.c utility.c queue.c
OBJECTS = $(subst .c,.o,$(SOURCES))
EXE = hostd

hostd : hostd.o utility.o queue.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

all : $(EXE)

clean:
	rm -f $(OBJECTS) $(EXE) *~

help:
	@echo "valid targets:"
	@echo "  all:    generates all binary files"
	@echo "  debug:  generates debug binary files"
	@echo "  clean:  removes .o and .exe files"
