CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address,undefined

spchk: spchk.c
	$(CC) $(CFLAGS) $^ -o $@