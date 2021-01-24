CC=gcc
CFLAGS=-std=c99 -pedantic -Werror

all: main

main:
	@printf "This is just a header file, no need to compile\n"

install:
	cp -f -- RND_ErrMsg.h /usr/include/RND_ErrMsg.h
	chmod 644 -- /usr/include/RND_ErrMsg.h

clean:

example: example.o
	$(CC) $(CFLAGS) example.c -o a.out
