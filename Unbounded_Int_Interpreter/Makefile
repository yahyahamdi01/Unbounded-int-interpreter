TEST_PROGRAM = test_unbounded
INTERPRETER = calc_unbounded_int

.PHONY: default all clean

default: $(TEST_PROGRAM) $(INTERPRETER)
all: default

CC = gcc
CFLAGS = -g -Wall -pedantic -std=c11


ALL_OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
ALL_HEADERS = $(wildcard *.h)


%.o: %.c $(ALL_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_PROGRAM): $(TEST_PROGRAM).o unbounded_int.o
	$(CC) $^ $(CFLAGS) -o $@ 

$(INTERPRETER): $(INTERPRETER).o unbounded_int.o
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f $(ALL_OBJECTS)
	rm -f $(TEST_PROGRAM) $(INTERPRETER)
