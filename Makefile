# variables
OUTPUT=bmp-handle
CC=gcc
CFLAGS=-std=c11 -Wall -Werror 
C_FILES=main.c transformations.c bmp.c

# targets
all: bmp-handle

bmp-handle: main.o transformations.o bmp.o
	@echo "Building application"
	$(CC) $(CFLAGS) $^ -o $(OUTPUT)

main.o: main.c
	@echo "Building main.c file"
	$(CC) $(CFLAGS) -c $^ -o $@

transformations.o: transformations.c
	@echo "Building transformations.c file"
	$(CC) $(CFLAGS) -c $^ -o $@

bmp.o: bmp.c
	@echo "Building bmp.c file"
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@echo "Cleaning project..."
	rm -rf *.o $(OUTPUT)
